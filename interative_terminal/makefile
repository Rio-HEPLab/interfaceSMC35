all: interativeSerial

LIBRARY_DIR=./lib

interativeSerial : main.cpp libComLib.so
	g++ -Wall -O2 -o interativeSerial main.cpp -I${LIBRARY_DIR} -L${LIBRARY_DIR} -lStepMotor -lComLib

libComLib.so : ComLib.o
		ar rcs lib/libComLib.so lib/ComLib.o

ComLib.o : lib/ComLib.h lib/ComLib.c
	g++ -Wall -O2 -g -c -o lib/ComLib.o lib/ComLib.c

clean:
	rm interativeSerial
	rm ./lib/ComLib.o
	rm ./lib/libComLib.so
