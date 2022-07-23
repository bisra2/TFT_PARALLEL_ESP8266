#ifndef _TFT_PARALLEL_ESP8266H_
#define _TFT_PARALLEL_ESP8266H_
#include "Arduino.h"
#include <Adafruit_GFX.h>

/**********************************************************
**
** define here macros for optimisation
**
** (Copy Bodmer sources (TFT_eSPI) with modifications
**
**********************************************************/
#define PIN_LOW(x)  (GPOC = (1 << x))
#define PIN_HIGH(x)  (GPOS = (1 << x))
#define PIN16_LOW() (GP16O &= ~1)
#define PIN16_HIGH() (GP16O |= 1)
#define DC_C digitalWrite(TFT_RS,LOW) //PIN_LOW(TFT_RS)
#define DC_D digitalWrite(TFT_RS,HIGH) //PIN_HIGH(TFT_RS)
#define WR_H PIN_HIGH(TFT_WR)
#define WR_L PIN_LOW(TFT_WR)

// Create a bit set lookup table for data bus - wastes 1kbyte of RAM but speeds things up dramatically
 
#define CONSTRUCTOR_INIT_TFT_DATA_BUS            \
for (int32_t c = 0; c<256; c++)                  \
{                                                \
  xset_mask[c] = 0;                              \
  if ( c & 0x01 ) xset_mask[c] |= (1 << TFT_D0); \
  if ( c & 0x02 ) xset_mask[c] |= (1 << TFT_D1); \
  if ( c & 0x04 ) xset_mask[c] |= (1 << TFT_D2); \
  if ( c & 0x08 ) xset_mask[c] |= (1 << TFT_D3); \
  if ( c & 0x10 ) xset_mask[c] |= (1 << TFT_D4); \
  if ( c & 0x20 ) xset_mask[c] |= (1 << TFT_D5); \
  if ( c & 0x40 ) xset_mask[c] |= (1 << TFT_D6); \
  if ( c & 0x80 ) xset_mask[c] |= (1 << TFT_D7); \
}                                                \

// Mask for the 8 data bits to set pin directions
#define dir_mask ((1 << TFT_D0) | (1 << TFT_D1) | (1 << TFT_D2) | (1 << TFT_D3) | (1 << TFT_D4) | (1 << TFT_D5) | (1 << TFT_D6) | (1 << TFT_D7))

// Data bits and the write line are cleared to 0 in one step
#define clr_mask (dir_mask | (1 << TFT_WR))

// A lookup table is used to set the different bit patterns, this uses 1kByte of RAM
#define set_mask(C) xset_mask[C] // 63fps Sprite rendering test 33% faster, graphicstest only 1.8% faster than shifting in real time

// Write 8 bits to TFT
#define tft_Write_8(C)  GPOC = clr_mask; GPOS = set_mask((uint8_t)(C)); WR_H

// Write 16 bits to TFT
#define tft_Write_16(C) GPOC = clr_mask; GPOS = set_mask((uint8_t)((C) >> 8)); WR_H; \
                        GPOC = clr_mask; GPOS = set_mask((uint8_t)((C) >> 0)); WR_H

// 16 bit write with swapped bytes
#define tft_Write_16S(C) GPOC = clr_mask; GPOS = set_mask((uint8_t) ((C) >> 0)); WR_H; \
                         GPOC = clr_mask; GPOS = set_mask((uint8_t) ((C) >> 8)); WR_H

// Write 32 bits to TFT
#define tft_Write_32(C) GPOC = clr_mask; GPOS = set_mask((uint8_t) ((C) >> 24)); WR_H; \
                        GPOC = clr_mask; GPOS = set_mask((uint8_t) ((C) >> 16)); WR_H; \
                        GPOC = clr_mask; GPOS = set_mask((uint8_t) ((C) >>  8)); WR_H; \
                        GPOC = clr_mask; GPOS = set_mask((uint8_t) ((C) >>  0)); WR_H

// Write two concatenated 16 bit values to TFT - used by setAddrWindow
#define tft_Write_32C(C,D) GPOC = clr_mask; GPOS = set_mask((uint8_t) ((C) >> 8)); WR_H; \
                           GPOC = clr_mask; GPOS = set_mask((uint8_t) ((C) >> 0)); WR_H; \
                           GPOC = clr_mask; GPOS = set_mask((uint8_t) ((D) >> 8)); WR_H; \
                           GPOC = clr_mask; GPOS = set_mask((uint8_t) ((D) >> 0)); WR_H

