#include "TFT_PARALLEL_ESP8266.h"


  uint32_t xset_mask[256]; // For macros
  
/*****************************************************************************
**                      TFT_PARALLEL_ESP8266 CONSTRUCTOR
/*****************************************************************************/
TFT_PARALLEL_ESP8266::TFT_PARALLEL_ESP8266(void) : Adafruit_GFX(TFTWIDTH, TFTHEIGHT)

{
  CONSTRUCTOR_INIT_TFT_DATA_BUS;
  _width=320; _height=480;
  delay(1000);
  PIN16_LOW(); 
//setResolution(TFT_WIDTH,TFT_HEIGHT);
  pinMode(TFT_D0,OUTPUT);
  pinMode(TFT_D1,OUTPUT);
  pinMode(TFT_D2,OUTPUT);
  pinMode(TFT_D3,OUTPUT);
  pinMode(TFT_D4,OUTPUT);
  pinMode(TFT_D5,OUTPUT);
  pinMode(TFT_D6,OUTPUT);
  pinMode(TFT_D7,OUTPUT);
  pinMode(TFT_RS, OUTPUT);
  pinMode(TFT_WR, OUTPUT);
  pinMode(TFT_CS,OUTPUT);
}

/*****************************************************************************
**                    WRITEBUS8 
******************************************************************************/
/*
void inline writebus(uint8_t d) {
  digitalWrite(TFT_SPI_U4,LOW);
  ////////////////////
  //Gestion du 74HC595
  ////////////////////
  GPOC=(((uint32_t)1<<TFT_SPI_CS));
  SPI.write(d);
  GPOS=((uint32_t)1<<TFT_SPI_CS);

  ////////////////////////
  //Gestion de l'&fficheur
  ////////////////////////
  CS_ACTIVE;              //SS du bus 8080
  GPOC =(1<<TFT_WR);      //Flanc descendant sur WR du bus 8080
  GPOS= (1<<TFT_WR);      //Flanc montant sur WR du bus 8080
  CS_IDLE;                //Dévalide bus 8080
}
*/
void writebus(uint8_t d)
{
  tft_Write_8(d);
}

/*****************************************************************************
**                        WRITEBUS16
** Après les 16 décalages, le poids fort est dans U5 et le poids faible dans U4
** Il faut écrire 16 bits sur le bus 8 bit poids fort en tête puis poids faible.
*****************************************************************************/
void inline writeBus16(uint16_t data) {
 tft_Write_16(data);
}

/*****************************************************************************
**                          WRITEDATA16 
******************************************************************************/
void TFT_PARALLEL_ESP8266::writedata16(uint16_t c)
{
  //CD_DATA;
  DC_D;
  //writebus(c>>8);
  //writebus(c&0xFF);
  //writeBus16(c);
  tft_Write_16(c);
}

/*****************************************************************************
**                          WRITEDATA16 
******************************************************************************/

void TFT_PARALLEL_ESP8266::writedata16(uint16_t color, uint32_t num)
{
  CD_DATA;
  for (uint32_t n=0;n<num;n++) {
    //writebus(color>>8);
    //writebus(color&0xFF);
    //writeBus16(color);
    tft_Write_16(color);
  }
}

/*****************************************************************************
**                          WRITECOMMAND
******************************************************************************/
void TFT_PARALLEL_ESP8266::writecommand(uint8_t c)
{
  //CD_COMMAND;
  DC_C;
  //digitalWrite(TFT_RS,LOW);
    //writebus(c);
  tft_Write_8(c);
   
}
/*****************************************************************************
**                          WRITEDATA
******************************************************************************/
void TFT_PARALLEL_ESP8266::TFT_PARALLEL_ESP8266:: writedata(uint8_t c)
{
  //CD_DATA;
  DC_D;
  //digitalWrite(TFT_RS,HIGH);
  tft_Write_8(c);
}



/*******************************************************************************************************
**                                  BEGIN 
********************************************************************************************************/
void TFT_PARALLEL_ESP8266::begin(void)
{
  #if defined (ILI9488)
    #include "TFT_Drivers/ILI9488_Init.h"
  #elif defined (HX8357D)
    #include "TFT_Drivers/HX8357D_Init.h"
  #elif defined (R61581)
    #include "TFT_Drivers/R61581_Init.h"
  #elif defined (RM68140)
    #include "TFT_Drivers/RM68140_Init.h"
  #endif
  delay(1000);
}

/*****************************************************************************
**                       SET ROTATION 
******************************************************************************/
void TFT_PARALLEL_ESP8266::setRotation(uint8_t m)
{
 #if defined (ILI9488)
  #include "TFT_Drivers/ILI9488_Rotation.h"
#elif defined (HX8357D)
  #include "TFT_Drivers/HX8357D_Rotation.h"
#elif defined (R61581)
  #include "TFT_Drivers/R61581_Rotation.h"
#elif defined (RM68140)
  #include "TFT_Drivers/RM68140_Rotation.h"
#endif

}


/*****************************************************************************
**                          SETADDRWINDOW
******************************************************************************/
void TFT_PARALLEL_ESP8266::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  writecommand(TFT_CASET); // Column addr set
  //writedata16(x0);
  //writedata16(x1);
  DC_D;
  tft_Write_32C(x0,x1);
  
  writecommand(TFT_PASET); // Row addr set
  //writedata16(y0);
  //writedata16(y1);
  DC_D;
  tft_Write_32C(y0,y1);
  writecommand(TFT_RAMWR); // write to RAM
  
}
/*****************************************************************************
**                          PUSHCOLOR
******************************************************************************/
void TFT_PARALLEL_ESP8266::pushColor(uint16_t color)
{
  writedata16(color);
}
/*****************************************************************************
**                          DRAWPIXEL
******************************************************************************/
void TFT_PARALLEL_ESP8266::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;

  setAddrWindow(x, y, x + 1, y + 1);
  //pushColor(color);
  writedata16(color);
}

