#include "StepMotor.h"

char* StepMotorCmd(const int USB, char* motorCmd, int verbose)
{
	if ( verbose ) {
		(void)fprintf(stdout, "-------------------------- ") ;
		(void)fprintf(stdout, "CMD = %s", motorCmd) ;
		(void)fprintf(stdout, " -------------------------\n") ;
	}
	(void)sprintf(motorCmd, "%s\r\n", motorCmd) ;
	char* ret = ComWrt(USB, motorCmd) ;
	(void)fputs(ret, stdout) ;
	if ( verbose ) {
		(void)fprintf(stdout, "--------------------------------") ;
		(void)fprintf(stdout, "-------------------------------\n") ;
	}

	return(ret) ;
}

int StepMotorInit(const int fdUSB, int verbose)
{
	int initOK = 0 ;

	char InitCmds[8][16] = {
		"vm=200\r\n",
		"ac=200\r\n",
		"pls=1\r\n",
		"cb25=0\r\n",
		"nls=1\r\n",
		"cb26=0\r\n",
		"son=1\r\n",
		"con=1.\r\n"
	} ;case /* value */:

	for (int c=0; c<8; c++) {
		char* ret = ComWrt(fdUSB, InitCmds[c]) ;
		if ( verbose )
			(void)fprintf(stdout, "%s %s\n", InitCmds[c], ret) ;
		if ( ret[0] != 'Y' )
			initOK = -1 ;
	}

	return(initOK) ;
}

int StepMotorMove(const int USB, const int nPulses)
{
	char cmd[16] ;
	(void)sprintf(cmd, "sr=%d\r\n", nPulses) ;
	char* ret = ComWrt(USB, cmd) ;

	return(ret[0] == 'Y' ? 0 : -1) ;
}

int StepMotorMoveCm(const int USB, const int stepCm)
{
	const int nPulses = stepCm * MOTOR_PULSES_PER_CM ;
	char cmd[16] ;
	(void)sprintf(cmd, "sr=%d\r\n", nPulses) ;
	char* ret = ComWrt(USB, cmd) ;

	return(ret[0] == 'Y' ? 0 : -1) ;
}

int StepMotorIsHalted(const int USB)
{
	char cmd[16] ;
	(void)sprintf(cmd, "cv\r\n") ;

	int velocity = -1 ;
	char* ret = ComWrt(USB, cmd) ;
	(void)sscanf(ret, "CV=%d", &velocity) ;

	return(velocity == 0 ? 1 : 0) ;
}

void StepMotorWait(const int USB)
{
	char cmd[16] ;
	(void)sprintf(cmd, "cv\r\n") ;

	int velocity = -1 ;
	do {
		char* ret = ComWrt(USB, cmd) ;
		(void)sscanf(ret, "CV=%d", &velocity) ;
		(void)usleep(100000) ;
	} while ( velocity ) ;
}

void StepMotorWhereIs(const int USB)
{
	char cmd[16] ;
	(void)sprintf(cmd, "sr\r\n") ;
	char* ret = ComWrt(USB, cmd) ;
	int sr ;
	(void)sscanf(ret, "SR=%d", &sr) ;


	(void)sprintf(cmd, "ap\r\n") ;
	ret = ComWrt(USB, cmd) ;
	int ap ;
	(void)sscanf(ret, "AP=%d", &ap) ;

	(void)sprintf(cmd, "app\r\n") ;
	ret = ComWrt(USB, cmd) ;
	int app ;
	(void)sscanf(ret, "APP=%d", &app) ;

	(void)fprintf(stdout,
	"Motor Present Position: AP=%d, APP=%d [SR=%d]\n", ap, app, sr) ;
}

int StepMotorGetRelPos(const int USB)
{
	char cmd[16] ;
	(void)sprintf(cmd, "sr\r\n") ;
	char* ret = ComWrt(USB, cmd) ;
	int sr ;
	(void)sscanf(ret, "SR=%d", &sr) ;
	return(sr) ;
}

int StepMotorGetPos(const int USB)
{
	char cmd[16] ;
	(void)sprintf(cmd, "ap\r\n") ;
	char* ret = ComWrt(USB, cmd) ;
	int ap ;
	(void)sscanf(ret, "AP=%d", &ap) ;
	return(ap) ;
}

