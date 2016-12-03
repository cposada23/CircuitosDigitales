#include <TimerOne.h>


int a = A0;
int b = A1;
int c = A2;
int d = A3;
int contador = 9;
int n = contador;
void setup() {
  // put your setup code here, to run once:

  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  digitalWrite(a, 1);
  digitalWrite(b, 0);
  digitalWrite(c, LOW);
  digitalWrite(d, 1);
  Serial.begin(9600);
  
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(conteoRegresivo);
  
}

void loop() {
  
    if (n != contador)
          {     
                
                binario(contador);
                n = contador ;
          }
}

void conteoRegresivo(){
    contador--;
    if(contador < 0) contador = 9;
}

void binario(int num){
  
  int bin[4];
  bin[3] = num %2;
  
  num = num/2;
  bin[2] = num%2;
  num = num/2;
  bin[1] = num%2;
  num = num/2;
  bin[0] = num%2;

  Serial.println(bin[3]);
  Serial.println(bin[2]);
  Serial.println(bin[1]);
  Serial.println(bin[0]);
  digitalWrite( a, bin[3] );
  digitalWrite( b, bin[2] );
  digitalWrite( c, bin[1] );
  digitalWrite( d, bin[0] );  
}

