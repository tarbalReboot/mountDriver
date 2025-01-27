#ifndef GLOBALVARS_H
#define GLOBALVARS_H
#include "encodercount.h"

extern int piNumber;
extern int pwmRange;

extern int backlashAz;
extern int backlashAlt;
extern int lastDirectionAz;
extern int lastDirectionAlt;

extern bool programRunning;
extern bool camRunning;
extern bool tracking;

extern const int pinRotateA;
extern const int pinRotateB;
extern const int pinInclineA;
extern const int pinInclineB;

extern const int encoderRotateA;
extern const int encoderRotateB;

extern const int encoderInclineA;
extern const int encoderInclineB;

extern const double polarisRA;
extern const double polarisDEC;
extern const double latitude;
extern const double longitude;

extern double targetAz;
extern double targetAlt;
extern double targetZen;

extern double angleAz;
extern double angleAlt;
extern double angleZen;

extern double AltInc;
extern double AzInc;
extern double incremental;

extern double errSumAz;
extern double errSumAlt;

extern int counterAz;
extern int counterAlt;

extern double gearRatio;
extern int quadratureStates;

extern int whichAxisA;  // Az = 0
extern int whichAxisB;  // Alt = 1;

extern double rAscension;
extern double declination;

extern double jTime1;
extern double jTime2;

void actualCallback(int direction, int axis);


#endif // GLOBALVARS_H
