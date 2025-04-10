#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cfloat> 
#include <cmath>

using namespace std;

// Container for data: headers, features and target vector.
class Data {
    public:
        vector<string> header;             // Column names (last column is the target name)
        vector<vector<string>> features;   // All feature values as strings
        vector<string> target;             // Target values as strings
    };

    // Helper function to trim whitespace from both ends of a string.
    string trim(const string &s) {
        size_t start = s.find_first_not_of(" \t\n\r");
        size_t end = s.find_last_not_of(" \t\n\r");
        return (start == string::npos) ? "" : s.substr(start, end - start + 1);
    }

    // Helper function to convert string to double.
    double toDouble(const string &s) {
        try {
            return stod(s);
        } catch (const invalid_argument &) {
            throw runtime_error("Conversion error: '" + s + "' is not a valid numeric value.");
        }
    }

    // Read data from a CSV file.
    // Assumes that the first row contains headers, and each subsequent row contains:
    // feature1, feature2, ..., featureN, target
    Data readCSV(const string &filename) {
        Data data;  
        ifstream file(filename);
        if (!file) {
            throw runtime_error("Cannot open file " + filename);
        }
        string line;
        bool isHeader = true;
        while (getline(file, line)) {
            istringstream stream(line);
            string cell;
            vector<string> row;
            while (getline(stream, cell, ',')) {
                row.push_back(trim(cell));
            }
            if (isHeader) {
                data.header = row;  // Store the header row
                isHeader = false;
            } else if (!row.empty()) {
                // Assume last column is the target.
                data.target.push_back(row.back());
                row.pop_back();
                data.features.push_back(row);
            }
        }
        return data;
    }

    // Function to compute maximum width for each column (features + target).
    vector<size_t> computeColumnWidths(const Data &data) {
        size_t numCols = data.header.size();
        vector<size_t> colWidths(numCols, 0);

        // Update widths based on header.
        for (size_t i = 0; i < numCols; i++) {
            colWidths[i] = data.header[i].length();
        }
        // Update widths for each row.
        for (size_t i = 0; i < data.features.size(); i++) {
            const auto &row = data.features[i];
            for (size_t j = 0; j < row.size(); j++) {
                colWidths[j] = max(colWidths[j], row[j].length());
            }
            // Update for target (last column)
            colWidths[numCols - 1] = max(colWidths[numCols - 1], data.target[i].length());
        }
        return colWidths;
    }

    // Function to display the dataset like a DataFrame with row numbers.
    void displayDataFrame(const Data &data) {
        vector<size_t> colWidths = computeColumnWidths(data);
        size_t numCols = data.header.size();

        // Compute width for the row number column (header "Row").
        size_t rowNumberWidth = string("Row").length();
        size_t numRows = data.features.size();
        for (size_t i = 0; i < numRows; i++) {
            rowNumberWidth = max(rowNumberWidth, to_string(i + 1).length());
        }

        // Print header with row number header.
        cout << setw(rowNumberWidth + 2) << left << "Row";
        for (size_t i = 0; i < numCols; i++) {
            cout << setw(colWidths[i] + 2) << left << data.header[i];
        }
        cout << endl;

        // Print a separator line.
        cout << string(rowNumberWidth + 2, '-');
        for (size_t i = 0; i < numCols; i++) {
            cout << string(colWidths[i] + 2, '-');
        }
        cout << endl;

        // Print each row with its row number.
        for (size_t i = 0; i < data.features.size(); i++) {
            cout << setw(rowNumberWidth + 2) << left << to_string(i + 1);
            const auto &row = data.features[i];
            for (size_t j = 0; j < row.size(); j++) {
                cout << setw(colWidths[j] + 2) << left << row[j];
            }
            // Print target in the last column.
            cout << setw(colWidths[numCols - 1] + 2) << left << data.target[i] << endl;
        }
    }
    // Min-Max Normalization
    void minMaxNormalize(Data &data) {
        size_t m = data.features.size();
        size_t n = data.features[0].size();

        vector<double> minVal(n, DBL_MAX);
        vector<double> maxVal(n, -DBL_MAX);

        // Find min and max for each feature
        for (size_t j = 0; j < n; j++) {
            for (size_t i = 0; i < m; i++) {
                double value = toDouble(data.features[i][j]);
                minVal[j] = min(minVal[j], value);
                maxVal[j] = max(maxVal[j], value);
            }
        }

        // Normalize data
        for (size_t j = 0; j < n; j++) {
            for (size_t i = 0; i < m; i++) {
                double value = toDouble(data.features[i][j]);
                double normalizedValue = (value - minVal[j]) / (maxVal[j] - minVal[j] + 1e-8);
                data.features[i][j] = to_string(normalizedValue);
            }
        }
    }
    // Z-Score Normalization (Standardization)
    void standardize(Data &data) {
        size_t m = data.features.size();
        size_t n = data.features[0].size();

        vector<double> mean(n, 0.0);
        vector<double> stddev(n, 0.0);

        // Compute mean
        for (size_t j = 0; j < n; j++) {
            for (size_t i = 0; i < m; i++) {
                mean[j] += toDouble(data.features[i][j]);
            }
            mean[j] /= m;
        }

        // Compute standard deviation
        for (size_t j = 0; j < n; j++) {
            for (size_t i = 0; i < m; i++) {
                double value = toDouble(data.features[i][j]);
                stddev[j] += pow(value - mean[j], 2);
            }
            stddev[j] = sqrt(stddev[j] / m);
        }

        // Normalize data
        for (size_t j = 0; j < n; j++) {
            for (size_t i = 0; i < m; i++) {
                double value = toDouble(data.features[i][j]);
                double normalizedValue = (value - mean[j]) / (stddev[j] + 1e-8);
                data.features[i][j] = to_string(normalizedValue);
            }
        }
    }

    // Compute Mean Squared Error (Linear Regression cost).
    double computeMeanSquaredError(const Data &data, const vector<double> &theta) {
        size_t m = data.features.size();
        size_t n = data.features[0].size();
        double mse = 0.0;
        for (size_t i = 0; i < m; i++) {
            double prediction = theta[0];  // intercept
            for (size_t j = 0; j < n; j++) {
                double xj = toDouble(data.features[i][j]);
                prediction += theta[j + 1] * xj;
            }
            double y = toDouble(data.target[i]);
            double error = prediction - y;
            mse += error * error;
        }
        mse /= (2.0 * m);
        return mse;
    }

    // Compute Log Loss (Logistic Regression cost).
    double computeLogLoss(const Data &data, const vector<double> &theta) {
        size_t m = data.features.size();
        size_t n = data.features[0].size();
        double loss = 0.0;
        const double eps = 1e-15; // to avoid log(0)
        for (size_t i = 0; i < m; i++) {
            double z = theta[0];  // intercept
            for (size_t j = 0; j < n; j++) {
                double xj = toDouble(data.features[i][j]);
                z += theta[j + 1] * xj;
            }
            double h = 1.0 / (1.0 + exp(-z));
            double y = toDouble(data.target[i]);
            loss += -(y * log(h + eps) + (1 - y) * log(1 - h + eps));
        }
        loss /= m;
        return loss;
    }

