#ifndef FAT_H
#define FAT_H

#include <stdint.h>
#include <stddef.h>
#include "ata.h"
#include "mbr.h"
#include <stdbool.h>
#pragma once






void ReadBiosBlock(uint32_t partitionOffset);










#endif