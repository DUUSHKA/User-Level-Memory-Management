#include "my_vm.h"
#include <math.h>
#include <string.h>

struct tlb* tlb_start;
struct tlb* tlb_end;
int tlb_size;

void* physical_memory;

bool physical_memory_initialized = false; 

int offset; 
int num_bits_pde; 
int num_bits_pte;
int pde_size;
int pte_size; 
pde_t* starting_address_pde; 
pte_t* starting_address_pte;

//typedef unsigned char* bitmap_t;
bool* physical_bitmap;
bool* virutal_bitmap;
//int bitmapArray;

int hits, miss; 

//static unsigned long total_pages;

pthread_mutex_t lock; 

void init_bitmaps(){
    physical_bitmap = (bool*) calloc(MEMSIZE / PGSIZE, sizeof(bool));
    virutal_bitmap = (bool*) calloc(MAX_MEMSIZE / (unsigned long long) PGSIZE, sizeof(bool));
    physical_bitmap[0] = false;
    virutal_bitmap[0] = false; 
}
unsigned int get_offset(void * va){
    unsigned int temp = (unsigned int) (pow(2.0,offset)-1);
    return ((unsigned int)va) & temp;
}
unsigned int get_table_index(void *va){
    unsigned int temp = (unsigned int) (pow(2.0,pte_size)-1);
    return ((unsigned int)va >> offset) & temp;
}
unsigned int get_directory_index(void * va){
    unsigned int temp = (unsigned int) (pow(2.0, pde_size)-1);
    return ((unsigned int) va >> (offset+ pte_size)) & temp;
}
void* get_next_physical_address(){
    unsigned int i = 1;
    while(physical_bitmap[i] == true && i < MEMSIZE/PGSIZE){
        i++;
    }
    if(i < MEMSIZE/PGSIZE){
        physical_bitmap[i] = true;
        return (void *) (i *PGSIZE+physical_memory);
    }
    return NULL;
}
void push(struct tlb* new_tlb){
    new_tlb-> next = tlb_start;
    new_tlb -> prev = NULL;

    if(tlb_size < TLB_ENTRIES){
        tlb_size++;
    }
    if(tlb_start == NULL){
        tlb_start = new_tlb;
        tlb_end = new_tlb;
        return; 
    }

    tlb_start->prev = new_tlb;
    tlb_start = new_tlb;
    return; 
}
struct tlb* pop(){
    if(tlb_end == NULL){
        return NULL;
    }
    if(tlb_end == tlb_start){
        tlb_start = NULL;
    }else{
        tlb_end -> prev -> next = NULL;
    }
    struct tlb* popped = tlb_end;
    tlb_end = popped -> prev;

    return popped; 
}
/*
Function responsible for allocating and setting your physical memory 
*/
void set_physical_mem() {
    init_bitmaps();
    tlb_start = NULL;
    tlb_end = NULL;
    hits = 0;
    miss = 0;
    tlb_size = 0;
    if(pthread_mutex_init(&lock,NULL)!= 0){
        printf("\n error\n");
    }
    
    physical_memory = malloc(MEMSIZE);
    
    //Allocate physical memory using mmap or malloc; this is the total size of
    //your memory you are simulating
    offset = (int) log2(PGSIZE);
    int vpn =  ADDRESS_BITS - offset; 
    if(vpn % 2 != 0){
        num_bits_pde = vpn/2 - 1;
    }else{
        num_bits_pde = vpn/2;
    }
    num_bits_pte = vpn /2;
    pde_size = (int) pow(2.0,num_bits_pde);
    pte_size = (int) pow(2.0,num_bits_pte);
    
    starting_address_pde = (pde_t*) calloc(pde_size,sizeof(pde_t));
    //HINT: Also calculate the number of physical and virtual pages and allocate
    //virtual and physical bitmaps and initialize them

}


/*
 * Part 2: Add a virtual to physical page translation to the TLB.
 * Feel free to extend the function arguments or return type.
 */
