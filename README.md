# Arduino_SAAB_ICM2_LCD_Driver
An Adafruit_GFX-based driver for the unique LCD used in the SAAB 9-3 "Infotainment Control Module 2".

### Hello there
Thanks for checking out my ICM2 display library, it's actually my very first "proper library that I've put effort into", heres a list of mistakes I am aware of...

- Addressing of the display is not fully understood / explained / explored / I am lazy.
    - This means the very top row of the display is not used at the moment.
    - Also it means you are not allowed to use the extra space on the display as a framebuffer, sorry.
    - Also, no scrolling. Sorry again!

- No limiting of max bytes send over I2C, IE no byteSent > WIRE_MAX checks. 
- No option to ask the display driver to initialise I2C (ie: call Wire.begin). Other Adafruit-based drivers have this option, I chose to remove it for simplicity.