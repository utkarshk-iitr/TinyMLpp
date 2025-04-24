// predict.cpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <sys/resource.h>
#include <numeric>
#include <stdexcept>
#include "../src/data_handling.h"         // Data, readCSV, standardize, train_test_split, computeLogLoss, computeAccuracy
#include "../src/linear_regression.cpp"   // LinearRegression
#include "../src/logistic_regression.cpp" // LogisticRegression
#include "../src/knn.cpp"               // KNN
#include "../src/svm.cpp"               // SVM
#include "../src/k_means_clustering.cpp" // KMeans
// #include "../src/decision_tree.cpp"     // DecisionTree


using namespace std;
using namespace handle;

// Simple argv parser for --model and --parameters
void parseArgs(int argc, char** argv,string &modelName,string &paramf, string &weightsf, string &dataset, int &k)
{
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--model" && i+1 < argc) {
            modelName = argv[++i];
        }
        else if (arg == "--weights" && i+1 < argc) {
            weightsf = argv[++i];
        }
        else if (arg == "--features" && i+1 < argc) {
            paramf = argv[++i];
        }
        else if(arg == "--kvalue" && i+1 < argc) {
            k = stoi(argv[++i]);
        }
        else {
            throw runtime_error("Unknown argument: " + arg);
        }
    }
    if (modelName.empty() || paramf.empty() || weightsf.empty()) {
        throw runtime_error(
            "Usage: ./predict --model \"<model>\" "
            "--weights \"weights.txt\" "
            "--features \"features.txt\""
            "--kvalue \"<k_value>\""
        );
    }
}

// Parse comma‑separated parameters from the file
void parseParams(const string& filename, vector<string>& params) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Unable to open parameter file: " + filename);
    }

    string line;
    getline(file, line); // Read the entire line
    file.close();

    if (line.empty()) 
    {
        throw runtime_error("Parameter file is empty: " + filename);
    }

    stringstream ss(line);
    string token;

    while (getline(ss, token, ',')) 
    {
        // Trim leading and trailing whitespace
        token.erase(token.begin(), find_if(token.begin(), token.end(), [](unsigned char ch) {
            return !isspace(ch);
        }));
        token.erase(find_if(token.rbegin(), token.rend(), [](unsigned char ch) {
            return !isspace(ch);
        }).base(), token.end());

        if (!token.empty()) {
            params.push_back(token);
        }
    }
}

void readCSVtoMatrix(const string& filename, vector<vector<double>>& matrix) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Cannot open file: " + filename);
    }

    string line;

    while (getline(file, line)) {                                           // read row :contentReference[oaicite:0]{index=0}
        vector<double> row;
        stringstream ss(line);
        string cell;
        while (getline(ss, cell, ',')) {                                     // split columns :contentReference[oaicite:1]{index=1}
            // trim whitespace (optional)
            cell.erase(cell.begin(), find_if(cell.begin(), cell.end(),
                         [](unsigned char ch){ return !isspace(ch); }));
            cell.erase(find_if(cell.rbegin(), cell.rend(),
                         [](unsigned char ch){ return !isspace(ch); }).base(),
                         cell.end());                                               // trim trailing :contentReference[oaicite:2]{index=2}

            if (!cell.empty()) {
                row.push_back(stod(cell));                                   // convert to double 
            }
        }
        if (!row.empty()) {
            matrix.push_back(move(row));                                     // add row to matrix :contentReference[oaicite:3]{index=3}
        }
    }
}

int main(int argc, char** argv) {
    try {
        // 1) Parse command‑line
        string modelName, paramf, weightsf, dataset;
        int k;
        parseArgs(argc, argv, modelName, paramf, weightsf, dataset, k);
        vector<string> weight;
        vector<vector<double>> centroid;
        vector<string> feature;
        Data data;
        parseParams(paramf, feature);
        if(modelName == "k_means_clustering")
        {
            readCSVtoMatrix(weightsf, centroid);
        }
        else if(modelName == "knn")
        {
            data = readCSV(weightsf);
        }
        else
        parseParams(weightsf, weight);

        if(modelName == "linear-regression")
        {
            LinearRegression model;

            for(auto x:weight)
            {
                model.theta.push_back(toDouble(x));
            }

            ofstream js("predict.json");
            js << "{\n";
            js << "  \"prediction\": "   <<  model.predictSingle(feature)   << "\n";

            js << "}\n";
            js.close();
        }
        else if(modelName == "logistic-regression")
        {
            LogisticRegression model;

            for(auto x:weight)
            {
                model.theta.push_back(toDouble(x));
            }

            ofstream js("predict.json");
            js << "{\n";
            js << "  \"prediction\": "   <<  model.predictSingle(feature)   << "\n";

            js << "}\n";
            js.close();
        }
        else if(modelName == "svm")
        {
            SVM model;
            int i=0;
            for(auto x:weight)
            {
                if(i==0)
                model.bias = toDouble(x);
                else
                model.weights.push_back(toDouble(x));
                i++;
            }

            ofstream js("predict.json");
            js << "{\n";
            js << "  \"prediction\": "   <<  (model.predictSingle(feature)==-1?0:1)   << "\n";

            js << "}\n";
            js.close();
        }
        else if(modelName == "k_means_clustering")
        {
           KMeans model(k);
           model.centroids = centroid;

           ofstream js("predict.json");
           js << "{\n";
           js << "  \"prediction\": "   <<  model.predictSingle(feature)   << "\n";

           js << "}\n";
           js.close();
        // Time & memory measurement start
        }
        else if(modelName == "knn")
        {
            KNN model(k);
            model.train(data);
            vector<double> query;
            for (auto val : feature) {
                query.push_back(handle::toDouble(val));
            }
            // query.pop_back();
            string label = model.predictOne(query);
            
            ofstream js("predict.json");
            js << "{\n";
            js << "  \"prediction\": "   <<  label  << "\n";

            js << "}\n";
            js.close();
        }
        else
        {
            throw runtime_error("Unknown model name: " + modelName);
        }
    }
       catch (const exception &ex) 
       {
        cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}
