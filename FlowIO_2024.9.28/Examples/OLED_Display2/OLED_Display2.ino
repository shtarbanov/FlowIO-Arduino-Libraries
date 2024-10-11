/*
  Rui Santos
  https://randomnerdtutorials.com/guide-for-oled-display-with-arduino/

//This font is very wide
FreeMono9pt7b.h
FreeMonoBold9pt7b.h		
FreeMonoOblique9pt7b.h
FreeMonoBoldOblique9pt7b.h

//This font is compact
FreeSans9pt7b.h
FreeSansBold9pt7b.h
FreeSansOblique9pt7b.h
FreeSansBoldOblique9pt7b.h

//This font has serifs
FreeSerif9pt7b.h
FreeSerifBold9pt7b.h
FreeSerifBoldItalic9pt7b.h
FreeSerifItalic9pt7b.h

Fonts come in 9, 12, 28, 24pt versions.
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans9pt7b.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }
  delay(2000);

  display.setFont(&FreeSans9pt7b);
  display.clearDisplay();
  display.setTextSize(1);             
  display.setTextColor(WHITE);        
  display.setCursor(0,20);             
  display.println("Hello, world!");
  display.display();
  delay(2000); 
}
void loop() {
  
}