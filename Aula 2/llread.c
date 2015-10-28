#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <signal.h>

#define BUF_SIZE 255

int llread(int fd, char* buffer)
{
	char buf[BUF_SIZE];

	int string_length = 0;
	int res = 0;

	//abrir a porta
	fd = open(argv[1], O_RDWR | O_NOCTTY);

	if (fd <0)
	{
		perror(argv[1]);
		exit(-1);
	}

	if ( tcgetattr(fd,&oldtio) == -1)
	{ /* save current port settings */
		perror("tcgetattr");
		exit(-1);
	}



	while(1)
	{
		//verificar comprimento da string
		if(length <=0)
			continue;
		//Validar a string --> se for válida fazer break

	}

	//tratar informação que está no i0
	int i;
	for(i = 0; i < length; ++i)
	{

	}


	// Enviar o RR1

	char rr1[] = {SERIAL_FLAG,
			SERIAL_A_ANS_RECEIVER,
			SERIAL_C_RR_N1,
			SERIAL_A_ANS_RECEIVER^SERIAL_C_RR_N1,
			SERIAL_FLAG};

	write(fd,rr1,5);

	return res;
}
