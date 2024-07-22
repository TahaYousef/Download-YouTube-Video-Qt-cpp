#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QProcess>
#include <QComboBox>
#include <QProgressBar>
#include <QStatusBar>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void downloadVideo();
    void handleProcessOutput();
    void handleProcessError();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void setupUi();
    void setupConnections();

    QLineEdit *urlInput;
    QComboBox *qualityComboBox;
    QLabel *outputLabel;
    QProcess *process;
    QString outputDirectory;
    QProgressBar *progressBar;
    QStatusBar *statusBar;
    QPushButton *downloadButton;
};

#endif // MAINWINDOW_H
