// headfile.h

#ifndef ENCODER_H
#define ENCODER_H


#include <algorithm>
#include <climits>
#include <vector>
using namespace std;

enum class MatrixType {
    RID,
    TWO_TONE,
    CUSTOMIZE
};

class Encoder {
public:
    // Constructor with message input and default matrix type
    Encoder(const vector<vector<int>>& message, int n, MatrixType matrixType);

    // Constructor with message and generator matrix input
    Encoder(const vector<vector<int>>& message, const vector<vector<int>>& generatorMatrix);

    vector<vector<int>> encode();

private:
    vector<vector<int>> messages;
    vector<vector<int>> generatorMatrix;
    int n;
    int k;
    int L;
    MatrixType matrixType;

    static vector<int> rightShift(const vector<int>& vec, int shift, int newLength);
};

#endif
