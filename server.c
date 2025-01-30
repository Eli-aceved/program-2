/******************************************************************************
* myServer.c
* 
* Writen by Prof. Smith, updated Jan 2023
* Use at your own risk.  
*
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdint.h>

#include "networks.h"
#include "safeUtil.h"
#include "pdu_io.h"
#include "pollLib.h"

#define MAXBUF 1024
#define DEBUG_FLAG 1

void recvFromClient(int clientSocket);
int checkArgs(int argc, char *argv[]);
void serverControl(int mainServerSocket, int resp_sock, int clientSocket);
void addNewSocket(int mainServerSocket);
void processClient(int clientSocket);

int main(int argc, char *argv[])
{
	int mainServerSocket = 0;   //socket descriptor for the server socket
	int clientSocket = 0;   	//socket descriptor for the client socket
	int portNumber = 0;
	
	portNumber = checkArgs(argc, argv);
	
	//create the server socket
	mainServerSocket = tcpServerSetup(portNumber);

	// 
	addToPollSet(mainServerSocket);

	while (1) {
		int resp_sock = pollCall(-1); // Blocks until a socket is ready
		serverControl(mainServerSocket, resp_sock, clientSocket);
	}
	
	/* close the sockets */
	close(clientSocket);
	close(mainServerSocket);

	
	return 0;
}

void recvFromClient(int clientSocket)
{
	uint8_t dataBuffer[MAXBUF];
	int messageLen = 0;
	
	//now get the data from the client_socket
	if ((messageLen = recvPDU(clientSocket, dataBuffer, MAXBUF)) < 0)
	{
		perror("recv call");
		exit(-1);
	}

	if (messageLen > 0)
	{
		printf("Message received, length: %d Data: %s\n", messageLen, dataBuffer);
	}
	else
	{
		printf("Connection closed by other side\n");
	}
}

int checkArgs(int argc, char *argv[])
{
	// Checks args and returns port number
	int portNumber = 0;

	if (argc > 2)
	{
		fprintf(stderr, "Usage %s [optional port number]\n", argv[0]);
		exit(-1);
	}
	
	if (argc == 2)
	{
		portNumber = atoi(argv[1]);
	}
	
	return portNumber;
}

void serverControl(int mainServerSocket, int resp_sock, int clientSocket) {
	// Accepts new connections and adds them to the poll set
	if (resp_sock == mainServerSocket) {
			// New connection
			addNewSocket(mainServerSocket);
	}
	// Processes existing connections
	else if (resp_sock == clientSocket) {
		// Existing connection
		processClient(clientSocket);
	}
}

void addNewSocket(int mainServerSocket) {
	int newSocket = tcpAccept(mainServerSocket, DEBUG_FLAG);
	addToPollSet(newSocket);
}

void processClient(int clientSocket) {
	recvFromClient(clientSocket);
}

