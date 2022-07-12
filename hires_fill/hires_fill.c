
#include <stdint.h>

#define WHITE               0x7F           // White hires byte
#define BLACK               0x00

#define HGR1SCRN_PAGE       0x20
#define HGRSCRN_LENGTH      0x20           // number of pages
#define TXTCLR              0xC050         // graphics mode
#define HIRES               0xC057         // hires mode

#define CASSETTE_OUT        0xC020

#define STROBE(x) asm("bit %0" :: "i"(x));

// Addresses
enum Zero_page
{
    DATA1 = 0x26,
    DATA2,
    ADDR1L,
    ADDR1H,
};

// pointers to zero page memory
#define DATA1_P             *((uint8_t*)DATA1)
#define DATA2_P             *((uint8_t*)DATA2)
#define ADDR1L_P            *((uint8_t*)ADDR1L)
#define ADDR1H_P            *((uint8_t*)ADDR1H)


static inline void pageset(uint8_t page, uint8_t value, uint8_t length)
{
    // this method takes 114ms
    DATA1_P = value;
    DATA2_P = length;
    ADDR1H_P = page;
    ADDR1L_P = 0x00;

    // init registers with memory
    asm volatile ("lda %0" :: "i"(DATA1));    // value to fill page(s) with
    asm volatile ("ldx %0" :: "i"(DATA2));    // number of pages
    asm volatile ("ldy %0" :: "i"(ADDR1L)); // address two bytes

    // nested loops
    asm volatile ("1: sta (%0),y" :: "i"(ADDR1L));
    asm volatile ("iny");
    asm volatile ("bne 1b");
    asm volatile ("inc %0" :: "i"(ADDR1H));
    asm volatile ("dex");
    asm volatile ("bne 1b");
}

int main(void)
{
    STROBE(TXTCLR);
    STROBE(HIRES);
    STROBE(CASSETTE_OUT);
    pageset(HGR1SCRN_PAGE, WHITE, HGRSCRN_LENGTH);
    STROBE(CASSETTE_OUT);
    while(1)
    {

    }
    return 0;
}
