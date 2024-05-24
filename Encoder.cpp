// encoder.cpp

#include "Encoder.h"


// Constructor with message input and default matrix type
Encoder::Encoder(const vector<vector<int>>& message, int n, MatrixType matrixType)
        : messages(message), n(n), matrixType(matrixType) {
    k = message.size();
    L = message[0].size();
    generatorMatrix = vector<vector<int>>(n, vector<int>(k, 0));

    if (matrixType == MatrixType::RID) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < k; ++j) {
                generatorMatrix[i][j] = (i * j); // RID matrix: default shift values
            }
        }
    } else if (matrixType == MatrixType::TWO_TONE) {
        int d = n / 2; // divide d, ensure d covers half of the n
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
}

// Constructor with message and generator matrix input
Encoder::Encoder(const vector<vector<int>>& message, const vector<vector<int>>& generatorMatrix)
        : messages(message), generatorMatrix(generatorMatrix), matrixType(MatrixType::CUSTOMIZE) { // default matrixType if matrix is given
    n = generatorMatrix.size();
    k = generatorMatrix[0].size();
    L = message[0].size();
}

vector<vector<int>> Encoder::encode() {
    vector<vector<int>> encodedMessage(n, vector<int>(L, 0));
    vector<int> t_star(n, 0);

    // Precompute t_star values
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < k; ++j) {
            if (generatorMatrix[i][j] != INT_MAX) {
                t_star[i] = max(t_star[i], generatorMatrix[i][j]);
            }
        }
    }

    // Resize encodedMessage to the correct lengths
    for (int i = 0; i < n; ++i) {
        encodedMessage[i].resize(L + t_star[i], 0);
    }

    // Encode messages
    for (int j = 0; j < k; ++j) {
        for (int i = 0; i < n; ++i) {
            if (generatorMatrix[i][j] != INT_MAX) {
                vector<int> shiftedMessage = rightShift(messages[j], generatorMatrix[i][j], L + t_star[i]);
                for (int bit = 0; bit < L + t_star[i]; ++bit) {
                    encodedMessage[i][bit] ^= shiftedMessage[bit];
                }
            }
        }
    }

    return encodedMessage;
}

vector<int> Encoder::rightShift(const vector<int>& vec, int shift, int newLength) {
    int n = vec.size();
    vector<int> result(newLength, 0);
    for (int i = 0; i < n; ++i) {
        if (i + shift < newLength) {
            result[i + shift] = vec[i];
        }
    }
    return result;
}
