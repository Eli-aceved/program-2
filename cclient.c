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

void sendToServer(int socketNum);
int readFromStdin(uint8_t * buffer);
void checkArgs(int argc, char * argv[]);
void clientControl(int socketNum);
void processStdin(int socketNum);
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
	
	close(socketNum);
	
	return 0;
}

void sendToServer(int socketNum)
{
	uint8_t sendBuf[MAXBUF];   //data buffer
	int sendLen = 0;        //amount of data to send
	int sent = 0;            //actual amount of data sent/* get the data and send it   */
	
	sendLen = readFromStdin(sendBuf);
	printf("read: %s string len: %d (including null)\n", sendBuf, sendLen);
	
	sent =  sendPDU(socketNum, sendBuf, sendLen);
	if (sent < 0)
	{
		perror("send call");
		exit(-1);
	}
	

	printf("Amount of data sent is: %d\n", sent);
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

void checkArgs(int argc, char * argv[])
{
	/* check command line arguments  */
	if (argc != 3)
	{
		printf("usage: %s host-name port-number \n", argv[0]);
		exit(1);
	}
}

void clientControl(int socketNum) {
	printf("Enter data: ");
	fflush(stdout);
	// Wait for a socket to be ready
	int c_sock = pollCall(-1); // Blocks until a socket is ready
	
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

void processStdin(int socketNum) {
	sendToServer(socketNum);
}

void processMsgFromServer(int socketNum) {
	uint8_t dataBuffer[MAXBUF];
	int bytesRead;
	uint16_t pduLength = 0;

	bytesRead = safeRecv(socketNum, dataBuffer, MAXBUF, 0);

	memcpy(&pduLength, dataBuffer, 2);
	pduLength = ntohs(pduLength);

	if (bytesRead == 0) {
		printf("\nServer has terminated\n");
		close(socketNum);
		exit(-1);
	}
	else if (bytesRead > 0) {
		printf("\nMessage received from server: %s + Length from server: %u\n", &dataBuffer[2], pduLength);
	}

}
