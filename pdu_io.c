/*******************************************************************************
 * 
 * Author: Elizabeth Acevedo
 * 
 * File: pdu_io.c
 * 
 * Description: T
 * 
 * Utilization: 
 * 
 * Date: 01/28/2025
 * 
 ******************************************************************************/

/*  Includes */
#include "pdu_io.h"

#include <sys/socket.h>
#include <arpa/inet.h>  // Internet operations

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/*  Definitions */
# define TOTAL_LEN (2 + lengthOfData)    // Total length of PDU = 2 bytes for header + lengthOfData bytes for payload

/*  Function Prototypes */
int sendPDU(int clientSocket, uint8_t *dataBuffer, int lengthOfData);
int recvPDU(int clientNumber, uint8_t *dataBuffer, int bufferSize);


/*  Functions */
int sendPDU(int clientSocket, uint8_t *dataBuffer, int lengthOfData) {
    short dataBytesSent = 0;
    uint8_t pduBuffer[TOTAL_LEN];    // pduBuffer = 2 bytes for header + lengthOfData bytes for payload
    int pduLength = htons(lengthOfData);    // Convert to network byte order
    // Creating application level PDU
    // Setting up header (2 bytes) and payload (lengthOfData bytes)
    pduBuffer[0] = (pduLength & 0xFF00) >> 8;
    pduBuffer[1] = pduLength & 0xFF;
    memcpy(&pduBuffer[2], dataBuffer, lengthOfData);

    // Send PDU
    dataBytesSent = send(clientSocket, pduBuffer, TOTAL_LEN, 0);

    // Check for errors
    if (dataBytesSent < 0) {
        perror("Sending PDU failed");
        exit(-1);
    }

    // Return the number of bytes sent
    return dataBytesSent;
}


int recvPDU(int clientNumber, uint8_t *dataBuffer, int bufferSize) {
    
}