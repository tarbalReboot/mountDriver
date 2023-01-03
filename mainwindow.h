#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QCameraInfo>
#include <QCameraImageCapture>
#include <QMediaRecorder>
#include <camera.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static void stopAxis(int axis);
    void keepDriving(double rA, double dec);
    void motorSetup();

    int motorPower1;
    int motorPower2;

    int PowerClamp(int input);

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_haltButton_clicked();

    void on_trackButton_clicked();

    void on_upButton_clicked();

    void on_downButton_clicked();

    void on_rightButton_clicked();

    void on_leftButton_clicked();

    void setCamera(const QCameraInfo &cameraInfo);

    void record();
    void pause();
    void stop();

    void takeImage();
    void displayCaptureError(int, QCameraImageCapture::Error, const QString &errorString);

    void displayRecorderError();
    void displayCameraError();

    void setExposureCompensation(int index);

    void readyForCapture(bool ready);
    void imageSaved(int id, const QString &fileName);


    void on_toggleCamera_clicked();

private:
    Ui::MainWindow *ui;

    astroCam camFeed;

    QLineEdit *NGCCode;
    QLineEdit *ICCode;
    QLineEdit *MCode;

    QPushButton *upButton;
    QPushButton *downButton;
    QPushButton *leftButton;
    QPushButton *rightButton;
    QPushButton *slewButton;
    QPushButton *trackButton;
    QPushButton *clearButton;
    QPushButton *updateButton;
    QPushButton *stopButton;

    QCamera *camera = nullptr;
    QCameraImageCapture *imageCapture = nullptr;
    QMediaRecorder* mediaRecorder = nullptr;

    QImageEncoderSettings imageSettings;
    QVideoEncoderSettings videoSettings;
    QString videoContainerFormat;
    bool isCapturingImage = false;
    bool applicationExiting = false;
};


#endif // MAINWINDOW_H
