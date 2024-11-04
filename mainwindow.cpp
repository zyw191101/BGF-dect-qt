#include "mainwindow.h"
#include <QFormLayout>
#include <QGroupBox>
#include <QDebug>  // 确保包含 QDebug 头文件

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), isDisplaying(false)
{
    setupUI();

    connect(selectVideoButton, &QPushButton::clicked, this, &MainWindow::onSelectVideoButtonClicked);
    connect(toggleDisplayButton, &QPushButton::clicked, this, &MainWindow::onToggleDisplayClicked);

    connectSlidersToLabels();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{
    selectVideoButton = new QPushButton("Select Video", this);
    videoFilePathLabel = new QLabel("No video selected", this);
    toggleDisplayButton = new QPushButton("Start Display", this);

    backgroundUpdateRateSlider = new QSlider(Qt::Horizontal, this);
    backgroundUpdateRateSlider->setRange(0, 100);
    backgroundUpdateRateSlider->setValue(90);
    backgroundUpdateRateLabel = new QLabel("Background Update Rate: 0.9", this);

    alphaSlider = new QSlider(Qt::Horizontal, this);
    alphaSlider->setRange(0, 100);
    alphaSlider->setValue(70);
    alphaLabel = new QLabel("Alpha: 0.7", this);

    betaSlider = new QSlider(Qt::Horizontal, this);
    betaSlider->setRange(0, 100);
    betaSlider->setValue(90);
    betaLabel = new QLabel("Beta: 0.9", this);

    cvThresholdSlider = new QSlider(Qt::Horizontal, this);
    cvThresholdSlider->setRange(1, 254);
    cvThresholdSlider->setValue(25);
    cvThresholdLabel = new QLabel("CV Threshold: 25", this);

    meanStdDevSlider = new QSlider(Qt::Horizontal, this);
    meanStdDevSlider->setRange(1, 254);
    meanStdDevSlider->setValue(40);
    meanStdDevLabel = new QLabel("Mean Std Dev Data: 40", this);

    // 新增 fw 和 fh 的滑块和标签
    fwSlider = new QSlider(Qt::Horizontal, this);
    fwSlider->setRange(0, 100);
    fwSlider->setValue(5);
    fwLabel = new QLabel("Min Width (fw): 5", this);

    fhSlider = new QSlider(Qt::Horizontal, this);
    fhSlider->setRange(0, 100);
    fhSlider->setValue(5);
    fhLabel = new QLabel("Min Height (fh): 5", this);

    // 新增同时控制 fw 和 fh 的滑块
    fwfhSlider = new QSlider(Qt::Horizontal, this);
    fwfhSlider->setRange(0, 100);
    fwfhSlider->setValue(5);
    fwfhLabel = new QLabel("Min Width and Height (fw=fh): 5", this);

    // 新增 fwMax 和 fhMax 的滑块和标签
    fwMaxSlider = new QSlider(Qt::Horizontal, this);
    fwMaxSlider->setRange(0, 100);
    fwMaxSlider->setValue(20);
    fwMaxLabel = new QLabel("Max Width (fw): 20", this);

    fhMaxSlider = new QSlider(Qt::Horizontal, this);
    fhMaxSlider->setRange(0, 100);
    fhMaxSlider->setValue(20);
    fhMaxLabel = new QLabel("Max Height (fh): 20", this);

    // 新增同时控制 fw 和 fh 的滑块
    fwfhMaxSlider = new QSlider(Qt::Horizontal, this);
    fwfhMaxSlider->setRange(0, 100);
    fwfhMaxSlider->setValue(20);
    fwfhMaxLabel = new QLabel("Max Width and Height (fw=fh): 20", this);

    QFormLayout *paramLayout = new QFormLayout;
    paramLayout->addRow(backgroundUpdateRateLabel, backgroundUpdateRateSlider);
    paramLayout->addRow(alphaLabel, alphaSlider);
    paramLayout->addRow(betaLabel, betaSlider);
    paramLayout->addRow(cvThresholdLabel, cvThresholdSlider);
    paramLayout->addRow(meanStdDevLabel, meanStdDevSlider);

    // 添加新增加的 fw 和 fh 控件
    paramLayout->addRow(fwLabel, fwSlider);
    paramLayout->addRow(fhLabel, fhSlider);
    paramLayout->addRow(fwfhLabel, fwfhSlider);

    // 添加新增加的 fwMax 和 fhMax 控件
    paramLayout->addRow(fwMaxLabel, fwMaxSlider);
    paramLayout->addRow(fhMaxLabel, fhMaxSlider);
    paramLayout->addRow(fwfhMaxLabel, fwfhMaxSlider);

    QGroupBox *paramGroupBox = new QGroupBox("Parameter Settings", this);
    paramGroupBox->setLayout(paramLayout);

    QHBoxLayout *videoLayout = new QHBoxLayout;
    videoLayout->addWidget(selectVideoButton);
    videoLayout->addWidget(videoFilePathLabel);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(videoLayout);
    mainLayout->addWidget(paramGroupBox);
    mainLayout->addStretch();
    mainLayout->addWidget(toggleDisplayButton);

    QWidget *controlPanel = new QWidget(this);
    controlPanel->setLayout(mainLayout);

    setCentralWidget(controlPanel);
}

void MainWindow::connectSlidersToLabels()
{
    connect(backgroundUpdateRateSlider, &QSlider::valueChanged, this, [this](int value) {
        backgroundUpdateRateLabel->setText(QString("Background Update Rate: %1").arg(value / 100.0f));
    });

    connect(alphaSlider, &QSlider::valueChanged, this, [this](int value) {
        alphaLabel->setText(QString("Alpha: %1").arg(value / 100.0f));
    });

    connect(betaSlider, &QSlider::valueChanged, this, [this](int value) {
        betaLabel->setText(QString("Beta: %1").arg(value / 100.0f));
    });

    connect(cvThresholdSlider, &QSlider::valueChanged, this, [this](int value) {
        cvThresholdLabel->setText(QString("CV Threshold: %1").arg(value));
    });

    connect(meanStdDevSlider, &QSlider::valueChanged, this, [this](int value) {
        meanStdDevLabel->setText(QString("Mean Std Dev Data: %1").arg(value));
    });
    // 连接 fw 和 fh 滑块
    connect(fwSlider, &QSlider::valueChanged, this, [this](int value) {
        fwLabel->setText(QString("Min Width (fw): %1").arg(value));
    });

    connect(fhSlider, &QSlider::valueChanged, this, [this](int value) {
        fhLabel->setText(QString("Min Height (fh): %1").arg(value));
    });

    // 连接同步控制的 fw=fh 滑块
    connect(fwfhSlider, &QSlider::valueChanged, this, [this](int value) {
        fwfhLabel->setText(QString("Min Width and Height (fw=fh): %1").arg(value));

        // 同时更新 fw 和 fh 的滑块
        fwSlider->setValue(value);
        fhSlider->setValue(value);
    });

    // 连接 fwMax 和 fhMax 滑块
    connect(fwMaxSlider, &QSlider::valueChanged, this, [this](int value) {
        fwMaxLabel->setText(QString("Max Width (fw): %1").arg(value));
    });

    connect(fhMaxSlider, &QSlider::valueChanged, this, [this](int value) {
        fhMaxLabel->setText(QString("Max Height (fh): %1").arg(value));
    });

    // 连接同步控制的 fwMax=fhMax 滑块
    connect(fwfhMaxSlider, &QSlider::valueChanged, this, [this](int value) {
        fwfhMaxLabel->setText(QString("Max Width and Height (fw=fh): %1").arg(value));

        // 同时更新 fwMax 和 fhMax 的滑块
        fwMaxSlider->setValue(value);
        fhMaxSlider->setValue(value);
    });
}

void MainWindow::onSelectVideoButtonClicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Select Video File", "", "Video Files (*.mp4 *.avi *.mkv)");
    if (!file.isEmpty())
    {
        videoFilePath = file;
        videoFilePathLabel->setText(videoFilePath);
    }
    else
    {
        videoFilePathLabel->setText("No video selected");
    }
}

