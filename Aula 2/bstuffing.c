#include "bstuffing.h"

int bytestuff(char * dataPacket, int size, char * stuffedPacket){
	int i;
	int j;
	for(i = 0, j = 0;i < size; i++, j++){
		if(dataPacket[i] == FLAG || dataPacket[i] == ESC){
			stuffedPacket[j] = ESC;
			j++;
		}
		stuffedPacket[j] = dataPacket[i];			
	}

	return j;
}

int destuffing(char * stuffedPacket, int size, char * dataPacket){
	int i;
	int j;
	for(i = 0, j = 0; j < size; i++; j++){
		if(stuffedPacket[j] == ESC)
			j++;
		dataPacket[i] = stuffedPacket[j];
	}
}