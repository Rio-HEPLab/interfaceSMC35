
#ifndef _STEPMOTOR_H
# define _STEPMOTOR_H       1

#include <stdio.h>
#include <stdlib.h>
#include <queue>

#include "ComLib.h"

#define MOTOR_PULSES_PER_CM 1896

enum _Eixo {x = 1,
           y = 2,
           both = 3};

typedef struct struct_move
{
  void *eixoObject;
	float con;
	int eixo;
	int vm;
	int vs;
	int ac;
	int nPulses;
	int delay;
} _move;

class StepMotor
{
private:

    static int USB;
    static std::queue<_move> comandQueue;
    static int stoped;

    int eixo;
    int vs;
    int vm;
    int ac;
    float con;

    int position = 0;
    int relativePosition = 0;

    int maxPosition = 20;
    int minPosition = -20;

    static int moveevt;
    static int nextPulses;
    static StepMotor *toAtualize;

    static char* Cmd(char* motorCmd, int verbose=0);
    static int Move(_move move);
    static int Stop();
    static int Pause();
    static int Start();
    static int IsHalted(int debug);
    static int PositionAtualize();
    static void DisplaySettings(FILE* fp=stdout);
    static const char* GetSettings();

public:

    StepMotor(_Eixo _eixo, int _vm, int _vs, int _ac, float _con);
    static int Init(int _USB, int verbose=0);
    static int NextMove();
    //static int MoveValidate(_move move);

    int SetVelocity(const int v);
    int SetAcceleration(const int a);
    int SetStartRate(const int a);
    int SetConversionFactor(const float a);
    int SetMaxPosition(const int a);
    int SetMinPosition(const int a);
    int SetPosition(const int a);

    static int paramStop(StepMotor *eixo, float a );
    static int paramPause(StepMotor *eixo, float a );
    static int paramStart(StepMotor *eixo, float a );
    static int paramDelay(StepMotor *eixo, float a );
    static int paramMove(StepMotor *eixo, float a );
    static int paramVelocity(StepMotor *eixo, float a );
    static int paramAcceleration(StepMotor *eixo, float a );
    static int paramStartRate(StepMotor *eixo, float a );
    static int paramConversionFactor(StepMotor *eixo, float a );
    static int paramMaxPosition(StepMotor *eixo, float a );
    static int paramMinPosition(StepMotor *eixo, float a );
};

#endif /* !_STEPMOTOR_H */
