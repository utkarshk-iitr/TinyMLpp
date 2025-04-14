#ifndef LOGISTIC_REGRESSION_H
#define LOGISTIC_REGRESSION_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>
#include "base.h"           // Assumes Model is defined in model.h
#include "data_handling.h" // Assumes Data, toDouble(), and computeLogLoss() are defined here

using namespace std;
using namespace handle;

class LogisticRegression : public Model {
private:
    vector<double> theta; // Model parameters: theta[0] is the intercept; theta[1..n] are the feature weights.

    // Sigmoid function.
    double sigmoid(double z) {
        return 1.0 / (1.0 + exp(-z));
    }

public:
    // Constructor with optional parameters for learning rate and number of iterations.
    LogisticRegression(double lr = 0.01, int ep = 1000) : Model(lr, ep) {}

    // Train the logistic regression model using gradient descent.
    void* train(Data &data) override {
        size_t m = data.features.size();
        if (m == 0) {
            throw runtime_error("No data available");
        }
        size_t n = data.features[0].size();
        theta.assign(n + 1, 0.0); // Initialize theta (theta[0] is the intercept)

        for (int iter = 0; iter < epochs; iter++) {
            vector<double> gradient(n + 1, 0.0);

            // Compute gradient over all examples.
            for (size_t i = 0; i < m; i++) {
                double z = theta[0];  // Intercept contribution.
                for (size_t j = 0; j < n; j++) {
                    double xj = toDouble(data.features[i][j]);
                    z += theta[j + 1] * xj;
                }
                double h = sigmoid(z);  // Predicted probability.
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
                theta[j] -= learningRate * (gradient[j] / static_cast<double>(m));
            }
            // Optionally, print the log loss every 100 iterations.
            if (iter % 100 == 0) {
                double loss = computeLogLoss(data, theta);
                cout << "Logistic Regression Iteration " << iter << ", Log Loss: " << loss << endl;
            }
        }
    }

    // Predict outcomes using the trained logistic regression model.
    // Returns the predicted probability for each example.
    vector<double> predict(Data &data) override {
        size_t m = data.features.size();
        size_t n = data.features[0].size();
        vector<double> predictions(m, 0.0);

        for (size_t i = 0; i < m; i++) {
            double z = theta[0]; // Start with the intercept.
            for (size_t j = 0; j < n; j++) {
                double xj = toDouble(data.features[i][j]);
                z += theta[j + 1] * xj;
            }
            // Apply the sigmoid function to get the probability.
            predictions[i] = sigmoid(z);
        }
        return predictions;
    }
};

#endif // LOGISTIC_REGRESSION_H
