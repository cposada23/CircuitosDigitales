

#include <Servo.h>


#include <Keypad.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include "rgb_lcd.h"

/**
 * para la tarjeta
 */
// If using the breakout with SPI, define the pins for SPI communication.
#define PN532_SCK  (2)
#define PN532_MOSI (3)
#define PN532_SS   (4)
#define PN532_MISO (5)

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield


// Or use this line for a breakout or shield with an I2C connection:
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
// also change #define in Adafruit_PN532.cpp library file
   #define Serial SerialUSB
#endif


//uint8_t keya[6] = { 0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7 };
uint8_t keya[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t success;
uint8_t uidLength;  
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
/**
 *Para la pantalla
 */
rgb_lcd lcd;

int colorR = 200;
int colorG = 100;
int colorB = 255;

/**
 * Para el teclado
 */
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Three columns
// Define the Keymap
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = { 5,4,3,2 };
// Connect keypad COL0, COL1 and COL2 to these Arduino pins.
byte colPins[COLS] = { A1, A0,7,6 }; 

// Create the Keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


//char contrasena [4] = {'0','1','2','3'}; 
String contrasena;
String data = "0123";
//uint8_t pass [6] = {'1','2','3','4','5','6'}; 
uint8_t otro[6];
int contador = 0;

int estado = 0; //Desactivada  en cero
int estadoPuerta = 0; //cero cerrada
int alarmaActiva = 0; //desctiva en 0

int32_t claveMaestra = 2077405938;

/**
 * Para el motor
 */

Servo miServo;
int angulo = 0;


/*int ledVerde = 0;
int tierra = A2;*/
void setup()
{
  /* Setup para la lectura de la tarjeta */
  #ifndef ESP8266
    while (!Serial); // for Leonardo/Micro/Zero
  #endif
  Serial.begin(9600);
  Serial.println("Hello!");
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  nfc.SAMConfig();
  lcd.begin(16, 2);
  pinMode(A2,OUTPUT);
  miServo.attach(8);
  miServo.write(0);
  borrarPantalla();
  delay(1000);
}

void loop()
{
  if(alarmaActiva == 0){
    char key = kpd.getKey();
    if(key)  // Check for a valid key.
    { 
      contador = 0;
      switch (key)
      {
        case 'A':
          if(estado != 1){
            Serial.println("Activar alarma");
            borrarPantalla();
            ingresoDeCodigo("activar alarma.", 8, false);
          }else{
            Serial.println("Alarma ya esta activa");  
          }
          break;
        case 'B':   
          if(estado != 0){
            Serial.println("Desactivar alarma");
            lcd.setRGB(255, 0, 0);
            borrarPantalla();
            ingresoDeCodigo("desactivar alarma.", 9, false);
          }else{
            Serial.println("la alarma ya esta desactivada");
          }    
          break;
        case 'C': 
          if(estadoPuerta != 1){
            Serial.println("Apertura de puerta");
            borrarTodo();
            lcd.home();
            lcd.print("Apertura garaje");
            ingresoDeCodigo("Apertura de garaje",10, true);
          }else{
            Serial.println("La puerta ya esta abierta");  
          }      
          break;
        case 'D':   
          if(estadoPuerta!=0){
            Serial.println("Cerrado de puerta");
            borrarTodo();
            lcd.home();
            lcd.print("Cerrado d garaje");
            ingresoDeCodigo("Cerrado de garaje", 8, true);
          }else{
            Serial.println("La puerta ya esta cerrada");
          }    
          
          break;
        default:
          Serial.println(key);
          break;
          
          
      }
    }
  }else{
      Serial.println("Alarma Activa");
      titilar();
    }
  
}
void borrarTodo(){
  lcd.home();
  lcd.print("                                                                              ");  
}
bool comparar(uint8_t  p1[6], uint8_t p2[6]){
    int i;
    Serial.println("comparando");
    //nfc.PrintHexChar(p1, 6);
    //nfc.PrintHexChar(p2, 6);
    
    Serial.println("p1");
    for( i = 0; i<6; i++){
      Serial.print(p1[i]);  
    }
    Serial.println("");
    Serial.println("p2");
    for( i = 0; i<6; i++){
      Serial.print(p2[i]);  
    }
    for (i = 0; i<6;i++){
      if(p1[i] != p2[i]){
        return false; 
      }  
    }
    return true;
}

void borrarPantalla(){
  lcd.home();
  lcd.print("                                                                      ");  
  lcd.home();
  switch(estado){
    case 0:
      lcd.print("Alarma inactiva");
      lcd.setRGB(0, 255, 0);
      
      break; 
    case 1:
      lcd.print("Alarma activa");
      lcd.setRGB(255, 0, 0);
      
      break; 
  }
  lcd.setCursor(0, 1);
  switch(estadoPuerta){
    case 0:
      lcd.print("Garaje Cerrado");
      break;
    case 1:
      lcd.print("Garaje Abierto");
      break;  
  }
}

void ingresoDeCodigo(String accion, int bloque, bool apertura){
  uint8_t codigo[6];
  int intentos = 0;
  String t = "";
  Serial.println("Ingrese el codigo para "+ accion);
  lcd.home();
  lcd.print("                                                      ");
  lcd.home();
  lcd.print(accion);
  do{
    contador = 0;
    t = "";
    while(contador <6  ){
      char key = kpd.getKey();
      if(key)  // Check for a valid key.
      { 
        Serial.println(key);
        t = t + "*";
        lcd.setCursor(0, 1);
        lcd.print(t);
        codigo[contador] = key;
        contador++;
      }
    }
    if(!lecturaDeTarjeta(bloque, codigo)){
      intentos++;
      error(intentos);
    
    }else{
      break;  
    }
  }while(intentos<3); 

  if(intentos>=3){
    alarmaActiva = 1;
  }else{
    Serial.println("Autenticacion correcta");
    bien();
    if(apertura){
      estadoPuerta = (estadoPuerta + 1) % 2;
      abrirCerrarGaraje();
      borrarPantalla();  
    }else{
      estado = (estado +1) %2;
      borrarPantalla();  
    }
  }
}

bool lecturaDeTarjeta(int bloque, uint8_t pass[6]){
  Serial.println("Lectura de tarjeta ... passs:");
  //nfc.PrintHexChar(pass, 6);
  int i;
  for( i = 0; i<6; i++){
      Serial.print(pass[i]);  
    }
   
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if(success){
    if(uidLength == 4){
      
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, bloque, 0, keya);
      if(success){
      
        uint8_t data[6];
        // Try to read the contents of block 4
        success = nfc.mifareclassic_ReadDataBlock(bloque, data);
        if(success){
         
          //nfc.PrintHexChar(data, 6);
          
          int i;
          for (i = 0; i<6;i++){
            if(pass[i] != data[i]){
            
              
              return false; 
            }  
          }
        
          
          return true;
        }else{
          
          return false; 
        }
      }else{
      
        return false;
      }
    }  
  }
  
  return false;
}

