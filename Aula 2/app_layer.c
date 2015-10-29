#include "app_layer.h"


void app_layer(LinkLayer *link_layer, char* file_name) {
	if (link_layer->status == TRANSMITTER)
		app_layer_transmitter(link_layer, file_name);
	else if (link_layer->status == RECEIVER)
		app_layer_receiver(link_layer);
}

void app_layer_receiver(LinkLayer *link_layer) {
	fprintf(stderr, "Abrir Ligação\n");
	ll_open(link_layer);
	fprintf(stderr, "Ligação Estabelecida\n");
	FileInfo file;	
	
	

	int bytesRead = al_readFile(link_layer, &file);
	
	
	//write(fd, file.file, bytesRead);

	setTries(0);
	if(ll_close(link_layer) < 0)
		fprintf(stderr, "Ligação terminada com sucesso\n");
	else
		fprintf(stderr, "Não foi possivel terminar a ligação correctamente\n");
}

int al_readFile(LinkLayer * link_layer, FileInfo * file){
	fprintf(stderr, "À espera de packet inicio\n");
	
	while(!al_readInitControlPacket(link_layer, file)){
		fprintf(stderr, "Continua à espera de packet inicio\n");
	}

	int received = FALSE;
	int bytesRead = 0;
	fprintf(stderr, "A recever dados\n");
	do{
		fprintf(stderr, "Started reading packet %d\n",file->sequenceNumber);		
		int packetSize = ll_read(link_layer);

		if(al_checkEndCtrlPacket(link_layer,file, packetSize) > 0){
			received = TRUE;
			fprintf(stderr, "Recevido pacote controlo 2\n"); 
			break;
		}

		int bytes = readInformationPacket(link_layer,file,packetSize,bytesRead);

		if(bytes < 0)
			fprintf(stderr, "Failed reading packet %d\n", file->sequenceNumber);
		else{
			fprintf(stderr, "Success reading packet %d\n", file->sequenceNumber);
			bytesRead += bytes;
		}
		(file->sequenceNumber)++;

	}while(!received);

	fprintf(stderr, "Expected %d bytes. Received %d bytes!!\n", file->size,bytesRead);

	return bytesRead;
}

int readInformationPacket(LinkLayer * link_layer, FileInfo * file, int packetSize, int bytesRead){

	char * dataPacket = link_layer->dataPacket; 
	if(dataPacket[0] != C_DATA)
		return -1;

	int seqNum = dataPacket[1];
	int size = ((unsigned int)dataPacket[2]<<8) +((unsigned int) dataPacket[3]); // FOI AQUI

	if(size != packetSize -4)
		return -1;
	
	if(seqNum != file->sequenceNumber)
		return -1;

	write(file->fd, &dataPacket[4], size);

	return size;
}

int al_readInitControlPacket(LinkLayer * link_layer, FileInfo * file){
	int dataPacketSize = ll_read(link_layer);
	if(dataPacketSize < 7)
		return -1;
	char * dataPacket = link_layer->dataPacket;
	if(dataPacket[0] != C_START)
		return -1;

	int fieldLength=0;
	int fileSize = readFileSize(&dataPacket[1], &fieldLength);
	if(fileSize < 0)
		return -1;

	char * fileName = readFileName(&dataPacket[1+2+fieldLength], &fieldLength);
	if(fileName == NULL)
		return -1;

	file->size = fileSize;
	file->name = fileName;

	int fd = open(fileName, O_WRONLY | O_TRUNC | O_CREAT, 0660);
	file->fd = fd;
	file->sequenceNumber = 0;
	return 1;
}

int al_checkEndCtrlPacket(LinkLayer * link_layer, FileInfo * file, int packetSize){
	char * dataPacket = link_layer->dataPacket;
	fprintf(stderr, "Ctrl check packet %d\n packetSize %d, dataPacket %d\n", file->sequenceNumber, packetSize, dataPacket[0]);
	if(packetSize < 7)
		return -1;
	if(dataPacket[0] != C_END)
		return -1;

	int fieldLength=0;
	int fileSize = readFileSize(&dataPacket[1], &fieldLength);
	fprintf(stderr, "Packet %d fileSize %d\n", file->sequenceNumber,fileSize);
	if(fileSize < 0)
		return -1;

	char * fileName = readFileName(&dataPacket[1+2+fieldLength], &fieldLength);
	if(fileName == NULL)
		return -1;

	if(file->size != fileSize)
		return -1;

	if(strcmp(file->name,fileName) != 0)
		return -1;
	return 1;
}

int readFileSize(char * dataPacket, int * fieldLength){
	if(dataPacket[0] != F_SIZE)
		return -1;
	*fieldLength = (unsigned int) dataPacket[1];
	int fileSize=*((uint32_t *) &dataPacket[2]);
	//int i = 0;

	
	

	return fileSize;
}

