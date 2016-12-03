#include <TimerOne.h>

// the setup function runs once when you press reset or power the board

int cont = 1;

void setup() {
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  Serial.begin(9600);
  Serial.println("Empezo");
  Timer1.initialize(500000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here
}

// the loop function runs over and over again forever
void loop() {

       
}
void timerIsr()
{
    // Toggle LED
    digitalWrite( 13, digitalRead( 13 ) ^ 1 );
    cont = ((cont + 1) % 20);
    Serial.println(cont);
    if(cont == 0){
      digitalWrite( 10, digitalRead( 10 ) ^ 1 );
      Serial.println("led 10 cambio");
    }
    
    if (cont % 2 == 0){
     digitalWrite( 12, digitalRead( 12 ) ^ 1 );
      Serial.println("led 12 cambio");
    }

    if (cont % 4 == 0){
      Serial.println("led 11 cambio");
     digitalWrite( 11, digitalRead( 11 ) ^ 1 );
    }

    
}
