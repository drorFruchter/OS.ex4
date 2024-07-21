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

// int isolate_first_bits(uint64_t virtualAddress)
// {
//     int len_to_move = (WORD_WIDTH / OFFSET_WIDTH);
//     len_to_move = len_to_move * OFFSET_WIDTH;
//     uint64_t ret_val =  virtualAddress >> len_to_move;
//     int delta = WORD_WIDTH % OFFSET_WIDTH;
//     return (ret_val & create_ones_of_len(delta));
//     }

uint64_t bit_set_for_level (uint64_t virtualAddress, int tree_lvl) {
    // int num_levels = (int) (WORD_WIDTH / OFFSET_WIDTH);
    int num_shifts = (TABLES_DEPTH - tree_lvl - 1) * OFFSET_WIDTH;
    // int num_shifts = (num_levels - tree_lvl) * OFFSET_WIDTH;
    uint64_t ret_val =  virtualAddress >> num_shifts;
    return (ret_val & create_ones_of_len(OFFSET_WIDTH));
}

bool is_frame_free(uint64_t address) {
    int temp_val = 0;
    for (int i = 0; i < PAGE_SIZE; i++) {
        PMread((address+i), &temp_val);
        if (temp_val != 0){
            return false;
        }
    }
    return true;
}

// /**
//  *
//  * @return -1 if it there is no free frame
//  */
// uint64_t dfs_free_frame() {
//     uint64_t cur_cell = 0;
// }

/**
 * goes over the entire tree and finds the greatest number in use
 * does it recursvly.
 * when initialize, enter adress 0 first and give it a pointer to a max val
 * that will also be 0.
 */
void max_free_idx(uint64_t address, int *max_val) {
    for (int i = 0; i< PAGE_SIZE ; i++) {
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
    if (depth == TABLES_DEPTH){return 0;}
    for (int i = 0 ; i < PAGE_SIZE; i++) {
        int optional_frame = 0;
        PMread(address+i, &optional_frame);
        if ((optional_frame!=0) && (is_frame_free(optional_frame))) {
            return optional_frame;
        }
        if (optional_frame!=0) {
            uint64_t temp = find_empty_frame(optional_frame, depth+1);
            if (temp != 0) {
                return temp;
            }
        }
    }
    // todo - not sure that it's the right place
    return 0;
}

uint64_t find_not_used_frame(uint64_t adress) {

}


uint64_t find_new_frame(uint64_t* address_to_fill) {
    uint64_t frame = 0;
    frame = find_empty_frame(0,0);
    if (frame != 0){return frame;}

}






uint64_t DFS(uint64_t virtualAddress) {
    int max_val = 0;
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
            max_free_idx(0, &max_val);
            create_new_table(cur_address, max_val);
        }
        else {
            cur_frame = cur_val;
        }
        level++;
    }
    offset = bit_set_for_level(virtualAddress, level);
    cur_address = (cur_frame * OFFSET_WIDTH) + offset;
    return cur_address;
}












void VMinitialize()
{
    create_new_table(0, 0);
}

int VMread(uint64_t virtualAddress, word_t* value)
{
    uint64_t cur_address = DFS(virtualAddress);
    PMread(cur_address, value);
}

int VMwrite(uint64_t virtualAddress, word_t value) {
    uint64_t cur_address = DFS(virtualAddress);
    PMwrite(cur_address, value);
}
