/****************************************************************
 * Author: Elizabeth Acevedo
 * 
 * Created: 02/02/2025
 * 
 * File: handle_table.c
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
#include "handle_table.h"

struct HandleTable *handletable;

/* Computes a hash value for a given handle */
unsigned int hash_func(const char *handle, int size) {
    unsigned int hash = 0;  
    while (*handle) {
        hash = (hash * 31) + *handle++;
    }
    // returns an index in table where entry should be stored
    return hash % size;   // Keeps the hash within the table size bounds
}

/* Initializes a new handle table */
void create_table(int size) {
    handletable = (struct HandleTable*)malloc(sizeof(struct HandleTable));
    if (!handletable) {
        perror("Error creating handle table");
        exit(-1);
    }

    handletable->buckets = calloc(size, sizeof(ClientNode *));
    if (!handletable->buckets) {
        perror("Error creating buckets");
        free(handletable);
        exit(-1);
    }
    handletable->size = size; // Sets the size of the table
    handletable->count = 0;   // Initializes the count of elements in the table

}

/* Resizes the table when it reaches a certain threshold */
void resize_table() {
    int new_size = handletable->size * 2;  // Doubles the size of the table
    rehash_table(new_size);   // Rehashes the table with the new size
}

/* Rehashes the table with a new size */
void rehash_table(int new_size) {
    // Allocates memory for the new buckets
    ClientNode **newBuckets = calloc(new_size, sizeof(ClientNode *));
    // Error handling in memory allocation failure
    if (!newBuckets) {
        perror("Error creating new buckets");
        free(handletable->buckets);
        exit(-1);
    }

    for (int i = 0; i < handletable->size; i++) {
        ClientNode *current = handletable->buckets[i];
        while (current) {
            unsigned int new_index = hash_func(current->handle, new_size);

            ClientNode *newNode = malloc(sizeof(ClientNode));
            newNode->handle = strdup(current->handle);
            newNode->socket = current->socket;
            newNode->next = newBuckets[new_index];
            newBuckets[new_index] = newNode;

            // Free the old node
            ClientNode *temp = current;
            current = current->next;
            free(temp->handle);
            free(temp);
        }
    }
    free(handletable->buckets);
    handletable->buckets = newBuckets;
    handletable->size = new_size;
}

/* Adds a new key-value pair into the table */
void addHandleSockPair(const char *handle, int socket) {
    // Checks if the table is full before adding a new element

    if (handletable == NULL) {
        printf("better luck next time!!\n");
    }

    if (handletable->count == handletable->size || (float)(handletable->count / handletable->size) >= PERCENT_FULL) {
        resize_table();
    }
    

    unsigned int index = hash_func(handle, handletable->size); // Computes and gets the bucket index for the handle
    

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
        return;
    }

    // Assigns the socket number to the new node
    newNode->socket = socket;
    // Checks if a bucket is empty before inserting
    newNode->next = handletable->buckets[index]; // Inserts at the head of the list
    handletable->buckets[index] = newNode;

    handletable->count++; // Increments the count of elements in the table
}

/* Removes a key-value pair from the table */
void removeHandleSockPair(const char *handle) {
    unsigned int index = hash_func(handle, handletable->size);
    ClientNode *current = handletable->buckets[index];
    ClientNode *prev = NULL;
    
    while (current) {
        if (strcmp(current->handle, handle) == 0) {   // Found the key
            if (prev) {
                prev->next = current->next;     // Skip the deleted node
            } else {
                handletable->buckets[index] = current->next;  // Update the head of the list
            }
            free(current->handle);
            free(current);
            handletable->count--; // Decrements the count of elements in the table
            return;
        }
        prev = current;
        current = current->next;
    }
}

/* Retrieves the value associated with a given key */
int getSockNum(const char *handle) {
    unsigned int index = hash_func(handle, handletable->size);
    ClientNode *current = handletable->buckets[index];

    while (current) {   // Traverse the linked list & stops when current is NULL
        if (strcmp(current->handle, handle) == 0) {
            return (current->socket); // If found, return the socket number
        }
        current = current->next;
    }
    return -1; // Socket not found
}


/* Prints the contents of the table */
/*
void print_table() {
    printf("\n");
    for (int i = 0; i < handletable->size; i++) {
        printf("Bucket %d: ", i);
        ClientNode *current = handletable->buckets[i];
        while (current) {
            printf("(%s, %d) -> ", current->handle, current->socket);
            current = current->next;
        }
        printf("NULL\n");   // Cosmetic Null
    }
}
*/

/***************** Anything below was used for testing purposes ********************/
/* Frees the memory allocated for the table */
void free_table() {
    for (int i = 0; i < handletable->size; i++) {
        ClientNode *current = handletable->buckets[i];
        while (current) {
            ClientNode *temp = current;
            current = current->next;
            free(temp->handle);
            free(temp);
        }
    }
    free(handletable->buckets);
    free(handletable);
}

/*
int main(int argc, char * argv[]) {
    HandleTable *table = create_table(INIT_TABLE_SIZE);
    printf("Table created with added handles\n");

    addHandleSockPair(table, "Engineer831", 2);
    addHandleSockPair(table, "HelloWorldUserUsesThisUsername1", 4);
    addHandleSockPair(table, "SinisterBaloney", 6);
    addHandleSockPair(table, "DestroyerEli31", 8);
    addHandleSockPair(table, "VaccuumCleaner123", 3);
    addHandleSockPair(table, "ApplesNBananas934", 7);
    addHandleSockPair(table, "StarryNight", 9);
    addHandleSockPair(table, "BaconEggSandwich45869335", 5);
    addHandleSockPair(table, "WinterNightsAreCold1234", 10);
    addHandleSockPair(table, "CallMeOnMyCellPhone", 11);
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
*/
