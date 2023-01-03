#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <pigpiod_if2.h>
#include <globalvars.h>
#include <cmath>
#include <iostream>
#include <QCameraViewfinder>
#include <QCameraInfo>
#include <QMediaRecorder>
#include <QMessageBox>
#include <QMediaMetaData>
#include <QDir>
#include <camera.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    motorSetup();
    ui->setupUi(this);
//    setCamera(QCameraInfo::defaultCamera());
//    astroCam camFeed;
}

MainWindow::~MainWindow()
{
    tracking = false;
    programRunning = false;
    stopAxis(whichAxisA);
    stopAxis(whichAxisB);
    pigpio_stop(piNumber);
//    delete mediaRecorder;
//    delete imageCapture;
//    delete camera;
    delete ui;
}

void MainWindow::setCamera(const QCameraInfo &cameraInfo)
{
    delete imageCapture;
    delete mediaRecorder;
    delete camera;

    camera = new QCamera(cameraInfo);

    connect(camera, &QCamera::stateChanged, this, &MainWindow::on_toggleCamera_clicked);
    connect(camera, QOverload<QCamera::Error>::of(&QCamera::error), this, &MainWindow::displayCameraError);

//    mediaRecorder = new QMediaRecorder(camera);
//    connect(mediaRecorder, &QMediaRecorder::stateChanged, this, &MainWindow::toggleCamera);

    imageCapture = new QCameraImageCapture(camera);

//    connect(mediaRecorder, QOverload<QMediaRecorder::Error>::of(&QMediaRecorder::error),
//            this, &MainWindow::displayRecorderError);

//    mediaRecorder->setMetaData(QMediaMetaData::Title, QVariant(QLatin1String("Test Title")));

    connect(ui->exposureCompensation, &QAbstractSlider::valueChanged, this, &MainWindow::setExposureCompensation);

    camera->setViewfinder(ui->viewfinder);

    connect(imageCapture, &QCameraImageCapture::readyForCaptureChanged, this, &MainWindow::readyForCapture);
//    connect(imageCapture, &QCameraImageCapture::imageCaptured, this, &MainWindow::processCapturedImage);
    connect(imageCapture, &QCameraImageCapture::imageSaved, this, &MainWindow::imageSaved);
    connect(imageCapture, QOverload<int, QCameraImageCapture::Error, const QString &>::of(&QCameraImageCapture::error),
            this, &MainWindow::displayCaptureError);

    camera->start();
}

void MainWindow::record()
{
    mediaRecorder->record();
}

void MainWindow::pause()
{
    mediaRecorder->pause();
}

void MainWindow::stop()
{
    mediaRecorder->stop();
}

void MainWindow::takeImage()
{
    isCapturingImage = true;
    imageCapture->capture();
}

void MainWindow::displayCaptureError(int id, const QCameraImageCapture::Error error, const QString &errorString)
{
    Q_UNUSED(id);
    Q_UNUSED(error);
    QMessageBox::warning(this, tr("Image Capture Error"), errorString);
    isCapturingImage = false;
}

void MainWindow::on_toggleCamera_clicked()
{
    std::cout << "test 1" << std::endl;
/*
    if(!camRunning)
    {
        camFeed.binMultiple = 2;
        camFeed.exposure = 4000;
        camFeed.bDisplay = 0;
        camFeed.bChangeFormat = 2;
        camFeed.width = 1920;
        camFeed.height = 1080;

        camFeed.start(QThread::HighestPriority);

        camera->start();
        camRunning = true;
        ui->toggleCamera->setStyleSheet("QPushButton {color: green;}");
    }

    else
    {
        camRunning = false;
        camera->stop();
        camera->unload();
        camFeed.shutCamDown();
        ui->toggleCamera->setStyleSheet("QPushButton {color: black;}");
    }
    */
}

void MainWindow::setExposureCompensation(int index)
{
    camera->exposure()->setExposureCompensation(index*0.5);
}

void MainWindow::displayRecorderError()
{
    QMessageBox::warning(this, tr("Capture Error"), mediaRecorder->errorString());
}

void MainWindow::displayCameraError()
{
    QMessageBox::warning(this, tr("Camera Error"), camera->errorString());
}

void MainWindow::readyForCapture(bool ready)
{
    ui->capture->setEnabled(ready);
}

void MainWindow::imageSaved(int id, const QString &fileName)
{
    Q_UNUSED(id);
    ui->statusBar->showMessage(tr("Captured \"%1\"").arg(QDir::toNativeSeparators(fileName)));

    isCapturingImage = false;
    if (applicationExiting)
        close();
}

void MainWindow::on_trackButton_clicked()
{
    stopAxis(whichAxisA);
    stopAxis(whichAxisB);
    errSumAz = 0;
    errSumAlt = 0;

    if(!tracking)
    {
        tracking = true;
        lastDirectionAz = 1;
        lastDirectionAlt = 1;

        ui->trackButton->setStyleSheet("QPushButton {color: green;}");
        rAscension = (ui->RAFieldHour->text().toDouble() + (ui->RAFieldMin->text().toDouble()/60) + (ui->RAFieldSec->text().toDouble()/3600)) * 3.141592654 / 12;
        declination = (ui->DecFieldDeg->text().toDouble() + (ui->DecFieldMin->text().toDouble()/60) + (ui->DecFieldSec->text().toDouble()/3600)) * 3.141592654 / 180;
    }
    else
    {
        tracking = false;
        ui->trackButton->setStyleSheet("QPushButton {color: black;}");
    }
}

void MainWindow::motorSetup()
{
    set_mode(piNumber, pinInclineA, PI_OUTPUT);
    set_mode(piNumber, pinInclineB, PI_OUTPUT);
    set_mode(piNumber, pinRotateA, PI_OUTPUT);
    set_mode(piNumber, pinRotateB, PI_OUTPUT);

//    set_PWM_range(piNumber, pinInclineA, pwmRange);
//    set_PWM_range(piNumber, pinInclineB, pwmRange);
//    set_PWM_range(piNumber, pinRotateA, pwmRange);
//    set_PWM_range(piNumber, pinRotateB, pwmRange);
}


void MainWindow::stopAxis(int axis)
{
    if(axis == 0)
    {
        set_PWM_dutycycle(piNumber, pinRotateA, 0);
        set_PWM_dutycycle(piNumber, pinRotateB, 0);
    }
    if(axis == 1)
    {
        set_PWM_dutycycle(piNumber, pinInclineA, 0);
        set_PWM_dutycycle(piNumber, pinInclineB, 0);
    }
}

void MainWindow::on_upButton_clicked()
{
    if(!tracking)
        AltInc += incremental;
}

void MainWindow::on_downButton_clicked()
{
    if(!tracking)
        AltInc -= incremental;
}

void MainWindow::on_rightButton_clicked()
{
    if(!tracking)
        AzInc += incremental;
}

void MainWindow::on_leftButton_clicked()
{
    if(!tracking)
        AzInc -= incremental;
}

void MainWindow::on_haltButton_clicked()
{
    stopAxis(whichAxisA);
    stopAxis(whichAxisB);
    errSumAz = 0;
    errSumAlt = 0;

    AzInc = 0;
    AltInc = 0;

    std::cout << "Az: " << counterAz << " counts and " << angleAz << " degrees" << "    " << "Alt: " << counterAlt << " counts and " << angleAlt << " degrees" << std::endl;

    tracking = true;
    MainWindow::on_trackButton_clicked();
}
