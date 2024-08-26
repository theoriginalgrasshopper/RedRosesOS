#include <interrupts/io.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sprint.h>
#include <include/constants.h>
#include <interrupts/irq.h>
#include <a_tools/convert_to_int.h>
#include "ata.h"
#define ATA_SECTOR_SIZE 512
// This is going to be bad... oh no
// so this is like a VERY bad thing to do, but i do not care, it works
// the big ass commented out chunk is the preious system, it didn't work  
// like at all, the fucking pointers were doing some bs                              
// idk WHY it didn't work, but again, i do not care                                  


// typedef struct {
//     uint16_t dataPort;
//     uint8_t errorPort;
//     uint8_t sectorCountPort;
//     uint8_t lbaLowPort;
//     uint8_t lbaMidPort;
//     uint8_t lbaHiPort;
//     uint8_t devicePort;
//     uint8_t commandPort;
//     uint8_t controlPort;
//     bool master;
// } AdvancedTechnologyAttachment;

// void AdvancedTechnologyAttachment_Init(AdvancedTechnologyAttachment* ata, bool master, uint16_t portbase){
//     ata->dataPort = portbase;
//     ata->errorPort = portbase + 0x1;
//     ata->sectorCountPort = portbase + 0x2;
//     ata->lbaLowPort = portbase + 0x3;
//     ata->lbaMidPort = portbase + 0x4;
//     ata->lbaHiPort = portbase + 0x5;
//     ata->devicePort = portbase + 0x6;
//     ata->commandPort = portbase + 0x7;
//     ata->controlPort = portbase + 0x206;
//     ata->master = master;
// }

void ATA_Identify_PM(){
    sprint("ata indentifying... \n", blue);
    // devicePort.Write -> outb_special, outw if 16 bit
    // devicePort.Read -> inb_special, inw if 16 bit
    outb_special(PM_DEVICE_PORT, PM_MASTER ? 0xA0 : 0xB0);
    outb_special(PM_CONTROL_PORT, 0);

    outb_special(PM_DEVICE_PORT, 0xA0);
    uint8_t status = inb_special(PM_COMMAND_PORT);
    if(status == 0xFF){
        sprint("status was 0xFF, aborting \n", red);
        return;
    }
    sprint("first check success\n", green);

    outb_special(PM_DEVICE_PORT, PM_MASTER ? 0xA0 : 0xB0);
    outb_special(PM_SECTOR_COUNT_PORT, 0);
    outb_special(PM_LBA_LOW_PORT, 0);
    outb_special(PM_LBA_MID_PORT, 0);
    outb_special(PM_LBA_HI_PORT, 0);
    outb_special(PM_COMMAND_PORT, 0xEC); // identify

    status = inb_special(PM_CONTROL_PORT);
    if (status == 0x00){
        sprint("status was 0x00, aborting \n", red);
        return;
    }
    sprint("second check success\n", green);


    while (((status & 0x80) == 0x80) && ((status & 0x08) != 0x08)){
        status = inb_special(PM_COMMAND_PORT);
    }

    if (status & 0x01){
        sprint("ERROR status & 0x01 \n", red);
        return;
    }
    sprint("third check success\n", green);
    sprint("DRIVE IDENTIFIED: \n", cyan);
    for (int i = 0; i < 256; i++){
        uint16_t data = inw_special(PM_DATA_PORT);
        char text[3] = " \0";
        text[0] = (data >> 8) & 0xFF;
        text[1] = data & 0xFF;
        sprint(text, green);
    }
    sprint("\n", white);
}

static uint8_t ata_read_buffer[ATA_SECTOR_SIZE] = {0};

