#include "MainWindow.h"
#include <QFileDialog>
#include <QString>
#include <exception>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Central widget + layout.
    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);

    // Buttons for data operations.
    QPushButton *openFileButton = new QPushButton("Open CSV File", this);
    QPushButton *showDataButton = new QPushButton("Show Data", this);
    QPushButton *normalizeButton = new QPushButton("Normalize (Min-Max)", this);
    QPushButton *standardizeButton = new QPushButton("Standardize (Z-Score)", this);

    // Combo box for algorithm selection.
    QLabel *algoLabel = new QLabel("Select Algorithm:", this);
    QComboBox *algoCombo = new QComboBox(this);
    algoCombo->addItem("K-Means");
    algoCombo->addItem("Linear Regression");
    algoCombo->addItem("Logistic Regression");
    algoCombo->addItem("KNN");

    // Buttons for ML operations.
    QPushButton *predictButton = new QPushButton("Predict", this);
    QPushButton *evaluateButton = new QPushButton("Evaluate", this);

    // A text area to display output.
    outputArea = new QTextEdit(this);
    outputArea->setReadOnly(true);

    // Add widgets to layout.
    layout->addWidget(openFileButton);
    layout->addWidget(showDataButton);
    layout->addWidget(normalizeButton);
    layout->addWidget(standardizeButton);
    layout->addWidget(algoLabel);
    layout->addWidget(algoCombo);
    layout->addWidget(predictButton);
    layout->addWidget(evaluateButton);
    layout->addWidget(outputArea);

    setCentralWidget(central);

    // Connect signals and slots.
    connect(openFileButton, &QPushButton::clicked, this, &MainWindow::onOpenFile);
    connect(showDataButton, &QPushButton::clicked, this, &MainWindow::onShowData);
    connect(normalizeButton, &QPushButton::clicked, this, &MainWindow::onNormalizeData);
    connect(standardizeButton, &QPushButton::clicked, this, &MainWindow::onStandardizeData);
    connect(predictButton, &QPushButton::clicked, this, [this, algoCombo](){
        onPredict(algoCombo->currentText());
    });
    connect(evaluateButton, &QPushButton::clicked, this, [this, algoCombo](){
        onEvaluate(algoCombo->currentText());
    });
}

MainWindow::~MainWindow() {}

void MainWindow::onOpenFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open CSV", "", "CSV Files (*.csv)");
    if (!filePath.isEmpty()) {
        currentFile = filePath.toStdString();
        try {
            data = handle::readCSV(currentFile);
            outputArea->append("Loaded file: " + filePath);
        } catch (std::exception &e) {
            outputArea->append(QString("Error: %1").arg(e.what()));
        }
    }
}

void MainWindow::onShowData()
{
    if (data.features.empty()) {
        outputArea->append("No data loaded.");
        return;
    }

    int rowsToPrint = std::min(static_cast<int>(data.features.size()), 10);
    for (int i = 0; i < rowsToPrint; i++) {
        QString row;
        for (const auto &cell : data.features[i]) {
            row += QString::fromStdString(cell) + "\t";
        }
        outputArea->append(row);
    }
}

void MainWindow::onNormalizeData()
{
    if (data.features.empty()) {
        outputArea->append("No data to normalize.");
        return;
    }
    handle::minMaxNormalize(data);
    outputArea->append("Data has been normalized (Min-Max).");
}

void MainWindow::onStandardizeData()
{
    if (data.features.empty()) {
        outputArea->append("No data to standardize.");
        return;
    }
    handle::standardize(data);
    outputArea->append("Data has been standardized (Z-Score).");
}

void MainWindow::onPredict(const QString &algo)
{
    if (data.features.empty()) {
        outputArea->append("No data available for prediction.");
        return;
    }
    // TODO: Instantiate and predict using the chosen algorithm.
    outputArea->append("Prediction called using: " + algo);
}

void MainWindow::onEvaluate(const QString &algo)
{
    if (data.features.empty()) {
        outputArea->append("No data available for evaluation.");
        return;
    }
    // TODO: Evaluate the model's performance metrics.
    outputArea->append("Evaluation called for: " + algo);
}