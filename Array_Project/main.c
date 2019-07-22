#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

// Structure of relation table, one table for each relation 
typedef struct rel_str {
    
    char* dest;
    int n_rel;
    char** dest_of;
    char** src_of;
    
} t_rel_str;

// Structure of report array
typedef struct rep_el {
    
    char* rel;
    char** max_dest;
    int n_rel;
    t_rel_str* rel_arr;
    
} t_rep_el;

// DEFINES

#define REPORT_ARRAY_SIZE 128   
#define MAX_DESTINATION_SIZE 32

#define ENTITY_ARRAY_SIZE 4096 

#define RELATION_ARRAY_SIZE 1024
#define DESTINATION_OF_SIZE 512
#define SOURCE_OF_SIZE 512

#define BUFFER_SIZE 256+1               
#define ENTITY_SIZE 64+1                
#define RELATION_SIZE 64+1              
#define COMMAND_SIZE 6+1 

// END OF DEFINES

// GLOBAL VARIABLES

FILE* file;                         // input file
t_rep_el* rep_arr;                  // report array

char** ent_arr;                     // entity array
int current_ent;                    // current number of entity in entity array
int max_ent;                        // length of the entity array

t_rel_str* rel_arr;                 // relation array

// END OF GLOBAL VARIABLES

// CONTROL FUNCTIONS

/*
 * Print entity array
 */
void print_ent_arr() {
    
    int i;
    for (i=0; i<current_ent; i++) 
        printf("ent_arr[%d] = %s\n", i, ent_arr[i]); 
    
    printf("\n");
}

// END OF CONTROL FUNCTIONS

/*
 * Search for a new entity in the array. 
 * Return position if found, -1 else
 */
int search_entity(char* target) {
    
    int bottom = 0;
    int mid;
    int top = current_ent - 1;

    while(bottom <= top) {                            
        mid = (bottom + top)>>1;
        if (strcmp(ent_arr[mid], target) == 0) 
            return mid;
        else if (strcmp(ent_arr[mid], target) > 0)          
            top = mid - 1;
        else if (strcmp(ent_arr[mid], target) < 0)
            bottom = mid + 1;
    }
    
    return -1;
}

/*
 * Compare function for qsort
 */
static int myCompare(const void* a, const void* b) { 
    
    return strcmp(*(const char**)a, *(const char**)b); 
} 

/*
 * Adds entity into entity array. Double the size if it's full.
 */
void add_entity(char* new_ent) {
    
    if (search_entity(new_ent) == -1) {
        strncpy(ent_arr[current_ent++], new_ent, ENTITY_SIZE);     // dest, src
        qsort(ent_arr, current_ent, sizeof(char*), myCompare);     // sorting by alphabetical order
    }
}

/*
 * Reads file passed as input until 'end' is reached
 * Parses all commands and manages operations related to them
 */
void read_file(FILE* file) {
    
    char buffer[BUFFER_SIZE];           // buffer for each line
    char ent[ENTITY_SIZE];              // entity
    char dest[ENTITY_SIZE];             // destination of a relation
    char orig[ENTITY_SIZE];             // origin of a relation
    char rel[RELATION_SIZE];            // relation
    char command[COMMAND_SIZE];         // command
    
    if (fgets(buffer, BUFFER_SIZE, file))        // read first line
        sscanf(buffer, "%s", command);

    while(strcmp(command, "end") != 0) {                      // if not "end" command, continues to read

        if (strcmp(command, "addent") == 0) {                       // addent
            sscanf(buffer, "%s %s", command, ent);
            add_entity(ent);
        }   
        
        else if (strcmp(command, "delent") == 0) {                  // delent
            sscanf(buffer, "%s %s", command, ent);
            printf("delent\n");
        }   

        else if (strcmp(command, "addrel") == 0) {                  // addrel
            sscanf(buffer, "%s %s %s", orig, dest, rel);  
            printf("addrel\n");
        }   
        
        else if (strcmp(command, "delrel") == 0) {                  // delrel
            sscanf(buffer, "%s %s %s", orig, dest, rel);   
            printf("delrel\n");
        }   

        else if (strcmp(command, "report") == 0) {                  // report
            printf("report\n");
        }
        
        if (fgets(buffer, BUFFER_SIZE, file))
            sscanf(buffer, "%s", command);
    }   
}

/*
 * Sets up all array and their sizes
 */
void initialize() {
    
    int i;
    
    // initialization of entity array
    current_ent = 0;
    max_ent = ENTITY_ARRAY_SIZE;
    ent_arr = calloc(max_ent, sizeof(char*));
    for (i=0; i<max_ent; i++) 
        ent_arr[i] = calloc(ENTITY_SIZE, sizeof(char));
}

/*
 * Relationship monitoring built only with array structures
 */
int main(int argc, char** argv) {
    
    initialize();
    
    file = fopen("../Test.txt", "r+");
    read_file(file);

    return(EXIT_SUCCESS);
}

