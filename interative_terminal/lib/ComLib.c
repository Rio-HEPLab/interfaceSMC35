#include "ComLib.h"

int ComInit(char *baudRate, const char* const USBDevice)
{
	int fdUSB = open(USBDevice, O_RDWR | O_SYNC ) ;
	if ( fdUSB < 0 ) {
		printf("Error to open the comunication\n");
		perror(USBDevice) ;
		return(-1) ;
	}

	struct termios tty ;
	(void)memset(&tty, 0, sizeof tty) ;

	if ( tcgetattr(fdUSB, &tty) ) {
		(void)perror(USBDevice) ;
		return(-1) ;
	}

	// set speed
	cfsetospeed (&tty, str2speed(baudRate)) ;
	cfsetispeed (&tty, str2speed(baudRate)) ;

	// set port control
	tty.c_cflag     &=  ~CSIZE;
	tty.c_cflag     |=  CS7 ; // 7bits
	tty.c_cflag     |=  PARENB|PARODD ; // odd parity
	tty.c_cflag     &=  ~CSTOPB ; // 1 stop bit
	tty.c_cflag     &=  ~CRTSCTS ; // no flow control
	tty.c_iflag	&= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

	tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
	tty.c_cflag     |=  CREAD | CLOCAL ; // turn on READ & ignore ctrl lines

	// flush port, then apply attributes
	tcflush(fdUSB, TCIFLUSH) ;

	if ( tcsetattr(fdUSB, TCSANOW, &tty) ) {
   		perror(USBDevice) ;
		return(-1) ;
	}

	return(fdUSB) ;
}

char* ComWrt(int dev, char* buf, const bool debug)
{
	if ( debug )
		(void)fprintf(stdout, "Writing '%s' (len=%ld)\n",
		buf, strlen(buf)) ;

	size_t i;

	i = write(dev, buf, strlen(buf)) ;
	
	if(strstr(buf,"?") != NULL) usleep(1000000);

	// get answer
	static char ret[1024];
	memset(ret, '\0', sizeof(ret)) ;
	i = read(dev, ret, sizeof(ret));

	if ( debug )
		(void)fprintf(stdout,
			"Got back '%s' (len=%ld)\n", ret, strlen(ret)) ;

	// replace CR
	for (char* p=ret; *p!='\0'; p++) {
		if ( *p == '\r' )
			*p = '\n' ;
	}

	return(ret) ;
}

char* ComRd(int dev, const bool debug)
{
	static char buf[80] ;
	memset(buf, '\0', sizeof(buf)) ;
	size_t i;

	i = read(dev, buf, sizeof(buf)) ;

	// skip CR
	for (char* p=buf; *p!='\0'; p++) {
	 	if ( *p == '\r' )
	 		*p = '\n' ;
	}

	if ( debug )
		(void)fprintf(stdout, "Read '%s' (len=%ld)\n",
		buf, strlen(buf)) ;

	return(buf) ;
}

speed_t str2speed(char *str)
{
	speed_t baudRate = B9600;

	if(!strcmp(str,"B0")) baudRate = B0;
	else if(!strcmp(str,"B50")) baudRate = B50;
	else if(!strcmp(str,"B75")) baudRate = B75;
	else if(!strcmp(str,"B110")) baudRate = B110;
	else if(!strcmp(str,"B143")) baudRate = B134;
	else if(!strcmp(str,"B1500")) baudRate = B150;
	else if(!strcmp(str,"B200")) baudRate = B200;
	else if(!strcmp(str,"B300")) baudRate = B300;
	else if(!strcmp(str,"B600")) baudRate = B600;
	else if(!strcmp(str,"B1200")) baudRate = B1200;
	else if(!strcmp(str,"B1800")) baudRate = B1800;
	else if(!strcmp(str,"B2400")) baudRate = B2400;
	else if(!strcmp(str,"B4800")) baudRate = B4800;
	else if(!strcmp(str,"B9600")) baudRate = B9600;
	else if(!strcmp(str,"B19200")) baudRate = B19200;
	else if(!strcmp(str,"B38400")) baudRate = B38400;
	else if(!strcmp(str,"B57600")) baudRate = B57600;
	else if(!strcmp(str,"B115200")) baudRate = B115200;
	else if(!strcmp(str,"B230400")) baudRate = B230400;
	else printf("baudrate não disponivel! Usando padrao 9600\n");

	return baudRate;
}
