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

int isolate_first_bits(uint64_t virtualAddress) {
    int len_to_move =(int) (WORD_WIDTH / OFFSET_WIDTH);
    len_to_move = len_to_move * OFFSET_WIDTH;
    uint64_t ret_val =  virtualAddress >> len_to_move;
    int delta = WORD_WIDTH % OFFSET_WIDTH;
    uint64_t delta_bits = (1ULL << delta) - 1;
    return ret_val & delta_bits;
    }

int VMread(uint64_t virtualAddress, word_t* value) {

}



int VMwrite(uint64_t virtualAddress, word_t value) {

}
