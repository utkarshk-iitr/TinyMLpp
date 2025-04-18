#ifndef SVM_H
#define SVM_H

#include <vector>
#include <cmath>
#include <algorithm>
#include <stdexcept>

#include "base.h"
#include "data_handling.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include "../include/matplotlibcpp.h"
#pragma GCC diagnostic pop

using namespace handle;
namespace plt = matplotlibcpp;

class SVM : public Model {
    private:
        std::vector<double> weights;  // w (size = #features)
        double bias;                  // b
        double C;                     // regularization parameter
    
    public:
        // C: penalty term, lr: learning rate, ep: epochs
        SVM(double C_ = 1.0, double lr = 0.001, int ep = 1000)
          : Model(lr, ep), C(C_), bias(0.0) {}
    
        // Train using batch subgradient descent on ½||w||² + C·hinge
        void* train(Data &data) override {
            size_t m = data.features.size();
            if (m == 0) throw std::runtime_error("No data provided to SVM::train");
            size_t n = data.features[0].size();
    
            weights.assign(n, 0.0);
            bias = 0.0;
    
            for (int epoch = 0; epoch < epochs; ++epoch) {
                std::vector<double> grad_w(n, 0.0);
                double grad_b = 0.0;
    
                // 1) hinge‐loss subgradient
                for (size_t i = 0; i < m; ++i) {
                    double yi = toDouble(data.target[i]);
                    double dot = bias;
                    for (size_t j = 0; j < n; ++j)
                        dot += weights[j] * toDouble(data.features[i][j]);
    
                    if (yi * dot < 1.0) {
                        for (size_t j = 0; j < n; ++j)
                            grad_w[j] += -C * yi * toDouble(data.features[i][j]);
                        grad_b   += -C * yi;
                    }
                }
    
                // 2) regularization gradient: ∇½||w||² = w
                for (size_t j = 0; j < n; ++j)
                    grad_w[j] += weights[j];
    
                // 3) update (no averaging by m)
                for (size_t j = 0; j < n; ++j)
                    weights[j] -= learningRate * grad_w[j];
                bias -= learningRate * grad_b;
            }
    
            // pack parameters: [b, w₀, w₁, …]
            double* params = new double[n + 1];
            params[0] = bias;
            for (size_t j = 0; j < n; ++j)
                params[j + 1] = weights[j];
            return static_cast<void*>(params);
        }
    
        // Predict labels {-1, +1}
        std::vector<double> predict(Data &data) override {
            size_t m = data.features.size();
            size_t n = data.features[0].size();
            std::vector<double> preds(m);
    
            for (size_t i = 0; i < m; ++i) {
                double sum = bias;
                for (size_t j = 0; j < n; ++j)
                    sum += weights[j] * toDouble(data.features[i][j]);
                preds[i] = (sum >= 0.0 ? 1.0 : -1.0);
            }
            return preds;
        }

    // 2D plot (only works if features.size()==2)
    void plotSVM(Data &data, const vector<double>& params) {
        // if (data.features.empty() || data.features[0].size() != 2)
        //     throw runtime_error("plotSVM requires exactly 2 features");

        // extract bias & weights
        double b = params[0];
        double w0 = params[1];
        double w1 = params[2];

        // split points by true label
        vector<double> x_pos, y_pos, x_neg, y_neg;
        for (size_t i = 0; i < data.features.size(); ++i) {
            double x = toDouble(data.features[i][0]);
            double y = toDouble(data.features[i][1]);
            double lab = toDouble(data.target[i]);
            if (lab > 0) {
                x_pos.push_back(x);
                y_pos.push_back(y);
            } else {
                x_neg.push_back(x);
                y_neg.push_back(y);
            }
        }

        // decision boundary line: w0*x + w1*y + b = 0  ⇒  y = -(w0/w1)x - b/w1
        double x_min = *min_element(x_pos.begin(), x_pos.end());
        x_min = min(x_min, *min_element(x_neg.begin(), x_neg.end()));
        double x_max = *max_element(x_pos.begin(), x_pos.end());
        x_max = max(x_max, *max_element(x_neg.begin(), x_neg.end()));

        const int num = 100;
        vector<double> xb(num), yb(num);
        double step = (x_max - x_min) / (num - 1);
        for (int i = 0; i < num; ++i) {
            xb[i] = x_min + i * step;
            yb[i] = -(w0 / w1) * xb[i] - b / w1;
        }

        plt::figure_size(800, 600);
        plt::scatter(x_pos, y_pos, 30.0, {{"color", "blue"}, {"label", "+1"}});
        plt::scatter(x_neg, y_neg, 30.0, {{"color", "red"},   {"label", "-1"}});
        plt::plot(xb, yb, {{"color", "black"}, {"label", "Decision Boundary"}});
        plt::xlabel("Feature 0");
        plt::ylabel("Feature 1");
        plt::title("Linear SVM Classification");
        plt::legend();
        plt::grid(true);
        plt::show();
    }
};

#endif // SVM_H
