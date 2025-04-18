#ifndef DECISION_TREE_H
#define DECISION_TREE_H

#include <algorithm>
#include <limits>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <tuple>
#include "base.h"
#include "data_handling.h"
#include "gnuplot-iostream.h"
using namespace handle;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic pop

class DecisionTree : public Model {
public:
    struct Node {
        bool isLeaf;
        double prediction;
        int featureIndex;
        double threshold;
        Node* left;
        Node* right;
        Node(double pred)
            : isLeaf(true), prediction(pred), featureIndex(-1), threshold(0.0), left(nullptr), right(nullptr) {}
        Node(int featIdx, double thresh)
            : isLeaf(false), prediction(0.0), featureIndex(featIdx), threshold(thresh), left(nullptr), right(nullptr) {}
    };

    DecisionTree(int maxDepth = 5, int minSamplesSplit = 2, double lr = 0.01, int ep = 100)
        : Model(lr, ep), root(nullptr), maxDepth(maxDepth), minSamplesSplit(minSamplesSplit) {}

    void* train(handle::Data &data) {
        size_t m = data.features.size();
        if (m == 0) throw std::runtime_error("No data available");
        size_t n = data.features[0].size();
    
        std::vector<std::vector<double>> X(m, std::vector<double>(n));
        std::vector<double> y(m);
        for (size_t i = 0; i < m; ++i) {
            for (size_t j = 0; j < n; ++j)
                X[i][j] = toDouble(const_cast<std::string&>(data.features[i][j]));
            y[i] = toDouble(const_cast<std::string&>(data.target[i]));
        }
    
        root = buildTree(X, y, 0);
        return static_cast<void*>(root);
    }


    std::vector<double> predict(handle::Data &data) {
        size_t m = data.features.size();
        size_t n = data.features[0].size();
        std::vector<std::vector<double>> X(m, std::vector<double>(n));
        for (size_t i = 0; i < m; ++i)
            for (size_t j = 0; j < n; ++j)
                X[i][j] = toDouble(const_cast<std::string&>(data.features[i][j]));
    
        std::vector<double> preds;
        preds.reserve(m);
        for (size_t i = 0; i < m; ++i)
            preds.push_back(traverse(root, X[i]));
        return preds;
    }

    ~DecisionTree() {
        freeTree(root);
    }


    void collectTreeLayout(Node* root, std::map<Node*, std::pair<int, int>>& positions, int depth = 0, int& xOffset = *(new int(0))) {
        if (!root) return;
        collectTreeLayout(root->left, positions, depth + 1, xOffset);
        positions[root] = {xOffset++, -depth};   
        collectTreeLayout(root->right, positions, depth + 1, xOffset);
    }
    
    void plotTree(Node* root) {
        if (!root) return;
        Gnuplot gp;
    
        std::map<Node*, std::pair<int, int>> positions;
        int xOffset = 0;
        collectTreeLayout(root, positions, 0, xOffset);
    
        // Store nodes and edges
        std::vector<std::tuple<int, int, std::string>> nodes; // x, y, label
        std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> edges; // (x1,y1)->(x2,y2)
    
        for (const auto& pair : positions) {
            Node* node = pair.first;
            int x = pair.second.first;
            int y = pair.second.second;
    
            std::string label;
            if (node->isLeaf)
                label = node->label;
            else
                label = "x" + std::to_string(node->featureIndex) + " <= " + std::to_string(node->threshold);
    
            nodes.emplace_back(x, y, label);
    
            if (node->left) {
                auto childPos = positions[node->left];
                edges.push_back({{x, y}, {childPos.first, childPos.second}});
            }
            if (node->right) {
                auto childPos = positions[node->right];
                edges.push_back({{x, y}, {childPos.first, childPos.second}});
            }
        }
    
        // Plot nodes
        gp << "set title 'Decision Tree'\n";
        gp << "set size ratio -1\n";
        gp << "set xrange [-1:" << xOffset + 1 << "]\n";
        gp << "set yrange [-" << positions[root].second + 3 << ":1]\n";
        gp << "unset key\n";
        gp << "set label font ',10'\n";
        gp << "set term qt 0\n";
        gp << "plot '-' using 1:2:3 with labels offset 1,1 notitle, '-' with lines lt rgb 'blue' notitle\n";
        
        gp.send1d(nodes); // Send labels
    
        // Convert edges into line segments
        std::vector<std::pair<double, double>> lines;
        for (auto& e : edges) {
            lines.push_back({e.first.first, e.first.second});
            lines.push_back({e.second.first, e.second.second});
            lines.push_back({NAN, NAN}); // break line
        }
        gp.send1d(lines);
    }

private:
    Node* root;
    int maxDepth;
    int minSamplesSplit;

    Node* buildTree(const std::vector<std::vector<double>> &X, const std::vector<double> &y, int depth) {
        size_t m = y.size();
        if (depth >= maxDepth || m < static_cast<size_t>(minSamplesSplit))
        return new Node(computeMean(y));

        size_t nFeatures = X[0].size();
        double bestMSE = std::numeric_limits<double>::infinity();
        int bestF = -1;
        double bestT = 0.0;
        std::vector<size_t> L, R;

        for (size_t j = 0; j < nFeatures; ++j) {
            std::vector<double> vals = X[j];
            std::sort(vals.begin(), vals.end());
            for (size_t k = 1; k < m; ++k) {
                double thresh = 0.5*(vals[k-1] + vals[k]);
                std::vector<double> yL, yR;
                std::vector<size_t> iL, iR;
                
                for (size_t i = 0; i < m; ++i){
                    if (X[i][j] <= thresh){
                        yL.push_back(y[i]); 
                        iL.push_back(i); 
                    }
                    else{
                        yR.push_back(y[i]); 
                        iR.push_back(i); 
                    }
                }

                if (yL.empty() || yR.empty()) continue;
                double mseL = computeMSE(yL);
                double mseR = computeMSE(yR);
                double wM = (yL.size()*mseL + yR.size()*mseR)/m;
                if (wM < bestMSE) {
                    bestMSE = wM;
                    bestF = j;
                    bestT = thresh;
                    L = iL;
                    R = iR;
                }
            }
        }
        if (bestF < 0) return new Node(computeMean(y));

        std::vector<std::vector<double>> XL, XR;
        std::vector<double> yL, yR;
        for (auto i: L) { XL.push_back(X[i]); yL.push_back(y[i]); }
        for (auto i: R) { XR.push_back(X[i]); yR.push_back(y[i]); }

        Node* node = new Node(bestF, bestT);
        node->left  = buildTree(XL, yL, depth+1);
        node->right = buildTree(XR, yR, depth+1);
        return node;
    }

    double computeMean(const std::vector<double> &y){
        double s = 0; for (double v: y) s += v;
        return s/y.size();
    }

    double computeMSE(const std::vector<double> &y) {
        double m = y.size(), mu = computeMean(y), s=0;
        for (double v: y) s += (v-mu)*(v-mu);
        return s/m;
    }

    double traverse(Node* node, const std::vector<double> &x) {
        if (node->isLeaf) return node->prediction;
        return x[node->featureIndex] <= node->threshold 
            ? traverse(node->left, x)
            : traverse(node->right, x);
    }

    void freeTree(Node* node) {
        if (!node) return;
        freeTree(node->left);
        freeTree(node->right);
        delete node;
    }
};

#endif // DECISION_TREE_H