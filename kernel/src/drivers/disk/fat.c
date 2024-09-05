#include <stdint.h>
#include <stdbool.h>
#include <include/constants.h>
#include "ata.h"
#include <sprint.h>
#include <a_tools/convert_to_int.h>
#include <drivers/keyboard.h>
#include <stddef.h>
#include <ctype.h>
#include "fat.h"
#include <memory_management/pmm.h>
#include <include/util.h>

// ICON LOADING

// STRINGS -----------------------------------------------------------------
void append_string(char* destination, const char* source) {
    while (*destination) {
        destination++;
    } 
    while (*source) {
        *destination = *source;
        destination++;
        source++;
    }
    *destination = '\0';
}

static bool char_is_lowercase(char c) {
    return c >= 'a' && c <= 'z';
}

static char caps_char(char c) {
    return char_is_lowercase(c) ? (c - 'a' + 'A') : c;
}

// STRUCTS ---------------------------------------------------------------------

typedef struct {
    uint8_t jump_boot[3];
    char oem_name[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sector_count;
    uint8_t num_fats;
    uint16_t root_entry_count;
    uint16_t total_sectors_16;
    uint8_t media_type;
    uint16_t fat_size_16;
    uint16_t sectors_per_track;
    uint16_t num_heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors_32;

    // fat32
    uint32_t fat_size_32;
    uint16_t ext_flags;
    uint16_t fs_version;
    uint32_t root_cluster;
    uint16_t fs_info;
    uint16_t backup_boot_sector;
    uint8_t reserved[12];
    uint8_t drive_number;
    uint8_t reserved1;
    uint8_t boot_signature;
    uint32_t volume_id;
    char volume_label[11];
    char fs_type[8];
} __attribute__((packed)) FAT32_BPB;

typedef struct {
    char name[11];
    uint8_t attr;
    uint8_t reserved;
    uint8_t creation_time_tenth;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t first_cluster_hi;
    uint16_t write_time;
    uint16_t write_date;
    uint16_t first_cluster_lo;
    uint32_t file_size;
} __attribute__((packed)) FAT32_DirectoryEntry;


// BPB ------------------------------------------------------------------------------------------------

uint32_t root_dir_sector;
uint32_t root_dir_sector_public;
FAT32_BPB* bpb;
char buffer1[512];

void Read_BPB(uint32_t sector) {
    ATA_Read28_PM_INTO_BUFFER(2048 + sector, 512, buffer1);
    bpb = (FAT32_BPB*)buffer1;

    // info for debug and whatnot
    sprint("bytes per sector: ", yellow);
    char bps_str[6];
    int_to_str(bpb->bytes_per_sector, bps_str);
    sprint(bps_str, cyan);
    sprint("\n", white);

    sprint("sectors per cluster: ", yellow);
    char spc_str[4];
    int_to_str(bpb->sectors_per_cluster, spc_str);
    sprint(spc_str, cyan);
    sprint("\n", white);

    sprint("number of FATs: ", yellow);
    char fats_str[4];
    int_to_str(bpb->num_fats, fats_str);
    sprint(fats_str, cyan);
    sprint("\n", white);

    sprint("total sectors (32-bit): ", yellow);
    char ts_str[12];
    int_to_str(bpb->total_sectors_32, ts_str);
    sprint(ts_str, cyan);
    sprint("\n", white);

    sprint("theee root cluster!!!!: ", yellow);
    char root_cluster_str[12];
    int_to_str(bpb->root_cluster, root_cluster_str);
    sprint(root_cluster_str, cyan);
    sprint("\n", white);

    // first sector of root
    root_dir_sector = 2048 + (bpb->reserved_sector_count + (bpb->num_fats * bpb->fat_size_32)) + ((bpb->root_cluster - 2) * bpb->sectors_per_cluster);
    root_dir_sector_public = root_dir_sector;
}

void Read_BPB_quiet(uint32_t sector) {
    ATA_Read28_PM_INTO_BUFFER(2048 + sector, 512, buffer1);
    bpb = (FAT32_BPB*)buffer1;
    // first sector of root
    root_dir_sector = 2048 + (bpb->reserved_sector_count + (bpb->num_fats * bpb->fat_size_32)) + ((bpb->root_cluster - 2) * bpb->sectors_per_cluster);
    root_dir_sector_public = root_dir_sector;
}

// FILES ---------------------------------------------------------------------------------------
void Overwrite_File_path(uint32_t root_dir_sector, const char* filepath, const uint8_t* data, uint32_t data_size) {
    if (data_size == NULL){
        sprint("data size was not specified.\n", red);
        return;
    }
    char components[16][12]; // Hardcoding 16 components
    int component_count = 0;
    split_path(filepath, components, &component_count);

    if (component_count == 0) {
        sprint("invalid file path\n", red);
        return;
    }

    uint32_t current_cluster = bpb->root_cluster;

    // Navigate through directories except the last component (which is the file name)
    for (int i = 0; i < component_count - 1; i++) {
        bool found = false;
        char buffer[512];
        FAT32_DirectoryEntry* entry = NULL;
        uint32_t sector = 2048 + (bpb->reserved_sector_count + (bpb->num_fats * bpb->fat_size_32)) + ((current_cluster - 2) * bpb->sectors_per_cluster);

        for (int j = 0; j < bpb->sectors_per_cluster; j++) {
            ATA_Read28_PM_INTO_BUFFER(sector + j, 512, buffer);
            entry = (FAT32_DirectoryEntry*)buffer;

            for (int k = 0; k < 16; k++) {
                if (entry->name[0] == 0x00) {
                    break; // No more entries
                }
                if (entry->name[0] == 0xE5 || entry->attr == 0x0F) {
                    entry++;
                    continue; // Skip deleted or long file name entries
                }

                char name[12];
                memcpy(name, entry->name, 11);
                name[11] = '\0';

                if (string_same(name, components[i]) && (entry->attr & 0x10)) {
                    current_cluster = entry->first_cluster_lo | (entry->first_cluster_hi << 16);
                    found = true;
                    break;
                }
                entry++;
            }

            if (found) break;
        }

        if (!found) {
            sprint("directory not found: ", red);
            sprint(components[i], red);
            sprint("\n", white);
            return;
        }
    }

    // Now we are in the directory with our needed file
    char buffer[512];
    FAT32_DirectoryEntry* entry = NULL;
    uint32_t sector = 2048 + (bpb->reserved_sector_count + (bpb->num_fats * bpb->fat_size_32)) + ((current_cluster - 2) * bpb->sectors_per_cluster);

    for (int i = 0; i < bpb->sectors_per_cluster; i++) {
        ATA_Read28_PM_INTO_BUFFER(sector + i, 512, buffer);
        entry = (FAT32_DirectoryEntry*)buffer;

        for (int j = 0; j < 16; j++) {
            if (entry->name[0] == 0x00) {
                break; // No more entries, file does not exist
            }
            if (entry->name[0] == 0xE5 || entry->attr == 0x0F) {
                entry++;
                continue; // Skip deleted or long file name entries
            }

            char name[12];
            memcpy(name, entry->name, 11);
            name[11] = '\0';

            if (string_same(name, components[component_count - 1])) {
                uint32_t cluster = entry->first_cluster_lo | (entry->first_cluster_hi << 16);

                // Overwrite the file if it exists and is not empty
                if (entry->file_size > 0) {
                    uint32_t bytes_written = 0;
                    while (bytes_written < data_size) {
                        uint32_t sector = 2048 + (bpb->reserved_sector_count + (bpb->num_fats * bpb->fat_size_32)) + ((cluster - 2) * bpb->sectors_per_cluster);
                        uint32_t write_size = (data_size - bytes_written < 512) ? data_size - bytes_written : 512;
                        ATA_Write28_PM(sector, &data[bytes_written], write_size);

                        bytes_written += write_size;

                        // If more data to write, move to the next cluster
                        if (bytes_written < data_size) {
                            ATA_Read28_PM_INTO_BUFFER(2048 + bpb->reserved_sector_count + (cluster / 128), 512, buffer);
                            cluster = ((uint32_t*)buffer)[cluster % 128];

                            if (cluster >= 0x0FFFFFF8) {
                                sprint("file is too small to hold all data, need to allocate more clusters\n", red);
                                return;
                            }
                        }
                    }

                    // update the file size in the directory entry
                    entry->file_size = data_size;

                    // update the directory entry's cluster information
                    entry->first_cluster_lo = cluster & 0xFFFF;
                    entry->first_cluster_hi = (cluster >> 16) & 0xFFFF;

                    // write the directory entry back to disk
                    ATA_Write28_PM(sector + i, (uint8_t*)buffer, 512);

                    sprint("file overwritten successfully: ", green);
                    sprint(components[component_count - 1], cyan);
                    sprint("\n", white);
                } else {
                    sprint("file is empty, writing data\n", yellow);
                    // treat as a new file write
                    entry->file_size = data_size;
                    uint32_t cluster = entry->first_cluster_lo | (entry->first_cluster_hi << 16);

                    // allocate clusters and write data
                    uint32_t bytes_written = 0;
                    while (bytes_written < data_size) {
                        uint32_t sector = 2048 + (bpb->reserved_sector_count + (bpb->num_fats * bpb->fat_size_32)) + ((cluster - 2) * bpb->sectors_per_cluster);
                        uint32_t write_size = (data_size - bytes_written < 512) ? data_size - bytes_written : 512;
                        ATA_Write28_PM(sector, &data[bytes_written], write_size);

                        bytes_written += write_size;

                        // If more data to write, allocate a new cluster
                        if (bytes_written < data_size) {
                            uint32_t next_cluster = Find_Free_Cluster();
                            if (next_cluster == 0xFFFFFFFF) {
                                sprint("no free clusters available\n", red);
                                return;
                            }

                            FAT32_Set_FAT_Entry(cluster, next_cluster);
                            cluster = next_cluster;
                        }
                    }

                    // Update file size and cluster information in the directory entry
                    entry->first_cluster_lo = cluster & 0xFFFF;
                    entry->first_cluster_hi = (cluster >> 16) & 0xFFFF;
                    entry->file_size = data_size;
                    entry->file_size = data_size;
                    entry->file_size = data_size;
                    entry->file_size = data_size;
                    ATA_Write28_PM(sector + i, (uint8_t*)buffer, 512);
                    ATA_Write28_PM(sector + i, (uint8_t*)buffer, 512);
                    ATA_Write28_PM(sector + i, (uint8_t*)buffer, 512);

                    sprint("file written successfully\n", green);
                }

                return;
            }

            entry++;
        }
    }
    sprint("file not found.\n", red);
}



void Read_File_path(uint32_t root_dir_sector, const char* filepath) {
    char components[16][12]; // hardcoding 16
    int component_count = 0;
    split_path(filepath, components, &component_count);

    if (component_count == 0) {
        sprint("invalid file path\n", red);
        return;
    }

    uint32_t current_cluster = bpb->root_cluster;

    // navigate through directories except the last component (which is the file name)
    for (int i = 0; i < component_count - 1; i++) {
        bool found = false;
        char buffer[512];
        FAT32_DirectoryEntry* entry = NULL;
        uint32_t sector = 2048 + (bpb->reserved_sector_count + (bpb->num_fats * bpb->fat_size_32)) + ((current_cluster - 2) * bpb->sectors_per_cluster);

        for (int j = 0; j < bpb->sectors_per_cluster; j++) {
            ATA_Read28_PM_INTO_BUFFER(sector + j, 512, buffer);
            entry = (FAT32_DirectoryEntry*)buffer;

            for (int k = 0; k < 16; k++) {
                if (entry->name[0] == 0x00) {
                    break; // no more entries
                }
                if (entry->name[0] == 0xE5 || entry->attr == 0x0F) {
                    entry++;
                    continue; // skip deleted or long file name entries
                }

                char name[12];
                memcpy(name, entry->name, 11);
                name[11] = '\0';

                if (string_same(name, components[i]) && (entry->attr & 0x10)) {
                    current_cluster = entry->first_cluster_lo | (entry->first_cluster_hi << 16);
                    found = true;
                    break;
                }
                entry++;
            }

            if (found) break;
        }

        if (!found) {
            sprint("directory not found: ", red);
            sprint(components[i], red);
            sprint("\n", white);
            return;
        }
    }

    // now we are in the dir with our needed file
    char buffer[512];
    FAT32_DirectoryEntry* entry = NULL;
    uint32_t sector = 2048 + (bpb->reserved_sector_count + (bpb->num_fats * bpb->fat_size_32)) + ((current_cluster - 2) * bpb->sectors_per_cluster);

    for (int i = 0; i < bpb->sectors_per_cluster; i++) {
        ATA_Read28_PM_INTO_BUFFER(sector + i, 512, buffer);
        entry = (FAT32_DirectoryEntry*)buffer;

        for (int j = 0; j < 16; j++) {
            if (entry->name[0] == 0x00) {
                sprint("file not found: ", red);
                sprint(components[component_count - 1], red);
                sprint("\n", white);
                return; // file not found
            }
            if (entry->name[0] == 0xE5 || entry->attr == 0x0F) {
                entry++;
                continue; // skip deleted or long file name entries
            }

            char name[12];
            memcpy(name, entry->name, 11);
            name[11] = '\0';

            if (string_same(name, components[component_count - 1])) {
                uint32_t cluster = entry->first_cluster_lo | (entry->first_cluster_hi << 16);
                uint32_t file_size = entry->file_size;

            if (file_size == 0) { // if i don't do this, it'll hang when reading empty files. woopsie
                sprint("the file was empty.\n", yellow);
                return;
            }
                while (cluster < 0x0FFFFFF8) {
                    uint32_t sector = 2048 + (bpb->reserved_sector_count + (bpb->num_fats * bpb->fat_size_32)) + ((cluster - 2) * bpb->sectors_per_cluster);
                    ATA_Read28_PM_INTO_BUFFER(sector, 512, buffer);

                    // print file contents
                    for (int i = 0; i < 512 && file_size > 0; i++, file_size--) {
                        char ch[2] = {buffer[i], '\0'};
                        sprint(ch, white);
                    }
                    if(file_size < 513){
                        return;
                    }

                    ATA_Read28_PM_INTO_BUFFER(2048 + bpb->reserved_sector_count + (cluster / 128), 512, buffer);
                    cluster = ((uint32_t*)buffer)[cluster % 128];
                }

                sprint("\n", white);
                return; // file read successfully
            }

            entry++;
        }
    }

    sprint("file not found: ", red);
    sprint(components[component_count - 1], red);
    sprint("\n", white);
}



char* Read_File_path_INTO_BUFFER(uint32_t root_dir_sector, const char* filepath) {
    char components[16][12];
    int component_count = 0;
    split_path(filepath, components, &component_count);

    if (component_count == 0) {
        sprint("invalid file path\n", red);
        return NULL;
    }

    uint32_t current_cluster = bpb->root_cluster;

    for (int i = 0; i < component_count - 1; i++) {
        bool found = false;
        char buffer[512];
        FAT32_DirectoryEntry* entry = NULL;
        uint32_t sector = 2048 + (bpb->reserved_sector_count + (bpb->num_fats * bpb->fat_size_32)) + ((current_cluster - 2) * bpb->sectors_per_cluster);

        for (int j = 0; j < bpb->sectors_per_cluster; j++) {
            ATA_Read28_PM_INTO_BUFFER(sector + j, 512, buffer);
            entry = (FAT32_DirectoryEntry*)buffer;

            for (int k = 0; k < 16; k++) {
                if (entry->name[0] == 0x00) {
                    break;
                }
                if (entry->name[0] == 0xE5 || entry->attr == 0x0F) {
                    entry++;
                    continue;
                }

                char name[12];
                memcpy(name, entry->name, 11);
                name[11] = '\0';

                if (string_same(name, components[i]) && (entry->attr & 0x10)) {
                    current_cluster = entry->first_cluster_lo | (entry->first_cluster_hi << 16);
                    found = true;
                    break;
                }
                entry++;
            }

            if (found) break;
        }

        if (!found) {
            sprint("directory not found: ", red);
            sprint(components[i], red);
            sprint("\n", white);
            return NULL;
        }
    }

    char buffer[512];
    FAT32_DirectoryEntry* entry = NULL;
    uint32_t sector = 2048 + (bpb->reserved_sector_count + (bpb->num_fats * bpb->fat_size_32)) + ((current_cluster - 2) * bpb->sectors_per_cluster);

    for (int i = 0; i < bpb->sectors_per_cluster; i++) {
        ATA_Read28_PM_INTO_BUFFER(sector + i, 512, buffer);
        entry = (FAT32_DirectoryEntry*)buffer;

        for (int j = 0; j < 16; j++) {
            if (entry->name[0] == 0x00) {
                sprint("file not found: ", red);
                sprint(components[component_count - 1], red);
                sprint("\n", white);
                return NULL;
            }
            if (entry->name[0] == 0xE5 || entry->attr == 0x0F) {
                entry++;
                continue;
            }

            char name[12];
            memcpy(name, entry->name, 11);
            name[11] = '\0';

            if (string_same(name, components[component_count - 1])) {
                uint32_t cluster = entry->first_cluster_lo | (entry->first_cluster_hi << 16);
                uint32_t file_size = entry->file_size;

                if (file_size == 0) {
                    sprint("the file was empty.\n", yellow);
                    return NULL;
                }

                // allocate buffer dynamically based on file size
                size_t pages_needed = CEIL_DIV(file_size, PAGE_SIZE);
                size_t buffer_size = pages_needed * PAGE_SIZE;
                char* output_read = (char*)pmm_alloc_quiet(buffer_size);
                if (output_read == NULL) {
                    sprint("failed to allocate memory for file\n", red);
                    return;
                }

                int output_read_pos = 0;
                while (cluster < 0x0FFFFFF8 && output_read_pos < file_size) {
                    uint32_t sector = 2048 + (bpb->reserved_sector_count + (bpb->num_fats * bpb->fat_size_32)) + ((cluster - 2) * bpb->sectors_per_cluster);
                    ATA_Read28_PM_INTO_BUFFER(sector, 512, buffer);

                    // store file contents
                    for (int i = 0; i < 512 && file_size > 0 && output_read_pos < buffer_size; i++, file_size--) {
                        output_read[output_read_pos++] = buffer[i];
                    }

                    ATA_Read28_PM_INTO_BUFFER(2048 + bpb->reserved_sector_count + (cluster / 128), 512, buffer);
                    cluster = ((uint32_t*)buffer)[cluster % 128];
                }

                return output_read;
            }

            entry++;
        }
    }

    sprint("file not found: ", red);
    sprint(components[component_count - 1], red);
    sprint("\n", white);
    return NULL;
}

void Create_File_path(uint32_t root_dir_sector, const char* filepath, const char* extension) {
    char components[16][12]; // hardcoding 16
    int component_count = 0;
    split_path(filepath, components, &component_count);

    if (component_count == 0) {
        sprint("invalid file path\n", red);
        return;
    }

    uint32_t current_cluster = bpb->root_cluster;

    // navigate through directories except the last component (which is the file name)
    for (int i = 0; i < component_count - 1; i++) {
        bool found = false;
        char buffer[512];
        FAT32_DirectoryEntry* entry = NULL;
        uint32_t sector = 2048 + (bpb->reserved_sector_count + (bpb->num_fats * bpb->fat_size_32)) + ((current_cluster - 2) * bpb->sectors_per_cluster);

        for (int j = 0; j < bpb->sectors_per_cluster; j++) {
            ATA_Read28_PM_INTO_BUFFER(sector + j, 512, buffer);
            entry = (FAT32_DirectoryEntry*)buffer;

            for (int k = 0; k < 16; k++) {
                if (entry->name[0] == 0x00) {
                    break; // no more entries
                }
                if (entry->name[0] == 0xE5 || entry->attr == 0x0F) {
                    entry++;
                    continue; // skip deleted or long file name entries
                }

                char name[12];
                memcpy(name, entry->name, 11);
                name[11] = '\0';

                if (string_same(name, components[i]) && (entry->attr & 0x10)) {
                    current_cluster = entry->first_cluster_lo | (entry->first_cluster_hi << 16);
                    found = true;
                    break;
                }
                entry++;
            }

            if (found) break;
        }

        if (!found) {
            sprint("directory not found: ", red);
            sprint(components[i], red);
            sprint("\n", white);
            return;
        }
    }

    // now we are in the directory where we want to create the file
    char buffer[512];
    FAT32_DirectoryEntry* entry = NULL;
    uint32_t sector = 2048 + (bpb->reserved_sector_count + (bpb->num_fats * bpb->fat_size_32)) + ((current_cluster - 2) * bpb->sectors_per_cluster);

    for (int i = 0; i < bpb->sectors_per_cluster; i++) {
        ATA_Read28_PM_INTO_BUFFER(sector + i, 512, buffer);
        entry = (FAT32_DirectoryEntry*)buffer;

        for (int j = 0; j < 16; j++) {
            if (entry->name[0] == 0x00 || entry->name[0] == 0xE5) {
                // found empty entry
                memset(entry->name, ' ', 11); // fill name with spaces

                // copy the filename into the entry
                memcpy(entry->name, components[component_count - 1], strlen(components[component_count - 1]));
                memcpy(&entry->name[8], extension, strlen(extension));

                entry->attr = 0x20; // archive attribute for funnies
                entry->reserved = 0;
                entry->creation_time_tenth = 0;
                entry->creation_time = 0;
                entry->creation_date = 0;
                entry->last_access_date = 0;
                entry->first_cluster_hi = 0; // i'll set this later if needed
                entry->write_time = 0;//year               month      day
                entry->write_date =  ((1984 - 1980) << 9) | (1 << 5) | 1; // all the files created by the user will have their date set to 1984. Literally 1984 :1984: 
                entry->first_cluster_lo = 0; // i'll set this later if needed
                entry->file_size = 0; // start with empty file

                // write updated buffer back to disk
                ATA_Write28_PM(sector + i, (uint8_t*)buffer, 512);

                sprint("file created: ", green);
                sprint(components[component_count - 1], cyan);
                sprint(".", cyan);
                sprint(extension, cyan);
                sprint("\n", white);

                return;
            }
            entry++;
        }
    }

    sprint("no empty directory entry found, the directory is most likely full\n", red);
}


// DIRECTORIES ------------------------------------------------------------------------------------------------------------------

// HELPERS ----------------------------------------------------------------


void FAT32_Set_FAT_Entry(uint32_t current_cluster, uint32_t next_cluster) {
    // Calculate the sector where the FAT entry resides
    uint32_t fat_sector = 2048 + bpb->reserved_sector_count + (current_cluster / 128);

    // Buffer to hold the FAT sector data
    char buffer[512];
    
    // Read the FAT sector
    ATA_Read28_PM_INTO_BUFFER(fat_sector, 512, buffer);
    
    // Calculate the offset of the FAT entry in the sector
    uint32_t offset = (current_cluster % 128) * 4; // 4 bytes per FAT32 entry
    
    // Update the FAT entry with the next cluster value
    ((uint32_t*)buffer)[offset / 4] = next_cluster;
    
    // Write the updated FAT sector back to disk
    ATA_Write28_PM(fat_sector, (uint8_t*)buffer, 512);
}

void split_path(const char* path, char components[][12], int* count) {
    const char* start = path;
    int len = 0;
    *count = 0;

    while (*start != '\0') {
        if (*start == '/') {
            if (len > 0) {
                memcpy(components[*count], start - len, len);
                components[*count][len] = '\0';
                (*count)++;
            }
            len = 0;
        } else {
            len++;
        }
        start++;
    }

    if (len > 0) {
        memcpy(components[*count], start - len, len);
        components[*count][len] = '\0';
        (*count)++;
    }
}

void convert_from_normal_to_fucking_insane(const char* path, char* formatted_path) {
    char components[32][12]; 
    int component_count = 0;
    split_path(path, components, &component_count);

    formatted_path[0] = '\0'; 

    for (int i = 0; i < component_count; i++) {
        char padded_component[12] = "           "; 

        for (int j = 0; j < 11 && components[i][j] != '\0'; j++) {
            padded_component[j] = caps_char(components[i][j]);
        }
        append_string(formatted_path, "/"); 
        append_string(formatted_path, padded_component);
    }
}


static void fill_char_arr(const char* source, char* dest, int start, int length) {
    if (length > 0) {
        memcpy(dest, source + start, length);
        dest[length] = '\0'; 
    }
}



// ACTUAL -----------------------------------------------------------------------------------------

// Find a free cluster in the FAT
uint32_t Find_Free_Cluster() {
    uint32_t cluster = 2; // Start searching from cluster 2
    char buffer[512];
    
    while (cluster < 0x0FFFFFF8) {
        uint32_t fat_sector = 2048 + bpb->reserved_sector_count + (cluster / 128);
        ATA_Read28_PM_INTO_BUFFER(fat_sector, 512, buffer);
        
        uint32_t fat_entry = ((uint32_t*)buffer)[cluster % 128];
        
        if (fat_entry == 0) {
            // Free cluster found
            return cluster;
        }
        
        cluster++;
    }

    return 0xFFFFFFFF; // No free clusters
}

// Create a directory entry in the given directory cluster
bool Create_Directory_Entry(uint32_t dir_cluster, const char* name, uint8_t attr, uint32_t first_cluster) {
    char buffer[512];
    FAT32_DirectoryEntry* entry;
    uint32_t sector;
    
    for (int i = 0; i < bpb->sectors_per_cluster; i++) {
        sector = 2048 + (bpb->reserved_sector_count + (bpb->num_fats * bpb->fat_size_32)) + ((dir_cluster - 2) * bpb->sectors_per_cluster) + i;
        ATA_Read28_PM_INTO_BUFFER(sector, 512, buffer);
        entry = (FAT32_DirectoryEntry*)buffer;

        // Search for an empty slot
        for (int j = 0; j < 16; j++) {
            if (entry->name[0] == 0x00 || entry->name[0] == 0xE5) {
                memset(entry, 0, sizeof(FAT32_DirectoryEntry));
                memcpy(entry->name, name, 11);
                entry->attr = attr;
                entry->first_cluster_hi = (first_cluster >> 16) & 0xFFFF;
                entry->first_cluster_lo = first_cluster & 0xFFFF;
                entry->file_size = 0;

                ATA_Write28_PM(sector, (uint8_t*)buffer, 512);
                return true;
            }
            entry++;
        }
    }

    return false; // No space in the directory
}

// Main function to create a directory at the specified path
void Create_Directory(uint32_t root_dir_sector, const char* dirpath) {
    char components[16][12];
    int component_count = 0;
    split_path(dirpath, components, &component_count);

    if (component_count == 0) {
        sprint("invalid directory path\n", red);
        return;
    }

    uint32_t current_cluster = bpb->root_cluster;

    // Navigate to the parent directory
    for (int i = 0; i < component_count - 1; i++) {
        bool found = false;
        char buffer[512];
        FAT32_DirectoryEntry* entry = NULL;
        uint32_t sector = 2048 + (bpb->reserved_sector_count + (bpb->num_fats * bpb->fat_size_32)) + ((current_cluster - 2) * bpb->sectors_per_cluster);

        for (int j = 0; j < bpb->sectors_per_cluster; j++) {
            ATA_Read28_PM_INTO_BUFFER(sector + j, 512, buffer);
            entry = (FAT32_DirectoryEntry*)buffer;

            for (int k = 0; k < 16; k++) {
                if (entry->name[0] == 0x00) {
                    break; // no more entries
                }
                if (entry->name[0] == 0xE5 || entry->attr == 0x0F) {
                    entry++;
                    continue; // skip deleted or long file name entries
                }

                char name[12];
                memcpy(name, entry->name, 11);
                name[11] = '\0';

                if (string_same(name, components[i]) && (entry->attr & 0x10)) {
                    current_cluster = entry->first_cluster_lo | (entry->first_cluster_hi << 16);
                    found = true;
                    break;
                }
                entry++;
            }

            if (found) break;
        }

        if (!found) {
            sprint("directory not found: ", red);
            sprint(components[i], red);
            sprint("\n", white);
            return;
        }
    }

    // We are now in the parent directory
    uint32_t new_cluster = Find_Free_Cluster();
    if (new_cluster == 0xFFFFFFFF) {
        sprint("No free cluster available\n", red);
        return;
    }

    // Mark the new cluster as used
    char buffer[512];
    uint32_t fat_sector = 2048 + bpb->reserved_sector_count + (new_cluster / 128);
    ATA_Read28_PM_INTO_BUFFER(fat_sector, 512, buffer);
    ((uint32_t*)buffer)[new_cluster % 128] = 0x0FFFFFF8; // End of chain
    ATA_Write28_PM(fat_sector, (uint8_t*)buffer, 512);

    // Create the "." and ".." entries in the new directory
    memset(buffer, 0, 512);
    FAT32_DirectoryEntry* entry = (FAT32_DirectoryEntry*)buffer;

    // Create the "." entry
    memcpy(entry->name, ".          ", 11);
    entry->attr = 0x10; // Directory attribute
    entry->first_cluster_hi = (new_cluster >> 16) & 0xFFFF;
    entry->first_cluster_lo = new_cluster & 0xFFFF;
    entry++;

    // Create the ".." entry
    memcpy(entry->name, "..         ", 11);
    entry->attr = 0x10; // Directory attribute
    entry->first_cluster_hi = (current_cluster >> 16) & 0xFFFF;
    entry->first_cluster_lo = current_cluster & 0xFFFF;

    uint32_t new_sector = 2048 + (bpb->reserved_sector_count + (bpb->num_fats * bpb->fat_size_32)) + ((new_cluster - 2) * bpb->sectors_per_cluster);
    ATA_Write28_PM(new_sector, (uint8_t*)buffer, 512);

    // Add the directory entry in the parent directory
    if (!Create_Directory_Entry(current_cluster, components[component_count - 1], 0x10, new_cluster)) {
        sprint("Failed to create directory entry\n", red);
        return;
    }

    sprint("Directory created successfully: ", green);
    sprint(dirpath, cyan);
    sprint("\n", white);
}


void Read_Directory(uint32_t cluster) {
    char buffer[512];
    FAT32_DirectoryEntry* entry = NULL;
    uint32_t sector = 2048 + (bpb->reserved_sector_count + (bpb->num_fats * bpb->fat_size_32)) + ((cluster - 2) * bpb->sectors_per_cluster);

    for (int i = 0; i < bpb->sectors_per_cluster; i++) {
        ATA_Read28_PM_INTO_BUFFER(sector + i, 512, buffer);
        entry = (FAT32_DirectoryEntry*)buffer;

        for (int j = 0; j < 16; j++) {
            if (entry->name[0] == 0x00) {
                return; 
            }
            if (entry->name[0] == 0xE5 || entry->attr == 0x0F) {
                entry++;
                continue; 
            }

            char name[12];
            memcpy(name, entry->name, 11);
            name[11] = '\0';

            sprint("name: ", yellow);
            sprint(name, cyan);

            sprint(" | size: ", yellow);
            char size_str[12];
            int_to_str(entry->file_size, size_str);
            sprint(size_str, cyan);
            sprint(" bytes", cyan);

            char ext[4];
            memcpy(ext, &entry->name[8], 3);
            ext[3] = '\0';

            sprint(" | file type: ", yellow);
            if(string_same(ext, "   ") && !(entry->attr & 0x10)){ // if file is both an empty extension and not a dir
                sprint("RAW", cyan);
            }
            if(string_same(ext, "RSI")){
                sprint("Rose Icon | ", magenta);
            }
            if(string_same(ext, "RAS")){
                sprint("Rose ASCII art | ", magenta);
            }
            if (entry->attr & 0x10) {
                sprint("<DIR>", cyan);
            } 
            else {
                sprint(ext, cyan);
            }

            sprint("\n", white);
            entry++;
        }
    }
}

void Navigate_To_Directory_Path(const char* path) {
    char components[16][12]; // hardcoding 16 entries
    int component_count = 0;
    split_path(path, components, &component_count);

    uint32_t current_cluster = bpb->root_cluster; 

    for (int i = 0; i < component_count; i++) {
        char buffer[512];
        FAT32_DirectoryEntry* entry = NULL;
        uint32_t sector = 2048 + (bpb->reserved_sector_count + (bpb->num_fats * bpb->fat_size_32)) + ((current_cluster - 2) * bpb->sectors_per_cluster);

        bool found = false;

        for (int j = 0; j < bpb->sectors_per_cluster; j++) {
            ATA_Read28_PM_INTO_BUFFER(sector + j, 512, buffer);
            entry = (FAT32_DirectoryEntry*)buffer;

            for (int k = 0; k < 16; k++) {
                if (entry->name[0] == 0x00) {
                    break; // no entries
                }
                if (entry->name[0] == 0xE5 || entry->attr == 0x0F) {
                    entry++;
                    continue; // deleted or long file name entry? yknow the deal, fuck off
                }

                char name[12];
                memcpy(name, entry->name, 11);
                name[11] = '\0';

                if (string_same(name, components[i]) && (entry->attr & 0x10)) {
                    current_cluster = entry->first_cluster_lo | (entry->first_cluster_hi << 16);
                    found = true;
                    break;
                }
                entry++;
            }

            if (found) break;
        }

        if (!found) {
            sprint("directory not found: ", red);
            sprint(components[i], red);
            sprint("\n", white);
            return;
        }
    }
    Read_Directory(current_cluster);
}

void Read_root_dir(){
    for (int i = 0; i < bpb->sectors_per_cluster; i++) {

        ATA_Read28_PM_INTO_BUFFER(root_dir_sector + i, 512, buffer1);
        
        FAT32_DirectoryEntry* entry = (FAT32_DirectoryEntry*)buffer1;

        // 16 magic number
        for (int j = 0; j < 16; j++) { 
            if (entry->name[0] == 0x00) {
                break; // none files
            }
            if (entry->name[0] == 0xE5) {
                entry++;
                continue; // deleted 
            }
            if (entry->attr == 0x0F) {
                entry++; // if long file name entry, you fuck off 
                continue;
            }

            char name[12];
            memcpy(name, entry->name, 11);
            name[11] = '\0';

            sprint("name: ", yellow);
            sprint(name, cyan);

            sprint(" | size: ", yellow);
            char size_str[12];
            int_to_str(entry->file_size, size_str);
            sprint(size_str, cyan);
            sprint(" bytes", cyan);

            sprint("\n", white);

            entry++;
        }
    }
}

// WRAPPERS--------------------------------------------------------------------------------------

void read_dir(const char* dirname){
    Read_BPB_quiet(0);
    Read_BPB_quiet(0);
    Navigate_To_Directory_Path(dirname);
}

void readfile(const char* filename){
    Read_BPB_quiet(0);
    Read_BPB_quiet(0);
    Read_BPB_quiet(0);
    Read_File_path(root_dir_sector_public, filename);
}
char* readfile_into_buffer(const char* filename){
    Read_BPB_quiet(0);
    Read_BPB_quiet(0);
    Read_BPB_quiet(0);
    Read_File_path_INTO_BUFFER(root_dir_sector_public, filename);
}

void createfile(const char* filename2, const char* ext){
    Read_BPB_quiet(0);
    Create_File_path(root_dir_sector_public, filename2, ext);
}
void createdir(const char* filename4){
    Read_BPB_quiet(0);
    Create_Directory(root_dir_sector_public, filename4);
}

void overwrite(const char* filename3, const char* data, uint32_t size){ // overwrite does not like being in the root directory. Not very behaving
    Read_BPB_quiet(0);
    Read_BPB_quiet(0);
    Read_BPB_quiet(0);
    Overwrite_File_path(root_dir_sector_public, filename3, data, size);
}