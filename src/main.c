#include <stdio.h>

//#define z88dk 1
#ifdef z88dk
//#include <cpc.h>
//#include <fcntl.h>
#endif
// My CPC stuff
#include "bent-cpc.h"


void FillScreen_PixelTest();

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
    //SetPixel_m0(20, 20, 1);
    //SetPixel_m0(100, 30, 1);
    //for(u16 i = 0; i < 100; i++)
    //    SetPixel_m0(20 + i, 160, 1);
    //for(u16 i = 0; i < 100; i++)
    //    SetPixel_m0(20 + i, 160, 1);

    // end when key pressed 
    //WaitForKey();
    while(1){
	printf("hi");
	}
    return 0;
}

void FillScreen_PixelTest()
{
    volatile u8* p;
    for(u16 i = 0; i < 0x3fff; i++){
        p = (u8*)(VRAM_BASE + i);
        *p = i % 256;
    }
}

void SetPalette(u8 pen, u8 c)
{
    pen; c;
    __asm
    
    push ix
    ld ix, #0
    add ix, sp

    ld a, 4 (ix)
    ld b, 5 (ix)
    ld c, 5 (ix)
    call #SCR_SET_INK

    pop ix
    ret
    
    __endasm;
}

void SetVideoMode(unsigned char mode) //__z88dk_fastcall __naked //HL
{
    #ifdef z88dk
    EXTERN firmware
    ld a,l
    call firmware 
    defw 0xbc0e
    ret
    #else
    switch (mode) {
        case 0: 
            __asm
                ld a,#0                     ; mode 0
                call #CHANGE_VIDEO_MODE     ;
            __endasm;
            break;
        case 1:
            __asm
                ld a,#1                     ; mode 1
                call #CHANGE_VIDEO_MODE     ;
            __endasm;
            break;
        case 2:
            __asm
                ld a,#2                     ; mode 2
                call #CHANGE_VIDEO_MODE     ;
            __endasm;
            break;
    }
    #endif  
}

void WaitForKey()
{
    __asm
        call #0xBB06
    __endasm; 
}

// mode 0 pixel format:
//  01010101 ; left pixel = 0, right pixel = 1
//  00221133 ; bit color 0-15
void SetPixel_m0(u8 x, u8 y, u8 val)
{
    volatile u8* p = (u8*)VRAM_BASE + (u8)(x / 2);
    u8 v = 0;
    if((x % 2) == 0) 
    {
        v = ((val & 0b1000) >> 2);
        v |= ((val & 0b0100) << 3);
        v |= ((val & 0b0010) << 2);
        v |= ((val & 0b0001) << 7);
    }
    else{
        v = ((val & 0b1000) >> 3);
        v |= ((val & 0b0100) << 2);
        v |= ((val & 0b0010) << 1);
        v |= ((val & 0b0001) << 6);
    }
    u16 yofs = (u8)(y / 8) * 0x50;
    yofs += (u8)(y % 8) * 0x800;
    p += yofs;
    *p |= v;
}

// mode 1 pixel format:
//  01230123 ; pixels 0-3 left to right
//  11110000 ; bit color 0-3
void SetPixel_m1(u16 x, u8 y, u8 val)
{
    volatile u8* p = (u8*)VRAM_BASE + (u8)(x / 4);
    u8 v = 0;
    u8 o = x % 4;
    
    v = ( (val & 0b01) << (3 - o) );
    v |= ( (val & 0b10) << (6 - o) );
    
    u16 yofs = (u8)(y / 8) * 0x50;
    yofs += (u8)(y % 8) * 0x800;
    p += yofs;
    *p |= v;
}

// mode 2 pixel format:
// each bit on/off per pixel
void SetPixel_m2(u16 x, u8 y, u8 val)
{
    volatile u8* p = (u8*)VRAM_BASE + (u8)(x / 8);
    u8 v = 0;
    u8 o = x % 8;
    u16 yofs = (u8)(y / 8) * 0x50;
    yofs += (u8)(y % 8) * 0x800;
    p += yofs;
    if(val > 0)
        *p |= (1 << o);
    else 
        *p ^= (1 << o);
}
