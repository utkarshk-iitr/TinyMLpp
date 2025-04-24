// demo.cpp

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
#include "../src/decision_tree.cpp"     // DecisionTree

// Base64 encoding table
static const string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

string base64_encode(const vector<unsigned char>& data) {
    string encoded;
    int val = 0, valb = -6;
    for (unsigned char c : data) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            encoded.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) encoded.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    while (encoded.size() % 4) encoded.push_back('=');
    return encoded;
}

vector<unsigned char> read_file_binary(const string& filename) {
    ifstream file(filename, ios::binary);
    return vector<unsigned char>((istreambuf_iterator<char>(file)), {});
}


using namespace std;
using namespace handle;

// Simple argv parser for --model and --parameters
void parseArgs(int argc, char** argv,
               string &modelName,
               string &paramStr)
{
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--model" && i+1 < argc) {
            modelName = argv[++i];
        }
        else if (arg == "--parameters" && i+1 < argc) {
            paramStr = argv[++i];
        }
    }
    if (modelName.empty() || paramStr.empty()) {
        throw runtime_error(
            "Usage: ./demo --model \"<model>\" "
            "--parameters \"dataset=path,lr=0.01,epochs=500,k=3,C=1.0\""
        );
    }
}

// Parse comma‑separated key=val pairs into a map
map<string,string> parseParams(const string &s) {
    map<string,string> m;
    istringstream ss(s);
    string kv;
    while (getline(ss, kv, ',')) {
        auto pos = kv.find('=');
        if (pos != string::npos) {
            string k = kv.substr(0,pos);
            string v = kv.substr(pos+1);
            m[k] = v;
        }
    }
    return m;
}

