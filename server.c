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
#include "handle_table.h"

/* Definitions */
#define MAXBUF 1024
#define MAX_MSG_SIZE 200
#define MAX_H 101
#define MAX_NUM_HANDLES 9
#define DEBUG_FLAG 1

/* Function Prototypes */
int checkArgs(int argc, char *argv[]);
void serverControl(int mainServerSocket, int clientSocket);
void addNewSocket(int mainServerSocket);
void processClient(int clientSocket); // renamed from recvFromClient
void flagCheck(uint8_t *dataBuffer, int clientSocket, int messageLen);
void processMCMsgs(uint8_t *dataBuffer, int clientSocket, int messageLen);

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
	create_table(INIT_TABLE_SIZE); 
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
	//printf("Buffer received at server: %s\n", dataBuffer);
	// Check if there is data to be received
	if (messageLen > 0)
	{
		//printf("Message received on socket: %d, length: %d Data: %s\n", clientSocket, messageLen, dataBuffer);
		flagCheck(dataBuffer, clientSocket, messageLen);
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
	if (dataBuffer[0] == 1) { // Add handle flag
		printf("Initial packet flag received by server.\n");
		uint8_t handle_len = dataBuffer[1];
		uint8_t handle[MAX_H] = {0};
		memcpy(handle, &dataBuffer[2], handle_len);
		addHandleSockPair((const char *)handle, clientSocket);
	}
	else if(dataBuffer[0] == 5) { // Send message flag
		printf("Send message flag received by server.\n");
		processMCMsgs(dataBuffer, clientSocket, messageLen);
	}
	else if (dataBuffer[0] == 6) { // Multicast flag
		printf("Multicast flag received by server.\n");
		processMCMsgs(dataBuffer, clientSocket, messageLen);

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
void processMCMsgs(uint8_t *dataBuffer, int clientSocket, int messageLen) {
	int packetIndex = 1; // Skip the flag, start at the sender handle/number of handles
	uint8_t lengthofsenderhandle = dataBuffer[packetIndex++]; // 1 byte
	packetIndex += lengthofsenderhandle; // Skip the sender handle

	// Parse buffer to check for num handles
	// Check how many handles are in dataBuffer[packetIndex]
	uint8_t num_handles = dataBuffer[packetIndex++]; // 1 byte
	
	uint8_t lengthofdestinationhandle = 0;
	uint8_t handleArray[MAX_NUM_HANDLES][MAX_H] = {0}; // Buffer to store destination handles
	// Parse buffer for destination handles
	for (int i = 0; i < num_handles && i < MAX_H; i++) {
		lengthofdestinationhandle = dataBuffer[packetIndex++]; // 1 byte
		memcpy(handleArray[i], &dataBuffer[packetIndex], lengthofdestinationhandle);
		packetIndex += lengthofdestinationhandle;
	};
	
	// figure out how to send message to each socket
	// Iterate through handle array to send message to each socket
	for (int handle_indx = 0; handle_indx < num_handles; handle_indx++) {
		
		uint8_t *current_handle = handleArray[handle_indx];
		uint8_t current_handle_length = strlen((char *) current_handle);
		
		// Get socket number from handle table
		int destSocket = getSockNum((char *)current_handle);
	

		if (destSocket == - 1) {
			// Create "client does not exist" packet
			uint8_t error_packet[MAX_H + 2] = {0};
			error_packet[0] = 7; 	// 1 byte Flag
			error_packet[1] = current_handle_length;	// 1 byte
			memcpy(&error_packet[1], handleArray[handle_indx], current_handle_length);
			// Send a "client does not exist" packet to destination handle
			sendPDU(clientSocket, error_packet, current_handle_length + 2);
		}
		else {
			// Send original packet to destination handle
			sendPDU(destSocket, dataBuffer, messageLen);
		}
		
	}

}
