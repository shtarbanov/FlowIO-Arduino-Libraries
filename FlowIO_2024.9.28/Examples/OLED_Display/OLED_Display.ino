//Datasheet for SSD1306: https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf#:~:text=SSD1306%20is%20a%20single-chip%20CMOS%20OLED/PLED

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int x=0, y=0;

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };

void setup() {
  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // display.display(); //Shows default display buffer (library sets this to the Adafruit logo)
  // delay(2000);

  // testdrawline();
  // delay(100);
  // testdrawRectEmpty();
  // delay(100);
  // testdrawRectFilled();
  // delay(100);
  // testdrawCircleEmpty();    // Draw circles (outlines)
  // delay(200);
  // testdrawCircleFilled();    // Draw circles (filled)
  // delay(300);
  // testdrawRoundRectEmpty(); // Draw rounded rectangles (outlines)
  // delay(300);
  // testdrawRoundRectFilled(); // Draw rounded rectangles (filled)
  // delay(300);
  // testdrawtriangle();  // Draw triangles (outlines)
  // testfilltriangle();  // Draw triangles (filled)
  // testdrawchar();      // Draw characters of the default font
  drawFlowIOLogo();    // Draw 'stylized' characters
  // testscrolltext();    // Draw scrolling text
  // testdrawbitmap();    // Draw a small bitmap image

  // display.invertDisplay(true); //inverts the display
  // delay(1000);

}

void loop() {
}
