#include <SoftwareSerial.h>
#include <Keypad.h>

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
byte rowPins[ROWS] = { 2,3,6,7 };
// Connect keypad COL0, COL1 and COL2 to these Arduino pins.
byte colPins[COLS] = { A2, A1,A0, 9 }; 

// Create the Keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


SoftwareSerial mySerial(A5, A4); // RX, TX


char estado;
char key;
int turno = 0;
int led = 5;
void setup()
{
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB port only
  }
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);  
  Serial.begin(9600);
  Serial.println("empezo Escoja su turno");
  mySerial.begin(9600);
}

void loop()
{
  
  /** Quien empieza 1 --> yo 2 --> El otro? **/

  while(turno == 0){
    key = kpd.getKey();
    if(key){
      switch(key)
      {
        case '1':
          Serial.println("Jugador 1: Usted empieza");
          turno = 1;
          break;
        case '2':
          Serial.println("Jugador 2: Espere el turno del jugador 1");
          turno = 2;
          break;
        default:
          Serial.println(key);
      }
    }
    
  }

  /** Turno de enviar datos **/
  if(turno == 1){
    digitalWrite(led, HIGH);  
    key = kpd.getKey();
    if(key)  // Check for a valid key.
    {
      switch (key)
      {
        default:
          //mySerial.write("Hola");
          mySerial.write(key);
          Serial.println(key);
          turno = 2;
      }
    } 
  }
  /** turno de recivir datos **/
  else if(turno == 2){
    digitalWrite(led, LOW);
    if (mySerial.available()>0) 
    {       
      /*while(mySerial.available()>0){
        estado = mySerial.read(); 
        Serial.print(estado);
      }*/
      estado = mySerial.read(); 
      Serial.print(estado);
      Serial.println("terminado de recivir data");
      Serial.println("Es su turno de enviar");
       
        
      turno = 1;
    }    
  }
  
}
