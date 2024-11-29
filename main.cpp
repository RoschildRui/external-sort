#include "external_sort.h"
#include <iostream>
#include <chrono>

using namespace std;

int main() {
    const string inputFile = "input.bin";
    const string outputFile = "output.bin";
    const string tempDir = "temp";

    // 生成测试数据（约4GB大小）
    size_t numIntegers = 1024 * 1024 * 1024; // 10亿个整数
    cout << "生成测试数据..." << endl;
    ExternalSort::generateTestData(inputFile, numIntegers);

    // 创建临时目录
    system("mkdir -p temp");

    // 开始排序
    cout << "开始排序..." << endl;
    auto start = chrono::high_resolution_clock::now();

    ExternalSort sorter(inputFile, outputFile, tempDir);
    sorter.sort();

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(end - start);
    
    cout << "排序完成！用时: " << duration.count() << " 秒" << endl;

    // 清理临时目录
    system("rm -rf temp");

    return 0;
} 