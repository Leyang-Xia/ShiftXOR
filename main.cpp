#include <iostream>
#include <random>
#include "Encoder.h"

using namespace std;

int main() {
    const int BLOCK_SIZE = 32768;  // 32KB
    const int K = 8;               // 8个数据块
    const int N = 12;              // 12个总块（包括4个冗余块）

    // 生成随机测试数据
    vector<vector<uint8_t>> message(K, vector<uint8_t>(BLOCK_SIZE));
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 255);

    for (auto& block : message) {
        for (uint8_t& byte : block) {
            byte = dis(gen);
        }
    }

    // 测试不同的矩阵类型
    vector<MatrixType> matrixTypes = {
        MatrixType::RID,
        MatrixType::TWO_TONE,
        MatrixType::SYSTEMATIC_TWOTONE,
        MatrixType::SYSTEMATIC_RID
    };

    const int iterations = 1000;

    for (auto type : matrixTypes) {
        cout << "\n测试矩阵类型: ";
        switch (type) {
            case MatrixType::RID: cout << "RID"; break;
            case MatrixType::TWO_TONE: cout << "TWO_TONE"; break;
            case MatrixType::SYSTEMATIC_TWOTONE: cout << "SYSTEMATIC_TWOTONE"; break;
            case MatrixType::SYSTEMATIC_RID: cout << "SYSTEMATIC_RID"; break;
            default: cout << "OTHER"; break;
        }
        cout << endl;

        Encoder encoder(message, N, type);
        double avgTime = encoder.benchmarkEncode(iterations);
        double throughput = (K * BLOCK_SIZE) / (avgTime * 1024 * 1024); // MB/s

        cout << "平均编码时间: " << avgTime * 1000 << " ms" << endl;
        cout << "编码吞吐量: " << throughput << " MB/s" << endl;
    }

    return 0;
}
