#ifndef HANDLE_TABLE_H
#define HANDLE_TABLE_H

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Definitions */
#define INIT_TABLE_SIZE 11
#define PERCENT_FULL 0.55 // Should resize after 6 elements are added to a table if they hash to separate buckets

/* Structures */
typedef struct ClientNode {
    char *handle;            // Key associated with the value
    int socket;                 // Value associated with the key
    struct ClientNode *next;    // Pointer to the next node in the linked list (Handles collisions w/ chaining)
} ClientNode;

typedef struct {
    ClientNode **buckets;    // Array of pointers to linked lists
    int size;                // Size of the table
    int count;               // Number of elements in the table
} HandleTable; // Hash table structure

/* Function Prototypes */
unsigned int hash_func(const char *handle, int size);
HandleTable *create_table(int size);
void resize_table(HandleTable *table);
void rehash_table(HandleTable *table, int new_size);
void addHandleSockPair(HandleTable *table, const char *handle, int socket);
int getSockNum(HandleTable *table, const char *handle);
void removeHandleSockPair(HandleTable *table, const char *handle);
//void print_table(HandleTable *table); // For debugging
//void free_table(HandleTable *table); // Frees the memory allocated for the table


#endif