// Write 16 bit value twice to TFT 
#define tft_Write_32D(C) GPOC = clr_mask; GPOS = set_mask((uint8_t) ((C) >> 8)); WR_H; \
                         GPOC = clr_mask; GPOS = set_mask((uint8_t) ((C) >> 0)); WR_H; \
                         GPOC = clr_mask; GPOS = set_mask((uint8_t) ((C) >> 8)); WR_H; \
                         GPOC = clr_mask; GPOS = set_mask((uint8_t) ((C) >> 0)); WR_H


#define CD_COMMAND   GPOC=(1<<TFT_RS);
#define CD_DATA      GPOS=(1<<TFT_RS);

#define CS_ACTIVE    GPOC = ((uint32_t)1 << TFT_CS);
#define CS_IDLE      GPOS = ((uint32_t)1 << TFT_CS);     

#define swap(a, b) { int16_t t = a; a = b; b = t; }
//====================================================================================================

////////////////////////////////////////////////////////////////////////////////////////
// Define the parallel bus interface chip pin drive code
////////////////////////////////////////////////////////////////////////////////////////
#define TFT_RD //connected permanently to VCC
#define TFT_WR   14 //(D5)
#define TFT_RS   15 //(D8)
#define TFT_CS   16 //(D0)
#define TFT_D0   0  //(D3)
#define TFT_D1   1  //(Tx)
#define TFT_D2   2  //(D4)
#define TFT_D3   3  //(Rx)
#define TFT_D4   4  //(D2)
#define TFT_D5   5  //(D1)
#define TFT_D6   12 //(D6)
#define TFT_D7   13 //(D7)

//TFT_RST wired to RST ESP8266

/***********************************
**    Choice of driver 
**
** Remove comment for correct driver
**
***********************************/
//#define ILI9488
//#define HX8357D
#define R61581
//#define RM68140

#if defined (ILI9488)
  #include "TFT_Drivers/ILI9488_Defines.h"
#elif defined (HX8357D)
  #include "TFT_Drivers/HX8357D_Defines.h"
#elif defined (R61581)
  #include "TFT_Drivers/R61581_Defines.h"
#elif defined (RM68140)
  #include "TFT_Drivers/RM68140_Defines.h"
#endif

#define TFTWIDTH  320
#define TFTHEIGHT 480

// Color definitions
#define BLACK       0x0000      /*   0,   0,   0 */
#define NAVY        0x000F      /*   0,   0, 128 */
#define DARKGREEN   0x03E0      /*   0, 128,   0 */
#define DARKCYAN    0x03EF      /*   0, 128, 128 */
#define MAROON      0x7800      /* 128,   0,   0 */
#define PURPLE      0x780F      /* 128,   0, 128 */
#define OLIVE       0x7BE0      /* 128, 128,   0 */
#define LIGHTGREY   0xC618      /* 192, 192, 192 */
#define DARKGREY    0x7BEF      /* 128, 128, 128 */
#define BLUE        0x001F      /*   0,   0, 255 */
#define GREEN       0x07E0      /*   0, 255,   0 */
#define CYAN        0x07FF      /*   0, 255, 255 */
#define RED         0xF800      /* 255,   0,   0 */
#define MAGENTA     0xF81F      /* 255,   0, 255 */
#define YELLOW      0xFFE0      /* 255, 255,   0 */
#define WHITE       0xFFFF      /* 255, 255, 255 */
#define ORANGE      0xFD20      /* 255, 165,   0 */
#define GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define PINK        0xF81F



/*****************************************************************************
 ** 
 **                    TFT_PARALLEL_ESP8266 CLASS
 **                    
 *****************************************************************************/
class TFT_PARALLEL_ESP8266 : public Adafruit_GFX {
  public:

    TFT_PARALLEL_ESP8266(void);

    void begin() ;
    void initDisplay();
    void setAddrWindow(uint16_t, uint16_t, uint16_t, uint16_t) ;
    void pushColor(uint16_t) ;
    void fillScreen(uint16_t) ;
    void clearScreen();
    void drawLine(int16_t, int16_t,int16_t, int16_t, uint16_t) ;
    void drawPixel(int16_t, int16_t, uint16_t) ;
    void drawFastVLine(int16_t, int16_t, int16_t, uint16_t) ;
    void drawFastHLine(int16_t, int16_t, int16_t, uint16_t) ;
    void fillRect(int16_t, int16_t, int16_t, int16_t, uint16_t) ;
    void setRotation(uint8_t) ;
    void invertDisplay(boolean) ;
    uint16_t color565(uint8_t, uint8_t, uint8_t) ;
    void reset();
    void enable(bool) ;


   private:
    uint8_t tabcolor;
    void writecommand(uint8_t) ;
    void writedata(uint8_t) ;
    void writedata16(uint16_t) ;
    void writedata16(uint16_t, uint32_t) ;
};


#endif //endif of the header file
