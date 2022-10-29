/*
 * CS3375 Computer Architecture
 * Course Project
 * Cache Simulator Design and Development
 * FALL 2017
 * By Yong Chen
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "cachesim.h"
#include<stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: %s <direct> <trace file name>\n", argv[0]);
        return 1;
    }
   
#ifdef DBG
    printf("BLOCK SIZE = %d Bytes\n", BLOCK_SIZE);
    printf("%d-WAY\n", WAY_SIZE);
    printf("CACHE SIZE = %d Bytes\n", CACHE_SIZE);
    printf("NUMBER OF BLOCKS = %d\n", NUM_BLOCKS);
    printf("NUMBER OF SETS = %d\n", NUM_SETS);
    printf("\n");
#endif

    char* trace_file_name = argv[2];
    struct direct_mapped_cache d_cache;
    // struct kway_mapped_cache k_cache;
    char mem_request[20];
    uint64_t address;
    FILE *fp;
    kway_mapped_cache k_cache;

    /* Initialization */
    for (int i=0; i<NUM_BLOCKS; i++) {
        d_cache.valid_field[i] = 0;
        d_cache.dirty_field[i] = 0;
        d_cache.tag_field[i] = 0;
    }
    d_cache.hits = 0;
    d_cache.misses = 0;
    d_cache.hit_rate =0.0;                 
    d_cache.miss_rate=0.0; 
    k_cache.hits=0;
    k_cache.misses =0;
    k_cache.hit_rate=0.0;
    k_cache.miss_rate =0.0;


    /* Opening the memory trace file */
    fp = fopen(trace_file_name, "r");
    if(fp == NULL)
    {
        printf("file is empty \n");
    }

    if(strncmp(argv[1], "direct", 6)==0){
          /* Simulating direct-mapped cache */
            printf("in direct \n");
            way_size =1;
            num_sets = NUM_BLOCKS;
            while (fgets(mem_request, 20, fp)!= NULL) {
                address = convert_address(mem_request);
                direct_mapped_cache_access(&d_cache, address);
            }
            fclose(fp);
            print_cache_details(num_sets,way_size,&d_cache,argv[1]);
    }
    else if(strncmp(argv[1], "fully", 5)==0){
           /* Simulating fully associative cache */
            printf("in fully\n");
            way_size = NUM_BLOCKS;
            num_sets = (NUM_BLOCKS/way_size);
            allocate_kway_cache(&k_cache,num_sets,way_size);
             while (fgets(mem_request, 20, fp)!= NULL) {
                address = convert_address(mem_request);
                kway_associative_cache_access(&k_cache,address,num_sets,way_size);
            }
            fclose(fp);
            print_cache_details(num_sets,way_size,&d_cache,argv[1]);
            free_kway_cache(k_cache,num_sets,way_size);
    }
    else{ 
        if((strncmp(argv[1], "2way", 4)==0) || (strncmp(argv[1], "4way", 4)==0) || (strncmp(argv[1], "8way", 4)==0))
        { 
            /* Simulating k_way associative cache */
            printf("in kway \n");
            // way_size = (*argv)[1]+0;
            // k_way_associative_cache(mem_request,&d_cache,address,way_size);
        
        }  
    }   
    
    return 0;
}

void print_cache_details(int num_sets,int way_size,struct direct_mapped_cache *cache,char* argv ){

    printf("\n");
    printf("in print cache details \n");
    printf("BLOCK SIZE = %d Bytes\n", BLOCK_SIZE);
    printf("WAY SIZE = %d \n", way_size);
    printf("CACHE SIZE = %d Bytes\n", CACHE_SIZE);
    printf("NUMBER OF BLOCKS = %d\n", NUM_BLOCKS);
    printf("NUMBER OF SETS = %d\n", num_sets);
    cache->hit_rate = (double) cache->hits/(cache->hits + cache->misses);
    cache->miss_rate = (double) cache->misses/(cache->hits + cache->misses);
    printf("\n==================================\n");
    printf("Cache type:    %s-Mapped Cache\n", argv);
    printf("==================================\n");
    printf("Cache Hits:    %d\n", cache->hits);
    printf("Cache Misses:  %d\n", cache->misses);
    printf("Cache Hit rate %.2f\n", cache->hit_rate);
    printf("Cache Miss rate %.2f\n", cache->miss_rate);
    printf("\n");
}

uint64_t convert_address(char memory_addr[])
/* Converts the physical 32-bit address in the trace file to the "binary" \\
 * (a uint64 that can have bitwise operations on it) */
{
    printf("in convert address \n");
    uint64_t binary = 0;
    int i = 0;

    while (memory_addr[i] != '\n') {
        if (memory_addr[i] <= '9' && memory_addr[i] >= '0') {
            binary = (binary*16) + (memory_addr[i] - '0'); 
        } else {
            if(memory_addr[i] == 'a' || memory_addr[i] == 'A') {
                binary = (binary*16) + 10;
            }
            if(memory_addr[i] == 'b' || memory_addr[i] == 'B') {
                binary = (binary*16) + 11;
            }
            if(memory_addr[i] == 'c' || memory_addr[i] == 'C') {
                binary = (binary*16) + 12;
            }
            if(memory_addr[i] == 'd' || memory_addr[i] == 'D') {
                binary = (binary*16) + 13;
            }
            if(memory_addr[i] == 'e' || memory_addr[i] == 'E') {
                binary = (binary*16) + 14;
            }
            if(memory_addr[i] == 'f' || memory_addr[i] == 'F') {
                binary = (binary*16) + 15;
            }
        }
        i++;
    }
    // printf("%s converted to %llu\n", memory_addr, binary);

#ifdef DBG
    printf("%s converted to %llu\n", memory_addr, binary);
#endif
    return binary;
}

