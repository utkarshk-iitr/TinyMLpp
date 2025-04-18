#include "MainWindow.h"
#include <QFileDialog>
#include <QString>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <exception>
#include <QApplication>
#include <QStyleFactory>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // --- Window Settings ---
    setWindowTitle("TinyML++");
    showMaximized(); // Fullscreen

    // --- Central Widget & Main Layout ---
    auto *central = new QWidget(this);
    auto *mainLayout = new QHBoxLayout(central); // horizontal split

    // --- Left Controls Panel ---
    auto *controlsLayout = new QVBoxLayout();

    auto *openFileButton     = new QPushButton("📂 Open CSV File", this);
    auto *showDataButton     = new QPushButton("📊 Show Data", this);
    auto *normalizeButton    = new QPushButton("📈 Normalize (Min-Max)", this);
    auto *standardizeButton  = new QPushButton("📉 Standardize (Z-Score)", this);

    auto *algoLabel = new QLabel("🧠 Select Algorithm:", this);
    auto *algoCombo = new QComboBox(this);
    algoCombo->addItems({
        "K-Means",
        "Linear Regression",
        "Logistic Regression",
        "KNN"
    });

    auto *predictButton  = new QPushButton("⚙️ Predict", this);
    auto *evaluateButton = new QPushButton("📐 Evaluate", this);

    // Add controls to left layout
    controlsLayout->addWidget(openFileButton);
    controlsLayout->addWidget(showDataButton);
    controlsLayout->addWidget(normalizeButton);
    controlsLayout->addWidget(standardizeButton);
    controlsLayout->addSpacing(10);
    controlsLayout->addWidget(algoLabel);
    controlsLayout->addWidget(algoCombo);
    controlsLayout->addSpacing(10);
    controlsLayout->addWidget(predictButton);
    controlsLayout->addWidget(evaluateButton);
    controlsLayout->addStretch(); // Push everything up

    // --- Right Output Area ---
    outputArea = new QTextEdit(this);
    outputArea->setReadOnly(true);

    // --- Assemble Layout ---
    mainLayout->addLayout(controlsLayout, 1);   // 1/3 of space
    mainLayout->addWidget(outputArea, 2);       // 2/3 of space

    setCentralWidget(central);

    // --- Dark Mode Styling ---
    qApp->setStyle(QStyleFactory::create("Fusion"));
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(30, 30, 30));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(30, 30, 30));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(45, 45, 45));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    qApp->setPalette(darkPalette);

    // --- Signal-Slot Connections ---
    connect(openFileButton,    &QPushButton::clicked, this, &MainWindow::onOpenFile);
    connect(showDataButton,    &QPushButton::clicked, this, &MainWindow::onShowData);
    connect(normalizeButton,   &QPushButton::clicked, this, &MainWindow::onNormalizeData);
    connect(standardizeButton, &QPushButton::clicked, this, &MainWindow::onStandardizeData);

    connect(predictButton, &QPushButton::clicked, this, [=]() {
        onPredict(algoCombo->currentText());
    });

    connect(evaluateButton, &QPushButton::clicked, this, [=]() {
        onEvaluate(algoCombo->currentText());
    });
}

MainWindow::~MainWindow() {}


// --- File Open Handler ---
void MainWindow::onOpenFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open CSV", "", "CSV Files (*.csv)");
    if (!filePath.isEmpty()) {
        currentFile = filePath.toStdString();
        try {
            data = handle::readCSV(currentFile);
            outputArea->append("✅ Loaded file: " + filePath);
        } catch (const std::exception &e) {
            outputArea->append("❌ Error: " + QString::fromStdString(e.what()));
        }
    }
}

// --- Display Data Preview ---
void MainWindow::onShowData()
{
    if (data.features.empty()) {
        outputArea->append("⚠️ No data loaded.");
        return;
    }

    outputArea->append(QString("📄 Previewing first rows (%1 total rows)").arg(data.features.size()));
    int rowsToPrint = std::min<int>(10, data.features.size());

    for (int i = 0; i < rowsToPrint; ++i) {
        QString row;
        for (const auto &cell : data.features[i]) {
            row += QString::fromStdString(cell) + "\t";
        }
        outputArea->append(row);
    }
}

// --- Normalize Handler ---
void MainWindow::onNormalizeData()
{
    if (data.features.empty()) {
        outputArea->append("⚠️ No data to normalize.");
        return;
    }
    handle::minMaxNormalize(data);
    outputArea->append("✅ Data normalized using Min-Max scaling.");
}

// --- Standardize Handler ---
void MainWindow::onStandardizeData()
{
    if (data.features.empty()) {
        outputArea->append("⚠️ No data to standardize.");
        return;
    }
    handle::standardize(data);
    outputArea->append("✅ Data standardized using Z-Score.");
}

// --- Prediction Handler ---
void MainWindow::onPredict(const QString &algo)
{
    if (data.features.empty()) {
        outputArea->append("⚠️ No data available for prediction.");
        return;
    }

    outputArea->append("🔮 Predicting with algorithm: " + algo);

    // TODO: Add prediction logic per algorithm.
}

// --- Evaluation Handler ---
void MainWindow::onEvaluate(const QString &algo)
{
    if (data.features.empty()) {
        outputArea->append("⚠️ No data available for evaluation.");
        return;
    }

    outputArea->append("📏 Evaluating model with algorithm: " + algo);

    // TODO: Add evaluation logic per algorithm.
}
