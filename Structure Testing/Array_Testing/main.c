#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_SIZE 10
#define ENT_SIZE 50+1
#define COMMAND_SIZE 10+1

#define TOMBSTONE 127

/*
 * Prints the Entity Array
 */
void print(char* arr[], int size) {
    
    int i = 0;
    for (i=0; i<size; i++) 
        printf("ent_arr[%d] = %s\n", i, arr[i]);  
}

/*
 * Reallocates the array, requires new length of the array
 */
char** reallocate(char* arr[], const int new_size) {
    
    int i = 0;   
    arr = realloc(arr, new_size * sizeof(char*));
    for (i=new_size>>1; i<new_size; i++) 
        arr[i] = calloc(ENT_SIZE, sizeof(char));         
    
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
int binary_search(char *arr[], int size, char *target) {
    
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
        strncpy(arr[elem_count++], new_elem, ENT_SIZE);     // dest, src
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
        strncpy(arr[target_pos], tombstone, ENT_SIZE);     // dest, src
        qsort(arr, elem_count, sizeof(char*), myCompare);     // sorting by alphabetical order
        elem_count--;
    }
    
    return elem_count;
}

/*
 * Dinamic Array for Entity Table
 */
int main(int argc, char** argv) {
    
    FILE* stdin = fopen("Nomi.txt", "r");
    char** ent_arr;
    int curr_size = INITIAL_SIZE;

    // initialization of array
    int i = 0;
    ent_arr = calloc(curr_size, sizeof(char*));
    for (i=0; i<curr_size; i++) 
        ent_arr[i] = calloc(ENT_SIZE, sizeof(char));
    
    int ent_count = 0;                              // number of entity in the array
    char buffer[ENT_SIZE + COMMAND_SIZE];           // buffer for each line
    char ent[ENT_SIZE];                          
    char command[COMMAND_SIZE];
    
    if (fgets(buffer, ENT_SIZE + COMMAND_SIZE, stdin))        // read first line
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
        
        if (fgets(buffer, ENT_SIZE + COMMAND_SIZE, stdin))
            sscanf(buffer, "%s", command);
    }
    
    print(ent_arr, curr_size);
    
    return(EXIT_SUCCESS);
}



