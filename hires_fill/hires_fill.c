
#include <stdint.h>
#include <string.h>

#define ROWS                192	// number of scanlines
#define ROW_FIRST           0
#define ROW_LAST            ROWS - 1
#define ROW_SECOND_LAST     ROW_LAST - 1
#define COLUMNS             40            // Number of columns/bytes per row
#define COLUMN_FIRST        0
#define COLUMN_LAST         COLUMNS - 1
#define WHITE               0x7F           // White hires byte
#define BLACK               0x00
#define SPRITE_BUFFER_SIZE  45
#define HGR1SCRN            0x2000
#define HGR_SCRN_LEN        0x2000

#define HGR1SCRN_PAGE       0x20
#define HGRSCRN_LENGTH      0x20           // number of pages
#define TXTCLR              0xC050         // graphics mode
#define HIRES               0xC057         // hires mode

#define CASSETTE_OUT        0xC020

#define STROBE(x) asm volatile ("bit %0" :: "i"(x) : "memory");

#define BYTE_LO_GET(x)      *((uint8_t *) & x)
#define BYTE_HI_GET(x)      *(((uint8_t *) & x) + 1)

// Addresses
enum Zero_page
{
    DATA1 = 0xE0,
    DATA2,
    DATA3,
    DATA4,
    DATA5,

    ADDR1L,
    ADDR1H,
    ADDR2L,
    ADDR2H,

    LKLOL,
    LKLOH,
    LKHIL,
    LHHIH,

    SBUFRL,
    SBUFRH,

    SPRITEL,
    SPRITEH,
};

// low byte is used for some instructions, alias so that it maps to assembly version
#define LKLO                LKLOL
#define LKHI                LKHIL
#define SPRITE              SPRITEL
#define SBUFR               SBUFRL // sprint buffer location

// pointers to zero page memory
uint8_t volatile *const DATA1_P = (uint8_t *)DATA1;
uint8_t volatile *const DATA2_P = (uint8_t*)DATA2;
uint8_t volatile *const DATA3_P = (uint8_t*)DATA3;
uint8_t volatile *const DATA4_P = (uint8_t*)DATA4;
uint8_t volatile *const DATA5_P = (uint8_t*)DATA5;

uint8_t volatile *const ADDR1L_P = (uint8_t*)ADDR1L;
uint8_t volatile *const ADDR1H_P = (uint8_t*)ADDR1H;
uint8_t volatile *const ADDR2L_P = (uint8_t*)ADDR2L;
uint8_t volatile *const ADDR2H_P = (uint8_t*)ADDR2H;

uint8_t volatile *const LKLOL_P = (uint8_t*)LKLOL;
uint8_t volatile *const LKLOH_P = (uint8_t*)LKLOH;
uint8_t volatile *const LKHIL_P = (uint8_t*)LKHIL;
uint8_t volatile *const LKHIH_P = (uint8_t*)LHHIH;

uint8_t volatile *const SBUFRL_P = (uint8_t*)SBUFRL;
uint8_t volatile *const SBUFRH_P = (uint8_t*)SBUFRH;
uint8_t volatile *const SPRITEL_P = (uint8_t*)SPRITEL;
uint8_t volatile *const SPRITEH_P = (uint8_t*)SPRITEH;


static const uint8_t lklo[256] =
{
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80,
    0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8,
    0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8,
    0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8,
    0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0X28, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8, 0XA8,
    0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0,
    0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0,
    0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0,
    0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0X50, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0, 0XD0,

    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
};

