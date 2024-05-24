// main.cpp

#include <iostream>
#include "Encoder.h"

using namespace std;

int main() {
    vector<vector<int>> message = {
            {1, 0, 1}, // x1
            {0, 1, 0}  // x2
    };
    int n = 5; // User input for n

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
