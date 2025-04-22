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
        string filename = "./datasets/diabetes.csv";          // expects last column = class label {-1, +1} or {0,1}
        Data data = readCSV(filename);
        standardize(data);

    // convert labels {0,1} → {-1,+1}
    for (auto &lbl : data.target) {
        double y = toDouble(lbl);
        lbl = (y == 0.0 ? "-1" : "1");
    }

    // 2) Train C++ SVM
    SVM svm(1.0, 0.01, 1000);
    void* raw = svm.train(data);
    double* params = static_cast<double*>(raw);
    
    size_t n_feats = data.features[0].size();
    vector<double> svmParams(n_feats + 1);
    svmParams[0] = params[0];  // bias
    for (size_t j = 0; j < n_feats; ++j)
        svmParams[j + 1] = params[j + 1];  // weights
    

    // 3) Get C++ predictions
    vector<double> cppPreds = svm.predict(data);

    // 4) Print out results
    cout << "=== C++ SVM Results ===\n\n";

    // cout << "Predictions (" << cppPreds.size() << " samples):\n";
    // for (double p : cppPreds) cout << p << ' ';
    // cout << "\n\n";
    vector<double> labels;
    for (size_t i = 0; i < data.target.size(); ++i) {
        double y = toDouble(data.target[i]);
        labels.push_back(y);
    }
    cout << computeAccuracy(labels, cppPreds)*100 << " % accuracy\n";

    cout << "Parameters:\n";
    cout << "  bias = " << svmParams[0] << '\n';
    cout << "  weights = [ ";
    for (size_t j = 0; j < n_feats; ++j)
        cout << svmParams[j+1] << (j+1 < n_feats ? ", " : " ");
    cout << "]\n";

    delete[] params;
        svm.plotSVM(data, svmParams);
    }
    catch (const exception &e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
