/*  Keypadtest.pde
 *
 *  Demonstrate the simplest use of the  keypad library.
 *
 *  The first step is to connect your keypad to the
 *  Arduino  using the pin numbers listed below in
 *  rowPins[] and colPins[]. If you want to use different
 *  pins then  you  can  change  the  numbers below to
 *  match your setup.
 *
 */
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


uint8_t keya[6] = { 0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7 };
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
uint8_t pass [6] = {'1','2','3','4','5','6'}; 
uint8_t otro[6];
int contador = 0;
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

  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  
  lcd.setRGB(colorR, colorG, colorB);
  
  // Print a message to the LCD.
  lcd.print("Contraseña: ");
  contrasena = "";
  delay(1000);
}

void loop()
{

  
  char key = kpd.getKey();
  
  if(key)  // Check for a valid key.
  {
    
    switch (key)
    {
      case '*':
        if(contrasena == data){
          Serial.println("valida");
        }else{
          Serial.println("No valida");  
        }
        break;
      case '#':
        success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
        if(success){
          if(uidLength == 4){
            Serial.println("Parece ser una MifareClassic.... Autenticando");
            success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);
            if(success){
              Serial.println("Autenticado");
              uint8_t data[6];
              memcpy(data, (const uint8_t[]){ '1', '2', '3', '4' ,'5','6'}, sizeof data);
              success = nfc.mifareclassic_WriteDataBlock (4, data);
      
              // Try to read the contents of block 4
              success = nfc.mifareclassic_ReadDataBlock(4, data);
              if(success){
                Serial.println("leyendo el bloque 4");
                nfc.PrintHexChar(data, 6);
                
                if(comparar(data, pass)){
                  Serial.println("Toda la contraseña autentica");
                }else{
                  Serial.println("no autentica toda la contraseña");  
                }

                
                Serial.println("");
                delay(1000);
              }else{
                Serial.println("Fallo en la lectura del bloque");  
              }
            }else{
              Serial.println("Fallo en la autenticacion");
            }
          }  
        }
        /*Se hace lectura de la tarjeta */
        
        break;
      default:
        otro[contador] = key;
        contrasena = contrasena + key;
        Serial.println(key);
        lcd.setCursor(0, 1);
        lcd.print(contrasena);
        
    }
  }
}

bool comparar(uint8_t  p1[6], uint8_t p2[6]){
    int i;
    for (i = 0; i<6;i++){
      if(p1[i] != p2[i]){
        return false; 
      }  
    }
    return true;
}
