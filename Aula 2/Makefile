transmitter:
	gcc -Wall -o writenoncanonical writenoncanonical.c link_layer.c state.c alarm.c bstuffing.c

receiver:
	gcc -Wall -o noncanonical noncanonical.c link_layer.c state.c alarm.c bstuffing.c

clean:
	rm writenoncanonical noncanonical