/******************************************************************************
* myServer.c
* 
* Writen by Prof. Smith, updated Jan 2023
* Modified by Elizabeth Acevedo on 01/29/2025
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
void serverControl(int mainServerSocket, int clientSocket);
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


	addToPollSet(mainServerSocket);

	while (1) {
		serverControl(mainServerSocket, clientSocket);

	}
	
	/* close the sockets */
	close(mainServerSocket);

	
	return 0;
}

void recvFromClient(int clientSocket)
{
	uint8_t dataBuffer[MAXBUF];
	int messageLen = 0;
	
	// Now get the data from the client_socket
	if ((messageLen = recvPDU(clientSocket, dataBuffer, MAXBUF)) < 0)
	{
		perror("recv call");
		exit(-1);
	}

	if (messageLen > 0)
	{
		printf("Message received on socket: %d, length: %d Data: %s\n", clientSocket, messageLen, dataBuffer);
	}
	else if (messageLen == 0)
	{
		printf("Connection closed by other side: %d\n", clientSocket);
		removeFromPollSet(clientSocket);
		close(clientSocket);
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

void serverControl(int mainServerSocket, int clientSocket) {
	// Wait for a socket to be ready
	int resp_sock = pollCall(-1); // Blocks until a socket is ready

	if (resp_sock < 0) {
		perror("pollCall");
		exit(-1);
	}
	// Accepts new connections and adds them to the poll set
	else if (resp_sock == mainServerSocket) {
			// New connection
			addNewSocket(mainServerSocket);
	}
	// Processes existing connections
	else {
		// Existing connection
		processClient(resp_sock);
	}
}

void addNewSocket(int mainServerSocket) {
	// Accept a new connection
	int newSocket = tcpAccept(mainServerSocket, DEBUG_FLAG);
	// Add the new socket to the poll set
	addToPollSet(newSocket);
}

void processClient(int clientSocket) {
	// Receive data from the client
	recvFromClient(clientSocket);
}