void titilar(){
  bool autentica = false;
  
  lcd.home();
  lcd.print("Colocar Tarjeta");
  lcd.setRGB(255, 255, 0);
  digitalWrite(A2, 1);
  do{
     success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
     if (success) {

    //nfc.PrintHex(uid, uidLength);
    
    if (uidLength == 4)
    {
      // We probably have a Mifare Classic card ... 
      uint32_t cardid = uid[0];
      cardid <<= 8;
      cardid |= uid[1];
      cardid <<= 8;
      cardid |= uid[2];  
      cardid <<= 8;
      cardid |= uid[3]; 
      if(cardid == claveMaestra){
        autentica = true;  
      }
    }

  }
    
  }while(!autentica);
  
 
  
  
  digitalWrite(A2, 0);
  alarmaActiva = 0;
  
  borrarPantalla();
 
}




void error(int intentos){
  String strin = "Intentos ";
  strin.concat(intentos);
  borrarTodo();
  lcd.home();
  lcd.print(strin);
  //lcd.print(intentos);
  int tempo = 300;
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
  playTone(956, tempo*2);
}

void bien(){
  digitalWrite(A2, HIGH);
    delayMicroseconds(1000);
    digitalWrite(A2, LOW);
    delayMicroseconds(100);
  
}

void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(A2, HIGH);
    delayMicroseconds(tone);
    digitalWrite(A2, LOW);
    delayMicroseconds(tone);
  }
}



void abrirCerrarGaraje(){
  int i;
    switch(estadoPuerta){
      case 0:
        angulo = 90;
        for ( i = angulo; i>0; i-=10){
          miServo.write(i);
          lcd.setRGB(255,255,255);
          delay(100);  
          lcd.setRGB(0, 255, 0);
          delay(100);
        }
        break;
      case 1: 
        angulo = 0;
        for ( i = angulo; i<90; i+=10){
          miServo.write(i);
          lcd.setRGB(255,255,255);
          delay(100);  
          lcd.setRGB(255, 0, 0);
          delay(100);
        }
        break;  
    }

    
}

