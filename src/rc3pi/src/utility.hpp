#ifndef UTILITY_H
#define UTILITY_H

double utility_getTime_ms();

void utility_startTimer(int timerId);

double utility_getTimerTime_ms(int timerId);

double utility_getTimerTime_s(int timerId);

void sendCommand(int robotId, int leftMotorSpeed, int rightMotorSpeed);

#endif /** UTILITY_H */
