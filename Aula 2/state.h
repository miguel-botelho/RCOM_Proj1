#ifndef ___STATE
#define ___STATE

int send_SET(int fd, char *SET);
int send_UA(int fd, char *UA);
int send_DISC(int fd, char *DISC);

void receive_SET(int fd, char *SET);
void receive_UA(int fd, char *UA);
void receive_DISC(int fd, char * DISC);

int check_UA(char *sent);
int check_DISC(char *DISC_rec);

int getStopUA();
void setStopUA(int st);

int getStopSET();
void setStopSET(int st);

int getStopDISC();
void setStopDISC(int st);

#endif /* ___STATE */