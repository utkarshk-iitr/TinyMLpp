#!/bin/bash
cd /home/kavy/Projects/TinyMLpp

if [ "$1" = "-r" ]; then
    ./TinyMLppGUI
    exit 0
fi

# Generate Meta-Object code from header
moc test/MainWindow.h -o test/moc_MainWindow.cpp

# Compile everything
g++ -std=c++20 -fPIC \
    test/GUI.cpp \
    test/MainWindow.cpp \
    test/moc_MainWindow.cpp \
    src/data_handling.cpp \
    src/logistic_regression.cpp \
    src/knn.cpp \
    src/k_means_clustering.cpp \
    -Igui \
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

# Run the app
./TinyMLppGUI
