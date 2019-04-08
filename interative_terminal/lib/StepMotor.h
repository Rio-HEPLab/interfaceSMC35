
#ifndef _STEPMOTOR_H
# define _STEPMOTOR_H       1

#include <stdio.h>
#include <stdlib.h>

#include "ComLib.h"

#define MOTOR_PULSES_PER_CM 1896

extern char* StepMotorCmd(const int USB, char* motorCmd, int verbose=0) ;
extern int StepMotorInit(const int fdUSB, int verbose=0) ;
extern int StepMotorMove(const int USB, const int nPulses) ;
extern int StepMotorMoveCm(const int USB, const int stepCm) ;
extern void StepMotorWait(const int USB) ;
extern void StepMotorWhereIs(const int USB) ;
extern int StepMotorGetPos(const int USB) ;
extern int StepMotorGetPosPulses(const int USB) ;
extern int StepMotorGetRelPos(const int USB) ;
extern int StepMotorZero(const int USB) ;
extern int StepMotorHalt(const int USB) ;
extern int StepMotorIsHalted(const int USB) ;
extern int StepMotorPositiveSwitch(const int USB) ;
extern int StepMotorNegativeSwitch(const int USB) ;
extern void StepMotorDisplaySettings(const int USB, FILE* fp=stdout) ;
extern const char* const StepMotorGetSettings(const int USB) ;
extern void StepMotorSetVelocity(const int USB, const int v) ;
extern int StepMotorGetVelocity(const int USB) ;
extern void StepMotorSetAcceleration(const int USB, const int a) ;
extern int StepMotorGetAcceleration(const int USB) ;

#endif /* !_STEPMOTOR_H */
