/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#define BAUDRATE B9600
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define F 0x7E
#define A 0x03
#define C_UA 0x03
#define C_SET 0x07
#define BCC (A^C_SET)

#define START 0
#define FLAG_RCV 1
#define A_RCV 2
#define C_RCV 3
#define BCC_OK 4
#define STOP_ST 5

volatile int STOP=FALSE;

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;

    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS5", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }


  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
  
    
    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */



  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) próximo(s) caracter(es)
  */



    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");

char SET[5];
char UA[5];
char c;
sleep(5);

unsigned int i = 0;
int option = START;

while(!(STOP))
{	
	res = read(fd, &c, 1);
	switch (option)
	{
	case START:
		if (c == F) //ver
			{
				option = FLAG_RCV;
				SET[0] = c;
			}
		else
			option = START;
		break;

	case FLAG_RCV:
		if (c == F) //ver
			{
				option = FLAG_RCV;
				SET[0] = c;
			}
		else if (c == A) //ver
			{
				option = A_RCV;
				SET[1] = c;
			}
		else
			option = START;
		break;

	case A_RCV:
		if (c == F) //ver
			{
				option = FLAG_RCV;
				SET[0] = c;
			}
		else if (c == C_SET) //ver
			{
				option = FLAG_RCV;
				SET[2] = c;
			}
		else
			option = START;
		break;

	case C_RCV:
		if (c == F) //ver
			{
				option = FLAG_RCV;
				SET[0] = c;
			}
		else if (c == BCC) //ver
			{
				option = FLAG_RCV;
				SET[3] = c;
			}
		else
			option = START;
		break;

	case BCC_OK:
		if (c == F) //ver
			{
				option = STOP_ST;
				SET[4] = c;
			}
		else
			option = START;
		break;

	case STOP_ST:
		STOP = TRUE;
		break;

	default:
		break;
	} 
}

printf("FLAGS READ FROM SET: %x, %x, %x, %x, %x\n", SET[0], SET[1], SET[2], SET[3], SET[4]);

res = 0;

UA[0] = F;
UA[1] = A;
UA[2] = C_UA;
UA[3] = UA[1]^UA[2];
UA[4] = F;

res = write(fd, UA, strlen(UA) + 1);

printf("FLAGS SENT FROM UA: %x, %x, %x, %x, %x\n", UA[0], UA[1], UA[2], UA[3], UA[4]);

   
    sleep(5);



  /* 
    O ciclo WHILE deve ser alterado de modo a respeitar o indicado no guião 
  */



    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}
