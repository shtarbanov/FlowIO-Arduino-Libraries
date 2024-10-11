/*********
  Tutorial video:
  https://www.youtube.com/watch?v=N_sWjcKR1Nw 
  Convert any images to Bitmap C++ Array:
  https://javl.github.io/image2cpp/
    1. Select any image from your computer
    2. Set the canvas size to anything less than or equalt to 128 x 64
    3. Set "Scaling" to "Scale to fit".
    4. Set "Background color" to Black 
    5. Check "Invert Image Colors"
    6. Look at the preview displayed to confirm it's okay.
  
Use this example code as a starting point do display various status indicator icons on the screen. Each icon and icon-variation
can be put into another .ino file, just like it is done on the JavaScript GUI interface. The same icons used for the GUI can 
also be used for the OLED display, thereby ensuring consistency.
*********/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "icons.h" //The icons are in .h rather than .ino file, because the must be delcared before our main loop, and this allows us to
//include that .h file here, whereas a .ino file would be included (by default) after the arduino main schetch, resulting in compilation fail.
//This .h approach allows us to have a clean solution without any problems.

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define I2C_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
 
void setup() { 
  if(!display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDRESS)) {
    for(;;);
  }
  delay(2000); // Pause for 2 seconds
 
  // Clear the buffer.
  display.clearDisplay();
  
  // Draw bitmap on the screen
  display.drawBitmap(0, 0, icon48x48_GENERAL_IO, 48, 48, 1);
  display.display();
}
 
void loop() {
  
}