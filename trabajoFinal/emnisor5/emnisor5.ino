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

Adafruit_ST7735 tft = Adafruit_ST7735(10, 8, -1);

#define TFT_SCLK 13   
#define TFT_MOSI 11 
char keys[4][4] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[4] = { 2,3,6,7 };
byte colPins[4] = { A2, A1,A0, 9 }; 
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, 4, 4 );
SoftwareSerial mySerial(A5, A4); // RX, TX
char estado;
int num = 0;
char  n, le;
byte posBarcos [11][11];
byte matAtaque [11][11];
byte posAtaqueX, posAtaqueY;
char letra; // LETRA, NUMERO
byte numero;
char l;
uint8_t b; // PAra el joystick
char key; // Para el teclado 
int turno = 0; // a que jugador le toca enviar o recivir datos
bool empezar = false;  // True -> Ha terminado de escoger los barcos
char Matrizletras[3][4] = { // PARA obtener la letra dependiendo del numero de veces que se unda una tecla
  {' ','A','D','G'},
  {' ','B','E','H'},
  {' ','C','F',' '}
};

void setup()
{
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB port only
  }

  /** Pantalla **/
  tft.initR(INITR_BLACKTAB);
   /* Inicializo la lectura de la SD */
  if (!SD.begin(4)) {
    //Serial.println("failed!");
    return;
  }
  bmpDraw("fondoDef.bmp", 0, 0);
  Serial.begin(9600);
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
          //Serial.println(key);
          switch(key)
          {
            case '1':
              bmpDraw("fondoAtc.bmp", 0, 0);
              turno = 1;
              break;
            case '2':
              bmpDraw("fondoDef.bmp", 0, 0);
              turno = 2;
              break;
          }
        }
      }
    
      /** Turno de enviar datos **/
      if(turno == 1){ 
        mostrarMisAtaques();
        escogerPosAtaque();  
        turno = 2; 
        bmpDraw("fondoDef.bmp", 0, 0);
      }
      /** turno de recivir datos **/
      else if(turno == 2){
        mostrarAtaques();
        if (mySerial.available()>0) 
        {       
          estado = mySerial.read();
          llenarDatos(estado, '#');  
          if(estado != '*' && estado != '#'){
             l = estado;
          }
          if(estado == '*'){
            //mandaron la letra en el estado anterior
            letra = l;
            
          }if(estado == '#'){
            //mandaron el numero
            llenarDatos(letra, l);  
            numero = l - '0'; 
            cambiarMatrizDefensa(letra, numero);
            turno = 1;
            bmpDraw("fondoAtc.bmp", 0, 0);
          } 
        }    
      }
  }
}

byte getPosDeLetra(char l){
  char i;
  byte j = 0;
  for(i = 'A'; i<'K'; i++){
    if(i == l) break;
    j++;
  }
  return j;
}

void escogerPosAtaque()
{
  byte cont = 0;
  byte y = 0;
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
          letra = Matrizletras[(cont -1) % 3][y - 1];
          llenarDatos(letra, '0');
          mySerial.write(letra);
      }
    }
  }
  while(true)
  { /**Escoge el numero **/
    key = kpd.getKey();
    if(key )  // Check for a valid key.
    {
      if(key == '#'){
        mySerial.write(key);
        break;
      }else{
          posAtaqueY = key - '0'; 
          llenarDatos(letra, key);
          mySerial.write(key);
      }
    }
  }
  cambiarMatrizAtaque(letra, posAtaqueY, 2);
  //
}
void cambiarMatrizAtaque(char l, byte n, byte z){ // z = 1 pantalla ataque, z = pantalla defenza
  byte nl = getPosDeLetra(l);
  if(z==1){
    matAtaque[n][nl] = 2;
  }else{
    matAtaque[n][nl] = 1;
  }
}

