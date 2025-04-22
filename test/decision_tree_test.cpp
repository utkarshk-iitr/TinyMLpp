// Decision Tree Test Program
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
#include <string>
#include "../src/data_handling.h"   // Data definitions and utilities
#include "../src/decision_tree.cpp"   // DecisionTree definitions and gnuplot plotting

using namespace std;
using namespace handle;

// Execute a shell command and capture its output.
string exec(const char* cmd) {
    array<char, 128> buffer;
    string result;
    shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw runtime_error("popen() failed!");
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

// Parse whitespace-separated doubles from Python output.
vector<double> parsePythonOutputDouble(const string &output) {
    vector<double> preds;
    istringstream iss(output);
    double v;
    while (iss >> v) preds.push_back(v);
    return preds;
}

// Compare two vectors within tolerance.
bool compareVectors(const vector<double>& a, const vector<double>& b, double tol = 1e-4) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (fabs(a[i] - b[i]) > tol) return false;
    }
    return true;
}

int main() {
    try {
        // 1. Load and preprocess data.
        string filename = "./datasets/placement.csv";
        Data data = readCSV(filename);
        // displayDataFrame(data);
        standardize(data);

        // 2. Train C++ DecisionTree.
        DecisionTree dt(5,2);
        dt.train(data);
        
        vector<double> cppPreds = dt.predict(data);
        cout << "C++ Decision Tree Predictions: ";
        for (const auto& pred : cppPreds) {
            cout << pred << " ";
        }
        cout << endl;
        dt.plot(data,0.1);

    } catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}
