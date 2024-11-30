#include "external_sort.h"
#include <algorithm>
#include <random>
#include <iostream>
#include <climits>
#include <execution>
#include <thread>
#include <future>

using namespace std;

ExternalSort::ExternalSort(const string& input, const string& output, const string& temp)
    : inputFile(input), outputFile(output), tempDir(temp) {}

string ExternalSort::getTempFileName(int index) {
    return tempDir + "/chunk_" + to_string(index) + ".tmp";
}

void ExternalSort::sortChunk(vector<int>& buffer) {
    std::sort(execution::par_unseq, buffer.begin(), buffer.end());
}

void ExternalSort::processChunk(const string& chunkFile, vector<int>& buffer) {
    sortChunk(buffer);
    
    ofstream chunk(chunkFile, ios::binary);
    // 设置输出缓冲区
    vector<char> writeBuffer(BUFFER_SIZE);
    chunk.rdbuf()->pubsetbuf(writeBuffer.data(), BUFFER_SIZE);
    
    chunk.write(reinterpret_cast<const char*>(buffer.data()), 
                buffer.size() * sizeof(int));
    chunk.close();
}

vector<string> ExternalSort::splitAndSort() {
    vector<string> chunkFiles;
    ifstream input(inputFile, ios::binary);
    
    // 设置输入缓冲区
    vector<char> readBuffer(BUFFER_SIZE);
    input.rdbuf()->pubsetbuf(readBuffer.data(), BUFFER_SIZE);
    
    vector<future<void>> futures;
    int chunkIndex = 0;

    while (input) {
        vector<int> buffer;
        buffer.reserve(CHUNK_SIZE / sizeof(int)); // 预分配内存
        
        // 批量读取数据
        while (buffer.size() * sizeof(int) < CHUNK_SIZE && !input.eof()) {
            vector<int> tempBuffer(BUFFER_SIZE / sizeof(int));
            size_t bytesRead = input.read(reinterpret_cast<char*>(tempBuffer.data()), 
                                        BUFFER_SIZE).gcount();
            size_t numInts = bytesRead / sizeof(int);
            buffer.insert(buffer.end(), tempBuffer.begin(), tempBuffer.begin() + numInts);
        }

        if (buffer.empty()) break;

        string chunkFile = getTempFileName(chunkIndex++);
        chunkFiles.push_back(chunkFile);

        // 异步处理块
        futures.push_back(
            async(launch::async, 
                  &ExternalSort::processChunk, 
                  this, 
                  chunkFile, 
                  move(buffer))
        );

        // 控制并发数量
        if (futures.size() >= MAX_THREADS) {
            for (auto& f : futures) f.wait();
            futures.clear();
        }
    }

    // 等待所有任务完成
    for (auto& f : futures) f.wait();
    input.close();
    return chunkFiles;
}

void ExternalSort::mergeChunks(const vector<string>& chunks) {
    vector<ifstream> inputs;
    vector<vector<char>> inputBuffers(chunks.size(), vector<char>(BUFFER_SIZE));
    
    // 为每个输入流设置缓冲区
    for (size_t i = 0; i < chunks.size(); i++) {
        inputs.emplace_back(chunks[i], ios::binary);
        inputs[i].rdbuf()->pubsetbuf(inputBuffers[i].data(), BUFFER_SIZE);
    }

    priority_queue<pair<int, int>, 
                  vector<pair<int, int>>, 
                  ChunkCompare> pq;
    
    // 初始化优先队列
    for (size_t i = 0; i < chunks.size(); i++) {
        int num;
        if (inputs[i].read(reinterpret_cast<char*>(&num), sizeof(int))) {
            pq.push({num, i});
        }
    }

    ofstream output(outputFile, ios::binary);
    vector<char> writeBuffer(BUFFER_SIZE);
    output.rdbuf()->pubsetbuf(writeBuffer.data(), BUFFER_SIZE);

    vector<int> mergeBuffer;
    mergeBuffer.reserve(BUFFER_SIZE / sizeof(int));

    while (!pq.empty()) {
        auto [num, index] = pq.top();
        pq.pop();

        mergeBuffer.push_back(num);

        if (mergeBuffer.size() == mergeBuffer.capacity()) {
            output.write(reinterpret_cast<const char*>(mergeBuffer.data()), 
                        mergeBuffer.size() * sizeof(int));
            mergeBuffer.clear();
        }

        int nextNum;
        if (inputs[index].read(reinterpret_cast<char*>(&nextNum), sizeof(int))) {
            pq.push({nextNum, index});
        }
    }

    // 写入剩余数据
    if (!mergeBuffer.empty()) {
        output.write(reinterpret_cast<const char*>(mergeBuffer.data()), 
                    mergeBuffer.size() * sizeof(int));
    }

    // 清理资源
    for (auto& input : inputs) input.close();
    output.close();

    for (const auto& chunk : chunks) {
        remove(chunk.c_str());
    }
}

void ExternalSort::sort() {
    auto chunks = splitAndSort();
    mergeChunks(chunks);
}

void ExternalSort::generateTestData(const string& filename, size_t size) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(INT_MIN, INT_MAX);

    ofstream file(filename, ios::binary);
    vector<char> writeBuffer(BUFFER_SIZE);
    file.rdbuf()->pubsetbuf(writeBuffer.data(), BUFFER_SIZE);

    vector<int> buffer;
    buffer.reserve(BUFFER_SIZE / sizeof(int));

    for (size_t i = 0; i < size; i++) {
        buffer.push_back(dis(gen));
        
        if (buffer.size() == buffer.capacity()) {
            file.write(reinterpret_cast<const char*>(buffer.data()), 
                      buffer.size() * sizeof(int));
            buffer.clear();
        }
    }

    if (!buffer.empty()) {
        file.write(reinterpret_cast<const char*>(buffer.data()), 
                  buffer.size() * sizeof(int));
    }

    file.close();
} 