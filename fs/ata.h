#ifndef ATA_H
#define ATA_H

#include <stdint.h>

void ata_init();
int ata_read_sectors(uint32_t lba, uint8_t count, uint16_t *buffer);
int ata_write_sectors(uint32_t lba, uint8_t count, const uint16_t *buffer);
int ata_driver_present();
#endif
