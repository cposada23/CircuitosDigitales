#include <SoftwareSerial.h>
#include <Keypad.h>
#include <SD.h>
#include <SPI.h>


#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>


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


uint8_t b;


/**
 * Para el teclado
 */
//const byte ROWS = 4; // Four rows
//const byte COLS = 4; // Three columns
// Define the Keymap
char keys[4][4] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

char Matrizletras[3][4] = {
  {' ','A','D','G'},
  {' ','B','E','H'},
  {' ','C','F',' '}
};

//char lugar [1][2];


// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[4] = { 2,3,6,7 };
// Connect keypad COL0, COL1 and COL2 to these Arduino pins.
byte colPins[4] = { A2, A1,A0, 9 }; 

// Create the Keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, 4, 4 );



SoftwareSerial mySerial(A5, A4); // RX, TX


char estado;
char key;
int turno = 0;
int led = 5;
int letra = 0;
int numero = 0;
int num = 0;
int contador = 0;
bool empezar = false;
char l, n, le;

/** Matrices **/

byte posBarcos [11][11]= {
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0}
};

byte posAtaques[11][11] = {
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0}
};

byte pos[11][11][2];

char letras[11] = {'A','B','C','D','E','F','G','H','I','J','K'};
byte posAtaqueX, posAtaqueY;


void setup()
{
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB port only
  }

  /** Pantalla **/
  tft.initR(INITR_BLACKTAB);
   /* Inicializo la lectura de la SD */
  if (!SD.begin(SD_CS)) {
    //Serial.println("failed!");
    return;
  }
  creaMatriz();
  bmpDraw("fondoDef.bmp", 0, 0);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);  
  Serial.begin(9600);
  //Serial.println("e");
  mySerial.begin(9600);
}

void loop()
{
  if(!empezar){
    //Serial.println("escoger pos de barcos");
    crearBarcos();
    empezar = true;
  }
  else{
      /** Quien empieza 1 --> yo 2 --> El otro? **/
      while(turno == 0){
        key = kpd.getKey();
        if(key){
          switch(key)
          {
            case '1':
              //Serial.println("Jugador 1: Usted empieza");
              bmpDraw("fondoAtc.bmp", 0, 0);
              turno = 1;
              break;
            case '2':
              //Serial.println("Jugador 2: Espere el turno del jugador 1");
              bmpDraw("fondoDef.bmp", 0, 0);
              turno = 2;
              break;
            //default:
              //Serial.println(key);
          }
        }
      }
    
      /** Turno de enviar datos **/
      if(turno == 1){
        digitalWrite(led, HIGH); 
        escogerPosAtaque();  
        turno = 2; 
        bmpDraw("fondoDef.bmp", 0, 0);
        contador = 0;
      }
      /** turno de recivir datos **/
      else if(turno == 2){
        digitalWrite(led, LOW);
        if (mySerial.available()>0) 
        {       
          estado = mySerial.read();
          if(estado != '*' && estado != '#'){
            l = estado;
          }
          if(estado == '*'){
            //mandaron la letra en el estado anterior
            le = l;  
          }if(estado == '#'){
            //mandaron el numero
            n = l; 
            turno = 1;
            bmpDraw("fondoAtc.bmp", 0, 0);
            letra = 0;
            numero = 0;
            contador = 0;
          } 
        }    
      }
  }
}

byte getPosDeLetra(char l){
  for(byte i = 0; i<11; i++){
    if(letras[i] == l){
      return i;
    }
  }
}

