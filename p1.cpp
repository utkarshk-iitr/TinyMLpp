#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cctype>
#include <cfloat>
#include <cstdio>
#include <cmath>
#include <vector>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <map>
using namespace std;

namespace ml {

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

    // Linear Regression using Gradient Descent or Closed-form solution.
    // When only one input column is present, the function uses the closed-form solution:
    //   theta1 = sum((x - mean(x))*(y - mean(y))) / sum((x - mean(x))^2)
    //   theta0 = mean(y) - theta1 * mean(x)
    // Otherwise, it performs Gradient Descent.
    vector<double> linearRegressionGD(const Data &data, double learningRate = 0.01, int iterations = 1000) {
        size_t m = data.features.size();
        if (m == 0) {
            throw runtime_error("No data available");
        }
        size_t n = data.features[0].size();
        // cout<<n<<endl;
        if (n == 1) {
            // Use the closed-form solution for simple linear regression.
            double sumX = 0.0, sumY = 0.0;
            for (size_t i = 0; i < m; i++) {
                double x = toDouble(data.features[i][0]);
                double y = toDouble(data.target[i]);
                sumX += x;
                sumY += y;
            }
            double meanX = sumX / m;
            double meanY = sumY / m;
            double numerator = 0.0, denominator = 0.0;
            for (size_t i = 0; i < m; i++) {
                double x = toDouble(data.features[i][0]);
                double y = toDouble(data.target[i]);
                numerator += (x - meanX) * (y - meanY);
                denominator += (x - meanX) * (x - meanX);
            }
            double theta1 = (denominator == 0 ? 0 : numerator / denominator);
            double theta0 = meanY - theta1 * meanX;
            vector<double> theta = {theta0, theta1};
            cout << "Closed-form Linear Regression Solution:" << endl;
            return theta;
        } else {
            // Use Gradient Descent for multiple features.
            vector<double> theta(n + 1, 0.0); // Initialize theta vector (theta[0] is intercept)
            for (int iter = 0; iter < iterations; iter++) {
                vector<double> gradient(n + 1, 0.0);
                // Compute gradient over all examples.
                for (size_t i = 0; i < m; i++) {
                    double prediction = theta[0];  // intercept
                    for (size_t j = 0; j < n; j++) {
                        double xj = toDouble(data.features[i][j]);
                        prediction += theta[j + 1] * xj;
                    }
                    double y = toDouble(data.target[i]);
                    double error = prediction - y;
                    gradient[0] += error;
                    for (size_t j = 0; j < n; j++) {
                        double xj = toDouble(data.features[i][j]);
                        gradient[j + 1] += error * xj;
                    }
                }
                // Update theta using the average gradient.
                for (size_t j = 0; j < theta.size(); j++) {
                    theta[j] -= learningRate * (gradient[j] / double(m));
                }
                // Optional: Print cost every 100 iterations.
                if (iter % 100 == 0) {
                    double cost = computeMeanSquaredError(data, theta);
                    cout << "Linear Regression Iteration " << iter << ", Cost: " << cost << endl;
                }
            }
            return theta;
        }
    }

    // Logistic Regression using Gradient Descent.
    // Returns a vector of parameters (theta), where theta[0] is the intercept.
    // Expects target values to be 0 or 1.
    vector<double> logisticRegressionGD(const Data &data, double learningRate = 0.01, int iterations = 1000) {
        size_t m = data.features.size();
        if (m == 0) {
            throw runtime_error("No data available");
        }
        size_t n = data.features[0].size();
        vector<double> theta(n + 1, 0.0); // Initialize theta

        for (int iter = 0; iter < iterations; iter++) {
            vector<double> gradient(n + 1, 0.0);
            // Compute gradient over all examples.
            for (size_t i = 0; i < m; i++) {
                double z = theta[0];  // intercept
                for (size_t j = 0; j < n; j++) {
                    double xj = toDouble(data.features[i][j]);
                    z += theta[j + 1] * xj;
                }
                double h = 1.0 / (1.0 + exp(-z));  // sigmoid output
                double y = toDouble(data.target[i]);
                double error = h - y;
                gradient[0] += error;
                for (size_t j = 0; j < n; j++) {
                    double xj = toDouble(data.features[i][j]);
                    gradient[j + 1] += error * xj;
                }
            }
            // Update theta using the average gradient.
            for (size_t j = 0; j < theta.size(); j++) {
                theta[j] -= learningRate * (gradient[j] / double(m));
            }
            // Optional: Print log loss every 100 iterations.
            if (iter % 100 == 0) {
                double loss = computeLogLoss(data, theta);
                cout << "Logistic Regression Iteration " << iter << ", Log Loss: " << loss << endl;
            }
        }
        return theta;
    }


