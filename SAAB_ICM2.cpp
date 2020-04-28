#include <Adafruit_GFX.h>
#include <SAAB_ICM2.h>

#ifdef __AVR__
#include <avr/pgmspace.h>
#elif defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>
#endif

// Many (but maybe not all) non-AVR board installs define macros
// for compatibility with existing PROGMEM-reading AVR code.
// Do our own checks and defines here for good measure...

#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif

/*!
    @brief  Constructor for the I2C-interfaced ICM2 display.
    @param  w
            Display width in pixels (106)
    @param  h
            Display height in pixels (65)
    @param  twi
            Pointer to an existing TwoWire instance (e.g. &Wire, the
            microcontroller's primary I2C bus).
*/
SAAB_ICM2::SAAB_ICM2() : Adafruit_GFX(_width, _height), buffer(NULL) {}

/*!
    @brief  Destructor for Adafruit_SSD1306 object.
*/
SAAB_ICM2::~SAAB_ICM2(void)
{
    if (buffer)
    {
        free(buffer);
        buffer = NULL;
    }
}

// INTERNAL FUNCTIONS

void SAAB_ICM2::icm2_command1(uint8_t c)
{
    // I2C
    Wire.beginTransmission(i2caddr);
    Wire.write(c);
    Wire.endTransmission();
}

void SAAB_ICM2::icm2_commandList(const uint8_t *c, uint8_t n)
{

    Wire.beginTransmission(i2caddr);
    Wire.write(c, n);
    Wire.endTransmission();
}

// A public version of icm2_command1(), for existing user code that
// might rely on that function. This encapsulates the command transfer
// in a transaction start/end, similar to old library's handling of it.
/*!
    @brief  Issue a single low-level command directly to the ICM2
            display, bypassing the library.
    @param  c
            Command to issue (0x00 to 0xFF, see datasheet).
    @return None (void).
*/
void SAAB_ICM2::icm2_command(uint8_t c)
{
    icm2_command1(c);
}

// Private variable getters
int8_t SAAB_ICM2::width()
{
    return _width;
}

int8_t SAAB_ICM2::height()
{
    return _height;
}

// ALLOCATE & INIT DISPLAY -------------------------------------------------

/*!
    @brief  Allocate RAM for image buffer, initialize peripherals and pins.
    @param  addr
            I2C address of corresponding ICM2 display.
    @return true on successful allocation/init, false otherwise.
            Well-behaved code should check the return value before
            proceeding.
    @note   MUST call this function before any drawing or updates!
*/
bool SAAB_ICM2::begin(void)
{
    // Allocate dat memory
    if ((!buffer) && !(buffer = (uint8_t *)malloc(_width * ((_height + 7) / 8))))
        return false;

    // Zero the framebuffer
    clearDisplay();

    // run all the init commands
    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x10}, 3);
    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x0e, 0x12}, 4);
    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x0e, 0x06}, 4);
    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x0e, 0x0a}, 4);

    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x0e, 0x24}, 4);
    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x0e, 0x84}, 4);
    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x0d, 0x0a}, 4);
    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x0d, 0x13}, 4);

    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x0d, 0xb7}, 4);
    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x0d, 0x07}, 4);
    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x0b, 0x04}, 4);
    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x0b, 0x40}, 4);

    return true; // Success
}

// DRAWING FUNCTIONS

/*!
    @brief  Set/clear/invert a single pixel. This is also invoked by the
            Adafruit_GFX library in generating many higher-level graphics
            primitives.
    @param  x
            Column of display -- 0 at left to (screen width - 1) at right.
    @param  y
            Row of display -- 0 at top to (screen height -1) at bottom.
    @param  color
            Pixel color, one of: ICM2_ON, ICM2_OFF, ICM2_INVERSE
    @return None (void).
    @note   Changes buffer contents only, no immediate effect on display.
            Follow up with a call to display(), or with other graphics
            commands as needed by one's own application.
*/
void SAAB_ICM2::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if ((x >= 0) && (x < _width) && (y >= 0) && (y < _height))
    {
        // Upgraded to do bytewise y-flipping
        switch (color)
        {
        case ICM2_ON:
            buffer[x + (y / 8) * _width] |= (128 >> (y & 7));
            break;
        case ICM2_OFF:
            buffer[x + (y / 8) * _width] &= ~(128 >> (y & 7));
            break;
        case ICM2_INVERSE:
            buffer[x + (y / 8) * _width] ^= (128 >> (y & 7));
            break;
        }
    }
}

