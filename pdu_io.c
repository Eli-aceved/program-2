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


/*  Function Prototypes */
int sendPDU(int clientSocket, uint8_t *dataBuffer, int lengthOfData);
int recvPDU(int clientNumber, uint8_t *dataBuffer, int bufferSize);


/*  Functions */
int sendPDU(int clientSocket, uint8_t *dataBuffer, int lengthOfData) {
    short dataBytesSent = 0;
    int pduLength = htons(lengthOfData);    // Convert to network byte order
    // Create application level PDU
    // Set up header (2 bytes) and payload
    

    // Send PDU

    // Check for errors
    if (dataBytesSent < 0) {
        perror("Sending PDU failed");
        exit(-1);
    }

    // Return the number of bytes sent
    return dataBytesSent;
}