int add_TLB(void *va, void *pa)
{
	if(tlb_size >= TLB_ENTRIES){
        pop();
    }
    struct tlb* new_entry = (struct tlb*)malloc(sizeof(struct tlb));
    new_entry->valid = true;
    new_entry->virutal_address = (unsigned short)((unsigned int)va / PGSIZE);
    new_entry ->physical_address = (unsigned short)((pa - physical_memory)/PGSIZE);
    push(new_entry);

   
    /*Part 2 HINT: Add a virtual to physical page translation to the TLB */


/*
 * Part 2: Check TLB for a valid translation.
 * Returns the physical page address.
 * Feel free to extend this function and change the return type.
 */
    /*Part 2 HINT: Add a virtual to physical page translation to the TLB */

    return -1;
}


/*
 * Part 2: Check TLB for a valid translation.
 * Returns the physical page address.
 * Feel free to extend this function and change the return type.
 */
pte_t * check_TLB(void *va) {
    unsigned int page = (unsigned int) va/PGSIZE;
    unsigned int offset_value = get_offset(va);
    struct tlb* ptr = tlb_start;
    while(ptr!= NULL){
        if(ptr -> virutal_address == page){
            hits++;
            return (void*)(((ptr->physical_address * PGSIZE)+physical_memory)+offset_value);
        }
        ptr = ptr->next;
    }
    miss++;
    return NULL;
    /* Part 2: TLB lookup code here */



   /*This function should return a pte_t pointer*/
}


/*
 * Part 2: Print TLB miss rate.
 * Feel free to extend the function arguments or return type.
 */

void print_TLB_missrate()
{
double miss_rate = (float) miss/ (float) (miss+hits);
    miss_rate *= 100;
    fprintf(stderr,"TLB miss rate: %.21f%%\n", miss_rate);

    /*Part 2 Code here to calculate and print the TLB miss rate*/




    fprintf(stderr, "TLB miss rate %lf \n", miss_rate);
}



/*
The function takes a virtual address and page directories starting address and
performs translation to return the physical address
*/
pte_t *translate(pde_t *pgdir, void *va) {
    /* Part 1 HINT: Get the Page directory index (1st level) Then get the
    * 2nd-level-page table index using the virtual address.  Using the page
    * directory index and page table index get the physical address.
    *
    * Part 2 HINT: Check the TLB before performing the translation. If
    * translation exists, then you can return physical address from the TLB.
    */
    int offset_value = get_offset(va);
    if(ENABLE_TLB){
        void* translated_pointer_address = check_TLB(va);
        if(translated_pointer_address!= NULL){
            return translated_pointer_address;
        }
    }

    int directory_index = get_directory_index(va);
    int table_index = get_table_index(va);
    if(pgdir[directory_index] == NULL){
        return NULL;
    }
    if(pgdir[directory_index][table_index]==NULL){
       return NULL;
    }
    void* physical_address = pgdir[directory_index][table_index];
    if(ENABLE_TLB){
        add_TLB(va,physical_address);
    }
    
    //If translation not successful, then return NULL
    return physical_address + offset_value; 
}


/*
The function takes a page directory address, virtual address, physical address
as an argument, and sets a page table entry. This function will walk the page
directory to see if there is an existing mapping for a virtual address. If the
virtual address is not present, then a new entry will be added
*/
int page_map(pde_t *pgdir, void *va, void *pa)
{

    int directory_index = get_directory_index(va);
    int table_index = get_table_index(va);
    if(pgdir[directory_index] == NULL){
        pgdir[directory_index] = calloc(pte_size,sizeof(pte_t));
    }
    if(pgdir[directory_index][table_index]==NULL){
        pgdir[directory_index][table_index]= pa;
        return 1; //Successful
    }
    /*HINT: Similar to translate(), find the page directory (1st level)
    and page table (2nd-level) indices. If no mapping exists, set the
    virtual to physical mapping */

    return -1;
}


