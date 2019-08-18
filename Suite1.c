#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

// Structure for destination array, one array for each relation 
typedef struct dest_str {
    
    char* dest;                         // name of the destination entity
    
    char** dest_of;                     // array of entity that he is destination of
    size_t dest_of_count;               // number of entity he is destination of 
    size_t dest_of_size;
    
} t_dest_str;

// Structure for relation array
typedef struct rel_str {
    
    char* rel;                          // name of the relation
    
    int n_most_dest;                     // number of relation received by these entities
    char** most_dest_arr;                // array of entities that are receiving the most for this relation
    size_t most_dest_count;             
    size_t most_dest_size;
    
    t_dest_str* dest_arr;               // entities that are destination of this relation
    size_t dest_count;
    size_t dest_size;
    
} t_rel_str;

// DEFINES

#define ENTITY_ARRAY_SIZE 2048
#define RELATION_ARRAY_SIZE 128

#define MOST_DESTINATION_ARRAY_SIZE 256
#define DESTINATION_ARRAY_SIZE 1024

#define DESTINATION_OF_SIZE 1024

#define BUFFER_SIZE 255+1               
#define ENTITY_SIZE 63+1                
#define RELATION_SIZE 63+1              
#define COMMAND_SIZE 7+1 

// END OF DEFINES

// GLOBAL VARIABLES

FILE* input;                            // input file
FILE* output;                           // output file

t_rel_str* rel_arr;                     // relation array
size_t rel_count;                       // current number of relation
size_t rel_size;                        // length of relation array

char** ent_arr;                         // entity array
size_t ent_count;                       // current number of entity 
size_t ent_size;                        // length of the entity array

// END OF GLOBAL VARIABLES

// DEBUGGING FUNCTIONS

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
 * Print destination structure
 */
void print_dest_str(t_dest_str el) {
    
    int i;
    
    printf("%s\n", el.dest);
    printf("\t\tdest of count: %zu\n", el.dest_of_count);
    printf("\t\tdest of arr ->");
    for (i=0; i<el.dest_of_count; i++)
        printf(" %s ", el.dest_of[i]);
    printf("\n");
}

/*
 * Print relation structure
 */
void print_rel_str(t_rel_str el) {
    
    int i;
    
    printf("%s\n", el.rel);
    printf("\tmost dest entity ->");
    for (i=0; i<el.most_dest_count; i++) 
        printf(" %s", el.most_dest_arr[i]);
    printf("\n\tmax rel received: %d\n", el.n_most_dest);
    printf("\tcurr total dest: %zu\n", el.dest_count);
    for (i=0; i<el.dest_count; i++) {
        printf("\tdest_arr[%d] -> ", i);
        print_dest_str(el.dest_arr[i]);
    }
}

/*
 * Print relation array
 */
void print_rel_arr() {
    
    int i;
    for (i=0; i<rel_count; i++) {
        printf("rel_elem[%d] -> ", i);
        print_rel_str(rel_arr[i]);
    }
    
    printf("\n");
}

// END OF DEBUGGING FUNCTIONS

/*
 * Release all memory
 */
void free_all();

/*
 * Sets up all array and their sizes
 */
void initialize();

/*
 * Reads file passed as input until 'end' is reached
 * Parses all commands and manages operations related to them
 */
void execute(FILE* input);

/*
 * Relationship monitoring built only with array structures
 */
int main(int argc, char** argv) {
    
    input = stdin;
    output = stdout;
    
    initialize();
    execute(input);
    free_all();

    return(EXIT_SUCCESS);
}

/*
 * Free passed relation structure
 */
static inline void free_rel_str(t_rel_str* rel_str) {
    
    int i;
    
    free(rel_str->most_dest_arr);
        
    for (i=0; i<rel_str->dest_count; i++) 
        free(rel_str->dest_arr[i].dest_of);

    free(rel_str->dest_arr);
    free(rel_str->rel);
}

/*
 * Release all memory
 */
void free_all() {
    
    int i, j;
    
    // free entity array
    for (i=0; i<ent_count; i++) 
        free(ent_arr[i]);
    free(ent_arr);

    // free relation array
    for (i=0; i<rel_count; i++) 
        free_rel_str(&rel_arr[i]);
       
    free(rel_arr);
}

/*
 * Sets up all array and their sizes
 */