void cambiarMatrizDefensa(char l, byte n){
  byte nl = getPosDeLetra(l);
  if(posBarcos[n][nl] == 1){
    posBarcos[n][nl] = 2;
  }else{
    posBarcos[n][nl] = 3;
  }
}
byte getPos(byte n , byte fc){
  if(fc == 1) return 18 + (10*n); // quiero saber la fila
  return 31 + (10*n);  
}
void mostrarMisAtaques(){
  byte pos[2];
  for(byte i = 0; i <11; i++){
    for(byte j = 0; j < 11; j++){
      if(matAtaque[i][j] == 1){
          pos[0] = getPos(i,0);
          pos[1] = getPos(j,1);
          creaCirculo(pos[1], pos[0], 1);
      } 
      else if (matAtaque[i][j] == 2){
        pos[0] = getPos(i,0);
        pos[1] = getPos(j,1);
        creaCirculo(pos[1], pos[0], 2);
      } 
    }
  }
}

void mostrarAtaques(){
  byte pos[2];
  for(byte i = 0; i <11; i++){
    for(byte j = 0; j < 11; j++){
      if(posBarcos[i][j] != 1 && posBarcos[i][j] != 0){
          pos[0] = getPos(i,0);
          pos[1] = getPos(j,1);
          creaCirculo(pos[1], pos[0], 1);
      }  
    }
  }
}
void llenarDatos(char l, char n){
  limpiaEntrada();
  tft.setCursor(5,148);
  tft.setTextColor(ST7735_RED);
  tft.print("Ataque->");
  tft.setCursor(70,148);//coord enadas de la posicion 1
  tft.setTextColor(ST7735_BLUE);
  tft.print(l);//estas posiciones se ingresan mediante teclado
  tft.setCursor(95,148);//coordenadas de la posicion 2
  tft.setTextColor(ST7735_BLUE);
  tft.print(n);
}
void crearBarcos()
{
  byte i = 1;
  byte x = 18, y = 31; 
  creaCirculo(x, y,1);
  while (i == 1){
    b = readButton();
    delay(150);
    if(b)
    {
      switch(b){
        case 1:
          y = y + 10;
          break;
        case 5:
          x = x - 10;
          break;
        case 4:
          y = y - 10;
          break;
        case 2:
          x = x + 10;
          break;
        case 3:
          i = 2;
          break;
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
  if (a < 0.2) return 1;
  if (a < 1.0) return 2;
  if (a < 1.5) return 3;
  if (a < 2.0) return 4;
  if (a < 3.2) return 5;
  else return 0;
}


void bmpDraw(char *filename, uint8_t x, uint8_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   
  uint8_t  bmpDepth;              
  uint32_t bmpImageoffset;        
  uint32_t rowSize;               
  uint8_t  sdbuffer[3*20]; 
  uint8_t  buffidx = sizeof(sdbuffer); 
  boolean  goodBmp = false;       
  boolean  flip    = true;        
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  if((x >= tft.width()) || (y >= tft.height())) return;
  
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print("File not found");
    return;
  }
  if(read16(bmpFile) == 0x4D42) { 
    Serial.println(read32(bmpFile));
    (void)read32(bmpFile);
    bmpImageoffset = read32(bmpFile); 
    Serial.println(bmpImageoffset, DEC);
    Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { 
      bmpDepth = read16(bmpFile); 
      Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { 
        goodBmp = true; 
        Serial.print(bmpWidth);
        Serial.println(bmpHeight);
        rowSize = (bmpWidth * 3 + 3) & ~3;
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;
        tft.setAddrWindow(x, y, x+w-1, y+h-1);
        for (row=0; row<h; row++) { 
          if(flip)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else    
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { 
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); 
          }
          for (col=0; col<w; col++) { 
            if (buffidx >= sizeof(sdbuffer)) { 
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0;
            }
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.Color565(r,g,b));
          } 
        } 
        Serial.print(millis() - startTime);
      } 
    }
  }
  bmpFile.close();
  if(!goodBmp) Serial.println("recognized.");
}

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); 
  ((uint8_t *)&result)[1] = f.read(); 
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); 
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); 
  return result;
}
