#pragma once
#ifndef DECISION_TREE_H
#define DECISION_TREE_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <limits>
#include <algorithm>
#include <unordered_map>
#include <tuple>
#include <string>
#include "base.h"            // Assumes Model is defined here.
#include "data_handling.h"
#include <gnuplot-iostream.h>

using namespace std;

// A Decision Tree classifier using the Gini impurity measure.
class DecisionTree : public Model {
private:
    // Structure for a node in the decision tree.
    struct Node {
        int featureIndex;      // Index of the feature used for splitting.
        double threshold;      // Threshold value for splitting.
        int prediction;        // Class label prediction (if leaf).
        bool isLeaf;           // Indicates if this node is a leaf.
        Node* left;            // Left child (feature value < threshold).
        Node* right;           // Right child (feature value >= threshold).

        Node() : featureIndex(-1), threshold(0.0), prediction(-1), isLeaf(false), left(nullptr), right(nullptr) {}
    };

    Node* root;                // Root of the decision tree.
    int maxDepth;              // Maximum depth of the tree.
    int minSamplesSplit;       // Minimum number of samples required to split a node.

    // Helper function to compute the Gini impurity for a set of labels.
    double computeGini(const vector<int>& labels) {
        if (labels.empty()) return 0.0;
        unordered_map<int, int> counts;
        for (int label : labels) {
            counts[label]++;
        }
        double impurity = 1.0;
        double total = static_cast<double>(labels.size());
        for (auto &p : counts) {
            double prob = p.second / total;
            impurity -= prob * prob;
        }
        return impurity;
    }

    // Utility: Extracts labels corresponding to given indices.
    vector<int> extractLabels(const vector<int>& y, const vector<int>& indices) {
        vector<int> subset;
        for (int idx : indices) {
            subset.push_back(y[idx]);
        }
        return subset;
    }

    // Helper function to perform the best split on the dataset.
    // Returns a tuple: {bestFeatureIndex, bestThreshold, bestGini, validSplitFound}.
    tuple<int, double, double, bool> findBestSplit(const vector<vector<double>>& X, const vector<int>& y, const vector<int>& indices) {
        int bestFeature = -1;
        double bestThreshold = 0.0;
        double bestImpurity = numeric_limits<double>::max();
        bool validSplitFound = false;

        double currentGini = computeGini(extractLabels(y, indices));
        // If the current node is pure, no split will improve it.
        if (currentGini == 0.0) {
            return make_tuple(bestFeature, bestThreshold, bestImpurity, validSplitFound);
        }

        int numFeatures = X[0].size();
        // Iterate over all features.
        for (int feature = 0; feature < numFeatures; feature++) {
            // Get unique candidate thresholds from the values in this feature.
            vector<double> featureValues;
            for (int idx : indices) {
                featureValues.push_back(X[idx][feature]);
            }
            sort(featureValues.begin(), featureValues.end());
            featureValues.erase(unique(featureValues.begin(), featureValues.end()), featureValues.end());

            // Try potential thresholds (midpoint between consecutive unique values).
            for (size_t i = 1; i < featureValues.size(); i++) {
                double threshold = (featureValues[i - 1] + featureValues[i]) / 2.0;
                vector<int> leftLabels, rightLabels;
                for (int idx : indices) {
                    if (X[idx][feature] < threshold) {
                        leftLabels.push_back(y[idx]);
                    } else {
                        rightLabels.push_back(y[idx]);
                    }
                }
                // Ensure both splits have at least one sample.
                if (leftLabels.empty() || rightLabels.empty()) {
                    continue;
                }
                double leftGini = computeGini(leftLabels);
                double rightGini = computeGini(rightLabels);
                double weightLeft = static_cast<double>(leftLabels.size()) / indices.size();
                double weightRight = static_cast<double>(rightLabels.size()) / indices.size();
                double weightedGini = weightLeft * leftGini + weightRight * rightGini;
                if (weightedGini < bestImpurity) {
                    bestImpurity = weightedGini;
                    bestFeature = feature;
                    bestThreshold = threshold;
                    validSplitFound = true;
                }
            }
        }
        return make_tuple(bestFeature, bestThreshold, bestImpurity, validSplitFound);
    }

    // Recursively builds the decision tree.
    Node* buildTree(const vector<vector<double>>& X, const vector<int>& y, const vector<int>& indices, int depth) {
        Node* node = new Node();

        // Determine the majority class for this node.
        vector<int> currentLabels = extractLabels(y, indices);
        int countZero = 0, countOne = 0;
        // This example assumes binary classification with labels 0 and 1.
        for (int label : currentLabels) {
            if (label == 0)
                countZero++;
            else
                countOne++;
        }
        node->prediction = (countZero >= countOne) ? 0 : 1;

        // Check stopping criteria.
        if (depth >= maxDepth || indices.size() < static_cast<size_t>(minSamplesSplit) || computeGini(currentLabels) == 0.0) {
            node->isLeaf = true;
            return node;
        }

        // Find the best split.
        int bestFeature;
        double bestThreshold, bestImpurity;
        bool validSplit;
        tie(bestFeature, bestThreshold, bestImpurity, validSplit) = findBestSplit(X, y, indices);

        if (!validSplit) {
            node->isLeaf = true;
            return node;
        }

        node->featureIndex = bestFeature;
        node->threshold = bestThreshold;
        node->isLeaf = false;

        // Split indices into left and right nodes.
        vector<int> leftIndices, rightIndices;
        for (int idx : indices) {
            if (X[idx][bestFeature] < bestThreshold)
                leftIndices.push_back(idx);
            else
                rightIndices.push_back(idx);
        }
        if (leftIndices.empty() || rightIndices.empty()) {
            node->isLeaf = true;
            return node;
        }
        // Recursively build left and right subtrees.
        node->left = buildTree(X, y, leftIndices, depth + 1);
        node->right = buildTree(X, y, rightIndices, depth + 1);
        return node;
    }

