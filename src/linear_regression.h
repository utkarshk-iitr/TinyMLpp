#pragma once
#ifndef LINEAR_REGRESSION_H
#define LINEAR_REGRESSION_H

#include <vector>
#include "base.h"
#include "data_handling.h"

class LinearRegression : public Model {
private:
    /**
     * @brief Model parameters: theta[0] is the intercept;
     *        theta[1..n] are feature weights.
     */
    std::vector<double> theta; 

public:
    /**
     * @brief Constructor with optional parameters for learning rate and epochs.
     * @param lr Learning rate for gradient descent.
     * @param ep Number of training epochs.
     */
    LinearRegression(double lr = 0.01, int ep = 1000);

    /**
     * @brief Train the model using a closed-form solution (single feature)
     *        or gradient descent (multiple features).
     * @param data The training data.
     * @return Pointer to parameters (theta).
     */
    void* train(handle::Data &data) override;

    /**
     * @brief Predict outcomes using the trained model.
     * @param data The dataset to predict.
     * @return A vector of predicted values.
     */
    std::vector<double> predict(handle::Data &data) override;

    /**
     * @brief (Optional) Plot the regression line against the data
     *        (for single-feature data).
     * @param data The dataset.
     * @param theta Model parameters for plotting.
     */
    void plotLinearRegression(handle::Data &data, std::vector<double> &theta);
};

#endif // LINEAR_REGRESSION_H