/**************************************************************************/
/*!
    @brief  Fill the framebuffer completely with one color
    @param  color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void SAAB_ICM2::fillScreen(uint16_t color) {
    memset(buffer, color ? 0xFF : 0x00, _width * ((_height + 7) / 8));
}

/*!
    @brief  Get base address of display buffer for direct reading or writing.
    @return Pointer to an unsigned 8-bit array, column-major, columns padded
            to full byte boundary if needed.
*/
uint8_t *SAAB_ICM2::getBuffer(void)
{
    return buffer;
}

/*!
    @brief  Clear contents of display buffer (set all pixels to off).
    @return None (void).
    @note   Changes buffer contents only, no immediate effect on display.
            Follow up with a call to display(), or with other graphics
            commands as needed by one's own application.
*/
void SAAB_ICM2::clearDisplay(void)
{
    memset(buffer, 0, _width * ((_height + 7) / 8));
}

// REFRESH DISPLAY ---------------------------------------------------------

// Write all zeros after an init to clear debris lmao
void SAAB_ICM2::forceClear(void)
{   
    // Note an extra line is being cleared, this removes the "top 1px line"
    // In the future this should be line zero
    for (int line = 0; line < 9; line++)
    {
        // Set address we are writing too, which is split into "lines" of 8 pixels height.
        uint8_t lineAddr = 0x40 + line;
        uint8_t dat[3] = {0x00, 0x01, lineAddr};
        icm2_commandList(dat, sizeof(dat));
        icm2_commandList((const uint8_t[]){0x00, 0x01, 0x20}, 3);
        icm2_commandList((const uint8_t[]){0x00, 0x01, 0x8d}, 3);

        Wire.beginTransmission(i2caddr);
        Wire.write((uint8_t)0x40);
        for(int i = 0; i<106; i++)
        {
            Wire.write(0);
        }
        Wire.endTransmission();
    }
}

/*!
    @brief  Push data currently in RAM to SSD1306 display.
    @return None (void).
    @note   Drawing operations are not visible until this function is
            called. Call after each graphics command, or after a whole set
            of graphics commands, as best needed by one's own application.
*/
void SAAB_ICM2::display(void)
{
    // The display is split into 9 rows of 8 pixels high, each column represtented by a byte.
    // What is weird is that the first line pixels (1-pixel high), is actually found in the 9th row as the MSB. 
    // In this sense, the first line of the display is "at the bottom" of the address space in memory.

    // To prevent weird drawing artifacts, it's best to skip to the 9th row, draw those pixels, and then continue onto rows 1-8.
    // IE - we physically draw from top to bottom, whilst jumping around in address space.
    // The frame buffer is setup so that we kind of have to perform bit shifting to get these printed out nicely.
    // No worries...    
    
    // DRAW FIRST PIXEL ROW (top row of pixels).
    uint8_t *ptr0 = buffer;
    
    // Start at row addr 0x48 (9th row in address space)
    uint8_t lineAddr = 0x40 + 8;
    uint8_t dat[3] = {0x00, 0x01, lineAddr};
    
    icm2_commandList(dat, sizeof(dat));
    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x20}, 3);
    icm2_commandList((const uint8_t[]){0x00, 0x01, 0x8d}, 3);
    
    uint16_t count = _width;

    Wire.beginTransmission(i2caddr);
    Wire.write((uint8_t)0x40);
    
    while (count--)
    {
        Wire.write(*ptr0++ & 0b10000000); // Only grab MSB. This isn't strictly neccesary as only the MSB is actually displayed.
    }
    Wire.endTransmission();

    // Draw remaining rows, in groups of 8 pixel columns as mentioned.
    // Reset pointer to beginning and draw the remaining 8 lines, this sort of "overlaps" the buffer for the first row, in a sense.
    uint8_t *ptr = buffer;

    for (int line = 0; line < 8; line++)
    {
        // Set address location in memory
        uint8_t lineAddr = 0x40 + line;
        uint8_t dat[3] = {0x00, 0x01, lineAddr};
        icm2_commandList(dat, sizeof(dat));
        icm2_commandList((const uint8_t[]){0x00, 0x01, 0x20}, 3);
        icm2_commandList((const uint8_t[]){0x00, 0x01, 0x8d}, 3);

        uint16_t count = _width;

        Wire.beginTransmission(i2caddr);
        Wire.write((uint8_t)0x40);

        while (count--)
        {
            Wire.write(((*ptr & 127) << 1) | ((*(ptr + _width) & 0b10000000) >> 7));
            ptr++;
        }
        Wire.endTransmission();
    }
}

