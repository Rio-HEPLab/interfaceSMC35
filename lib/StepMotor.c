#include "StepMotor.h"
#include <string.h>

std::queue<_move> StepMotor::comandQueue;
int StepMotor::USB;
int StepMotor::stoped;
int StepMotor::moveevt;
int StepMotor::nextPulses;
StepMotor *StepMotor::toAtualize;

StepMotor::StepMotor(_Eixo _eixo, int _vm, int _vs, int _ac, float _con)
{
	eixo = (int)_eixo;
	StepMotor::SetVelocity(_vm);
	StepMotor::SetAcceleration(_ac);
	StepMotor::SetStartRate(_vs);
	StepMotor::SetConversionFactor(_con);
}

char* StepMotor::Cmd(char* motorCmd, int verbose)
{
	if ( verbose ) {
		(void)fprintf(stdout, "-------------------------- ") ;
		(void)fprintf(stdout, "CMD = %s", motorCmd) ;
		(void)fprintf(stdout, " -------------------------\n") ;
	}
	//(void)sprintf(motorCmd, "%s\r\n", motorCmd) ;
	strcat(motorCmd,"\r\n");
	char* ret = ComWrt(StepMotor::USB, motorCmd) ;
	(void)fputs(ret, stdout) ;
	if ( verbose ) {
		(void)fprintf(stdout, "--------------------------------") ;
		(void)fprintf(stdout, "-------------------------------\n") ;
	}

	return(ret) ;
}

int StepMotor::Init(int _USB, int verbose)
{
	int initOK = 0 ;

	StepMotor::USB = _USB;
	StepMotor::stoped = 0;
	StepMotor::moveevt = 0;
	StepMotor::nextPulses = 0;
	StepMotor::toAtualize = NULL;

	char InitCmds[9][16] = {
		"vm=200\r\n",
		"ac=200\r\n",
		"pls=1\r\n",
		"cb25=0\r\n",
		"nls=1\r\n",
		"cb26=0\r\n",
		"son=1\r\n",
		"con=1.\r\n",
		"ap=0\r\n"
	} ;

	for (int c=0; c<8; c++) {
		char* ret = ComWrt(StepMotor::USB, InitCmds[c]) ;
		if ( verbose )
			(void)fprintf(stdout, "%s %s\n", InitCmds[c], ret) ;
		if ( ret[0] != 'Y' )
			initOK = -1 ;
	}

	return(initOK) ;
}

int StepMotor::Move(_move move )
{
	char cmd[16];
	char* ret;

	if(move.delay == 0)
	{

		(void)sprintf(cmd, "con=%.4f\r\n", move.con);
		ret = ComWrt(StepMotor::USB, cmd);

		(void)sprintf(cmd, "cb36=%d\r\n", move.eixo);
		ret = ComWrt(StepMotor::USB, cmd);

		(void)sprintf(cmd, "vm=%d\r\n", move.vm);
		ret = ComWrt(StepMotor::USB, cmd);

		(void)sprintf(cmd, "vs=%d\r\n", move.vs);
		ret = ComWrt(StepMotor::USB, cmd);

		(void)sprintf(cmd, "ac=%d\r\n", move.ac);
		ret = ComWrt(StepMotor::USB, cmd);

		(void)sprintf(cmd, "sr=%d\r\n", move.nPulses);
		ret = ComWrt(StepMotor::USB, cmd);
	}
	else
	{
		sleep(move.delay);
		return 0;
	}

	return(ret[0] == 'Y' ? 0 : -1) ;
}

// return 1 running, return 0 halted
int StepMotor::NextMove()
{
	if(StepMotor::IsHalted(0))
	{
		if(StepMotor::moveevt)PositionAtualize();
		if(!stoped)
		{
			if(!StepMotor::comandQueue.empty())
			{
				StepMotor::toAtualize = (StepMotor*)StepMotor::comandQueue.front().eixoObject;
				StepMotor::moveevt = 1;
				if (StepMotor::Move(StepMotor::comandQueue.front()) == -1){
					StepMotor::comandQueue.pop();
					//StepMotor::Stop();	//Comentado pra debugar em casa
					return 2;	//error in comunication with smc35
				}
				else
				{
					StepMotor::comandQueue.pop();
					return 1;	//running
				}

			}
			else return 0;	//halted
		}
		else return 0;	//halted
	}

	return 1;	// running
}

int StepMotor::PositionAtualize()
{
	  int moveqty = 0;
		//static int nPulses = 0;
		//static StepMotor *toAtualize = NULL;
		int err;
		char cmd[16];
		char *ret;

		(void)sprintf(cmd, "AP\r\n");
		ret = ComWrt(StepMotor::USB, cmd);

		sscanf(ret,"AP=%d",&moveqty);

		//if(moveqty != StepMotor::nextPulses) return -1; // Not implemented for while

		if(toAtualize != NULL)toAtualize->SetPosition(moveqty);

		StepMotor::moveevt = 0;

		(void)sprintf(cmd, "AP=0\r\n");
		ret = ComWrt(StepMotor::USB, cmd);

		return(ret[0] == 'Y' ? 0 : -1);
}

