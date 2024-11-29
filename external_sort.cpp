#include "external_sort.h"
#include <algorithm>
#include <random>
#include <iostream>
#include <climits>

using namespace std;

ExternalSort::ExternalSort(const string& input, const string& output, const string& temp)
    : inputFile(input), outputFile(output), tempDir(temp) {}

string ExternalSort::getTempFileName(int index) {
    return tempDir + "/chunk_" + to_string(index) + ".tmp";
}

vector<string> ExternalSort::splitAndSort() {
    vector<string> chunkFiles;
    ifstream input(inputFile, ios::binary);
    vector<int> buffer;
    int chunkIndex = 0;

    while (input) {
        buffer.clear();
        
        // 读取一个块的数据
        while (buffer.size() * sizeof(int) < CHUNK_SIZE && !input.eof()) {
            int num;
            if (input.read(reinterpret_cast<char*>(&num), sizeof(int))) {
                buffer.push_back(num);
            }
        }

        if (buffer.empty()) break;

        // 对块进行排序
        std::sort(buffer.begin(), buffer.end());

        // 将排序后的块写入临时文件
        string chunkFile = getTempFileName(chunkIndex++);
        ofstream chunk(chunkFile, ios::binary);
        chunk.write(reinterpret_cast<const char*>(buffer.data()), buffer.size() * sizeof(int));
        chunk.close();

        chunkFiles.push_back(chunkFile);
    }

    input.close();
    return chunkFiles;
}

void ExternalSort::mergeChunks(const vector<string>& chunks) {
    vector<ifstream> inputs;
    priority_queue<pair<int, int>, 
                  vector<pair<int, int>>, 
                  ChunkCompare> pq;

    // 打开所有临时文件
    for (size_t i = 0; i < chunks.size(); i++) {
        inputs.emplace_back(chunks[i], ios::binary);
        int num;
        if (inputs[i].read(reinterpret_cast<char*>(&num), sizeof(int))) {
            pq.push({num, i});
        }
    }

    ofstream output(outputFile, ios::binary);

    // 合并所有块
    while (!pq.empty()) {
        auto [num, index] = pq.top();
        pq.pop();

        output.write(reinterpret_cast<const char*>(&num), sizeof(int));

        int nextNum;
        if (inputs[index].read(reinterpret_cast<char*>(&nextNum), sizeof(int))) {
            pq.push({nextNum, index});
        }
    }

    // 关闭所有文件
    for (auto& input : inputs) {
        input.close();
    }
    output.close();

    // 删除临时文件
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
    for (size_t i = 0; i < size; i++) {
        int num = dis(gen);
        file.write(reinterpret_cast<const char*>(&num), sizeof(int));
    }
    file.close();
} 