static const uint8_t lkhi[256] =
{
    0X20, 0X24, 0X28, 0X2C, 0X30, 0X34, 0X38, 0X3C, 0X20, 0X24, 0X28, 0X2C, 0X30, 0X34, 0X38, 0X3C,
    0X21, 0X25, 0X29, 0X2D, 0X31, 0X35, 0X39, 0X3D, 0X21, 0X25, 0X29, 0X2D, 0X31, 0X35, 0X39, 0X3D,
    0X22, 0X26, 0X2A, 0X2E, 0X32, 0X36, 0X3A, 0X3E, 0X22, 0X26, 0X2A, 0X2E, 0X32, 0X36, 0X3A, 0X3E,
    0X23, 0X27, 0X2B, 0X2F, 0X33, 0X37, 0X3B, 0X3F, 0X23, 0X27, 0X2B, 0X2F, 0X33, 0X37, 0X3B, 0X3F,
    0X20, 0X24, 0X28, 0X2C, 0X30, 0X34, 0X38, 0X3C, 0X20, 0X24, 0X28, 0X2C, 0X30, 0X34, 0X38, 0X3C,
    0X21, 0X25, 0X29, 0X2D, 0X31, 0X35, 0X39, 0X3D, 0X21, 0X25, 0X29, 0X2D, 0X31, 0X35, 0X39, 0X3D,
    0X22, 0X26, 0X2A, 0X2E, 0X32, 0X36, 0X3A, 0X3E, 0X22, 0X26, 0X2A, 0X2E, 0X32, 0X36, 0X3A, 0X3E,
    0X23, 0X27, 0X2B, 0X2F, 0X33, 0X37, 0X3B, 0X3F, 0X23, 0X27, 0X2B, 0X2F, 0X33, 0X37, 0X3B, 0X3F,
    0X20, 0X24, 0X28, 0X2C, 0X30, 0X34, 0X38, 0X3C, 0X20, 0X24, 0X28, 0X2C, 0X30, 0X34, 0X38, 0X3C,
    0X21, 0X25, 0X29, 0X2D, 0X31, 0X35, 0X39, 0X3D, 0X21, 0X25, 0X29, 0X2D, 0X31, 0X35, 0X39, 0X3D,
    0X22, 0X26, 0X2A, 0X2E, 0X32, 0X36, 0X3A, 0X3E, 0X22, 0X26, 0X2A, 0X2E, 0X32, 0X36, 0X3A, 0X3E,
    0X23, 0X27, 0X2B, 0X2F, 0X33, 0X37, 0X3B, 0X3F, 0X23, 0X27, 0X2B, 0X2F, 0X33, 0X37, 0X3B, 0X3F,

    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
};

// divide-by 7 table
const uint8_t DIV7[256] =
{
     0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4,
     4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 9,
     9, 9, 9, 9, 9, 9,10,10,10,10,10,10,10,11,11,11,11,11,11,11,12,12,12,12,12,12,12,13,13,13,13,13,
    13,13,14,14,14,14,14,14,14,15,15,15,15,15,15,15,16,16,16,16,16,16,16,17,17,17,17,17,17,17,18,18,
    18,18,18,18,18,19,19,19,19,19,19,19,20,20,20,20,20,20,20,21,21,21,21,21,21,21,22,22,22,22,22,22,
    22,23,23,23,23,23,23,23,24,24,24,24,24,24,24,25,25,25,25,25,25,25,26,26,26,26,26,26,26,27,27,27,
    27,27,27,27,28,28,28,28,28,28,28,29,29,29,29,29,29,29,30,30,30,30,30,30,30,31,31,31,31,31,31,31,
    32,32,32,32,32,32,32,33,33,33,33,33,33,33,34,34,34,34,34,34,34,35,35,35,35,35,35,35,36,36,36,36,
};

// modulo-by-7 table
const uint8_t MOD7[256] =
{
    0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3,
    4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0,
    1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4,
    5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1,
    2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5,
    6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2,
    3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6,
    0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3,
};

#define SPRITE_XH_CALC(x) (DIV7[x])
#define SPRITE_XL_CALC(x) (MOD7[x])

