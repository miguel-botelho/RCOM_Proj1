#ifndef ___APPLAYER
#define ___APPLAYER

#include "utils.h"

#define C_START 1
#define C_END 2
#define C_DATA 0

#define F_SIZE 0
#define F_NAME 1

#define NUMBER_FLAGS 2;
#define FRAME_HEADER_SIZE 3+1;
#define PACKET_HEADER_SIZE 4;


#define 

void app_layer(LinkLayer *link_layer, char** argv);

void app_layer_receiver(LinkLayer *link_layer);
void app_layer_transmitter(LinkLayer *link_layer, char** argv);

void controlPacket_size(struct stat * file_stat, char * controlPacket);
void controlPacket_name(struct stat * file_stat, char * controlPacket, char * name);

int al_sendPacket(LinkLayer * link_layer, char * packet, int size);
int al_sendFile(LinkLayer * link_layer, char * file_buffer, int size);

void al_readFile(LinkLayer * link_layer, char * file);


#endif /* ___APPLAYER */