/****************************************************************
 * Author: Elizabeth Acevedo
 * 
 * Created: 02/03/2025
 * 
 * File: commands.c
 * 
 * Description: This program 
 * 
 * Utilization: 
 * 
 * 
 ****************************************************************/
/* Includes */
#include "packetFactory.h"

/* Definitions */
#define MAX_MSG_SIZE 200

/* Function Prototypes */
size_t sendMsgCmd(uint8_t *packetBuffer, uint8_t **saveptr, uint8_t *sender_handle, int isMulticast);
size_t broadcastCmd(uint8_t **saveptr, uint8_t *sender_handle);
void listCmd(uint8_t *dataBuffer);


size_t readCommand(uint8_t *refBuff, uint8_t *dataBuffer, uint8_t *sender_handle) {
    // Want to parse the buffer and determine what command was entered using strtok()
    // Then call the appropriate function based on the command (if strtok() permits exiting current function) 
    // Ensure dataBuffer is mutable
    size_t packet_len = 0;
    // Parse the first token in the buffer
    char *saveptr;
    char *token = strtok_r((char *)refBuff, " ", &saveptr);  
    
    // Check if the token is NULL
    if (token == NULL) {
        printf("No command entered at '$: '\n");
        return 0;
    }

    // Check if the token is send message
    if (strcmp(token, "%M") == 0 || strcmp(token, "%m") == 0) {
        printf("Send message command entered\n");
        // Call the sendMsgCmd function
        packet_len = sendMsgCmd(dataBuffer, (uint8_t **)&saveptr, sender_handle, 0);
    }
        // Check if the token is send message
    else if (strcmp(token, "%C") == 0 || strcmp(token, "%c") == 0) {
        printf("Mulitcast command entered\n");
        // Call the sendMsgCmd function
        packet_len = sendMsgCmd(dataBuffer, (uint8_t **)&saveptr, sender_handle, 1);
    }
        // Check if the token is send message
    else if (strcmp(token, "%B") == 0 || strcmp(token, "%b") == 0) {
        printf("Broadcast command entered\n");
        // Call the broadcastCmd function
        packet_len = broadcastCmd((uint8_t **)&saveptr, sender_handle);
    }
        // Check if the token is send message
    else if (strcmp(token, "%L") || strcmp(token, "%l")) {
        printf("List of handles command entered\n");
        // Call the listCmd function
        //packet_len = listCmd((uint8_t **)&saveptr, sender_handle);
    }
    else {
        printf("Invalid command entered\n");
    }

    return packet_len;

}

