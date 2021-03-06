#include "link_layer.h"


static int s = 0;

int ll_open(LinkLayer *link_layer) {
    if (link_layer->status == RECEIVER)
		return ll_open_receiver(link_layer);
    else 
		return ll_open_transmitter(link_layer);
}

int ll_close(LinkLayer *link_layer) {
    if (link_layer->status == RECEIVER)
		return ll_close_receiver(link_layer);
    else 
		return ll_close_transmitter(link_layer);
}

int ll_open_receiver(LinkLayer *link_layer) {
    char SET[5];
    char UA[5];
    
    UA[0] = F;
    UA[1] = A;
    UA[2] = C_UA;
    UA[3] = A^C_UA;
    UA[4] = F;

    receive_SET(link_layer->fd, SET);
    
    //printf("FLAGS READ FROM SET: %x, %x, %x, %x, %x\n", SET[0], SET[1], SET[2], SET[3], SET[4]);

    send_UA(link_layer->fd, UA);
    return -1;

}

int ll_open_transmitter(LinkLayer *link_layer) {
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
		
		if(!(check_UA(UA))){
		    //printf("FLAGS READ FROM UA: %x, %x, %x, %x, %x\n\n", UA[0], UA[1], UA[2], UA[3], UA[4]);
		    tries=-1;
		    break;
		}else{
		    tries++;
		}		
	}
	return tries;
}

int ll_close_receiver(LinkLayer *link_layer) {
 
    char DISC[link_layer->maxFrameSize];
    char UA[5];
    char DISC_send[5];
    
    DISC_send[0] = F;
    DISC_send[1] = A;
    DISC_send[2] = C_DISC;
    DISC_send[3] = A^C_DISC;
    DISC_send[4] = F;
    
    int diskSize = 0;
    char dataPacket[link_layer->maxFrameSize]; 

    do {
      setStopFRAME(FALSE);
      diskSize = receive_FRAME(link_layer->fd, DISC, link_layer->maxFrameSize);
      int result = check_I(dataPacket, s, DISC, diskSize);
      if(result == RE_SEND_RR){
      	send_RR(link_layer->fd,s);
      }
    }while(check_DISC(DISC));
    
    
    printf("FLAGS READ WITH SUCCESS FROM DISC: %x, %x, %x, %x, %x\n", DISC[0], DISC[1], DISC[2], DISC[3], DISC[4]);
        
    int tries = getTries();
    fprintf(stderr, "Tries = %d\n", tries);
    
    while(tries <= ATTEMPTS){
		printf("Attempt %d\n", tries);
		tries = getTries();
		
		send_DISC(link_layer->fd, DISC_send);
    		

		setStopUA(FALSE);	
		receive_UA(link_layer->fd, UA);		
		if(!(check_UA(UA))){
			printf("FLAGS READ FROM UA: %x, %x, %x, %x, %x\n\n", UA[0], UA[1], UA[2], UA[3], UA[4]);
			tries=-1;
			break;
		}
		else{
		  tries++;
		}
	}
	return tries;
}

int ll_close_transmitter(LinkLayer *link_layer) {
  
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
			//printf("FLAGS READ FROM DISC: %x, %x, %x, %x, %x\n\n", DISC_rec[0], DISC_rec[1], DISC_rec[2], DISC_rec[3], DISC_rec[4]);
			tries = -1;
			break;
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
	if(tries < 0)
		send_UA(link_layer->fd, UA);
	return tries;
}

