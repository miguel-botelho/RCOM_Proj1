#ifndef ___LINKLAYER
#define ___LINKLAYER

struct settings_t {
	int maxTries;
	int fd;
	int timeout;
	int maxPacketSize;
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