    // Helper for prediction: Traverse the tree for a single data point.
    int traverseTree(Node* node, const vector<double>& x) {
        if (node->isLeaf) {
            return node->prediction;
        }
        if (x[node->featureIndex] < node->threshold) {
            return traverseTree(node->left, x);
        } else {
            return traverseTree(node->right, x);
        }
    }

    // Recursively free memory of the tree nodes.
    void freeTree(Node* node) {
        if (node == nullptr) return;
        freeTree(node->left);
        freeTree(node->right);
        delete node;
    }

    // ==== Helpers for plotting the tree ==== //

    // Compute node positions using an in-order traversal.
    // The x-coordinate is assigned based on traversal order and y is derived from the depth.
    void computePositions(Node* node, int depth, double &xCounter,
                          unordered_map<Node*, pair<double,double>>& coords) {
        if (!node) return;
        computePositions(node->left, depth + 1, xCounter, coords);
        double x = xCounter;
        double y = -depth;  // Negative depth so root is at the top.
        coords[node] = {x, y};
        xCounter += 1;
        computePositions(node->right, depth + 1, xCounter, coords);
    }

    // Collects edges and node labels for plotting.
    void collectNodesAndEdges(Node* node,
                              unordered_map<Node*, pair<double,double>>& coords,
                              vector<tuple<double, double, double, double>>& edges,
                              vector<tuple<double, double, string>>& nodeLabels) {
        if (!node) return;
        double x = coords[node].first;
        double y = coords[node].second;
        // Create a label: for leaves, show "Leaf: prediction"; for internal nodes, show "X[feature] < threshold".
        string label;
        if (node->isLeaf) {
            label = "Leaf: " + to_string(node->prediction);
        } else {
            label = "X" + to_string(node->featureIndex) + " < " + to_string(node->threshold);
        }
        nodeLabels.push_back(make_tuple(x, y, label));

        if (node->left) {
            double xChild = coords[node->left].first;
            double yChild = coords[node->left].second;
            edges.push_back(make_tuple(x, y, xChild, yChild));
            collectNodesAndEdges(node->left, coords, edges, nodeLabels);
        }
        if (node->right) {
            double xChild = coords[node->right].first;
            double yChild = coords[node->right].second;
            edges.push_back(make_tuple(x, y, xChild, yChild));
            collectNodesAndEdges(node->right, coords, edges, nodeLabels);
        }
    }

public:
    /**
     * @brief Constructor for DecisionTree classifier.
     * 
     * @param max_d Maximum depth of the tree (default: 5).
     * @param min_samples Minimum samples required to split (default: 2).
     * @param lr Not used (default: 0.0).
     * @param ep Not used (default: 0).
     */
    DecisionTree(int max_d = 5, int min_samples = 2, double lr = 0.0, int ep = 0)
        : Model(lr, ep), root(nullptr), maxDepth(max_d), minSamplesSplit(min_samples) {}

    /**
     * @brief Trains the decision tree on the given data.
     * 
     * Converts feature values and target from strings to doubles/ints,
     * then recursively builds the tree using Gini impurity for split evaluation.
     * 
     * @param data The dataset to train on.
     * @throws runtime_error if data is empty or inconsistent.
     * @return A pointer (cast to void*) to a vector of training predictions.
     */
    void* train(handle::Data &data) override {
        size_t m = data.features.size();
        if (m == 0) {
            throw runtime_error("No data available for training.");
        }

        size_t numFeatures = data.features[0].size();
        // Convert feature values to double and target to int.
        vector<vector<double>> X(m, vector<double>(numFeatures, 0.0));
        vector<int> y(m, 0);
        for (size_t i = 0; i < m; i++) {
            if (data.features[i].size() != numFeatures) {
                throw runtime_error("Inconsistent feature dimensions in data.");
            }
            for (size_t j = 0; j < numFeatures; j++) {
                X[i][j] = handle::toDouble(data.features[i][j]);
            }
            y[i] = static_cast<int>(handle::toDouble(data.target[i]));
        }

        // Create a list of indices for all samples.
        vector<int> indices(m);
        for (size_t i = 0; i < m; i++) {
            indices[i] = static_cast<int>(i);
        }

        // Build the decision tree recursively.
        if (root != nullptr) {  // Clean up previous tree if it exists.
            freeTree(root);
        }
        root = buildTree(X, y, indices, 0);

        // Optionally, compute predictions on the training data.
        vector<int> predictions;
        for (size_t i = 0; i < m; i++) {
            int pred = traverseTree(root, X[i]);
            predictions.push_back(pred);
        }
        return static_cast<void*>(new vector<int>(predictions));
    }

