
#pragma once
#ifndef DATA_HANDLING_H
#define DATA_HANDLING_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cfloat>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <random>

using namespace std;

namespace handle
{
    
/**
 * @brief Container for data: headers, features, and target vector.
 *
 * - header: Column names (last column is the target name)
 * - features: All feature values as strings
 * - target: Target values as strings
 */
class Data {
public:
    vector<string> header;             // Column names (last column is the target name)
    vector<vector<string>> features;   // All feature values as strings
    vector<string> target;             // Target values as strings
};

/**
 * @brief Trims whitespace from both ends of a string.
 * 
 * @param s The input string.
 * @return A trimmed version of the string.
 */
string trim(string &s);

/**
 * @brief Converts a string to a double.
 * 
 * @param s The input string.
 * @return The converted double value.
 * @throws runtime_error if the conversion fails.
 */
double toDouble(string &s);

/**
 * @brief Reads data from a CSV file.
 * 
 * Assumes that the first row contains headers, and each subsequent row contains:
 * feature1, feature2, ..., featureN, target.
 * 
 * @param filename The name of the CSV file.
 * @return A Data object containing the headers, features, and target values.
 */
Data readCSV(string &filename);

/**
 * @brief Computes the maximum width for each column (features + target).
 * 
 * @param data The Data object.
 * @return A vector of size_t representing the maximum width for each column.
 */
vector<size_t> computeColumnWidths(Data &data);

/**
 * @brief Displays the dataset like a DataFrame with row numbers.
 * 
 * @param data The Data object to display.
 */
void displayDataFrame(Data &data);

/**
 * @brief Applies Min-Max Normalization to the data.
 * 
 * For each feature, normalizes the values using:
 * normalizedValue = (value - min) / (max - min + epsilon)
 * 
 * @param data The Data object whose features will be normalized.
 */
void minMaxNormalize(Data &data);

/**
 * @brief Applies Z-Score Normalization (Standardization) to the data.
 * 
 * For each feature, standardizes the values using the mean and standard deviation.
 * 
 * @param data The Data object whose features will be standardized.
 */
void standardize(Data &data);

/**
 * @brief Computes the Mean Squared Error (cost) for Linear Regression.
 * 
 * @param data The Data object containing features and target values.
 * @param theta The model parameters (theta[0] is the intercept; theta[1..n] are feature weights).
 * @return The Mean Squared Error.
 */


 pair<Data, Data> train_test_split(Data& dataset, double test_size = 0.2, unsigned seed = random_device{}());


double computeMeanSquaredError(Data &data, vector<double> &theta);

/**
 * @brief Computes the Log Loss (cost) for Logistic Regression.
 * 
 * @param data The Data object containing features and target values.
 * @param theta The model parameters (theta[0] is the intercept; theta[1..n] are feature weights).
 * @return The computed Log Loss value.
 */
double computeLogLoss(Data &data, vector<double> &theta);

double computeAccuracy(vector<double> &true_labels, vector<double> &predicted_labels);

// Compute Precision
double computePrecision(const vector<double> &true_labels, const vector<double> &predicted_labels);

// Compute Recall
double computeRecall(const vector<double> &true_labels, const vector<double> &predicted_labels);

// Compute F1 Score
double computeF1Score(const vector<double> &true_labels, const vector<double> &predicted_labels);


#endif // DATA_HANDLING_H
}


