#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFileDialog>
#include <opencv2/opencv.hpp>
#include <opencv2/bgsegm.hpp>
#include "BackgroundlDetect.h"  // 假设你有这个类

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString getVideoFilePath() const;

    float getBackgroundUpdateRate() const;
    float getAlpha() const;
    float getBeta() const;
    int getCvThreshold() const;
    int getMeanStdDevData() const;
    int getMinFw() const;
    int getMinFh() const;
    int getMaxFw() const;
    int getMaxFh() const;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onSelectVideoButtonClicked();
    void onToggleDisplayClicked();

private:
    QPushButton *selectVideoButton;
    QPushButton *toggleDisplayButton;
    QLabel *videoFilePathLabel;

    QString videoFilePath;
    bool isDisplaying;

    QSlider *backgroundUpdateRateSlider;
    QLabel *backgroundUpdateRateLabel;
    QSlider *alphaSlider;
    QLabel *alphaLabel;
    QSlider *betaSlider;
    QLabel *betaLabel;
    QSlider *cvThresholdSlider;
    QLabel *cvThresholdLabel;
    QSlider *meanStdDevSlider;
    QLabel *meanStdDevLabel;

    QSlider *fwSlider;
    QLabel *fwLabel;
    QSlider *fhSlider;
    QLabel *fhLabel;
    QSlider *fwfhSlider;
    QLabel *fwfhLabel;
    QSlider *fwMaxSlider;
    QLabel *fwMaxLabel;
    QSlider *fhMaxSlider;
    QLabel *fhMaxLabel;
    QSlider *fwfhMaxSlider;
    QLabel *fwfhMaxLabel;

    void setupUI();
    void connectSlidersToLabels();
};

#endif // MAINWINDOW_H