    /**
     * @brief Predicts class labels for a given dataset.
     * 
     * @param data The dataset for which predictions are required.
     * @return A vector of doubles, where each value is the predicted class label.
     */
    vector<double> predict(handle::Data &data) override {
        size_t m = data.features.size();
        if (m == 0) {
            throw runtime_error("No data available for prediction.");
        }
        size_t numFeatures = data.features[0].size();
        vector<vector<double>> X(m, vector<double>(numFeatures, 0.0));
        for (size_t i = 0; i < m; i++) {
            if (data.features[i].size() != numFeatures) {
                throw runtime_error("Inconsistent feature dimensions in data.");
            }
            for (size_t j = 0; j < numFeatures; j++) {
                X[i][j] = handle::toDouble(data.features[i][j]);
            }
        }
        vector<double> predictions;
        for (size_t i = 0; i < m; i++) {
            int pred = traverseTree(root, X[i]);
            predictions.push_back(static_cast<double>(pred));
        }
        return predictions;
    }

    /**
     * @brief Plots the decision tree using gnuplot-iostream.
     * 
     * Computes node positions using an in-order traversal and plots nodes (with labels inside boxes)
     * and edges (as arrows) to represent the tree structure.
     */
    void plot(handle::Data &data, double step = 0.05) {
        if (!root) 
            throw runtime_error("Decision tree is empty! Train before plotting.");
        if (data.features.empty() || data.features[0].size() < 2)
            throw runtime_error("Need at least two features for 2D plotting.");
    
        // 1) compute bounds + padding
        double xmin = DBL_MAX, xmax = -DBL_MAX;
        double ymin = DBL_MAX, ymax = -DBL_MAX;
        for (auto &row : data.features) {
            double x = handle::toDouble(row[0]);
            double y = handle::toDouble(row[1]);
            xmin = min(xmin, x); xmax = max(xmax, x);
            ymin = min(ymin, y); ymax = max(ymax, y);
        }
        double xpad = (xmax - xmin) * 0.1, ypad = (ymax - ymin) * 0.1;
        xmin -= xpad; xmax += xpad;
        ymin -= ypad; ymax += ypad;
    
        // 2) build grid of predictions
        int nX = static_cast<int>((xmax - xmin) / step) + 1;
        int nY = static_cast<int>((ymax - ymin) / step) + 1;
        vector<vector<int>> grid(nY, vector<int>(nX));
        for (int i = 0; i < nY; ++i) {
            double yv = ymin + i * step;
            for (int j = 0; j < nX; ++j) {
                double xv = xmin + j * step;
                grid[i][j] = traverseTree(root, {xv, yv});
            }
        }
    
        // 3) plot
        Gnuplot gp;
        gp << "set title 'Decision Boundary'\n";
        gp << "unset key\n";
        gp << "set xrange [" << xmin << ":" << xmax << "]\n";
        gp << "set yrange [" << ymin << ":" << ymax << "]\n";
    
        // <-- pastel-blue for class 0 region, pastel-red for class 1 region -->
        gp << "set palette defined ( 0 'yellow', 1 'red' )\n";
    
        // We’ll send 3 datasets:
        //   1) the grid-region,
        //   2) the class 0 points in royal‑blue,
        //   3) the class 1 points in crimson.
        gp << "plot "
              "'-' matrix with image notitle, "
              "'-' using 1:2 with points pt 7 ps 1 lc rgb 'blue' title 'Class 0', "
              "'-' using 1:2 with points pt 7 ps 1 lc rgb 'green'  title 'Class 1'\n";
    
        // 3a) send grid
        for (int i = 0; i < nY; ++i) {
            for (int j = 0; j < nX; ++j) {
                gp << grid[i][j] << " ";
            }
            gp << "\n";
        }
        gp << "e\n";
    
        // 3b) send class 0 points
        for (size_t i = 0; i < data.features.size(); ++i) {
            int cls = static_cast<int>(handle::toDouble(data.target[i]));
            if (cls == 0) {
                double x = handle::toDouble(data.features[i][0]);
                double y = handle::toDouble(data.features[i][1]);
                gp << x << " " << y << "\n";
            }
        }
        gp << "e\n";
    
        // 3c) send class 1 points
        for (size_t i = 0; i < data.features.size(); ++i) {
            int cls = static_cast<int>(handle::toDouble(data.target[i]));
            if (cls == 1) {
                double x = handle::toDouble(data.features[i][0]);
                double y = handle::toDouble(data.features[i][1]);
                gp << x << " " << y << "\n";
            }
        }
        gp << "e\n";
    }
    
    ~DecisionTree() {
        freeTree(root);
    }
};

#endif // DECISION_TREE_H
