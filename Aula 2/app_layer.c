#include "app_layer.h"
#include "link_layer.h"
#include "state.h"
#include "alarm.h"
#include "utils.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>


void app_layer(LinkLayer *link_layer, char* file_name) {
	if (link_layer->status == TRANSMITTER)
		app_layer_transmitter(link_layer, file_name);
	else if (link_layer->status == RECEIVER)
		app_layer_receiver(link_layer);
}

void app_layer_receiver(LinkLayer *link_layer) {

	ll_open(link_layer);

	//ficar a ler

	setTries(1);

	ll_close(link_layer);
}

char * al_readFile(LinkLayer * link_layer, char * file){

	//ler pacote inicial

	//reservar memória para ficheiro

	//ler packets até receber um pacote final

	//verificar se o numero de bytes que leu corresponde ao que era suposto

	//retornar apontador ficheiro

}

void app_layer_transmitter(LinkLayer *link_layer, char * file_name) {

	int file;
	char * file_buffer;
    struct stat * file_stat = mallo(sizeof(struct stat));

    file = open(file_name, O_RDONLY);

    fstat(file, file_stat);

    file_buffer = malloc(file_stat);

    read(file, &file_buffer, file_stat->st_size);

    printf("tamanho!: %lld\n", (long long)file_stat->st_size);

	//ll_open(link_layer);

	char controlPacket[3 + 4 + 3 + strlen(file_name) + 1];

	controlPacketStart[0] = C_START;
	controlPacket_size(file_stat, controlPacket);
	controlPacket_name(file_stat, &controlPacket[3 + controlPacket[2]], file_name);
	memcpy(link_layer->dataPacket, controlPacket, sizeof(controlPacket));
	llwrite(link_layer,sizeof(controlPacket));


	int sentBytes = al_sendFile(link_layer, file_buffer, file_stat->st_size);


	controlPacket[0] = C_END;
	memcpy(link_layer->dataPacket, controlPacket, sizeof(controlPacket));
	llwrite(link_layer,sizeof(controlPacket));


	setTries(1);
	  
	ll_close(link_layer);

	double cenas = ceil(1.2);
	printf("t: %f\n", cenas);
}

int al_sendFile(LinkLayer * link_layer, char * file_buffer, int size){
	int defautlPacketSize = getPacketSize(link_layer->maxFrameSize);
	int numPackets = size/defautlPacketSize;

	int sentBytes = 0;
	int i = 0;
	for(; i < numPackets; i ++){
		int packetSize;
		if((size - sentBytes) >= defautlPacketSize )
			packetSize = defautlPacketSize;
		else
			packetSize = size - defautlPacketSize;
		if(al_sendPacket(link_layer, file_buffer[i*packetSize], packetSize) < 0, i){
			fprintf(stderr, "Error sending dataPacket i=%d\n",i );	
			exit(-1);
		}

		sentBytes += packetSize;
	}
	
	return sentBytes;
}

int getPacketSize(int maxFrameSize){
	return (maxFrameSize - NUMBER_FLAGS)/2 - FRAME_HEADER_SIZE - PACKET_HEADER_SIZE;
}

int al_sendPacket(LinkLayer * link_layer, char * packet, int size,i){
	link_layer->dataPacket[0] = C_DATA;
	link_layer->dataPacket[1] = i;
	link_layer->dataPacket[2] = size >> 4;
	link_layer->dataPacket[3] = size % 16;

	memcpy(&(link_layer->dataPacket[4]), packet, size);

	return llwrite(link_layer, size + PACKET_HEADER_SIZE);
}

void controlPacket_size(struct stat * file_stat, char * controlPacket){
	controlPacketStart[1] = 0; //tamanho ficheiro
	controlPacketStart[2] = 4; // 4 bytes é o máximo
	int i = 0;
	for(;i < controlPacketStart[2]; i++){
		controlPacketStart[i+3] = 0xFF & (file_stat->st_size >> (8 * i));
	}
}

void controlPacket_name(struct stat * file_stat, char * controlPacket, char * name){
	controlPacketStart[0] = 1;
	controlPacketStart[1] = 1; 					//nome do ficheiro
	controlPacketStart[2] = strlen(name) + 1; 	//+ 1 porque /0

	memcpy(&controlPacketStart[3], name, strlen(name) +1);
}