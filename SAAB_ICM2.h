// Modified the Adafruit SSD1306 library to drive the ICM2 display nicely!

#ifndef _SAAB_ICM2_H_
#define _SAAB_ICM2_H_

#include <Wire.h>
#include <Adafruit_GFX.h>

#define ICM2_OFF 0 ///< Draw 'off' pixels
#define ICM2_ON 1  ///< Draw 'on' pixels
#define ICM2_INVERSE 2  ///< Draw 'inverted' pixels

class SAAB_ICM2 : public Adafruit_GFX
{
public:
  // Constructor
  SAAB_ICM2(void);

  // Deconstructor
  ~SAAB_ICM2(void);

  boolean begin(void);
  void    display(void);
  void    clearDisplay(void);
  void    forceClear(void);
  void    drawPixel(int16_t x, int16_t y, uint16_t color);
  void    fillScreen(uint16_t color);
  void    icm2_command(uint8_t c);
  int8_t  width();
  int8_t  height();
  uint8_t *getBuffer(void);

private:
  void icm2_command1(uint8_t c); // Sends a sequence of Wire commands
  void icm2_commandList(const uint8_t *c, uint8_t n); // Sends a sequence of Wire commands

  uint8_t     *buffer;
  int8_t       i2caddr = 0x3c;
  int8_t       _width = 106;
  int8_t       _height = 65;

  uint32_t     wireClk;    // Wire speed for SSD1306 transfers
  uint32_t     restoreClk; // Wire speed following SSD1306 transfers
};

#endif // _SAAB_ICM2_H_