uint8_t* ATA_Read28_PM(uint32_t sectorNum, int count){

    // empty previous bs
    ata_read_buffer[count] = '\0';

    // continue
    if (sectorNum > 0x0FFFFFFF){
        sprint("sector number was above the limit, aborting \n", red);
        return;
    }
    // 77
    outb_special(PM_DEVICE_PORT, (PM_MASTER ? 0xE0 : 0xF0) | ((sectorNum & 0x0F000000) >> 24));
    outb_special(PM_ERROR_PORT, 0);
    outb_special(PM_SECTOR_COUNT_PORT, 1);
    outb_special(PM_LBA_LOW_PORT, sectorNum & 0x000000FF);
    outb_special(PM_LBA_MID_PORT, (sectorNum & 0x0000FF00) >> 8);
    outb_special(PM_LBA_HI_PORT, (sectorNum & 0x00FF0000) >> 16);
    outb_special(PM_COMMAND_PORT, 0x20);

    uint8_t status = inb_special(PM_COMMAND_PORT);

    while (((status & 0x80) == 0x80) || ((status & 0x08) != 0x08)){
        status = inb_special(PM_COMMAND_PORT);
    }
    if (status & 0x01){
        sprint("status masked with 0x01, aborting \n", red);
        return;
    }

    sprint("Reading ATA drive: ", green);

    // Read data into the static buffer
    for (int i = 0; i < ATA_SECTOR_SIZE; i += 2) {
        uint16_t wdata = inw_special(PM_DATA_PORT);
        ata_read_buffer[i] = wdata & 0xFF;
        if (i + 1 < ATA_SECTOR_SIZE) {
            ata_read_buffer[i + 1] = (wdata >> 8) & 0xFF;
        }
    }

    // Print the data that was read (optional)
    for (int i = 0; i < count; i++) {
        char text[2] = {ata_read_buffer[i], '\0'};
        sprint(text, green);
    }

    sprint("\n", white);

    // Return the buffer pointer to allow further use of the data
    return ata_read_buffer;
}

// 118
void ATA_Write28_PM(uint32_t sectorNum, uint8_t* data, uint32_t count){
    
    if(sectorNum > 0x0FFFFFFF){
        sprint("sector number was above the limit, aborting \n", red);
        return;
    }
    if(count > 512){
        sprint("count was above the limit, aborting \n", red);
        return;
    }

    outb_special(PM_DEVICE_PORT, (PM_MASTER ? 0xE0 : 0xF0) | ((sectorNum & 0x0F000000) >> 24));
    outb_special(PM_ERROR_PORT, 0);
    outb_special(PM_SECTOR_COUNT_PORT, 1);

    outb_special(PM_LBA_LOW_PORT, sectorNum & 0x000000FF);
    outb_special(PM_LBA_MID_PORT, (sectorNum & 0x0000FF00) >> 8);
    outb_special(PM_LBA_HI_PORT, (sectorNum & 0x00FF0000) >> 16);
    
    outb_special(PM_COMMAND_PORT, 0x30);

    sprint("Writing to ATA Drive: ", green);

    for(int i = 0; i < count; i+= 2){
        uint16_t wdata = data[i];
        if(i+1 < count){
            wdata |= ((uint16_t)data[i+1]) << 8;
        }
        outw_special(PM_DATA_PORT, wdata);

        char text[3];
        int_to_str(wdata, text);
        sprint(text, green);
    }
    for(int i = count + (count%2); i < 512; i += 2){
        outw_special(PM_DATA_PORT, 0x0000);
    }
    sprint("\n", white);
}

void ATA_Flush_PM(){
    outb_special(PM_DEVICE_PORT, PM_MASTER ? 0xE0 : 0xF0);
    outb_special(PM_COMMAND_PORT, 0xE7);

    uint8_t status = inb_special(PM_COMMAND_PORT);
    if(status == 0x00){
        sprint("status returned 0, aborting \n", red);
        return;
    }
    while(((status & 0x80) == 0x80) && ((status & 0x01) != 0x01)){
        status = inb_special(PM_COMMAND_PORT);
    }
    if(status & 0x01){
        sprint("status masked with 0x01, aborting \n", red);
        return;
    }
}

InterruptRegisters* ATA_IRQ_Handler(InterruptRegisters* regs){
    inb_special(0x1F7);
    return regs;
}

void ATA_INIT_IRQ(){
    IRQ_installHandler(14, ATA_IRQ_Handler);
}

void ATA_ALL_INIT(){
    ATA_INIT_IRQ();
    ATA_Identify_PM();
}