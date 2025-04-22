#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <cstdlib>
#include "../src/data_handling.h"  // Contains the Data definition and readCSV(), toDouble(), etc.
#include "../src/k_means_clustering.cpp"         // Contains the KMeans class

using namespace std;

/**
 * @brief Executes a shell command and captures its output.
 * 
 * @param cmd The command string to execute.
 * @return The full output of the command.
 */
string exec(const char* cmd) {
    array<char, 128> buffer;
    string result;
    // Open a pipe to run the command.
    shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

/**
 * @brief Parses a whitespace‐separated string of integers into a vector.
 * 
 * @param output The input string.
 * @return A vector of integers parsed from the string.
 */
vector<int> parsePythonOutput(const string &output) {
    vector<int> assignments;
    istringstream iss(output);
    int val;
    while (iss >> val) {
        assignments.push_back(val);
    }
    return assignments;
}

int main() {
    try {
        std::string filename = "./datasets/iris2.csv";
        handle::Data data = handle::readCSV(filename);
        KMeans kmeans(3, 100, 1e-7);
        kmeans.train(data); 
        vector<double> cppAssignmentsDouble = kmeans.predict(data);
        vector<int> cppAssignments;
        for (double d : cppAssignmentsDouble) {
            cppAssignments.push_back(static_cast<int>(d));
        }
        kmeans.plotKMeansClusters(data, cppAssignments, 3);
    } catch (const exception &e) {
        cerr << "Test failed: " << e.what() << endl;
        return 1;
    }
    return 0;
}
