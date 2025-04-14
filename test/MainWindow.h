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
    void onOpenFile();
    void onShowData();
    void onNormalizeData();
    void onStandardizeData();
    void onPredict(const QString &algo);
    void onEvaluate(const QString &algo);

private:
    std::string currentFile;
    handle::Data data;
    QTextEdit *outputArea = nullptr;
};

#endif // MAINWINDOW_H
