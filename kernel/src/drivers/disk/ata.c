#include <interrupts/io.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sprint.h>
#include <include/constants.h>
#include <interrupts/irq.h>
#include <a_tools/convert_to_int.h>
typedef struct {
    uint16_t dataPort;
    uint8_t errorPort;
    uint8_t sectorCountPort;
    uint8_t lbaLowPort;
    uint8_t lbaMidPort;
    uint8_t lbaHiPort;
    uint8_t devicePort;
    uint8_t commandPort;
    uint8_t controlPort;
    bool master;
} AdvancedTechnologyAttachment;

void AdvancedTechnologyAttachment_Init(AdvancedTechnologyAttachment* ata, bool master, uint16_t portbase){
    ata->dataPort = portbase;
    ata->errorPort = portbase + 0x1;
    ata->sectorCountPort = portbase + 0x2;
    ata->lbaLowPort = portbase + 0x3;
    ata->lbaMidPort = portbase + 0x4;
    ata->lbaHiPort = portbase + 0x5;
    ata->devicePort = portbase + 0x6;
    ata->commandPort = portbase + 0x7;
    ata->controlPort = portbase + 0x206;
    ata->master = master;
}
void ATA_Identify(AdvancedTechnologyAttachment* ata){
    sprint("ata indentifying... \n", blue);
    // devicePort.Write -> outb_special, outw if 16 bit
    // devicePort.Read -> inb_special, inw if 16 bit
    outb_special(ata->devicePort, ata->master ? 0xA0 : 0xB0);
    outb_special(ata->controlPort, 0);

    outb_special(ata->devicePort, 0xA0);
    uint8_t status = inb_special(ata->commandPort);
    if(status == 0xFF){
        sprint("status was 0xFF, aborting \n", red);
        return;
    }
    sprint("first check success", green);

    outb_special(ata->devicePort, ata->master ? 0xA0 : 0xB0);
    outb_special(ata->sectorCountPort, 0);
    outb_special(ata->lbaLowPort, 0);
    outb_special(ata->lbaMidPort, 0);
    outb_special(ata->lbaHiPort, 0);
    outb_special(ata->commandPort, 0xEC); // identify

    status = inb_special(ata->controlPort);
    if (status == 0x00){
        sprint("status was 0x00, aborting \n", red);
        return;
    }
    sprint("second check success", green);


    while (((status & 0x80) == 0x80) && ((status & 0x08) != 0x08)){
        status = inb_special(ata->commandPort);
    }

    if (status & 0x01){
        sprint("ERROR status & 0x01 \n", red);
        return;
    }
    sprint("third check success", green);

    for (int i = 0; i < 256; i++){
        uint16_t data = inw_special(ata->dataPort);
        char *text = " \0";
        char text1[1];
        char text2[1];
        int_to_str((data >> 8) & 0xFF, text1);
        int_to_str(data & 0xFF, text2);
        text[0] = text1;
        text[1] = text2;
        sprint(text, green);
    }
    sprint("\n", white);
}

void ATA_Read28(AdvancedTechnologyAttachment* ata, uint32_t sectorNum, int count){

    if (sectorNum > 0x0FFFFFFF){
        sprint("sector number was above the limit, aborting \n", red);
        return;
    }
    // 77
    outb_special(ata->devicePort, (ata->master ? 0xE0 : 0xF0) | ((sectorNum & 0x0F000000) >> 24));
    outb_special(ata->errorPort, 0);
    outb_special(ata->sectorCountPort, 1);
    outb_special(ata->lbaLowPort, sectorNum & 0x000000FF);
    outb_special(ata->lbaMidPort, (sectorNum & 0x0000FF00) >> 8);
    outb_special(ata->lbaHiPort, (sectorNum & 0x00FF0000) >> 16);
    outb_special(ata->commandPort, 0x20);

    uint8_t status = inb_special(ata->commandPort);

    while (((status & 0x80) == 0x80) || ((status & 0x08) != 0x08)){
        status = inb_special(ata->commandPort);
    }
    if (status & 0x01){
        sprint("Error, status masked with 0x01 \n", red);
        return;
    }


    sprint("Reading ATA drive: ", green);

    for(int i = 0; i < count; i+=2){
        uint16_t wdata = inw_special(ata->dataPort);

        char *text = "  \0";
        text[0] = wdata & 0xFF;

        if(i+1 < count){
            text[1] = (wdata >> 8) & 0xFF;
        }
        else{
            text[1] = '\0';
        }
        sprint(text, green);
    }
    for(int i = count + (count%2); i < 512; i+= 2){
        inw_special(ata->dataPort);
    }
}
// 118
void ATA_Write28(AdvancedTechnologyAttachment* ata, uint32_t sectorNum, uint8_t* data, uint32_t count){
    
    if(sectorNum > 0x0FFFFFFF){
        sprint("sector number was above the limit, aborting \n", red);
        return;
    }
    if(count > 512){
        sprint("count was above the limit, aborting \n", red);
        return;
    }

    outb_special(ata->devicePort, (ata->master ? 0xE0 : 0xF0) | ((sectorNum & 0x0F000000) >> 24));
    outb_special(ata->errorPort, 0);
    outb_special(ata->sectorCountPort, 1);

    outb_special(ata->lbaLowPort, sectorNum & 0x000000FF);
    outb_special(ata->lbaMidPort, (sectorNum & 0x0000FF00) >> 8);
    outb_special(ata->lbaHiPort, (sectorNum & 0x00FF0000) >> 16);
    
    outb_special(ata->commandPort, 0x30);

    sprint("Writing to ATA Drive: ", green);

    for(int i = 0; i < count; i+= 2){
        uint16_t wdata = data[i];
        if(i+1 < count){
            wdata |= ((uint16_t)data[i+1]) << 8;
        }
        outw_special(ata->dataPort, wdata);

        char *text = "  \0";
        text[0] = (wdata >> 8) & 0xFF;
        text[1] = wdata & 0xFF;
        sprint(text, green);
    }
    for(int i = count + (count%2); i < 512; i += 2){
        outw_special(ata->dataPort, 0x0000);
    }
}

void ATA_Flush(AdvancedTechnologyAttachment* ata){
    outb_special(ata->devicePort, ata->master ? 0xE0 : 0xF0);
    outb_special(ata->commandPort, 0xE7);

    uint8_t status = inb_special(ata->commandPort);
    if(status == 0x00){
        sprint("status returned 0, aborting \n", red);
        return;
    }
    while(((status & 0x80) == 0x80) && ((status & 0x01) != 0x01)){
        status = inb_special(ata->commandPort);
    }
    if(status & 0x01){
        sprint("status masked with 0x01, aborting \n", red);
        return;
    }
}

InterruptRegisters* ATA_IRQ_Handler(InterruptRegisters* regs){
    uint8_t status = inb_special(0x1F7);
    return regs;
}

void ATA_INIT_IRQ(){
    IRQ_installHandler(14, ATA_IRQ_Handler);
}

void ATA_ALL_INIT(){
    ATA_INIT_IRQ();

    AdvancedTechnologyAttachment ataPM;
    AdvancedTechnologyAttachment_Init(&ataPM, true, 0x1F0);

    ATA_Identify(&ataPM);
}