


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


void setup() {
  /* Configuraciones iniciales */
  Serial.begin(9600);
  Serial.println("empezo");
}

void loop() 
{
      
  char key = kpd.getKey();
  if(key)  // Check for a valid key.
  {
    switch (key)
    {
      default:
        Serial.println(key);
    }
  }     
}










  
