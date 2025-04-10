#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "data_handling.h"// Ensure this file defines the Data structure

class Model {
public:
    double learningRate;
    int epochs;
        
    // Constructor with default parameters for learning rate and epochs.
    Model(double lr = 0.01, int ep = 100) : learningRate(lr), epochs(ep) {}

    // Pure virtual functions to enforce implementation in derived classes.
    virtual void* train(handle::Data &data) = 0;
    virtual std::vector<double> predict(handle::Data &data) = 0;
    virtual ~Model() {} // Virtual destructor for safe inheritance.
};

#endif // MODEL_H
