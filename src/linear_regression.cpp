
#ifndef LINEAR_REGRESSION_H
#define LINEAR_REGRESSION_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include "base.h"         
#include "data_handling.h"
#include "../include/matplotlibcpp.h"
using namespace handle;
namespace plt = matplotlibcpp;

class LinearRegression : public Model {
private:
    std::vector<double> theta; // Model parameters: theta[0] is the intercept; theta[1..n] are the weights.
public:
    // Constructor with optional parameters for learning rate and number of epochs.
    LinearRegression(double lr = 0.01, int ep = 1000) : Model(lr, ep) {}

    // Train the model using either the closed-form solution (for one feature)
    // or gradient descent (for multiple features).
    void* train(Data &data) override {
        size_t m = data.features.size();
        if (m == 0) {
            throw std::runtime_error("No data available");
        }
        size_t n = data.features[0].size();

        if (n == 1) {
            // Closed-form solution for simple linear regression.
            double sumX = 0.0, sumY = 0.0;
            for (size_t i = 0; i < m; ++i) {
                double x = toDouble(data.features[i][0]);
                double y = toDouble(data.target[i]);
                sumX += x;
                sumY += y;
            }
            double meanX = sumX / m;
            double meanY = sumY / m;

            double numerator = 0.0, denominator = 0.0;
            for (size_t i = 0; i < m; ++i) {
                double x = toDouble(data.features[i][0]);
                double y = toDouble(data.target[i]);
                numerator += (x - meanX) * (y - meanY);
                denominator += (x - meanX) * (x - meanX);
            }
            double theta1 = (denominator == 0 ? 1e9 : numerator / denominator);
            double theta0 = meanY - theta1 * meanX;
            theta = { theta0, theta1 };

            std::cout << "Closed-form Linear Regression Solution:" << std::endl;
        } else {
            // Use gradient descent for multiple features.
            theta.assign(n + 1, 0.0); // theta[0] is intercept; theta[1..n] are feature weights.
            for (int iter = 0; iter < epochs; ++iter) {
                std::vector<double> gradient(n + 1, 0.0);

                for (size_t i = 0; i < m; ++i) {
                    double prediction = theta[0];
                    for (size_t j = 0; j < n; ++j) {
                        double xj = toDouble(data.features[i][j]);
                        prediction += theta[j + 1] * xj;
                    }
                    double y = toDouble(data.target[i]);
                    double error = prediction - y;

                    gradient[0] += error;
                    for (size_t j = 0; j < n; ++j) {
                        double xj = toDouble(data.features[i][j]);
                        gradient[j + 1] += error * xj;
                    }
                }
                // Update theta using the average gradient.
                for (size_t j = 0; j < theta.size(); ++j) {
                    theta[j] -= learningRate * (gradient[j] / static_cast<double>(m));
                }
                // Optionally, print the cost every 100 iterations.
                if (iter % 100 == 0) {
                    double cost = computeMeanSquaredError(data, theta);
                    std::cout << "Linear Regression Iteration " << iter << ", Cost: " << cost << std::endl;
                }
            }
        }
        double* params = new double[theta.size()];
        for (size_t i = 0; i < theta.size(); ++i) {
            params[i] = theta[i];
        }
        return static_cast<void*>(params); // Return the parameters as a void pointer.
    }

    // Predict outcomes using the trained model.
    std::vector<double> predict(Data &data) override {
        size_t m = data.features.size();
        size_t n = data.features[0].size();
        std::vector<double> predictions(m, 0.0);

        for (size_t i = 0; i < m; ++i) {
            double pred = theta[0]; // Start with the intercept.
            for (size_t j = 0; j < n; ++j) {
                pred += theta[j + 1] * toDouble(data.features[i][j]);
            }
            predictions[i] = pred;
        }
        return predictions;
    }

    void plotLinearRegression(Data &data, vector<double>& theta) {
        // For plotting, we have assumed a single feature for now.
        vector<double> x_data, y_data;
        for (size_t i = 0; i < data.features.size(); i++) {
            double x = toDouble(data.features[i][0]);
            double y = toDouble(data.target[i]);
            x_data.push_back(x);
            y_data.push_back(y);
        }
        // Determine the range for x to plot the regression line.
        double x_min = *min_element(x_data.begin(), x_data.end());
        double x_max = *max_element(x_data.begin(), x_data.end());
        int num_points = 100;
        vector<double> x_line(num_points), y_line(num_points);
        double step = (x_max - x_min) / (num_points - 1);
        for (int i = 0; i < num_points; i++) {
            x_line[i] = x_min + i * step;
            // For simple linear regression, predicted y = theta0 + theta1 * x.
            y_line[i] = theta[0] + theta[1] * x_line[i];
        }
        plt::scatter(x_data, y_data, 10.0, {{"color", "blue"}, {"label", "Data Points"}});
        plt::plot(x_line, y_line, {{"color", "red"}, {"label", "Regression Line"}});
        plt::xlabel("Feature 1");
        plt::ylabel("Target");
        plt::title("Linear Regression Fit");
        plt::legend();
        plt::grid(true);
        plt::show();
    }
};

#endif // LINEAR_REGRESSION_H
