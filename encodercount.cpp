#include "encodercount.h"
#include <pigpiod_if2.h>
#include <chrono>
#include <thread>
#include <globalvars.h>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <QMutex>

encoderCount::encoderCount(unsigned int pi, unsigned int gpioNumberA, unsigned int gpioNumberB, re_decoderCB_t callback)
{
    levelARotate = gpio_read(piNumber, encoderRotateA);
    levelBRotate = gpio_read(piNumber, encoderRotateB);

    levelAIncline = gpio_read(piNumber, encoderInclineA);
    levelBIncline = gpio_read(piNumber, encoderInclineB);

    pinA = gpioNumberA;
    pinB = gpioNumberB;

    lastABRotate = 0;
    lastABIncline = 0;

    currentABRotate = 0;
    currentABIncline = 0;

    debugAz = 0;
    debugAlt = 0;

    myCallback = callback;

    set_mode(piNumber, gpioNumberA, PI_INPUT);
    set_mode(piNumber, gpioNumberB, PI_INPUT);
    set_pull_up_down(piNumber, gpioNumberA, PI_PUD_UP);
    set_pull_up_down(piNumber, gpioNumberB, PI_PUD_UP);

}

void encoderCount::_pulseEx(int pinum, unsigned int gpio, unsigned int level, uint32_t tick, void *user)
{
    if(pinum > -1)
    {
        encoderCount *mySelf;
        mySelf = (encoderCount*) user;

        mySelf->_pulse(gpio, level);

//        delete mySelf;
        return;
    }
}

void encoderCount::_pulse(int gpio, unsigned int level)
{
    int index;
       if(gpio == encoderRotateA)
       {
           levelARotate = level;
           currentABRotate = (levelARotate << 1) | levelBRotate;
           index = (lastABRotate << 2) | currentABRotate;
           if (index > 15 || index < 0)
               index = 0;
           myCallback(outcome[index], whichAxisA);
           lastABRotate = currentABRotate;
       }
       else if (gpio == encoderRotateB)
       {
           levelBRotate = level;
           currentABRotate = (levelARotate << 1) | levelBRotate;
           index = (lastABRotate << 2) | currentABRotate;
           if (index > 15 || index < 0)
               index = 0;
           myCallback(outcome[index], whichAxisA);
           lastABRotate = currentABRotate;
       }
       else if(gpio == encoderInclineA)
       {
           levelAIncline = level;
           currentABIncline = (levelAIncline << 1) | levelBIncline;
           index = (lastABIncline << 2) | currentABIncline;
           if (index > 15 || index < 0)
               index = 0;
           myCallback(outcome[index], whichAxisB);
           lastABIncline = currentABIncline;
       }
       else if (gpio == encoderInclineB)
       {
           levelBIncline = level;
           currentABIncline = (levelAIncline << 1) | levelBIncline;
           index = (lastABIncline << 2) | currentABIncline;
           if (index > 15 || index < 0)
               index = 0;
           myCallback(outcome[index], whichAxisB);
           lastABIncline = currentABIncline;
       }
}


void encoderCount::CBcancel(void)
{
   callback_cancel(cba);
   callback_cancel(cbb);
}

void encoderCount::speedOutput()
{
    double debugSpeed = std::abs(debugAz - counterAz) * 360 * 3600 / (40000 * 28.8);
    std::cout << " Az: " << counterAz << " " << debugAz << " " << debugSpeed << " " << "arcsec/sec" << std::endl;
    debugAz = counterAz;
}

void encoderCount::run()
{
    QMutex mutie;
    mutie.lock();
    cba = callback_ex(piNumber, pinA, EITHER_EDGE, _pulseEx, this);
    cbb = callback_ex(piNumber, pinB, EITHER_EDGE, _pulseEx, this);

    while(programRunning)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//        speedOutput();
        continue;
    }
    encoderCount::CBcancel();
    mutie.unlock();
}
