/*
 * CS3375 Computer Architecture
 * Course Project
 * Cache Simulator Design and Development
 * FALL 2017
 * By Yong Chen
 */

#define BLOCK_SIZE  64        /* Cache block size (or cache line size) in bytes \\
                                 (must be power of 2). 4 Bytes = 1 Word */
                                  
#define CACHE_SIZE  32768     /* Cache capacity in bytes (must be power of 2)*/

#define NUM_BLOCKS  (CACHE_SIZE / BLOCK_SIZE)
// #define 

//#define DBG
int way_size =0;  /* Associativity; 1-way = direct-mapped */
int num_sets=0;

/*The data structure of direct-mapped cache*/
struct direct_mapped_cache {
    unsigned valid_field[NUM_BLOCKS];   /* Valid field */
    unsigned dirty_field[NUM_BLOCKS];   /* Dirty field; since we don't distinguish writes and \\
                                           reads in this project yet, this field doesn't really matter */
    uint64_t tag_field[NUM_BLOCKS];     /* Tag field */
    char data_field[NUM_BLOCKS][BLOCK_SIZE];  /* Data field; since we don't really fetch data, \\
                                                 this field doesn't really matter */
    int hits;                          /* Hit count */
    int misses;                         /* Miss count */
    double hit_rate;
    double miss_rate;
};

typedef struct {
    uint64_t valid_field;
    uint64_t tag_field;
    // char data_field[NUM_BLOCKS][BLOCK_SIZE];
    uint64_t dirty_field;
}Block;
typedef struct {
   Block* blocks;
}Set;
typedef struct{
    Set* sets;
    int hits;                          /* Hit count */
    int misses;                         /* Miss count */
    double hit_rate;
    double miss_rate;
}kway_mapped_cache;


/*Read the memory traces and convert it to binary*/
uint64_t convert_address(char memory[]);

/*Simulate the direct-mapped cache*/
void direct_mapped_cache_access(struct direct_mapped_cache *cache, uint64_t address);
void print_direct_cache_det(int num_sets,int way_size,struct direct_mapped_cache *cache,char* argv);
void print_kway_cache_det(int num_sets,int way_size,kway_mapped_cache *cache,char* argv);
void allocate_kway_cache(kway_mapped_cache *k_cache, int num_sets,int way_size);
void kway_associative_cache_access(kway_mapped_cache *cache, uint64_t address, int num_sets,int way_size);
void free_kway_cache(kway_mapped_cache k_cache,int num_sets,int way_size);

