#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>

#define INITIAL_ARRAY_SIZE 1024

#define BUFFER_SIZE 100+1               // length of buffer 
#define ENTITY_SIZE 30+1                // length of entity name
#define COMMAND_SIZE 10+1               // length of command input

#define TOMBSTONE 127

/*
 * Prints the Entity Array
 */
void print(char* arr[], int size) {
    
    int i = 0;
    for (i=0; i<size; i++) 
        printf("ent_arr[%d] = %s\n", i, arr[i]); 
    
    printf("\n");
}

/*
 * Reallocates the array, requires new length of the array
 */
char** reallocate(char* arr[], const int new_size) {
    
    int i = 0;   
    arr = realloc(arr, new_size * sizeof(char*));
    for (i=new_size>>1; i<new_size; i++) 
        arr[i] = calloc(ENTITY_SIZE, sizeof(char));         
    
    return arr;
}

/*
 * Compares function for qsort
 */
static int myCompare(const void* a, const void* b) { 
    
    return strcmp(*(const char**)a, *(const char**)b); 
} 
  
/*
 * Search the array for entity
 * Return position if found, -1 else
 */
int binary_search(char *arr[], const int size, char *target) {
    
    int bottom = 0;
    int mid;
    int top = size - 1;

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
 * Function to insert new element in the array
 * Reallocates if array is full
 * Returns new number of element
 */
int insert(char* arr[], char* new_elem, int elem_count) { 
    
    if (binary_search(arr, elem_count, new_elem) < 0) {
        strncpy(arr[elem_count++], new_elem, ENTITY_SIZE);     // dest, src
        qsort(arr, elem_count, sizeof(char*), myCompare);     // sorting by alphabetical order
    }

    return elem_count;
} 

/*
 * Function to delete element in the array
 * Does nothing if element is not present
 * Returns new number of element
 */
int delete(char* arr[], char* target, int elem_count) {
    
    char tombstone[1] = {TOMBSTONE};
    
    int target_pos = binary_search(arr, elem_count, target);
    if (target_pos >= 0) {
        strncpy(arr[target_pos], tombstone, ENTITY_SIZE);     // dest, src
        qsort(arr, elem_count, sizeof(char*), myCompare);     // sorting by alphabetical order
        elem_count--;
    }
    
    return elem_count;
}

/*
 * Dinamic Array for Entity Table
 */
int main(int argc, char** argv) {
    
    FILE* input = fopen("../Test.txt", "rw+");
    char** ent_arr;
    int curr_size = INITIAL_ARRAY_SIZE;
    int ent_count = 0;                              // number of entity in the array
    int i = 0;
    
    char buffer[BUFFER_SIZE];           // buffer for each line
    char ent[ENTITY_SIZE];                          
    char command[COMMAND_SIZE];
    
    // initialization of array
    ent_arr = calloc(curr_size, sizeof(char*));
    for (i=0; i<curr_size; i++) 
        ent_arr[i] = calloc(ENTITY_SIZE, sizeof(char));

    if (fgets(buffer, BUFFER_SIZE, input))        // read first line
        sscanf(buffer, "%s", command);

    while(strcmp(command, "end") != 0) {                      // if not "end" command, continues to read

        if (strcmp(command, "addent") == 0) {                 // addent

            sscanf(buffer, "%s %s", command, ent);
            if (ent_count == curr_size) {                 // resize array if needed
                curr_size *= 2;
                ent_arr = reallocate(ent_arr, curr_size);
            }
            ent_count = insert(ent_arr, ent, ent_count);       
        }   

        else if (strcmp(command, "delent") == 0) {                 // addent

            sscanf(buffer, "%s %s", command, ent);     
            ent_count = delete(ent_arr, ent, ent_count);       
        }   

        else if (strcmp(command, "report") == 0) 
            print(ent_arr, ent_count);
        
        if (fgets(buffer, BUFFER_SIZE, input))
            sscanf(buffer, "%s", command);
    }

    return(EXIT_SUCCESS);
}



