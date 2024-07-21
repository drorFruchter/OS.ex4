#include "VirtualMemory.h"

#include <float.h>

#include "PhysicalMemory.h"
#include "MemoryConstants.h"

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

uint64_t create_ones_of_len (int len) {
    return (1ULL << len) - 1;
}

uint64_t offset_for_level (uint64_t virtualAddress, int tree_lvl) {
    int num_shifts = (TABLES_DEPTH - tree_lvl - 1) * OFFSET_WIDTH;
    uint64_t ret_val =  virtualAddress >> num_shifts;
    return (ret_val & create_ones_of_len(OFFSET_WIDTH));
}

bool is_frame_free(uint64_t address) {
    int temp_val = 0;
    for (int i = 0; i < PAGE_SIZE; i++) {
        PMread(address + i, &temp_val);
        if (temp_val != 0){
            return false;
        }
    }
    return true;
}
    

/**
 * goes over the entire tree and finds the greatest number in use
 * does it recursvly.
 * when initialize, enter adress 0 first and give it a pointer to a max val
 * that will also be 0.
 */
void max_free_idx(uint64_t address, int *max_val) {
    for (int i = 0; i < PAGE_SIZE ; i++) {
        int cur_val = 0;
        VMread(address + i, &cur_val);
        if (*max_val < cur_val){
            *max_val = cur_val;
            max_free_idx (address + i, max_val);
        }
    }
}


uint64_t find_empty_frame(uint64_t address, int depth) {
    // todo - how do we know we've reached a leaf?
    if (depth == TABLES_DEPTH) {
        return 0;
    }
    for (int i = 0 ; i < PAGE_SIZE; i++) {
        int optional_frame = 0;
        PMread(address + i, &optional_frame);
        if ((optional_frame != 0) && (is_frame_free(optional_frame))) {
            return optional_frame;
        }
        if (optional_frame != 0) {
            uint64_t temp = find_empty_frame(optional_frame, depth+1);
            if (temp != 0) {
                return temp;
            }
        }
    }
    // todo - not sure that it's the right place
    return 0;
}

uint64_t choose_frame_evict(uint64_t address){
    
}


uint64_t find_new_frame(uint64_t parent_frame) {
    uint64_t frame = 0;
    frame = find_empty_frame(0,0);
    if (frame != 0){return frame;}
    max_free_idx(0, &frame);
    if (frame < NUM_FRAMES) {return frame;}
    
}

uint64_t DFS_find_frame(uint64_t current_address,
             uint64_t previous_address,
             int* depth,
             int* max_frame_num,
             int* current_frame
             ) 
{
    bool empty_flag = true;
    for (int i = 0 ; i < PAGE_SIZE; i++){
        word_t cur_val=0;
        PMread(current_address * PAGE_SIZE + i, &cur_val);
        // if we have an empty table - option 1
        if (cur_val != 0){empty_flag = false;}
        
        // update max value - option 2
        if (*max_frame_num < cur_val){*max_frame_num = cur_val;}
        // cyclic removel - option 3
        if (*depth == TABLES_DEPTH - 1)
        {
            continue;
        }
        else
        {
            previous_address = current_address * PAGE_SIZE + i;
            DFS_find_frame();
        }
         
    }
    if (empty_flag){}
         
    
}
    


uint64_t DFS(uint64_t virtualAddress) {
    int level = 0;
    int cur_frame = 0;
    uint64_t offset = 0;
    uint64_t cur_address = 0;
    while(level < TABLES_DEPTH - 1) {
        int cur_val;
        offset = bit_set_for_level(virtualAddress, level);
        cur_address = (cur_frame * OFFSET_WIDTH) + offset;
        PMread(cur_address, &cur_val);
        if (cur_val == 0) {
            int new_frame = find_new_frame();
            create_new_table(cur_address, new_frame);
            cur_frame = new_frame;
        }
        else {
            cur_frame = cur_val;
        }
        level++;
    }
    offset = bit_set_for_level(virtualAddress, level);
    cur_address = (cur_frame * OFFSET_WIDTH) + offset;
    PMrestore(cur_frame, cur_val);
    return cur_address;
}

void VMinitialize()
{
    create_new_table(0, 0);
}

uint64_t getFinalAddress(uint64_t virtualAddress) {
    uint64_t previous_frame = 0;
    word_t current_value = 0;
    for (int i = 0; i < TABLES_DEPTH - 1; i++) {
        uint64_t next_frame_address = previous_frame * OFFSET_WIDTH + offest_for_level(virtualAddress, i);
        PMread(next_frame_address, &current_value);
        if (current_value == 0) {
            uint64_t new_frame = find_new_frame(previous_frame);
            create_new_table(next_frame_address, new_frame);
            previous_frame = new_frame;
        }
        else {
            previous_frame = current_value;
        }
    }
    PMread(previous_frame * OFFSET_WIDTH + offest_for_level(virtualAddress, TABLES_DEPTH), &current_value);
    if (current_value == 0) {
          //uint64_t frameNum = restorePage (pageNumber, previousAddress);
          //PMwrite (previousAddress * PAGE_SIZE + translatedAddress[1], (word_t) frameNum);
          //previousAddress = frameNum;
        }
    else {
        previous_frame = current_value;
        }
    return previous_frame;
}

int VMread(uint64_t virtualAddress, word_t* value)
{
    uint64_t physicalAddress = getFinalAddress(virtualAddress);
}

int VMwrite(uint64_t virtualAddress, word_t value) {
    uint64_t cur_address = DFS(virtualAddress);
    PMwrite(cur_address, value);
}
