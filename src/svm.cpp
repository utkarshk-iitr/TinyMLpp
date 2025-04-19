#ifndef SVM_H
#define SVM_H

#include <vector>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <gnuplot-iostream.h>
#include "base.h"
#include "data_handling.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic pop
using namespace handle;

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
        // Uncomment this to enforce 2D feature check
        // if (data.features.empty() || data.features[0].size() != 2)
        //     throw runtime_error("plotSVM requires exactly 2 features");
    
        // extract bias & weights
        double b = params[0];
        double w0 = params[1];
        double w1 = params[2];
    
        // split points by true label
        vector<pair<double, double>> pos_points, neg_points;
        vector<double> y_vals, x_vals;
        for (size_t i = 0; i < data.features.size(); ++i) {
            double x = stod(data.features[i][0]);
            double y = stod(data.features[i][1]);
            double lab = stod(data.target[i]);
            if (lab > 0)
                pos_points.emplace_back(x, y);
            else
                neg_points.emplace_back(x, y);
            y_vals.push_back(y);
            x_vals.push_back(x);
        }
    
        // find min and max x values
        double x_min = numeric_limits<double>::max();
        double x_max = numeric_limits<double>::lowest();
    
        for (const auto& pt : pos_points) {
            x_min = min(x_min, pt.first);
            x_max = max(x_max, pt.first);
        }
        for (const auto& pt : neg_points) {
            x_min = min(x_min, pt.first);
            x_max = max(x_max, pt.first);
        }
    
        // decision boundary line: y = -(w0/w1)x - b/w1
        vector<pair<double, double>> boundary;
        const int num = 100;
        double step = (x_max - x_min) / (num - 1);
        for (int i = 0; i < num; ++i) {
            double x = x_min + i * step;
            double y = -(w0 / w1) * x - b / w1;
            boundary.emplace_back(x, y);
        }
    
        // plot using gnuplot-iostream
        Gnuplot gp;
        double y_min = *min_element(y_vals.begin(), y_vals.end())*2;
        double y_max = *max_element(y_vals.begin(), y_vals.end())*2;
    
        gp << "set title 'Linear SVM Classification'\n";
        gp << "set xlabel 'Feature 0'\n";
        gp << "set ylabel 'Feature 1'\n";
        gp << "set grid\n";
        gp << "set key top right opaque box font ',8'\n";
        gp << "set yrange [" << y_min << ":" << y_max << "]\n";
        // gp << "set style fill solid 1.0\n";
        gp << "plot '-' with points pointtype 7 pointsize 1 lc rgb 'blue' title '+1', "
              "'-' with points pointtype 7 pointsize 1 lc rgb 'red' title '-1', "
              "'-' with lines lc rgb 'black' title 'Decision Boundary'\n";
    
        gp.send1d(pos_points);
        gp.send1d(neg_points);
        gp.send1d(boundary);
    }
};

#endif // SVM_H
