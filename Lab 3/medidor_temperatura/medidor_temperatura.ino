
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SD.h>
#include <SPI.h>

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

#define SD_CS    4  // Chip select line for SD card
#define TFT_CS  10  // Chip select line for TFT display
#define TFT_DC   8  // Data/command line for TFT
#define TFT_RST  -1  // Reset line for TFT (or connect to +5V)

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!

#define BUTTON_NONE 0
#define BUTTON_DOWN 1
#define BUTTON_RIGHT 2
#define BUTTON_SELECT 3
#define BUTTON_UP 4
#define BUTTON_LEFT 5

#include <math.h>
 

const int B=4275;                 // B value of the thermistor
const int R0 = 100000;            // R0 = 100k
const int pinTempSensor = A1;     // Grove - Temperature Sensor connect to A3
boolean timbro=false;
int x = 0;
int y = 0;

void setup(void) 
{
  Serial.begin(9600);
  tft.initR(INITR_BLACKTAB);
  uint16_t time = millis();
  time = millis() - time;
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  pinMode(A0, OUTPUT);
  
  tft.setCursor(0, 110);
  delay(2000);
  Serial.print("Initializing SD card...");
    if (!SD.begin(SD_CS)) {
      Serial.println("failed!");
      return;
    }
  bmpDraw("parrot1.bmp", 0, 0);
  creaRecuadro();
  tft.setTextColor(ST7735_BLACK);
}


uint8_t readButton(void) 
{
  float a = analogRead(3);
  
  a *= 5.0;
  a /= 1024.0;
  
  Serial.print("Button read analog = ");
  Serial.println(a);
  if (a < 0.2) return BUTTON_DOWN;
  if (a < 1.0) return BUTTON_RIGHT;
  if (a < 1.5) return BUTTON_SELECT;
  if (a < 2.0) return BUTTON_UP;
  if (a < 3.2) return BUTTON_LEFT;
  else return BUTTON_NONE;
}

void loop() 
{
  if(!timbro)
  {
    creaRecuadroTemp();
    int a = analogRead(pinTempSensor);
    float R = 1023.0/((float)a)-1.0;
    R = 100000.0*R;
    float temperature=1.0/(log(R/100000.0)/B+1/298.15)-273.15;//convert to temperature via datasheet ;
    tft.setCursor(50,90);
    tft.print(temperature);
    
    if(temperature>30&&!timbro)
    {
      digitalWrite(A0, HIGH);
      timbro = true;
      cargaVolcanE();
      x = 80;
      y = 60;
      creaRecuadro();
    }
  }
  else
  {
    //borraCir(x,y);
    uint8_t b = readButton();
    delay(100);
    if (b == BUTTON_DOWN) 
    {
      y = y + 4;
    }
    if (b == BUTTON_LEFT) 
    {
      x = x - 4;
    }
    if (b == BUTTON_UP) 
    {
      y = y - 4;
    }
    if (b == BUTTON_RIGHT) 
    {
      x = x + 4;
    }
    if ((b == BUTTON_SELECT)) 
    {

    }
    graficaCir(x,y);
    if(x == 64 && y == 80)
    {
      digitalWrite(A0, LOW);
      timbro = false;
      cargaVolcanC();
      creaRecuadro();
    }
  }
}

void graficaCir(int x,int y)
{
  tft.drawCircle(x, y, 5, ST7735_RED);
}

void borraCir(int x,int y)
{
  tft.drawCircle(x, y, 5, ST7735_WHITE);
}

void creaRecuadro()
{
   for(int i=40;i<121;i++)
   {
    for(int j=32;j<97;j++)
    {
      tft.drawPixel(j, i, ST7735_WHITE);
    }
   }
   
   if(timbro)
   {
    tft.drawCircle(64, 80, 5, ST7735_RED);
    tft.drawCircle(80, 60, 5, ST7735_RED);
    x = 80;
    y = 60;
   }
   else
   {
    tft.setCursor(32,48);
    tft.setTextColor(ST7735_BLACK);
    tft.print("Temperatura");
   }
}

void cargaVolcanE()
{
  tft.setCursor(0, 110);
  delay(2000);
  bmpDraw("parrot.bmp", 0, 0);
}

void cargaVolcanC()
{
  tft.setCursor(0, 110);
  delay(2000);
  bmpDraw("parrot1.bmp", 0, 0);
}

void creaRecuadroTemp()
{
  for(int i=80;i<121;i++)
   {
    for(int j=36;j<95;j++)
    {
      tft.drawPixel(j, i, ST7735_WHITE);
    }
   }
  tft.drawRect(33, 70, 63, 50, ST7735_GREEN);
}

// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

#define BUFFPIXEL 20

void bmpDraw(char *filename, uint8_t x, uint8_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print("Loading image '");
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print("File not found");
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print("File size: "); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print("Image Offset: "); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print("Header size: "); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print("Bit Depth: "); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print("Image size: ");
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.Color565(r,g,b));
          } // end pixel
        } // end scanline
        Serial.print("Loaded in ");
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println("BMP format not recognized.");
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
