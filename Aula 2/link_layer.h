#ifndef ___LINKLAYER
#define ___LINKLAYER

struct linkLayer_t {
	char port[20]; 						/*Dispositivo /dev/ttySx, x = 0, 1*/
	int baudRate; 						/*Velocidade de transmissão*/
	unsigned int sequenceNumber;		/*Número de sequência da trama: 0, 1*/
	unsigned int timeout; 				/*Valor do temporizador: 1 s*/
	unsigned int numTransmissions; 		/*Número de tentativas em caso de falha*/

	char frame[MAX_SIZE]; 				/*Trama*/
};

void ll_open(int flag, int fd);
void ll_close(int flag, int fd);

void ll_open_receiver(int fd);
void ll_open_transmitter(int fd);

void ll_close_receiver(int fd);
void ll_close_transmitter(int fd);

int ll_write(settings set, char *data_packet, int size);

int ll_read(settings set, char *data_packet);

#endif /* __LINKLAYER */