void direct_mapped_cache_access(struct direct_mapped_cache *cache, uint64_t address)
{
     /* Read the memory request address and access the cache */
       
    uint64_t block_addr = address >> (unsigned)log2(BLOCK_SIZE);
    uint64_t index = block_addr % NUM_BLOCKS;
    uint64_t tag = block_addr >> (unsigned)log2(NUM_BLOCKS);
    // printf("Memory address: %llu, Block address: %llu, Index: %llu, Tag: %llu \n", address, block_addr, index, tag);

#ifdef DBG
    printf("Memory address: %llu, Block address: %llu, Index: %llu, Tag: %llu ", address, block_addr, index, tag);
#endif

    if (cache->valid_field[index] && cache->tag_field[index] == tag) { /* Cache hit */
        cache->hits += 1;
#ifdef DBG
        printf("Hit!\n");
#endif
    } else {
        /* Cache miss */
        cache->misses += 1;
#ifdef DBG
        printf("Miss!\n");
#endif
        if (cache->valid_field[index] && cache->dirty_field[index]) {
            /* Write the cache block back to memory */
        }
        cache->tag_field[index] = tag;
        cache->valid_field[index] = 1;
        cache->dirty_field[index] = 0;
    }
}

void allocate_kway_cache(kway_mapped_cache k_cache,int num_sets,int way_size)
{
    printf("in allocate memories \n");
    printf("kway is %d", way_size);
    printf("num sets %d", num_sets);
    k_cache.sets = (Set*) malloc(num_sets * sizeof(Set));
    for(int n=0; n< num_sets; n++)
    {
        k_cache.sets[n].blocks = (Block*)malloc(way_size*sizeof(Block));
    }
    for(int i=0; i<num_sets;i++)
    {
        for(int j=0;j<way_size;j++)
        {
            k_cache.sets[i].blocks[j].valid_field =0;
            k_cache.sets[i].blocks[j].tag_field =0;
            k_cache.sets[i].blocks[j].dirty_field=0;
        }
    }
     
}

void kway_associative_cache_access(kway_mapped_cache cache, uint64_t address, int num_sets,int way_size)
{
    printf("in kway mapping");
    uint64_t block_addr = address >> (unsigned)log2(BLOCK_SIZE);
    uint64_t set_num = block_addr % num_sets;
    uint64_t tag = block_addr >> (unsigned)log2(num_sets);
    int random =0;
     printf("Memory address: %llu, Block address: %llu, set_num: %llu, Tag: %llu ", address, block_addr, set_num, tag);

#ifdef DBG
    printf("Memory address: %llu, Block address: %llu, Index: %llu, Tag: %llu ", address, block_addr, index, tag);
#endif
    for(int n=0; n<way_size;n++)
    {
        printf("in for");
        printf("valid is %llu \n",cache.sets[set_num].blocks[n].valid_field);
        printf("tag is %llu\n",cache.sets[set_num].blocks[n].tag_field);
        printf("dirty is %llu\n",cache.sets[set_num].blocks[n].dirty_field);
        if(cache.sets[set_num].blocks[n].valid_field ==0)
        {
            printf("Memory address: %llu, Block address: %llu, set_num: %llu, Tag: %llu ", address, block_addr, set_num, tag);
            cache.sets[set_num].blocks[n].tag_field = tag;
            cache.sets[set_num].blocks[n].valid_field = 1;
            cache.sets[set_num].blocks[n].dirty_field = 0;
            cache.misses += 1;
            break;
        }
    else if (cache.sets[set_num].blocks[n].valid_field ==1 && cache.sets[set_num].blocks[n].tag_field == tag)
    { /* Cache hit */
        printf("Memory address: %llu, Block address: %llu, set_num: %llu, Tag: %llu ", address, block_addr, set_num, tag);
         cache.hits += 1;
         break;
    }
#ifdef DBG
        printf("Hit!\n");
#endif
    else if(n ==(way_size -1))
    {
        printf("Memory address: %llu, Block address: %llu, set_num: %llu, Tag: %llu ", address, block_addr, set_num, tag);
       random = rand()%(num_sets);
       cache.sets[set_num].blocks[random].tag_field = tag;
       cache.sets[set_num].blocks[random].valid_field =1;
       cache.sets[set_num].blocks[random].dirty_field =0;
       cache.misses += 1;
#ifdef DBG
        printf("Miss!\n");
#endif
    }
}
}

void free_kway_cache(kway_mapped_cache k_cache,int num_sets,int way_size)
{
        for(int a=0; a<num_sets;a++)
        {
            free(k_cache.sets[a].blocks);
        }
        free(k_cache.sets);
}
