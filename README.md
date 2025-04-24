# TinyMLpp

## Running the Project

To run the project, use the following command:

```bash
g++ ./test/linear_regression_test.cpp ./src/linear_regression.cpp ./src/data_handling.cpp -o tinymlpp -std=c++17 -lboost_iostreams -lboost_system && ./tinymlpp
```

```bash
g++ ./demo.cpp ../src/logistic_regression.cpp ../src/linear_regression.cpp ../src/knn.cpp ../src/k_means_clustering.cpp ../src/decision_tree.cpp ../src/svm.cpp ../src/data_handling.cpp -lboost_iostreams -lboost_system -o demo
```

```bash
g++ ./predict.cpp ../src/logistic_regression.cpp ../src/linear_regression.cpp ../src/knn.cpp ../src/k_means_clustering.cpp ../src/decision_tree.cpp ../src/svm.cpp ../src/data_handling.cpp -lboost_iostreams -lboost_system -o predict
```

**Note**: The above command works only for Linux. For Windows, replace the file paths with the appropriate paths on your system after installing the required dependencies.

## Prerequisites

Ensure you have the following installed to enable graph plotting and testing:

1. **Python 3.10**: Required for integration with Python.
2. **NumPy**: Python library for numerical computations.
3. **g++**: A C++ compiler supporting C++17 standard.

You can install the prerequisites using the following commands:

```bash
sudo apt update
sudo apt install g++ python3.10 python3-numpy
```

Once the prerequisites are installed, you can compile and run the project as described above.

## Project Progress

### Test Coverage
- Linear Regression
- K-Means Clustering

### Core Algorithms Coded
- K-Means Clustering
- Linear Regression
- Logistic Regression
- K-Nearest Neighbors (KNN)

### To-Do
- Add test coverage for other algorithms.
- Complete the implementation of remaining core algorithms.
- Optimize the performance of the algorithms.
- Benchmark the algorithms.
