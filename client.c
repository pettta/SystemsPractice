#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <netdb.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include <pthread.h> 

void * server_output(void *arg); 

int
main(int argc, char *argv[]) {
	int sock, connectStatus, argSize;
	long unsigned int writeSize, readSize; 
	size_t size; 
	struct sockaddr_in server;
	struct hostent *host; 
	pthread_t tid; 
	char* buff;

	size = 1024; 
	buff = malloc(1024*sizeof(char));
	argSize = strlen(argv[1]); 

	sock = socket(AF_INET, SOCK_STREAM, 0); 
	server.sin_family = AF_INET; 
	server.sin_port= htons(2022); 

	host = gethostbyname("tux3.cs.drexel.edu");
	if(host==NULL){
		fprintf(stderr, "Did not connect to host\n" ); 
		return 1; 
	}

	server.sin_addr = * (struct in_addr*) host->h_addr_list[0];
	connectStatus = connect(sock, (struct sockaddr*) &server, sizeof(server)); // TODO possibly remove the retrurn val 

	if(connectStatus==-1){
		fprintf(stderr, "Did not connect to the socket\n");
		return 1; 
	}

	send(sock, argv[1], argSize, 0);

	pthread_create(&tid, NULL, server_output, &sock);
	while(1){
		readSize = getline(&buff, &size, stdin);
		writeSize = send(sock, buff, readSize, 0);
		if(readSize!=writeSize){
			fprintf(stdout, "Error: message not delivered\n"); 
		}
	}

	close(sock); 
	exit(0);
}

void *
server_output(void *arg) {
	int sockId, inSize; 
	char buff[1024]; 
	
	sockId=*((int *)arg);
	while(1){
		memset(buff, '\0', 1024);
		inSize = recv(sockId, buff, 1024, 0); 
		if (inSize > 0) {
			buff[1023] = '\0';
			fwrite(buff, 1024, 1, stdout);
		}
	}
		
}
