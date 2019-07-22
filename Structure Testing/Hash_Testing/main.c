#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define LOAD_FACTOR_PERCENTAGE 80           // load factor tolerated before resizing
#define INITIAL_HASH_SIZE 503               // initial hash size, prime number

#define PRIME_SEED 163                  // seed for hash function, prime > 128

#define DELETED_VALUE -10               // marker for value of deleted item
#define NOT_FOUND_VALUE -1              // marker for value of an item not found

#define BUFFER_SIZE 100+1           // length of buffer 
#define ENTITY_SIZE 30+1            // length of entity name
#define COMMAND_SIZE 10+1           // length of command input

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

static hash_item_t DELETED_ITEM = {NULL, DELETED_VALUE};

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
 * Delete item passed as parameter 
 */
void delete_item(hash_item_t* i) {
    
    free(i->key);
    free(i);
}

/*
 * Creates hash table and returns it address
 */
hash_table_t* create_table(size_t size) {
    
    hash_table_t* ht = malloc(sizeof(hash_table_t));

    ht->size = size;
    ht->count = 0;
    ht->buckets = calloc((size_t)ht->size, sizeof(hash_item_t*));
    
    return ht;
}

/*
 * Delete the hash table
 */
void delete_table(hash_table_t* ht) {
    
    hash_item_t* item; 
    for (int i=0; i<ht->size; i++) {
        item = ht->buckets[i];
        if (item) 
            delete_item(item);
    }
    
    free(ht->buckets);
    free(ht);
}

/*
 * Convert string s into a int using power of PRIME_SEED
 */
static inline long ascii_value(const char* s) {
    
    long value = 0;
    const int len_s = strlen(s);
    for (int i=0; i<len_s; i++) 
        value += (long) pow(PRIME_SEED, i+1) * s[i];
    
    return value;
}

/*
 * Return primary hash value. hash = k mod m. 
 */
static inline int primary_hash(const long k, const int m) {
    
    return k % m;
}

/*
 * Return secondary hash value. hash = 1 + k mod m'
 */
static inline int secondary_hash(const long k, const int m) {

    return 1 + (k % (m-1));
}

/*
 * Return secondary hash value. hash = 1 + k mod m'
 */
static inline int get_hash(const long k, const int m, const int attempt) {

    return (primary_hash(k,m) + attempt * secondary_hash(k,m)) % m;
}

/*
 * Search value associated to passed key. Return NOT_FOUND_VALUE if not found
 */
int search(hash_table_t* ht, const char* key) {
    
    long k = ascii_value(key);
    int index = get_hash(k, ht->size, 0);
    hash_item_t* item = ht->buckets[index];
    
    int i = 1;
    while (item) {
        
        if (item != &DELETED_ITEM) 
            if (strcmp(item->key, key) == 0) 
                return item->val;   
        
        index = get_hash(k, ht->size, i);
        item = ht->buckets[index];
        i++;
    } 
    
    return NOT_FOUND_VALUE;
}

/*
 * Insert key - value couple into the hash table. If key already present, does nothing
 */
void insert(hash_table_t* ht, const char* key, int value) {
    
    if (search(ht, key) != NOT_FOUND_VALUE)
        return;
    
    hash_item_t* item = create_new_item(key, value);
    
    long k = ascii_value(key);
    int index = get_hash(k, ht->size, 0);
    hash_item_t* cur_item = ht->buckets[index];
    
    int i = 1;
    while (cur_item && cur_item != &DELETED_ITEM) {
        index = get_hash(k, ht->size, i);
        cur_item = ht->buckets[index];
        i++;
    } 
    
    ht->buckets[index] = item;
    ht->count++;
}

/*
 * Deleting key - value couple. When delete, leaves a tombstone to not interrupt chain path to other element
 */
void delete(hash_table_t* ht, const char* key) {
    
    long k = ascii_value(key);
    int index = get_hash(k, ht->size, 0);
    hash_item_t* item = ht->buckets[index];
    
    int i = 1;
    while (item) {
        
        if (item != &DELETED_ITEM) 
            if (strcmp(item->key, key) == 0) {
                delete_item(item);
                ht->buckets[index] = &DELETED_ITEM;
                return;
            }
            
        index = get_hash(k, ht->size, i);
        item = ht->buckets[index];
        i++;
    } 
}

/*
 * Calculate a^n%mod
 */
size_t power(size_t a, size_t n, size_t mod) {
    
    size_t power = a;
    size_t result = 1;
 
    while (n) {
        if (n & 1)
            result = (result * power) % mod;
        power = (power * power) % mod;
        n >>= 1;
    }
    
    return result;
}
 
/*
 * n−1 = 2^s * d with d odd by factoring powers of 2 from n−1
 */
int witness(size_t n, size_t s, size_t d, size_t a) {
    
    size_t x = power(a, d, n);
    size_t y;
 
    while (s) {
        y = (x * x) % n;
        if (y == 1 && x != 1 && x != n-1)
            return 0;
        x = y;
        --s;
    }
    
    if (y != 1)
        return 0;
    
    return 1;
}
 
/*
 * Miller-Rabin primality test
 * if n < 1,373,653, it is enough to test a = 2 and 3;
 */
 
int is_prime(size_t n) {
    
    if (((!(n & 1)) && n != 2 ) || (n < 2) || (n % 3 == 0 && n != 3))
        return 0;
    if (n <= 3)
        return 1;
 
    size_t d = n / 2;
    size_t s = 1;
    while (!(d & 1)) {
        d /= 2;
        ++s;
    }
 
    return witness(n, s, d, 2) && witness(n, s, d, 3);
}

/*
 * Find next prime number after x
 */
int next_prime(int x) {
    
    while (!is_prime(++x)); 

    return x;
}

/*
 * Resize the hash table
 */
static void ht_resize(hash_table_t* ht) {

    hash_table_t* new_ht = create_table(next_prime(2*ht->size));
    
    hash_item_t* item;
    for (int i=0; i<ht->size; i++) {
        item = ht->buckets[i];
        if (item && item != &DELETED_ITEM) 
            insert(new_ht, item->key, item->val);
    }

    ht->count = new_ht->count;

    const int tmp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = tmp_size;

    hash_item_t** tmp_buckets = ht->buckets;
    ht->buckets = new_ht->buckets;
    new_ht->buckets = tmp_buckets;

    delete_table(new_ht);
}

/*
 * Hash Table testing for Relation Table
 */
int main(int argc, char** argv) {
    
    hash_table_t* table = create_table(INITIAL_HASH_SIZE);
    delete_table(table);
    
    /*FILE* input = fopen("../Test.txt", "r");
    
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
    }*/

    return(EXIT_SUCCESS);
}

