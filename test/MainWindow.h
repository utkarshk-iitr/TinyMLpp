// MainWindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QString>
#include "../src/data_handling.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // File operations
    void onOpenFile();
    void onShowData();

    // Data preprocessing
    void onNormalizeData();
    void onStandardizeData();

    // Machine learning operations
    void onPredict(const QString &algo);
    void onEvaluate(const QString &algo);

private:
    std::string currentFile;     // Path to currently loaded CSV
    handle::Data data;           // Data container from TinyML++

    QTextEdit *outputArea = nullptr; // Output display widget
};

#endif // MAINWINDOW_H