void escogerPosAtaque()
{
  byte cont = 0;
  byte y = 0;
  char aux[2];
  while(true)
  {
    key = kpd.getKey();
    if(key )  // Check for a valid key.
    {
      /**Escogee letra */
      if(key == '*'){
        mySerial.write(key);
        break;
      }else{
          cont++;
          y  = key - '0'; 
          aux[0] = Matrizletras[(cont -1) % 3][y - 1];
          limpiaEntrada();
          tft.setCursor(5,148);
          tft.setTextColor(ST7735_RED);
          tft.print("Ataque->");
          tft.setCursor(70,148);//coord enadas de la posicion 1
          tft.setTextColor(ST7735_BLUE);
          tft.print(aux[0]);//estas posiciones se ingresan mediante teclado
          mySerial.write(aux[0]);
          tft.setCursor(95,148);//coordenadas de la posicion 2
          tft.setTextColor(ST7735_BLUE);
          posAtaqueX = getPosDeLetra(aux[0]);
          //Serial.println(posAtaqueX);
      }
    }
  }

  while(true)
  {
    key = kpd.getKey();
    if(key )  // Check for a valid key.
    {
      if(key == '#'){
        mySerial.write(key);
        break;
      }else{
          posAtaqueY = key - '0'; 
          aux[1] = key;
          limpiaEntrada();
          tft.setCursor(5,148);
          tft.setTextColor(ST7735_RED);
          tft.print("Ataque->");
          tft.setCursor(70,148);//coord enadas de la posicion 1
          tft.setTextColor(ST7735_BLUE);
          tft.print(aux[0]);//estas posiciones se ingresan mediante teclado
          tft.setCursor(95,148);//coordenadas de la posicion 2
          tft.setTextColor(ST7735_BLUE);
          tft.print(aux[1]);
          mySerial.write(key);
      }
    }
  }

  //posAtaques[posAtaqueX][posAtaqueY] = 1;
  //dibujarCirculos(1);
  //creaCirculo(pos[posAtaqueX][posAtaqueY][0],pos[posAtaqueX][posAtaqueY][1],1);
  //delay(7000);
  
}

void dibujarCirculos(byte donde){
    for(byte i = 0; i<11; i++){
      for(byte j = 0; j<11; j++){
        if (posAtaques[i][j] == donde){
          creaCirculo(pos[i][j][0],pos[i][j][1],1);
        }
      }
    }
}

void creaMatriz()
{
  for(byte i=0;i<11;i++)
  {
    for(byte j=0;j<11;j++)
    {
      for(byte k=0;k<2;k++)
      {
        if(k==0)
        {
          pos[i][j][k]=18+(i*10);
        }
        if(k==1)
        {
          pos[i][j][k]=31+(j*10);
        }
      }
    }
  } 
}


void crearBarcos()
{
  
  int i = 1;
  byte x = 18, y = 31; 
  creaCirculo(x, y,1);
  while (i == 1){
    b = readButton();
    delay(150);
    if(b)
    {
      if (b == BUTTON_DOWN) 
      {
        y = y + 10;
      }
      if (b == BUTTON_LEFT) 
      {
        x = x - 10;
      }
      if (b == BUTTON_UP) 
      {
         y = y - 10;
      }
      if (b == BUTTON_RIGHT) 
      {
        
        x = x + 10;
      }
      if(b == BUTTON_SELECT)
      {
        i = 2;
      }
      creaCirculo(x, y,1);
    }

    
  }
  
}

void limpiaEntrada()
{
  tft.fillRect(4, 148,119, 9, ST7735_WHITE);
}

void creaBarco(byte x1,byte y1,byte x2,byte y2)
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

void creaCirculo(byte x,byte y,byte acerto)
{
  if(acerto==1)
  {
    tft.drawCircle(x, y, 3, ST7735_GREEN);
    tft.drawLine(x+4,y,x-4,y,ST7735_GREEN);
    tft.drawLine(x,y+4,x,y-4,ST7735_GREEN);  
  }
  else
  {
    tft.drawCircle(x, y, 3, ST7735_BLUE);
    tft.drawLine(x+4,y,x-4,y,ST7735_BLUE);
    tft.drawLine(x,y+4,x,y-4,ST7735_BLUE);
  }
}

void limpia()
{
  tft.fillRect(4, 139,119, 18, ST7735_WHITE);
  tft.fillRect(4, 3, 119, 11, ST7735_WHITE);
}


uint8_t readButton(void) 
{
  float a = analogRead(3);
  
  a *= 5.0;
  a /= 1024.0;
  
  /*Serial.print("Button read analog = ");
  Serial.println(a);*/
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
  //Serial.print("Loading image '");
  Serial.print(filename);
  //Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print("File not found");
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    //Serial.print("File size: ");
    Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    //Serial.print("Image Offset: ");
    Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    //Serial.print("Header size: "); 
    Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      //Serial.print("Bit Depth: ");
      Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        //Serial.print("Image size: ");
        Serial.print(bmpWidth);
        //Serial.print('x');
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
        //Serial.print("Loaded in ");
        Serial.print(millis() - startTime);
        //Serial.println(" ms");
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
