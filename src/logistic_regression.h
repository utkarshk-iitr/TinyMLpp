#pragma once
#ifndef LOGISTIC_REGRESSION_H
#define LOGISTIC_REGRESSION_H

#include <vector>
#include <string>
#include "base.h"
#include "data_handling.h"

class LogisticRegression : public Model {
private:
    /**
     * @brief Model parameters:
     *        theta[0] is the intercept;
     *        theta[1..n] are feature weights.
     */
    std::vector<double> theta;

    /**
     * @brief Sigmoid activation function.
     * @param z The input value.
     * @return The sigmoid of z.
     */
    double sigmoid(double z);

public:
    /**
     * @brief Constructor with optional parameters for learning rate and epochs.
     * @param lr Learning rate for gradient descent.
     * @param ep Number of training epochs.
     */
    LogisticRegression(double lr = 0.01, int ep = 1000);

    /**
     * @brief Train the logistic regression model using gradient descent.
     * @param data The training data.
     * @return A void pointer (e.g., to the parameter array).
     */
    void* train(handle::Data &data) override;

    /**
     * @brief Predict outcomes using the logistic function.
     * @param data The dataset to predict.
     * @return A vector of predicted probabilities.
     */
    std::vector<double> predict(handle::Data &data) override;
};

#endif // LOGISTIC_REGRESSION_H