#include "Encoder.h"
#include <iostream>
#include <thread>

Encoder::Encoder(const vector<vector<uint8_t>>& message, int n, MatrixType matrixType)
        : messages(message), n(n), matrixType(matrixType) {
    k = message.size();
    L = message[0].size();
    generatorMatrix = vector<vector<int>>(n, vector<int>(k, 0));

    if (matrixType == MatrixType::TWO_TONE) {
        int d = n / 2;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < k; ++j) {
                if (i < d) {
                    generatorMatrix[i][j] = (d - i) * (k - j - 1);
                } else {
                    generatorMatrix[i][j] = (i - d) * j;
                }
            }
        }
    }
    else if (matrixType == MatrixType::SYSTEMATIC_TWOTONE) {
        // 前k行为单位矩阵（原始数据）
        for (int i = 0; i < k; ++i) {
            for (int j = 0; j < k; ++j) {
                generatorMatrix[i][j] = (i == j) ? 0 : INT_MAX; // 0表示不移位，INT_MAX表示不参与异或
            }
        }

        // 后(n-k)行为Two-tone矩阵（冗余数据）
        int d = (n - k) / 2;
        for (int i = k; i < n; ++i) {
            for (int j = 0; j < k; ++j) {
                if (i < k + d) {
                    generatorMatrix[i][j] = (d - (i - k)) * (k - j - 1);
                } else {
                    generatorMatrix[i][j] = ((i - k) - d) * j;
                }
            }
        }
    }
    else if (matrixType == MatrixType::SYSTEMATIC_RID) {
        // 前k行为单位矩阵（原始数据）
        for (int i = 0; i < k; ++i) {
            for (int j = 0; j < k; ++j) {
                generatorMatrix[i][j] = (i == j) ? 0 : INT_MAX;
            }
        }

        // 后(n-k)行为RID矩阵（冗余数据）
        for (int i = k; i < n; ++i) {
            for (int j = 0; j < k; ++j) {
                generatorMatrix[i][j] = (i - k + 1) * j;
            }
        }
    }
}

// 优化的移位异或操作
void Encoder::fastShiftXOR(uint8_t* dest, const uint8_t* src, int shift, int length) {
    int byteShift = shift / 8;
    int bitShift = shift % 8;

    if (bitShift == 0) {
        // 按字节对齐的情况
        for (int i = 0; i < (length - shift) / 8; ++i) {
            dest[byteShift + i] ^= src[i];
        }
    } else {
        // 非字节对齐的情况
        uint16_t buffer;
        for (int i = 0; i < (length - shift) / 8; ++i) {
            buffer = (src[i] << bitShift);
            if (i + 1 < length / 8) {
                buffer |= (src[i + 1] >> (8 - bitShift));
            }
            dest[byteShift + i] ^= buffer & 0xFF;
        }
    }
}

void Encoder::encodeWorker(int startRow, int endRow, vector<vector<uint8_t>>& encodedMessage, const vector<int>& t_star) {
    for (int i = startRow; i < endRow; ++i) {
        for (int j = 0; j < k; ++j) {
            if (generatorMatrix[i][j] != INT_MAX) {
                fastShiftXOR(encodedMessage[i].data(),
                           messages[j].data(),
                           generatorMatrix[i][j],
                           L + t_star[i]);
            }
        }
    }
}

vector<vector<uint8_t>> Encoder::encode() {
    vector<int> t_star(n, 0);

    // 计算每行的最大移位
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < k; ++j) {
            if (generatorMatrix[i][j] != INT_MAX) {
                t_star[i] = max(t_star[i], generatorMatrix[i][j]);
            }
        }
    }

    // 初始化编码结果
    vector<vector<uint8_t>> encodedMessage(n);
    for (int i = 0; i < n; ++i) {
        encodedMessage[i].resize((L + t_star[i] + 7) / 8 * 8, 0);
    }

    // 使用多线程并行计算
    const int numThreads = thread::hardware_concurrency();
    vector<thread> threads;
    int rowsPerThread = n / numThreads;

    for (int i = 0; i < numThreads; ++i) {
        int startRow = i * rowsPerThread;
        int endRow = (i == numThreads - 1) ? n : (i + 1) * rowsPerThread;
        threads.emplace_back(&Encoder::encodeWorker, this, startRow, endRow,
                           ref(encodedMessage), ref(t_star));
    }

    for (auto& t : threads) {
        t.join();
    }

    return encodedMessage;
}

double Encoder::benchmarkEncode(int iterations) {
    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        encode();
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = end - start;
    return diff.count() / iterations;
}
