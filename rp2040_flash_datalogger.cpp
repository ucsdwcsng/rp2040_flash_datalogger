#include "rp2040_flash_datalogger.h"
#include <cstdarg> // for va_list

FlashWriter::FlashWriter()
{
    erased_sector_count = 0;
    initializeFlash();
}

void FlashWriter::initializeFlash()
{
    seekFirstEmptySector();
}

void FlashWriter::seekFirstEmptySector()
{
    bool isSectorEmpty;

    for (current_flash_target_offset = SECTOR_START_OFFSET;
         current_flash_target_offset <= FLASH_TARGET_OFFSET_MAX;
         current_flash_target_offset += FLASH_SECTOR_SIZE)
    {

        isSectorEmpty = true; // Assume the sector is empty initially
        current_page = 0;
        seekFirstEmptyPage();

        // Check if an empty page was found in this sector
        if (current_page != -1)
        {
            break;
        }
    }

    if (current_page == -1)
    {
        // THIS SHOULD NEVER HAPPEN!!
        // All sectors are full, erase the first two sectors
        eraseSector(SECTOR_START_OFFSET);
        eraseSector(SECTOR_START_OFFSET + FLASH_SECTOR_SIZE);
        current_flash_target_offset = SECTOR_START_OFFSET; // Reset to the first sector
        seekFirstEmptyPage();                              // Find the first empty page in the erased sector
    }
}

void FlashWriter::seekFirstEmptyPage()
{
    int *p;
    int addr;
    bool found = false;

    for (current_page = 0; current_page < FLASH_PAGES_PER_SECTOR; current_page++)
    {
        addr = findPageAddress(current_page);
        p = (int *)addr;

        // Check if the first int in the page is -1 (indicating it's unwritten)
        if (*p == -1)
        {
            found = true;
            break; // Exit the loop once an empty page is found
        }
    }

    if (!found)
    {
        current_page = -1; // Setting to -1 as an indicator of no empty page found
    }
}

void FlashWriter::eraseSector(int sector_offset)
{
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(sector_offset, FLASH_SECTOR_SIZE); // Erasing the first sector
    restore_interrupts(ints);

    erased_sector_count++;
}

void FlashWriter::seekNextSector()
{
    current_flash_target_offset += FLASH_SECTOR_SIZE;
    if (current_flash_target_offset > FLASH_TARGET_OFFSET_MAX)
    {
        current_flash_target_offset = SECTOR_START_OFFSET;
    }

    // Erase next sector in anticipation
    if (current_flash_target_offset == FLASH_TARGET_OFFSET_MAX)
    {
        eraseSector(SECTOR_START_OFFSET);
    }
    else
    {
        eraseSector(current_flash_target_offset + FLASH_SECTOR_SIZE);
    }
}

void FlashWriter::seekNextPage()
{
    current_page += 1;
    if (current_page == FLASH_PAGES_PER_SECTOR)
    {
        current_page = 0;
        seekNextSector();
    }
}

int FlashWriter::findPageAddress(int page)
{
    return XIP_BASE + current_flash_target_offset + (page * FLASH_PAGE_SIZE);
}

void FlashWriter::print(String data)
{
    buffer += data; // Append the data to the internal buffer
    if (buffer.length() >= FLASH_PAGE_SIZE || data[data.length() - 1] == '\n')
    {
        flush();
    }
}

void FlashWriter::println(String data)
{
    print(data + "\n");
}

void FlashWriter::printf(const char *format, ...)
{
    char buffer[128]; // Define a suitable buffer size
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // return String(buffer);

    print(String(buffer)); // Use your existing write method
}

void FlashWriter::flush()
{
    // Convert String buffer to int array (considering the buffer size and data type)
    // This conversion needs to be handled according to the data format and size
    memset(buf, 0, FLASH_PAGE_SIZE); // Clear the buffer

    for (unsigned int i = 0; i < buffer.length() && i < FLASH_PAGE_SIZE; i++)
    {
        ((char *)buf)[i] = buffer[i];
    }

    // Write buffer to flash
    uint32_t ints = save_and_disable_interrupts();
    flash_range_program(current_flash_target_offset + (current_page * FLASH_PAGE_SIZE), (uint8_t *)buf, FLASH_PAGE_SIZE);
    restore_interrupts(ints);

    buffer = ""; // Clear the buffer after flushing
    seekNextPage();
}

// take default argument for max sector
FlashReader::FlashReader() : FlashReader(FLASH_TARGET_OFFSET_MAX)
{
}

FlashReader::FlashReader(unsigned int max_sector)
{
    // max sector should be min of max_sector and FLASH_TARGET_OFFSET_MAX
    if (max_sector > FLASH_TARGET_OFFSET_MAX)
    {
        this->max_sector = FLASH_TARGET_OFFSET_MAX;
    }
    else
    {
        this->max_sector = max_sector;
    }

    current_sector = SECTOR_START_OFFSET;
    current_page = 0;
}

String FlashReader::read()
{
    String output;
    int *p;
    int addr;

    if (current_sector > max_sector)
    {
        return output; // Reached the maximum sector limit
    }

    addr = findPageAddress(current_page, current_sector);
    p = (int *)addr;

    // if (*p == -1) then don't read the page
    if (*p != -1)
    {

        // Assuming the data is stored as char array in the ints
        for (unsigned int i = 0; i < FLASH_PAGE_SIZE; i++)
        {
            char ch = ((char *)p)[i];
            if (ch != '\0' && ch != -1)
            {
                output += ch;
            }
            else
            {
                break; // End of data in this page
            }
        }
    }
    else
    {
        max_sector = current_sector; // Reached the end of data
    }

    current_page++;
    if (current_page >= FLASH_PAGES_PER_SECTOR)
    {
        current_page = 0;
        current_sector += FLASH_SECTOR_SIZE;
    }

    return output;
}

int FlashReader::findPageAddress(int page, unsigned int sector_offset)
{
    return XIP_BASE + sector_offset + (page * FLASH_PAGE_SIZE);
}
