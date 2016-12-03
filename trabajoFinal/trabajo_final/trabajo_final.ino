#include <TimerOne.h>

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

/* Variables para el correcto funcionamiento del sensor de temperatura */
const int B=4275;                 // B value of the thermistor
const int R0 = 100000;            // R0 = 100k
const int pinTempSensor = A1;     // Grove - Temperature Sensor connect to A3

float temperature = 0.0;  //Valor de temperatura del sensor


/* Variables que me controlan cuando leo la temperatura del sensor */
volatile int contador = 0;
int n = contador;  
bool timbro = false;
/* ---------- */



/* Variables que indican la posicion en la que se dibuja el circulo rojo */
int x = 0;
int y = 0;

void setup() {
  /* Configuraciones iniciales */
  Serial.begin(9600);
  tft.initR(INITR_BLACKTAB); //Inicializo la pantalla
  Serial.println("empezo");
  
  /* Interrupcion por tiempo */
  Timer1.initialize(1000000); //Cada segundo se lee la temperatura del sensor
  Timer1.attachInterrupt(conteo);

  /* Inicializo la lectura de la SD */
  if (!SD.begin(SD_CS)) {
    Serial.println("failed!");
    return;
  }
  bmpDraw("fondoDef.bmp", 0, 0); //Cargo la imagen de fondo  
}

void loop() {
  tft.setCursor(7,5);
  tft.setTextColor(ST7735_BLACK);
  tft.print("Iniciando juego...");
  creaBarcos(18,31,28,31);
  creaBarcos(38,61,38,111);
  /*else if(timbro){

    
      uint8_t b = readButton();
      tft.fillCircle(x,y,4,ST7735_WHITE);
      if (b == BUTTON_DOWN) 
      {
        y = y + 2;
      }
      if (b == BUTTON_LEFT) 
      {
        x = x - 2;
      }
      if (b == BUTTON_UP) 
      {
        y = y - 2;
      }
      if (b == BUTTON_RIGHT) 
      {
        x = x + 2;
      }
  
      
      tft.fillCircle(x,y,4,ST7735_RED);
  
      if(x <= 64 && x>=60 && y <= 110 && y >=106){
        
        enfriar(); //Desactivo el timbre
      }
  }*/
}

/**
 * Metodo para actualizar el contador, dependiendo del valor de este se medirá la 
 * temperatura para saber cuanto a cambiado, esto se hace para no tener que estar
 * realizando constantemente la lectura de la temperatura del sensor
 */
void conteo(){
  contador = (contador + 1) % 3;
}

void creaBarcos(int x1,int y1,int x2,int y2)
{
  if(y1==y2)
  {
    tft.fillRect(x1, y1-3, x2-x1, 7, ST7735_RED);
    
  }
  else
  {
    tft.fillRect(x1-3, y1, 7, y2-y1, ST7735_RED);
  }
  tft.fillCircle(x1,y1,3,ST7735_RED);
  tft.fillCircle(x2,y2,3,ST7735_RED);
  
}
/**
 * Esta función activa el timbre cuando la temperatura supera cierto umbral
 */
 
void timbrar(){
  timbro = true; 
  bmpDraw("parrot.bmp", 0, 0);
  tft.fillCircle(64, 110, 8, ST7735_BLUE);
  x = 80;
  y = 140;
  n = 1;
//  digitalWrite(timbre, HIGH); 
}

/**
 * Esta funcion desactiva el timbre cuando la bola roja llega a la bola azul
 */
void enfriar(){
 // digitalWrite(timbre, LOW);  
  timbro = false;
  bmpDraw("parrot1.bmp", 0, 0);

}

/**
 * Crea el recuadro en el que se muestra el valor de la temperatura
 * esto para no estar actualizando todos los pixeles de la pantalla
 * cada vez que la temperatura cambie
 */
void mostrarTemperatura(float temp){
  tft.fillRect(30, 120, 70, 40, ST7735_WHITE);
  tft.setCursor(45,142);
  if(temp < 28){
    tft.setTextColor(ST7735_BLACK);
  }else{
    tft.setTextColor(ST7735_RED);
  }
  
  tft.print(temp);
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