int main(int argc, char** argv) {
    try {
        // 1) Parse command‑line
        string modelName, paramStr;
        parseArgs(argc, argv, modelName, paramStr);
        auto params = parseParams(paramStr);

        // Required: dataset path
        if (!params.count("dataset"))
            throw runtime_error("Missing `dataset` param");
        string datasetFile = params["dataset"];

        // Optional hyperparameters
        double lr = params.count("lr") ? stod(params["lr"]) : 0.01;
        int epochs = params.count("epochs") ? stoi(params["epochs"]) : 1000;
        int k = params.count("k") ? stoi(params["k"]) : 3; // For KNN, not used here
        double C = params.count("C") ? stod(params["C"]) : 1.0; // For SVM, not used here

        // Time & memory measurement start
        using clock = chrono::high_resolution_clock;
        auto t0 = clock::now();

        // 2) Load & normalize full dataset
        Data all = readCSV(datasetFile);
        displayDataFrame(all);
        if(modelName != "k_means_clustering")
        standardize(all);
        // 3) Split 80/20, seed=42
        auto [trainD, testD] = train_test_split(all, 0.2, 42);

        // 4) Instantiate model
        Model* model = nullptr;
        if (modelName == "linear_regression") {
            model = new LinearRegression(lr, epochs);
        }
        else if (modelName == "logistic_regression") {
            model = new LogisticRegression(lr, epochs);
        }
        else if (modelName == "knn") {
            model = new KNN(k, lr, epochs);
        }
        else if (modelName == "svm") {
            model = new SVM(C, lr, epochs);
            for (auto &lbl : testD.target) {
                double y = toDouble(lbl);
                lbl = (y == 0.0 ? "-1" : "1");
            }
            for (auto &lbl : trainD.target) {
                double y = toDouble(lbl);
                lbl = (y == 0.0 ? "-1" : "1");
            }
        }
        else if (modelName == "k_means_clustering") {
            model = new KMeans(k, epochs, 1e-4); // lr is not used in KMeans
        }
        else if(modelName == "decision_tree") {
            model = new DecisionTree(lr, epochs); // lr is not used in DecisionTree
        }
        else {
            throw runtime_error("Unknown model: " + modelName);
        }


        // Train on training split
        void* rawTheta;
        
        if(modelName !="k_means_clustering")
        rawTheta = model->train(trainD);
        else
        rawTheta = model->train(all);

        // Predict on test split
        vector<double> preds = model->predict(testD);

        auto t1 = clock::now();
        double time_ms = chrono::duration<double, milli>(t1 - t0).count();

        // Peak RSS in KB
        struct rusage usage{};
        getrusage(RUSAGE_SELF, &usage);
        long peakRSS_kb = usage.ru_maxrss;

        // 6) Compute accuracy or R^2
        double accuracy = 0.0;
        // double llerror = 0.0;
        double mse = 0.0;
        double inertia = 0.0;
        double precision = 0.0;
        double recall = 0.0;
        double f1 = 0.0;
        double r2 = 0.0;

        if (modelName == "logistic_regression") {
            // Classification accuracy
            vector<double> yTrue;
            for (auto &s : testD.target) yTrue.push_back(toDouble(s));
            // Round probabilities at 0.5
            vector<double> yPred;
            for (double p : preds) yPred.push_back(p > 0.5 ? 1.0 : 0.0);
            accuracy = computeAccuracy(yTrue, yPred);
            // llerror = computeLogLoss(testD, preds);
            precision = computePrecision(yTrue, yPred);
            recall = computeRecall(yTrue, yPred);
            f1 = computeF1Score(yTrue, yPred);
        }
        else if (modelName == "linear_regression") {
            // Regression: compute R^2 = 1 - SSE/SST
            vector<double> yTrue;
            for (auto &s : testD.target) yTrue.push_back(toDouble(s));
            double meanY = accumulate(yTrue.begin(), yTrue.end(), 0.0) / yTrue.size();
            double ssTot = 0.0, ssRes = 0.0;
            for (size_t i=0; i<yTrue.size(); ++i) {
                ssTot += (yTrue[i] - meanY)*(yTrue[i] - meanY);
                ssRes += (yTrue[i] - preds[i])*(yTrue[i] - preds[i]);
            }
            r2 = 1.0 - ssRes/ssTot;
            mse = computeMeanSquaredError(testD, preds);
        }
        else if (modelName == "knn") {
            // KNN: accuracy is already computed above
            vector<double> yTrue;
            for (auto &s : testD.target) yTrue.push_back(toDouble(s));
            accuracy = computeAccuracy(yTrue, preds);
            precision = computePrecision(yTrue, preds);
            recall = computeRecall(yTrue, preds);
            f1 = computeF1Score(yTrue, preds);
        }
        else if (modelName == "svm") {
            // SVM: accuracy is already computed above
            vector<double> yTrue;
            for (auto &s : testD.target) yTrue.push_back(toDouble(s));
            accuracy = computeAccuracy(yTrue, preds);
            precision = computePrecision(yTrue, preds);
            recall = computeRecall(yTrue, preds);
            f1 = computeF1Score(yTrue, preds);
        }
        else if (modelName == "k_means_clustering") {
            vector<int> assignments = static_cast<KMeans*>(model)->getAssignments();
            const auto& centroids = static_cast<KMeans*>(model)->getCentroids();
        
            for (size_t i = 0; i < testD.features.size(); ++i) {
                int cluster = assignments[i];
                double dist = 0.0;
                for (size_t j = 0; j < testD.features[i].size(); ++j) {
                    double diff = toDouble(testD.features[i][j]) - centroids[cluster][j];
                    dist += diff * diff;
                }
                inertia += dist;
            }
        }
        else if(modelName == "decision_tree"){
            vector<double> yTrue;
            for (auto &s : testD.target) yTrue.push_back(toDouble(s));
            accuracy = computeAccuracy(yTrue, preds);
            precision = computePrecision(yTrue, preds);
            recall = computeRecall(yTrue, preds);
            f1 = computeF1Score(yTrue, preds);
        }
        model->plot(testD); // Plot the model's predictions
        // 7) Write JSON to metrics.json
        ofstream js("metrics.json");
        js << "{\n";
        js << "  \"time_ms\": "   << time_ms    << ",\n";
        js << "  \"memory_kb\": " << peakRSS_kb << ",\n";

        if (modelName == "k_means_clustering") {
            js << "  \"inertia\": " << inertia << ",\n";
        } 
        else if (modelName == "linear_regression") {
            js << "  \"r2\": "  << r2*100   << ",\n";
            js << "  \"mse\": "  << mse   << ",\n";
            // js << "  \"mse\": "       << mse            << "\n";
        } 
        else {
            js << "  \"accuracy\": "  << accuracy*100   << ",\n";
            js << "  \"precision\": " << precision*100  << ",\n";
            js << "  \"recall\": "    << recall*100     << ",\n";
            js << "  \"f1_score\": "  << f1*100         << ",\n";
        }

        // yaha pe change kiya hai dekh lena
        // Read the image file and encode it in base64
        string filename = "../imgs/graph.png";
        vector<unsigned char> image_data = read_file_binary(filename);
        string encoded_string = base64_encode(image_data);
        js << "  \"img\": \"" << encoded_string << "\"\n";

        js << "}\n";
        js.close();

        // Cleanup
        delete[] static_cast<double*>(rawTheta);
        delete model;

        cout << "Metrics written to metrics.json\n";
        return 0;
    }
    catch (const exception &ex) {
        cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
    
}
