#ifndef rawflash_datalogger_h
#define rawflash_datalogger_h

#include <Arduino.h>

extern "C"
{
#include <hardware/sync.h>
#include <hardware/flash.h>
};

// FLASH_PAGE_SIZE = 256
// FLASH_SECTOR_SIZE = 4096
// FLASH_BLOCK_SIZE = 65536
// PICO_FLASH_SIZE_BYTES = 8388608
// XIP_BASE = 0x10000000
#define FLASH_TARGET_OFFSET_MAX (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)
#define FLASH_PAGES_PER_SECTOR (FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE)
#define SECTOR_START_OFFSET 4194304 // 4MB

class FlashWriter
{
public:
    FlashWriter();
    void write(String data);
    void flush();
    int current_page;
    int erased_sectors;
    int current_flash_target_offset;

private:
    String buffer;
    int buf[FLASH_PAGE_SIZE / sizeof(int)]; // One page buffer of ints

    void initializeFlash();
    void seekFirstEmptySector();
    void seekFirstEmptyPage();
    void seekNextSector();
    void seekNextPage();

    void eraseSector(int sector_offset);
    int findPageAddress(int page);
};

class FlashReader
{
public:
    FlashReader(unsigned int max_sector);
    String read();
    unsigned int current_sector;
    unsigned int max_sector;

private:
    int current_page;
    int findPageAddress(int page, unsigned int sector_offset);
};

#endif