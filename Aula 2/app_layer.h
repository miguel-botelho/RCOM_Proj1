#ifndef ___APPLAYER
#define ___APPLAYER

#include "utils.h"
#include "link_layer.h"
#include "state.h"
#include "alarm.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>

#define C_START 1
#define C_END 2
#define C_DATA 0

#define F_SIZE 0
#define F_NAME 1

#define NUMBER_FLAGS 2
#define FRAME_HEADER_SIZE 3+1
#define PACKET_HEADER_SIZE 4


typedef struct {
	int size;
	char * name;
	char * file;
	int sequenceNumber;
}FileInfo;
 

void app_layer(LinkLayer *link_layer, char* file_name);

void app_layer_receiver(LinkLayer *link_layer);
void app_layer_transmitter(LinkLayer *link_layer, char* file_name);

void controlPacket_size(struct stat * file_stat, char * controlPacket);
void controlPacket_name(struct stat * file_stat, char * controlPacket, char * name);

int al_sendPacket(LinkLayer * link_layer, char * packet, int size, int i);
int al_sendFile(LinkLayer * link_layer, char * file_buffer, int size);

int al_readFile(LinkLayer * link_layer, FileInfo * file);
int al_readInitControlPacket(LinkLayer * link_layer, FileInfo * file);
int readFileSize(char * dataPacket, int * fieldLength);
char * readFileName(char * dataPacket, int * fieldLength);
int al_checkEndCtrlPacket(LinkLayer * link_layer, FileInfo * file, int packetSize);
int readFileSize(char * dataPacket, int * fieldLength);
int readInformationPacket(LinkLayer * link_layer, FileInfo * file, int packetSize, int bytesRead);

int getPacketSize(int maxFrameSize);

#endif /* ___APPLAYER */