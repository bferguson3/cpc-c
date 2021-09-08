// CPC System Routines
#define CHANGE_VIDEO_MODE 0xbc0e 
#define SCR_SET_INK 0xbc32
#define VRAM_BASE 0xc000

#define BG_PEN 0
#define FG_PEN 1

// Typedefs
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned int u16;
typedef signed int s16;
typedef volatile unsigned char vu8;
typedef volatile signed char vs8;
typedef volatile unsigned int vu16;
typedef volatile signed int vs16;

// Routines
void WaitForKey();

void SetVideoMode(u8 mode);
void SetPalette(u8 pen, u8 c);

void SetPixel_m0(u8 x, u8 y, u8 val);
void SetPixel_m1(u16 x, u8 y, u8 val);
void SetPixel_m2(u16 x, u8 y, u8 val);

