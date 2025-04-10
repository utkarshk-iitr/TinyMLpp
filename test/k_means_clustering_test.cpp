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
        // --------------------------------------------------------------------
        // 1. Load dataset from "exported_dataset.csv" using our C++ data handler.
        // --------------------------------------------------------------------
        std::string filename = "iris_data_without_labels.csv";
        handle::Data data = handle::readCSV(filename);

        // --------------------------------------------------------------------
        // 2. Run C++ k-Means clustering.
        // --------------------------------------------------------------------
        // For example, we use k=3 clusters, maxIterations=100, tol=1e-4.
        KMeans kmeans(3, 100, 1e-4);
        kmeans.train(data); 
        vector<double> cppAssignmentsDouble = kmeans.predict(data);
        vector<int> cppAssignments;
        for (double d : cppAssignmentsDouble) {
            cppAssignments.push_back(static_cast<int>(d));
        }

        // --------------------------------------------------------------------
        // 3. Write a temporary Python script to perform k-Means clustering.
        // --------------------------------------------------------------------
        // The Python script will read "exported_dataset.csv" and use a comparable algorithm.
        ofstream pyFile("temp_kmeans.py");
        if (!pyFile) {
            throw runtime_error("Unable to create temporary Python file.");
        }
        pyFile << R"(#!/usr/bin/env python3
import sys
import csv
import numpy as np

def kmeans_clustering(filename, k=3, max_iterations=100, tol=1e-4):
    with open(filename, newline='') as csvfile:
        reader = csv.reader(csvfile)
        rows = list(reader)
    header = rows[0]
    data = rows[1:]
    
    # Convert all rows (features and target) to floats.
    points = []
    for row in data:
        pts = list(map(float, row))
        points.append(pts)
    points = np.array(points)
    
    m, dim = points.shape
    # Initialize centroids using the first k points.
    centroids = points[:k, :].copy()
    assignments = np.zeros(m, dtype=int)
    
    for _ in range(max_iterations):
        new_assignments = np.zeros(m, dtype=int)
        for i in range(m):
            distances = np.linalg.norm(points[i] - centroids, axis=1)
            new_assignments[i] = np.argmin(distances)
        
        new_centroids = np.zeros_like(centroids)
        counts = np.zeros(k, dtype=int)
        for i in range(m):
            new_centroids[new_assignments[i]] += points[i]
            counts[new_assignments[i]] += 1
        for j in range(k):
            if counts[j] == 0:
                new_centroids[j] = centroids[j]
            else:
                new_centroids[j] = new_centroids[j] / counts[j]
        
        movement = np.linalg.norm(centroids - new_centroids, axis=1).sum()
        centroids = new_centroids
        assignments = new_assignments
        if movement < tol:
            break
    return assignments

if __name__ == "__main__":
    filename = sys.argv[1] if len(sys.argv) > 1 else "iris_data_without_labels.csv"
    k = 3  # adjust k as needed
    assignments = kmeans_clustering(filename, k=k)
    # Output the cluster assignments as whitespace-separated integers.
    print(" ".join(map(str, assignments)))
)";
        pyFile.close();

        // Make the temporary Python script executable.
        system("chmod +x temp_kmeans.py");

        // --------------------------------------------------------------------
        // 4. Run the Python script and capture its output.
        // --------------------------------------------------------------------
        string pyCommand = "./temp_kmeans.py iris_data_without_labels.csv";
        string pyOutput = exec(pyCommand.c_str());
        vector<int> pyAssignments = parsePythonOutput(pyOutput);

        // --------------------------------------------------------------------
        // 5. Compare the results from C++ and Python.
        // --------------------------------------------------------------------
        bool passed = (cppAssignments.size() == pyAssignments.size());
        if (passed) {   
            for (size_t i = 0; i < cppAssignments.size(); i++) {
                if (cppAssignments[i] != pyAssignments[i]) {
                    passed = false;
                    break;
                }
            }
        }

        // --------------------------------------------------------------------
        // 6. Report the test result.
        // --------------------------------------------------------------------
        if (passed) {
            cout << "Test passed: C++ and Python k-Means assignments are equal." << endl;
        } else {
            cout << "Test FAILED: C++ and Python k-Means assignments differ." << endl;
            cout << "C++ assignments: ";
            for (int a : cppAssignments)
            cout << a << " ";
            cout << "\nPython assignments: ";
            for (int a : pyAssignments)
            cout << a << " ";
            cout << endl;
        }

        // --------------------------------------------------------------------
        // 7. Optionally, plot the clusters using matplotlibcpp.
        // --------------------------------------------------------------------
        kmeans.plotKMeansClusters(data, cppAssignments, 3);
        
        // Optionally, delete the temporary Python file.
        remove("temp_kmeans.py");
        // Optionally, plot the clusters using matplotlibcpp.
    } catch (const exception &e) {
        cerr << "Test failed: " << e.what() << endl;
        return 1;
    }
    return 0;
}
