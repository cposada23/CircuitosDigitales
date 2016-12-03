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

char Matrizletras[3][4] = {
  {' ','A','D','G'},
  {' ','B','E','H'},
  {' ','C','F',' '}
};

char lugar [1][2];


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
int letra = 0;
int numero = 0;
int num = 0;
int contador = 0;
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

     /** Escoger letra **/
    while(letra == 0)
    {
      key = kpd.getKey();
      if(key)  // Check for a valid key.
      {
        switch (key)
        {
          case '*':
            Serial.println("Terminado de escoger letra");
            letra = 1;
          default:
            mySerial.write(key);
            Serial.println(key);
        }
      }
    }

    /** Escoger Número **/
    while(numero == 0)
    {
      key = kpd.getKey();
      if(key) 
      {
        switch (key)
        {
          case '#':
            Serial.println("Terminado de escoger numero");
            numero = 1;
            turno = 2;
          default:
            mySerial.write(key);
            Serial.println(key);
        }
      }
    }
    
     
  }
  /** turno de recivir datos **/
  else if(turno == 2){
    digitalWrite(led, LOW);
    if (mySerial.available()>0) 
    {       
      
      estado = mySerial.read();
      if(estado != '*' && estado!= '#'){
        num  = estado - '0'; 
        contador++; 
      }
      if(estado == '*'){
        Serial.println("letra recivida"); 
        Serial.println(num); 
        if((num - 1) == 3){
          contador = (contador - 1 ) % 2;
        }else{contador = (contador -1) % 3;}
        Serial.println(contador);
        lugar[0][0] = Matrizletras[contador][num - 1];
        Serial.println(lugar[0][0]);
      }
      if(estado == '#'){
        Serial.println("numero recivido");
        Serial.println("Es su turno de enviar");
        turno = 1;
        letra = 0;
        numero = 0;
        contador = 0;
      } 
      Serial.print(estado);
    }    
  }
  
}
