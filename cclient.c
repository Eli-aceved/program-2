/******************************************************************************
* myClient.c
*
* Writen by Prof. Smith, updated Jan 2023
* Modified by Elizabeth Acevedo on 01/29/2025
*
* This program sets up a TCP client that connects to a server, sends data from
* the standard input to the server, and processes messages received from the server.
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
#include <ctype.h>
#include <stddef.h>
#include "networks.h"
#include "safeUtil.h"
#include "pdu_io.h"
#include "pollLib.h"
#include "packetFactory.h"
#include "handle_table.h"

/* Definitions */
#define MAXBUF 1402
#define DEBUG_FLAG 1
#define MAX_HANDLE_SIZE 100

/* Function Prototypes */
int readFromStdin(uint8_t *buffer);
void checkArgs(int argc, char *argv[], uint8_t *sender_handle);
void clientControl(int socketNum, uint8_t *sender_handle);
void processStdin(int socketNum, uint8_t *sender_handle);	// renamed from sendToServer
void processMsgFromServer(int socketNum);
void flagCheck(uint8_t *dataBuffer, int clientSocket, int messageLen);
void sendFlag1(int socketNum, uint8_t *sender_handle);
void printMCMsgs(uint8_t *dataBuffer);



int main(int argc, char * argv[])
{
	int socketNum = 0;         //socket descriptor
	// Store the sender's handle
	uint8_t sender_handle[MAX_HANDLE_SIZE] = {0};
	
	checkArgs(argc, argv, sender_handle);

	socketNum = tcpClientSetup(argv[2], argv[3], DEBUG_FLAG);


	setupPollSet();
	addToPollSet(socketNum);
	sendFlag1(socketNum, (uint8_t*)argv[1]);

	addToPollSet(STDIN_FILENO);

	while (1) {
		/* set up the TCP Client socket  */
		clientControl(socketNum, sender_handle);
	}
	
	close(socketNum); // Close the socket
	return 0;
}

int readFromStdin(uint8_t *buffer)
{
	char aChar = 0;
	int inputLen = 0;        
	
	// Important you don't input more characters than you have space 
	buffer[0] = '\0';
	while (inputLen < (MAXBUF - 1) && aChar != '\n')
	{
		aChar = getchar();
		if (aChar != '\n')
		{
			buffer[inputLen] = aChar;
			inputLen++;
		}
	}
	
	// Null terminate the string
	buffer[inputLen] = '\0';
	inputLen++;
	
	return inputLen;
}

/* checkArgs: verifies the correct number of command line arguments */
void checkArgs(int argc, char * argv[], uint8_t *sender_handle)
{
	/* check command line arguments */
	if (argc != 4) {
		printf("usage: %s <sender's handle> <host-name> <port-number> \n", argv[0]);
		exit(1);
	}

 	// Store the sender's handle in the provided buffer
	strncpy((char *)sender_handle, argv[1], MAX_HANDLE_SIZE - 1);

	 // Check if the last character is a null terminator
    if (sender_handle[MAX_HANDLE_SIZE - 1] != '\0') {
        // Manually adds the null terminator if not already present
        sender_handle[MAX_HANDLE_SIZE - 1] = '\0';
    }
}


/* Handles client control operations for the given socket number ()*/
void clientControl(int socketNum, uint8_t *sender_handle) {
	printf("$: ");	// Prompts user to send multiple msgs after the server returns client msg
	fflush(stdout);			// Flushes the output buffer to ensure the prompt is displayed 
	// Wait for a socket to be ready
	int c_sock = pollCall(-1); // Blocks until a socket is ready
	// Error handling
	if (c_sock < 0) {
		perror("pollCall");
		exit(-1);
	}
	// Accepts new connections and adds them to the poll set
	else if (c_sock == socketNum) {
		// New connection
		processMsgFromServer(socketNum);
	}
	// Processes existing connections
	else if (c_sock == STDIN_FILENO) {
		// Existing connection
		processStdin(socketNum, sender_handle);
	}

}

/* Processes the standard input */
void processStdin(int socketNum, uint8_t *sender_handle) // Used to be called sendToServer
{
	uint8_t readBuff[MAXBUF] = {0};		//data buffer
	
	// Read the data from the standard input
	readFromStdin(readBuff);
	
	uint8_t destPackBuff[MAXBUF] = {0};	// Buffer to store the data that will be sent to the server
	size_t packet_len = 0;			// Length of the packet
	
	// Creates packets for every command type
	packet_len = readCommand(readBuff, destPackBuff, sender_handle); // sendBuf holds the data that was typed in by the user

	// Send the data to the server
	sendPDU(socketNum, destPackBuff, packet_len);
	if (read < 0)
	{
		perror("send call");
		exit(-1);
	}
	
}

/* Processes messages received from server and displays them on client terminal */
void processMsgFromServer(int socketNum) {
	uint8_t dataBuffer[MAXBUF] = {0};	// Buffer to store the data received from the server
	int bytesRead;				// Number of bytes read

	// Receive the data from the server
	bytesRead = recvPDU(socketNum, dataBuffer, MAXBUF);

	flagCheck(dataBuffer, socketNum, bytesRead);

	// Ensure connection is still open
	if (bytesRead == 0) {
		printf("\nServer has terminated\n");
		close(socketNum);
		exit(-1);
	}
	// Display the message received from the server if there are bytes to read
	else if (bytesRead > 0) {
		printf("\nMessage received from server of length: %u\n", bytesRead);//WILL BE REMOVED
	}
}

void flagCheck(uint8_t *dataBuffer, int clientSocket, int messageLen) {
	if (dataBuffer[0] == 5) { // Add handle flag
		printf("\nSend message flag received by client.\n");
		printMCMsgs(dataBuffer);
	}
	else if (dataBuffer[0] == 6) {
		printf("\nMulticast flag received by client.\n");
		printMCMsgs(dataBuffer);
	}
	else {
		printf("\nInvalid flag received by client.\n");
	}
}

/* Initial flag when client attempts to connect to the server (sends flag to server)*/
void sendFlag1(int socketNum, uint8_t *sender_handle) {
	uint8_t packetBuffer[MAX_HANDLE_SIZE] = {0};	// Buffer to store the data that will be sent to the server
	uint8_t flag = 1;
	uint8_t handle_len = strlen((char *)sender_handle);

	packetBuffer[0] = flag;
	packetBuffer[1] = handle_len;

	memcpy(&packetBuffer[2], sender_handle, handle_len);

	sendPDU(socketNum, packetBuffer, handle_len + 2);

}

/* Prints the message received from the server onto the client terminal */
void printMCMsgs(uint8_t *dataBuffer) {
	uint8_t senderHandle[MAX_HANDLE_SIZE] = {0};
	int packetIndex = 1; // Skip the flag, start at the sender handle/number of handles
	uint8_t lengthofsenderhandle = dataBuffer[packetIndex++]; // 1 byte
	memcpy(senderHandle, &dataBuffer[packetIndex], lengthofsenderhandle);
	packetIndex += lengthofsenderhandle;

	// Check how many handles are in packet
	uint8_t num_handles = dataBuffer[packetIndex++]; // 1 byte
	
	uint8_t lengthofdestinationhandle = 0;
	// Parse buffer to skip dest. handles
	for (int i = 0; i < num_handles && i < MAX_HANDLE_SIZE; i++) {
		lengthofdestinationhandle = dataBuffer[packetIndex++]; // 1 byte
		packetIndex += lengthofdestinationhandle;
	};
	
	printf("%s: %s\n", senderHandle, &dataBuffer[packetIndex]);

}