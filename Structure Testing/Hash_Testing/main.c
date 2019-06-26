#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOAD_FACTOR_PERCENTAGE 80
#define INITIAL_HASH_SIZE 503

#define BUFFER_SIZE 100+1           // length of buffer 
#define ENTITY_SIZE 30+1            // length of entity name
#define COMMAND_SIZE 10+1           // length of command input

#define TOMBSTONE 127

/*
 * Bucket of the hash table
 */
typedef struct hash_item {
    char* key;      // entity name
    int val;        // in final implementation -> val = struct rel_str
} hash_item_t;

/*
 * Hash table struct, array of buckets
 */
typedef struct hash_table {
    int size;
    int count;
    hash_item_t** buckets;        // start of the table
} hash_table_t;

/*
 * Create a new item for the hash table
 */
hash_item_t* create_new_item(const char* key, const int val) {
    
    hash_item_t* item = malloc(sizeof(hash_item_t));
    strncpy(item->key, key, ENTITY_SIZE);               // key is the entity name
    item->val = val;
    
    return item;
}

/*
 * Delete passed item
 */
void delete_item(hash_item_t* i) {
    
    free(i->key);
    free(i);
}

/*
 * Creates hash table and returns it address
 */
hash_table_t* create_table() {
    
    hash_table_t* tb = malloc(sizeof(hash_table_t));

    tb->size = INITIAL_HASH_SIZE;
    tb->count = 0;
    tb->buckets = calloc((size_t)tb->size, sizeof(hash_item_t*));
    
    return tb;
}

/*
 * Delete the hash table
 */
void delete_table(hash_table_t* tb) {
    
    hash_item_t* item; 
    for (int i=0; i<tb->size; i++) {
        item = tb->buckets[i];
        if (item) 
            delete_item(item);
    }
    
    free(tb->buckets);
    free(tb);
}

/*
 * Hash Table for Relation Table
 */
int main(int argc, char** argv) {
    
    FILE* input = fopen("../Test.txt", "r");
    
    char buffer[BUFFER_SIZE];           // buffer for each line
    char ent[ENTITY_SIZE];                          
    char command[COMMAND_SIZE];

    if (fgets(buffer, BUFFER_SIZE, input))        // read first line
        sscanf(buffer, "%s", command);

    while(strcmp(command, "end") != 0) {                      // if not "end" command, continues to read

        if (strcmp(command, "addent") == 0) {                 // addent

              
        }   

        else if (strcmp(command, "delent") == 0) {                 // addent

           
        }   

        else if (strcmp(command, "report") == 0) 
            
        
        if (fgets(buffer, BUFFER_SIZE, input))
            sscanf(buffer, "%s", command);
    }

    return(EXIT_SUCCESS);
}