int StepMotor::IsHalted(int debug)
{
	char cmd[16] ;
	(void)sprintf(cmd, "cv\r\n") ;

	int velocity = -1 ;
	char* ret = ComWrt(StepMotor::USB, cmd) ;
	(void)sscanf(ret, "CV=%d", &velocity) ;

	if(debug) velocity = 0;

	return(velocity == 0 ? 1 : 0) ;
}


int StepMotor::Stop()
{
	char cmd[16] ;
	(void)sprintf(cmd, "h\r\n") ;
	char* ret = ComWrt(StepMotor::USB, cmd);

	StepMotor::stoped = 1;

	while(!StepMotor::comandQueue.empty()) StepMotor::comandQueue.pop();

	return(ret[0] == 'Y' ? 0 : -1) ;
}

int StepMotor::Pause()
{
	StepMotor::stoped = 1;
	return 0;
}

int StepMotor::Start()
{
	StepMotor::stoped = 0;
	return 0;
}

void StepMotor::DisplaySettings(FILE* fp)
{
	char cmd[16] ;
	(void)sprintf(cmd, "?\r\n") ;
	char* ret = ComWrt(StepMotor::USB, cmd) ;
	(void)fputs(ret, fp) ;
}

const char* StepMotor::GetSettings()
{
	char cmd[16] ;
	(void)sprintf(cmd, "?\r\n") ;
	char* ret = ComWrt(StepMotor::USB, cmd) ;

	static char lOutput[4096] ;
	(void)strcpy(lOutput, ret) ;
	return(lOutput) ;
}

int StepMotor::SetVelocity(const int v)
{

	int velocity = v;
	if ( v < 0 )
		velocity = 0;
	if ( v > 65535 )
		velocity = 65535;

	vm = velocity;
	return 0;
}


int StepMotor::SetAcceleration(const int a)
{

	int acceleration = a ;
	if ( a < 0 )
		acceleration = 0 ;
	if ( a > 100000 )
		acceleration = 100000 ;

	ac = acceleration;
	return 0;
}


int StepMotor::SetStartRate(const int a)
{

	int startRate = a ;
	if ( a < 0 )
		startRate = 0 ;
	if ( a > 100000 )
		startRate = 100000 ;

	vs = startRate;
	return 0;
}


int StepMotor::SetConversionFactor(const float a)
{

	float conversionFactor = a ;
	if ( a < 0.0001 )
		conversionFactor = 0.0001 ;
	if ( a > 9999.9999 )
		conversionFactor = 9999.9999 ;

	con = conversionFactor;
	return 0;
}

int StepMotor::SetMaxPosition(const int a)
{
	maxPosition = a;

	return 0;
}

int StepMotor::SetMinPosition(const int a)
{
	minPosition = a;

	return 0;
}

int StepMotor::SetPosition(const int a)
{
	position += a;

	printf("Posicao atual: %d\n",position);
	
	return position;
}

int StepMotor::paramMove(StepMotor *_eixo, float a )
{
	_move tmp;

	if(a == 0) return -1;  //command not found

	if((_eixo->relativePosition + a) > _eixo->maxPosition ||
 			(_eixo->relativePosition + a) < _eixo->minPosition)
	{
		return -2; //pulses will break the limit position
	}
	else _eixo->relativePosition += a;

	//printf("%d %d\n",_eixo->relativePosition,_eixo->position);
	//fflush(stdout);

	tmp = {
		.eixoObject = (void*)_eixo,
		.con = _eixo->con,
		.eixo = _eixo->eixo,
		.vm = _eixo->vm,
		.vs = _eixo->vs,
		.ac = _eixo->ac,
		.nPulses = (int) a,
		.delay = 0
	};

	StepMotor::comandQueue.push(tmp);

	return 0;
}
int StepMotor::paramDelay(StepMotor *eixo, float a)
{
	if(a == 0) return -1;

	_move tmp = {
		.eixoObject = NULL,
		.con = 0,
		.eixo = 0,
		.vm = 0,
		.vs = 0,
		.ac = 0,
		.nPulses = 0,
		.delay = (int)a
	};

	StepMotor::comandQueue.push(tmp);

	return 0;
}
int StepMotor::paramStop(StepMotor *eixo, float a)
{
	return StepMotor::Stop();
}
int StepMotor::paramPause(StepMotor *eixo, float a )
{
	return StepMotor::Pause();
}
int StepMotor::paramStart(StepMotor *eixo, float a )
{
	return StepMotor::Start();
}
int StepMotor::paramVelocity(StepMotor *eixo, float a)
{
	if(eixo == NULL) return -1;
	return eixo->SetVelocity((int) a);
}
int StepMotor::paramAcceleration(StepMotor *eixo, float a)
{
	if(eixo == NULL) return -1;
	return eixo->SetAcceleration((int) a);
}
int StepMotor::paramStartRate(StepMotor *eixo, float a)
{
	if(eixo == NULL) return -1;
	return eixo->SetStartRate((int) a);
}
int StepMotor::paramConversionFactor(StepMotor *eixo, float a)
{
	if(eixo == NULL) return -1;
	return eixo->SetConversionFactor(a);
}
