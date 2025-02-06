/****************************************************************
 * Author: Elizabeth Acevedo
 * 
 * Created: 02/03/2025
 * 
 * File: packetFactory.c
 * 
 * Description: This program handles the commands entered 
 *              by the user and handles other tasks. 
 * 
 * Utilization: Expected user input format on terminal:
 *              $: %M <handle> <message>
 *              $: %C <num_handles> <handle1> <handle2> ... <handleN> <message>
 *              $: %B <message>
 *              $: %L
 * 
 ****************************************************************/
/* Includes */
#include "packetFactory.h"

/* Definitions */
#define MAX_MSG_SIZE 200

/* Reads commands inputted from user on terminal and calls functions to handle command specs */
size_t readCommand(uint8_t *refBuff, uint8_t *dataBuffer, uint8_t *sender_handle) {
    size_t packet_len = 1;
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
        // Call the sendMsgCmd function
        packet_len = sendMsgCmd(dataBuffer, (uint8_t **)&saveptr, sender_handle, 0);
    }
        // Check if the token is send message
    else if (strcmp(token, "%C") == 0 || strcmp(token, "%c") == 0) {
        // Call the sendMsgCmd function
        packet_len = sendMsgCmd(dataBuffer, (uint8_t **)&saveptr, sender_handle, 1);
    }
        // Check if the token is send message
    else if (strcmp(token, "%B") == 0 || strcmp(token, "%b") == 0) {
        // Call the broadcastCmd function
        packet_len = broadcastCmd(dataBuffer, (uint8_t **)&saveptr, sender_handle);
    }
        // Check if the token is send message
    else if (strcmp(token, "%L") || strcmp(token, "%l")) {
        // Call the listCmd function
        packetFlag10(dataBuffer);
    }
    else {
        printf("Invalid command entered\n");
    }

    return packet_len;
}

/* Creates packet for send message %M command */
size_t sendMsgCmd(uint8_t *packetBuffer, uint8_t **saveptr, uint8_t *sender_handle, int isMulticast) { // Issues with buffer allocation and size
    uint8_t Mflag = 5; // Flag for send message command
    uint8_t Cflag = 6; // Flag for multicast command
    int packetIndex = 0;
  
    // Get length of sender handle
    uint8_t senderhandle_len = strlen((char *)sender_handle);
    if (senderhandle_len > 100) {
        printf("Error: Sender handle exceeds 100 bytes\n");
        return 0;
    }

    // Add flag into buffer depending on command
    if (isMulticast) {
        // add flag into buffer
        packetBuffer[packetIndex++] = Cflag;
    }
    else {
        // add flag into buffer
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
            break;
        }
    }
    message[message_len] = '\0';  // Null terminate the message

    // Copy message into buffer
    memcpy(&packetBuffer[packetIndex], message, message_len); // Offset adjusted for sender handle length for more than 1 handle
    packetIndex += message_len;

    // Return the packet buffer length
    size_t packetBuffer_len = strlen((char *)packetBuffer);

    return packetBuffer_len;
}

/* Creates packet for broadcast %B command */
size_t broadcastCmd(uint8_t *packetBuffer, uint8_t **saveptr, uint8_t *sender_handle) {
    uint8_t Bflag = 4; // Flag for broadcast command
    int packetIndex = 0;
    // Add flag into buffer
    packetBuffer[packetIndex++] = Bflag; // Add flag into buffer

    // Get length of sender handle
    uint8_t senderhandle_len = strlen((char *)sender_handle);
    if (senderhandle_len > 100) {
        printf("Error: Sender handle exceeds 100 bytes\n");
        return 0;
    }

    // Copy sender handle length into buffer (1 byte)
    memcpy(&packetBuffer[packetIndex++], &senderhandle_len, sizeof(senderhandle_len));
    // Copy sender handle into buffer (variable length)
    memcpy(&packetBuffer[packetIndex], sender_handle, senderhandle_len); 
    packetIndex += senderhandle_len;
    

    // Gets next token after command (either handle or number of handles)
    char *token;

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

    // Copy message into buffer
    memcpy(&packetBuffer[packetIndex], message, message_len); // Offset adjusted for sender handle length for more than 1 handle
    packetIndex += message_len;

    // Return the packet buffer length
    size_t packetBuffer_len = strlen((char *)packetBuffer);

    return packetBuffer_len;
}

/* Processes flag 1 command */
void processFlag1(uint8_t *dataBuffer, int clientSocket) {
    // Get handle length
    uint8_t handle_len = dataBuffer[1];
    uint8_t handle[MAX_H] = {0};
    uint8_t returnPacket[MAX_H + 2] = {0};
    uint8_t returnFlag = 0;

    // Copy handle into buffer
    memcpy(handle, &dataBuffer[2], handle_len);
    // Check if handle is already in the table
    if (getSockNum((char *)handle) == -1) {
        returnFlag = 2;
        returnPacket[0] = returnFlag;
        addHandleSockPair((const char *)handle, clientSocket);
        sendPDU(clientSocket, returnPacket, 1);
    }
    // Handle is in the table
    else {
        returnFlag = 3;
        returnPacket[0] = returnFlag;
        sendPDU(clientSocket, returnPacket, 1);
    }
}

/* Processes list command (calls flag functions)*/
void listCmd(int clientSocket) {
    	sendFlag11(clientSocket);
		sendFlag12(clientSocket);
		sendFlag13(clientSocket);
}

/* Creates packet for flag 10 command */
void packetFlag10(uint8_t *packetBuffer) {
	uint8_t flag = 10;
	packetBuffer[0] = flag;
}

/* Server sends flag packet to client holding number of handles */
void sendFlag11(int clientSocket) {
	// Get all handles from the handle table
	int num_handles = get_handle_count();

	// Convert the number of handles to network byte order
	uint32_t num_handles_net = htonl(num_handles);
	
	// Create a packet buffer to send to the client
	uint8_t packetBuffer[MAXBUF] = {0};
	int packetIndex = 0;
	
	// Add flag 11 to the packet buffer
	packetBuffer[packetIndex++] = 11;

	// Add the number of handles to the packet buffer
	memcpy(&packetBuffer[packetIndex], &num_handles_net, sizeof(num_handles_net));
	packetIndex += sizeof(num_handles_net); // increment packetIndex by 4 bytes

	// Send the packet to the client
	sendPDU(clientSocket, packetBuffer, packetIndex);
	
}

/* Server sends flag packet to client holding handle name */
void sendFlag12(int clientSocket) {
	int num_handles = get_handle_count();
	char **handle_list = get_all_handles(&num_handles);
	int flag = 12;

	// 
	for (int handle_index = 0; handle_index < num_handles; handle_index++) {
		uint8_t packetBuffer[MAXBUF] = {0};
		uint8_t handle_len = strlen(handle_list[handle_index]);
		// flag, handle len, handle
		packetBuffer[0] = flag;
		packetBuffer[1] = handle_len; // handle length of current handle
		memcpy(&packetBuffer[2], handle_list[handle_index], handle_len); // copy handle into packet buffer
		sendPDU(clientSocket, packetBuffer, handle_len + 2); // send packet to client (Handle + (flag + handle length))
	}

	// Free the memory allocated for the handle list
	for (int i = 0; i < num_handles; i++) {
		free(handle_list[i]);
	}
	free(handle_list);
}

/* Server sends flag packet to client telling client command is done */
void sendFlag13(int clientSocket) {
	int flag = 13;
	uint8_t packetBuffer[MAXBUF] = {0};
	packetBuffer[0] = flag;
	sendPDU(clientSocket, packetBuffer, 1); // send packet to client
}

