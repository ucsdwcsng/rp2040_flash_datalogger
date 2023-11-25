#include <Arduino.h>

extern "C" {
#include <hardware/sync.h>
#include <hardware/flash.h>
};
//FLASH_PAGE_SIZE = 256
//FLASH_SECTOR_SIZE = 4096
//FLASH_BLOCK_SIZE = 65536
//PICO_FLASH_SIZE_BYTES = 8388608
//XIP_BASE = 0x10000000

#include <rawflash_datalogger.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) ;
  Serial.println("FLASH_PAGE_SIZE = " + String(FLASH_PAGE_SIZE, DEC));
  Serial.println("FLASH_SECTOR_SIZE = " + String(FLASH_SECTOR_SIZE, DEC));
  Serial.println("FLASH_BLOCK_SIZE = " + String(FLASH_BLOCK_SIZE, DEC));
  Serial.println("PICO_FLASH_SIZE_BYTES = " + String(PICO_FLASH_SIZE_BYTES, DEC));
  Serial.println("XIP_BASE = 0x" + String(XIP_BASE, HEX));

  FlashWriter writer_flash = FlashWriter();
  Serial.print("I erased: ");
  Serial.println(writer_flash.erased_sectors);
  Serial.println("--- page and target offset ---");

  unsigned long startTime = micros(); // Start timing
  for(int i=0;i<10;i++){
    writer_flash.write("kjasdhf, hnlakjsyh, poqi24p58oqyu, kjdsahfasldf");
    writer_flash.flush();
  }
  unsigned long endTime = micros(); // End timing

  unsigned long elapsedTime = endTime - startTime;
  Serial.println("Time taken to write: " + String(elapsedTime) + " microseconds");

  Serial.println(writer_flash.current_page);
  Serial.println(writer_flash.current_flash_target_offset);

  FlashReader reader_flash = FlashReader(SECTOR_START_OFFSET+FLASH_SECTOR_SIZE);

  while (reader_flash.current_sector <= reader_flash.max_sector){
    String haha = reader_flash.read();
    Serial.println(haha);
    Serial.println("---");
  }
  


  // Erase everything!!
  int sector_offset = 0;

//  for (sector_offset=0; sector_offset<=FLASH_TARGET_OFFSET_MAX; sector_offset += FLASH_SECTOR_SIZE){
//    uint32_t ints = save_and_disable_interrupts();
//    flash_range_erase(sector_offset, FLASH_SECTOR_SIZE); // Erasing the first sector
//    restore_interrupts(ints);
//    Serial.print("Cleared");
//    Serial.println(sector_offset);
//  }
  



  Serial.println("Done!");

  

}

void loop() {
  // put your main code here, to run repeatedly:

}
