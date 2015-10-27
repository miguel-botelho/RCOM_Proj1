#include "link_layer.h"
#include "utils.h"
#include "state.h"
#include "alarm.h"
#include "bstuffing.h"

#include <unistd.h>
#include <stdio.h>

static s = 0;

void ll_open(int flag, LinkLayer_t link_layer) {
    if (flag == RECEIVER)
	ll_open_receiver(link_layer->fd);
    else if (flag == TRANSMITTER)
	ll_open_transmitter(link_layer->fd);
}

void ll_close(int flag, LinkLayer_t link_layer) {
    if (flag == RECEIVER)
	ll_close_receiver(link_layer->fd);
    else if (flag == TRANSMITTER)
	ll_close_transmitter(link_layer->fd);
}

void ll_open_receiver(LinkLayer_t link_layer) {
    char SET[5];
    char UA[5];
    
    UA[0] = F;
    UA[1] = A;
    UA[2] = C_UA;
    UA[3] = A^C_UA;
    UA[4] = F;

    receive_SET(link_layer->fd, SET);
    
    printf("FLAGS READ FROM SET: %x, %x, %x, %x, %x\n", SET[0], SET[1], SET[2], SET[3], SET[4]);

    send_UA(link_layer->fd, UA);

    sleep(1);
}

void ll_open_transmitter(LinkLayer_t link_layer) {
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
		
		send_SET(link_layer->fd, SET);

		setStopUA(FALSE);	
		
		receive_UA(link_layer->fd, UA);
		
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

void ll_close_receiver(LinkLayer_t link_layer) {
 
    char DISC[MAX_FRAME_SIZE];
    char UA[5];
    char DISC_send[5];
    
    DISC_send[0] = F;
    DISC_send[1] = A;
    DISC_send[2] = C_DISC;
    DISC_send[3] = A^C_DISC;
    DISC_send[4] = F;
    
    int diskSize = 0;
    int dataPacket[MAX_FRAME_SIZE];

    do {
      setStopDISC(FALSE);
      diskSize = receive_FRAME(link_layer->fd, DISC);
      int result = check_I(DISK, diskSize, dataPacket, s);
      if(result == RE_SEND_RR){
      	send_RR(s);
      }
    }while(check_DISC(DISC));
    
    
    printf("FLAGS READ WITH SUCCESS FROM DISC: %x, %x, %x, %x, %x\n", DISC[0], DISC[1], DISC[2], DISC[3], DISC[4]);
    
    // por while
    
    int tries = getTries();	
    
    while(tries <= ATTEMPTS){
		printf("Attempt %d\n", tries);
		tries = getTries();
		
		send_DISC(link_layer->fd, DISC_send);
    		

		setStopUA(FALSE);	
		
		receive_UA(link_layer->fd, UA);		
		
		if(!(check_UA(UA))){
			printf("FLAGS READ FROM UA: %x, %x, %x, %x, %x\n\n", UA[0], UA[1], UA[2], UA[3], UA[4]);
			tries=99;
		}
		else{
		  tries++;
		}

		//TODO esperar por I e enviar RR com nr a seguir
	}
}

void ll_close_transmitter(LinkLayer_t link_layer) {
  
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
		
		send_DISC(link_layer->fd, DISC);
		
		setStopDISC(FALSE);
		
		receive_DISC(link_layer->fd, DISC_rec);
		
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
	
	send_UA(link_layer->fd, UA);
}

int ll_write(linkLayer_t link_layer, char * data_packet, int size) {

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

	while (tries < link_layer->maxTries) {

		write(link_layer->fd, frame, frameSize); //enviar packet

		alarm(3);
		setFlag(1);

		char answerRR[5];

		char ans = receive_RR(link_layer->fd, answerRR, current_s); //receber RR

		tries = getTries();

		if (ans == -1) {
			continue;
		}
		s = ans;
		break;	
	}


	if (tries == link_layer->maxTries)
		return -1;

	return frameSize;
}

int ll_read(linkLayer_t link_layer, char *dataPacket) {
	int dataPacketSize = 0;
	int validated = FALSE;
	while(!validated){
//		int maxFrameSize = link_layer->maxPacketSize * 2 + (3 + 1) * 2 + 2; // 3 + 1 = A, C, BCC1, BCC2 * 2 devido ao stuffing. 
															  	  // + 2 = 2 flags (inicial e final)

		char frame[MAX_FRAME_SIZE];

		STOP_FRAME = FALSE;
		int frameSize = receive_FRAME(link_layer->fd, frame, maxFrameSize);


		dataPacketSize = check_I(char * stuffedPacket, int stuffedPacketSize, char * dataPacket, int s);

		switch(dataPacketSize){
			case FAILED:
				break;
			case RE_SEND_RR:
				char currC = (s<<5);
				send_RR(currC);
				break;
			case RE_SEND_SET:
				char UA[5];
			    UA[0] = F;
			    UA[1] = A;
			    UA[2] = C_UA;
			    UA[3] = A^C_UA;
			    UA[4] = F;

				send_UA(link_layer->fd,UA);
				break;
			default:
				validated = TRUE;
		}


	}

		send_RR(s ^ 0x1);
		s ^= 0x01;

		return dataPacketSize;
}