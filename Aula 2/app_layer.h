#ifndef ___APPLAYER
#define ___APPLAYER

#include "utils.h"

#define C_START 1
#define C_END 2

#define F_SIZE 0
#define F_NAME 1

#define 

void app_layer(LinkLayer *link_layer, char** argv);

void app_layer_receiver(LinkLayer *link_layer);
void app_layer_transmitter(LinkLayer *link_layer, char** argv);

void controlPacket_size(struct stat * file_stat, char * controlPacket);
void controlPacket_name(struct stat * file_stat, char * controlPacket, char * name);

#endif /* ___APPLAYER */