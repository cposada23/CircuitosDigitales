#include <SoftwareSerial.h>
char estado;
int led = 5; //Led connected to pin 13
const int Pulsador = 4;  // Pin digital para el pulsador
int estadoPulsador = 0;  // Variable para ver el estado del pulsador

SoftwareSerial mySerial(11, 12); // RX, TX

void setup()
{
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB port only
  }
  Serial.begin(9600);
  mySerial.begin(4800);
  pinMode(led, OUTPUT);
  pinMode(Pulsador, INPUT);  // Pin digital 4 como entrada  
}

void loop()
{
  
 
    estadoPulsador = digitalRead(Pulsador);
    if(estadoPulsador == HIGH)
    {
      mySerial.write(1);
      Serial.println("Encienda la otra tarjeta");
    }
    else 
    {
      mySerial.write(2);
      Serial.println("Apague la otra tabla");
    }    
     
    if (mySerial.available()>0) 
    {           
      // Almaceno el carácter que llega por el puerto serie (RX) 
      estado = mySerial.read();  
      // Si es una 'H'
      if (estado == 1)          
      {
        // Enciendo el LED (nivel ALTO)
        digitalWrite(led, HIGH);    

        Serial.println("Encendido por otro tablero");   
      } 
      else         
      {
        // Apago el LED (nivel BAJO)
        digitalWrite(led, LOW);    
        Serial.println("Apagado por otro tablero");
      }
    }
}