/************************************************************************************
**                              DRAW FAST VLINE 
*************************************************************************************/
void TFT_PARALLEL_ESP8266::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  // Rudimentary clipping
  if ((x >= _width) || (y >= _height || h < 1)) return;
  if ((y + h - 1) >= _height) { h = _height - y; }
  if (h < 2 ) { drawPixel(x, y, color); return; }

  setAddrWindow(x, y, x, y + h - 1);
  writedata16(color, h);
}

/****************************************************************************************
**                            DRAW FAST HLINE 
*****************************************************************************************/
void TFT_PARALLEL_ESP8266::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  // Rudimentary clipping
  if ((x >= _width) || (y >= _height || w < 1)) return;
  if ((x + w - 1) >= _width) { w = _width - x; }
  if (w < 2 ) { drawPixel(x, y, color); return; }

  setAddrWindow(x, y, x + w - 1, y);
  writedata16(color, w);
}

/*****************************************************************************************
**                             FILL SCREEN 
******************************************************************************************/
void TFT_PARALLEL_ESP8266::fillScreen(uint16_t color) {
  setAddrWindow(0, 0,  _width - 1, _height - 1);
  
  writedata16(color, (_width * _height)/2);
  //yield();
  writedata16(color, (_width * _height)/2);
}


/*****************************************************************************
**                          FILLRECT
******************************************************************************/
void TFT_PARALLEL_ESP8266::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  // rudimentary clipping (drawChar w/big text requires this)
  //uint16_t n =76800;
  if ((x >= _width) || (y >= _height || h < 1 || w < 1)) return;
  if ((x + w - 1) >= _width) { w = _width  - x; }
  if ((y + h - 1) >= _height) { h = _height - y; }
  if (w == 1 && h == 1) {
    drawPixel(x, y, color);
    return;
  }

  setAddrWindow(x, y, x + w - 1, y + h - 1);
  writedata16(color, (w*h)/2); /*yield()*/;writedata16(color, (w*h)/2); 
}

/**************************************************************************************************
* Draw lines faster by calculating straight sections and drawing them with fastVline and fastHline.
***************************************************************************************************/
void TFT_PARALLEL_ESP8266::drawLine(int16_t x0, int16_t y0,int16_t x1, int16_t y1, uint16_t color)
{
  if ((y0 < 0 && y1 <0) || (y0 > _height && y1 > _height)) return;
  if ((x0 < 0 && x1 <0) || (x0 > _width && x1 > _width)) return;
  if (x0 < 0) x0 = 0;
  if (x1 < 0) x1 = 0;
  if (y0 < 0) y0 = 0;
  if (y1 < 0) y1 = 0;

  if (y0 == y1) {
    if (x1 > x0) {
      drawFastHLine(x0, y0, x1 - x0 + 1, color);
    }
    else if (x1 < x0) {
      drawFastHLine(x1, y0, x0 - x1 + 1, color);
    }
    else {
      drawPixel(x0, y0, color);
    }
    return;
  }
  else if (x0 == x1) {
    if (y1 > y0) {
      drawFastVLine(x0, y0, y1 - y0 + 1, color);
    }
    else {
      drawFastVLine(x0, y1, y0 - y1 + 1, color);
    }
    return;
  }

  bool steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }
  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  }
  else {
    ystep = -1;
  }

  int16_t xbegin = x0;
  if (steep) {
    for (; x0 <= x1; x0++) {
      err -= dy;
      if (err < 0) {
        int16_t len = x0 - xbegin;
        if (len) {
          drawFastVLine (y0, xbegin, len + 1, color);
          //writeVLine_cont_noCS_noFill(y0, xbegin, len + 1);
        }
        else {
          drawPixel(y0, x0, color);
          //writePixel_cont_noCS(y0, x0, color);
        }
        xbegin = x0 + 1;
        y0 += ystep;
        err += dx;
      }
    }
    if (x0 > xbegin + 1) {
      //writeVLine_cont_noCS_noFill(y0, xbegin, x0 - xbegin);
      drawFastVLine(y0, xbegin, x0 - xbegin, color);
    }

  }
  else {
    for (; x0 <= x1; x0++) {
      err -= dy;
      if (err < 0) {
        int16_t len = x0 - xbegin;
        if (len) {
          drawFastHLine(xbegin, y0, len + 1, color);
          //writeHLine_cont_noCS_noFill(xbegin, y0, len + 1);
        }
        else {
          drawPixel(x0, y0, color);
          //writePixel_cont_noCS(x0, y0, color);
        }
        xbegin = x0 + 1;
        y0 += ystep;
        err += dx;
      }
    }
    if (x0 > xbegin + 1) {
      //writeHLine_cont_noCS_noFill(xbegin, y0, x0 - xbegin);
      drawFastHLine(xbegin, y0, x0 - xbegin, color);
    }
  }
}
/*****************************************************************************/
// Pass 8-bit (each) R,G,B, get back 16-bit packed color
/*****************************************************************************/
uint16_t TFT_PARALLEL_ESP8266::color565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

/*****************************************************************************
/**                            INVERT DISPLAY
******************************************************************************/
void TFT_PARALLEL_ESP8266::invertDisplay(boolean i)
{
  writecommand(i ? TFT_INVON : TFT_INVOFF);
}

void TFT_PARALLEL_ESP8266::enable(bool enable) {
  writecommand(enable ? 0x29 : 0x28);
}
