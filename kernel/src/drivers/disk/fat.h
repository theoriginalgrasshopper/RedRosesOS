#ifndef __MYOS__FILESYSTEM__FAT_h
#define __MYOS__FILESYSTEM__FAT_h

#include <stdint.h>
#include "ata.h"
#include <stddef.h>

void readfile(const char* filename);
void createdir(const char* filename4);
void Read_root_dir();
void Read_BPB_quiet(uint32_t sector);
void read_dir(const char* dirname);
void Read_BPB(uint32_t sector);
void Create_File_path(uint32_t root_dir_sector, const char* filepath, const char* extension);
void Navigate_To_Directory_Path(const char* path);
void readfile(const char* filename);
void Read_File_path(uint32_t root_dir_sector, const char* filepath);
void convert_from_normal_to_fucking_insane(const char* path, char* formatted_path);
void createfile(const char* filename2, const char* ext);
uint32_t Find_Free_Cluster();
void FAT32_Set_FAT_Entry(uint32_t current_cluster, uint32_t next_cluster);
void overwrite(const char* filename3, const char* data, uint32_t size);
char* Read_File_path_INTO_BUFFER(uint32_t root_dir_sector, const char* filepath);
void split_path(const char* path, char components[][12], int* count);
uint32_t* get_file_size(const char* filename);
char* readfile_into_buffer(const char* filename);

#endif