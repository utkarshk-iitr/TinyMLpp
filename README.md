# TinyMLpp

GitHub Repository:
```bash
https://github.com/utkarshk-iitr/TinyMLpp/
```

TinyMLpp is a lightweight, header-only C++17 machine learning library designed for fast experimentation and performance benchmarking. It includes clean implementations of several core ML algorithms and integrates graph plotting using Gnuplot. Additionally, it features a full-stack web interface for an interactive experience.

## Prerequisites
Before running the project, ensure the following dependencies are installed:

Required Packages
1. **g++**: C++ compiler with support for the C++17 standard.

2. **Gnuplot**: A Linux-native plotting utility for graph visualization.

3. **Boost Libraries**: Specifically libboost-iostreams-dev and libboost-system-dev.

Install with APT (Debian/Ubuntu-based systems):

```bash
sudo apt update
sudo apt install libboost-iostreams-dev libboost-system-dev gnuplot libgnuplot-iostream-dev
```

Running the Project
Backend (Node.js)
We also built a complete backend with an interactive frontend.

To start the backend server:

```bash
cd backend
npm install
node index.js
```

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

## Project Features & Progress

### Completed Core Features
- Fully functional frontend and backend built using Node.js
- Performance-optimized algorithms
- Benchmarking of each algorithm
- Graph plotting using Gnuplot for model insights and diagnostics

### Machine Learning Algorithms Implemented
- Linear Regression
- Logistic Regression
- K-Nearest Neighbors (KNN)
- K-Means Clustering
- Decision Tree Classifier
- Support Vector Machine (SVM)

### Test Coverage
Each algorithm has been thoroughly tested for correctness, performance, and visual output where applicable:

- Linear Regression
- K-Means Clustering
- K-Nearest Neighbors
- Logistic Regression
- Decision Tree
- SVM

### Web Interface
Our web interface offers a clean, interactive way to run and visualize ML algorithms directly in the browser. The frontend connects seamlessly with the backend and supports:

- Uploading datasets
- Running ML models
- Viewing visual results and benchmarks


### Planned enhancements include:
- Adding more advanced ML algorithms (e.g., Random Forest, PCA)
- Creating a downloadable binary package for easier installation
- Making the project fully cross-platform (Windows/MacOS/Linux)
- Improving test coverage and adding unit tests