int ll_write(LinkLayer *link_layer, int size) {
	char * data_packet = link_layer->dataPacket;
	char frameAdder[3 + 1 + size];

	int current_s = s;

	char C = 0;
	C = C|(s << 5);
	frameAdder[0] = A;
	frameAdder[1] = C;
	frameAdder[2] = frameAdder[0] ^ frameAdder[1];

	int i=0;
	frameAdder[3] = data_packet[0];
	//fprintf(stderr, "data_packet[%d]=%x\n",i, data_packet[i]);
	char bcc_2 = frameAdder[3];
	for (i = 1; i < size; i++) {
		frameAdder[i+3]=data_packet[i];
		//fprintf(stderr, "data_packet[%d]=%x\n",i, (unsigned char) data_packet[i]);
		bcc_2^=data_packet[i];
	}
	frameAdder[size+3] = bcc_2;
	//fprintf(stderr, "bcc_2 %x\n", (unsigned char) bcc_2);
	char frame[(size + 4) * 2 + 2];
	char * stuffedPacket = frame + 1;
	int size_stuffed_packet = bytestuffing(frameAdder, size + 4, stuffedPacket);
	//fprintf(stderr, "size_stuffed_packet=%d size%d\n", size_stuffed_packet, size);
	int frameSize = size_stuffed_packet + 2;
	//fprintf(stderr, "bcc_2 stuffed %x\n",(unsigned char) stuffedPacket[size_stuffed_packet - 1]);


	frame[0] = FLAG;
	frame[frameSize - 1] = FLAG;

	setTries(1);
	int tries = getTries();

	while (tries < link_layer->maxTries) {

		write(link_layer->fd, frame, frameSize); //enviar packet
		alarm(3);
		setFlag(0);

		char answerRR[5];
		setStopRR(FALSE);
		int ans = receive_RR(link_layer->fd, answerRR, current_s); //receber RR
		//printf("FLAGS READ FROM RR: %x, %x, %x, %x, %x\n\n", answerRR[0], answerRR[1], answerRR[2], answerRR[3], answerRR[4]);
		tries = getTries();
		fprintf(stderr, "%d\n", ans);
		if (ans < 0) {
			continue;
		}
		s = ans;
		break;	
	}


	if (tries == link_layer->maxTries)
		return -1;

	return frameSize;
}

int ll_read(LinkLayer * link_layer) {
	char *dataPacket = link_layer->dataPacket;
	int dataPacketSize = 0;
	int validated = FALSE;
	char currC;
	char UA[5];


	while(!validated){
		char frame[link_layer->maxFrameSize];

		setStopFRAME(FALSE);
		int frameSize = receive_FRAME(link_layer->fd, frame, link_layer->maxFrameSize);
		
		dataPacketSize = check_I(dataPacket, s, frame, frameSize);
		fprintf(stderr, "check_I %d\n", dataPacketSize);
		switch(dataPacketSize){
			case FAILED:
				break;
			case RE_SEND_RR:
				currC = (s<<5);
				send_RR(link_layer->fd,currC);
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
	fprintf(stderr, "Vai enviar o RR com s = %d\n", s ^ 0x1);
	send_RR(link_layer->fd,(s ^ 0x1));
	s ^= 0x01;

	return dataPacketSize;
}



void ll_init(LinkLayer * newLinkLayer, char * port, int baudRate, unsigned int timeout, unsigned int maxTries, unsigned int maxFrameSize, int status){
    struct termios * oldtio = malloc(sizeof(struct termios));
    struct termios newtio;

    newLinkLayer->port = port;
	
	int fd = open(port, O_RDWR | O_NOCTTY );
	if (fd < 0) {
		fprintf(stderr, "Error opening port %s\n", port);
		exit (-1);
	}


	if ( tcgetattr(fd,oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }


    bzero(&newtio, sizeof(newtio));

    newtio.c_cflag = baudRate | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */

	/* 
	VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
	leitura do(s) próximo(s) caracter(es)
	*/


    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    newLinkLayer->fd = fd;
    newLinkLayer->baudRate = baudRate;
    newLinkLayer->port = port;
    newLinkLayer->timeout = timeout;
    newLinkLayer->maxTries = maxTries;
    newLinkLayer->maxFrameSize = maxFrameSize;
    newLinkLayer->status = status;
    newLinkLayer->oldtio = oldtio;

    newLinkLayer->dataPacket = malloc( (maxFrameSize - 2)/2 - (3 + 1));

    fprintf(stderr,"New termios structure set\n");

}


void ll_end(LinkLayer * linkLayer){
	if ( tcsetattr(linkLayer->fd,TCSANOW,linkLayer->oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }
   close(linkLayer->fd);
}
