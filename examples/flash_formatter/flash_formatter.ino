#include <Arduino.h>

extern "C"
{
#include <hardware/sync.h>
#include <hardware/flash.h>
};

#include <rp2040_flash_datalogger.h>

void setup()
{

  Serial.begin(9600);
  while (!Serial)
    ;

  FlashWriter flash_writer = FlashWriter();
  Serial.print("First non-empty sector: ");
  Serial.println(flash_writer.current_flash_target_offset);

  Serial.println("Erasing everything, type \"OK\" in caps without quotes to authorize");
  while (true)
  {
    if (Serial.available() > 0)
    {
      // Read the incoming string
      String input = Serial.readStringUntil('\n');

      // Remove any trailing newline or carriage return characters
      input.trim();

      // Check if the input is "OK"
      if (input == "OK")
      {
        // Authorized action
        Serial.println("Authorization successful.");
        break;
      }
      else
      {
        // Unauthorized or invalid input
        Serial.println("Invalid input. Please type 'OK' to authorize.");
      }
    }
  }

  // Erase everything!!
  for (int sector_offset = SECTOR_START_OFFSET; sector_offset <= flash_writer.current_flash_target_offset; sector_offset += FLASH_SECTOR_SIZE)
  {
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(sector_offset, FLASH_SECTOR_SIZE); // Erasing the first sector
    restore_interrupts(ints);
    Serial.print("Cleared ");
    Serial.println(sector_offset);
  }

  Serial.println("Done!");
}

void loop()
{
  // put your main code here, to run repeatedly:
}
