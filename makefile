all: interface_sm35

LIBRARY_DIR=./lib

interface_sm35 : main.cpp libComLib.so libStepMotorLib.so
	g++ -Wall -O2 -o interface.exe main.cpp -I${LIBRARY_DIR} -L${LIBRARY_DIR} -lComLib -lStepMotorLib -lboost_program_options -lncurses

libComLib.so : ComLib.o
	ar rcs lib/libComLib.so lib/ComLib.o

ComLib.o : lib/ComLib.h lib/ComLib.c
	g++ -Wall -O2 -g -c -o lib/ComLib.o lib/ComLib.c

libStepMotorLib.so : StepMotorLib.o
	ar rcs lib/libStepMotorLib.so lib/StepMotorLib.o

StepMotorLib.o : lib/StepMotor.h lib/StepMotor.c
	g++ -Wall -O2 -g -c -o lib/StepMotorLib.o lib/StepMotor.c

clean:
	rm interface.exe
	rm ./lib/ComLib.o
	rm ./lib/libComLib.so
	rm ./lib/StepMotorLib.o
	rm ./lib/libStepMotorLib.so
