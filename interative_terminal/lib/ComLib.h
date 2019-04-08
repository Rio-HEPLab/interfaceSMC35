#ifndef _COMLIB_H

# define _COMLIB_H       1

#include <stdio.h>      // standard input / output functions
#include <stdlib.h>
#include <string.h>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <unistd.h>     // syncfs

extern int ComInit(char *baudRate, const char* const USBDevice);
extern char* ComWrt(int dev, char* buf, const bool debug=false);
extern char* ComRd(int dev, const bool debug=false);
extern speed_t str2speed(char *str);

#endif /* !_COMLIB_H */
