#include "VirtualMemory.h"
#include "PhysicalMemory.h"

uint64_t x_power_y (uint64_t x, uint64_t y)
{
    uint64_t original_x = x;
      for (int i = 0; i < (int) y - 1; ++i)
        {
          x *= original_x;
        }
      return x;
}

int min_func (int x, int y)
{ 
    return x > y ? y : x; 
}

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


word_t cyclic_calculation(uint64_t cur_page, uint64_t new_page)
{
    int temp_val = (int) (cur_page - new_page);
    if (temp_val < 0) 
    {
        temp_val = -1 * temp_val;
    }
    word_t cur_val = (word_t) min_func (NUM_PAGES - temp_val, temp_val);
    return cur_val;
}


void DFS(uint64_t cur_frame,
             uint64_t* max_frame_num,
             word_t* max_cyclic_value,
             word_t* page_to_evict,
             word_t* frame_to_evict,
             uint64_t* address_to_remove,
             uint64_t* empty_table,
             uint64_t target_page,
             uint64_t current_page,
             uint64_t* parent_table,
             uint64_t* final_parent_table,
             uint64_t current_working_tbale,
             int depth
            )
{
    bool is_empty_table = true;
    word_t next_frame = 0;
    uint64_t current_starting_page = current_page;
    for (int i = 0; i < PAGE_SIZE; i++) 
    {
        auto address_of_next_frame = cur_frame * PAGE_SIZE + i;
        current_page = current_starting_page * PAGE_SIZE + i;
        PMread(address_of_next_frame, &next_frame);
        if (next_frame != 0) {
            is_empty_table = false;
            if (*max_frame_num < next_frame) { //option 2
                *max_frame_num = next_frame;
            }
            if (depth == TABLES_DEPTH - 1) { // option 3 - evict page from frame
                word_t current_calc = cyclic_calculation(current_page, target_page);
                if (*max_cyclic_value < current_calc) {
                    *max_cyclic_value = current_calc;
                    *frame_to_evict = (word_t) next_frame;
                    *page_to_evict = (word_t) current_page;
                    *address_to_remove = address_of_next_frame;
                }
            }
            else {
                *parent_table = address_of_next_frame;
                //DFS - keep travrsing the tree
                DFS(next_frame,
                    max_frame_num,
                    max_cyclic_value,
                    page_to_evict,
                    frame_to_evict,
                    address_to_remove,
                    empty_table,
                    target_page,
                    current_page,
                    parent_table,
                    final_parent_table,
                    current_working_tbale,
                    depth + 1);
            }
        }
    }
    if (is_empty_table && cur_frame != current_working_tbale) { // option 1
        *empty_table = cur_frame;
        *final_parent_table = *parent_table;
    }
}

word_t find_frame(uint64_t target_page, uint64_t parent_table) 
{
    uint64_t max_frame = 0;
    word_t next_frame;
    word_t frame_to_evict = 0;
    word_t page_to_evict = 0;
    uint64_t address_to_remove = 0;
    uint64_t empty_table = 0;
    word_t max_cyclic_value = 0;
    uint64_t final_parent_table = 0;
    uint64_t empty_parent_table = 0;
    
    DFS(0, 
        &max_frame, 
        &max_cyclic_value, 
        &page_to_evict, 
        &frame_to_evict,
        &address_to_remove, 
        &empty_table, 
        target_page, 
        0, 
        &empty_parent_table, 
        &final_parent_table, 
        parent_table, 
        0);

    if (empty_table != parent_table && empty_table != 0) {
        PMwrite(final_parent_table, 0);
        next_frame = (word_t) empty_table;
    }
    else if (max_frame == NUM_FRAMES - 1) {
        PMevict(frame_to_evict, page_to_evict);
        PMwrite(address_to_remove, 0);
        next_frame = frame_to_evict;
    }
    else {
        next_frame = (word_t) max_frame + 1;
    }
    return next_frame;
}

uint64_t restore_page (uint64_t page_num, uint64_t table)
{
  uint64_t frame = find_frame (page_num, table);
  PMrestore (frame, page_num);
  return frame;
}

/**
 * Translate the virtual address binary value to a vector of the different entrence index in each level table.
 */
void virt_address_to_vec (uint64_t address, uint64_t *vec_to_fill)
{
    for (int i = 0; i < TABLES_DEPTH + 1; i++)
    {
        vec_to_fill[i] = address & (x_power_y (2, OFFSET_WIDTH) - 1);
        address = address >> OFFSET_WIDTH;
    }
}

uint64_t get_final_address(uint64_t virt_address, uint64_t *entrences_vec) 
{
    virt_address_to_vec(virt_address, entrences_vec);
    uint64_t page_num = virt_address >> OFFSET_WIDTH;
    uint64_t prev_address = 0;
    word_t cur_address = 0;
    for (int i = TABLES_DEPTH; i > 1; --i)
        {
            auto dest_adress = prev_address * PAGE_SIZE + entrences_vec[i];
            PMread (dest_adress, &cur_address);
            if (cur_address == 0)
                {
                    word_t frame = find_frame (page_num, prev_address);
                    create_new_table(dest_adress, frame);
                    prev_address = frame;
                }
            else
                {
                    prev_address = cur_address;
                }
        }
    PMread (prev_address * PAGE_SIZE + entrences_vec[1], &cur_address);
    if (cur_address == 0)
        {
            uint64_t num_frame = restore_page (page_num, prev_address);
            PMwrite (prev_address * PAGE_SIZE + entrences_vec[1], (word_t) num_frame);
            prev_address = num_frame;
        }
    else
        {
              prev_address = cur_address;
        }
    return prev_address;
}


void VMinitialize()
{
    create_new_table(0, 0);
}

int VMread(uint64_t virtualAddress, word_t* value)
{
    if (virtualAddress < 0 || virtualAddress >= VIRTUAL_MEMORY_SIZE)
        {
          return 0;
        }
    uint64_t address_vec[TABLES_DEPTH + 1];
    uint64_t prev_address = get_final_address (virtualAddress, address_vec);
    PMread (prev_address * PAGE_SIZE + address_vec[0], value);
    return 1;
}

int VMwrite(uint64_t virtualAddress, word_t value)
{
    if (virtualAddress < 0 || virtualAddress >= VIRTUAL_MEMORY_SIZE)
        {
          return 0;
        }
    uint64_t address_vec[TABLES_DEPTH + 1];
    uint64_t prev_address = get_final_address (virtualAddress, address_vec);
    PMwrite (prev_address * PAGE_SIZE + address_vec[0], value);
    return 1;
}
