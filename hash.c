#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <string.h>

// Function prototypes (forward declarations)
struct Table* NewTable(size_t capacity);
void freeTable(struct Table *table);
size_t hash(const char* key, int tableSize);
void resize_table(struct Table *table);
char* insert_key(struct Table *table, const char* key, void* value);
char* insert(struct Table *table, const char* key, void* value);
void* get(const struct Table* table, const char* key);


#define INITIAL_CAPACITY 16 
#define FNV_prime 16777619
#define FNV_offset_basis 2166136261U
#define LOAD_FACTOR_THRESHOLD 0.75  


/**
 * PLAN
 *  Implentation steps extend the functionality as you go
 *  - key is int and val is int
 *  - key is int and val is  anything
 *  - key should be anything(supported values) and val should be anything
 *   */

 /**
  * have an entriesay key is the index and value is the entriesay value at the index;
  */
 typedef struct Entry
{
    /* data */
    char * key; //key of the string type 
    void* value; //the value can be of any type
}Entry;
 
  typedef struct Table
 {
    /**
     * capacity/size = load ratio
     * after load ration exceeds a certain limit expand the array
     * */ 
    
    Entry *entries ;
    size_t size;
    size_t capacity;

 }Table ;
 struct Table* NewTable(size_t capacity){
   struct Table* table =(struct Table*)malloc(sizeof(struct Table));
   if(table==NULL){
      return NULL;
   }
   table->capacity = capacity;
   table->size = 0;

   table->entries = (Entry*)calloc(capacity, sizeof(Entry));
   if(table->entries==NULL){
      free(table);
      return NULL;
   }
   
   return table;
 }
 void freeTable(struct Table *table){
   //here i have a doubt on why value of Entry is not freed up
   if(table!=NULL){
      for(int i = 0;i<table->capacity;i++){
         free((void*)table->entries[i].key);
      }
   free(table->entries);
   free(table);   
   }
 }
 size_t hash(const char* key,int tableSize){
   if(key==NULL) return 0;
   size_t hash_key = FNV_offset_basis;
   for(int i = 0;key[i]!='\0';i++){
      hash_key=hash_key * FNV_prime;
      hash_key = hash_key ^ key[i];
   }
   return hash_key%tableSize;
 }
 
 // Internal function for inserting without resize check (used during rehashing)
 char* insert_key(struct Table *table, const char* key, void* value){
   if(table==NULL){
      return NULL;
   }
   
   size_t index = hash(key,table->capacity);

   while(table->entries[index].key!=NULL){
      if(strcmp(table->entries[index].key,key)==0){
         table->entries[index].value = value;
         return table->entries[index].key;
      }
      index++;//linear probing may be change t quadratic probing
      if(index>=table->capacity){
         index= 0;
      }
   }

   char *c  = strdup(key);
   table->entries[index].key = c;
   table->entries[index].value = value;
   table->size++;
   return table->entries[index].key;
 }

 char* insert(struct Table *table, const char* key, void* value){
  //handle collision seperately
   if(table==NULL){
      return NULL;
   }
   
   // Check load factor and resize if needed
   if((double)table->size / table->capacity >= LOAD_FACTOR_THRESHOLD){
        resize_table(table);
   }
   
   // Use insert_key for the actual insertion
   return insert_key(table, key, value);
 }
 void* get(const struct Table* table,const char* key){
   if(table==NULL){
      return NULL;//cannot return with -1 should return something else;
   }
   // while()
   size_t index = hash(key,table->capacity);
   while(table->entries[index].key!=NULL){
      if(strcmp(table->entries[index].key,key)==0){
         return table->entries[index].value;
      }
      index++;
      if(index>=table->capacity){
         index=0;
      }
   }

return NULL;
 }
 void resize_table(Table *table){
    if (table == NULL) return;
    
    // Save old data
    Entry *old_entries = table->entries;
    size_t old_capacity = table->capacity;
    
    // Update table capacity and reset size
    table->capacity *= 2;
    table->size = 0;  // Reset size, will be recalculated during rehashing
    
    // Allocate new entries array
    table->entries = (Entry*)calloc(table->capacity, sizeof(Entry));
    if (table->entries == NULL) {
        // Restore on failure
        table->entries = old_entries;
        table->capacity = old_capacity;
        return;
    }
    
    // Rehash all existing entries into new table using insert_key
    for (size_t i = 0; i < old_capacity; i++) {
        if (old_entries[i].key != NULL) {
            // Use insert_key to avoid triggering another resize
            insert_key(table, old_entries[i].key, old_entries[i].value);
            // Don't free the key here - insert_key will strdup it
        }
    }
    
    // Free old keys and entries array
    for (size_t i = 0; i < old_capacity; i++) {
        if (old_entries[i].key != NULL) {
            free(old_entries[i].key);
        }
    }
    free(old_entries);
 }

