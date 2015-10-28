transmitter:
	gcc -Wall -o writenoncanonical writenoncanonical.c link_layer.c state.c alarm.c bstuffing.c app_layer.c

receiver:
	gcc -Wall -o noncanonical noncanonical.c link_layer.c state.c alarm.c bstuffing.c app_layer.c

clean:
	rm writenoncanonical noncanonical