void initialize() {
    
    // initialization of entity array
    ent_count = 0;
    ent_size = ENTITY_ARRAY_SIZE;
    ent_arr = calloc(ent_size, sizeof(char*));
    
    // initialization of report array
    rel_count = 0;
    rel_size = RELATION_ARRAY_SIZE;
    rel_arr = calloc(rel_size, sizeof(t_rel_str));
}

/*
 * Search for a string in the passed array. 
 * Return position of the corrisponding string if found, -1 else
 */
int search_string_array(char** arr, const size_t elem_count, char* target) {
    
    int bottom = 0;
    int mid;
    int top = elem_count - 1;
    
    while(bottom <= top) {      
        mid = (bottom + top)>>1;
        if (strcmp(arr[mid], target) == 0) 
            return mid;
        else if (strcmp(arr[mid], target) > 0)          
            top = mid - 1;
        else if (strcmp(arr[mid], target) < 0)
            bottom = mid + 1;
    }
    
    return -1;
}

/*
 * Compare function for qsort for array of strings
 */
static int string_compare(const void* a, const void* b) { 
    
    return strcmp(*(const char**)a, *(const char**)b);      // ascii order given by strcmp
} 

/*
 * Reallocate passed array of string with double the size. Returns new max size.
 */
static inline char** realloc_string_array(char** arr, size_t *max_size) {
    
    *max_size = (*max_size) << 1;                               // double the size
    
    return realloc(arr, (*max_size) * sizeof(char*));
}

/*
 * Adds entity into entity array in order. Double the size if it's full.
 */
void add_entity(char* new_ent) {
   
    if (search_string_array(ent_arr, ent_count, new_ent) == -1) {

        if (ent_count == ent_size) 
            ent_arr = realloc_string_array(ent_arr, &ent_size);

        ent_arr[ent_count] = calloc(ENTITY_SIZE, sizeof(char));
        strncpy(ent_arr[ent_count++], new_ent, ENTITY_SIZE);            // dest, src
        qsort(ent_arr, ent_count, sizeof(char*), string_compare);       // sorting by ascii order
    }
}

/*
 * Search for a relation in the relation array. 
 * Return position if found, -1 else
 */
int search_relation(char* target) {
    
    int bottom = 0;
    int mid;
    int top = rel_count - 1;
    
    while(bottom <= top) {      
        mid = (bottom + top)>>1;
        if (strcmp(rel_arr[mid].rel, target) == 0) 
            return mid;
        else if (strcmp(rel_arr[mid].rel, target) > 0)          
            top = mid - 1;
        else if (strcmp(rel_arr[mid].rel, target) < 0)
            bottom = mid + 1;
    }
    
    return -1;
}

/*
 * Create relation structure when a new relation is introduced
 * Fills relation name
 */
void fill_rel_str(t_rel_str* el, char* rel) {
    
    // copy name of relation
    el->rel = calloc(RELATION_SIZE, sizeof(char));
    strncpy(el->rel, rel, RELATION_SIZE);     // dest, src      
    
    el->n_most_dest = 0;
    el->most_dest_arr = calloc(MOST_DESTINATION_ARRAY_SIZE, sizeof(char*));     // create most destination array
    el->most_dest_count = 0;
    el->most_dest_size = MOST_DESTINATION_ARRAY_SIZE;
    
    el->dest_arr = calloc(DESTINATION_ARRAY_SIZE, sizeof(t_dest_str));          // create destination array   
    el->dest_count = 0; 
    el->dest_size = DESTINATION_ARRAY_SIZE;
}

/*
 * Inserts element into relation array in order. 
 * Does not check for boundaries nor membership.
 * Returns new element count.
 */
int insert_relation_element(t_rel_str* arr, char* new_elem, size_t elem_count) {
    
    int i, target;
    
    for (i=0; (i<elem_count) && (strcmp(arr[i].rel, new_elem)<0); i++);         // find place where to insert new element

    if (i == elem_count)   
        fill_rel_str(&arr[elem_count], new_elem);                               // if it's last element, just fill it
    else {
        target = i;
        for (i=elem_count-1; i>=target; i--)                                    // else, shift left than fill it
            memmove(&arr[i+1], &arr[i], sizeof(t_rel_str));
        fill_rel_str(&arr[target], new_elem);
    }
    
    return ++elem_count;
}

