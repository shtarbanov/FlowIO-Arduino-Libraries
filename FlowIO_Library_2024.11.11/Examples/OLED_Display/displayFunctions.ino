/*
Height = 64px
Width = 128px
 .============================.
 || (0,0)            (127,0) ||
 ||                          ||
 ||                          ||
 ||                          ||
 || (0,63)          (127,63) ||
 '============================'

*/

void testdrawline() {
  display.clearDisplay(); // Clear display buffer
  display.drawLine(0, 15, 127, 15, 1); //first pair of args are the starting coordinates.
  display.display(); // Update screen
}

void testdrawRectEmpty(){
  display.clearDisplay(); // Clear display buffer
  display.drawRect(0, 10, 116, 54, 1); //first pair of args are the starting coordinates.
  display.display();
}

void testdrawRectFilled(){
  // display.clearDisplay();
  display.fillRect(30, 30, 50, 50, 2);
  display.display();
}

void testdrawCircleEmpty(void) {
  display.clearDisplay();
  display.drawCircle(30, 30, 30, 1);
  display.display();
}

void testdrawCircleFilled(void) {
    // display.clearDisplay();
    display.fillCircle(30, 30, 30, 2); //Inver the color of the interior
    display.display(); 
}

void testdrawRoundRectEmpty(void) {
  display.clearDisplay();
  for(int16_t i=0; i<display.height()/2-2; i+=2) {
    display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, SSD1306_WHITE);
    display.display();
    delay(1);
  }
}

void testdrawRoundRectFilled(void) {
  display.clearDisplay();
  for(int16_t i=0; i<display.height()/2-2; i+=2) {
    // The INVERSE color is used so round-rects alternate white/black
    display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i, display.height()/4, SSD1306_INVERSE);
    display.display();
    delay(1);
  }
}

void testdrawtriangle(void) {
  display.clearDisplay();
  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=5) {
    display.drawTriangle(
      display.width()/2  , display.height()/2-i,
      display.width()/2-i, display.height()/2+i,
      display.width()/2+i, display.height()/2+i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
}

void testfilltriangle(void) {
  display.clearDisplay();
  for(int16_t i=max(display.width(),display.height())/2; i>0; i-=5) {
    // The INVERSE color is used so triangles alternate white/black
    display.fillTriangle(
      display.width()/2  , display.height()/2-i,
      display.width()/2-i, display.height()/2+i,
      display.width()/2+i, display.height()/2+i, SSD1306_INVERSE);
    display.display();
    delay(1);
  }
}

void testdrawchar(void) {
  display.clearDisplay();
  display.setTextSize(3);      // Normal 1:1 pixel scale
  display.setTextColor(1); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  for(int16_t i=80; i<256; i++) {
    if(i == '\n') display.write(' ');
    else          display.write(i);
  }
  display.display();
}

void drawFlowIOLogo(void) {
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(0,1);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("www.softrobotics.io\n"));
  display.setTextSize(3);
  display.setTextColor(1);
  display.setCursor(5,15);
  display.println("FlowIO");
  display.setTextSize(2);
  display.setTextColor(1);
  display.setCursor(5,43);
  display.println("PLATFORM");
  display.drawRect(0, 10, 116, 54, 1);
  display.display();
}

void testscrolltext(void) {
  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("scroll"));
  display.display();      // Show initial text
  delay(100);
  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}

void testdrawbitmap(void) {
  display.clearDisplay();
  display.drawBitmap(
    (128  - LOGO_WIDTH ) / 2,
    (64 - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(1000);
}

// #define XPOS   0 // Indexes into the 'icons' array in function below
// #define YPOS   1
// #define DELTAY 2

// void testanimate(const uint8_t *bitmap, uint8_t w, uint8_t h) {
//   int8_t f, icons[NUMFLAKES][3];

//   // Initialize 'snowflake' positions
//   for(f=0; f< NUMFLAKES; f++) {
//     icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
//     icons[f][YPOS]   = -LOGO_HEIGHT;
//     icons[f][DELTAY] = random(1, 6);
//     Serial.print(F("x: "));
//     Serial.print(icons[f][XPOS], DEC);
//     Serial.print(F(" y: "));
//     Serial.print(icons[f][YPOS], DEC);
//     Serial.print(F(" dy: "));
//     Serial.println(icons[f][DELTAY], DEC);
//   }

//   for(;;) { // Loop forever...
//     display.clearDisplay(); // Clear the display buffer

//     // Draw each snowflake:
//     for(f=0; f< NUMFLAKES; f++) {
//       display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SSD1306_WHITE);
//     }

//     display.display(); // Show the display buffer on the screen
//     delay(200);        // Pause for 1/10 second

//     // Then update coordinates of each flake...
//     for(f=0; f< NUMFLAKES; f++) {
//       icons[f][YPOS] += icons[f][DELTAY];
//       // If snowflake is off the bottom of the screen...
//       if (icons[f][YPOS] >= display.height()) {
//         // Reinitialize to a random position, just off the top
//         icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
//         icons[f][YPOS]   = -LOGO_HEIGHT;
//         icons[f][DELTAY] = random(1, 6);
//       }
//     }
//   }
// }