    // K-Nearest Neighbour classification.
    // 'data' is the training data.
    // 'query' is a vector of feature values for the query example.
    // 'k' is the number of closest neighbours to consider.
    // Returns the predicted label as a string.
    string knnPredict(const Data &data, const vector<double> &query, int k) {
        size_t m = data.features.size();
        if (m == 0) {
            throw runtime_error("No training data available.");
        }
        size_t n = data.features[0].size();
        if (query.size() != n) {
            throw runtime_error("Query feature size does not match training data.");
        }
        
        // Compute distances from query to each training example.
        vector<pair<double, string>> distances;
        for (size_t i = 0; i < m; i++) {
            double distance = 0.0;
            for (size_t j = 0; j < n; j++) {
                double value = toDouble(data.features[i][j]);
                double diff = value - query[j];
                distance += diff * diff;
            }
            distances.push_back(make_pair(distance, data.target[i]));
        }

        // Sort training examples by increasing distance.
        sort(distances.begin(), distances.end(), [](const pair<double, string> &a, const pair<double, string> &b) {
            return a.first < b.first;
        });

        // Vote among the k closest neighbours.
        map<string, int> freq;
        for (int i = 0; i < k && i < distances.size(); i++) {
            freq[distances[i].second]++;
        }

        // Find the label with the highest frequency
        string bestLabel;
        int maxCount = -1;
        for (const auto &entry : freq) {
            if (entry.second > maxCount) {
                bestLabel = entry.first;
                maxCount = entry.second;
            }
        }

        return bestLabel;
    }
    // k-Means Clustering Implementation
    // Returns a vector of cluster assignments (one integer per data point).
    // Each integer is in the range [0, k-1] indicating the assigned cluster.
    // Parameters:
    //   data          - the dataset (features should be numeric)
    //   k             - number of clusters
    //   maxIterations - maximum number of iterations (default: 100)
    //   tol           - tolerance for centroid movement (default: 1e-4)
    vector<int> kMeansClustering(const Data &data, int k, int maxIterations = 100, double tol = 1e-4) {
        size_t m = data.features.size();  // number of data points
        if (m == 0) {
            throw runtime_error("No data available for clustering.");
        }
        
        // Adjust the dimension: include the target column as an extra feature.
        size_t originalDim = data.features[0].size();
        size_t dim = originalDim + 1;  // combine features and target

        // Convert features from string to double and include the target column.
        vector<vector<double>> points(m, vector<double>(dim, 0.0));
        for (size_t i = 0; i < m; i++) {
            if (data.features[i].size() != originalDim) {
                throw runtime_error("Inconsistent feature dimensions in data.");
            }
            // Convert the feature columns.
            for (size_t j = 0; j < originalDim; j++) {
                points[i][j] = toDouble(data.features[i][j]);
            }
            // Append the target column (treated as a feature) at the end.
            points[i][dim - 1] = toDouble(data.target[i]);
        }
        
        // Check that k is not greater than the number of points.
        if (k > m) {
            throw runtime_error("k cannot be greater than the number of data points.");
        }
        
        // Initialize centroids.
        // For simplicity, we choose the first k points as initial centroids.
        vector<vector<double>> centroids(points.begin(), points.begin() + k);
        
        // Container to store the cluster index for each data point.
        vector<int> assignments(m, -1);
        
        // Lambda for computing Euclidean distance between two points.
        auto euclideanDistance = [&] (const vector<double>& a, const vector<double>& b) -> double {
            double sum = 0.0;
            for (size_t i = 0; i < a.size(); i++) {
                double diff = a[i] - b[i];
                sum += diff * diff;
            }
            return sqrt(sum);
        };

        for (int iter = 0; iter < maxIterations; iter++) {
            bool assignmentChanged = false;
            
            // Step 1: Assign each point to the nearest centroid.
            for (size_t i = 0; i < m; i++) {
                double minDist = DBL_MAX;
                int bestCluster = -1;
                for (int cluster = 0; cluster < k; cluster++) {
                    double dist = euclideanDistance(points[i], centroids[cluster]);
                    if (dist < minDist) {
                        minDist = dist;
                        bestCluster = cluster;
                    }
                }
                if (assignments[i] != bestCluster) {
                    assignments[i] = bestCluster;
                    assignmentChanged = true;
                }
            }
            
            // Step 2: Update centroids based on current assignments.
            vector<vector<double>> newCentroids(k, vector<double>(dim, 0.0));
            vector<int> counts(k, 0);
            for (size_t i = 0; i < m; i++) {
                int cluster = assignments[i];
                counts[cluster]++;
                for (size_t j = 0; j < dim; j++) {
                    newCentroids[cluster][j] += points[i][j];
                }
            }
            // Compute the mean for each centroid.
            for (int cluster = 0; cluster < k; cluster++) {
                if (counts[cluster] == 0) {
                    // No points assigned to this centroid; keep the old value.
                    newCentroids[cluster] = centroids[cluster];
                } else {
                    for (size_t j = 0; j < dim; j++) {
                        newCentroids[cluster][j] /= counts[cluster];
                    }
                }
            }
            
            // Step 3: Check for convergence (if centroids have moved less than tol).
            double totalMovement = 0.0;
            for (int cluster = 0; cluster < k; cluster++) {
                totalMovement += euclideanDistance(centroids[cluster], newCentroids[cluster]);
            }
            
            centroids = newCentroids;  // Update centroids for the next iteration.
            
            // Optional: Print status every 10 iterations.
            if (iter % 10 == 0) {
                cout << "Iteration " << iter << ", total centroid movement: " << totalMovement << endl;
            }
            
            if (!assignmentChanged || totalMovement < tol) {
                cout << "Convergence reached at iteration " << iter << endl;
                break;
            }
        }  
        return assignments;
    }

} // end namespace ml

