// test_svm.cpp
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
#include "../src/data_handling.h"   // Data, readCSV(), toDouble(), etc.
#include "../src/svm.cpp"             // Your from‑scratch SVM class

using namespace std;
using namespace handle;

/** 
 * @brief Run a shell command and capture its entire output.
 */
string exec(const char* cmd) {
    array<char, 128> buffer;
    string result;
    shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw runtime_error("popen() failed!");
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        result += buffer.data();
    return result;
}

/**
 * @brief Parse whitespace‑separated doubles into a vector.
 */
vector<double> parsePythonOutputDouble(const string &output) {
    vector<double> v;
    istringstream iss(output);
    double x;
    while (iss >> x) v.push_back(x);
    return v;
}

/**
 * @brief Compare two double vectors within a tolerance.
 */
bool compareVectors(const vector<double>& a, const vector<double>& b, double tol = 1e-4) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i)
        if (fabs(a[i] - b[i]) > tol) return false;
    return true;
}

int main() {
    try {
        // ----------------------------------------------------------------
        // 1) Load dataset
        // ----------------------------------------------------------------
        string filename = "diabetes.csv";          // expects last column = class label {-1, +1} or {0,1}
        Data data = readCSV(filename);
        // Convert labels to {-1,+1} in place:
        for (auto &lbl : data.target) {
            double y = toDouble(lbl);
            lbl = (y == 0.0 ? "-1" : "1");
        }

        // ----------------------------------------------------------------
        // 2) Train C++ SVM
        // ----------------------------------------------------------------
        SVM svm(1.0, 0.005, 10000);
        void* raw = svm.train(data);
        double* params = static_cast<double*>(raw);
        size_t n_feats = data.features[0].size();
        
        cout<<n_feats<<endl;
        // get predictions
        vector<double> cppPreds = svm.predict(data);

        // copy params into vector for plotting
        vector<double> svmParams;
        svmParams.push_back(params[0]); // bias
        cout<<svmParams[0]<<" ";
        for (size_t i = 0; i < n_feats; ++i)
        {
            svmParams.push_back(params[i+1]);
            cout<<svmParams[i]<<" ";
        }   
        cout<<endl;
        delete[] params;

        // ----------------------------------------------------------------
        // 3) Write Python test script (scikit‑learn SVM)
        // ----------------------------------------------------------------
        ofstream py("temp_svm.py");
        if (!py) throw runtime_error("Cannot write temp_svm.py");
        py << R"(#!/usr/bin/env python3
import sys
import numpy as np
import pandas as pd
from sklearn.svm import SVC

# load
df = pd.read_csv(sys.argv[1])
X = df.iloc[:, :-1].values
y = df.iloc[:, -1].map({0: -1, 1: 1}).values

# train & predict
clf = SVC(C=1.0, kernel='linear')
clf.fit(X, y)
preds = clf.predict(X)

# print predictions
# print(" ".join(map(str, preds)))

# print parameters
w = clf.coef_[0]
b = clf.intercept_[0]

# Print as: bias w0 w1 w2 ...
params = [b] + w.tolist()
print(" ".join(map(str, params)))

)";
        py.close();
        system("chmod +x temp_svm.py");

        // ----------------------------------------------------------------
        // 4) Run Python script & parse
        // ----------------------------------------------------------------
        string out = exec("./temp_svm.py diabetes.csv");
        vector<double> pyPreds = parsePythonOutputDouble(out);

        // ----------------------------------------------------------------
        // 5) Compare
        // ----------------------------------------------------------------
        bool ok = compareVectors(cppPreds, pyPreds, 1e-6);
        if (ok) {
            cout << "Test passed: C++ and Python SVM predictions match.\n";
        } else {
            // cout << "Test FAILED.\nC++ preds: ";
            // for (auto &v : cppPreds) cout << v << " ";
            cout << "\nPython preds: ";
            for (auto &v : pyPreds)  cout << v << " ";
            cout << "\n";
        }

        // ----------------------------------------------------------------
        // 6) Plot decision boundary (only for 2D features)
        // ----------------------------------------------------------------
        svm.plotSVM(data, svmParams);

        // cleanup
        remove("temp_svm.py");
    }
    catch (const exception &e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
