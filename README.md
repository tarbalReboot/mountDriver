# TelescopeMount

Update 1/3/2022: I have decided to make the latest version publicly available on this new github. PID parameters still 
need to be tuned to your hardware and entered in main.cpp. My parameters are included. Global variables need to be set for your circumstances,
e.g., gps coordinates, altitude, gpio pins, etc in global.cpp.

Trueblood, M. and Genet, R.M., Telescope Control (Second English Edition), Willmann-Bell, Inc. (Richmond, 1997). 

If you are taking the project seriously, I would recommend another book as well:

Smart, W.M., Textbook on Spherical Astronomy (Sixth Edition, reprinted), Cambridge University Press (Cambridge, 1979).

At some point, you'll need to account for geometric errors, which are covered in the Trueblood/Genet book. Good luck.

Here is some telescope mount control software which can be used with any mount using DC gear motors and incremental encoders,
all controlled by a Raspberry Pi 4b (preferred) or 3b+ (untested on other versions). This software does not include geometric corrections for 
mechanical imperfections. If it does not point accurately, then you have some mechanical problems -- most likely backlash.
RA and Dec should be entered in terms of epoch 2000 after calibrating/aligning with polaris (If you are in the
southern hemisphere, set the polaris RA and DEC in globalvars.cpp to a convenient star which you wish to use to calibrate).

The GUI is very simple, and you may want to build on this code. This is the last this repository will be updated.
A future version may or may not be public, but it will have it's own repository. This code is  
GNU General Public License (GNU GPL), copyright John Emil Petersen III, 2019-2021.

You simply need to change the global variables in globalvars.cpp, such as gearRatio and quadratureStates to what is 
appropriate for your mount. You may want to measure environmental variables such as temperature with instrumentation, 
rather than updating every time and compiling.

Set the variable quadrature states to the number of pulses per revolution on your encoder times 4. So, if your encoders are
10,000 ppr, you set quadratureStates = 40000.

The variables pinRotateA, pinRotateB, PinInclineA, and PinInclineB correspond to the gpio pins attached to your
motor drivers' PWM connection. The same goes for the encoderRotate and encoderIncline variables.

In order to compile on a Raspberry Pi 3b+, you will need to link the appropriate libraries. I suggest you compile
within QTCreator after you have downloaded and installed the boost, pigpio, and sofa libraries. Then, you need to 
run pigpiod. If you want it to run automatically at startup, as well as sync the time after a 30 second wait, and set the 
cpu to performance mode, add the following

#!/bin/sh -e

pigpiod;

echo 'GOVERNOR="performance"' | tee /etc/default/cpufrequtils;

systemctl disable ondemand;

sleep 30;

service ntp stop;

ntpdate time.nist.gov;

service ntp start;

exit 0;


to your /etc/rc.local file. If it does not exist, then create it. Don't forget to make it executable by typing

sudo chmod a+x /etc/rc.local

All of this has been tested on Ubuntu 18.04 only. Good luck. The reason for 18.04 is due to the way GPIOs are 
controlled with this distribution. At some point, cpu performance was changed to an "on demand" setting, 
which causes problems, so now... you need to add cpufrequtility and set it to stay running at 1.5 GHz...

Once you have followed every step, You still need to tune the PID parameters in the main function. I cannot stress how
important this is.
