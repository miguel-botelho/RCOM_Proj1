#include "state.h"
#include "utils.h"

static volatile int STOP_UA=FALSE;

volatile int STOP_SET=FALSE;

volatile int STOP_DISC=FALSE;


int send_SET(int fd, char *SET) {
	
	int res;
	int flag = getFlag();
	
	if (flag)
	{
		alarm(3);
		setFlag(0);
	}

	res = write(fd, SET, sizeof(SET));

	printf("FLAGS SENT FROM SET: %x, %x, %x, %x, %x\n\n", SET[0], SET[1], SET[2], SET[3], SET[4]);
	
}

int send_UA(int fd, char *UA) {
  int res;
  
  res = write(fd, UA, sizeof(UA));

  printf("FLAGS SENT FROM UA: %x, %x, %x, %x, %x\n", UA[0], UA[1], UA[2], UA[3], UA[4]);
}

int send_DISC(int fd, char *DISC) {
  
  int res;
  
  int flag = getFlag();
	
	if (flag)
	{
		alarm(3);
		setFlag(0);
	}
  
  res = write(fd, DISC, 5);

  printf("FLAGS SENT FROM DISC: %x, %x, %x, %x, %x\n", DISC[0], DISC[1], DISC[2], DISC[3], DISC[4]);
}


int receive_UA(int fd, char *UA) {
	
	int res;
	int option = START;
	char flag_ST;

	while(!(STOP_UA)) {
		
		res = read(fd, &flag_ST, 1);
		int flag = getFlag();
		if(flag && flag != -1){
		      alarm(0);                 // activa alarme de 3s
		      setFlag(-1);
		      STOP_UA = TRUE;
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
			STOP_UA = TRUE;
			break;

		default:
			break;
		} 
	}

}

int receive_SET(int fd, char *SET) {
  
  char flag_ST;
  int res;
  unsigned int i = 0;
  int option = START;
  
  while(!(STOP_SET))
  {	
	res = read(fd, &flag_ST, 1);

	switch (option)
	{
	case START:
		if (flag_ST == F) //ver
			{
				option = FLAG_RCV;
				SET[0] = flag_ST;
			}
		else
			option = START;
		break;

	case FLAG_RCV:
		if (flag_ST == F) //ver
			{
				option = FLAG_RCV;
				SET[0] = flag_ST;
			}
		else if (flag_ST == A) //ver
			{
				option = A_RCV;
				SET[1] = flag_ST;
			}
		else
			option = START;
		break;

	case A_RCV:
		if (flag_ST == F) //ver
			{
				option = FLAG_RCV;
				SET[0] = flag_ST;
			}
		else if (flag_ST == C_SET) //ver
			{
				option = C_RCV;
				SET[2] = flag_ST;
			}
		else
			option = START;
		break;

	case C_RCV:
		if (flag_ST == F) //ver
			{
				option = FLAG_RCV;
				SET[0] = flag_ST;
			}
		else if (flag_ST == BCC) //ver
			{
				option = BCC_OK;
				SET[3] = flag_ST;
			}
		else
			option = START;
		break;

	case BCC_OK:
		if (flag_ST == F) //ver
			{
				option = STOP_ST;
				SET[4] = flag_ST;
			}
		else
			option = START;
		break;

	case STOP_ST:
		STOP_SET = TRUE;
		break;

	default:
		break;
	} 
  }

}

int receive_DISC(int fd, char *DISC_rec) {

  char flag_ST;
  int res;
  int option = START;
  
  while(!(STOP_DISC))
  {	
	res = read(fd, &flag_ST, 1);
	
	int flag = getFlag();
	    if(flag && flag != -1){
		  alarm(0);            
		  setFlag(-1);
		  STOP_DISC = TRUE;
		}

	switch (option)
	{
	case START:
		if (flag_ST == F) //ver
			{
			option = FLAG_RCV;
			DISC_rec[0] = flag_ST;
			}
		else
			option = START;
		break;

	case FLAG_RCV:
		if (flag_ST == F) //ver
			{
			option = FLAG_RCV;
			DISC_rec[0] = flag_ST;
			}
		else if (flag_ST == A) //ver
			{
			option = A_RCV;
			DISC_rec[1] = flag_ST;
			}
		else
			option = START;
		break;

	case A_RCV:
		if (flag_ST == F) //ver
			{
				option = FLAG_RCV;
				DISC_rec[0] = flag_ST;
			}
		else if (flag_ST == C_DISC) //ver
			{
				option = C_RCV;
				DISC_rec[2] = flag_ST;
			}
		else
			option = START;
		break;

	case C_RCV:
		if (flag_ST == F) //ver
			{
				option = FLAG_RCV;
				DISC_rec[0] = flag_ST;
			}
		else if (flag_ST == BCC) //ver
			{
				option = BCC_OK;
				DISC_rec[3] = flag_ST;
			}
		else
			option = START;
		break;

	case BCC_OK:
		if (flag_ST == F) //ver
			{
				option = STOP_ST;
				DISC_rec[4] = flag_ST;
			}
		else
			option = START;
		break;

	case STOP_ST:
		STOP_DISC = TRUE;
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

int check_DISC(char *DISC_rec) {
	int error = 0;
	if (DISC_rec[0] != F || DISC_rec[1] != A || DISC_rec[2] != C_DISC || DISC_rec[3] != (DISC_rec[1]^DISC_rec[2]) || DISC_rec[4] != F)
		error = 1;
	return error;
}

int getStopUA() {
  return STOP_UA;
}

void setStopUA(int st) {
  STOP_UA = st;
}

int getStopSET() {
  return STOP_SET;
}

void setStopSET(int st) {
  STOP_SET = st;
}

int getStopDISC() {
  return STOP_DISC;
}

void setStopDISC(int st) {
  STOP_DISC = st;
}