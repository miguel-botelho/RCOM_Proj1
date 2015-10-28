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

	//ll_open(link_layer);

	// ficar a ler
	setTries(1);

	//ll_close(link_layer);
}

void app_layer_transmitter(LinkLayer *link_layer, char * file_name) {

	int file;
	char * file_buffer;
    struct stat * file_stat = mallo(sizeof(struct stat));

    file = open(file_name, O_RDONLY);

    fstat(file, file_stat);

    file_buffer = malloc(file_stat);

    read(file, &file_buffer, file_stat.st_size);

    printf("tamanho!: %lld\n", (long long)file_stat->st_size);

	//ll_open(link_layer);

	char controlPacketStart[3 + 4 + 3 + strlen(file_name) + 1];
	char controlPacketEnd;


	controlPacket_size(file_stat, controlPacket);
	
	controlPacket_name(file_stat, &controlPacket[3 + controlPacket[2]], file_name);

	setTries(1);
	  
	//ll_close(link_layer);

	double cenas = ceil(1.2);
	printf("t: %f\n", cenas);
}

void controlPacket_size(struct stat * file_stat, char * controlPacket){
	controlPacketStart[0] = 0x1;
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

	memcpy(&controlPacketStart[i + 3 + controlPacketStart[2] + 3], argv[2], strlen(name) +1);
}