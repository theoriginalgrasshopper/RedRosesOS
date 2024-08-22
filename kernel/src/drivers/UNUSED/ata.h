#ifndef ATA_H
#define ATA_H
#include <stdint.h>
#include <stddef.h>
typedef struct {
    int master;
    uint16_t dataPort;
    uint8_t errorPort;
    uint8_t sectorCountPort;
    uint8_t lbaLowPort;
    uint8_t lbaMidPort;
    uint8_t lbaHiPort;
    uint8_t devicePort;
    uint8_t commandPort;
    uint8_t controlPort;
} AdvancedTechnologyAttachment;

void ATA_Init(AdvancedTechnologyAttachment* ata, int master, uint16_t portBase);
void ATA_Identify(AdvancedTechnologyAttachment* ata);
void ATA_Read28(AdvancedTechnologyAttachment* ata, uint32_t sectorNum, int count);
void ATA_Write28(AdvancedTechnologyAttachment* ata, uint32_t sectorNum, uint8_t* data, uint32_t count);
void ATA_Flush(AdvancedTechnologyAttachment* ata);
void Probe_ATA_Ports(uint16_t portBase);
void Clear_Fault_Bit();





#endif