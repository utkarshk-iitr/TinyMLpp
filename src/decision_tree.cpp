// decision_tree.cpp
#include "decision_tree.h"
#include <algorithm>
#include <limits>
#include <gnuplot-iostream.h>
using namespace handle;

void* DecisionTree::train(handle::Data &data) {
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

std::vector<double> DecisionTree::predict(handle::Data &data) {
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

DecisionTree::Node* DecisionTree::buildTree(const std::vector<std::vector<double>> &X,
                                             const std::vector<double> &y,
                                             int depth) {
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
            for (size_t i = 0; i < m; ++i) {
                if (X[i][j] <= thresh) { yL.push_back(y[i]); iL.push_back(i); }
                else                   { yR.push_back(y[i]); iR.push_back(i); }
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

double DecisionTree::computeMean(const std::vector<double> &y) {
    double s = 0; for (double v: y) s += v;
    return s/y.size();
}

double DecisionTree::computeMSE(const std::vector<double> &y) {
    double m = y.size(), mu = computeMean(y), s=0;
    for (double v: y) s += (v-mu)*(v-mu);
    return s/m;
}

double DecisionTree::traverse(Node* node, const std::vector<double> &x) {
    if (node->isLeaf) return node->prediction;
    return x[node->featureIndex] <= node->threshold 
        ? traverse(node->left, x)
        : traverse(node->right, x);
}

void DecisionTree::freeTree(Node* node) {
    if (!node) return;
    freeTree(node->left);
    freeTree(node->right);
    delete node;
}

DecisionTree::~DecisionTree() { freeTree(root); }

// In-order layout: left-subtree, node, right-subtree
void DecisionTree::assignPositions(Node* node, int depth, std::map<Node*, double> &xpos, int &counter) {
    if (!node) return;
    assignPositions(node->left,  depth+1, xpos, counter);
    xpos[node] = counter++;
    assignPositions(node->right, depth+1, xpos, counter);
}

void DecisionTree::collectPlotData(Node* node, int depth,
                                   const std::map<Node*, double> &xpos,
                                   std::vector<std::tuple<double,double,std::string>> &nodes,
                                   std::vector<std::tuple<double,double,double,double>> &edges) {
    if (!node) return;
    double x = xpos.at(node), y = -depth;
    std::string lbl = node->isLeaf
        ? "Leaf: " + std::to_string(node->prediction)
        : "x"+std::to_string(node->featureIndex)+" <= "+std::to_string(node->threshold);
    nodes.emplace_back(x,y,lbl);
    if (!node->isLeaf) {
        if (node->left) {
            double x2=xpos.at(node->left), y2=-(depth+1);
            edges.emplace_back(x,y,x2-x,y2-y);
        }
        if (node->right) {
            double x2=xpos.at(node->right), y2=-(depth+1);
            edges.emplace_back(x,y,x2-x,y2-y);
        }
    }
    collectPlotData(node->left,  depth+1, xpos, nodes, edges);
    collectPlotData(node->right, depth+1, xpos, nodes, edges);
}

void DecisionTree::plotTree(bool interactive, const std::string &outFile) {
    std::map<Node*, double> xpos;
    int cnt = 0;
    assignPositions(root, 0, xpos, cnt);
    std::vector<std::tuple<double,double,std::string>> nodes;
    std::vector<std::tuple<double,double,double,double>> edges;
    collectPlotData(root, 0, xpos, nodes, edges);

    Gnuplot gp;
    if (interactive) {
        gp << "set term qt size 1000,800 enhanced font 'Helvetica,12'\n";
    } else {
        gp << "set term pngcairo size 1000,800 enhanced font 'Helvetica,12'\n";
        gp << "set output '"<<outFile<<"'\n";
    }
    gp << "set title 'Decision Tree Structure'\n";
    gp << "unset key; unset border; unset tics\n";
    gp << "set style arrow 1 head filled size screen 0.02,15 lw 2 lc rgb 'purple'\n";
    gp << "plot '-' with vectors arrowstyle 1 notitle, \
          '-' using 1:2:3:4 with labels font ',10' notitle\n";
    gp.send1d(edges);
    gp.send1d(nodes);
    if (!interactive) gp << "unset output\n";
}