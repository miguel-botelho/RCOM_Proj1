#ifndef ___UTILS
#define ___UTILS

#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define FLAG 0x7E
#define ESC 0x5E

#define F 0x7E
#define A 0x03
#define C_SET 0x07
#define BCC (A^C_SET)

#define START 0
#define FLAG_RCV 1
#define A_RCV 2
#define C_RCV 3
#define BCC_OK 4
#define STOP_ST 5

#define ATTEMPTS 4
#define TIME_OUT 3

#define C_UA 0x03

#define C_DISC 0x0B

#endif /* UTILS */