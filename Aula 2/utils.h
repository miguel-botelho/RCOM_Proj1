#ifndef ___UTILS
#define ___UTILS

#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define F 0x7E
#define FLAG 0x7E
#define A 0x03
#define C_SET 0x07
#define BCC_SET (A^C_SET)
#define BCC_UA (A^C_UA)
#define BCC_DISC (A^C_DISC)
#define A_RECEPTOR 0x01

#define START 0
#define FLAG_RCV 1
#define A_RCV 2
#define C_RCV 3
#define BCC_OK 4
#define STOP_ST 5
#define ESC 0x7D
#define SUBS 0x20	

#define ATTEMPTS 4
#define TIME_OUT 3

#define C_UA 0x03

#define C_DISC 0x0B

#define RECEIVER 0
#define TRANSMITTER 1

#define FAILED -1
#define RE_SEND_RR -2
#define RE_SEND_SET -3


typedef struct {
	char *port; 						//Dispositivo /dev/ttySx, x = 0, 1
	int fd;
	int baudRate; 						//Velocidade de transmissão
	unsigned int sequenceNumber;		//Número de sequência da trama: 0, 1
	unsigned int timeout; 				//Valor do temporizador: 1 s
	unsigned int maxTries; 				//Número de tentativas em caso de falha
	int status;
} LinkLayer;

#endif /* UTILS */