void MainWindow::onToggleDisplayClicked()
{
    if (isDisplaying)
    {
        isDisplaying = false;
        toggleDisplayButton->setText("Start Display");
        cv::destroyAllWindows();
    }
    else
    {
        if (videoFilePath.isEmpty())
        {
            qDebug() << "No video file selected!";
            return;
        }

        cv::VideoCapture cap(videoFilePath.toStdString());
        if (!cap.isOpened())
        {
            qDebug() << "Error: Unable to open the video";
            return;
        }

        isDisplaying = true;
        toggleDisplayButton->setText("Stop Display");

        // 创建MOG2背景减除器
       cv::Ptr<cv::BackgroundSubtractor> pBackSub = cv::createBackgroundSubtractorMOG2();
       cv::Ptr<cv::BackgroundSubtractorMOG2> mog = cv::createBackgroundSubtractorMOG2(50, 8);
       BackgroundlDetect backgroundlDetect; // 直接作为成员变量
       cv::Mat save = cv::Mat::zeros(544, 960, CV_8U);
       // Display the result in a window
       cv::namedWindow("display1", cv::WINDOW_NORMAL);
       cv::resizeWindow("display1", 1280, 1024);
        while (isDisplaying)
        {
            cv::Mat frame;
            cap >> frame;

            if (frame.empty())
            {
                cap.set(cv::CAP_PROP_POS_FRAMES, 0);
                continue;
            }

            float background_update_rate = getBackgroundUpdateRate();
            float alpha = getAlpha();
            float beta = getBeta();
            int cv_threshold = getCvThreshold();
            int meanStdDev_data = getMeanStdDevData();
            int minw=getMinFw();
            int minh=getMinFh();
            int maxw=getMaxFw();
            int maxh=getMaxFh();
            cv::resize(frame, frame, cv::Size(1920, 1080));
            cv::resize(frame, frame, cv::Size(1920, 1080));  // 保持彩色帧
            cv::Mat grayFrame;
            cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);  // 转换为灰度用于处理

            int crop_width = 960;
            int crop_height = 544;
            int x = (frame.cols - crop_width) / 2;
            int y = (frame.rows - crop_height) / 2;
            cv::Rect roi(x, y, crop_width, crop_height);
            cv::Mat cropped_frame = grayFrame(roi);  // 灰度图用于检测

            std::vector<BoxInfo> ret = backgroundlDetect.backgroundInference(
                cropped_frame, &save, mog, background_update_rate, alpha, beta, cv_threshold, meanStdDev_data,minw,minh,maxw,maxh);


             // 在原始彩色帧上绘制检测框
            for (const auto& box : ret)
            {
                cv::Point topLeft((int)box.x1 + x, (int)box.y1 + y);  // 转换为原始图像坐标
                cv::Point bottomRight((int)box.x2 + x, (int)box.y2 + y);

                // 使用红色或绿色框绘制（根据检测结果）
                cv::Scalar boxColor = box.is_low ? cv::Scalar(0, 0, 255) : cv::Scalar(0, 255, 0);

                // 在彩色图像上绘制框
                cv::rectangle(frame, topLeft, bottomRight, boxColor, 2);

//                // Display class ID and score on the bounding box (optional)
//                std::string label = "Class: " + std::to_string(box.class_id) +
//                                    " Score: " + std::to_string(box.score);

//                // Calculate label position (top-left corner of the bounding box)
//                int baseLine;
//                cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
//                cv::Point labelOrigin(topLeft.x, std::max(topLeft.y - 5, labelSize.height));

//                // Draw a filled rectangle behind the label to improve visibility
//                cv::rectangle(cropped_frame,
//                              labelOrigin + cv::Point(0, baseLine),
//                              labelOrigin + cv::Point(labelSize.width, -labelSize.height),
//                              boxColor, cv::FILLED);

//                // Put the label text on the frame
//                cv::putText(cropped_frame, label, labelOrigin,
//                            cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
            }

            // Clone the frame with bounding boxes for display
            cv::Mat show_frame_with_boxes = frame.clone();
            cv::imshow("display1", show_frame_with_boxes);
            cv::waitKey(1);  // Wait for 1 ms for a key press
        }

        cv::destroyAllWindows();
        isDisplaying = false;
        toggleDisplayButton->setText("Start Display");
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    isDisplaying = false;
    cv::destroyAllWindows();
    QMainWindow::closeEvent(event);
}

QString MainWindow::getVideoFilePath() const { return videoFilePath; }
float MainWindow::getBackgroundUpdateRate() const { return backgroundUpdateRateSlider->value() / 100.0f; }
float MainWindow::getAlpha() const { return alphaSlider->value() / 100.0f; }
float MainWindow::getBeta() const { return betaSlider->value() / 100.0f; }
int MainWindow::getCvThreshold() const { return cvThresholdSlider->value(); }
int MainWindow::getMeanStdDevData() const { return meanStdDevSlider->value(); }
int MainWindow::getMinFw() const{    return fwSlider->value();}
int MainWindow::getMinFh() const{   return fhSlider->value();}

int MainWindow::getMaxFw() const{    return fwMaxSlider->value();}
int MainWindow::getMaxFh() const{   return fhMaxSlider->value();}
