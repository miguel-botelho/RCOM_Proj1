#ifndef ___APPLAYER
#define ___APPLAYER

#include "utils.h"

void app_layer(LinkLayer *link_layer, char** argv);

void app_layer_receiver(LinkLayer *link_layer);
void app_layer_transmitter(LinkLayer *link_layer, char** argv);


#endif /* ___APPLAYER */