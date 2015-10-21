#ifndef ___STATE
#define ___STATE

int send_SET(int fd, char *SET);
int send_UA(int fd, char *UA);
int send_DISC(int fd, char *DISC);

int receive_SET(int fd, char *SET);
int receive_UA(int fd, char *UA);
int receive_DISC(int fd, char * DISC);

int check_UA(char *sent);
int check_DISC(char *DISC_rec);

int getStopUA();
void setStopUA(int st);

int getStopSET();
void setStopSET(int st);

int getStopDISC();
void setStopDISC(int st);

#endif /* ___STATE */