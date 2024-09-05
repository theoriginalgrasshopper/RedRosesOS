#include "pmm.h"
#include <limine.h>
#include "mem.h"
#include <sprint.h>
#include <include/binary.h>
#include <include/util.h>
#include <interrupts/cpu.h>
#include <include/constants.h>
// RIPPED FROM ABDOOOS BY ABDOOOWD
/**
 * The idea:
 * - Have a bitmap which is an array of bytes. Each element in 
 * the array is another array of bytes for i entry.
 * Meaning, if I want to access the bitmap of entry i at byte x 
 * I do: bitmap[i][x]
 * 
 * ^ this is a 2d array, AbdooOwd..
 * 
 * 
 */

// TODO: I use too much loops...
// ^ nah it's fine

/* Limine Features */
static volatile struct limine_memmap_request memmap_req = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

static volatile struct limine_hhdm_request hhdm_req = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

static volatile struct limine_memmap_response* memmap;
static volatile struct limine_hhdm_response* hhdm;

/**
 * @brief each element for each entry, then the arrays of each entry are for the pages
 */
uint64_t** bitmap;       
uint64_t* bitmap_sizes;	// measured in bytes
uint32_t free_entry_count;
uint32_t full_bitmap_size;   // measured in bytes

// TODO: Have a data structure to store the free entry index and its actual index in the memmap (+ some info)
// TODO: Optimize
// TODO: Add Error Handling (eg: no entry big enough)
void pmm_init() {
    sprint("INIT PMM", green);
    if (memmap_req.response == NULL)
        sprint("[PMM] Limine MemMap Request's Response is NULL", red);
    
    if (hhdm_req.response == NULL)
        sprint("[PMM] Limine HHDM Request's Response is NULL", red);
    
    memmap = memmap_req.response;
    hhdm = hhdm_req.response;

    /** Whats being done here:
     * - Get number of free entries
     * - get size required for the bitmap
     */
    for (size_t i = 0; i < memmap->entry_count; i++) {
        sprint("ENTERING LOOP", blue);
        struct limine_memmap_entry* entry = memmap->entries[i];

        if (entry->type != LIMINE_MEMMAP_USABLE)
            continue;
        
        sprint("[PMM] Entry  > Base: - Size: \n", blue);
        sprint("[PMM] This Entry's Bitmap Size: \n", blue);

        full_bitmap_size += CEIL_DIV(CEIL_DIV(entry->length, PAGE_SIZE), 8);  // we get it in bytes
        /** explanation:
         * Each page represents a bit. When we divide entry's length
         * by PAGE_SIZE, we get the number of pages that the entry
         * can have. So it also means the number of bits we need.
         * And then (number of bits) / 8 = (number of bytes)
         */

        free_entry_count++;
    }

    sprint ("[PMM] Full bitmap size: %i bytes\n", full_bitmap_size);

    /** Whats done
     * - find how much pages each bitmap (each entry) can have
     * - find a place to store the bitmap
     */
    for (size_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry* entry = memmap->entries[i];
        uint64_t bitmap_size_size = sizeof(uint64_t) * free_entry_count; // lol
        
        if (entry->type != LIMINE_MEMMAP_USABLE)
            continue;
		
		// not enough for bitmap
		if (entry->length < full_bitmap_size + bitmap_size_size)
			continue;
		
        // place the bitmap in memory
		bitmap = (uint64_t**) (entry->base + get_hhdm());
		memset(bitmap, 0, full_bitmap_size);
		entry->base += full_bitmap_size;
		entry->length -= full_bitmap_size;
        sprint("[PMM] Placed Bitmap in memory at \n", blue);

        // place bitmap sizes in memory
        bitmap_sizes = (uint64_t*) (entry->base + get_hhdm());
		memset(bitmap_sizes, 0, bitmap_size_size);
        entry->base += bitmap_size_size;
        entry->length -= bitmap_size_size;
        sprint("[PMM] Placed Bitmap Sizes in Memory at \n", blue);

        sprint("[PMM] New entry base: \n", blue);
		break;
    }

    /** Whats done
     * - Set the size of each entry's bitmap
     */
    size_t free_i = 0;
    /**
     * Iterating over entry count and not free entries count
     * cuz I need the entry's length (extra un-optimized)
     */
	for (size_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry* entry = memmap->entries[i];

		if (entry->type != LIMINE_MEMMAP_USABLE)
			continue;

        // Each index is a byte, and each bit is a page. So each element is (size of datatype * 8) pages.
        bitmap_sizes[free_i] = CEIL_DIV(CEIL_DIV(entry->length, PAGE_SIZE), 8);
        free_i++;
	}
    sprint("SUCCESS", green);
}

