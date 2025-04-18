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
        string fn="advertising.csv";
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

        // 3. Write corrected Python snippet (no sigmoid)
        ofstream py("temp_linreg.py");
        py<<R"(#!/usr/bin/env python3
import sys, pandas as pd, numpy as np

def main():
    fn = sys.argv[1] if len(sys.argv)>1 else "advertising.csv"
    df = pd.read_csv(fn)
    X = df.iloc[:,:-1].values
    y = df.iloc[:,-1].values.reshape(-1,1)
    # Standardize
    mu = X.mean(axis=0, keepdims=True)
    sd = X.std(axis=0, keepdims=True) + 1e-8
    Xn = (X - mu)/sd
    # Split 80/20 seed=42
    np.random.seed(42)
    idx = np.arange(len(Xn))
    np.random.shuffle(idx)
    split = int(len(idx)*0.8)
    tr, te = idx[:split], idx[split:]
    Xt, yt = Xn[tr], y[tr]
    Xs, ys = Xn[te], y[te]
    # Add intercept
    m,d = Xt.shape
    Xbt = np.hstack([np.ones((m,1)), Xt])
    Xbs = np.hstack([np.ones((len(Xs),1)), Xs])
    # GD hyperparams
    alpha = 0.01; iters = 1000
    theta = np.zeros((d+1,1))
    # Gradient descent
    for _ in range(iters):
        pred = Xbt.dot(theta)
        grad = (Xbt.T.dot(pred - yt))/m
        theta -= alpha*grad
    # MSE train/test
    mse_tr = float(((Xbt.dot(theta)-yt)**2).mean())/2
    mse_te = float(((Xbs.dot(theta)-ys)**2).mean())/2
    # Full-data preds
    Xbf = np.hstack([np.ones((len(Xs),1)), Xs])
    preds = Xbf.dot(theta).flatten()
    # Print: θ, train MSE, test MSE, full preds
    print(" ".join(map(str,theta.flatten())))
    print(mse_tr)
    print(mse_te)
    print(" ".join(map(str,preds)))

if __name__=="__main__":
    main()
)";
        py.close();
        system("chmod +x temp_linreg.py");

        // 4. Run Python and parse 4 lines
        string out = exec("./temp_linreg.py advertising.csv");
        istringstream iss(out);
        string L1,L2,L3,L4;
        if(!getline(iss,L1)||!getline(iss,L2)||!getline(iss,L3)||!getline(iss,L4))
            throw runtime_error("Python output parse failed");

        auto pyTheta = parseLine(L1);
        double mse_tr_py = stod(L2), mse_te_py = stod(L3);
        auto pred_py = parseLine(L4);

        // 5. Compare θ
        bool ok_t = compareVec(cppTheta,pyTheta,1e-2);
        cout<<(ok_t?"θ match":"θ differ!")<<endl;
        if(!ok_t){
            cout<<"C++ θ:"; for(auto v:cppTheta) cout<<" "<<v;
            cout<<"\nPY θ:"; for(auto v:pyTheta) cout<<" "<<v;
            cout<<endl;
        }

        // 6. Compare MSE
        bool ok_tr = fabs(mse_tr_cpp - mse_tr_py)<1e-3;
        bool ok_te = fabs(mse_te_cpp - mse_te_py)<1e-3;
        cout<<(ok_tr?"Train MSE match":"Train MSE differ!")<<endl;
        cout<<(ok_te?"Test MSE match":"Test MSE differ!")<<endl;
        if(!ok_tr) cout<<"C++ tr="<<mse_tr_cpp<<" vs PY tr="<<mse_tr_py<<endl;
        if(!ok_te) cout<<"C++ te="<<mse_te_cpp<<" vs PY te="<<mse_te_py<<endl;

        // 7. Compare Predictions
        bool ok_p = compareVec(pred_cpp,pred_py,1e-2);
        cout<<(ok_p?"Predictions match":"Predictions differ!")<<endl;
        if(!ok_p){
            cout<<"C++ pred:"; for(auto v:pred_cpp) cout<<" "<<v;
            cout<<"\nPY pred:";  for(auto v:pred_py) cout<<" "<<v;
            cout<<endl;
        }

        // 8. Plot the regression line
        lr.plotLinearRegression(testD, cppTheta);

        // Cleanup
        remove("temp_linreg.py");
        delete[] theta_cpp;

        return (ok_t && ok_tr && ok_te && ok_p)?0:1;
    }
    catch(const exception &e){
        cerr<<"ERROR: "<<e.what()<<endl;
        return 1;
    }
}
