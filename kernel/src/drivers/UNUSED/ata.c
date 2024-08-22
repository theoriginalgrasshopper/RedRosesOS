#include "ata.h"
#include <sprint.h>
#include <include/constants.h>
#include <a_tools/convert_to_int.h>
#include <interrupts/idt.h>
#include <interrupts/irq.h>
// TEMP, MOVE TO PORTS.C


void Port_Write8(uint16_t port, uint8_t data) {
    __asm__ volatile("outb %0, %1" : : "a" (data), "Nd" (port));
}

uint8_t Port_Read8(uint16_t port) {
    uint8_t result;
    __asm__ volatile("inb %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

void Port_Write16(uint16_t port, uint16_t data) {
    __asm__ volatile("outw %0, %1" : : "a" (data), "Nd" (port));
}
uint16_t Port_Read16(uint16_t port) {
    uint16_t result;
    __asm__ volatile("inw %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

void Port_Write32(uint16_t port, uint32_t data) {
    __asm__ volatile("outl %0, %1" : : "a" (data), "Nd" (port));
}

uint32_t Port_Read32(uint16_t port) {
    uint32_t result;
    __asm__ volatile("inl %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

// DEBUG------------------------------------------------------------------------

void Probe_ATA_Ports(uint16_t portBase) {
    sprint("Probing ATA Ports for port base: ", yellow);
    char port_str[256];
    int_to_str(portBase, port_str);
    sprint(port_str, yellow);
    sprint("\n\n", white);
    
    // Read from key ports
    uint8_t status = Port_Read8(portBase + 0x7); // Command/Status port
    uint8_t error = Port_Read8(portBase + 0x1);  // Error/Features port

    sprint("Status Register: ", white);
    char status_str[256];
    int_to_str(status, status_str);
    sprint(status_str, white);
    sprint("\n", white);
    
    sprint("Error Register: ", white);
    char error_str[256];
    int_to_str(error, error_str);
    sprint(error_str, white);
    sprint("\n", white);

    if(status == 0xFF) {
        sprint("No device detected or incorrect port.\n", red);
    } else {
        sprint("Device detected or port is correct.\n", green);
    }
}

uint8_t cleared_status;

void Clear_Fault_Bit() {
    Port_Write8(0x3F6, 0x04); 
    cleared_status = Port_Read8(0x3F6);

    char status_str[256];
    int_to_str(cleared_status, status_str);
    sprint("Status after reset: ", yellow);
    sprint(status_str, yellow);
    sprint("\n\n", white);
}

// ATA------------------------------------------------------------------------------------------------------


void ATA_Init(AdvancedTechnologyAttachment* ata, int master, uint16_t portBase) {
    ata->master = master;
    ata->dataPort = portBase;
    ata->errorPort = portBase + 0x1;
    ata->sectorCountPort = portBase + 0x2;
    ata->lbaLowPort = portBase + 0x3;
    ata->lbaMidPort = portBase + 0x4;
    ata->lbaHiPort = portBase + 0x5;
    ata->devicePort = portBase + 0x6;
    ata->commandPort = portBase + 0x7;
    ata->controlPort = (portBase == 0x1F0) ? 0x3F6 : 0x376;
}
void ATA_Identify(AdvancedTechnologyAttachment* ata) {

    Port_Write8(ata->devicePort, ata->master ? 0xA0 : 0xB0);
    for (volatile int i = 0; i < 10000; i++); // small delay

    Port_Write8(ata->controlPort, 0);
    for (volatile int i = 0; i < 10000; i++); // small delay

    sprint("Device Port set to: ", blue);

    char dev_port[256];
    int_to_str(Port_Read8(ata->devicePort), dev_port);
    sprint(dev_port, white);
    sprint("\n", white);

    Port_Write8(ata->controlPort, 0);

    sprint("Control Port set to: ", blue);

    char ctrl_port[256];
    int_to_str(Port_Read8(ata->controlPort), ctrl_port);
    sprint(ctrl_port, white);
    sprint("\n\n", white);

    Port_Write8(ata->devicePort, ata->master ? 0xA0 : 0xB0);
    Port_Write8(ata->controlPort, 0);  
    Port_Write8(ata->devicePort, 0xA0);
    Port_Write8(ata->devicePort, ata->master ? 0xA0 : 0xB0);

    for (volatile int i = 0; i < 10000; i++);  // small delay

    uint8_t status = Port_Read8(ata->commandPort);
    Clear_Fault_Bit();
    if (status == 0xFF) {
        sprint("ERROR: No ATA device found.\n\n", red);
        return;
    }
    Port_Write8(ata->devicePort, ata->master ? 0xA0 : 0xB0);
    Port_Write8(ata->sectorCountPort, 0);
    Port_Write8(ata->lbaLowPort, 0);
    Port_Write8(ata->lbaMidPort, 0);
    Port_Write8(ata->lbaHiPort, 0);
    Port_Write8(ata->commandPort, 0xEC); 
    
    status = Port_Read8(ata->commandPort);
    // FIX ME
    if(status == 0x00){
        return;
    }
    // wait til ready
    while(((status & 0x80) == 0x80) && ((status & 0x01) != 0x01)){
        status = Port_Read8(ata->commandPort);
    }
    status = Port_Read8(ata->commandPort);

    // FIX ME
    if(status & 0x01) {
        sprint("ERROR IN IDENTIFYING \n\n", red);
        return;
    }
    else{
        sprint("everything oke :D \n\n", green);
    }
    for (int i = 0; i < 256; i++){
        uint16_t data = Port_Read16(ata->dataPort);
        char text[3] = "  \0";
        text[0] = (data >> 8) & 0xFF;
        text[1] = data & 0xFF;
        sprint(text, white);
    }
}


// ATA READ WRITE ---------------------------------------------------------------------------------------

void ATA_Read28(AdvancedTechnologyAttachment* ata, uint32_t sectorNum, int count) {
    if(sectorNum > 0x0FFFFFFF)
        return;
    
    Port_Write8(ata->devicePort, (ata->master ? 0xE0 : 0xF0) | ((sectorNum & 0x0F000000) >> 24));
    Port_Write8(ata->errorPort, 0);
    Port_Write8(ata->sectorCountPort, 1);
    Port_Write8(ata->lbaLowPort, sectorNum & 0x000000FF);
    Port_Write8(ata->lbaMidPort, (sectorNum & 0x0000FF00) >> 8);
    Port_Write8(ata->lbaHiPort, (sectorNum & 0x00FF0000) >> 16);
    Port_Write8(ata->commandPort, 0x20);
    
    uint8_t status = Port_Read8(ata->commandPort);
    while(((status & 0x80) == 0x80) && ((status & 0x01) != 0x01))
        status = Port_Read8(ata->commandPort);
    
    if(status & 0x01) {
        sprint("ERROR IN READING \n\n", red);
        return;
    }
    
    sprint("reading ATA Drive: ", blue);
    
    for(int i = 0; i < count; i += 2) {
        uint16_t wdata = Port_Read16(ata->dataPort);
        
        char text[3] = {0};
        text[1] = wdata & 0xFF;
        
        if(i+1 < count)
            text[1] = (wdata >> 8) & 0xFF;
        else
            text[1] = '\0';
        
        sprint(text, white);
    }
    
    for(int i = count + (count % 2); i < 512; i += 2)
        Port_Read16(ata->dataPort);
}

void ATA_Write28(AdvancedTechnologyAttachment* ata, uint32_t sectorNum, uint8_t* data, uint32_t count) {
    if(sectorNum > 0x0FFFFFFF || count > 512)
        return;
    
    Port_Write8(ata->devicePort, (ata->master ? 0xE0 : 0xF0) | ((sectorNum & 0x0F000000) >> 24));
    Port_Write8(ata->errorPort, 0);
    Port_Write8(ata->sectorCountPort, 1);
    Port_Write8(ata->lbaLowPort, sectorNum & 0x000000FF);
    Port_Write8(ata->lbaMidPort, (sectorNum & 0x0000FF00) >> 8);
    Port_Write8(ata->lbaHiPort, (sectorNum & 0x00FF0000) >> 16);
    Port_Write8(ata->commandPort, 0x30);
    
    sprint("writing to ATA Drive: ", blue);

    for(uint32_t i = 0; i < count; i += 2) {
        uint16_t wdata = data[i];
        if(i + 1 < count)
            wdata |= ((uint16_t)data[i + 1]) << 8;
        Port_Write16(ata->dataPort, wdata);
        
        char text[3] = {0};
        text[0] = (wdata >> 8) & 0xFF;
        text[1] = wdata & 0xFF;
        sprint(text, white);
    }
    
    for(int i = count + (count % 2); i < 512; i += 2)
        Port_Write16(ata->dataPort, 0x0000);
}

void ATA_Flush(AdvancedTechnologyAttachment* ata) {
    Port_Write8(ata->devicePort, ata->master ? 0xE0 : 0xF0);
    Port_Write8(ata->commandPort, 0xE7);
    
    uint8_t status = Port_Read8(ata->commandPort);
    if(status == 0x00)
        return;
    
    while(((status & 0x80) == 0x80) && ((status & 0x01) != 0x01))
        status = Port_Read8(ata->commandPort);
    
    if(status & 0x01) {
        sprint("ERROR IN FLUSHING JOURNALS \n\n", red);
        return;
    }
}
