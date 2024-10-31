#ifndef ENCODER_H
#define ENCODER_H

#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <cstring>

using namespace std;

enum class MatrixType {
    RID,
    TWO_TONE,
    SYSTEMATIC_RID,     // 新增
    SYSTEMATIC_TWOTONE, // 新增
    CUSTOMIZE
};

class Encoder {
public:
    // 构造函数
    Encoder(const vector<vector<uint8_t>>& message, int n, MatrixType matrixType);
    Encoder(const vector<vector<uint8_t>>& message, const vector<vector<int>>& generatorMatrix);

    // 编码函数
    vector<vector<uint8_t>> encode();
    // 性能测试函数
    double benchmarkEncode(int iterations);

private:
    vector<vector<uint8_t>> messages;
    vector<vector<int>> generatorMatrix;
    int n, k, L;
    MatrixType matrixType;

    // 优化后的移位函数
    static void fastShiftXOR(uint8_t* dest, const uint8_t* src, int shift, int length);
    // 并行编码工作函数
    void encodeWorker(int startRow, int endRow, vector<vector<uint8_t>>& encodedMessage, const vector<int>& t_star);
};

#endif
