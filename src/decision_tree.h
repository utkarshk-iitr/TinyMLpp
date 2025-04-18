// decision_tree.h
#ifndef DECISION_TREE_H
#define DECISION_TREE_H

#include <vector>
#include "base.h"
#include "data_handling.h"

class DecisionTree : public Model {
public:
    struct Node {
        bool isLeaf;
        double prediction;
        int featureIndex;
        double threshold;
        Node* left;
        Node* right;
        // Leaf node constructor
        Node(double pred)
            : isLeaf(true), prediction(pred), featureIndex(-1), threshold(0.0), left(nullptr), right(nullptr) {}
        // Decision node constructor
        Node(int featIdx, double thresh)
            : isLeaf(false), prediction(0.0), featureIndex(featIdx), threshold(thresh), left(nullptr), right(nullptr) {}
    };

    /**
     * @brief Constructor for DecisionTree.
     * @param maxDepth Maximum tree depth.
     * @param minSamplesSplit Minimum samples to split.
     */
    DecisionTree(int maxDepth = 5, int minSamplesSplit = 2, double lr = 0.01, int ep = 100)
        : Model(lr, ep), root(nullptr), maxDepth(maxDepth), minSamplesSplit(minSamplesSplit) {}

    /**
     * @brief Train the decision tree model on the dataset.
     * @param data The input dataset (features + target).
     * @return Pointer to the root node of the trained tree.
     */
    void* train(handle::Data &data) override;

    /**
     * @brief Predict outcomes for the dataset using the trained tree.
     * @param data The dataset to predict.
     * @return Vector of predicted values.
     */
    std::vector<double> predict(handle::Data &data) override;

    /**
     * @brief Destructor to free allocated nodes.
     */
    ~DecisionTree();

private:
    Node* root;
    int maxDepth;
    int minSamplesSplit;

    /**
     * @brief Recursively build the decision tree.
     */
    Node* buildTree(const std::vector<std::vector<double>> &X,
                    const std::vector<double> &y,
                    int depth);

    /**
     * @brief Compute mean of target values.
     */
    double computeMean(const std::vector<double> &y);

    /**
     * @brief Compute mean squared error of target values.
     */
    double computeMSE(const std::vector<double> &y);

    /**
     * @brief Free all nodes in the tree.
     */
    void freeTree(Node* node);

    /**
     * @brief Traverse the tree to make a single prediction.
     */
    double traverse(Node* node, const std::vector<double> &x);
};

#endif // DECISION_TREE_H