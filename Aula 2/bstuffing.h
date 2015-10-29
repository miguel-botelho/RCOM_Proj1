#ifndef ___BSTUFFING
#define ___BSTUFFING

#include "utils.h"
#include <stdio.h>

/**
 * Applies byte stuffing to a given array of chars
 * @param dataPacket pointer to the packet to be stuffed
 * @param size size of the dataPacket array
 * @param stuffedPacket pointer to where the data packet will be stored after byte stuffing
 * @return size of the stuffed packet
 */
int bytestuffing(char * dataPacket, int size, char * stuffedPacket);


/**
 * Removes the byte stuffing to a given array of chars
 * @param stuffedPacket pointer to the packet to be destuffed 
 * @param size size of stuffedPacket
 * @param dataPacket pointer to where the destuffed data packet will be stored
 * @return size of the destuffed packet
 */
 int bytedestuffing(char * stuffedPacket, int size, char * dataPacket);

#endif /* ___BSTUFFING */