/*
 * Reallocate relation array with double the size
 */
static inline void realloc_rel_array() {
    
    rel_size = rel_size << 1;
    rel_arr = realloc(rel_arr, rel_size * sizeof(t_rel_str));
}

/*
 * Search for a destination in the destination array. 
 * Return position if found, -1 else
 */
int search_destination(t_dest_str* dest_arr, const int dest_count, char* target) {
    
    int bottom = 0;
    int mid;
    int top = dest_count - 1;
    
    while(bottom <= top) {      
        mid = (bottom + top)>>1;
        if (strcmp(dest_arr[mid].dest, target) == 0) 
            return mid;
        else if (strcmp(dest_arr[mid].dest, target) > 0)          
            top = mid - 1;
        else if (strcmp(dest_arr[mid].dest, target) < 0)
            bottom = mid + 1;
    }
    
    return -1;
}

/*
 * Reallocate destination array with double the size, return new size
 */
static inline t_dest_str* realloc_dest_array(size_t *max_dest, t_dest_str* dest_arr) {
    
    *max_dest = (*max_dest) << 1;
    
    return realloc(dest_arr, (*max_dest) * sizeof(t_dest_str));
}

/*
 * Create destination structure when a new destination for a relation is introduced
 * Fills destination name
 */
void fill_dest_str(t_dest_str* el, char* dest) {
         
    el->dest = dest;                // copy pointer of name of destination   
    
    el->dest_of = calloc(DESTINATION_OF_SIZE, sizeof(char*));      // create destination_of array
    el->dest_of_count = 0;
    el->dest_of_size = DESTINATION_OF_SIZE;
}

/*
 * Inserts element into destination array in order. 
 * Does not check for boundaries nor membership.
 * Returns new element count.
 */
int insert_dest_element(t_dest_str* arr, char* new_elem, size_t elem_count) {
    
    int i, target;
    
    for (i=0; (i<elem_count) && (strcmp(arr[i].dest, new_elem)<0); i++);        // find place where to insert new element

    if (i == elem_count)   
        fill_dest_str(&arr[elem_count], new_elem);                              // if it's last element, just fill it
    else {
        target = i;
        for (i=elem_count-1; i>=target; i--)                                    // else, shift left than fill it
            memmove(&arr[i+1], &arr[i], sizeof(t_dest_str));
        fill_dest_str(&arr[target], new_elem);
    }
    
    return ++elem_count;
}

/*
 * Update destination_of array with the new origin 
 */
void update_dest_of(t_dest_str* dest_str, char* orig) {
      
    if (dest_str->dest_of_count == dest_str->dest_of_size) 
        dest_str->dest_of = realloc_string_array(dest_str->dest_of, &dest_str->dest_of_size);

    int i, target;
    
    for (i=0; (i<dest_str->dest_of_count) && (strcmp(dest_str->dest_of[i], orig)<0); i++);   // find place where to insert new element

    if (i == dest_str->dest_of_count)                                                // if it's last, just insert
        dest_str->dest_of[i] = orig;
    else {
        target = i;
        for (i=dest_str->dest_of_count-1; i>=target; i--)                            // else, shift left the remaining array
            dest_str->dest_of[i+1] = dest_str->dest_of[i];
        dest_str->dest_of[target] = orig;                       
    }
    
    dest_str->dest_of_count++;
}

/*
 * Update relation structure
 * Most_dest_arr is not sorted, will be sorted by report
 */
static inline void update_rel_str(t_rel_str* rel_str, char* dest, int dest_of_count) {
    
    if (rel_str->n_most_dest < dest_of_count) {             // if new dest receives more relation 
        
        rel_str->most_dest_arr[0] = dest;
        rel_str->most_dest_count = 1;
        rel_str->n_most_dest = dest_of_count;
    }
    else if (rel_str->n_most_dest == dest_of_count) {       // if new dest receives same amount of relation
        
        if (rel_str->most_dest_count == rel_str->most_dest_size) 
            rel_str->most_dest_arr = realloc_string_array(rel_str->most_dest_arr, &rel_str->most_dest_size);
        rel_str->most_dest_arr[rel_str->most_dest_count++] = dest;
    }
}

/*
 * Adds new relation between two entity into relation array
 * Double the sizes of report array or relation array if full. 
 */
