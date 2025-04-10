#pragma once
#ifndef KMEANS_H
#define KMEANS_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <cfloat>
#include <algorithm>
#include "base.h"             // Assumes Model is defined here.
#include "data_handling.cpp"   // Assumes Data, toDouble(), etc. are defined here.

using namespace std;

class KMeans : public Model {
private:
    int k;                       // Number of clusters.
    int maxIterations;           // Maximum number of iterations.
    double tol;                  // Tolerance for centroid movement.
    vector<vector<double>> centroids; // Current centroids.
    vector<int> assignments;          // Cluster index assignment for each data point.

    /**
     * @brief Computes the Euclidean distance between two points.
     * 
     * @param a First point as a vector of doubles.
     * @param b Second point as a vector of doubles.
     * @return The Euclidean distance.
     */
    double euclideanDistance(const vector<double>& a, const vector<double>& b) {
        double sum = 0.0;
        for (size_t i = 0; i < a.size(); i++) {
            double diff = a[i] - b[i];
            sum += diff * diff;
        }
        return sqrt(sum);
    }

public:
    /**
     * @brief Constructor for KMeans clustering.
     * 
     * Since clustering is unsupervised, learningRate and epochs default to 0.
     * 
     * @param k_val Number of clusters.
     * @param maxIter Maximum number of iterations (default: 100).
     * @param tol_val Tolerance for centroid movement (default: 1e-4).
     * @param lr Not used (default: 0.0).
     * @param ep Not used (default: 0).
     */
    KMeans(int k_val, int maxIter = 100, double tol_val = 1e-4, double lr = 0.0, int ep = 0)
        : Model(lr, ep), k(k_val), maxIterations(maxIter), tol(tol_val) {}

    /**
     * @brief Clusters the data using k-Means and stores the final assignments.
     * 
     * The algorithm converts feature values and the target into a numerical point
     * in an augmented space (features plus target), initializes centroids as the first k
     * points, and iteratively refines cluster assignments and centroid locations.
     * 
     * @param data The dataset to cluster.
     * @throws runtime_error if data is empty, inconsistent, or if k is larger than the number of data points.
     */
    void train(const Data &data) override {
        size_t m = data.features.size();  // number of data points
        if (m == 0) {
            throw runtime_error("No data available for clustering.");
        }
        
        // Adjust the dimension: include the target column as an extra feature.
        size_t originalDim = data.features[0].size();
        size_t dim = originalDim + 1;  // combine features and target

        // Convert features from string to double and include the target column.
        vector<vector<double>> points(m, vector<double>(dim, 0.0));
        for (size_t i = 0; i < m; i++) {
            if (data.features[i].size() != originalDim) {
                throw runtime_error("Inconsistent feature dimensions in data.");
            }
            // Convert the feature columns.
            for (size_t j = 0; j < originalDim; j++) {
                points[i][j] = toDouble(data.features[i][j]);
            }
            // Append the target column (treated as a feature) at the end.
            points[i][dim - 1] = toDouble(data.target[i]);
        }
        
        // Ensure that k is not greater than the number of points.
        if (k > m) {
            throw runtime_error("k cannot be greater than the number of data points.");
        }
        
        // Initialize centroids: choose the first k points as initial centroids.
        centroids.assign(points.begin(), points.begin() + k);
        // Initialize assignments container.
        assignments.assign(m, -1);
        
        for (int iter = 0; iter < maxIterations; iter++) {
            bool assignmentChanged = false;
            
            // Step 1: Assign each point to the nearest centroid.
            for (size_t i = 0; i < m; i++) {
                double minDist = DBL_MAX;
                int bestCluster = -1;
                for (int cluster = 0; cluster < k; cluster++) {
                    double dist = euclideanDistance(points[i], centroids[cluster]);
                    if (dist < minDist) {
                        minDist = dist;
                        bestCluster = cluster;
                    }
                }
                if (assignments[i] != bestCluster) {
                    assignments[i] = bestCluster;
                    assignmentChanged = true;
                }
            }
            
            // Step 2: Update centroids based on current assignments.
            vector<vector<double>> newCentroids(k, vector<double>(dim, 0.0));
            vector<int> counts(k, 0);
            for (size_t i = 0; i < m; i++) {
                int cluster = assignments[i];
                counts[cluster]++;
                for (size_t j = 0; j < dim; j++) {
                    newCentroids[cluster][j] += points[i][j];
                }
            }
            // Compute the mean for each centroid.
            for (int cluster = 0; cluster < k; cluster++) {
                if (counts[cluster] == 0) {
                    // No points assigned to this centroid; retain the old centroid.
                    newCentroids[cluster] = centroids[cluster];
                } else {
                    for (size_t j = 0; j < dim; j++) {
                        newCentroids[cluster][j] /= counts[cluster];
                    }
                }
            }
            
            // Step 3: Check for convergence (centroid movement less than tol).
            double totalMovement = 0.0;
            for (int cluster = 0; cluster < k; cluster++) {
                totalMovement += euclideanDistance(centroids[cluster], newCentroids[cluster]);
            }
            
            centroids = newCentroids;  // Update centroids.
            
            // Optional: Print status every 10 iterations.
            if (iter % 10 == 0) {
                cout << "Iteration " << iter << ", total centroid movement: " << totalMovement << endl;
            }
            
            if (!assignmentChanged || totalMovement < tol) {
                cout << "Convergence reached at iteration " << iter << endl;
                break;
            }
        }
    }

    /**
     * @brief Predicts cluster assignments for a given dataset.
     * 
     * For each point in the input data, the method converts its feature values (and target)
     * to a numerical point in the augmented space, then assigns it to the nearest centroid.
     * The result is a vector of cluster indices (as double values for consistency with the Model interface).
     * 
     * @param data The dataset for which cluster assignments are required.
     * @return A vector of doubles, where each value is the cluster index assigned to that data point.
     */
    vector<double> predict(const Data &data) override {
        size_t m = data.features.size();
        if (m == 0) {
            throw runtime_error("No data available for prediction.");
        }
        size_t originalDim = data.features[0].size();
        size_t dim = originalDim + 1;
        vector<double> predictions;
        
        // Process each point in the input data.
        for (size_t i = 0; i < m; i++) {
            // Form the augmented point.
            vector<double> point(dim, 0.0);
            if (data.features[i].size() != originalDim) {
                throw runtime_error("Inconsistent feature dimensions in data.");
            }
            for (size_t j = 0; j < originalDim; j++) {
                point[j] = toDouble(data.features[i][j]);
            }
            point[dim - 1] = toDouble(data.target[i]);
            
            // Assign the point to the nearest centroid.
            double minDist = DBL_MAX;
            int bestCluster = -1;
            for (size_t cluster = 0; cluster < centroids.size(); cluster++) {
                double dist = euclideanDistance(point, centroids[cluster]);
                if (dist < minDist) {
                    minDist = dist;
                    bestCluster = static_cast<int>(cluster);
                }
            }
            predictions.push_back(static_cast<double>(bestCluster));
        }
        
        return predictions;
    }
    
    /**
     * @brief Returns the computed cluster assignments from the latest training.
     * 
     * @return A vector of integers, each in the range [0, k-1], representing cluster assignments.
     */
    vector<int> getAssignments() const {
        return assignments;
    }
};

#endif // KMEANS_H
