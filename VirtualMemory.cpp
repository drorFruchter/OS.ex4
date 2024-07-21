#include "VirtualMemory.h"
#include "PhysicalMemory.h"


int min_func (int x, int y)
{ return x > y ? y : x; }

void create_new_table(uint64_t physicalAddress, int frame)
{
    if (frame != 0)
    {
        PMwrite(physicalAddress, frame);
    }
    for (long long address = frame * PAGE_SIZE; address < (frame + 1) * PAGE_SIZE; address++)
    {
        PMwrite(address, 0);
    }
}


word_t cyclic_calculation(uint64_t cur_page, uint64_t new_page){
    int temp_val = (int) (cur_page - new_page);
    if (temp_val < 0){temp_val = -1 * temp_val;}
    word_t cur_val = (word_t) min_func (NUM_PAGES - temp_val, temp_val);
    return cur_val
}


uint64_t DFS(uint64_t cur_frame,
             uint64_t* previous_address,
             int* max_frame_num,
             uint64_t* max_cyclic_value,
             uint64_t* page_to_evict,
             uint64_t* frame_to_evict,
             uin64_t* address_to_remove,
             uint64_t target_page,
             uint64_t current_page,
             int depth,
            ) 
{
    bool is_empty_table = true;
    word_t next_frame = 0;
    uint64_t current_starting_page = current_page;
    for (int i = 0; i < PAGE_SIZE; i++) {
        auto address_of_next_frame = cur_frame * PAGE_SIZE + i;
        current_page = current_starting_page * PAGE_SIZE + i;
        PMread(address_of_next_frame, &next_frame);
        if (next_frame != 0) {
            is_empty_table = false;
            if (*max_frame_num < next_frame) { //option 2
                *max_frame_num = next_frame;
            }
            if (depth == TABLES_DEPTH - 1) { // option 3 - evict page from frame
                word_t currentCalc = cyclic_calculation(current_page, target_page);
                if (*max_cyclic_value < currentCalc) {
                    *max_cyclic_value = currentCalc;
                    *frame_to_evict = cur_frame;
                    *page_to_evict = (word_t) current_page;
                    *addres_to_remove = address_of_next_frame;
                }
            }
            else {
                //DFS - keep travrsing the tree
            }
        }
    }
    if (is_empty_table) { // option 1
        // make sure to delete from previous table
        return cur_frame;
    }
}


void VMinitialize()
{
    create_new_table(0, 0);
}

int VMread(uint64_t virtualAddress, word_t* value)
{
   
}

int VMwrite(uint64_t virtualAddress, word_t value) 
{
    
}