int StepMotorGetPosPulses(const int USB)
{
	char cmd[16] ;
	(void)sprintf(cmd, "app\r\n") ;
	char* ret = ComWrt(USB, cmd) ;
	int app ;
	(void)sscanf(ret, "APP=%d", &app) ;
	return(app) ;
}

int StepMotorZero(const int USB)
{
	char cmd[16] ;
	(void)sprintf(cmd, "ap=0\r\n") ;
	char* ret = ComWrt(USB, cmd) ;

	return(ret[0] == 'Y' ? 0 : -1) ;
}

int StepMotorHalt(const int USB)
{
	char cmd[16] ;
	(void)sprintf(cmd, "h\r\n") ;
	char* ret = ComWrt(USB, cmd) ;

	return(ret[0] == 'Y' ? 0 : -1) ;
}

int StepMotorNegativeSwitch(const int USB)
{
	char cmd[16] ;
	(void)sprintf(cmd, "sr-\r\n") ;
	char* ret = ComWrt(USB, cmd) ;

	return(ret[0] == 'Y' ? 0 : -1) ;
}

int StepMotorPositiveSwitch(const int USB)
{
	char cmd[16] ;
	(void)sprintf(cmd, "sr+\r\n") ;
	char* ret = ComWrt(USB, cmd) ;

	return(ret[0] == 'Y' ? 0 : -1) ;
}

int SwitchToFirst(const int USB)
{
	char cmd[16] ;
	(void)sprintf(cmd, "cb36=1\r\n") ;
	char* ret = ComWrt(USB, cmd) ;

	return(ret[0] == 'Y' ? 0 : -1) ;
}

int SwitchToSecond(const int USB)
{
	char cmd[16] ;
	(void)sprintf(cmd, "cb36=2\r\n") ;
	char* ret = ComWrt(USB, cmd) ;

	return(ret[0] == 'Y' ? 0 : -1) ;
}

int SwitchToBoth(const int USB)
{
	char cmd[16] ;
	(void)sprintf(cmd, "cb36=3\r\n") ;
	char* ret = ComWrt(USB, cmd) ;

	return(ret[0] == 'Y' ? 0 : -1) ;
}

void StepMotorDisplaySettings(const int USB, FILE* fp)
{
	char cmd[16] ;
	(void)sprintf(cmd, "?\r\n") ;
	char* ret = ComWrt(USB, cmd) ;
	(void)fputs(ret, fp) ;
}

const char* const StepMotorGetSettings(const int USB)
{
	char cmd[16] ;
	(void)sprintf(cmd, "?\r\n") ;
	char* ret = ComWrt(USB, cmd) ;

	static char lOutput[4096] ;
	(void)strcpy(lOutput, ret) ;
	return(lOutput) ;
}

void StepMotorSetVelocity(const int USB, const int v)
{
	char cmd[16] ;
	int velocity = v ;
	if ( v < 0 )
		velocity = 0 ;
	if ( v > 65535 )
		velocity = 65535 ;
	(void)sprintf(cmd, "VM=%d\r\n", velocity) ;
	char* ret = ComWrt(USB, cmd) ;
}

int StepMotorGetVelocity(const int USB)
{
	char cmd[16] ;
	(void)sprintf(cmd, "VM\r\n") ;
	char* ret = ComWrt(USB, cmd) ;
	int velocity ;
	(void)sscanf(ret, "VM=%d", &velocity) ;
	return(velocity) ;
}

void StepMotorSetAcceleration(const int USB, const int a)
{
	char cmd[16] ;
	int acceleration = a ;
	if ( a < 0 )
		acceleration = 0 ;
	if ( a > 100000 )
		acceleration = 100000 ;
	(void)sprintf(cmd, "AC=%d\r\n", acceleration) ;
	char* ret = ComWrt(USB, cmd) ;
}

int StepMotorGetAcceleration(const int USB)
{
	char cmd[16] ;
	(void)sprintf(cmd, "AC\r\n") ;
	char* ret = ComWrt(USB, cmd) ;
	int acceleration ;
	(void)sscanf(ret, "AC=%d", &acceleration) ;
	return(acceleration) ;
}
