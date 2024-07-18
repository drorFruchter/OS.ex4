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

int VMread(uint64_t virtualAddress, word_t* value){}



int VMwrite(uint64_t virtualAddress, word_t value){}
