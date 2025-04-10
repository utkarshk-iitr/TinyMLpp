#include <iostream>
#include <vector>
#include <stdexcept>
#include "base.cpp"
#include "data_handling.cpp"

using namespace std;

class LinearRegression : public Model
{
private:
    vector<double> theta; // Model parameters: theta[0] is the intercept; theta[1..n] are the weights.

public:
    LinearRegression(double lr = 0.01, int ep = 1000) : Model(lr, ep) {}

    // Train the model using either the closed-form solution (for one feature) or gradient descent (for multiple features).
    void train(const Data &data) override
    {
        size_t m = data.features.size();
        if (m == 0)
        {
            throw runtime_error("No data available");
        }
        size_t n = data.features[0].size();

        if (n == 1)
        {
            // Use the closed-form solution for simple linear regression.
            double sumX = 0.0, sumY = 0.0;
            for (size_t i = 0; i < m; ++i)
            {
                double x = toDouble(data.features[i][0]);
                double y = toDouble(data.target[i]);
                sumX += x;
                sumY += y;
            }
            double meanX = sumX / m;
            double meanY = sumY / m;

            double numerator = 0.0, denominator = 0.0;
            for (size_t i = 0; i < m; ++i)
            {
                double x = toDouble(data.features[i][0]);
                double y = toDouble(data.target[i]);
                numerator += (x - meanX) * (y - meanY);
                denominator += (x - meanX) * (x - meanX);
            }
            double theta1 = (denominator == 0 ? 0 : numerator / denominator);
            double theta0 = meanY - theta1 * meanX;
            theta = {theta0, theta1};

            cout << "Closed-form Linear Regression Solution:" << endl;
        }
        else
        {
            // Use gradient descent for multiple features.
            theta.assign(n + 1, 0.0); // theta[0] is intercept; theta[1..n] are feature weights.
            for (int iter = 0; iter < epochs; ++iter)
            {
                vector<double> gradient(n + 1, 0.0);

                for (size_t i = 0; i < m; ++i)
                {
                    double prediction = theta[0];
                    for (size_t j = 0; j < n; ++j)
                    {
                        double xj = toDouble(data.features[i][j]);
                        prediction += theta[j + 1] * xj;
                    }
                    double y = toDouble(data.target[i]);
                    double error = prediction - y;

                    gradient[0] += error;
                    for (size_t j = 0; j < n; ++j)
                    {
                        double xj = toDouble(data.features[i][j]);
                        gradient[j + 1] += error * xj;
                    }
                }
                // Update theta using the average gradient.
                for (size_t j = 0; j < theta.size(); ++j)
                {
                    theta[j] -= learningRate * (gradient[j] / static_cast<double>(m));
                }
                // Optionally, print the cost every 100 iterations.
                if (iter % 100 == 0)
                {
                    double cost = computeMeanSquaredError(data, theta);
                    cout << "Linear Regression Iteration " << iter << ", Cost: " << cost << endl;
                }
            }
        }
    }

    // Predict outcomes using the trained model.
    vector<double> predict(const Data &data) override
    {
        size_t m = data.features.size();
        size_t n = data.features[0].size();
        vector<double> predictions(m, 0.0);

        for (size_t i = 0; i < m; ++i)
        {
            double pred = theta[0]; // Start with the intercept.
            for (size_t j = 0; j < n; ++j)
            {
                pred += theta[j + 1] * toDouble(data.features[i][j]);
            }
            predictions[i] = pred;
        }
        return predictions;
    }
};