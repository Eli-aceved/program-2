/******************************************************************************
* myServer.c
* 
* Writen by Prof. Smith, updated Jan 2023
* Modified by Elizabeth Acevedo on 01/29/2025
*
*****************************************************************************/

/* Includes */
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

/* Definitions */
#define MAXBUF 1024
#define DEBUG_FLAG 1

/* Function Prototypes */
int checkArgs(int argc, char *argv[]);
void serverControl(int mainServerSocket, int clientSocket);
void addNewSocket(int mainServerSocket);
void processClient(int clientSocket); // renamed from recvFromClient

int main(int argc, char *argv[])
{
	int mainServerSocket = 0;   //socket descriptor for the server socket
	int clientSocket = 0;   	//socket descriptor for the client socket
	int portNumber = 0;

	// Assignes the port number to variable
	portNumber = checkArgs(argc, argv);
	
	//create the server socket
	mainServerSocket = tcpServerSetup(portNumber);

	addToPollSet(mainServerSocket);

	while (1) {
		serverControl(mainServerSocket, clientSocket);	// Handles processing connections
	}
	
	/* close the sockets */
	close(mainServerSocket);

	return 0;
}

/* Verifies the correct number of command line arguments */
int checkArgs(int argc, char *argv[])
{
	// Checks args and returns port number
	int portNumber = 0;

	// If # of arguments is greater than 2, print usage message
	if (argc > 2)
	{
		fprintf(stderr, "Usage %s [optional port number]\n", argv[0]);
		exit(-1);
	}
	// If # of arguments is 2, string argument converted to int and set as port number
	else
	if (argc == 2)
	{
		portNumber = atoi(argv[1]); 
	}
	
	return portNumber;
}

/* Handles processing new and exisiting connections */
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

/* Adds a new socket to the poll set */
void addNewSocket(int mainServerSocket) {
	// Accept a new connection
	int newSocket = tcpAccept(mainServerSocket, DEBUG_FLAG);
	// Add the new socket to the poll set
	addToPollSet(newSocket);
}

/* Processes data from an existing connection */
void processClient(int clientSocket) {
	// Receive data from the client
	recvFromClient(clientSocket);
}

/* Checks and receives data from the client */
void processClient(int clientSocket) // used to be recvFromClient
{
	uint8_t dataBuffer[MAXBUF];	// Buffer for the data from the client
	int messageLen = 0;			// Length of the message received
	
	// Now get the data from the client_socket
	if ((messageLen = recvPDU(clientSocket, dataBuffer, MAXBUF)) < 0)
	{
		perror("recv call");
		exit(-1);
	}

	// Check if there is data to be received
	if (messageLen > 0)
	{
		printf("Message received on socket: %d, length: %d Data: %s\n", clientSocket, messageLen, dataBuffer);

		// Perform some action with the data
		//
		//
		//
	}
	// Check if the connection is closed
	else if (messageLen == 0)
	{
		printf("Connection closed by other side: %d\n", clientSocket);
		removeFromPollSet(clientSocket);
		close(clientSocket);
	}
}