size_t sendMsgCmd(uint8_t *packetBuffer, uint8_t **saveptr, uint8_t *sender_handle, int isMulticast) { // Issues with buffer allocation and size
    uint8_t Mflag = 5; // Flag for send message command
    uint8_t Cflag = 6; // Flag for multicast command
    int packetIndex = 0;
  

    // Print sender handle
    printf("Sender handle: %s\n", sender_handle);   // Debugging
    // Get length of sender handle
    uint8_t senderhandle_len = strlen((char *)sender_handle);
    if (senderhandle_len > 100) {
        printf("Error: Sender handle exceeds 100 bytes\n");
        return 0;
    }

    // Add flag into buffer depending on command
    if (isMulticast) {
        // add flag into buffer
        printf("Multicast command entered (flag check)\n");
        packetBuffer[packetIndex++] = Cflag;
    }
    else {
        // add flag into buffer
        printf("Unicast command entered (flag check)\n");
        packetBuffer[packetIndex++] = Mflag;
    }

    // Copy sender handle length into buffer (1 byte)
    memcpy(&packetBuffer[packetIndex++], &senderhandle_len, sizeof(senderhandle_len));
    // Copy sender handle into buffer (variable length)
    memcpy(&packetBuffer[packetIndex], sender_handle, senderhandle_len); 
    packetIndex += senderhandle_len;
    

    // Gets next token after command (either handle or number of handles)
    char *token = strtok_r(NULL, " ", (char **)saveptr);
    if (token == NULL) {
        printf("No handle entered\n");
        return 0;
    }

    uint8_t desthandle_len = 0;
    // Check if the token is a number
    if (isdigit(token[0])) {
        // Multicast command: continue creating packet
        char *endptr;
        long num_handles = strtol(token, &endptr, 10); //Grabs number of destination handles
        if (*endptr == '\0' && num_handles >= 2 && num_handles <= 9) { // must be numbers 2-9 for %C command
            printf("Number of handles: %ld\n", num_handles);
            
            // Copy number of handles into buffer (1 byte)
            memcpy(&packetBuffer[packetIndex++], &num_handles, sizeof(num_handles)); // Good if i want to implement %C here too
            
            // Grab destination handles and add into packet buffer
            for (int i = 0; i < num_handles; i++) { // Loop through number of handles to grab and add to packet buffer
                token = strtok_r(NULL, " ", (char **)saveptr);
                if (token == NULL) {
                    printf("Not enough handles entered\n");
                    return 0;
                }
                char *destination_handle = token;
                // Get destination handle lengths
                desthandle_len = strlen(destination_handle);
                // Copy handle length into buffer (1 byte)
                memcpy(&packetBuffer[packetIndex++], &desthandle_len, sizeof(desthandle_len));
                // Copy handle into buffer (variable length)
                memcpy(&packetBuffer[packetIndex], destination_handle, desthandle_len);
                packetIndex += desthandle_len;
            }
        }
        else { 
            printf("Invalid number of handles entered\n");
            return 0;
        }
    }
    else {
        // Unicast command: continue creating packet
        uint8_t num_handles = 1;
        // Copy number of handles into buffer (1 byte)
        memcpy(&packetBuffer[packetIndex++], &num_handles, sizeof(num_handles)); // Good if i want to implement %C here too

        char *destination_handle = token;
        // Get destination handle length
        desthandle_len = strlen(destination_handle);
        // Copy handle length into buffer (1 byte)
        memcpy(&packetBuffer[packetIndex++], &desthandle_len, sizeof(desthandle_len));

        // Copy handle into buffer (variable length)
        memcpy(&packetBuffer[packetIndex], destination_handle, desthandle_len); 
        packetIndex += desthandle_len;
    }

    uint8_t message[MAX_MSG_SIZE] = {0};
    int message_len = 0;
    // Get next token: message
    while ((token = strtok_r(NULL, " ", (char **)saveptr)) != NULL) {
        // Check if adding the token won't overflow the message buffer
        int token_len = strlen(token);
        if (message_len + token_len + 1 < MAX_MSG_SIZE) { // +1 for the space or null terminator
            // If not, append the token to the message buffer
            if (message_len > 0) {
                message[message_len] = ' ';  // Add a space before the next token if it's not the first token
                message_len++;  // Increment position for the space
            }
            memcpy(&message[message_len], token, token_len);  // Copy token into message buffer
            message_len += token_len;  // Update the message length
        } else {
            // Handle overflow (if needed, for example, by truncating the message)
            printf("Message too large. Exceeds 200 bytes\n");
            break;  // Optionally, break if you can't add more tokens
        }
    }
    message[message_len] = '\0';  // Null terminate the message
    printf("Full Message: %s\n", message);  // Debugging

    // Copy message into buffer
    memcpy(&packetBuffer[packetIndex], message, message_len); // Offset adjusted for sender handle length for more than 1 handle
    packetIndex += message_len;

    // Return the packet buffer length
    size_t packetBuffer_len = strlen((char *)packetBuffer);

    printf("Packet buffer length: %zu, %d\n", packetBuffer_len, packetIndex);  // Debugging
    printf("Packet buffer: %u%u%s\n", packetBuffer[0], packetBuffer[1], &packetBuffer[2]);  // Debugging
    return packetBuffer_len;
}


size_t broadcastCmd(uint8_t **saveptr, uint8_t *sender_handle) {

}


void listCmd(uint8_t *dataBuffer) {}