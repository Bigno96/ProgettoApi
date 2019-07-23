#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

// Structure for destination array, one array for each relation 
typedef struct dest_str {
    
    char* dest;
    int n_dest;
    char** dest_of;
    char** src_of;
    
} t_dest_str;

// Structure for relation array
typedef struct rel_str {
    
    char* rel;
    char** max_dest_arr;
    int n_max_dest;
    t_dest_str* dest_arr;
    int curr_dest;
    int max_dest;
    
} t_rel_str;

// DEFINES

#define RELATION_ARRAY_SIZE 64
#define MAX_DESTINATION_ARRAY_SIZE 16

#define ENTITY_ARRAY_SIZE 1024 

#define DESTINATION_ARRAY_SIZE 128
#define DESTINATION_OF_SIZE 128
#define SOURCE_OF_SIZE 128

#define BUFFER_SIZE 256+1               
#define ENTITY_SIZE 64+1                
#define RELATION_SIZE 64+1              
#define COMMAND_SIZE 6+1 

// END OF DEFINES

// GLOBAL VARIABLES

FILE* file;                             // input file

t_rel_str* rel_arr;                     // relation array
size_t rel_count;                       // current number of relation
size_t max_rel;                         // length of relation array

char** ent_arr;                         // entity array
size_t ent_count;                       // current number of entity 
size_t max_ent;                         // length of the entity array

t_dest_str null_dest_str = { "NULL", 0, NULL, NULL };     // null relation structure

// END OF GLOBAL VARIABLES

// CONTROL FUNCTIONS

/*
 * Print entity array
 */
void print_ent_arr() {
    
    int i;
    for (i=0; i<ent_count; i++) 
        printf("ent_arr[%d] = %s\n", i, ent_arr[i]); 
    
    printf("\n");
}

/*
 * Print relation structure
 */
void print_rel_str(t_rel_str el) {
    
    printf("\trel_name: %s\n", el.rel);
}

/*
 * Print relation array
 */
void print_rel_arr() {
    
    int i;
    for (i=0; i<rel_count; i++) {
        printf("rep_elem[%d] -> ", i);
        print_rel_str(rel_arr[i]);
    }
    
    printf("\n");
}

// END OF CONTROL FUNCTIONS

/*
 * Search for an entity in the array. 
 * Return position if found, -1 else
 */
