#include "link_layer.h"


#include "state.h"

#include "alarm.h"
#include "bstuffing.h"

#include <unistd.h>
#include <stdio.h>

static int s = 0;

void ll_open(int flag, LinkLayer *link_layer) {
    if (flag == RECEIVER)
	ll_open_receiver(link_layer);
    else if (flag == TRANSMITTER)
	ll_open_transmitter(link_layer);
}

void ll_close(int flag, LinkLayer *link_layer) {
    if (flag == RECEIVER)
	ll_close_receiver(link_layer);
    else if (flag == TRANSMITTER)
	ll_close_transmitter(link_layer);
}

void ll_open_receiver(LinkLayer *link_layer) {
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

void ll_open_transmitter(LinkLayer *link_layer) {
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

void ll_close_receiver(LinkLayer *link_layer) {
 
    char DISC[MAX_FRAME_SIZE];
    char UA[5];
    char DISC_send[5];
    
    DISC_send[0] = F;
    DISC_send[1] = A;
    DISC_send[2] = C_DISC;
    DISC_send[3] = A^C_DISC;
    DISC_send[4] = F;
    
    int diskSize = 0;
    char dataPacket[MAX_FRAME_SIZE]; // querias por int aqui? ok...

    do {
      setStopDISC(FALSE);
      diskSize = receive_FRAME(link_layer->fd, DISC);
      int result = check_I(dataPacket, s, DISC, diskSize, link_layer);
      if(result == RE_SEND_RR){
      	send_RR(link_layer->fd,s);
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

void ll_close_transmitter(LinkLayer *link_layer) {
  
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

int ll_write(LinkLayer *link_layer, char * data_packet, int size) {

	char frameAdder[3 + 1 + size];

	int current_s = s;

	char C = 0;
	C = C|(s << 5);
	frameAdder[0] = A;
	frameAdder[1] = C;
	frameAdder[2] = frameAdder[1] ^ frameAdder[2];

	int i;
	frameAdder[3] = data_packet[0];

	char bcc_2 = frameAdder[3];
	for (i = 1; i < size + 3; i++) {
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

int ll_read(LinkLayer *link_layer, char *dataPacket) {
	int dataPacketSize = 0;
	int validated = FALSE;
	char currC;
	char UA[5];


	while(!validated)
	{
		char frame[MAX_FRAME_SIZE];

		setStopFRAME(FALSE);
		int frameSize = receive_FRAME(link_layer->fd, frame);

		dataPacketSize = check_I(dataPacket, s, frame, frameSize, link_layer);

		switch(dataPacketSize){
			case FAILED:
				break;
			case RE_SEND_RR:
				currC = (s<<5);
				send_RR(link_layer->fd,currC); //queres enviar que trama? WTF
				break;
			case RE_SEND_SET:
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

		send_RR(link_layer->fd,(s ^ 0x1));
		s ^= 0x01;

		return dataPacketSize;
}