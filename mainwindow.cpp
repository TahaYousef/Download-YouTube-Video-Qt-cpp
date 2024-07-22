#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDebug>
#include <QDir>
#include <QMenuBar>
#include <QFileDialog>
#include <QStyle>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), process(new QProcess(this)) {

    setupUi();
    setupConnections();
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUi() {
    QWidget *centralWidget = new QWidget(this);
    QGridLayout *layout = new QGridLayout(centralWidget);

    urlInput = new QLineEdit(this);
    urlInput->setPlaceholderText("Enter YouTube URL");
    layout->addWidget(new QLabel("YouTube URL:"), 0, 0);
    layout->addWidget(urlInput, 0, 1, 1, 3);

    qualityComboBox = new QComboBox(this);
    qualityComboBox->addItem("Best", "best");
    qualityComboBox->addItem("1080p", "bestvideo[height<=1080]+bestaudio/best[height<=1080]");
    qualityComboBox->addItem("720p", "bestvideo[height<=720]+bestaudio/best[height<=720]");
    qualityComboBox->addItem("480p", "bestvideo[height<=480]+bestaudio/best[height<=480]");
    qualityComboBox->addItem("360p", "bestvideo[height<=360]+bestaudio/best[height<=360]");
    layout->addWidget(new QLabel("Quality:"), 1, 0);
    layout->addWidget(qualityComboBox, 1, 1);

    downloadButton = new QPushButton("Download YouTube Video", this);
    downloadButton->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    downloadButton->setStyleSheet("QPushButton {"
                                  "background-color: #4CAF50;"
                                  "color: white;"
                                  "border: none;"
                                  "padding: 10px 24px;"
                                  "text-align: center;"
                                  "text-decoration: none;"
                                  "display: inline-block;"
                                  "font-size: 16px;"
                                  "margin: 4px 2px;"
                                  "cursor: pointer;"
                                  "border-radius: 12px;"
                                  "}"
                                  "QPushButton:hover {"
                                  "background-color: #45a049;"
                                  "}");
    layout->addWidget(downloadButton, 1, 2);

    outputLabel = new QLabel(this);
    layout->addWidget(outputLabel, 2, 0, 1, 3);

    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setVisible(false);
    layout->addWidget(progressBar, 3, 0, 1, 3);

    setCentralWidget(centralWidget);

    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);

    // Set the output directory to the Downloads folder
    outputDirectory = QDir::homePath() + "/Desktop/Downloads";
    QDir().mkpath(outputDirectory); // Ensure the directory exists

    // Menu bar
    QMenuBar *menuBar = new QMenuBar(this);
    QMenu *fileMenu = new QMenu("File", this);
    QAction *setOutputDirAction = new QAction("Set Output Directory", this);
    fileMenu->addAction(setOutputDirAction);
    menuBar->addMenu(fileMenu);
    setMenuBar(menuBar);

    connect(setOutputDirAction, &QAction::triggered, this, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Output Directory");
        if (!dir.isEmpty()) {
            outputDirectory = dir;
        }
    });
}

void MainWindow::setupConnections() {
    connect(downloadButton, &QPushButton::clicked, this, &MainWindow::downloadVideo);
    connect(process, &QProcess::readyReadStandardOutput, this, &MainWindow::handleProcessOutput);
    connect(process, &QProcess::readyReadStandardError, this, &MainWindow::handleProcessError);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MainWindow::processFinished);
}

void MainWindow::downloadVideo() {
    QString videoUrl = urlInput->text();
    if (videoUrl.isEmpty()) {
        outputLabel->setText("Please enter a YouTube URL.");
        return;
    }

    QString quality = qualityComboBox->currentData().toString();
    QString program = "yt-dlp"; // Ensure yt-dlp is in your system PATH
    QStringList arguments;
    arguments << videoUrl;
    arguments << "-f" << quality;
    arguments << "-o" << outputDirectory + "/%(title)s.%(ext)s";

    outputLabel->setText("Downloading...");
    progressBar->setVisible(true);
    progressBar->setValue(0);
    statusBar->showMessage("Downloading...");
    process->start(program, arguments);
}

void MainWindow::handleProcessOutput() {
    QByteArray output = process->readAllStandardOutput();
    QString outputString = QString::fromUtf8(output);
    qDebug() << outputString;

    // Regular expression to parse progress information
    QRegularExpression re(R"(.*?\[download\]\s+(\d+\.\d+)%.*)");
    QRegularExpressionMatch match = re.match(outputString);
    if (match.hasMatch()) {
        double progress = match.captured(1).toDouble();
        progressBar->setValue(static_cast<int>(progress));
    }
}

void MainWindow::handleProcessError() {
    QByteArray error = process->readAllStandardError();
    qDebug() << error;
    outputLabel->setText("Error occurred while downloading.");
    progressBar->setVisible(false);
    statusBar->showMessage("Error occurred while downloading.", 5000);
}

void MainWindow::processFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    progressBar->setVisible(false);
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        outputLabel->setText("Download completed successfully. File saved to: " + outputDirectory);
        statusBar->showMessage("Download completed successfully.", 5000);
    } else {
        outputLabel->setText("Download failed.");
        statusBar->showMessage("Download failed.", 5000);
    }
}
