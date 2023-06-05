#ifndef MY_VM_H_INCLUDED
#define MY_VM_H_INCLUDED
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

//Assume the address space is 32 bits, so the max memory size is 4GB
//Page size is 4KB

//Add any important includes here which you may need
#define ENABLE_TLB true 

#define PGSIZE 4096


// Maximum size of virtual memory
#define MAX_MEMSIZE 4ULL*1024*1024*1024

// Size of "physcial memory"    
#define MEMSIZE 1024*1024*1024

#define ADDRESS_BITS 32 

// Represents a page table entry
typedef void* pte_t;

// Represents a page directory entry
typedef pte_t* pde_t;

#define TLB_ENTRIES 512

//Structure to represents TLB
struct tlb {
    /*Assume your TLB is a direct mapped TLB with number of entries as TLB_ENTRIES
    * Think about the size of each TLB entry that performs virtual to physical
    * address translation.
    */
    unsigned short virutal_address;
    unsigned short physical_address; 
    bool valid;
    struct tlb *next, *prev;

};
struct tlb tlb_store;


void set_physical_mem();
pte_t* translate(pde_t *pgdir, void *va);
int page_map(pde_t *pgdir, void *va, void* pa);
bool check_in_tlb(void *va);
void put_in_tlb(void *va, void *pa);
void *t_malloc(unsigned int num_bytes);
void t_free(void *va, int size);
int put_value(void *va, void *val, int size);
void get_value(void *va, void *val, int size);
void mat_mult(void *mat1, void *mat2, int size, void *answer);
void print_TLB_missrate();
void init_bitmaps();
unsigned int get_offset(void * va);
unsigned int get_table_index(void * va);
unsigned int get_directroy_index(void * va);
struct tlb* pop();
void push(struct tlb* translation_table);

#endif
