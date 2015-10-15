/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define F 0x7E
#define A 0x03
#define C_SET 0x07
#define BCC (A^C_SET)

#define ATTEMPTS 4
#define TIME_OUT 3

#define C_UA 0x03

volatile int STOP=FALSE;

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];
    int i, sum = 0, speed = 0;
    
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


    fd = open(argv[1], O_RDWR | O_NOCTTY);
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
    newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */



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



  /* 
    O ciclo FOR e as instruções seguintes devem ser alterados de modo a respeitar 
    o indicado no guião 
  */

	char SET[5];
	char received_UA;
	char flag_ST;
	char UA[5];
	
	SET[0] = F;
	SET[1] = A;
	SET[2] = C_SET;
	SET[3] = BCC;
	SET[4] = F;

	/* WRITE SET */
	int tries = 1;
	
	while(tries <= ATTEMPTS){
		printf("Attempt %d\n", tries);
		tries++;
		res = write(fd, SET, strlen(SET));
	
		printf("FLAGS SENT FROM SET: %x, %x, %x, %x, %x\n\n", SET[0], SET[1], SET[2], SET[3], SET[4]);

		sleep(TIME_OUT);
	
		int i = 0;
		int failed = 0;
		int option = START;
		while(!(STOP)) {	
		res = read(fd, &flag_ST, 1);

		switch (option)
		{
		case START:
			if (flag_ST == F) //ver
				{
					option = FLAG_RCV;
					UA[0] = flag_ST;
				}
			else
				option = START;
			break;

		case FLAG_RCV:
			if (flag_ST == F) //ver
				{
					option = FLAG_RCV;
					UA[0] = flag_ST;
				}
			else if (flag_ST == A) //ver
				{
					option = A_RCV;
					UA[1] = flag_ST;
				}
			else
				option = START;
			break;

		case A_RCV:
			if (flag_ST == F) //ver
				{
					option = FLAG_RCV;
					UA[0] = flag_ST;
				}
			else if (flag_ST == C_UA) //ver
				{
					option = C_RCV;
					UA[2] = flag_ST;
				}
			else
				option = START;
			break;

		case C_RCV:
			if (flag_ST == F) //ver
				{
					option = FLAG_RCV;
					UA[0] = flag_ST;
				}
			else if (flag_ST == A^C_UA) //ver
				{
					option = BCC_OK;
					UA[3] = flag_ST;
				}
			else
				option = START;
			break;

		case BCC_OK:
			if (flag_ST == F) //ver
				{
					option = STOP_ST;
					UA[4] = flag_ST;
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
		printf("FLAGS READ FROM UA: %x, %x, %x, %x, %x\n\n", UA[0], UA[1], UA[2], UA[3], UA[4]);
		if (check_UA(UA))
			failed = 1;
	
		if (failed == 0)
			break;			
	
	}

	sleep(1);
   
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }




    close(fd);
    return 0;
}

int check_UA(char *sent)
{
	int error = 0;
	if (sent[0] != F || sent[1] != A || sent[2] != C_UA || sent[3] != (sent[1]^sent[2]) || sent[4] != F)
		error = 1;
	return error;
}