int search_entity(char* target) {
    
    int bottom = 0;
    int mid;
    int top = ent_count - 1;

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
 * Compare function for qsort for entity array
 */
static int ent_compare(const void* a, const void* b) { 
    
    return strcmp(*(const char**)a, *(const char**)b); 
} 

/*
 * Reallocates entity array with double the size
 */
void realloc_ent_array() {
    
    int i;
    int new_size = max_ent << 1;   
    max_ent = new_size;
    
    ent_arr = realloc(ent_arr, new_size * sizeof(char*));
    for (i=new_size>>1; i<new_size; i++) 
        ent_arr[i] = calloc(ENTITY_SIZE, sizeof(char));   
}

/*
 * Adds entity into entity array. Double the size if it's full.
 */
void add_entity(char* new_ent) {
    
    if (search_entity(new_ent) == -1) {
        if (ent_count == max_ent) 
            realloc_ent_array();
        strncpy(ent_arr[ent_count++], new_ent, ENTITY_SIZE);     // dest, src
        qsort(ent_arr, ent_count, sizeof(char*), ent_compare);     // sorting by ascii order
    }
}

/*
 * Search for a relation in the relation array. 
 * Return corrisponding destination array if found, null else
 */
t_dest_str* search_relation(char* target) {
    
    int bottom = 0;
    int mid;
    int top = rel_count - 1;
    
    while(bottom <= top) {      
        mid = (bottom + top)>>1;
        if (strcmp(rel_arr[mid].rel, target) == 0) 
            return rel_arr[mid].dest_arr;
        else if (strcmp(rel_arr[mid].rel, target) > 0)          
            top = mid - 1;
        else if (strcmp(rel_arr[mid].rel, target) < 0)
            bottom = mid + 1;
    }
    
    return NULL;
}

/*
 * Compare function for qsort for relation array
 */
static int rel_compare(const void* a, const void* b) { 
    
    return strcmp(((t_rel_str*)a)->rel, ((t_rel_str*)b)->rel); 
} 

/*
 * Create relation structure when a new relation is introduced
 * Fills relation name
 * Returns destination array
 */
t_dest_str* fill_rel_str(t_rel_str* el, char* rel, char* dest) {
    
    // copy name of relation
    el->rel = calloc(RELATION_SIZE, sizeof(char));
    strncpy(el->rel, rel, RELATION_SIZE);     // dest, src      
    
    el->max_dest_arr = calloc(MAX_DESTINATION_ARRAY_SIZE, sizeof(char*));      // create max destination array
    el->dest_arr = calloc(DESTINATION_ARRAY_SIZE, sizeof(t_dest_str));      // create destination array   
    
    return(el->dest_arr);
}

/*
 * Reallocate relation array with double the size
 */
void realloc_rel_array() {
    
    max_rel = max_rel << 1;
    rel_arr = realloc(rel_arr, max_rel * sizeof(t_rel_str));
}

/*
 * Search for a destination in the destination array. 
 * Return corrisponding destination structure if found, null structure else
 */
t_dest_str search_destination(t_dest_str* dest_arr, int dest_count, char* target) {
    
    int bottom = 0;
    int mid;
    int top = dest_count - 1;
    
    while(bottom <= top) {      
        mid = (bottom + top)>>1;
        if (strcmp(dest_arr[mid].dest, target) == 0) 
            return dest_arr[mid];
        else if (strcmp(dest_arr[mid].dest, target) > 0)          
            top = mid - 1;
        else if (strcmp(dest_arr[mid].dest, target) < 0)
            bottom = mid + 1;
    }
    
    return null_dest_str;
}

/*
 * Compare function for qsort for destination array
 */
static int dest_compare(const void* a, const void* b) { 
    
    return strcmp(((t_dest_str*)a)->dest, ((t_dest_str*)b)->dest); 
} 

/*
 * Reallocate destination array with double the size, return new size
 */
size_t realloc_dest_array(size_t max_dest, t_dest_str* dest_arr) {
    
    max_dest = max_dest << 1;
    dest_arr = realloc(dest_arr, max_dest * sizeof(t_dest_str));
    
    return max_dest;
}

/*
 * Adds new relation between two entity into report array and relation array.
 * Double the sizes of report array or relation array if full. 
 */
void add_rel(char* orig, char* dest, char* rel) {
    
    // if one of the entity is not registered, return
    if (search_entity(dest) == -1 || search_entity(orig) == -1)     
        return;
    
    // step 1: check if relation is new or not to use its destination array. If new, create new relation structure
    t_dest_str* dest_arr = search_relation(rel);
    
    if (!dest_arr) {
        if (rel_count == max_rel)                                       // resize relation array if full
            realloc_rel_array();
        dest_arr = fill_rel_str(&rel_arr[rel_count++], rel, dest);       // create new relation structure
        qsort(rel_arr, rel_count, sizeof(t_rel_str), rel_compare);       // sorting by ascii order of relation
    }
    
    // step 2: check if destination of relation is present in destination array. If not, create new destination structure.
    
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
            print_ent_arr();        // REMOVE
        }   
        
        else if (strcmp(command, "delent") == 0) {                  // delent
            sscanf(buffer, "%s %s", command, ent);
            printf("delent\n");
        }   

        else if (strcmp(command, "addrel") == 0) {                  // addrel
            sscanf(buffer, "%s %s %s %s", command, orig, dest, rel);  
            add_rel(orig, dest, rel);
            print_rel_arr();        // REMOVE
        }   
        
        else if (strcmp(command, "delrel") == 0) {                  // delrel
            sscanf(buffer, "%s %s %s %s", command, orig, dest, rel);   
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
    ent_count = 0;
    max_ent = ENTITY_ARRAY_SIZE;
    ent_arr = calloc(max_ent, sizeof(char*));
    for (i=0; i<max_ent; i++) 
        ent_arr[i] = calloc(ENTITY_SIZE, sizeof(char));
    
    // initialization of report array
    rel_count = 0;
    max_rel = RELATION_ARRAY_SIZE;
    rel_arr = calloc(max_rel, sizeof(t_rel_str));
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