int main(){
    printf("=== Hash Table Testing ===\n\n");
    
    // Test 1: Basic functionality
    printf("Test 1: Basic Insert and Get\n");
    struct Table* table = NewTable(4);  // Small capacity to force expansion
    printf("Initial capacity: %zu, size: %zu\n", table->capacity, table->size);
    
    int value1 = 10, value2 = 20, value3 = 30;
    insert(table, "key1", &value1);
    insert(table, "key2", &value2);
    insert(table, "key3", &value3);
    
    int* result1 = (int*)get(table, "key1");
    int* result2 = (int*)get(table, "key2");
    int* result3 = (int*)get(table, "key3");
    
    printf("key1: %d, key2: %d, key3: %d\n", 
           result1 ? *result1 : -1, 
           result2 ? *result2 : -1, 
           result3 ? *result3 : -1);
    printf("After 3 inserts - capacity: %zu, size: %zu\n\n", table->capacity, table->size);
    
    // Test 2: Force expansion by exceeding load factor
    printf("Test 2: Hash Table Expansion\n");
    printf("Load factor threshold: %.2f\n", LOAD_FACTOR_THRESHOLD);
    
    // Add more items to trigger expansion
    int values[10] = {40, 50, 60, 70, 80, 90, 100, 110, 120, 130};
    char keys[10][20];
    
    for (int i = 0; i < 10; i++) {
        sprintf(keys[i], "expand_key_%d", i);
        printf("Inserting %s (value: %d) - ", keys[i], values[i]);
        printf("Before: capacity=%zu, size=%zu, load=%.2f", 
               table->capacity, table->size, 
               (double)table->size / table->capacity);
        
        insert(table, keys[i], &values[i]);
        
        printf(" | After: capacity=%zu, size=%zu, load=%.2f\n", 
               table->capacity, table->size, 
               (double)table->size / table->capacity);
    }
    
    // Verify all values are still accessible after expansion
    printf("\nVerifying all values after expansion:\n");
    for (int i = 0; i < 10; i++) {
        int* result = (int*)get(table, keys[i]);
        printf("%s: %s\n", keys[i], result ? (sprintf(keys[0], "%d", *result), keys[0]) : "NOT FOUND");
    }
    
    // Test 3: Hash collision testing
    printf("\nTest 3: Hash Collision Testing\n");
    
    // Create keys that are likely to collide
    const char* collision_keys[] = {
        "abc", "acb", "bac", "bca", "cab", "cba",  // Similar characters
        "test1", "test2", "test3", "test4",        // Similar prefixes
        "a", "aa", "aaa", "aaaa"                   // Similar but different lengths
    };
    int collision_values[] = {100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113};
    int num_collision_tests = sizeof(collision_keys) / sizeof(collision_keys[0]);
    
    struct Table* collision_table = NewTable(8);  // Small table to increase collision chance
    
    printf("Testing %d keys in table with capacity %zu:\n", num_collision_tests, collision_table->capacity);
    
    // Insert all collision test keys
    for (int i = 0; i < num_collision_tests; i++) {
        size_t hash_value = hash(collision_keys[i], collision_table->capacity);
        printf("Key: %-8s | Hash: %zu | Index: %zu | ", 
               collision_keys[i], hash_value, hash_value % collision_table->capacity);
        
        insert(collision_table, collision_keys[i], &collision_values[i]);
        printf("Inserted successfully\n");
    }
    
    printf("\nCollision table final state - capacity: %zu, size: %zu\n", 
           collision_table->capacity, collision_table->size);
    
    // Verify all collision keys can be retrieved
    printf("\nRetrieving collision test keys:\n");
    for (int i = 0; i < num_collision_tests; i++) {
        int* result = (int*)get(collision_table, collision_keys[i]);
        printf("Key: %-8s | Expected: %3d | Got: %s\n", 
               collision_keys[i], collision_values[i], 
               result ? (sprintf(keys[0], "%d", *result), keys[0]) : "NOT FOUND");
    }
    
    // Test 4: Stress test - many insertions
    printf("\nTest 4: Stress Test\n");
    struct Table* stress_table = NewTable(2);  // Very small initial capacity
    printf("Starting stress test with initial capacity: %zu\n", stress_table->capacity);
    
    const int STRESS_COUNT = 50;
    int stress_values[STRESS_COUNT];
    char stress_keys[STRESS_COUNT][20];
    
    for (int i = 0; i < STRESS_COUNT; i++) {
        stress_values[i] = i * 10;
        sprintf(stress_keys[i], "stress_%02d", i);
        insert(stress_table, stress_keys[i], &stress_values[i]);
        
        if (i % 10 == 9) {  // Print every 10 insertions
            printf("Inserted %d items - capacity: %zu, size: %zu, load: %.2f\n", 
                   i + 1, stress_table->capacity, stress_table->size,
                   (double)stress_table->size / stress_table->capacity);
        }
    }
    
    // Verify random keys from stress test
    printf("\nVerifying random keys from stress test:\n");
    int test_indices[] = {0, 15, 25, 35, 49};
    for (int i = 0; i < 5; i++) {
        int idx = test_indices[i];
        int* result = (int*)get(stress_table, stress_keys[idx]);
        printf("Key: %s | Expected: %d | Got: %s\n", 
               stress_keys[idx], stress_values[idx],
               result ? (sprintf(keys[0], "%d", *result), keys[0]) : "NOT FOUND");
    }
    
    // Test 5: Update existing keys
    printf("\nTest 5: Update Existing Keys\n");
    int new_value = 999;
    printf("Updating 'key1' from %d to %d\n", value1, new_value);
    insert(table, "key1", &new_value);
    
    int* updated_result = (int*)get(table, "key1");
    printf("Updated value: %s\n", updated_result ? (sprintf(keys[0], "%d", *updated_result), keys[0]) : "NOT FOUND");
    
    // Cleanup
    printf("\nCleaning up...\n");
    freeTable(table);
    freeTable(collision_table);
    freeTable(stress_table);
    
    printf("All tests completed!\n");
    return 0;
}

