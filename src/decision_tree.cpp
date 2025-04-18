// decision_tree.cpp
#include "decision_tree.h"
#include <algorithm>
#include <limits>
#include <stdexcept>

using namespace handle;

void* DecisionTree::train(handle::Data &data) {
    size_t m = data.features.size();
    if (m == 0) {
        throw std::runtime_error("No data available");
    }
    size_t n = data.features[0].size();

    // Convert features and target to numeric
    std::vector<std::vector<double>> X(m, std::vector<double>(n));
    std::vector<double> y(m);
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            X[i][j] = toDouble(const_cast<std::string&>(data.features[i][j]));
        }
        y[i] = toDouble(const_cast<std::string&>(data.target[i]));
    }

    // Build the decision tree
    root = buildTree(X, y, 0);
    return static_cast<void*>(root);
}

std::vector<double> DecisionTree::predict(handle::Data &data) {
    size_t m = data.features.size();
    size_t n = data.features[0].size();
    std::vector<std::vector<double>> X(m, std::vector<double>(n));
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            X[i][j] = toDouble(const_cast<std::string&>(data.features[i][j]));
        }
    }

    std::vector<double> predictions;
    predictions.reserve(m);
    for (size_t i = 0; i < m; ++i) {
        predictions.push_back(traverse(root, X[i]));
    }
    return predictions;
}

DecisionTree::Node* DecisionTree::buildTree(const std::vector<std::vector<double>> &X,
                                             const std::vector<double> &y,
                                             int depth) {
    size_t m = y.size();
    // Stopping criteria
    if (depth >= maxDepth || m < static_cast<size_t>(minSamplesSplit)) {
        double leafValue = computeMean(y);
        return new Node(leafValue);
    }

    size_t nFeatures = X[0].size();
    double bestMSE = std::numeric_limits<double>::infinity();
    int bestFeature = -1;
    double bestThreshold = 0.0;
    std::vector<size_t> bestLeftIdx, bestRightIdx;

    // Try splitting on each feature
    for (size_t j = 0; j < nFeatures; ++j) {
        // Collect unique values for threshold candidates
        std::vector<double> values(m);
        for (size_t i = 0; i < m; ++i) values[i] = X[i][j];
        std::sort(values.begin(), values.end());
        for (size_t k = 1; k < m; ++k) {
            double thresh = 0.5 * (values[k - 1] + values[k]);
            std::vector<double> leftY, rightY;
            std::vector<size_t> leftIdx, rightIdx;
            for (size_t i = 0; i < m; ++i) {
                if (X[i][j] <= thresh) {
                    leftY.push_back(y[i]); leftIdx.push_back(i);
                } else {
                    rightY.push_back(y[i]); rightIdx.push_back(i);
                }
            }
            if (leftY.empty() || rightY.empty()) continue;
            double mseLeft = computeMSE(leftY);
            double mseRight = computeMSE(rightY);
            double weightedMSE = (leftY.size() * mseLeft + rightY.size() * mseRight) / m;
            if (weightedMSE < bestMSE) {
                bestMSE = weightedMSE;
                bestFeature = j;
                bestThreshold = thresh;
                bestLeftIdx = leftIdx;
                bestRightIdx = rightIdx;
            }
        }
    }

    // If no valid split found, make leaf
    if (bestFeature < 0) {
        double leafValue = computeMean(y);
        return new Node(leafValue);
    }

    // Partition data for child nodes
    std::vector<std::vector<double>> Xl, Xr;
    std::vector<double> yl, yr;
    for (size_t idx : bestLeftIdx) {
        Xl.push_back(X[idx]); yl.push_back(y[idx]);
    }
    for (size_t idx : bestRightIdx) {
        Xr.push_back(X[idx]); yr.push_back(y[idx]);
    }

    Node* node = new Node(bestFeature, bestThreshold);
    node->left  = buildTree(Xl, yl, depth + 1);
    node->right = buildTree(Xr, yr, depth + 1);
    return node;
}

double DecisionTree::computeMean(const std::vector<double> &y) {
    double sum = 0.0;
    for (double v : y) sum += v;
    return sum / y.size();
}

double DecisionTree::computeMSE(const std::vector<double> &y) {
    double mean = computeMean(y);
    double mse = 0.0;
    for (double v : y) mse += (v - mean) * (v - mean);
    return mse / y.size();
}

double DecisionTree::traverse(Node* node, const std::vector<double> &x) {
    if (node->isLeaf) {
        return node->prediction;
    }
    if (x[node->featureIndex] <= node->threshold) {
        return traverse(node->left, x);
    } else {
        return traverse(node->right, x);
    }
}

void DecisionTree::freeTree(Node* node) {
    if (!node) return;
    freeTree(node->left);
    freeTree(node->right);
    delete node;
}

DecisionTree::~DecisionTree() {
    freeTree(root);
}

// Plotting helpers
void DecisionTree::assignPositions(Node* node, int depth, std::map<Node*, double> &xpos, int &counter) {
    if (!node) return;
    assignPositions(node->left, depth+1, xpos, counter);
    xpos[node] = counter++;
    assignPositions(node->right, depth+1, xpos, counter);
}

void DecisionTree::collectPlotData(Node* node, int depth,
                                   const std::map<Node*, double> &xpos,
                                   std::vector<std::tuple<double,double,std::string>> &nodes,
                                   std::vector<std::tuple<double,double,double,double>> &edges) {
    if (!node) return;
    double x = xpos.at(node);
    double y = -depth;
    std::string label;
    if (node->isLeaf) {
        label = std::to_string(node->prediction);
    } else {
        label = "x" + std::to_string(node->featureIndex) + "<=" + std::to_string(node->threshold);
        if (node->left) {
            double x2 = xpos.at(node->left);
            double y2 = -(depth+1);
            edges.emplace_back(x, y, x2-x, y2-y);
        }
        if (node->right) {
            double x2 = xpos.at(node->right);
            double y2 = -(depth+1);
            edges.emplace_back(x, y, x2-x, y2-y);
        }
    }
    nodes.emplace_back(x, y, label);
    collectPlotData(node->left, depth+1, xpos, nodes, edges);
    collectPlotData(node->right, depth+1, xpos, nodes, edges);
}

void DecisionTree::plotTree(const std::string &outputFilename) {
    std::map<Node*, double> xpos;
    int counter = 0;
    assignPositions(root, 0, xpos, counter);

    std::vector<std::tuple<double,double,std::string>> nodes;
    std::vector<std::tuple<double,double,double,double>> edges;
    collectPlotData(root, 0, xpos, nodes, edges);

    Gnuplot gp;
    gp << "set term png size 800,600\n";
    gp << "set output '" << outputFilename << "'\n";
    gp << "unset key\n";
    gp << "plot '-' with vectors nohead notitle, '-' using 1:2:3:4 with labels offset char 0,0 center notitle\n";
    gp.send1d(edges);
    gp.send1d(nodes);
}
