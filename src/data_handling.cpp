#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cfloat>
#include <cmath>
#include "data_handling.h"

using namespace std;
namespace handle
{

    // Helper function to trim whitespace from both ends of a string.
    string trim(string &s)
    {
        size_t start = s.find_first_not_of(" \t\n\r");
        size_t end = s.find_last_not_of(" \t\n\r");
        return (start == string::npos) ? "" : s.substr(start, end - start + 1);
    }

    // Helper function to convert string to double.
    double toDouble(string &s)
    {
        try
        {
            return stod(s);
        }
        catch (invalid_argument &)
        {
            throw runtime_error("Conversion error: '" + s + "' is not a valid numeric value.");
        }
    }

    // Read data from a CSV file.
    // Assumes that the first row contains headers, and each subsequent row contains:
    // feature1, feature2, ..., featureN, target
    Data readCSV(string &filename)
    {
        Data data;
        ifstream file(filename);
        if (!file)
        {
            throw runtime_error("Cannot open file " + filename);
        }
        string line;
        bool isHeader = true;
        while (getline(file, line))
        {
            istringstream stream(line);
            string cell;
            vector<string> row;
            while (getline(stream, cell, ','))
            {
                row.push_back(trim(cell));
            }
            if (isHeader)
            {
                data.header = row; // Store the header row
                isHeader = false;
            }
            else if (!row.empty())
            {
                // Assume last column is the target.
                data.target.push_back(row.back());
                row.pop_back();
                data.features.push_back(row);
            }
        }
        return data;
    }

    // Function to compute maximum width for each column (features + target).
    vector<size_t> computeColumnWidths(Data &data)
    {
        size_t numCols = data.header.size();
        vector<size_t> colWidths(numCols, 0);

        // Update widths based on header.
        for (size_t i = 0; i < numCols; i++)
        {
            colWidths[i] = data.header[i].length();
        }
        // Update widths for each row.
        for (size_t i = 0; i < data.features.size(); i++)
        {
            auto &row = data.features[i];
            for (size_t j = 0; j < row.size(); j++)
            {
                colWidths[j] = max(colWidths[j], row[j].length());
            }
            // Update for target (last column)
            colWidths[numCols - 1] = max(colWidths[numCols - 1], data.target[i].length());
        }
        return colWidths;
    }

    // Function to display the dataset like a DataFrame with row numbers.
    void displayDataFrame(Data &data)
    {
        vector<size_t> colWidths = computeColumnWidths(data);
        size_t numCols = data.header.size();

        // Compute width for the row number column (header "Row").
        size_t rowNumberWidth = string("Row").length();
        size_t numRows = data.features.size();
        for (size_t i = 0; i < numRows; i++)
        {
            rowNumberWidth = max(rowNumberWidth, to_string(i + 1).length());
        }

        // Print header with row number header.
        cout << setw(rowNumberWidth + 2) << left << "Row";
        for (size_t i = 0; i < numCols; i++)
        {
            cout << setw(colWidths[i] + 2) << left << data.header[i];
        }
        cout << endl;

        // Print a separator line.
        cout << string(rowNumberWidth + 2, '-');
        for (size_t i = 0; i < numCols; i++)
        {
            cout << string(colWidths[i] + 2, '-');
        }
        cout << endl;

        // Print each row with its row number.
        for (size_t i = 0; i < data.features.size(); i++)
        {
            cout << setw(rowNumberWidth + 2) << left << to_string(i + 1);
            auto &row = data.features[i];
            for (size_t j = 0; j < row.size(); j++)
            {
                cout << setw(colWidths[j] + 2) << left << row[j];
            }
            // Print target in the last column.
            cout << setw(colWidths[numCols - 1] + 2) << left << data.target[i] << endl;
        }
    }
    // Min-Max Normalization
    void minMaxNormalize(Data &data)
    {
        size_t m = data.features.size();
        size_t n = data.features[0].size();

        vector<double> minVal(n, DBL_MAX);
        vector<double> maxVal(n, -DBL_MAX);

        // Find min and max for each feature
        for (size_t j = 0; j < n; j++)
        {
            for (size_t i = 0; i < m; i++)
            {
                double value = toDouble(data.features[i][j]);
                minVal[j] = min(minVal[j], value);
                maxVal[j] = max(maxVal[j], value);
            }
        }

        // Normalize data
        for (size_t j = 0; j < n; j++)
        {
            for (size_t i = 0; i < m; i++)
            {
                double value = toDouble(data.features[i][j]);
                double normalizedValue = (value - minVal[j]) / (maxVal[j] - minVal[j] + 1e-8);
                data.features[i][j] = to_string(normalizedValue);
            }
        }
    }
    // Z-Score Normalization (Standardization)
    void standardize(Data &data)
    {
        size_t m = data.features.size();
        size_t n = data.features[0].size();

        vector<double> mean(n, 0.0);
        vector<double> stddev(n, 0.0);

        // Compute mean
        for (size_t j = 0; j < n; j++)
        {
            for (size_t i = 0; i < m; i++)
            {
                mean[j] += toDouble(data.features[i][j]);
            }
            mean[j] /= m;
        }

        // Compute standard deviation
        for (size_t j = 0; j < n; j++)
        {
            for (size_t i = 0; i < m; i++)
            {
                double value = toDouble(data.features[i][j]);
                stddev[j] += pow(value - mean[j], 2);
            }
            stddev[j] = sqrt(stddev[j] / m);
        }

        // Normalize data
        for (size_t j = 0; j < n; j++)
        {
            for (size_t i = 0; i < m; i++)
            {
                double value = toDouble(data.features[i][j]);
                double normalizedValue = (value - mean[j]) / (stddev[j] + 1e-8);
                data.features[i][j] = to_string(normalizedValue);
            }
        }
    }