void* pmm_alloc(size_t size) {
    /**
     * # The slow way:
     * - We iterate over each entry, then iterate over its bitmap.
     * If the bitmap says that there isn't enough free space for our allocation
     * (if `FLOOR_DIV(size, PAGE_SIZE) > free space`), we go to the next entry.
     * The problem with this approach is that we have to iterate over each entry,
     * then over each bitmap element, then over each bit in the bitmap element... etc
     */

    /**
     * Another Idea is doing as we did with the bitmap and its sizes.
     * You give it that base, increase that base, 
     * and decrease the remaining "free length"
     */

    size_t pages_needed = CEIL_DIV(size, PAGE_SIZE);
    bool found_space = false;

    size_t continued_free_bits = 0;
    // TODO: replace bit_start_offset with those dividing technics like used in the framebuffer (more efficient)
    bitmap_offset_t bitmap_offsets = {
        .bit_offset = 0,
        .entry_index = 0,
        .u64_index = 0
    };  // bad practice (setting everything to 0), we don't know ANYTHING about our offsets!
    bool found_start = false;

    uint64_t cur_uint64_t = (uint64_t) bitmap; // base of current uint64_t

    sprint("[PMM] Need %i free pages\n", blue);

    // TODO: Make allocation reach other uint64_ts
    for (size_t i = 0; i < free_entry_count; i++) {
        if (found_space) break;
        
        /**
         * The idea:
         * - We scan each bit in the bitmap, if we meet a zero,
         * we go through another loop. The loop's condition will be that 
         * each bit we iterate to (??? bad english) should be clear.
         * If we meet a 1 in that loop, we break from it and search for another "free hole".
         * The goal of that is finding a "free hole" large enough for our allocation.
         */

        /**
         * 1. go through each uint64_t (first for loop 'uint64_t_i')
         * 2. go through each bit in the uint64_t (2nd loop 'b')
         * 3. repeat (go back to  step 1)
         */

        for (size_t uint64_t_i = 0; uint64_t_i < bitmap_sizes[i] / sizeof(uint64_t); uint64_t_i++) {
            // sprint("[PMM] uint64_t Index: %i\n", uint64_t_i);

            for (size_t j = 0; j != i; j++)
                cur_uint64_t += bitmap_sizes[j]; // find cur entry base
            
            for (size_t pp = 0; pp < uint64_t_i; pp++)
                cur_uint64_t += sizeof(uint64_t);
            
            sprint("[PMM] Base of current uint64_t:\n", blue);

            /**
             * DEVsprint <--- LOL
             * - I gotta use my bitmap_sizes to find where
             * the uint64_t is. As using it like a simple array
             * won't work. There isn't a fixed size.
             * Each entry's bitmap has a different size.
             */

            for (size_t b = 0; b < sizeof(uint64_t) * 8; b++) {
                // sprint("[PMM] Bit: %i - uint64_t: %i - Free Entry: %i\n", b, uint64_t_i, i);

                if (continued_free_bits >= pages_needed) {
                    sprint("[PMM] Enough space has been found for allocation of %i pages\n", red);
                    found_space = true;
                    break;
                }
                
                // if bit is 0 (page is free)
                if (!TEST_BIT(*((uint64_t*) cur_uint64_t), b)) {
                    if (!found_start) {
                        bitmap_offsets.u64_index = uint64_t_i;
                        bitmap_offsets.bit_offset = b;
                        bitmap_offsets.entry_index = i;
                        found_start = true;
                    }
                    continued_free_bits++;
                    continue;
                } else {
                    continued_free_bits = 0;
                    
                    bitmap_offsets.bit_offset = 0;
                    bitmap_offsets.u64_index = 0;
                    found_start = false;
                }
            }
        }
    }

    sprint("[PMM] Found %i free pages starting at bit %i of uint64_t %i in entry %i\n", green);

    if (found_start && found_space) {
        void* ptr;
        uint64_t ptr_base = 0;
        // step 1: set all found bits as used now
        for (size_t b = 0; b < pages_needed; b++)
            SET_BIT(*((uint64_t*) cur_uint64_t), b + bitmap_offsets.bit_offset);
        
        // step 2: get the address
        /**
         * 1. get entry index
         * 2. calculate the page address/position using the bitmap
         * 3. boom, you got the base
         */
        size_t free_i = 0;
        for (size_t i = 0; i < memmap->entry_count; i++) {
            struct limine_memmap_entry* entry = memmap->entries[i];

            if (entry->type != LIMINE_MEMMAP_USABLE)
                continue;
            
            if (free_i == bitmap_offsets.entry_index) {
                ptr_base = entry->base;
                break;
            }

            free_i++;
        }
        ptr = (void*) (ptr_base + (uint64_t) (((bitmap_offsets.u64_index * 8) + bitmap_offsets.bit_offset) * PAGE_SIZE) + get_hhdm());
        sprint("[PMM] Returning Pointer Address: %x\n", (uint64_t) ptr);
        return ptr;
    } else return NULL;
}