// Sample usage.
int main() {
    try {
        // Assume "placement.csv" is formatted so that the first row is a header,
        // and each subsequent row is: feature1, feature2, ..., featureN, target.
        ml::Data data = ml::readCSV("exported_data.csv");

        // Display the dataset in a dataframe-like format with row numbers.
        // ml::displayDataFrame(data);

        // Choose one of the normalization methods
        // standardize(data);      // For Gradient Descent, this is preferred
        // minMaxNormalize(data); // Optionally, you can use Min-Max scaling

        // Uncomment one of the following blocks to run the desired regression.

        //* Linear Regression using either the closed-form solution (if one feature) or Gradient Descent.
        
        // vector<double> linTheta = ml::linearRegressionGD(data, 0.01, 10000);
        // cout << "\nLinear Regression Parameters:" << endl;
        // for (size_t i = 0; i < linTheta.size(); i++) {
        //     cout << "theta[" << i << "] = " << linTheta[i] << endl;
        // }
        // double mse = ml::computeMeanSquaredError(data, linTheta);
        // cout << "Mean Squared Error: " << mse << endl;
        

        //* Logistic Regression using Gradient Descent
        
        // vector<double> logTheta = ml::logisticRegressionGD(data, 0.01, 2000);
        // cout << "\nLogistic Regression (Gradient Descent) Parameters:" << endl;
        // for (size_t i = 0; i < logTheta.size(); i++) {
        //     cout << "theta[" << i << "] = " << logTheta[i] << endl;
        // }
        // double logLoss = ml::computeLogLoss(data, logTheta);
        // cout << "Log Loss Error: " << logLoss << endl;


        //* K-Nearest Neighbour 

        vector<double> query = {6.2}; // Example query features
        int k = 3; // Number of nearest neighbours to consider
        string predictedLabel = ml::knnPredict(data, query, k);
        cout << "\nK-Nearest Neighbour Prediction:" << endl;
        cout << "Query: ";
        for (double val : query) {
            cout << val << " ";
        }
        cout << "\nPredicted Label: " << predictedLabel << endl;
        // ml::plotPlacement("placement.csv");

        // Example usage for k-means clustering:
        int k = 3;  // number of clusters (adjust as needed)
        vector<int> clusterAssignments = ml::kMeansClustering(data, k);
        cout << "\nK-Means Clustering Assignments:" << endl;

        // For each cluster, print all points (their full specifications) that belong to it.
        // Note: Each point's specification now includes its original features and the target value.
        for (int clusterID = 0; clusterID < k; clusterID++) {
            cout << "\nCluster " << clusterID << ":" << endl;
            for (size_t i = 0; i < clusterAssignments.size(); i++) {
                if (clusterAssignments[i] == clusterID) {
                    cout << "Data point " << i + 1 << ": ";
                    // Print each feature from data.features.
                    for (const auto &feature : data.features[i]) {
                        cout << feature << ", ";
                    }
                    
                    // Also print the target value (which is now treated as a feature).
                    cout << data.target[i];
                    cout << endl;
                }
            }
        }

    }
    catch (const exception &ex) {
        cerr << "Error: " << ex.what() << endl;
    }
    return 0;
}
