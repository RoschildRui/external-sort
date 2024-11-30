#ifndef EXTERNAL_SORT_H
#define EXTERNAL_SORT_H

#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <thread>

using namespace std;

class ExternalSort {
private:
    // 优化：增加缓冲区大小常量
    static const size_t BUFFER_SIZE = 8192; // 8KB 缓冲区
    static const size_t CHUNK_SIZE = 1024 * 1024 * 1024; // 1GB
    static const size_t MAX_THREADS = thread::hardware_concurrency(); // 获取CPU核心数
    
    string inputFile;
    string outputFile;
    string tempDir;

    struct ChunkCompare {
        bool operator()(const pair<int, int>& a, const pair<int, int>& b) {
            return a.first > b.first;
        }
    };

    vector<string> splitAndSort();
    void mergeChunks(const vector<string>& chunks);
    string getTempFileName(int index);
    
    // 新增：并行处理函数
    void sortChunk(vector<int>& buffer);
    void processChunk(const string& chunkFile, vector<int>& buffer);

public:
    ExternalSort(const string& input, const string& output, const string& temp);
    void sort();
    static void generateTestData(const string& filename, size_t size);
};

#endif 