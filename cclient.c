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
#include "networks.h"
#include "safeUtil.h"
#include "pdu_io.h"
#include "pollLib.h"

/* Definitions */
#define MAXBUF 1024
#define DEBUG_FLAG 1

/* Function Prototypes */
int readFromStdin(uint8_t * buffer);
void checkArgs(int argc, char * argv[]);
void clientControl(int socketNum);
void processStdin(int socketNum);	// renamed from sendToServer
void processMsgFromServer(int socketNum);

int main(int argc, char * argv[])
{
	int socketNum = 0;         //socket descriptor
	
	checkArgs(argc, argv);

	socketNum = tcpClientSetup(argv[1], argv[2], DEBUG_FLAG);

	setupPollSet();
	addToPollSet(socketNum);
	addToPollSet(STDIN_FILENO);

	while (1) {
		/* set up the TCP Client socket  */
		clientControl(socketNum);
	}
	
	close(socketNum); // Close the socket
	
	return 0;
}

int readFromStdin(uint8_t * buffer)
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
void checkArgs(int argc, char * argv[])
{
	/* check command line arguments  */
	if (argc != 3)
	{
		printf("usage: %s host-name port-number \n", argv[0]);
		exit(1);
	}
}

/* Handles client control operations for the given socket number ()*/
void clientControl(int socketNum) {
	printf("Enter data: ");	// Prompts user to send multiple msgs after the server returns client msg
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
		processStdin(socketNum);
	}

}

/* Processes the standard input */
void processStdin(int socketNum) // Used to be called sendToServer
{
	uint8_t sendBuf[MAXBUF];		//data buffer
	int sendLen = 0;        		//amount of data to send
	int sent = 0;            		//actual amount of data sent/* get the data and send it   */
	
	// Read the data from the standard input
	sendLen = readFromStdin(sendBuf);
	printf("read: %s string len: %d (including null)\n", sendBuf, sendLen);
	
	// Send the data to the server
	sent =  sendPDU(socketNum, sendBuf, sendLen);
	if (sent < 0)
	{
		perror("send call");
		exit(-1);
	}
	
	printf("Amount of data sent is: %d\n", sent);//WILL BE REMOVED
}

/* Processes messages received from server and displays them on client terminal */
void processMsgFromServer(int socketNum) {
	uint8_t dataBuffer[MAXBUF];	// Buffer to store the data received from the server
	int bytesRead;				// Number of bytes read
	uint16_t pduLength = 0;		// Length of the PDU

	// Receive the data from the server
	bytesRead = safeRecv(socketNum, dataBuffer, MAXBUF, 0);

	// Extract the length of the PDU
	memcpy(&pduLength, dataBuffer, 2);

	// Convert to host byte order
	pduLength = ntohs(pduLength);

	// Ensure connection is still open
	if (bytesRead == 0) {
		printf("\nServer has terminated\n");
		close(socketNum);
		exit(-1);
	}
	// Display the message received from the server if there are bytes to read
	else if (bytesRead > 0) {
		printf("\nMessage received from server: %s + Length from server: %u\n", &dataBuffer[2], pduLength);//WILL BE REMOVED
	}
}
