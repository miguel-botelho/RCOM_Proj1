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

/**
 * @struct FileInfo
 * @brief FileInfo contains all of the necessary data to send a file
 * @var FileInfo::size
 * Member 'size' is the size of the file
 * @var FileInfo::name
 * Member 'name' is the name of the file
 * @var FileInfo::fd
 * Member 'fd' is the connection to be written
 * @var sequenceNumber
 * Member 'sequenceNumber' is the number of the sequence
 */
typedef struct {
	int size;
	char * name;
	int fd;
	int sequenceNumber;
}FileInfo;

/**
 * Calls the application layer with a file name and the link layer. It calls either the receiver or transmitter application layer.
 * @param link_layer link layer which contains all the info needed
 * @param file_name the name of the file that is going to be sent and read
 */
void app_layer(LinkLayer *link_layer, char* file_name);

/**
 * The application layer from the part of the receiver.
 * @param link_layer link layer which contains all the info needed
 */
void app_layer_receiver(LinkLayer *link_layer);

/**
 * The application layer from the part of the transmitter.
 * @param link_layer link layer which contains all the info needed
 * @param file_name the name of the file that is going to be sent
 */
void app_layer_transmitter(LinkLayer *link_layer, char* file_name);

/**
 * Fills the controlPacket with the info needed for the size of the file sent
 * @param file_stat struct that contains all of the info on a file
 * @param controlPacket the array that's going to be changed
 */
void controlPacket_size(struct stat * file_stat, char * controlPacket);

/**
 * Fills the controlPacket with the info needed for the name of the file sent
 * @param file_stat struct that contains all of the info on a file
 * @param controlPacket the array that's going to be changed
 * @param name the name of the file
 */
void controlPacket_name(struct stat * file_stat, char * controlPacket, char * name);

/**
 * Sends a packet.
 * @param link_layer link layer which contains all of the info needed
 * @param packet the packet that is going to be sent
 * @param size
 * @param i
 * @return 0 for success, -1 for insuccess
 */
int al_sendPacket(LinkLayer * link_layer, char * packet, int size, int i);

/**
 * Sends the whole file.
 * @param link_layer link layer which contains all of the info needed
 * @param file_buffer the file stored in a char *
 * @param size
 * @return 0 for success, -1 for insuccess
 */
int al_sendFile(LinkLayer * link_layer, char * file_buffer, int size);

/**
 * Receives the file.
 * @param link_layer link layer which contains all of the info needed
 * @param file struct that contains all of the info needed to send and read a file
 * @return 0 for success, -1 for insuccess
 */
int al_readFile(LinkLayer * link_layer, FileInfo * file);

/**
 * Reads the first control packet.
 * @param link_layer link layer which contains all of the info needed
 * @param file struct that contains all of the info needed to send and read a file
 * @return 0 for success, -1 for insuccess
 */
int al_readInitControlPacket(LinkLayer * link_layer, FileInfo * file);

/**
 * Reads the file size.
 * @param dataPacket
 * @param fieldLength
 * @return 0 for success, -1 for insuccess
 */
int readFileSize(char * dataPacket, int * fieldLength);

/**
 * Reads the file name.
 * @param dataPacket
 * @param fieldLength
 * @return 0 for success, -1 for insuccess
 */
char * readFileName(char * dataPacket, int * fieldLength);

/**
 * Checks if the last control packet is correct.
 * @param link_layer link layer which contains all of the info needed
 * @param file all of the info on the file
 * @param packetSize
 * @return 0 for success, -1 for insuccess
 */
int al_checkEndCtrlPacket(LinkLayer * link_layer, FileInfo * file, int packetSize);

/**
 * Reads the size of the file.
 * @param dataPacket
 * @param fieldLength
 * @return 0 for success, -1 for insuccess
 */
int readFileSize(char * dataPacket, int * fieldLength);

/**
 * Reads the information on the packet.
 * @param link_layer link layer which contains all of the info needed
 * @param file all of the info on the file
 * @param packetSize
 * @param bytesRead number of the bytes read
 * @return 0 for success, -1 for insuccess
 */
int readInformationPacket(LinkLayer * link_layer, FileInfo * file, int packetSize, int bytesRead);

/**
 * Returns the packet size.
 * @param maxFrameSize the maximum size of the frame
 * @return packet size
 */
int getPacketSize(int maxFrameSize);

#endif /* ___APPLAYER */
