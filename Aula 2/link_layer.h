#ifndef ___LINKLAYER
#define ___LINKLAYER

#define MAX_FRAME_SIZE 100										//TEM QUE SER PAR
#define MAX_PACKE_SIZE (MAX_FRAME_SIZE - 2 - 2*(3 + 1)) / 2

struct linkLayer_t {
	char port[20]; 						/*Dispositivo /dev/ttySx, x = 0, 1*/
	int fd;
	int baudRate; 						/*Velocidade de transmissão*/
	unsigned int sequenceNumber;		/*Número de sequência da trama: 0, 1*/
	unsigned int timeout; 				/*Valor do temporizador: 1 s*/
	unsigned int maxTries; 				/*Número de tentativas em caso de falha*/
};

void ll_open(int flag, linkLayer_t link_layer);
void ll_close(int flag, linkLayer_t link_layer;

void ll_open_receiver(linkLayer_t link_layer);
void ll_open_transmitter(linkLayer_t link_layer);

void ll_close_receiver(linkLayer_t link_layer);
void ll_close_transmitter(linkLayer_t link_layer);

int ll_write(linkLayer_t link_layer, char *data_packet, int size);

int ll_read(linkLayer_t link_layer, char *data_packet);

#endif /* __LINKLAYER */


