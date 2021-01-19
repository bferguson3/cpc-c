#include <stdio.h>

//#define z88dk 1
#ifdef z88dk
//#include <cpc.h>
//#include <fcntl.h>
#endif
// My CPC stuff
#include "bent-cpc.h"

void FillScreen_PixelTest()
{
    volatile u8* p;
    for(u16 i = 0; i < 0x3fff; i++){
        p = (u8*)(VRAM_BASE + i);
        *p = i % 256;
    }
}


int main()
{
    // Set video and color
    SetVideoMode(0);
    SetPalette(BG_PEN, 0);
    SetPalette(FG_PEN, 6);

    // Test
    printf("Hi World");

    // smiley face
    //FillScreen_PixelTest();
    SetPixel_m0(20, 20, 1);
    SetPixel_m0(100, 30, 1);
    for(u16 i = 0; i < 100; i++)
        SetPixel_m0(20 + i, 160, 1);
    for(u16 i = 0; i < 100; i++)
        SetPixel_m0(20 + i, 160, 1);

    // end when key pressed 
    WaitForKey();
    return 0;
}