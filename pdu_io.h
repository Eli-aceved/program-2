/*******************************************************************************
 * 
 * Author: Elizabeth Acevedo
 * 
 * File: pdu_io.h
 * 
 * Description: This program 
 * 
 * Utilization: 
 * 
 * Date: 01/28/2025
 * 
 ******************************************************************************/

#ifndef PDU_IO_H
#define PDU_IO_H

/*  Includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* Prototypes */
int sendPDU(int clientSocket, uint8_t *dataBuffer, int lengthOfData);
int recvPDU(int socketNumber, uint8_t *dataBuffer, int bufferSize);


#endif
