/****************************************************************
 * Author: Elizabeth Acevedo
 * 
 * Created: 02/03/2025
 * 
 * File: packetFactory.h
 * 
 * Description: This program 
 * 
 * Utilization: 
 * 
 ****************************************************************/

#ifndef PACKETFACTORY_H
#define PACKETFACTORY_H

/* Includes */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>

#include "pdu_io.h"
#include "handle_table.h"

/* Definitions */
#define MAX_H 101
#define MAXBUF 1402


/* Function Prototypes */
size_t readCommand(uint8_t *refBuff, uint8_t *dataBuffer, uint8_t *sender_handle);
char **get_all_handles(int *num_handles);
size_t sendMsgCmd(uint8_t *packetBuffer, uint8_t **saveptr, uint8_t *sender_handle, int isMulticast);
size_t broadcastCmd(uint8_t *packetBuffer, uint8_t **saveptr, uint8_t *sender_handle);
void processFlag1(uint8_t *dataBuffer, int clientSocket);
void listCmd(int clientSocket);
void packetFlag10(uint8_t *packetBuffer);
void sendFlag11(int clientSocket);
void sendFlag12(int clientSocket);
void sendFlag13(int clientSocket);

#endif