void* pmm_alloc_quiet(size_t size) {
    /**
     * # The slow way:
     * - We iterate over each entry, then iterate over its bitmap.
     * If the bitmap says that there isn't enough free space for our allocation
     * (if `FLOOR_DIV(size, PAGE_SIZE) > free space`), we go to the next entry.
     * The problem with this approach is that we have to iterate over each entry,
     * then over each bitmap element, then over each bit in the bitmap element... etc
     */

    /**
     * Another Idea is doing as we did with the bitmap and its sizes.
     * You give it that base, increase that base, 
     * and decrease the remaining "free length"
     */

    size_t pages_needed = CEIL_DIV(size, PAGE_SIZE);
    bool found_space = false;

    size_t continued_free_bits = 0;
    // TODO: replace bit_start_offset with those dividing technics like used in the framebuffer (more efficient)
    bitmap_offset_t bitmap_offsets = {
        .bit_offset = 0,
        .entry_index = 0,
        .u64_index = 0
    };  // bad practice (setting everything to 0), we don't know ANYTHING about our offsets!
    bool found_start = false;

    uint64_t cur_uint64_t = (uint64_t) bitmap; // base of current uint64_t

    // TODO: Make allocation reach other uint64_ts
    for (size_t i = 0; i < free_entry_count; i++) {
        if (found_space) break;
        
        /**
         * The idea:
         * - We scan each bit in the bitmap, if we meet a zero,
         * we go through another loop. The loop's condition will be that 
         * each bit we iterate to (??? bad english) should be clear.
         * If we meet a 1 in that loop, we break from it and search for another "free hole".
         * The goal of that is finding a "free hole" large enough for our allocation.
         */

        /**
         * 1. go through each uint64_t (first for loop 'uint64_t_i')
         * 2. go through each bit in the uint64_t (2nd loop 'b')
         * 3. repeat (go back to  step 1)
         */

        for (size_t uint64_t_i = 0; uint64_t_i < bitmap_sizes[i] / sizeof(uint64_t); uint64_t_i++) {
            // sprint("[PMM] uint64_t Index: %i\n", uint64_t_i);

            for (size_t j = 0; j != i; j++)
                cur_uint64_t += bitmap_sizes[j]; // find cur entry base
            
            for (size_t pp = 0; pp < uint64_t_i; pp++)
                cur_uint64_t += sizeof(uint64_t);
            

            /**
             * DEVsprint <--- LOL
             * - I gotta use my bitmap_sizes to find where
             * the uint64_t is. As using it like a simple array
             * won't work. There isn't a fixed size.
             * Each entry's bitmap has a different size.
             */

            for (size_t b = 0; b < sizeof(uint64_t) * 8; b++) {
                // sprint("[PMM] Bit: %i - uint64_t: %i - Free Entry: %i\n", b, uint64_t_i, i);

                if (continued_free_bits >= pages_needed) {
                    found_space = true;
                    break;
                }
                
                // if bit is 0 (page is free)
                if (!TEST_BIT(*((uint64_t*) cur_uint64_t), b)) {
                    if (!found_start) {
                        bitmap_offsets.u64_index = uint64_t_i;
                        bitmap_offsets.bit_offset = b;
                        bitmap_offsets.entry_index = i;
                        found_start = true;
                    }
                    continued_free_bits++;
                    continue;
                } else {
                    continued_free_bits = 0;
                    
                    bitmap_offsets.bit_offset = 0;
                    bitmap_offsets.u64_index = 0;
                    found_start = false;
                }
            }
        }
    }

    if (found_start && found_space) {
        void* ptr;
        uint64_t ptr_base = 0;
        // step 1: set all found bits as used now
        for (size_t b = 0; b < pages_needed; b++)
            SET_BIT(*((uint64_t*) cur_uint64_t), b + bitmap_offsets.bit_offset);
        
        // step 2: get the address
        /**
         * 1. get entry index
         * 2. calculate the page address/position using the bitmap
         * 3. boom, you got the base
         */
        size_t free_i = 0;
        for (size_t i = 0; i < memmap->entry_count; i++) {
            struct limine_memmap_entry* entry = memmap->entries[i];

            if (entry->type != LIMINE_MEMMAP_USABLE)
                continue;
            
            if (free_i == bitmap_offsets.entry_index) {
                ptr_base = entry->base;
                break;
            }

            free_i++;
        }
        ptr = (void*) (ptr_base + (uint64_t) (((bitmap_offsets.u64_index * 8) + bitmap_offsets.bit_offset) * PAGE_SIZE) + get_hhdm());
        return ptr;
    } else return NULL;
}

