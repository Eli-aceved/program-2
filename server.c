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
void flagCheck(uint8_t *dataBuffer, int clientSocket, int messageLen);


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


/* Processes data from an existing connection and receives data from the client */
void processClient(int clientSocket) // used to be recvFromClient
{
	uint8_t dataBuffer[MAXBUF] = {0};	// Buffer for the data from the client
	int messageLen = 0;			// Length of the message received
	
	// Now get the data from the client_socket
	if ((messageLen = recvPDU(clientSocket, dataBuffer, MAXBUF)) < 0)
	{
		perror("recv call");
		exit(-1);
	}
	printf("Buffer received at server: %s\n", dataBuffer);
	// Check if there is data to be received
	if (messageLen > 0)
	{
		//printf("Message received on socket: %d, length: %d Data: %s\n", clientSocket, messageLen, dataBuffer);
		flagCheck(dataBuffer);
	}
	// Check if the connection is closed
	else if (messageLen == 0)
	{
		printf("Connection closed by other side: %d\n", clientSocket);
		removeFromPollSet(clientSocket);
		close(clientSocket);
	}
}

void flagCheck(uint8_t *dataBuffer, int clientSocket, int messageLen) {
	if (dataBuffer[0] == 5) { // Send message flag
		printf("Send message flag received by server.\n");
		processMCMsgs(dataBuffer, clientSocket, messageLen);
	}
	else if (dataBuffer[0] == 6) { // Multicast flag
		printf("Multicast flag received by server.\n");
	}
	else if (dataBuffer[0] == 4) { // Broadcast flag
		printf("Broadcast flag received by server.\n");
	}
	else if (dataBuffer[0] == 10) { // List of handles flag
		printf("List of handles flag received by server.\n");
	}
	else {
		printf("Invalid flag received by server.\n");
	}
}

 /* Processes messages received from server and displays them on client terminal */
/*void processMCMsgs(uint8_t *dataBuffer, int clientSocket, int messageLen) {
	int packetIndex = 1; // Skip the flag, start at the sender handle/number of handles
	// Parse buffer to check for num handles
	// Check how many handles are in dataBuffer[packetIndex]
	uint8_t num_handles = dataBuffer[packetIndex]; // 1 byte
	packetIndex++; // Increment to the next byte
	if (num_handles > 1) { // Multicast
		// Parse buffer for destination handles
		for (int i = 0; i < num_handles; i++) {
			uint8_t desthandle_len = dataBuffer[packetIndex]; // 1 byte
			packetIndex++; // Increment to the next byte
			

		}
		// add to handle table 
		// access handle table to figure what socket to send to

		// Parse buffer for message

		// figure out how to send message to each socket
	}
	else if (num_handles == 1) { // Unicast
		// Parse buffer for destination handles
		// add to handle table 
		// access handle table to figure what socket to send to

		// Parse buffer for message

		// figure out how to send message to each socket
	}
	


	// Parse buffer for destination handles
	// add to handle table 
	// access handle table to figure what socket to send to

	// Parse buffer for message

	// figure out how to send message to each socket


}*/