char * readFileName(char * dataPacket, int * fieldLength){
	if(dataPacket[0] != F_NAME)
		return NULL;

	*fieldLength = (unsigned int) dataPacket[1];
	char * fileName = malloc(*fieldLength);
	memcpy(fileName, &dataPacket[2], *fieldLength);
	fprintf(stderr, "size:%d\n name: %s\n", *fieldLength, fileName); 
	return fileName;
}

void app_layer_transmitter(LinkLayer *link_layer, char * file_name) {

    int file;
    char * file_buffer;
    struct stat * file_stat = malloc(sizeof(struct stat));
    fprintf(stderr, "Abrir ficheiro %s\n",file_name);
    file = open(file_name, O_RDONLY);

    fstat(file, file_stat);

    file_buffer = malloc(file_stat->st_size);

    read(file, file_buffer, file_stat->st_size);

    printf("tamanho!: %lld\n", (long long)file_stat->st_size);
	fprintf(stderr, "Abrir ligação\n");
	ll_open(link_layer);
	fprintf(stderr, "Ligação estabelecida\n");
	char controlPacket[3 + 4 + 3 + strlen(file_name) + 1];
	
	fprintf(stderr, "A enviar packet controlo 1\n");
	controlPacket[0] = C_START;
	controlPacket_size(file_stat, controlPacket);
	controlPacket_name(file_stat, &controlPacket[3 + controlPacket[2]], file_name);
	memcpy(link_layer->dataPacket, controlPacket, sizeof(controlPacket));
	ll_write(link_layer,sizeof(controlPacket));
	
	fprintf(stderr, "A começar envio de dados\n");
	int sentBytes = al_sendFile(link_layer, file_buffer, file_stat->st_size);
	printf("Sent %d bytes from %d\n", sentBytes,(int) file_stat->st_size);
	
	fprintf(stderr, "A enviar pacote controlo 2\n");
	controlPacket[0] = C_END;
	memcpy(link_layer->dataPacket, controlPacket, sizeof(controlPacket));
	ll_write(link_layer,sizeof(controlPacket));


	setTries(1);
	fprintf(stderr, "A terminar ligação\n");
	if(ll_close(link_layer) < 0)
		fprintf(stderr, "Ligação terminada com sucesso\n");
	else
		fprintf(stderr, "Não foi possivel terminar a ligação correctamente\n");

}

int al_sendFile(LinkLayer * link_layer, char * file_buffer, int size){
	int defautlPacketSize = getPacketSize(link_layer->maxFrameSize);

	int sentBytes = 0;
	int i = 0;
	while(sentBytes < size){
		int packetSize;
		if((size - sentBytes) >= defautlPacketSize )
			packetSize = defautlPacketSize;
		else
			packetSize = size - sentBytes;
		fprintf(stderr, "Sending dataPacket i = %d with size %d\n", i, packetSize);
		if(al_sendPacket(link_layer, &file_buffer[sentBytes-1], packetSize,i) < 0){ //ve isto
			fprintf(stderr, "Error sending dataPacket i=%d\n",i );	
			exit(-1);
		}
		fprintf(stderr, "dataPacket sent i = %d, with size %d\n", i, packetSize);
		i++;
		sentBytes += packetSize;
	}
	
	return sentBytes;
}

int getPacketSize(int maxFrameSize){
	return (maxFrameSize - NUMBER_FLAGS)/2 - FRAME_HEADER_SIZE - PACKET_HEADER_SIZE;
}

int al_sendPacket(LinkLayer * link_layer, char * packet, int size,int i){
	link_layer->dataPacket[0] = C_DATA;
	link_layer->dataPacket[1] = i;
	link_layer->dataPacket[2] = (size >> 8) & 0xFF; //aqui
	link_layer->dataPacket[3] = size & 0xFF ; // aqui

	memcpy(&(link_layer->dataPacket[4]), packet, size);

	fprintf(stderr, "%s\n", packet);

	return ll_write(link_layer, size + PACKET_HEADER_SIZE);
}

void controlPacket_size(struct stat * file_stat, char * controlPacket){
	controlPacket[1] = F_SIZE; //tamanho ficheiro
	controlPacket[2] = 4; // 4 bytes é o máximo
	int i = 0;
	for(;i < controlPacket[2]; i++){
		controlPacket[i+3] = 0xFF & (file_stat->st_size >> (8 * i));
	}
}

void controlPacket_name(struct stat * file_stat, char * controlPacket, char * name){
	controlPacket[0] = F_NAME; //nome do ficheiro
	controlPacket[1] = strlen(name) + 1; 	//+ 1 porque /0

	memcpy(&controlPacket[2], name, strlen(name) +1);
}
