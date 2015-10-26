#ifndef ___LINKLAYER
#define ___LINKLAYER

void ll_open(int flag, int fd);
void ll_close(int flag, int fd);

void ll_open_receiver(int fd);
void ll_open_transmitter(int fd);

void ll_close_receiver(int fd);
void ll_close_transmitter(int fd);

#endif /* __LINKLAYER */


