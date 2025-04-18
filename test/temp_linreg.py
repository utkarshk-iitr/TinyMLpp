#!/usr/bin/env python3
import sys
import pandas as pd
from sklearn.linear_model import LinearRegression

def read_csv(filename):
    data = pd.read_csv(filename)
    return data

if __name__ == "__main__":
    filename = sys.argv[1] if len(sys.argv) > 1 else "advertising.csv"
    data = read_csv(filename)
    # Assume last column is the target, remaining columns are features.
    X = data.iloc[:, :-1].values
    y = data.iloc[:, -1].values
    # Create and train the linear regression model.
    model = LinearRegression()
    model.fit(X, y)
    # Predict using the trained model.
    predictions = model.predict(X)
    # Output the predictions as whitespace-separated values.
    print(" ".join(map(str, predictions)))
