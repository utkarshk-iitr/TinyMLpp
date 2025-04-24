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
        string filename = "./datasets/iris.csv";
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

        knn.plot(testSet);
    } catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}