/*Function that gets the next available page
*/
void *get_next_avail(int num_pages) {
    unsigned long long start_page;
    unsigned long long end_page;

    start_page = 1;
    end_page = num_pages + start_page; 
    bool free_block = false;   
    for(start_page =1; start_page < MAX_MEMSIZE / (unsigned long long) PGSIZE; ){
    unsigned long long i;
    for(i = start_page; i < end_page  && i < MAX_MEMSIZE;i++){
        if(virutal_bitmap[i] == true){
            start_page = i+1;
            end_page = start_page+num_pages;
            i = start_page -1; 
                continue; 
        }
        if(i == end_page - 1 && virutal_bitmap[i] == false){
            free_block = true; 
            break; 
        }
    }
    if(free_block){
        break; 
    }
    }
    if(!free_block){
        return NULL;
        //unsuccessful attempt
    }
    //Use virtual address bitmap to find the next free page
    int bitmap_index = start_page;
    while(bitmap_index < end_page){
        virutal_bitmap[bitmap_index] = true;
        bitmap_index++;
    } 
    return (void *) (start_page * PGSIZE);
}


/* Function responsible for allocating pages
and used by the benchmark
*/
void *t_malloc(unsigned int num_bytes) {
    pthread_mutex_lock(&lock);
    if(!physical_memory_initialized){
        set_physical_mem();
        physical_memory_initialized = true; 
    }
    int num_pages = (int) ceil(((float)num_bytes) / ((float) PGSIZE));
    pte_t next_page = get_next_avail(num_pages);
    if(next_page == NULL){
        pthread_mutex_unlock(&lock);
        return NULL;
    }
    pte_t current_page_address;
    unsigned int i;
    for( i = 0; i <num_pages;i++){
        current_page_address = next_page + (i*PGSIZE);
        void* physical_address = get_next_physical_address();
        page_map(starting_address_pde, current_page_address, physical_address);
    }

    /* 
     * HINT: If the physical memory is not yet initialized, then allocate and initialize.
     */

   /* 
    * HINT: If the page directory is not initialized, then initialize the
    * page directory. Next, using get_next_avail(), check if there are free pages. If
    * free pages are available, set the bitmaps and map a new page. Note, you will 
    * have to mark which physical pages are used. 
    */
    pthread_mutex_unlock(&lock);
    return next_page;
}

/* Responsible for releasing one or more memory pages using virtual address (va)
*/
void t_free(void *va, int size) {
    pthread_mutex_lock(&lock);

    for(int i = 0;i < size; i+= PGSIZE){
        if(starting_address_pde[get_directory_index(va+i)][get_table_index(va+i)] == NULL){
            pthread_mutex_unlock(&lock);
        }
    }
    for(int i = 0;i < size;i+= PGSIZE){
        void * physical_address = starting_address_pde[get_directory_index(va+i)][get_table_index(va+i)];
        unsigned long long vb_location = ((unsigned int) va + i)/PGSIZE;	

        starting_address_pde[get_directory_index(va+i)][get_table_index(va+i)] = NULL;
        virutal_bitmap[vb_location] = false;
        physical_bitmap[vb_location] = false;
    }
    pthread_mutex_unlock(&lock);
    /* Part 1: Free the page table entries starting from this virtual address
     * (va). Also mark the pages free in the bitmap. Perform free only if the 
     * memory from "va" to va+size is valid.
     *
     * Part 2: Also, remove the translation from the TLB
     */
    
}


