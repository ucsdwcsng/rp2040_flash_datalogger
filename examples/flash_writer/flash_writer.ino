#include <Arduino.h>

extern "C"
{
#include <hardware/sync.h>
#include <hardware/flash.h>
};

#include <rp2040_flash_datalogger.h>

void write_job(FlashWriter *flash_writer)
{

  unsigned long startTime = micros(); // Start timing
  for (int i = 0; i < 10; i++)
  {
    flash_writer->print("10");
    flash_writer->print("10");
    flash_writer->print("10");
    flash_writer->print(", ");
    for (int j = 0; j < 20; j++)
    {
      flash_writer->printf("%02X", j);
    }
    flash_writer->print(", ");
    flash_writer->print("-64");
    flash_writer->print(", ");
    flash_writer->print("10");
    flash_writer->print(", ");
    flash_writer->print("0");
    flash_writer->print("\n");
  }
  unsigned long endTime = micros(); // End timing

  unsigned long elapsedTime = endTime - startTime;
  Serial.println("Time taken to write 10 logs: " + String(elapsedTime) + " microseconds");
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("FLASH_PAGE_SIZE = " + String(FLASH_PAGE_SIZE, DEC));
  Serial.println("FLASH_SECTOR_SIZE = " + String(FLASH_SECTOR_SIZE, DEC));
  Serial.println("FLASH_BLOCK_SIZE = " + String(FLASH_BLOCK_SIZE, DEC));
  Serial.println("PICO_FLASH_SIZE_BYTES = " + String(PICO_FLASH_SIZE_BYTES, DEC));
  Serial.println("XIP_BASE = 0x" + String(XIP_BASE, HEX));

  FlashWriter flash_writer = FlashWriter();
  FlashReader flash_reader = FlashReader();

  // Write some sample data and time performance
  write_job(&flash_writer);

  // Display current memory pointer and erased sectors
  Serial.print("Current sector: ");
  Serial.println(flash_writer.current_flash_target_offset);
  Serial.print("Current page: ");
  Serial.println(flash_writer.current_page);
  Serial.print("I have erased: ");
  Serial.println(flash_writer.erased_sector_count);

  // Read till first empty page
  while (flash_reader.current_sector <= flash_reader.max_sector)
  {
    String line = flash_reader.read();
    Serial.print(line);
  }

  Serial.println("Done!");
}

void loop()
{
  // put your main code here, to run repeatedly:
}
