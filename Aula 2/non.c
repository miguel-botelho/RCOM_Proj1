#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <signal.h>

#include "utils.h"
#include "state.h"
#include "alarm.h"

int main(int argc, char** argv){
	int fd;
  	struct termios oldtio,newtio;
  	LinkLayer *link_layer = malloc(sizeof(LinkLayer));

	struct sigaction sa;
	sa.sa_flags = 0;
	sa.sa_handler = atende;
	if (sigaction(SIGALRM, &sa, NULL) == -1) {
		perror("Error: cannot handle SIGALRM");
		return NULL;
	}

	if ( (argc < 2) ||
			((strcmp("/dev/ttyS0", argv[1])!=0) &&
					(strcmp("/dev/ttyS4", argv[1])!=0) )) {
		printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
		exit(1);
	}


	ll_init(link_layer, argv[1], BAUDRATE, 0, 1, 5, 1000, RECEIVER); 

	//app_layer(link_layer, argv);

	ll_open(link_layer);

	
	int size = ll_read(link_layer);

	printf("%s\n", link_layer->dataPacket);

	ll_close(link_layer);

	ll_end(link_layer);

	return 0;

}