/* The function copies data pointed by "val" to physical
 * memory pages using virtual address (va)
 * The function returns 0 if the put is successfull and -1 otherwise.
*/
int put_value(void *va, void *val, int size) {
    pthread_mutex_lock(&lock);

    pte_t physical_address;
    physical_address = translate(starting_address_pde,va);

    unsigned int offset_value = get_offset(va);

    if(offset_value + size <= PGSIZE){
        memcpy((void*)(physical_address), val, size);
        pthread_mutex_unlock(&lock);
        return -1;
    }
     memcpy((void*)(physical_address), val, PGSIZE-offset_value);
     unsigned int remaining_size = size - (PGSIZE-offset_value);
     va = va - offset_value + PGSIZE;
     val = val - offset_value + PGSIZE;
     while(remaining_size > 0){
        unsigned int copy = remaining_size;
        if(remaining_size > PGSIZE){
            copy = PGSIZE;
        }
        remaining_size -= copy;
        physical_address = translate(starting_address_pde,va);

        memcpy(physical_address,val,copy);
        va += PGSIZE;
        val += PGSIZE;
     }
     
    /* HINT: Using the virtual address and translate(), find the physical page. Copy
     * the contents of "val" to a physical page. NOTE: The "size" value can be larger 
     * than one page. Therefore, you may have to find multiple pages using translate()
     * function.
     */


    /*return -1 if put_value failed and 0 if put is successfull*/
    pthread_mutex_unlock(&lock);
    return 0;
}


/*Given a virtual address, this function copies the contents of the page to val*/
void get_value(void *va, void *val, int size) {

    pthread_mutex_lock(&lock);

    pte_t* physical_address = translate(starting_address_pde,va);
    unsigned int offset_value = get_offset(va);

    if(offset_value + size <= PGSIZE){
        memcpy(val, (void*)(physical_address),size);
        pthread_mutex_unlock(&lock);
        return;
    }
    /* HINT: put the values pointed to by "va" inside the physical memory at given
    * "val" address. Assume you can access "val" directly by derefencing them.
    */
    memcpy(val, (void*)(physical_address), PGSIZE - offset_value);
    unsigned int remaining_size = size - (PGSIZE - offset_value);
    va = va - offset_value + PGSIZE;
    val = val -offset_value + PGSIZE;
    while(remaining_size > 0){
        unsigned int copy = remaining_size;
        if(remaining_size > PGSIZE){
            copy = PGSIZE;
        }
        remaining_size -= copy; 
        physical_address = translate(starting_address_pde,va);
        memcpy(val,physical_address,copy);
        va+= PGSIZE;
        val+=PGSIZE;
    }

    pthread_mutex_unlock(&lock);
}



/*
This function receives two matrices mat1 and mat2 as an argument with size
argument representing the number of rows and columns. After performing matrix
multiplication, copy the result to answer.
*/
void mat_mult(void *mat1, void *mat2, int size, void *answer) {

    /* Hint: You will index as [i * size + j] where  "i, j" are the indices of the
     * matrix accessed. Similar to the code in test.c, you will use get_value() to
     * load each element and perform multiplication. Take a look at test.c! In addition to 
     * getting the values from two matrices, you will perform multiplication and 
     * store the result to the "answer array"
     */
    int x, y, val_size = sizeof(int);
    int i, j, k;
    for (i = 0; i < size; i++) {
        for(j = 0; j < size; j++) {
            unsigned int a, b, c = 0;
            for (k = 0; k < size; k++) {
                int address_a = (unsigned int)mat1 + ((i * size * sizeof(int))) + (k * sizeof(int));
                int address_b = (unsigned int)mat2 + ((k * size * sizeof(int))) + (j * sizeof(int));
                get_value( (void *)address_a, &a, sizeof(int));
                get_value( (void *)address_b, &b, sizeof(int));
                // printf("Values at the index: %d, %d, %d, %d, %d\n", 
                //     a, b, size, (i * size + k), (k * size + j));
                c += (a * b);
            }
            int address_c = (unsigned int)answer + ((i * size * sizeof(int))) + (j * sizeof(int));
            // printf("This is the c: %d, address: %x!\n", c, address_c);
            put_value((void *)address_c, (void *)&c, sizeof(int));
        }
    }
}

