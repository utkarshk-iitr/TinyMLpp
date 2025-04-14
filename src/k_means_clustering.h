#pragma once
#ifndef KMEANS_H
#define KMEANS_H

#include <vector>
#include "base.h"
#include "data_handling.h"

class KMeans : public Model {
public:
    /**
     * @brief Constructor for KMeans clustering.
     * @param k_val Number of clusters.
     * @param maxIter Maximum number of iterations (default: 100).
     * @param tol_val Tolerance for centroid movement (default: 1e-4).
     * @param lr Not used (default: 0.0).
     * @param ep Not used (default: 0).
     */
    KMeans(int k_val, int maxIter = 100, double tol_val = 1e-4, double lr = 0.0, int ep = 0);

    /**
     * @brief Performs k-Means clustering on the provided data.
     * @param data The dataset to cluster.
     * @return A void pointer that may store assignments or other info.
     */
    void* train(handle::Data &data) override;

    /**
     * @brief Predicts cluster assignments for new data points.
     * @param data The dataset to predict.
     * @return A vector of doubles representing cluster indices.
     */
    std::vector<double> predict(handle::Data &data) override;

    /**
     * @brief Retrieves final cluster assignments from training.
     */
    std::vector<int> getAssignments();

    /**
     * @brief Optionally, plot the resulting clusters (if needed).
     */
    void plotKMeansClusters(handle::Data &data, std::vector<int> &assignments, int k);
};

#endif // KMEANS_H