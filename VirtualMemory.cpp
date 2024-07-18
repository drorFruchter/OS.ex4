#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include "MemoryConstants.h"

void VMinitialize()
{
    for (int i=0; i<PAGE_SIZE;i++)
    {
        PMwrite(i, 0);
    }
}

uint64_t create_ones_of_len (int len) {
    return (1ULL << len) - 1;
}

uint64_t isolate_first_bits(uint64_t virtualAddress) {
    int len_to_move =(int) (WORD_WIDTH / OFFSET_WIDTH);
    len_to_move = len_to_move * OFFSET_WIDTH;
    uint64_t ret_val =  virtualAddress >> len_to_move;
    int delta = WORD_WIDTH % OFFSET_WIDTH;
    return (ret_val & create_ones_of_len(delta));
    }

uint64_t bit_set_for_level (uint64_t virtualAddress, int tree_lvl) {
    int num_levels = (int) (WORD_WIDTH / OFFSET_WIDTH);
    int num_shifts = (num_levels - tree_lvl) * OFFSET_WIDTH;
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

/**
 * 
 * @return -1 if it there is no free frame
 */
uint64_t dfs_free_frame() {
    uint64_t cur_cell = 0;

}

int VMread(uint64_t virtualAddress, word_t* value) {

}



int VMwrite(uint64_t virtualAddress, word_t value) {

}
