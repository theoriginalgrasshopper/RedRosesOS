#include <stdint.h>
#include <stdbool.h>
#include <include/constants.h>
#include "ata.h"
#include <sprint.h>
#include <a_tools/convert_to_int.h>
#include <drivers/keyboard.h>
#include <stddef.h>
#include "fat.h"

// MBR -----------------------------------------------------------------------------------------------------

#define MBR_SIZE 512
#define PARTITION_TABLE_OFFSET 446
#define PARTITION_ENTRY_SIZE 16
#define PARTITION_COUNT 4 // hardcoding that because mbr doesnt support LOL i've personally got like 8 partitions on my main machine with GPT. Times have changed for sure

uint32_t partition_lba_first = 0;

typedef struct {
    uint8_t bootable;        // 0x00: inactive, 0x80: bootable, can't make it a bool becuse fuck you
    uint8_t chs_first[3];    // cylinder-head-sector address of the first sector
    uint8_t type;            // partition type
    uint8_t chs_last[3];     // cylinder-head-sector address of the last sector
    uint32_t lba_first;      // lba of first absolute sector in partition
    uint32_t sector_count;   // num of sectors in partition
} __attribute__((packed)) PartitionEntry;

typedef struct {
    uint8_t boot_code[440];  // bootstrap
    uint32_t disk_signature; // disk signature (pointless)
    uint16_t reserved;       // reserverd bs (pointless)
    PartitionEntry partitions[PARTITION_COUNT]; // partition table entries
    uint16_t signature;      // 0x55AA (end of mbr marker)
} __attribute__((packed)) MasterBootRecord;

bool partition_exists;
bool read_mbr() {
    uint8_t* buffer = ATA_Read28_PM(0, MBR_SIZE);

    MasterBootRecord* mbr = (MasterBootRecord*)buffer;

    if (mbr->signature != 0xAA55) { // magic number
        sprint("\ninvalid mbr signature, fix up your mbr dude\n", red);
        return false;
    }

    sprint("\nfound valid mbr signature\n", green);
    
    for (int i = 0; i < PARTITION_COUNT; i++){
        PartitionEntry* partition = &mbr->partitions[i];

        sprint("partition ", magenta);

        char index_str[2] = { '0' + i, '\0' }; // magic
        sprint(index_str, blue);
        
        sprint(":\n", blue);

        // boot-ability...?

        sprint("  bootable?: ", yellow);
        if (partition->bootable == 0x80) {
            sprint("yea\n", cyan);
        } 
        else {
            sprint("no\n", red);
        }
        
        // type

        sprint("  type: ", yellow);

        char type_str[4];
        int_to_str(partition->type, type_str); 
        sprint(type_str, yellow);

        sprint("\n", white);

        // lba 

        sprint("  lba first sector: ", yellow);
        char lba_str[12];
        int_to_str(partition->lba_first, lba_str); 
        sprint(lba_str, yellow);
        sprint("\n", white);

        // sector count
        
        sprint("  sector Count: ", yellow);
        char sector_count_str[12];
        int_to_str(partition->sector_count, sector_count_str); // Assuming int_to_str is available
        sprint(sector_count_str, green);

        sprint("\n", white);

        // exists??
        if(string_same(type_str, "0")){
            partition_exists = false;    
        }
        else{
            partition_exists = true;
        }

        sprint("  does it exist?: ", yellow);
        if(partition_exists == false){
            sprint("no\n", red);
        }else if(partition_exists == true){
            sprint("yea\n", cyan);
        }
        // check if partition is FAT32 using magic numbers teehee
        if(partition_exists == true){
            if (partition->type == 0x0B || partition->type == 0x0C) {
                partition_lba_first = partition->lba_first;
                sprint("\nFAT32 partition found at LBA: ", green);
                sprint(lba_str, green);
                sprint("\n", white);

                ReadBiosBlock(2048);

                break; // if fat32 found, exit
            }
        }
    }
    return partition_lba_first != 0;
}


