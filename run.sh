#!/bin/bash
set -e  # Exit immediately on any error

cd /home/kavy/Projects/TinyMLpp || exit 1

if [ "$1" = "-r" ]; then
    ./TinyMLppGUI
    exit 0
fi

echo "Generating Qt MOC..."
moc test/MainWindow.h -o test/moc_MainWindow.cpp

echo "Compiling project..."
g++ -std=c++20 -fPIC \
    test/GUI.cpp \
    test/MainWindow.cpp \
    test/moc_MainWindow.cpp \
    src/data_handling.cpp \
    src/logistic_regression.cpp \
    src/linear_regression.cpp \
    src/knn.cpp \
    src/k_means_clustering.cpp \
    -Itest \
    -Isrc \
    -I/usr/include/python3.10 \
    -I/usr/lib/python3/dist-packages/numpy/core/include \
    -I/usr/include/x86_64-linux-gnu/qt5 \
    -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets \
    -I/usr/include/x86_64-linux-gnu/qt5/QtCore \
    $(pkg-config --cflags --libs Qt5Widgets) \
    -L/usr/lib/python3.10/config-3.10-x86_64-linux-gnu \
    -lpython3.10 \
    -o TinyMLppGUI

echo "Build successful! Running the app..."
./TinyMLppGUI
