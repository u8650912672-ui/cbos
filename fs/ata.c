//startup shit

#include "ata.h"
#include "io.h"
#include "vga.h"
#include <stdint.h>
#include <stddef.h>


//ATA in/out puts  primary channel master

#define ATA_PRIMARY_IO 0x1F0
#define ATA_SECONDARY_IO 0x170
#define ATA_DATA 0x00
#define ATA_ERROR 0x01
#define ATA_SECTOR_COUNT 0x02
#define ATA_LBA_LOW 0x03
#define ATA_LBA_MID 0x04
#define ATA_LBA_HIGH 0x05
#define ATA_DRIVE_SELECT 0x06
#define ATA_COMMAND 0x07
#define ATA_STATUS 0x07
#define ATA_CTRL 0x206
#define ATA_ALT_STATUS 0x206

//ata commands 

#define ATA_CMD_READ_PIO 0x20
#define ATA_CMD_WRITE_PIO 0x30
#define ATA_CMD_IDENTIFY 0xEC

//status flags

#define ATA_STATUS_ERR (1 << 0)
#define ATA_STATUS_DRQ (1 << 3)
#define ATA_STATUS_BSY (1 << 7)

//internal states

static int drive_present = 0;

//helpers wait for BSY to clear up

static void ata_wait_bsy(void) {
    while (inb(ATA_PRIMARY_IO + ATA_STATUS) & ATA_STATUS_BSY);
    __asm__ volatile ("pause");
}
static void ata_wait_drq(void) {
    uint8_t status;
    do {
        status = inb(ATA_PRIMARY_IO + ATA_STATUS);
        if (status & ATA_STATUS_ERR) return;  //error handeling :3
    } while (!(status & ATA_STATUS_DRQ));    
}
static int ata_check_error(void) {
    uint8_t err = inb(ATA_PRIMARY_IO + ATA_ERROR);
    if (err) {
        vga_print("ATA error encounterd: ");
        vga_print_int(err);
        vga_print("\n");
        return -1;
    }
    return 0;
}

//select drive (slave and master) and set LBA mode of 28 bits

static void ata_select_drive(uint32_t lba) {
    uint8_t drive_sel = 0xE0; //LBA mode, master drive
    drive_sel |= (lba >> 24) & 0x0F; //upper 4 bits of LBA
    outb(ATA_PRIMARY_IO + ATA_DRIVE_SELECT, drive_sel);
}

//identify drive

static int ata_identify(void) {
    //select master drive
    outb(ATA_PRIMARY_IO + ATA_DRIVE_SELECT, 0xA0); //master with no lba
    ata_wait_bsy();

    //send identify command

    outb(ATA_PRIMARY_IO + ATA_SECTOR_COUNT, 0);
    outb(ATA_PRIMARY_IO + ATA_LBA_LOW, 0);
    outb(ATA_PRIMARY_IO + ATA_LBA_MID, 0);
    outb(ATA_PRIMARY_IO + ATA_LBA_HIGH, 0);
    outb(ATA_PRIMARY_IO + ATA_COMMAND, ATA_CMD_IDENTIFY);

    //check if drive is actually there (would be stupid if its not even there)
    
    uint8_t status = inb(ATA_PRIMARY_IO + ATA_STATUS);
    if (status == 0) return 0; //the drive aint there

    //wait for DRQ or a error

    ata_wait_bsy();
    uint8_t err = inb(ATA_PRIMARY_IO + ATA_ERROR);
    if (err) return 0; //error
    
    //must be 512 bytes but fuck reading it all we care bout is if drive there = 1
    // if (result) return 1; then good to go
    return 1;
}

//public apis TODO : PLEASE 
