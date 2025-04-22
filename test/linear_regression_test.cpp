#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <cstdlib>
#include <cmath>

#include "../src/data_handling.h"       // Data, readCSV, standardize, train_test_split, computeMeanSquaredError
#include "../src/linear_regression.cpp" // LinearRegression

using namespace std;
using namespace handle;

// Run a shell command and capture stdout
string exec(const char* cmd) {
    array<char,128> buf;
    string out;
    shared_ptr<FILE> pipe(popen(cmd,"r"), pclose);
    if(!pipe) throw runtime_error("popen failed");
    while(fgets(buf.data(), buf.size(), pipe.get()))
        out += buf.data();
    return out;
}

// Parse one whitespace‑separated line of doubles
vector<double> parseLine(const string &l) {
    istringstream iss(l);
    vector<double> v; double x;
    while(iss >> x) v.push_back(x);
    return v;
}

// Compare two vectors within tolerance
bool compareVec(const vector<double>& a, const vector<double>& b, double tol=1e-4) {
    if(a.size()!=b.size()) return false;
    for(size_t i=0;i<a.size();i++)
        if(fabs(a[i]-b[i])>tol) return false;
    return true;
}

int main(){
    try {
        // 1. Load & Z‑score normalize
        string fn="./datasets/advertising.csv";
        Data data = readCSV(fn);
        displayDataFrame(data);  // optional
        standardize(data);

        // 2. Train C++ model (internally splits 80/20, seed 42)
        LinearRegression lr(0.01,1000);
        // Compute train/test MSE in C++
        auto [trainD,testD] = train_test_split(data,0.2,42);
        cout<<"Train set size: "<<trainD.features.size()<<endl;
        cout<<"Test set size: "<<testD.features.size()<<endl;
        void* raw = lr.train(trainD); 
        double* theta_cpp = static_cast<double*>(raw);
        size_t d = data.features[0].size();

        // Collect θ
        vector<double> cppTheta(d+1);
        cout<<"C++ θ:"<<endl;
        for(size_t i=0;i<=d;i++){
            cppTheta[i]=theta_cpp[i];
            cout<<" θ["<<i<<"]="<<cppTheta[i]<<endl;
        }

        
        double mse_tr_cpp = computeMeanSquaredError(trainD, cppTheta);
        double mse_te_cpp = computeMeanSquaredError(testD,  cppTheta);
        cout<<fixed<<setprecision(6)
            <<"C++ Train MSE="<<mse_tr_cpp<<" | Test MSE="<<mse_te_cpp<<endl;

        //Predictions on the test set
        vector<double> pred_cpp = lr.predict(testD);
        lr.plotLinearRegression(testD, cppTheta);
        return 0;
    }
    catch(const exception &e){
        cerr<<"ERROR: "<<e.what()<<endl;
        return 1;
    }
}
