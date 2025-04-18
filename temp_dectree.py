# coding: utf-8
import sys
import pandas as pd
from sklearn.tree import DecisionTreeRegressor

if __name__ == '__main__':
    fn = sys.argv[1] if len(sys.argv)>1 else 'toy.csv'
    df = pd.read_csv(fn)
    X = df.iloc[:, :-1].values
    y = df.iloc[:, -1].values
    model = DecisionTreeRegressor(max_depth=5, min_samples_split=2, random_state=42)
    model.fit(X, y)
    preds = model.predict(X)
    print(" ".join(map(str, preds)))
