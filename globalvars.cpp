#include <globalvars.h>
#include "mainwindow.h"
#include <cmath>

int piNumber;
bool programRunning;
bool tracking = false;
bool camRunning = false;
int whichAxisA = 0;
int whichAxisB = 1;

int backlashAz = 16400; // the measured number of encoder quadrature states measured in Az backlash
int backlashAlt = 1000; // same for Alt... As of now, backlash accounting is rudimentary-- perhaps set to zero for each
int lastDirectionAz;
int lastDirectionAlt;

const double latitude = 0.0; // set to your latitude
const double longitude = 0.0; // set to your longitude

/* The next four variables need to be set to the four GPIO ports
 * which are connected to your incremental encoders for Alt and Az axes
 */
const int encoderRotateA = 22; 
const int encoderRotateB = 27;
const int encoderInclineA = 23;
const int encoderInclineB = 24;

/* The next four variables need to be set to the four GPIO ports 
 * which are connected to your DC motors... Rotate is Az; Incline is Alt
 */
const int pinRotateA = 19;
const int pinRotateB = 13;
const int pinInclineA = 12;
const int pinInclineB = 18;

double targetAz;
double targetAlt;
double targetZen;

int pwmRange = 255;

const double polarisRA = 37.95291667 * 3.1415927654 / 180;
const double polarisDEC = 89.2641667 * 3.1415927654 / 180;

double angleAz;
double angleAlt;
double angleZen;

int counterAz;
int counterAlt;

double gearRatio = 28.8;  // set to your gear ratio -- my Alt and Az axes both have the same ratio, thus, only one variable
int quadratureStates = 40000;  // set to number of quadrature states of your encoder

double rAscension;
double declination;

double jTime1;
double jTime2;

double AzInc = 0;
double AltInc = 0;
double incremental = pwmRange / 20;
double errSumAz = 0;
double errSumAlt = 0;

void actualCallback(int direction, int axis)
{
    if(tracking && axis == 0) // && direction == lastDirectionAz)
    {
        counterAz += direction;
        if(counterAz > quadratureStates * gearRatio || counterAz < -quadratureStates * gearRatio)
            counterAz = 0;
        angleAz = counterAz * 2 * 3.141592654 / (gearRatio * quadratureStates);
    }
    else if(tracking && axis == 1) // && direction == lastDirectionAlt)
    {
        counterAlt += direction;
        if(counterAlt > quadratureStates * gearRatio || counterAlt < -quadratureStates * gearRatio)
            counterAlt = 0;
        angleAlt = counterAlt * 2 * 3.141592654 / (gearRatio * quadratureStates);
    }
/*
    if(tracking && axis == 0 && direction != lastDirectionAz)
    {
        backlashAz--;
        if(backlashAz < 1)
        {
            backlashAz = 16400;
            lastDirectionAz = direction;
        }
    }
    else if(tracking && axis == 1 && direction != lastDirectionAlt)
    {
        backlashAlt--;
        if(backlashAlt < 1)
        {
            backlashAlt = 1000;
            lastDirectionAlt = direction;
        }
    }
    */
}

