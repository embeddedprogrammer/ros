#include <sys/time.h>      // For gettimeofday
#include <string>
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include "serial/serial.h"

using std::string;
using std::exception;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;

double timerStartTime[10];

serial::Serial my_serial("/dev/ttyUSB0", 9600, serial::Timeout::simpleTimeout(1000));

double utility_getTime_ms()
{
        struct timeval tp;
        gettimeofday(&tp, NULL);
        return ((double) tp.tv_sec * 1e3 + (double) tp.tv_usec * 1e-3);
}

void utility_startTimer(int timerId)
{
        timerStartTime[timerId] = utility_getTime_ms();
}

double utility_getTimerTime_ms(int timerId)
{
        return (utility_getTime_ms() - timerStartTime[timerId]);
}

double utility_getTimerTime_s(int timerId)
{
        return (utility_getTime_ms() - timerStartTime[timerId]) / 1000;
}

void sendCommand(int robotId, int leftMotorSpeed, int rightMotorSpeed)
{
        char cmd[4];
        cmd[0] = robotId;
        cmd[1] = 128 + leftMotorSpeed;
        cmd[2] = 128 + rightMotorSpeed;
        cmd[3] = '\n';
        string cmdStr(cmd, cmd + 4);
        my_serial.write(cmdStr);
}
