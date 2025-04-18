#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cfloat>
#include <cmath>
#include "data_handling.h"    // for readCSV, toDouble, computeMeanSquaredError, train_test_split, etc.

using namespace std;
using namespace handle;       // brings Data, train_test_split, computeMeanSquaredError, etc. into scope

/**
 * Train/Test split defaults:
 *   test_size = 0.2  (20% of data reserved for test)
 *   seed      = 42   (fixed for reproducibility)
 */
static constexpr double DEFAULT_TEST_SIZE = 0.2;
static constexpr unsigned DEFAULT_SEED   = 42;

// LinearRegression class definition (methods)
class LinearRegression {
public:
    double learningRate;
    int epochs;
private:
    vector<double> theta;

public:
    LinearRegression(double lr = 0.01, int ep = 1000)
      : learningRate(lr), epochs(ep) {}

    // Splits data, trains on trainData, reports on both splits
    void* train(Data &data) {
        // 1. Split into train/test
        auto [trainData, testData] = train_test_split(data, DEFAULT_TEST_SIZE, DEFAULT_SEED);

        cout << "Training on " << trainData.features.size()
             << " samples; testing on " << testData.features.size() << " samples.\n";

        // Use trainData for fitting
        size_t m = trainData.features.size();
        if (m == 0) throw runtime_error("No training data available after split");
        size_t n = trainData.features[0].size();

        // Choose closed-form if single feature
        if (n == 1) {
            double sumX = 0, sumY = 0;
            for (auto &row : trainData.features) {
                sumX += toDouble(row[0]);
            }
            for (auto &t : trainData.target) {
                sumY += toDouble(t);
            }
            double meanX = sumX / m, meanY = sumY / m;

            double num = 0, den = 0;
            for (size_t i = 0; i < m; i++) {
                double x = toDouble(trainData.features[i][0]);
                double y = toDouble(trainData.target[i]);
                num += (x - meanX)*(y - meanY);
                den += (x - meanX)*(x - meanX);
            }
            double slope = den==0 ? 0 : num/den;
            double intercept = meanY - slope*meanX;
            theta = {intercept, slope};

            cout << "Used closed-form solution\n";
        }
        else {
            // Gradient descent
            theta.assign(n+1, 0.0);
            for (int iter = 0; iter < epochs; ++iter) {
                vector<double> grad(n+1, 0.0);
                for (size_t i = 0; i < m; ++i) {
                    double pred = theta[0];
                    for (size_t j = 0; j < n; ++j) {
                        pred += theta[j+1] * toDouble(trainData.features[i][j]);
                    }
                    double err = pred - toDouble(trainData.target[i]);
                    grad[0] += err;
                    for (size_t j = 0; j < n; ++j) {
                        grad[j+1] += err * toDouble(trainData.features[i][j]);
                    }
                }
                for (size_t j = 0; j < theta.size(); ++j) {
                    theta[j] -= learningRate * (grad[j] / m);
                }
                if (iter % 100 == 0) {
                    double cost = computeMeanSquaredError(trainData, theta);
                    cout << " Iter " << iter << " Train MSE: " << cost << "\n";
                }
            }
        }

        // 2. Report final train & test MSE
        double trainMSE = computeMeanSquaredError(trainData, theta);
        double testMSE  = computeMeanSquaredError(testData, theta);
        cout << fixed << setprecision(6)
             << "Final Train MSE: " << trainMSE
             << " | Test MSE: " << testMSE << "\n";

        // 3. Return copy of theta
        double *params = new double[theta.size()];
        for (size_t i = 0; i < theta.size(); ++i) {
            params[i] = theta[i];
        }
        return static_cast<void*>(params);
    }

    vector<double> predict(Data &data) {
        size_t m = data.features.size();
        size_t n = data.features[0].size();
        vector<double> out(m);
        for (size_t i = 0; i < m; ++i) {
            double yhat = theta[0];
            for (size_t j = 0; j < n; ++j) {
                yhat += theta[j+1] * toDouble(data.features[i][j]);
            }
            out[i] = yhat;
        }
        return out;
    }
};
