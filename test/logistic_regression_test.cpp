// logistic_regression_test.cpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <cstdlib>
#include <cmath>

#include "../src/data_handling.h"         // Data, readCSV(), toDouble(), computeLogLoss(), standardize(), etc.
#include "../src/logistic_regression.cpp" // Your LogisticRegression class

using namespace std;
using namespace handle;  // for Data

// Execute a shell command and capture its stdout.
string exec(const char* cmd) {
    array<char, 128> buffer;
    string result;
    shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw runtime_error("popen() failed!");
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        result += buffer.data();
    return result;
}

// Parse whitespace‑separated doubles from a single line into a vector.
vector<double> parseLine(const string &line) {
    istringstream iss(line);
    vector<double> v;
    double x;
    while (iss >> x) v.push_back(x);
    return v;
}

// Compare two double vectors for near‑equality.
bool compareVectors(const vector<double>& a, const vector<double>& b, double tol = 1e-3) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); i++) {
        if (fabs(a[i] - b[i]) > tol) return false;
    }
    return true;
}

int main() {
    try {
        // 1. Load and normalize in C++
        string filename = "./datasets/placement.csv";
        Data data = readCSV(filename);
        displayDataFrame(data);
        standardize(data);

        // 2. C++ Logistic Regression
        LogisticRegression lr(0.01, 1000);
        auto [trainData, testData] = train_test_split(data, 0.2, 42);
        cout << "Train set size: " << trainData.features.size() << endl;
        cout << "Test set size: " << testData.features.size() << endl;
        void* raw_params = lr.train(trainData); // Train the model
        double* cppThetaArr = static_cast<double*>(raw_params);
        size_t n = data.features[0].size();

        vector<double> cppTheta(n+1);
        cout << "C++ θ vector:" << endl;
        for (size_t i = 0; i <= n; i++) {
            cppTheta[i] = cppThetaArr[i];
            cout << " θ[" << i << "] = " << cppTheta[i] << endl;
        }
        vector<double> cppProbs = lr.predict(testData);

        
        lr.plotLR(testData, cppTheta);
        return 0;
    }
    catch (const exception &e) {
        cerr << "Test failed: " << e.what() << endl;
        return 1;
    }
}