void add_rel(char* orig, char* dest, char* rel) {
    
    int dest_pos = search_string_array(ent_arr, ent_count, dest);
    int orig_pos = search_string_array(ent_arr, ent_count, orig);
    int pos;
    t_rel_str* rel_str;
    t_dest_str* dest_str;
    
    // if one of the entity is not registered, return
    if (dest_pos == -1 || orig_pos == -1)     
        return;
    
    // step 1: check if relation is present to use its destination array. If new, create new relation structure
    pos = search_relation(rel);
    if (pos == -1) {
        
        if (rel_count == rel_size)                                          // resize relation array if full
            realloc_rel_array();
        
        rel_count = insert_relation_element(rel_arr, rel, rel_count);
        rel_str = &rel_arr[search_relation(rel)];
    }
    else
        rel_str = &rel_arr[pos];
    
    // step 2: check if destination of relation is present in destination array. If not, create new destination structure.
    pos = search_destination(rel_str->dest_arr, rel_str->dest_count, dest);
    if (pos == -1) {
        
        if (rel_str->dest_count == rel_str->dest_size)                     // resize destination array if full
            rel_str->dest_arr = realloc_dest_array(&rel_str->dest_size, rel_str->dest_arr);
        
        rel_str->dest_count = insert_dest_element(rel_str->dest_arr, ent_arr[dest_pos], rel_str->dest_count);
        dest_str = &rel_str->dest_arr[search_destination(rel_str->dest_arr, rel_str->dest_count, dest)];
    }
    else
        dest_str = &rel_str->dest_arr[pos];
    
    // step 3: update dest of, src of and rel_str
    if (search_string_array(dest_str->dest_of, dest_str->dest_of_count, orig) == -1) {        // search if dest is already destination of orig. if not, update
        update_dest_of(dest_str, ent_arr[orig_pos]);
        update_rel_str(rel_str, ent_arr[dest_pos], dest_str->dest_of_count);
    }
}

/*
 * Deletes passed relation structure and fix relation array order
 */
void remove_rel_str(t_rel_str* rel_str, const int rel_pos) {
    int i;
    
    free_rel_str(rel_str);
    for (i=rel_pos; i<rel_count; i++)
        memmove(&rel_arr[i], &rel_arr[i+1], sizeof(t_rel_str));             // fix relation array shifting left
    rel_count--;
}

/*
 * Updates destination of count. If only one origin is present, removes dest_str 
 */
void update_dest_of_count(t_dest_str* dest_str, t_rel_str* rel_str, const int dest_pos, const int orig_pos) {
    
    int i;
    
    if (dest_str->dest_of_count == 1) {             // if only 1 origin is in the array
        
        free(dest_str->dest_of);                    // clean dest_str and fix dest_arr
        for (i=dest_pos; i<rel_str->dest_count; i++)                                    
            memmove(&rel_str->dest_arr[i], &rel_str->dest_arr[i+1], sizeof(t_dest_str));
        rel_str->dest_count--;
    }
    else {
        
        for (i=orig_pos; i<dest_str->dest_of_count; i++)        // remove the origin and fix dest_of_arr
            dest_str->dest_of[i] = dest_str->dest_of[i+1];
        dest_str->dest_of_count--;
    }
}

/*
 * Recomputes which entities receives most number of relation and updates most_dest_arr
 */
void recompute_most_dest(t_rel_str* rel_str) {
    
    int i;
    rel_str->most_dest_count = 0;                   // resets initial values
    rel_str->n_most_dest = 0;
    
    for(i=0; i<rel_str->dest_count; i++)            // for each destination, update most_dest _arr
        update_rel_str(rel_str, rel_str->dest_arr[i].dest, rel_str->dest_arr[i].dest_of_count);
}

/*
 * Delete, if exists, passed relation. 
 */
