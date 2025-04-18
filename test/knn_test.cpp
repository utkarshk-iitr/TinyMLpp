#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <utility>
#include <array>
#include <cstdlib>
#include "../src/data_handling.h"  // Contains Data, readCSV(), toDouble(), etc.
#include "../src/knn.cpp"         // Contains the KNN class

using namespace std;

/**
 * @brief Executes a shell command and captures its output.
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
 * @brief Parses whitespace-separated integers from a string.
 */
vector<int> parsePythonOutput(const string &output) {
    vector<int> assignments;
    istringstream iss(output);
    int val;
    while (iss >> val) assignments.push_back(val);
    return assignments;
}

void writeCSV(const handle::Data &d, const string &fname) {
    ofstream out(fname);
    // header
    for (size_t j = 0; j < d.header.size(); ++j)
        out << d.header[j] << (j + 1 < d.header.size() ? "," : "\n");
    // rows
    for (size_t i = 0; i < d.features.size(); ++i) {
        for (size_t j = 0; j < d.features[i].size(); ++j)
            out << d.features[i][j] << ",";
        out << d.target[i] << "\n";
    }
}

int main() {
    try {
        // 1) Load dataset
        string filename = "Iris.csv";
        handle::Data data = handle::readCSV(filename);

        // 2) Setup KNN and split
        int k = 3;
        double test_size = 0.5;
        unsigned seed = 42;
        KNN knn(k, test_size, seed);

        auto [trainSet, testSet] = train_test_split(data, test_size, seed);
        writeCSV(trainSet, "train.csv");
        writeCSV(testSet,  "test.csv");

        // Train and predict in C++
        knn.train(trainSet);
        vector<double> predicted = knn.predict(testSet);
        vector<double> actual;
        for (auto &val : testSet.target)
            actual.push_back(handle::toDouble(val));

        double accuracy = handle::computeAccuracy(actual, predicted);
        cout << "C++ KNN Accuracy: " << accuracy * 100 << "%\n";
        for (size_t i = 0; i < predicted.size(); ++i)
        if(predicted[i] != actual[i])
            cout << predicted[i] << " vs " << actual[i] << "\n";
        cout << endl;

        // 3) Write temporary Python KNN script
        ofstream pyFile("temp_knn.py");
        if (!pyFile) throw runtime_error("Unable to create Python script.");
        pyFile << R"(#!/usr/bin/env python3
import sys, csv, numpy as np
from collections import Counter

def load_csv(fname):
    with open(fname) as f:
        reader = csv.reader(f)
        rows = list(reader)
    data = rows[1:]
    X = np.array([list(map(float, r[:-1])) for r in data])
    y = np.array(list(map(int, [r[-1] for r in data])))
    return X, y

def knn_predict(X_train, y_train, X_test, k=3):
    from numpy.linalg import norm
    preds = []
    for x in X_test:
        dists = norm(X_train - x, axis=1)
        nn = np.argsort(dists)[:k]
        vote = Counter(y_train[nn])
        preds.append(vote.most_common(1)[0][0])
    return preds

if __name__ == '__main__':
    train_file, test_file = sys.argv[1], sys.argv[2]
    k = int(sys.argv[3]) if len(sys.argv) > 3 else 3
    X_train, y_train = load_csv(train_file)
    X_test, y_test = load_csv(test_file)
    preds = knn_predict(X_train, y_train, X_test, k)
    print(" ".join(map(str, preds)))
    acc = sum(p == a for p,a in zip(preds, y_test)) / len(y_test)
    sys.stdout.write(f"\nAccuracy: {acc*100:.2f}%\n")
)";
        pyFile.close();
        system("chmod +x temp_knn.py");

        // 4) Run Python script with both train/test filenames and k
        string pyCommand = "./temp_knn.py train.csv test.csv " + to_string(k);
        string pyOutput = exec(pyCommand.c_str());
        vector<int> pyAssignments = parsePythonOutput(pyOutput);

        // 5) Compare C++ vs Python predictions
        bool passed = (predicted.size() == pyAssignments.size());
        if (passed) {
            for (size_t i = 0; i < predicted.size(); ++i) {
                if (static_cast<int>(predicted[i]) != pyAssignments[i]) {
                    passed = false;
                    break;
                }
            }
        }
        if (passed) cout << "Test passed: matching predictions." << endl;
        else {
            cout << "Test FAILED. C++: ";
            for (auto &v : predicted) cout << v << " ";
            cout << "\nPython: ";
            for (auto &v : pyAssignments) cout << v << " ";
            cout << endl;
        }

        knn.plotKNNResults(testSet, predicted);
        remove("temp_knn.py");
    } catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}
