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
bool empezar1 = false;  // True -> Ha terminado de escoger los barcos
char Matrizletras[3][5] = { // PARA obtener la letra dependiendo del numero de veces que se unda una tecla
  {' ','A','D','G','J'},
  {' ','B','E','H','K'},
  {' ','C','F','I',' '}
};
byte pos[4];
byte posAux[2];
byte hit;
byte hits = 0;
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
  if(!empezar1){
    //Serial.println("escoger pos de barcos");
    crearBarcos();
    empezar1 = true;
  }
  else{
      /** Quien empieza 1 --> yo 2 --> El otro? **/
      while(turno == 0){
        key = kpd.getKey();
        if(key){
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
        mostrarBarcos();
        mostrarAtaques();
        tft.setCursor(55,70);
        tft.setTextColor(ST7735_RED);
        tft.print(hits);
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
            hit = cambiarMatrizDefensa(letra, numero);
            if(hit == 1){
              hits--;
              
              tft.setCursor(55,70);
              tft.setTextColor(ST7735_RED);
              tft.print(hits);
              mySerial.write(hit);
            }
            
            delay(900);
            turno = 1;
            if(hits == 0){
              tft.setCursor(55,70);
              tft.setTextColor(ST7735_RED);
              tft.print("PERDISTE....");
              delay(1000);
              turno =3;
            }else{
              bmpDraw("fondoAtc.bmp", 0, 0);
            }
            
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

void creaBarcos(byte x1,byte y1,byte x2,byte y2)
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

void escogerPos(byte posi)
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
        break;
      }else{
          cont++;
          y  = key - '0'; 
          letra = Matrizletras[(cont -1) % 3][y - 1];
          llenarDatos2(letra, 'x',posi);
          posAux[0]=letra;
          
      }
    }
  }
  while(true)
  { /**Escoge el numero **/
    key = kpd.getKey();
    if(key )  // Check for a valid key.
    {
      if(key == '#'){
        break;
      }else{
        if(key == 'D')
        {
          key = '1';
        }       
          posAtaqueY = key - '0'; 
          llenarDatos2(letra, key,posi);
          posAux[1]=posAtaqueY;
      }
    }
  }
}

void llenarDatos2(char l, char n, byte posi){
  String text;
  text = l;
  text = text + n;
  if(posi==1)
  {
    tft.fillRect(70, 148,40, 9, ST7735_WHITE);
    tft.setCursor(70,148);//coord enadas de la posicion 1
  }else{
    tft.fillRect(95, 148,20, 9, ST7735_WHITE);
    tft.setCursor(95,148);//coordenadas de la posicion 2
  }
  tft.setTextColor(ST7735_BLUE);
  tft.print(text);
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


  tft.setCursor(55,70);
  
  delay(900);
  if(mySerial.available()>0){
    estado = mySerial.read();
    tft.setTextColor(ST7735_RED);
    tft.print("HITTTTTtttt");
    cambiarMatrizAtaque(letra, posAtaqueY, 2);
    
  }else{
    tft.setTextColor(ST7735_BLUE);
    tft.print("NO  HITTTTT");
    cambiarMatrizAtaque(letra, posAtaqueY, 1);
  }
  delay(900);
  /**while(true){
    
    if(mySerial.available()>0){
      estado = mySerial.read();
      if(estado == '1'){
        //hit
        tft.fillCircle(78, 78, 10, ST7735_RED);
        cambiarMatrizAtaque(letra, posAtaqueY, 2);
      }
      else{
        cambiarMatrizAtaque(letra, posAtaqueY, 2);
      }
      break;
    }
    
  }*/
  
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

byte cambiarMatrizDefensa(char l, byte n){
  byte nl = getPosDeLetra(l);
  byte i;
  if(posBarcos[n][nl] == 1){
    //hit
    posBarcos[n][nl] = 2;
    i = 1;
  }else{
    //Not hit
    posBarcos[n][nl] = 3;
    i = 2;
  }
  return i;
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
  byte empezar = 0;
  tft.setCursor(5,5);
  tft.setTextColor(ST7735_BLACK);
  tft.print("Ubicando barcos...");
  tft.setCursor(5,140);
  tft.setTextColor(ST7735_BLACK);
  tft.print("Tipo:");
  tft.setCursor(70,140);//coordenadas de la posicion 1
  tft.setTextColor(ST7735_GREEN);
  tft.print("Pos1");
  tft.setCursor(95,140);//coordenadas de la posicion 2
  tft.setTextColor(ST7735_GREEN);
  tft.print("Pos2");      
  while(empezar <1)
  {
    limpiaEntrada();
    tft.setCursor(5,148);
    tft.setTextColor(ST7735_RED);
    switch(empezar)
    {
      case 0:
        tft.print("Lancha1");
      break;
      /*case 1:
        tft.print("Lancha2");
      break;
      case 2:
        tft.print("Fragatas");
      break;*/
    }
    tft.setCursor(70,148);//coordenadas de la posicion 1
    tft.setTextColor(ST7735_BLUE);
    escogerPos(1);
    pos[0]=getPos(getPosDeLetra(posAux[0]),1); //letra
    
    pos[1]=getPos(posAux[1],0);//numero
    escogerPos(2);
    pos[2]=getPos(getPosDeLetra(posAux[0]),1); //letra
    pos[3]=getPos(posAux[1],0);//numero
    creaBarcos(pos[0],pos[1],pos[2],pos[3]);
    llenarMatrizBarcos(pos[0],pos[1],pos[2],pos[3]);
    empezar++;
  }
}

void llenarMatrizBarcos(byte x1, byte y1, byte x2, byte y2){
    
    byte i;
    x1 = ((x1-8)/10) -1;
    x2 = ((x2-8)/10) -1;
    y1 = ((y1-21)/10) -1;
    y2 = ((y2-21)/10) -1;
    if(x1 == x2){
      for(i = y1;i<=y2;i++){
          posBarcos[i][x1] =1;
          hits++;
      } 
    }else if (y1== y2){
      for(i = x1;i<=x2;i++){
          posBarcos[y1][i] =1;
          hits++;
      }  
    }
    
}

void mostrarBarcos(){
  byte i = 0, j = 0;
  byte pos[2];
  for (i = 0; i < 11 ;  i++){
    for(j= 0;j<11;j++){
      if(posBarcos[i][j] == 1 || posBarcos[i][j] == 2){ 
        pos[0] = getPos(i,0);
        pos[1] = getPos(j,1);
        creaCirculo(pos[1], pos[0], 3);
      }
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
    tft.drawCircle(x, y, 3, ST7735_RED);
    tft.drawLine(x+4,y,x-4,y,ST7735_RED);
    tft.drawLine(x,y+4,x,y-4,ST7735_RED);  
  }
  else if(acerto == 2)
  {
    tft.drawCircle(x, y, 3, ST7735_BLUE);
    tft.drawLine(x+4,y,x-4,y,ST7735_BLUE);
    tft.drawLine(x,y+4,x,y-4,ST7735_BLUE);
  }else if(acerto == 3){
    tft.fillCircle(x, y, 3, ST7735_BLUE);
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
