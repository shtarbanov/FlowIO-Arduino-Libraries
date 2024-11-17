/*
  Convert any images to Bitmap C++ Array:
  https://javl.github.io/image2cpp/
    1. Select any image from your computer
    2. Set the canvas size to anything less than or equalt to 128 x 64
    3. Set "Scaling" to "Scale to fit".
    4. Set "Background color" to Black 
    5. Check "Invert Image Colors"
    6. Look at the preview displayed to confirm it's okay.
  */

static const uint8_t PROGMEM icon48x48_GENERAL_IO[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x7f, 
	0xff, 0xff, 0xfe, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xfe, 0x00, 
	0x00, 0x7f, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x7f, 0xff, 0xff, 
	0xfe, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x7f, 
	0xff, 0xff, 0xfe, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xfe, 0x00, 
	0x00, 0x7f, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x1f, 0xff, 0xff, 
	0xf8, 0x00, 0x00, 0x00, 0xc0, 0x03, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x07, 0x80, 0x00, 0x00, 0x00, 
	0xc0, 0x07, 0xc0, 0x00, 0x00, 0x00, 0xc0, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0xc0, 0x1b, 0x70, 0x00, 
	0x00, 0x00, 0xc0, 0x33, 0x30, 0x00, 0x00, 0x00, 0xc0, 0x63, 0x10, 0x00, 0x00, 0x00, 0xc0, 0x03, 
	0x00, 0x00, 0x00, 0x00, 0xc0, 0x03, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x07, 0xc0, 0x00, 0x00, 0x07, 
	0xf0, 0x0f, 0xe0, 0x00, 0x00, 0x07, 0xf8, 0x1f, 0xe0, 0x00, 0x00, 0x0f, 0xf8, 0x1f, 0xf0, 0x00, 
	0x00, 0x0f, 0xf8, 0x1f, 0xf0, 0x00, 0x00, 0x07, 0xf8, 0x1f, 0xe0, 0x00, 0x00, 0x07, 0xf0, 0x0f, 
	0xe0, 0x00, 0x00, 0x03, 0xf0, 0x0f, 0xc0, 0x00, 0x00, 0x00, 0xc0, 0x03, 0x00, 0x00, 0x00, 0x00, 
	0xc0, 0x03, 0x00, 0x00, 0x00, 0x08, 0xc6, 0x03, 0x00, 0x00, 0x00, 0x0c, 0xcc, 0x03, 0x00, 0x00, 
	0x00, 0x0c, 0xd8, 0x03, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x03, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x03, 
	0x00, 0x00, 0x00, 0x01, 0xe0, 0x03, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char PROGMEM icon64x64_House[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x07, 0xfe, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0xe0, 0x07, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x78, 0x06, 0x06, 0x00, 
	0x00, 0x00, 0x00, 0x78, 0x1c, 0x06, 0x06, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x0f, 0x06, 0x06, 0x00, 
	0x00, 0x00, 0x03, 0xc0, 0x03, 0x86, 0x06, 0x00, 0x00, 0x00, 0x07, 0x80, 0x01, 0xee, 0x06, 0x00, 
	0x00, 0x00, 0x1e, 0x00, 0x00, 0x7e, 0x06, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x3e, 0x06, 0x00, 
	0x00, 0x00, 0x70, 0x00, 0x00, 0x1e, 0x06, 0x00, 0x00, 0x01, 0xe0, 0x03, 0x80, 0x07, 0x06, 0x00, 
	0x00, 0x03, 0xc0, 0x07, 0xe0, 0x03, 0xc6, 0x00, 0x00, 0x0f, 0x00, 0x1e, 0x70, 0x00, 0xe6, 0x00, 
	0x00, 0x1e, 0x00, 0x3c, 0x3c, 0x00, 0x7e, 0x00, 0x00, 0x78, 0x00, 0x70, 0x0e, 0x00, 0x1e, 0x00, 
	0x00, 0xf0, 0x01, 0xe0, 0x07, 0x80, 0x0f, 0x00, 0x03, 0xc0, 0x03, 0x80, 0x03, 0xc0, 0x03, 0x80, 
	0x07, 0x80, 0x0f, 0x00, 0x00, 0xe0, 0x01, 0xe0, 0x0e, 0x00, 0x1e, 0x00, 0x00, 0x78, 0x00, 0xf0, 
	0x3c, 0x00, 0x78, 0x00, 0x00, 0x1c, 0x00, 0x38, 0x78, 0x00, 0xf0, 0x00, 0x00, 0x0f, 0x00, 0x1e, 
	0xe0, 0x01, 0xc0, 0x00, 0x00, 0x07, 0x80, 0x0f, 0xe0, 0x07, 0x80, 0x00, 0x00, 0x01, 0xe0, 0x06, 
	0x70, 0x0e, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x0e, 0x30, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x1c, 
	0x38, 0x70, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x18, 0x1d, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x07, 0xb8, 
	0x0f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 
	0x06, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x03, 0x40, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 
	0x00, 0xc1, 0xff, 0xfe, 0x1f, 0xff, 0x83, 0x00, 0x00, 0xc3, 0xff, 0xff, 0x1f, 0xff, 0x83, 0x00, 
	0x00, 0xc3, 0x80, 0x03, 0x18, 0x01, 0x83, 0x00, 0x00, 0xc3, 0x80, 0x03, 0x18, 0x01, 0x83, 0x00, 
	0x00, 0xc3, 0x80, 0x03, 0x18, 0x01, 0x83, 0x00, 0x00, 0xc3, 0x80, 0x03, 0x18, 0x01, 0x83, 0x00, 
	0x00, 0xc3, 0x80, 0x03, 0x18, 0x01, 0x83, 0x00, 0x00, 0xc3, 0x80, 0x03, 0x18, 0x01, 0x83, 0x00, 
	0x00, 0xc3, 0x80, 0x03, 0x18, 0x01, 0x83, 0x00, 0x00, 0xc3, 0x80, 0x03, 0x18, 0x01, 0x83, 0x00, 
	0x00, 0xc3, 0x80, 0x03, 0x18, 0x01, 0x83, 0x00, 0x00, 0xc3, 0x80, 0x03, 0x18, 0x01, 0x83, 0x00, 
	0x00, 0xc3, 0x80, 0x03, 0x1f, 0xff, 0x83, 0x00, 0x00, 0xc3, 0x80, 0x33, 0x1f, 0xff, 0x83, 0x00, 
	0x00, 0xc3, 0x80, 0x3b, 0x00, 0x00, 0x03, 0x00, 0x00, 0xc3, 0x80, 0x03, 0x00, 0x00, 0x03, 0x00, 
	0x00, 0xc3, 0x80, 0x03, 0x00, 0x00, 0x03, 0x00, 0x00, 0xc3, 0x80, 0x03, 0x00, 0x00, 0x03, 0x00, 
	0x00, 0xc3, 0x80, 0x03, 0x00, 0x00, 0x03, 0x00, 0x00, 0xc3, 0x80, 0x03, 0x00, 0x00, 0x03, 0x00, 
	0x00, 0xc3, 0x80, 0x03, 0x00, 0x00, 0x03, 0x00, 0x00, 0xc3, 0x80, 0x03, 0x00, 0x00, 0x03, 0x00, 
	0x00, 0xc3, 0x80, 0x03, 0x00, 0x00, 0x03, 0x00, 0x00, 0xc3, 0x80, 0x03, 0x00, 0x00, 0x03, 0x00, 
	0x00, 0xc3, 0x80, 0x03, 0x00, 0x00, 0x03, 0x00, 0x00, 0xc3, 0x80, 0x03, 0x00, 0x00, 0x03, 0x00, 
	0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 
	0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};