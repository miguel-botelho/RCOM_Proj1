#ifndef ___STATE
#define ___STATE
#include "utils.h"

int send_SET(int fd, char *SET);
int send_UA(int fd, char *UA);
int send_DISC(int fd, char *DISC);
int send_RR(int fd, int r);

void receive_SET(int fd, char *SET);
void receive_UA(int fd, char *UA);
void receive_DISC(int fd, char *DISC);
int receive_RR(int fd, char *RR, int s);

int receive_FRAME(int fd, char *FRAME);

int check_UA(char *sent);
int check_DISC(char *DISC_rec);
int check_I(char * dataPacket, int s, char *frame, int frameSize, LinkLayer *link_layer);

int getStopUA();
void setStopUA(int st);

int getStopSET();
void setStopSET(int st);

int getStopDISC();
void setStopDISC(int st);

void setStopFRAME(int st);
int getStopFRAME();


#endif /* ___STATE */