    // Function to split the dataset into training and testing sets.
    pair<Data, Data> train_test_split(Data &dataset, double test_size, unsigned seed)
    {
        if (test_size <= 0.0 || test_size >= 1.0)
            throw invalid_argument("test_size must be in (0,1)");

        size_t n_rows = dataset.features.size();
        if (n_rows != dataset.target.size())
            throw runtime_error("features and target row-counts differ");

        // 1) build & shuffle index array
        vector<size_t> idx(n_rows);
        iota(idx.begin(), idx.end(), 0);
        mt19937 rng(seed);
        shuffle(idx.begin(), idx.end(), rng);

        // 2) compute split point
        size_t n_test = static_cast<size_t>(n_rows * test_size);
        size_t n_train = n_rows - n_test;

        // 3) allocate train/test Data structs and copy header
        Data train, test;
        train.header = dataset.header;
        test.header = dataset.header;

        // reserve capacity (optional but avoids reallocations)
        train.features.reserve(n_train);
        train.target.reserve(n_train);
        test.features.reserve(n_test);
        test.target.reserve(n_test);

        // 4) distribute rows according to shuffled indices
        for (size_t i = 0; i < n_train; ++i)
        {
            size_t row = idx[i + n_test]; // put last n_train into train
            train.features.push_back(dataset.features[row]);
            train.target.push_back(dataset.target[row]);
        }
        for (size_t i = 0; i < n_test; ++i)
        {
            size_t row = idx[i]; // first n_test into test
            test.features.push_back(dataset.features[row]);
            test.target.push_back(dataset.target[row]);
        }

        return {train, test};
    }

    // Compute Mean Squared Error (Linear Regression cost).
    double computeMeanSquaredError(Data &data, vector<double> &theta)
    {
        size_t m = data.features.size();
        size_t n = data.features[0].size();
        double mse = 0.0;
        for (size_t i = 0; i < m; i++)
        {
            double prediction = theta[0]; // intercept
            for (size_t j = 0; j < n; j++)
            {
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
    double computeLogLoss(Data &data, vector<double> &theta)
    {
        size_t m = data.features.size();
        size_t n = data.features[0].size();
        double loss = 0.0;
        double eps = 1e-15; // to avoid log(0)
        for (size_t i = 0; i < m; i++)
        {
            double z = theta[0]; // intercept
            for (size_t j = 0; j < n; j++)
            {
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

    double computeAccuracy(vector<double> &true_labels, vector<double> &predicted_labels)
    {
        if (true_labels.size() != predicted_labels.size())
        {
            throw invalid_argument("Size mismatch between true and predicted labels.");
        }

        size_t correct = 0;
        for (size_t i = 0; i < true_labels.size(); ++i)
        {
            if (true_labels[i] == predicted_labels[i])
            {
                ++correct;
            }
        }

        return static_cast<double>(correct) / true_labels.size();
    }

}