void pmm_free(void* ptr, size_t size) {
    uint64_t physical_address = (uint64_t)ptr - get_hhdm();

    size_t pages_to_free = CEIL_DIV(size, PAGE_SIZE);

    size_t free_i = 0;
    struct limine_memmap_entry* entry = NULL;
    for (size_t i = 0; i < memmap->entry_count; i++) {
        entry = memmap->entries[i];
        if (entry->type != LIMINE_MEMMAP_USABLE) continue;

        if (physical_address >= entry->base && physical_address < entry->base + entry->length) {
            break;
        }
        free_i++;
    }

    uint64_t offset_in_entry = physical_address - entry->base;
    size_t bit_offset = offset_in_entry / PAGE_SIZE;
    size_t u64_index = bit_offset / 64;
    size_t bit_in_u64 = bit_offset % 64;

    uint64_t* cur_uint64_t = (uint64_t*)((uint64_t)bitmap + bitmap_sizes[free_i] * u64_index);

    for (size_t b = 0; b < pages_to_free; b++) {
        size_t bit_pos = bit_in_u64 + b;
        size_t cur_u64_index = u64_index + (bit_pos / 64);
        size_t cur_bit_in_u64 = bit_pos % 64;
        
        uint64_t* bitmap_entry = (uint64_t*)((uint64_t)bitmap + bitmap_sizes[free_i] * cur_u64_index);
        CLEAR_BIT(*bitmap_entry, cur_bit_in_u64);
    }

    sprint("\n[PMM] Freed pages at address \n", white);
}

void pmm_free_auto(){
// insert code here...
}

uint64_t get_hhdm() {
	return hhdm->offset;
}