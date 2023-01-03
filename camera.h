#ifndef CAMERA_H
#define CAMERA_H
#include <QThread>
#include <globalvars.h>
#include <ASICamera2.h>
#include "opencv2/highgui/highgui_c.h"

class astroCam : public QThread
{
public:
    astroCam();
    ~astroCam();
    void run();
    void openCamFeed(char *DarkBMPPath);

    unsigned long GetTickCount();
    int binMultiple;
    int exposure;
    int bDisplay;

    int bChangeFormat;
    int imageType;
    int width;
    int height;
    void shutCamDown();

private:
    ASI_EXPOSURE_STATUS status;

    long imgSize;
    long displaySize;
    int iDropped;
    int displayHei;
    int displayWid;
    int iStrLen, iTextX, iTextY;
    unsigned char* imgBuf;


    enum CHANGE
    {
            change_imagetype = 0,
            change_bin,
            change_size_bigger,
            change_size_smaller
    };

    CHANGE change;
    std::string camInfo;
    std::string darkInfo;

    int time1,time2;
    int count;
    char buf[128];
    int CamNum;

    IplImage *pRgb;

    int numDevices;

    char *bayer[4] = {"RG","BG","GR","GB"};
    char *bayerPattern[4];


};
#endif // CAMERA_H
