#ifndef KMEANS_H
#define KMEANS_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <cfloat>
#include <algorithm>
#include "base.h"             // Assumes Model is defined here.
#include "data_handling.h"   // Assumes Data, toDouble(), etc. are defined here.

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include "../include/matplotlibcpp.h"  // or the relevant parts that trigger the warnings
#pragma GCC diagnostic pop

using namespace std;
using namespace handle;

class KNN : public Model {
private:
    Data trainingData; // Storage for the training data.
    int k;           // Number of closest neighbours to consider.

public:
    /**
     * @brief Constructor for KNN.
     * 
     * Since KNN does not use learning rate or epochs, default values (0) are supplied.
     * 
     * @param k_val Number of neighbours.
     * @param lr Not used.
     * @param ep Not used.
     */
    KNN(int k_val = 3, double lr = 0.0, int ep = 0)
        : Model(lr, ep), k(k_val) {}

    /**
     * @brief Train the KNN model.
     * 
     * For KNN, training simply means storing the training data.
     * 
     * @param data The training data.
     */
    void* train(Data &data) override {
        trainingData = data;
        return nullptr;
    }

    /**
     * @brief Predict the label for a single query.
     * 
     * @param query A vector of feature values for the query example.
     * @return The predicted label as a string.
     * @throws runtime_error if there is no training data or if the query size mismatches.
     */
    string predictOne(const vector<double> &query) {
        size_t m = trainingData.features.size();
        if (m == 0) {
            throw runtime_error("No training data available.");
        }
        size_t n = trainingData.features[0].size();
        if (query.size() != n) {
            throw runtime_error("Query feature size does not match training data.");
        }
        
        // Compute the squared Euclidean distance from the query to each training example.
        vector<pair<double, string>> distances;
        for (size_t i = 0; i < m; i++) {
            double distance = 0.0;
            for (size_t j = 0; j < n; j++) {
                double value = toDouble(trainingData.features[i][j]);
                double diff = value - query[j];
                distance += diff * diff;
            }
            distances.push_back(make_pair(distance, trainingData.target[i]));
        }

        // Sort training examples by increasing distance.
        sort(distances.begin(), distances.end(), [](const pair<double, string> &a, const pair<double, string> &b) {
            return a.first < b.first;
        });

        // Vote among the k closest neighbours.
        map<string, int> freq;
        for (int i = 0; i < k && i < distances.size(); i++) {
            freq[distances[i].second]++;
        }

        // Find the label with the highest frequency.
        string bestLabel;
        int maxCount = -1;
        for (const auto &entry : freq) {
            if (entry.second > maxCount) {
                bestLabel = entry.first;
                maxCount = entry.second;
            }
        }
        return bestLabel;
    }

    /**
     * @brief Overriden predict method from Model.
     * 
     * Since the base Model class expects a vector<double> as output,
     * this function attempts to convert each predicted label (string) to a double.
     * If conversion fails, the value 0.0 is returned.
     * 
     * @param data A Data object containing query examples.
     * @return A vector of doubles representing the predicted labels.
     */
    vector<double> predict(Data &data) {
        vector<double> predictions;
        for (auto &feat : data.features) {
            vector<double> query;
            for (auto &val : feat) {
                query.push_back(toDouble(val));
            }
            string label = predictOne(query);
            try {
                predictions.push_back(stod(label));
            } catch (...) {
                predictions.push_back(0.0);  // Could not convert label to double.
            }
        }
        return predictions;
    }
    
    /**
     * @brief Alternative prediction method.
     * 
     * Returns a vector of string labels instead of converting them to doubles.
     * 
     * @param data A Data object containing query examples.
     * @return A vector of strings representing the predicted labels.
     */
    vector<string> predictLabel(Data &data) {
        vector<string> predictions;
        for (auto &feat : data.features) {
            vector<double> query;
            for (auto &val : feat) {
                query.push_back(toDouble(val));
            }
            predictions.push_back(predictOne(query));
        }
        return predictions;
    }
};

#endif // KNN_H
