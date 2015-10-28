#include "bstuffing.h"
#include "utils.h"

int bytestuffing(char * dataPacket, int size, char * stuffedPacket){
	int i;
	int j;
	for(i = 0, j = 0;i < size; i++, j++){
		char data = dataPacket[i];
		if(data == FLAG || data == ESC){
			stuffedPacket[j] = ESC;
			j++;
			stuffedPacket[j] = data^SUBS;
		}else{
			stuffedPacket[j] = dataPacket[i];			
		}
	}

	return j;
}

int bytedestuffing(char * stuffedPacket, int size, char * dataPacket){
	int i;
	int j;
	for(i = 0, j = 0; j < size; i++, j++){
		if(stuffedPacket[j] == ESC){
			j++;
			dataPacket[i] = stuffedPacket[j]^SUBS;
		}
		dataPacket[i] = stuffedPacket[j];
	}
	return j;
}