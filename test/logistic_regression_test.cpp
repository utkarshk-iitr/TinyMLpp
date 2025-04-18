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
        string filename = "placementt.csv";
        Data data = readCSV(filename);
        // (optional) displayDataFrame(data);
        standardize(data);

        // 2. C++ Logistic Regression
        LogisticRegression lr(0.01, 1000);
        void* raw_params = lr.train(data);
        double* cppThetaArr = static_cast<double*>(raw_params);
        size_t n = data.features[0].size();

        vector<double> cppTheta(n+1);
        cout << "C++ θ vector:" << endl;
        for (size_t i = 0; i <= n; i++) {
            cppTheta[i] = cppThetaArr[i];
            cout << " θ[" << i << "] = " << cppTheta[i] << endl;
        }
        vector<double> cppProbs = lr.predict(data);

        // 3. Write Python script with gradient‑descent LR
        ofstream py("temp_logreg.py");
        if (!py) throw runtime_error("Cannot create temp_logreg.py");
        py << R"(#!/usr/bin/env python3
import sys
import pandas as pd
import numpy as np

def sigmoid(z):
    return 1.0 / (1.0 + np.exp(-z))

def main():
    fn = sys.argv[1] if len(sys.argv)>1 else "placementt.csv"
    df = pd.read_csv(fn)
    X = df.iloc[:, :-1].values
    y = df.iloc[:, -1].values.reshape(-1,1)
    
    # Standardize features
    mean = X.mean(axis=0, keepdims=True)
    std  = X.std(axis=0, keepdims=True) + 1e-8
    Xn = (X - mean) / std

    m, d = Xn.shape
    # Add intercept term
    Xb = np.hstack([np.ones((m,1)), Xn])
    
    # Hyperparameters
    lr = 0.01
    iters = 1000

    # Initialize theta
    theta = np.zeros((d+1, 1))

    # Gradient descent loop
    for _ in range(iters):
        z = Xb.dot(theta)               # (m x 1)
        h = sigmoid(z)                  # (m x 1)
        grad = (Xb.T.dot(h - y)) / m    # (d+1 x 1)
        theta -= lr * grad

    # Predictions
    probs = sigmoid(Xb.dot(theta)).flatten()

    # Print θ on first line, probs on second
    print(" ".join(map(str, theta.flatten())))
    print(" ".join(map(str, probs)))

if __name__ == "__main__":
    main()
)";
        py.close();
        system("chmod +x temp_logreg.py");

        // 4. Run Python script and capture output
        string output = exec("./temp_logreg.py placementt.csv");
        istringstream iss(output);
        string line1, line2;
        if (!getline(iss, line1) || !getline(iss, line2))
            throw runtime_error("Python output parsing failed.");

        vector<double> pyTheta = parseLine(line1);
        vector<double> pyProbs = parseLine(line2);

        // 5. Compare θ vectors
        bool thetaMatch = compareVectors(cppTheta, pyTheta, 1e-2);
        cout << (thetaMatch ? "θ vectors match." : "θ vectors differ!") << endl;
        cout << "C++ θ: ";
        for (auto v : cppTheta) cout << v << " ";
        cout << "\nPy  θ: ";
        for (auto v : pyTheta) cout << v << " ";
        cout << endl;
        
        lr.plotLR(data, cppTheta);
        // 6. Compare probabilities
        bool probMatch = compareVectors(cppProbs, pyProbs, 1e-3);
        cout << (probMatch ? "Probabilities match." : "Probabilities differ!") << endl;
        cout << "C++ probs: ";
        for (auto p : cppProbs) cout << p << " ";
        cout << "\nPy  probs: ";
        for (auto p : pyProbs) cout << p << " ";
        cout << endl;

        // Cleanup
        remove("temp_logreg.py");
        delete[] cppThetaArr;

        return (thetaMatch && probMatch) ? 0 : 1;
    }
    catch (const exception &e) {
        cerr << "Test failed: " << e.what() << endl;
        return 1;
    }
}