static const uint8_t sprites[] =
{
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,

    0x88, 0x80, 0x88, 0x80, 0x94, 0x80, 0x94, 0x80, 0x2A, 0x00, 0x2A, 0x00, 0x55, 0x00, 0x55, 0x00,
    0x90, 0x80, 0x90, 0x80, 0xA8, 0x80, 0xA8, 0x80, 0x54, 0x00, 0x54, 0x00, 0x2A, 0x01, 0x2A, 0x01,
    0xA0, 0x80, 0xA0, 0x80, 0xD0, 0x80, 0xD0, 0x80, 0x28, 0x01, 0x28, 0x01, 0x54, 0x02, 0x54, 0x02,
    0xC0, 0x80, 0xC0, 0x80, 0xA0, 0x81, 0xA0, 0x81, 0x50, 0x02, 0x50, 0x02, 0x28, 0x05, 0x28, 0x05,
    0x80, 0x81, 0x80, 0x81, 0xC0, 0x82, 0xC0, 0x82, 0x20, 0x05, 0x20, 0x05, 0x50, 0x0A, 0x50, 0x0A,
    0x80, 0x82, 0x80, 0x82, 0x80, 0x85, 0x80, 0x85, 0x40, 0x0A, 0x40, 0x0A, 0x20, 0x15, 0x20, 0x15,
    0x80, 0x84, 0x80, 0x84, 0x80, 0x8A, 0x80, 0x8A, 0x00, 0x15, 0x00, 0x15, 0x40, 0x2A, 0x40, 0x2A,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x88, 0x80, 0x88, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x90, 0x80, 0x90, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xA0, 0x80, 0xA0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xC0, 0x80, 0xC0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x80, 0x81, 0x80, 0x81, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x80, 0x82, 0x80, 0x82, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x80, 0x84, 0x80, 0x84, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x88, 0x80, 0x88, 0x80, 0x14, 0x00, 0x14, 0x00, 0x2A, 0x00, 0x2A, 0x00, 0xD5, 0x80, 0xD5, 0x80,
    0x90, 0x80, 0x90, 0x80, 0x28, 0x00, 0x28, 0x00, 0x54, 0x00, 0x54, 0x00, 0xAA, 0x81, 0xAA, 0x81,
    0xA0, 0x80, 0xA0, 0x80, 0x50, 0x00, 0x50, 0x00, 0x28, 0x01, 0x28, 0x01, 0xD4, 0x82, 0xD4, 0x82,
    0xC0, 0x80, 0xC0, 0x80, 0x20, 0x01, 0x20, 0x01, 0x50, 0x02, 0x50, 0x02, 0xA8, 0x85, 0xA8, 0x85,
    0x80, 0x81, 0x80, 0x81, 0x40, 0x02, 0x40, 0x02, 0x20, 0x05, 0x20, 0x05, 0xD0, 0x8A, 0xD0, 0x8A,
    0x80, 0x82, 0x80, 0x82, 0x00, 0x05, 0x00, 0x05, 0x40, 0x0A, 0x40, 0x0A, 0xA0, 0x95, 0xA0, 0x95,
    0x80, 0x84, 0x80, 0x84, 0x00, 0x0A, 0x00, 0x0A, 0x00, 0x15, 0x00, 0x15, 0xC0, 0xAA, 0xC0, 0xAA,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static uint8_t sprite_x1;
static uint8_t sprite_y1;
static uint8_t sprite_x2;
static uint8_t sprite_y2;

static uint8_t sprite_xl;
static uint8_t sprite_xh;

static uint8_t sprite_no_jump;

static uint8_t sprite_buffer[SPRITE_BUFFER_SIZE];


union test
{
    uint16_t *value;
    uint8_t b[2];
};

union test t;

static void pointers_init(void)
{
    *LKLOL_P = (uint8_t)((uint16_t)lklo);
    *LKLOH_P = (uint8_t)(((uint16_t)lklo)>> 8);
    *LKHIL_P = (uint8_t)((uint16_t)lkhi);
    *LKHIH_P = (uint8_t)(((uint16_t)lkhi) >> 8);
    *SBUFRL_P = (uint8_t)((uint16_t)sprite_buffer);
    *SBUFRH_P = (uint8_t)(((uint16_t)sprite_buffer)>> 8);
}

static inline void pageset(uint8_t page, uint8_t value, uint8_t length)
{
    // this method takes 114ms
    *DATA1_P = value;
    *DATA2_P = length;
    *ADDR1H_P = page;
    *ADDR1L_P = 0x00;

    // init registers with memory
    asm volatile ("lda %0" :: "i"(DATA1) : "a");    // value to fill page(s) with
    asm volatile ("ldx %0" :: "i"(DATA2) : "x");    // number of pages
    asm volatile ("ldy %0" :: "i"(ADDR1L) : "y"); // address two bytes

    // nested loops
    asm volatile ("1: sta (%0),y" :: "i"(ADDR1L) : "a", "memory");
    asm volatile ("iny" ::: "y");
    asm volatile ("bne 1b");
    asm volatile ("inc %0" :: "i"(ADDR1H) : "memory");
    asm volatile ("dex"::: "x");
    asm volatile ("bne 1b");
}

static inline void hline(uint8_t column, uint8_t row, uint8_t length, uint8_t pixels)
{
    // 1.7ms
    *DATA1_P = pixels;
    *DATA2_P = column;
    *DATA3_P = column + length;
    *ADDR1L_P = lklo[row];
    *ADDR1H_P = lkhi[row];

    // init
    asm volatile ("ldy %0" :: "i"(DATA3) : "y");

    // loop
    asm volatile ("1: dey" ::: "y");
    asm volatile ("lda (%0),y" :: "i"(ADDR1L) : "a");
    asm volatile ("eor %0" :: "i"(DATA1) : "memory");
    asm volatile ("sta (%0),y" :: "i"(ADDR1L) : "memory");
    asm volatile ("cpy %0" :: "i"(DATA2));
    asm volatile ("bne 1b");
}

static inline void vline(uint8_t column, uint8_t row, uint8_t length, uint8_t pixels)
{
    // 11.2ms
    *DATA1_P = pixels;
    *DATA2_P = column;
    *DATA3_P = row;
    *DATA4_P = row + length;

    // init
    asm volatile ("ldx %0" :: "i"(DATA4) : "x");            // Start at second-to-last row

    // loop
    asm volatile ("1: dex" ::: "x");
    asm volatile ("txa" ::: "a");                           // row to a
    asm volatile ("tay" ::: "y");                           // row to y
    asm volatile ("lda (%0),y" :: "i"(LKLO) : "a");         // Get the row address
    asm volatile ("sta %0" :: "i"(ADDR1L) : "memory");
    asm volatile ("lda (%0),y" :: "i"(LKHI) : "a");
    asm volatile ("sta %0" :: "i"(ADDR1H) : "memory");
    asm volatile ("ldy %0" :: "i"(DATA2) : "y");            // column
    asm volatile ("lda (%0),y" :: "i"(ADDR1L) : "a");
    asm volatile ("eor %0" :: "i"(DATA1) : "memory");
    asm volatile ("sta (%0),y" :: "i"(ADDR1L) : "memory");
    asm volatile ("cpx %0" :: "i"(DATA3));
    asm volatile ("bne 1b");
}

static inline void vline_c(uint8_t value)
{
    // 434ms
    STROBE(CASSETTE_OUT);
    for (uint8_t y = 0; y < 192; y++)
    {
        t.b[1] = lkhi[y];
        t.b[0] = lklo[y];
        for (uint8_t x = 0; x < 40; x++)
        {
            *t.value  = value;
            t.b[0]++;
        }
    }
}

static inline void hclear(void)
{
    pageset(HGR1SCRN_PAGE, BLACK, HGRSCRN_LENGTH);
    STROBE(HIRES);
    STROBE(TXTCLR);
}

static inline void hbox(void)
{
    hline(COLUMN_FIRST, ROW_FIRST, COLUMNS, WHITE);
    hline(COLUMN_FIRST, ROW_LAST, COLUMNS, WHITE);
    // vline(20, 20, 25, 0x03);
    // vline(COLUMN_LAST, 0, 192, 0x60);
}

int main(void)
{
    pointers_init();
    hclear();
    hbox();



    while(1)
    {
        vline_c(BLACK);
        vline_c(WHITE);
    }
    return 0;
}
