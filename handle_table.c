/****************************************************************
 * Author: Elizabeth Acevedo
 * 
 * Created: 02/02/2025
 * 
 * File: hashtable.c
 * 
 * Description: This program implements a hash table data
 *              structure
 * 
 * Utilization: Used in the server to store client handles 
 *              and socket numbers
 * 
 * Note: key    -> handle
 *       value  -> socket number
 * 
 ****************************************************************/
/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Definitions */
#define TABLE_SIZE 23

/* Structures */
typedef struct ClientNode {
    char *handle;               // Key associated with the value
    int socket;                 // Value associated with the key
    struct ClientNode *next;    // Pointer to the next node in the linked list (Handles collisions w/ chaining)
} ClientNode;

typedef struct {
    ClientNode *buckets[TABLE_SIZE];    // Array of linked lists
} HandleTable; // Hash table structure

/* Function Prototypes */
unsigned int hash_func(const char *handle);
HandleTable *create_table();
void addHandleSockPair(HandleTable *table, const char *handle, int socket);
int getSockNum(HandleTable *table, const char *handle);
void removeHandleSockPair(HandleTable *table, const char *handle);

/* Computes a hash value for a given handle */
unsigned int hash_func(const char *handle) {
    unsigned int hash = 0;  
    while (*handle) {
        hash = (hash * 31) + *handle++;
    }
    // returns an index in table where entry should be stored
    return hash % TABLE_SIZE;   // Keeps the hash within the table size bounds
}

/* Initializes a new handle table */
HandleTable *create_table() {
    HandleTable *table = malloc(sizeof(HandleTable));
    if (!table) {
        perror("Error creating handle table");
        exit(-1);
    }
    for (int i = 0; i < TABLE_SIZE; i++) {
        table->buckets[i] = NULL; // Initialize all buckets to NULL
    }
    return table;  // Returns a pointer to the table
}

/* Adds a new key-value pair into the table */
void addHandleSockPair(HandleTable *table, const char *handle, int socket) {
    unsigned int index = hash_func(handle); // Computes and gets the bucket index for the handle
    
    // Allocates memory for a new client entry node
    ClientNode *newNode = malloc(sizeof(ClientNode));
    // Error handling in memory allocation failure
    if (!newNode) {
        perror("Error creating new client node");
        //exit(-1);
    }

    // Allocates memory & copies the handle string
    newNode->handle = strdup(handle);
    // Error handling in memory allocation failure
    if (!newNode->handle) {
        perror("Error duplicating handle");
        free(newNode);
    }
    
    // Assigns the socket number to the new node
    newNode->socket = socket;
    // Checks if a bucket is empty before inserting
    newNode->next = table->buckets[index]; // Inserts at the head of the list
    table->buckets[index] = newNode;
}

/* Removes a key-value pair from the table */
void removeHandleSockPair(HandleTable *table, const char *handle) {
    unsigned int index = hash_func(handle);
    ClientNode *current = table->buckets[index];
    ClientNode *prev = NULL;
    
    while (current) {
        if (strcmp(current->handle, handle) == 0) {   // Found the key
            if (prev) {
                prev->next = current->next;     // Skip the deleted node
            } else {
                table->buckets[index] = current->next;  // Update the head of the list
            }
            free(current->handle);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

/* Retrieves the value associated with a given key */
int getSockNum(HandleTable *table, const char *handle) {
    unsigned int index = hash_func(handle);
    ClientNode *current = table->buckets[index];

    while (current) {   // Traverse the linked list & stops when current is NULL
        if (strcmp(current->handle, handle) == 0) {
            return (current->socket); // If found, return the socket number
        }
        current = current->next;
    }
    return -1; // Socket not found
}

/* Prints the contents of the table */
void print_table(HandleTable *table) {
    printf("\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        printf("Bucket %d: ", i);
        ClientNode *current = table->buckets[i];
        while (current) {
            printf("(%s, %d) -> ", current->handle, current->socket);
            current = current->next;
        }
        printf("NULL\n");   // Cosmetic Null
    }
}

/* Frees the memory allocated for the table */
void free_table(HandleTable *table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        ClientNode *current = table->buckets[i];
        while (current) {
            ClientNode *temp = current;
            current = current->next;
            free(temp->handle);
            free(temp);
        }
    }
    free(table);
}

int main(int argc, char * argv[]) {
    HandleTable *table = create_table();
    printf("Table created with added handles\n");

    addHandleSockPair(table, "SinisterBaloney", 6);
    addHandleSockPair(table, "DestroyerEli31", 8);
    addHandleSockPair(table, "VaccuumCleaner123", 3);
    addHandleSockPair(table, "ApplesNBananas934", 7);
    addHandleSockPair(table, "StaryNight", 9);
    print_table(table);
    
    // Test the get function
    if(getSockNum(table, "DestroyerEli31")) {
        printf("\n**** Value of 'DestroyerEli31': %d ****\n", getSockNum(table, "DestroyerEli31"));
    }
    else {
        printf("Key not found\n");
    }
 
    printf("\nTable after removing handles\n");   
    removeHandleSockPair(table, "SinisterBaloney");
    removeHandleSockPair(table, "DestroyerEli31");
    print_table(table);
    
    free_table(table);
    return 0;
}
