#ifndef ___LINKLAYER
#define ___LINKLAYER

#include "utils.h"
#include "state.h"
#include "alarm.h"
#include "bstuffing.h"

#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <strings.h>

/**
 * @struct LinkLayer
 * @brief Link Layer has all the data needed to set a connection
 * @var LinkLayer::port
 * Member 'port' states the name of the serial port
 * @var LinkLayer::fd
 * Member 'fd' opens the connection to the transmitter/receiver
 * @var LinkLayer::baudRate
 * Member 'baudRate' is the rate at which the data is sent
 * @var LinkLayer::timeout
 * Member 'timeout' is the time it takes for the connection to shut down
 * @var LinkLayer::maxTries
 * Member 'maxTries' is the maximum number of tries available
 * @var LinkLayer::maxFrameSize
 * Member 'maxFrameSize' is the maximum size of the frame
 * @var LinkLayer::maxPacketSize
 * Member 'maxPacketSize' is the maximum size of the packets to be sent
 * @var LinkLayer::status
 * Member 'status' is a flag that states if it's the receiver or transmitter
 * @var LinkLayer::oldtio
 * Member 'oldtio' previous settings of the serial port
 * @var LinkLayer::dataPacket
 * Member 'dataPacket' is the packet of data to be sent
 */
typedef struct {
	char * port; 						//Dispositivo /dev/ttySx, x = 0, 1
	int fd;
	int baudRate; 						//Velocidade de transmissão
	unsigned int timeout; 				//Valor do temporizador: 1 s
	unsigned int maxTries; 				//Número de tentativas em caso de falha
	unsigned int maxFrameSize;
	unsigned int maxPacketSize;
	int status;
	struct termios * oldtio;
	char * dataPacket;
} LinkLayer;

/**
 * Opens the connection by sending and receiving a number of frames.
 * @param link_layer link layer which contains all the info needed
 * @return 0 if success, -1 in insuccess
 */
int ll_open(LinkLayer *link_layer);

/**
 * Closes the connection by sending and receiving a number of frames.
 * @param link_layer link layer which contains all the info needed
 * @return 0 if success, -1 in insuccess
 */
int ll_close(LinkLayer *link_layer);

/**
 * Opens the connection on the side of the receiver.
 * @param link_layer link layer which contains all the info needed
 * @return 0 if success, -1 if unsuccess
 */
int ll_open_receiver(LinkLayer *link_layer);

/**
 * Opens the connection on the side of the transmitter.
 * @param link_layer link layer which contains all the info needed
 * @return 0 if success, -1 if unsuccess
 */
int ll_open_transmitter(LinkLayer *link_layer);

/**
 * Closes the connection on the side of the receiver.
 * @param link_layer link layer which contains all the info needed
 * @return 0 if success, -1 if unsuccess
 */
int ll_close_receiver(LinkLayer *link_layer);

/**
 * Closes the connection on the side of the transmitter.
 * @param link_layer link layer which contains all the info needed
 * @return 0 if success, -1 if unsuccess
 */
int ll_close_transmitter(LinkLayer *link_layer);

/**
 * Writes a frame.
 * @param link_layer which contains all the info needed
 * @param size
 * @return 0 if success, -1 if unsuccess
 */
int ll_write(LinkLayer *link_layer, int size);

/**
 * Reads a frame.
 * @param link_layer which contains all the info needed
 * @return 0 if success, -1 in unsuccess
 */
int ll_read(LinkLayer *link_layer);

/**
 * Initiates the connection on the link layer, setting the serial port.
 * @param newLinkLayer the link layer to be setted
 * @param port[20] the serial port name
 * @param baudRate the rate at which the data is sent
 * @param timeout the time it takes to close the connection (in seconds)
 * @param maxTries maximum number of tries
 * @param maxFrameSize maximum size of the frame
 * @param status a flag that states if it's the receiver or transmitter
 */
void ll_init(LinkLayer * newLinkLayer, char port[20], int baudRate, unsigned int timeout, unsigned int maxTries, unsigned int maxFrameSize, int status);

/**
 * Closes the connection on the link layer the settings.
 * @param linkLayer the link layer
 */
void ll_end(LinkLayer * linkLayer);

#endif /* __LINKLAYER */


