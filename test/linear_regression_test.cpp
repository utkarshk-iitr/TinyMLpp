// test_linear_regression.cpp
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
#include "../src/data_handling.h"       // Contains the Data definition, readCSV(), toDouble(), etc.
#include "../src/linear_regression.cpp"     // Contains the LinearRegression class

using namespace std;
using namespace handle;
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
 * @brief Parses a whitespace‐separated string of doubles into a vector.
 * 
 * @param output The input string.
 * @return A vector of doubles parsed from the string.
 */
vector<double> parsePythonOutputDouble(const string &output) {
    vector<double> predictions;
    istringstream iss(output);
    double val;
    while (iss >> val) {
        predictions.push_back(val);
    }
    return predictions;
}

/**
 * @brief Compares two vectors of doubles for near equality.
 * 
 * @param vec1 First vector.
 * @param vec2 Second vector.
 * @param tol Tolerance for differences (default: 1e-4).
 * @return true if the vectors are of equal size and all elements differ by no more than tol.
 */
bool compareVectors(const vector<double>& vec1, const vector<double>& vec2, double tol = 1e-4) {
    if (vec1.size() != vec2.size())
        return false;
    for (size_t i = 0; i < vec1.size(); i++) {
        if (fabs(vec1[i] - vec2[i]) > tol)
            return false;
    }
    return true;
}

int main() {
    try {
        // --------------------------------------------------------------------
        // 1. Load dataset from "exported_dataset.csv" using our data handler.
        // --------------------------------------------------------------------
        std::string filename = "placementt.csv";
        Data data = readCSV(filename);
        standardize(data); // Normalize the data (optional, but recommended for linear regression)
        // --------------------------------------------------------------------
        // 2. Run C++ Linear Regression.
        // --------------------------------------------------------------------
        // Create a LinearRegression object with learning rate = 0.01 and iterations = 1000.
        LinearRegression lr(0.01, 1000);
        lr.train(data);
        vector<double> cppPredictions = lr.predict(data);

        // --------------------------------------------------------------------
        // 3. Write a temporary Python script to perform Linear Regression.0
        // --------------------------------------------------------------------
        // This Python script reads the CSV file, separates the features and the target
        // (assumes the last column is the target), and uses gradient descent to fit theta.
        // It then outputs the predictions (X @ theta) as whitespace-separated values.
        ofstream pyFile("temp_linreg.py");
        if (!pyFile) {
            throw runtime_error("Unable to create temporary Python file.");
        }
        pyFile << R"(#!/usr/bin/env python3
import sys
import pandas as pd
from sklearn.linear_model import LinearRegression

def read_csv(filename):
    data = pd.read_csv(filename)
    return data

if __name__ == "__main__":
    filename = sys.argv[1] if len(sys.argv) > 1 else "placementt.csv"
    data = read_csv(filename)
    # Assume last column is the target, remaining columns are features.
    X = data.iloc[:, :-1].values
    y = data.iloc[:, -1].values
    # Create and train the linear regression model.
    model = LinearRegression()
    model.fit(X, y)
    # Predict using the trained model.
    predictions = model.predict(X)
    # Output the predictions as whitespace-separated values.
    print(" ".join(map(str, predictions)))
)";
        pyFile.close();

        // Make the temporary Python script executable.
        system("chmod +x temp_linreg.py");

        // --------------------------------------------------------------------
        // 4. Run the Python script and capture its output.
        // --------------------------------------------------------------------
        string pyCommand = "./temp_linreg.py placementt.csv";
        string pyOutput = exec(pyCommand.c_str());
        vector<double> pyPredictions = parsePythonOutputDouble(pyOutput);
        // --------------------------------------------------------------------
        // 5. Compare the results from C++ and Python.
        // --------------------------------------------------------------------
        bool passed = compareVectors(cppPredictions, pyPredictions, 1e-4);

        // --------------------------------------------------------------------
        // 6. Report the test result.
        // --------------------------------------------------------------------
        if (passed) {
            cout << "Test passed: C++ and Python Linear Regression predictions are equal." << endl;
        } else {
            cout << "Test FAILED: C++ and Python Linear Regression predictions differ." << endl;
            cout << "C++ predictions:" << endl;
            for (double p : cppPredictions) {
                cout << p << " ";
            }
            cout << "\nPython predictions:" << endl;
            for (double p : pyPredictions) {
                cout << p << " ";
            }
            cout << endl;
        }

        // Optionally, delete the temporary Python file.
        remove("temp_linreg.py");
        
    } catch (const exception &e) {
        cerr << "Test failed: " << e.what() << endl;
        return 1;
    }
    return 0;
}