void del_rel(char* orig, char* dest, char* rel) {
    
    int i;
    int rel_pos, dest_pos, orig_pos, most_dest_pos;
    t_dest_str* dest_str;
    t_rel_str* rel_str;
    
    rel_pos = search_relation(rel);             // find relation structure
    if (rel_pos == -1)
        return;
    rel_str = &rel_arr[rel_pos];
    
    dest_pos = search_destination(rel_str->dest_arr, rel_str->dest_count, dest);        // find destination structure
    if (dest_pos == -1)
        return;
    dest_str = &rel_str->dest_arr[dest_pos];
    
    orig_pos = search_string_array(dest_str->dest_of, dest_str->dest_of_count, orig);       // find position in destination_of
    if (orig_pos == -1)
        return;
    
    // find position in most_dest_arr
    for (i=0; i<rel_str->most_dest_count && strcmp(rel_str->most_dest_arr[i], dest)!=0; i++);
    most_dest_pos = i;
    
    // if it's the only relation for this relation, remove its relation structure 
    if (rel_str->dest_count == 1 && dest_str->dest_of_count == 1) 
        remove_rel_str(rel_str, rel_pos);
    
    // if dest it's not in the most_dest_arr, update it's dest_of_count
    else if (most_dest_pos == rel_str->most_dest_count) 
        update_dest_of_count(dest_str, rel_str, dest_pos, orig_pos);
    
    // if dest it's in the most_dest_arr
    else {
        
        // if most_dest_count > 1, remove the relation and update it's dest_of_count
        if (rel_str->most_dest_count > 1) { 
            for (i=most_dest_pos; i<rel_str->most_dest_count; i++)              // fix most_dest_arr
                rel_str->most_dest_arr[i] = rel_str->most_dest_arr[i+1];
            rel_str->most_dest_count--;
            
            update_dest_of_count(dest_str, rel_str, dest_pos, orig_pos);
        }
        // if most_dest_count == 1, update dest_of_count and recompute most_dest_arr
        else {
            update_dest_of_count(dest_str, rel_str, dest_pos, orig_pos);
            recompute_most_dest(rel_str);
        }
    }
}

/*
 * Print the report results
 */
void report() {
    
    int i, j;
    t_rel_str el;
    
    if (rel_count == 0)                                 // no elements, print none
        fputs("none", output);
    
    else 
        for (i=0; i<rel_count; i++) {                   // for every element in report array
            
            el = rel_arr[i];
            qsort(el.most_dest_arr, el.most_dest_count, sizeof(char*), string_compare);              // sorting most dest arr for printing
            
            fputs(el.rel, output);                      // first print rel name
            fputs(" ", output);
            
            for(j=0; j<el.most_dest_count; j++) {       // second print most receivers entities
                fputs(el.most_dest_arr[j], output);
                fputs(" ", output);
            }
            
            fprintf(output, "%d", el.n_most_dest);      // third print number of relation received
            fputs("; ", output);
        }
    
    fputs("\n", output);
}

/*
 * Reads file passed as input until 'end' is reached
 * Parses all commands and manages operations related to them
 */
void execute(FILE* input) {
    
    char buffer[BUFFER_SIZE];           // buffer for each line
    char ent[ENTITY_SIZE];              // entity
    char dest[ENTITY_SIZE];             // destination of a relation
    char orig[ENTITY_SIZE];             // origin of a relation
    char rel[RELATION_SIZE];            // relation
    char command[COMMAND_SIZE];         // command
    
    if (fgets(buffer, BUFFER_SIZE, input))        // read first line
        sscanf(buffer, "%s", command);

    while(strcmp(command, "end") != 0) {                      // if not "end" command, continues to read

        if (strcmp(command, "addent") == 0) {                       // addent
            sscanf(buffer, "%s %s", command, ent);
            add_entity(ent);
            //print_ent_arr();        // REMOVE
        }   
        
        else if (strcmp(command, "delent") == 0) {                  // delent
            sscanf(buffer, "%s %s", command, ent);
            //printf("delent\n");       
        }   

        else if (strcmp(command, "addrel") == 0) {                  // addrel
            sscanf(buffer, "%s %s %s %s", command, orig, dest, rel);  
            add_rel(orig, dest, rel);
           // print_rel_arr();        // REMOVE
        }   
        
        else if (strcmp(command, "delrel") == 0) {                  // delrel
            sscanf(buffer, "%s %s %s %s", command, orig, dest, rel);   
            del_rel(orig, dest, rel);
            //print_rel_arr();        // REMOVE
        }   

        else if (strcmp(command, "report") == 0)                  // report
            report();
        
        if (fgets(buffer, BUFFER_SIZE, input))
            sscanf(buffer, "%s", command);
    }   
}
