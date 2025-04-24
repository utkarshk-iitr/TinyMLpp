# TinyMLpp

## Running the Project
Although we have a fully working website you don't need to write to any sort of commands, GUI just works perfectly. 

Starting backend, navigate to backend directory then type
```bash
npm i
node index.js
```

For extra enthusiastic people, to run the project, use the following command:

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

1. **g++**: A C++ compiler supporting C++17 standard.
2. **gnuplot**: A linux native application used to plot graph. 

You can install the prerequisites using the following commands:

```bash
sudo apt update
sudo apt install libboost-iostreams-dev libboost-system-dev gnuplot libgnuplot-iostream-dev
```

Once the prerequisites are installed, you can compile and run the project as described above.

## Project Progress

### Test Coverage
- Linear Regression
- K-Means Clustering
- K-Nearest Neighbors (KNN)
- Logistic Regression
- Decision Tree
- SVM

### Core Algorithims Coded
- Linear Regression
- K-Means Clustering
- K-Nearest Neighbors (KNN)
- Logistic Regression
- Decision Tree
- SVM

### Task Completed
- Maded a fully working website with a stunning frontend and backend using Node js. 
- Optimize the performance of the algorithms.
- Benchmark the algorithms. 
