#pragma once
#ifndef KNN_H
#define KNN_H

#include <vector>
#include <string>
#include "base.h"
#include "data_handling.h"

class KNN : public Model {
private:
    handle::Data trainingData;
    int k;

public:
    /**
     * @brief Constructor for KNN.
     * 
     * @param k_val Number of neighbors to use.
     * @param lr Not used (default: 0.0).
     * @param ep Not used (default: 0).
     */
    KNN(int k_val = 3, double lr = 0.0, int ep = 0);

    /**
     * @brief Stores the training data (KNN “training” is just memorizing).
     * 
     * @param data The training dataset.
     * @return Always returns a void pointer (may be nullptr).
     */
    void* train(handle::Data &data) override;

    /**
     * @brief Predicts numeric labels for a dataset by converting string labels to doubles.
     * 
     * If conversion fails, returns 0.0.
     * @param data The dataset containing query examples.
     * @return A vector of doubles representing predicted labels.
     */
    std::vector<double> predict(handle::Data &data) override;

    /**
     * @brief Predicts string labels for a dataset.
     * 
     * @param data The dataset containing query examples.
     * @return A vector of strings representing predicted labels.
     */
    std::vector<std::string> predictLabel(handle::Data &data);

private:
    /**
     * @brief Predicts a single label for one query (vector<double>).
     * 
     * @param query A numeric feature vector.
     * @return A string label representing the majority vote among the k closest examples.
     */
    std::string predictOne(const std::vector<double> &query);
};

#endif // KNN_H