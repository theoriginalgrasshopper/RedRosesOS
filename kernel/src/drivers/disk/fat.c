#include "ata.h"
#include "mbr.h"
#include <stdint.h>
#include <stddef.h>
#include <sprint.h>
#include <include/constants.h>
#include <a_tools/convert_to_int.h>

typedef struct {
    uint8_t jump[3];
    uint8_t softName[8];
    uint16_t bytesPerSector;
    uint8_t sectorsPerCluster;
    uint16_t reservedSectors;
    uint8_t fatCopies;
    uint16_t rootDirEntries;
    uint16_t totalSectors; 
    uint8_t mediaType;
    uint16_t fatSectorCount;
    uint16_t sectorsPerTrack;
    uint16_t headCount;
    uint32_t hiddenSectors;
    uint32_t totalSectorCount;
    uint32_t tableSize;
    uint16_t extFlags;
    uint16_t fatVersion;
    uint32_t rootCluster;
    uint16_t fatInfo;
    uint16_t backupSector;
    uint8_t reserved0[12];
    uint8_t driveNumber;
    uint8_t reserved;
    uint8_t bootSignature;
    uint32_t volumeId;
    uint8_t volumeLabel[11];
    uint8_t fatTypeLabel[8];
} __attribute__((packed)) BiosParameterBlock;


typedef struct {
    uint8_t name[8]; 
    uint8_t ext[3];
    uint8_t attributes;
    uint8_t reserved;
    uint8_t cTimeTenth;
    uint16_t cTime;
    uint16_t cDate;
    uint16_t aTime;
    uint16_t firstClusterHi;
    uint16_t wTime;
    uint16_t wDate;
    uint16_t firstClusterLow;
    uint32_t size;
} __attribute__((packed)) DirectoryEntryFat32;

void ReadBiosBlock(uint32_t partitionOffset){
    char debug_info[256];
    uint8_t* buffer = ATA_Read28_PM(partitionOffset, sizeof(BiosParameterBlock));
    
    if (buffer == NULL) {
        sprint("buffer was null, aborting\n", red);
        return;
    }

    BiosParameterBlock* bpb = (BiosParameterBlock*)buffer;

    uint32_t fatStart = partitionOffset + bpb->reservedSectors;
    int_to_str(fatStart, debug_info);

    sprint("\n\n\n\n", white);

    sprint(debug_info, white);

    uint32_t fatSize = bpb->tableSize; 
    sprint("\n\n\n\n", white);
    int_to_str(fatSize, debug_info);

    sprint(debug_info, white);

    sprint("1\n", white);

    uint32_t dataStart = fatStart + (bpb->fatCopies * fatSize);
    
    int_to_str(dataStart, debug_info);
    sprint("\n\n\n\n", white);
    
    sprint(debug_info, white);
    
    sprint("2\n", white);

    
    uint32_t rootStart = dataStart + (bpb->rootCluster - 2) * bpb->sectorsPerCluster;
    
    int_to_str(rootStart, debug_info);
    sprint("\n\n\n\n", white);
    
    sprint(debug_info, white);

    if (rootStart > 0x0FFFFFFF) {
        sprint("rootStart exceeds 28-bit LBA limit, aborting\n", red);
        return;
    }
    sprint("3\n", white);

    DirectoryEntryFat32* dirent[16];
    uint8_t* buffer_part = ATA_Read28_PM(rootStart, 16*sizeof(DirectoryEntryFat32));
    dirent[0] = (DirectoryEntryFat32*)buffer_part;

    sprint("4\n", white);

    for(int i = 0; i < 16; i++){
        if(dirent[i]->name[0] == 0x00){
            break;
        }

        char* foo = "       \0";
        for(int j = 0; j < 8; j++){
            foo[j] = dirent[i]->name[j];
        }
        sprint(foo, magenta);
        sprint("\n", white); 
    }
}