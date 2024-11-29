#ifndef EXTERNAL_SORT_H
#define EXTERNAL_SORT_H

#include <string>
#include <vector>
#include <queue>
#include <fstream>

using namespace std;

class ExternalSort {
private:
    // 每个块的最大大小（字节）
    static const size_t CHUNK_SIZE = 1024 * 1024 * 1024; // 1GB
    string inputFile;
    string outputFile;
    string tempDir;

    // 用于多路归并的比较器
    struct ChunkCompare {
        bool operator()(const pair<int, int>& a, const pair<int, int>& b) {
            return a.first > b.first;
        }
    };

    // 将文件分割成较小的块并排序
    vector<string> splitAndSort();
    
    // 合并已排序的块
    void mergeChunks(const vector<string>& chunks);

    // 生成临时文件名
    string getTempFileName(int index);

public:
    ExternalSort(const string& input, const string& output, const string& temp);
    void sort();
    
    // 生成测试数据
    static void generateTestData(const string& filename, size_t size);
};

#endif 