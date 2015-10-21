#include "state.h"
#include "utils.h"

static volatile int STOP_st=FALSE;

int send_Frame(int fd, char *SET) {
	
	int res;
	int flag = getFlag();
	
	if (flag)
	{
		alarm(3);
		setFlag(0);
	}

	res = write(fd, SET, 5);

	printf("FLAGS SENT FROM SET: %x, %x, %x, %x, %x\n\n", SET[0], SET[1], SET[2], SET[3], SET[4]);
	
}

int receive_UA(int fd, char *UA) {
	
	int res;
	int option = START;
	char flag_ST;

	while(!(STOP_st)) {
		
		res = read(fd, &flag_ST, 1);

		if(flag){
		      alarm(3);                 // activa alarme de 3s
		      flag=0;
		      STOP_st = TRUE;
		   }

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
			else if (flag_ST == (A^C_UA)) //ver
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
			STOP_st = TRUE;
			break;

		default:
			break;
		} 
	}

}

int check_UA(char *sent) {
	int error = 0;
	if (sent[0] != F || sent[1] != A || sent[2] != C_UA || sent[3] != (sent[1]^sent[2]) || sent[4] != F)
		error = 1;
	return error;
}

int getStop() {
  return STOP_st;
}