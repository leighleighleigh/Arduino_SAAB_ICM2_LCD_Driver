# Arduino_SAAB_ICM2_LCD_Driver
An Adafruit_GFX-based driver for the unique LCD used in the SAAB 9-3 "Infotainment Control Module 2".

This display has a unique resolution: 106x65 monochrome pixels. 
This has made development lightly trickier for a beginner like me.

### Hello there
Thanks for checking out my ICM2 display library, it's actually my very first "proper library that I've put effort into".
Some current issues / TODOs:
- No scroll support (I believe)
- display.textWrap cannot be true. For some reason it does not respect the true display _width property.
- Default AdafruitGFX font doesn't work? Unsure what went wrong there.
- No limiting of max bytes send over I2C, IE no byteSent > WIRE_MAX checks. 
- No option to ask the display driver to initialise I2C (ie: call Wire.begin). Other Adafruit-based drivers have this option, I chose to remove it for simplicity.