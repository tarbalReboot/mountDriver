#include "stdio.h"
#include "opencv2/highgui/highgui_c.h"
#include "ASICamera2.h"
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "pthread.h"
#include <camera.h>

astroCam::astroCam()
{

    enum CHANGE{
            change_imagetype = 0,
            change_bin,
            change_size_bigger,
            change_size_smaller
    };

    buf[128]={0};

    numDevices = ASIGetNumOfConnectedCameras();
    CamNum = 0;
    iDropped = 0;
    displayHei = 252;
    displayWid = 462;
    imageType = ASI_IMG_RGB24;
    iStrLen = 0;
    iTextX = 40;
    iTextY = 60;
}

astroCam::~astroCam()
{
    astroCam::shutCamDown();
}

void astroCam::shutCamDown()
{
    ASIStopExposure(CamNum);
    ASICloseCamera(CamNum);
    if(imgBuf)
            delete[] imgBuf;
}

unsigned long astroCam::GetTickCount()
{

   struct timespec ts;
   clock_gettime(CLOCK_MONOTONIC,&ts);
   return (ts.tv_sec*1000 + ts.tv_nsec/(1000*1000));

}

void astroCam::run()
{
    astroCam::openCamFeed("/home/cosmo/Pictures/dark.bmp");
    imgSize = width*height*(1 + (imageType==ASI_IMG_RAW16));
    displaySize = displayWid*displayWid*(1 + (imageType==ASI_IMG_RAW16));

    while(camRunning)
    {
            ASIStartExposure(CamNum, ASI_FALSE);
            usleep(10000);//10ms
            status = ASI_EXP_WORKING;
            while(status == ASI_EXP_WORKING)
            {
                    ASIGetExpStatus(CamNum, &status);
            }
            if(status == ASI_EXP_SUCCESS)
            {
                    ASIGetDataAfterExp(CamNum, imgBuf, imgSize);
                    // sprintf(szTemp, "saveImage%d.jpg", bMain);

                    if(imageType==ASI_IMG_RAW16)
                    {
                            unsigned short *pCv16bit = (unsigned short *)(pRgb->imageData);
                            unsigned short *pImg16bit = (unsigned short *)imgBuf;
                            for(int y = 0; y < displayHei; y++)
                            {
                                    memcpy(pCv16bit, pImg16bit, displayWid*2);
                                    pCv16bit+=displayWid;
                                    pImg16bit+=width;
                            }
                    }
                    else{
                            unsigned char *pCv8bit = (unsigned char *)pRgb->imageData;
                            unsigned char *pImg8bit = (unsigned char *)imgBuf;
                            for(int y = 0; y < displayHei; y++)
                            {
                                    memcpy(pCv8bit, pImg8bit, displayWid);
                                    pCv8bit+=displayWid;
                                    pImg8bit+=width;
                            }
                    /*      if(bSave)
                                    cvSaveImage("saveImage.jpg", pRgb);
                            bSave = false;*/
                    }

            }
//                      ASIGetDataAfterExp(CamNum, (unsigned char*)pRgb->imageData, pRgb->imageSize);

            time2 = astroCam::GetTickCount();
            count++;

            if(time2-time1 > 1000 )
            {
                    ASIGetDroppedFrames(CamNum, &iDropped);

                    count = 0;
                    time1 = astroCam::GetTickCount();


            }
            if(imageType != ASI_IMG_RGB24 && imageType != ASI_IMG_RAW16)
            {
                    iStrLen = strlen(buf);
                    CvRect rect = cvRect(iTextX, iTextY - 15, iStrLen* 11, 20);
                    cvSetImageROI(pRgb , rect);
                    cvSet(pRgb, CV_RGB(180, 180, 180));
                    cvResetImageROI(pRgb);
            }

            pRgb=cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, 3);

    }

    astroCam::shutCamDown();
}

void astroCam::openCamFeed(char *DarkBMPPath)
{


        ASI_CAMERA_INFO ASICameraInfo;

        ASIGetCameraProperty(&ASICameraInfo, CamNum);
        camInfo = ASICameraInfo.Name;

        ASIInitCamera(CamNum);

        ASI_ERROR_CODE err = ASIEnableDarkSubtract(CamNum, DarkBMPPath);
        if(err == ASI_SUCCESS)
                darkInfo = "loading " + (std::string)DarkBMPPath + " succeeded\n";
        else
                darkInfo = "please take a dark frame image\n";

        int iMaxWidth, iMaxHeight;
        iMaxWidth = ASICameraInfo.MaxWidth;
        iMaxHeight =  ASICameraInfo.MaxHeight;
        bayerPattern[4] = bayer[ASICameraInfo.BayerPattern];

        ASI_CONTROL_CAPS ControlCaps;
        int iNumOfCtrl = 0;
        ASIGetNumOfControls(CamNum, &iNumOfCtrl);
        for(int i = 0; i < iNumOfCtrl; i++)
        {
                ASIGetControlCaps(CamNum, i, &ControlCaps);
                printf("%s\n", ControlCaps.Name);
        }

        long ltemp = 0;
        ASI_BOOL bAuto = ASI_FALSE;
        ASIGetControlValue(CamNum, ASI_TEMPERATURE, &ltemp, &bAuto);

        printf("sensor temperature:%02f\n", (float)ltemp/10.0);


        while(ASI_SUCCESS != ASISetROIFormat(CamNum, width, height, binMultiple, (ASI_IMG_TYPE)imageType));//IMG_RAW8

        ASIGetROIFormat(CamNum, &width, &height, &binMultiple, (ASI_IMG_TYPE*)&imageType);

        int displayWid = 1280, displayHei = 960;

        if(imageType == ASI_IMG_RAW16)
                pRgb=cvCreateImage(cvSize(displayWid, displayHei), IPL_DEPTH_16U, 1);
        else if(imageType == ASI_IMG_RGB24)
                pRgb=cvCreateImage(cvSize(displayWid, displayHei), IPL_DEPTH_8U, 3);
        else
                pRgb=cvCreateImage(cvSize(displayWid, displayHei), IPL_DEPTH_8U, 1);

        imgBuf = new unsigned char[imgSize];

        ASISetControlValue(CamNum, ASI_GAIN, 0, ASI_FALSE);

        ASISetControlValue(CamNum, ASI_EXPOSURE, exposure*1000, ASI_FALSE);
        ASISetControlValue(CamNum, ASI_BANDWIDTHOVERLOAD, 40, ASI_FALSE);

        time1 = GetTickCount();

        //bool bSave = true;


}
