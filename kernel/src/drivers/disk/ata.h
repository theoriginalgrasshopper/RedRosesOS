#ifndef ATA_H
#define ATA_H
#include <stdint.h>
#pragma once

void ATA_ALL_INIT();
uint8_t* ATA_Read28_PM(uint32_t sectorNum, int count);
void ATA_Write28_PM(uint32_t sectorNum, uint8_t* data, uint32_t count);
void ATA_Flush_PM();
void ATA_Read28_PM_INTO_BUFFER(uint32_t sectorNum, int count, char* buffer);
//                                                                                   
// primary master                                                                    

#define PM_PORT 0x1F0
#define PM_DATA_PORT 0x1F0
#define PM_ERROR_PORT 0x1F1
#define PM_SECTOR_COUNT_PORT 0x1F2
#define PM_LBA_LOW_PORT 0x1F3
#define PM_LBA_MID_PORT 0x1F4
#define PM_LBA_HI_PORT 0x1F5
#define PM_DEVICE_PORT 0x1F6
#define PM_COMMAND_PORT 0x1F7
#define PM_CONTROL_PORT 0x3F6
#define PM_MASTER 1

#endif