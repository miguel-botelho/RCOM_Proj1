#ifndef ___STATE
#define ___STATE

int send_Frame(int fd, char *SET);
int receive_UA(int fd, char *UA);
int check_UA(char *sent);

#endif /* ___STATE */