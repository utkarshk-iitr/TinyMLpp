#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cfloat>
#include <cmath>
#include "data_handling.h"    // for readCSV, toDouble, computeMeanSquaredError, train_test_split, etc.
#include "base.h"           // for Model
#include <gnuplot-iostream.h>

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
class LinearRegression : public Model {
private:
    vector<double> theta;

public:
    LinearRegression(double lr = 0.01, int ep = 1000)
      : Model(lr, ep) {}

    // Splits data, trains on data, reports on both splits
    void* train(Data &data) {
        // Use data for fitting
        size_t m = data.features.size();
        if (m == 0) throw runtime_error("No training data available after split");
        size_t n = data.features[0].size();

        // Choose closed-form if single feature
        if (n == 1) {
            double sumX = 0, sumY = 0;
            for (auto &row : data.features) {
                sumX += toDouble(row[0]);
            }
            for (auto &t : data.target) {
                sumY += toDouble(t);
            }
            double meanX = sumX / m, meanY = sumY / m;

            double num = 0, den = 0;
            for (size_t i = 0; i < m; i++) {
                double x = toDouble(data.features[i][0]);
                double y = toDouble(data.target[i]);
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
                        pred += theta[j+1] * toDouble(data.features[i][j]);
                    }
                    double err = pred - toDouble(data.target[i]);
                    grad[0] += err;
                    for (size_t j = 0; j < n; ++j) {
                        grad[j+1] += err * toDouble(data.features[i][j]);
                    }
                }
                for (size_t j = 0; j < theta.size(); ++j) {
                    theta[j] -= learningRate * (grad[j] / m);
                }
                if (iter % 100 == 0) {
                    double cost = computeMeanSquaredError(data, theta);
                    cout << " Iter " << iter << " Train MSE: " << cost << "\n";
                }
            }
        }

        // 2. Report final train & test MSE
        // double trainMSE = computeMeanSquaredError(data, theta);
        // double testMSE  = computeMeanSquaredError(testData, theta);
        // cout << fixed << setprecision(6)
            //  << "Final Train MSE: " << trainMSE << "\n";
            //  << " | Test MSE: " << testMSE << "\n";

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
    
    double predictsingle(vector<string> &features) {
        size_t n = features.size();
        if (theta.size() != n + 1) {
            throw runtime_error("Model not trained or feature size mismatch");
        }
    
        double yhat = theta[0]; // bias term
        for (size_t i = 0; i < n; ++i) {
            yhat += theta[i + 1] * toDouble(features[i]);
        }
        return yhat;
    }
    

    void plotLinearRegression(Data &data, vector<double>& theta) {
        Gnuplot gp;
        
        vector<pair<double, double>> data_points;
        vector<double> y_vals;
        for (size_t i = 0; i < data.features.size(); ++i) {
            double x = toDouble(data.features[i][0]);
            double y = toDouble(data.target[i]);
            data_points.emplace_back(x, y);
            y_vals.push_back(y);
        }
    
        // Determine x range for regression line
        double x_min = min_element(data_points.begin(), data_points.end())->first;
        double x_max = max_element(data_points.begin(), data_points.end())->first;
    
        // Determine y range with padding
        double y_min = *min_element(y_vals.begin(), y_vals.end());
        double y_max = *max_element(y_vals.begin(), y_vals.end());
        y_max *= 1.3;

        int num_points = 100;
        double step = (x_max - x_min) / (num_points - 1);
        vector<pair<double, double>> line_points;
        for (int i = 0; i < num_points; ++i) {
            double x = x_min + i * step;
            double y = theta[0] + theta[1] * x;
            line_points.emplace_back(x, y);
        }
    
        // Plot settings
        // gp << "set terminal pngcairo size 800,600 enhanced font 'Arial,10'\n";
        // gp << "set output './imgs/linear_plot.png'\n";
        gp << "set title 'Linear Regression Fit'\n";
        gp << "set xlabel 'Feature 1'\n";
        gp << "set ylabel 'Target'\n";
        gp << "set grid\n";
        gp << "set key top right opaque box font ',8'\n";
        gp << "set yrange [" << y_min << ":" << y_max << "]\n";
        gp << "plot '-' with points pointtype 7 lc rgb 'blue' title 'Data Points', "
              "'-' with lines lw 3 lc rgb 'red' title 'Regression Line'\n";
    
        gp.send1d(data_points);
        gp.send1d(line_points);
    }
};
