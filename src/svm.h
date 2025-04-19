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
        SVM(double C_ = 1.0, double lr = 0.001, int ep = 1000);
    
        // Train using batch subgradient descent on ½||w||² + C·hinge
        void* train(Data &data) override;

    // 2D plot (only works if features.size()==2)
    void plotSVM(Data &data, const vector<double>& params);
};

#endif // SVM_H
