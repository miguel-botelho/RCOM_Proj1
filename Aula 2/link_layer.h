#ifndef ___LINKLAYER
#define ___LINKLAYER

#include "utils.h"

#define MAX_FRAME_SIZE 100										//TEM QUE SER PAR
#define MAX_PACKE_SIZE ((MAX_FRAME_SIZE - 2 - 2*(3 + 1)) / 2)

void ll_open(LinkLayer *link_layer);
void ll_close(LinkLayer *link_layer);

void ll_open_receiver(LinkLayer *link_layer);
void ll_open_transmitter(LinkLayer *link_layer);

void ll_close_receiver(LinkLayer *link_layer);
void ll_close_transmitter(LinkLayer *link_layer);

int ll_write(LinkLayer *link_layer, char *data_packet, int size);

int ll_read(LinkLayer *link_layer, char *data_packet);

#endif /* __LINKLAYER */


