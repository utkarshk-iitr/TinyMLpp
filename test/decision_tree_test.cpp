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
        string filename = "toy.csv";
        Data data = readCSV(filename);
        displayDataFrame(data);
        standardize(data);

        // 2. Train C++ DecisionTree.
        DecisionTree dt(/*maxDepth=*/5, /*minSamplesSplit=*/2);
        dt.train(data);
        
        vector<double> cppPreds = dt.predict(data);

        // 3. Write Python test script.
        ofstream pyFile("temp_dectree.py");
        if (!pyFile) throw runtime_error("Unable to create Python script.");
        pyFile << R"(#!/usr/bin/env python3
# coding: utf-8
import sys
import pandas as pd
from sklearn.tree import DecisionTreeRegressor

if __name__ == '__main__':
    fn = sys.argv[1] if len(sys.argv)>1 else 'toy.csv'
    df = pd.read_csv(fn)
    X = df.iloc[:, :-1].values
    y = df.iloc[:, -1].values
    model = DecisionTreeRegressor(max_depth=5, min_samples_split=2, random_state=42)
    model.fit(X, y)
    preds = model.predict(X)
    print(" ".join(map(str, preds)))
)";
        pyFile.close();
        system("chmod +x temp_dectree.py");

        // 4. Execute Python script.
        string pyOut = exec("./temp_dectree.py toy.csv");
        vector<double> pyPreds = parsePythonOutputDouble(pyOut);

        // 5. Compare predictions.
        bool ok = compareVectors(cppPreds, pyPreds, 1e-2);
        if (ok) {
            cout << "Test passed: C++ and Python Decision Tree predictions match." << endl;
        } else {
            cout << "Test FAILED: Predictions differ." << endl;
            cout << "C++ preds:";
            for (double v : cppPreds) cout << " " << v;
            cout << "\nPython preds:";
            for (double v : pyPreds) cout << " " << v;
            cout << endl;
        }

        // 6. Plot the tree
        dt.plotTree();
        dt.plot();
        

        // 7. Cleanup.
        remove("temp_dectree.py");

    } catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}
