#ifndef LOGISTIC_REGRESSION_H
#define LOGISTIC_REGRESSION_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>
#include "base.h"           // Assumes Model is defined in model.h
#include "data_handling.h"
#include "gnuplot-iostream.h" // For plotting
#include <utility>         // For std::pair

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
        double* params = new double[theta.size()];
        for (size_t i = 0; i < theta.size(); ++i) {
            params[i] = theta[i];
        }
        return static_cast<void*>(params); // Return the parameters as a void pointer.
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

    void plotLR(Data& data,vector<double>& theta) {
        Gnuplot gp;

        vector<pair<double, double>> class0;
        vector<pair<double, double>> class1;
        vector<double> y_vals,x_vals;

        // Separate data points based on class label
        for (size_t i = 0; i < data.features.size(); ++i) {
            double x1 = toDouble(data.features[i][0]);
            double x2 = toDouble(data.features[i][1]);
            double label = toDouble(data.target[i]);
            if (label == 0)
                class0.emplace_back(x1, x2);
            else
                class1.emplace_back(x1, x2);
            y_vals.push_back(x2);
            x_vals.push_back(x1);
        }

        // Prepare decision boundary
        // x2 = -(theta0 + theta1*x1) / theta2
        vector<pair<double, double>> boundary;
        double x_min = *min_element(x_vals.begin(), x_vals.end()); // or use actual min/max from data
        double x_max = *max_element(x_vals.begin(), x_vals.end());
        x_max *= 1.3; 
        x_min *= 1.3;

        for (double x1 = x_min; x1 <= x_max; x1 += 0.1) {
            double x2 = -(theta[0] + theta[1]*x1) / theta[2];
            boundary.emplace_back(x1, x2);
        }

        double y_min = *min_element(y_vals.begin(), y_vals.end());
        double y_max = *max_element(y_vals.begin(), y_vals.end());
        y_max *= 1.3; 
        y_min *= 1.3;
        

        gp << "set title 'Logistic Regression'\n";
        gp << "set xlabel 'x1'\n";
        gp << "set ylabel 'x2'\n";
        gp << "set grid\n";
        gp << "set key top right opaque box font ',8'\n";
        gp << "set yrange [" << y_min << ":" << y_max << "]\n";
        gp << "set xrange [" << x_min << ":" << x_max << "]\n";
        gp << "plot '-' with points pointtype 7 lc rgb 'red' title 'Class 0', "
            "'-' with points pointtype 7 lc rgb 'blue' title 'Class 1', "
            "'-' with lines lt rgb 'black' lw 2 title 'Decision Boundary'\n";

        gp.send1d(class0);
        gp.send1d(class1);
        gp.send1d(boundary);
    }   

};

#endif // LOGISTIC_REGRESSION_H
