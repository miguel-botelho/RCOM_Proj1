#include "link_layer.h"
#include "utils.h"
#include "state.h"
#include "alarm.h"

#include <unistd.h>
#include <stdio.h>

static s = 0;

void ll_open(int flag, int fd) {
    if (flag == RECEIVER)
	ll_open_receiver(fd);
    else if (flag == TRANSMITTER)
	ll_open_transmitter(fd);
}

void ll_close(int flag, int fd) {
    if (flag == RECEIVER)
	ll_close_receiver(fd);
    else if (flag == TRANSMITTER)
	ll_close_transmitter(fd);
}

void ll_open_receiver(int fd) {
    char SET[5];
    char UA[5];
    
    UA[0] = F;
    UA[1] = A;
    UA[2] = C_UA;
    UA[3] = A^C_UA;
    UA[4] = F;

    receive_SET(fd, SET);
    
    printf("FLAGS READ FROM SET: %x, %x, %x, %x, %x\n", SET[0], SET[1], SET[2], SET[3], SET[4]);

    send_UA(fd, UA);

    sleep(1);
}

void ll_open_transmitter(int fd) {
	int tries = getTries();
	char UA[5];
	char SET[5];
	SET[0] = F;
	SET[1] = A;
	SET[2] = C_SET;
	SET[3] = BCC_SET;
	SET[4] = F;
	
	/* WRITE SET */
	
	while(tries <= ATTEMPTS){
		printf("Attempt %d\n", tries);
		tries = getTries();
		
		send_SET(fd, SET);

		setStopUA(FALSE);	
		
		receive_UA(fd, UA);
		
		if(!(check_UA(UA)))
		{
		    printf("FLAGS READ FROM UA: %x, %x, %x, %x, %x\n\n", UA[0], UA[1], UA[2], UA[3], UA[4]);
		    tries=99;
		}
		else
		{
		    tries++;
		}
				
	}

	sleep(1);
}

void ll_close_receiver(int fd) {
 
    char DISC[5];
    char UA[5];
    char DISC_send[5];
    
    DISC_send[0] = F;
    DISC_send[1] = A;
    DISC_send[2] = C_DISC;
    DISC_send[3] = A^C_DISC;
    DISC_send[4] = F;
    

    do {
      setStopDISC(FALSE);
      receive_DISC(fd, DISC);
    }while(check_DISC(DISC));
    
    
    printf("FLAGS READ WITH SUCCESS FROM DISC: %x, %x, %x, %x, %x\n", DISC[0], DISC[1], DISC[2], DISC[3], DISC[4]);
    
    // por while
    
    int tries = getTries();	
    
    while(tries <= ATTEMPTS){
		printf("Attempt %d\n", tries);
		tries = getTries();
		
		send_DISC(fd, DISC_send);
    		

		setStopUA(FALSE);	
		
		receive_UA(fd, UA);		
		
		if(!(check_UA(UA)))
		{
			printf("FLAGS READ FROM UA: %x, %x, %x, %x, %x\n\n", UA[0], UA[1], UA[2], UA[3], UA[4]);
			tries=99;
		}
		else
		{
		  tries++;
		}
	}
}

void ll_close_transmitter(int fd) {
  
    char DISC[5];
    char DISC_rec[5];
    char UA[5];
    
    DISC[0] = F;
    DISC[1] = A;
    DISC[2] = C_DISC;
    DISC[3] = DISC[1]^DISC[2];
    DISC[4] = F;


    UA[0] = F;
    UA[1] = A;
    UA[2] = C_UA;
    UA[3] = A^C_UA;
    UA[4] = F;
    
    int tries = getTries();
    
	while(tries <= ATTEMPTS){
		printf("Attempt %d\n", tries);
		
		send_DISC(fd, DISC);
		
		setStopDISC(FALSE);
		
		receive_DISC(fd, DISC_rec);
		
		if(!(check_DISC(DISC_rec)))
		{
			printf("FLAGS READ FROM DISC: %x, %x, %x, %x, %x\n\n", DISC_rec[0], DISC_rec[1], DISC_rec[2], DISC_rec[3], DISC_rec[4]);
			setTries(99);
		}
		else
		{
			if (tries == getTries())
			{
			    setTries(tries++);
			}
		}
		
		tries = getTries();
		
	
	}	
	
	send_UA(fd, UA);
}

int ll_write(settings set, char *	data_packet, int size) {

	char frameAdder[3 + 1 + size];

	int current_s = s;

	char C = 0;
	C = C|(S << 5);
	frameAdder[0] = A;
	frameAdder[1] = C;
	frameAdder[2] = frameAdder[1] ^ frameAdder[2];

	int i = 1;
	frameAdder[3] = data_packet[0];

	char bcc_2 = frameAdder[3];
	for (i; i < size + 3; i++) {
		frameAdder[i+3]=data_packet[i];
		bcc_2^=data_packet[i];
	}
	frameAdder[size+3] = bcc_2;

	char frame[(size + 4) * 2 + 1];
	char * stuffedPacket = frame + sizeof(*frame);
	int size_stuffed_packet = bytestuffing(frameAdder, size + 4, stuffedPacket);

	int frameSize = size_stuffed_packet + 2;

	frame[0] = FLAG;
	frame[frameSize - 1] = FLAG;

	setTries(1);
	int tries = getTries();

	while (tries < set->maxTries) {

		write(set->fd, frame, frameSize); //enviar packet

		alarm(3);
		setFlag(1);

		char answerRR[5];

		char ans = receive_RR(set->fd, answerRR, current_s); //receber RR

		tries = getTries();

		if (ans == -1) {
			continue;
		}
		s = ans;
		break;	
	}


	if (tries == set->maxTries)
		return -1;

	return frameSize;
}

int ll_read(settings set, char *data_packet) {

	// ler
	int frameSize = set->maxPacketSize * 2 + (3 + 1) * 2 + 2; // 3 + 1 = A, C, BCC1, BCC2 * 2 devido ao stuffing. 
														  	  // + 2 = 2 flags (inicial e final)

	char frame[frameSize];

	receive_I(set->fd, frame, frameSize);

	// remover flags

	// destuffing

	// validar A, C, BCC1,2

	// remover frameHeader

	// fazer store e retornar o tamanho
}