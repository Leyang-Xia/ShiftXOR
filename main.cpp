#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

using namespace std;

enum class MatrixType {
    RID,
    TWO_TONE
};

class Encoder {
public:
    // Constructor with message input and default matrix type
    Encoder(const vector<vector<int>>& message, int n, MatrixType matrixType)
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
    Encoder(const vector<vector<int>>& message, const vector<vector<int>>& generatorMatrix)
            : messages(message), generatorMatrix(generatorMatrix), matrixType(MatrixType::RID) { // default matrixType if matrix is given
        n = generatorMatrix.size();
        k = generatorMatrix[0].size();
        L = message[0].size();
    }

    vector<vector<int>> encode() {
        vector<vector<int>> encodedMessage(n);

        for (int i = 0; i < n; ++i) {
            int t_star_i = 0;
            for (int j = 0; j < k; ++j) {
                if (generatorMatrix[i][j] != INT_MAX) {
                    t_star_i = max(t_star_i, generatorMatrix[i][j]);
                }
            }
            int encodedLength = L + t_star_i;
            encodedMessage[i] = vector<int>(encodedLength, 0);
            for (int j = 0; j < k; ++j) {
                if (generatorMatrix[i][j] != INT_MAX) {
                    vector<int> shiftedMessage = rightShift(messages[j], generatorMatrix[i][j], encodedLength);
                    for (int bit = 0; bit < encodedLength; ++bit) {
                        encodedMessage[i][bit] ^= shiftedMessage[bit];
                    }
                }
            }
        }
        return encodedMessage;
    }

private:
    vector<vector<int>> messages;
    vector<vector<int>> generatorMatrix;
    int n;
    int k;
    int L;
    MatrixType matrixType;

    static vector<int> rightShift(const vector<int>& vec, int shift, int newLength) {
        int n = vec.size();
        vector<int> result(newLength, 0);
        for (int i = 0; i < n; ++i) {
            if (i + shift < newLength) {
                result[i + shift] = vec[i];
            }
        }
        return result;
    }
};

int main() {
    vector<vector<int>> message = {
            {1, 0, 1}, // x1
            {0, 1, 0}  // x2
    };
    int n = 6; // User input for n

    // Using constructor with message input and TWO_TONE matrix
    Encoder encoder(message, n, MatrixType::TWO_TONE);
    vector<vector<int>> encodedMessage = encoder.encode();

    cout << "Encoded message:" << endl;
    for (const auto& seq : encodedMessage) {
        for (int bit : seq) {
            cout << bit;
        }
        cout << endl;
    }

    return 0;
}
