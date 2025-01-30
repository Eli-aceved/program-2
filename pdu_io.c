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
#include "safeUtil.h"
#include "pollLib.h"
#include <sys/socket.h>
#include <arpa/inet.h>  // Internet operations
#include <unistd.h>     // close()

/*  Definitions */
#define TOTAL_PDU_LEN (2 + lengthOfData)    // Total length of PDU = 2 bytes for header + lengthOfData bytes for payload
#define BUF_SIZE 1024                       // PDU Buffer Size


/*  Functions */
int sendPDU(int clientSocket, uint8_t *dataBuffer, int lengthOfData) {
    uint8_t pduBuffer[BUF_SIZE] = {0};           // 2 bytes for header + lengthOfData bytes for payload
    
    // Creating application level PDU
    // Setting up header (2 bytes) and payload (lengthOfData bytes)
    pduBuffer[0] = TOTAL_PDU_LEN >> 8;
    pduBuffer[1] = TOTAL_PDU_LEN & 0x00FF;
    memcpy(&pduBuffer[2], dataBuffer, lengthOfData);

    // Send entire PDU
    if (safeSend(clientSocket, pduBuffer, TOTAL_PDU_LEN, 0) < 0) {
        perror("Error when sending PDU");
        exit(-1);
    }

    // Return the number of bytes sent
    return lengthOfData;
}


int recvPDU(int socketNumber, uint8_t *dataBuffer, int bufferSize) {
    int dataBytesReceived = 0;
    uint8_t pduBuff[BUF_SIZE] = {0};  // Buffer for the 2-byte len header
    uint16_t pduLength = 0;
    // Receive the 2-byte PDU length header
    dataBytesReceived = safeRecv(socketNumber, pduBuff, 2, MSG_WAITALL);
    if (dataBytesReceived < 0) {
        perror("Error with PDU length received");
        exit(-1);
    }
    if (dataBytesReceived == 0) {
        return 0;   // Connection closed
    }

    // Extracting the length of the PDU
    memcpy(&pduLength, pduBuff, sizeof(pduLength));

    // Convert to host byte order
    pduLength = ntohs(pduLength);

    // Ensure buffer is large enough
    if (pduLength > bufferSize) {
        perror("Received too many bytes, buffer is too small");
        exit(-1);
    }

    // Subtract header size from pduLength to get payload length
    int payloadLen = pduLength - 2;

    // Receive the payload
    dataBytesReceived = safeRecv(socketNumber, &pduBuff[2], payloadLen, MSG_WAITALL);

    memcpy(dataBuffer, &pduBuff[2], payloadLen);

    if (dataBytesReceived < 0) {
        perror("Error with PDU payload received");
        exit(-1);
    }
    if (dataBytesReceived == 0) {
        return 0;
    }

    // Send buffer to the client 
    safeSend(socketNumber, pduBuff, pduLength, 0);

    return dataBytesReceived;   // Return the length of the payload
}



