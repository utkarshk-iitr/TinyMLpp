// decision_tree.h
#ifndef DECISION_TREE_H
#define DECISION_TREE_H

#include <vector>
#include <string>
#include <map>
#include <tuple>
#include "base.h"
#include "data_handling.h"
#include <gnuplot-iostream.h>

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

    void* train(handle::Data &data) override;
    std::vector<double> predict(handle::Data &data) override;
    ~DecisionTree();

    /**
     * @brief Plot the trained tree structure using gnuplot.
     * @param interactive If true, opens a Qt window; otherwise outputs a PNG to outputFilename.
     * @param outputFilename Path to save PNG (ignored if interactive).
     */
    void plotTree(bool interactive = true, const std::string &outputFilename = "decision_tree.png");

private:
    Node* root;
    int maxDepth;
    int minSamplesSplit;

    Node* buildTree(const std::vector<std::vector<double>> &X,
                    const std::vector<double> &y,
                    int depth);
    double computeMean(const std::vector<double> &y);
    double computeMSE(const std::vector<double> &y);
    double traverse(Node* node, const std::vector<double> &x);
    void freeTree(Node* node);

    // Helpers for plotting
    void assignPositions(Node* node, int depth, std::map<Node*, double> &xpos, int &counter);
    void collectPlotData(Node* node, int depth,
                         const std::map<Node*, double> &xpos,
                         std::vector<std::tuple<double,double,std::string>> &nodes,
                         std::vector<std::tuple<double,double,double,double>> &edges);
};

#endif // DECISION_TREE_H