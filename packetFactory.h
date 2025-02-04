/****************************************************************
 * Author: Elizabeth Acevedo
 * 
 * Created: 02/03/2025
 * 
 * File: commands.h
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

/* Function Prototypes */
size_t readCommand(uint8_t *refBuff, uint8_t *dataBuffer, uint8_t *sender_handle);

#endif
