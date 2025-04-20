#pragma once
#ifndef KNN_H
#define KNN_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <map>
#include "base.h"             // Assuming Model is defined here
#include "data_handling.h"   // Assuming Data, toDouble(), etc. are defined here
#include "gnuplot-iostream.h"


using namespace handle;
using namespace std;

class KNN : public Model {
private:
    handle::Data trainingData; // Storage for the training data.
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
    void* train(handle::Data &data) override {
        trainingData = data;
        return nullptr; // No training needed for KNN.
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
                double value = handle::toDouble(trainingData.features[i][j]);
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
    vector<double> predict(handle::Data &data) override {
        vector<double> predictions;
        for (auto &feat : data.features) {
            vector<double> query;
            for (auto &val : feat) {
                query.push_back(handle::toDouble(val));
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

    
    void plotKNNResults(handle::Data& testData, vector<double>& predicted) {
        if (testData.features.empty() || testData.features[0].size() < 2) {
            throw runtime_error("Data must have at least two features for 2D plotting.");
        }
    
        Gnuplot gp;
    
        vector<pair<double, double>> correctPoints;
        vector<pair<double, double>> wrongPoints;
        vector<double> y_vals;
        
        for (size_t i = 0; i < testData.features.size(); ++i) {
            double x = handle::toDouble(testData.features[i][0]);
            double y = handle::toDouble(testData.features[i][1]);
            double actual = handle::toDouble(testData.target[i]);
            double pred = predicted[i];
            y_vals.push_back(y);
            if (actual == pred) {
                correctPoints.emplace_back(x, y);
            } else {
                wrongPoints.emplace_back(x, y);
            }
        }
    
        double y_min = *min_element(y_vals.begin(), y_vals.end());
        double y_max = *max_element(y_vals.begin(), y_vals.end());
        y_max *= 1.2;
        // Setup the plot
        // gp << "set terminal pngcairo size 800,600 enhanced font 'Arial,10'\n";
        // gp << "set output './imgs/knn_plot.png'\n";
        gp << "set title 'KNN Classification Results'\n";
        gp << "set xlabel 'Feature 1'\n";
        gp << "set ylabel 'Feature 2'\n";
        gp << "set grid\n";
        gp << "set key top right opaque box font ',8'\n";
        gp << "set yrange [" << y_min << ":" << y_max << "]\n";
        gp << "set style fill solid 1.0\n";
    
        gp << "plot '-' with points pointtype 7 ps 1.5 lc rgb 'green' title 'Correct', "
              "'-' with points pointtype 7 ps 1.5 lc rgb 'red' title 'Incorrect'\n";
    
        gp.send1d(correctPoints);
        gp.send1d(wrongPoints);
    }

    
    /**
     * @brief Alternative prediction method.
     * 
     * Returns a vector of string labels instead of converting them to doubles.
     * 
     * @param data A Data object containing query examples.
     * @return A vector of strings representing the predicted labels.
     */
    vector<string> predictLabel(handle::Data &data) {
        vector<string> predictions;
        for (auto &feat : data.features) {
            vector<double> query;
            for (auto &val : feat) {
                query.push_back(handle::toDouble(val));
            }
            predictions.push_back(predictOne(query));
        }
        return predictions;
    }
};

#endif // KNN_H
