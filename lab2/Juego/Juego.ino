#include <TimerOne.h>

volatile int contador = 0;
int LED1 = 4;
int LED2 = 5;
int n = contador +1;
long T0 = 0;
long T1 = 0;
char opcion;
int conteo = 0;

bool jugando = false;

/* leds para el umbral */
int l31 =7;
int l21 =8;
int l11 =9;
int empate = 10;
int l12 = 11;
int l22 = 12;
int l32 = 13;

/* SALIDAS PARA EL 7-SEGMENTOS */
int a = A0;
int b = A1;
int c = A2;
int d = A3;
int tiempo = 0;
int cambio = 0;



void setup() {
  /* Inicio el puerto serial */
  Serial.begin(9600);
  Serial.println("Empezó");
  attachInterrupt(0, jugador1, FALLING); //Interrupcion para el jugador 1
  attachInterrupt(1, jugador2, FALLING); //Interrupcion para el jugador 2
  /* Cofigurar los puertos de salida */
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(l11, OUTPUT);
  pinMode(l31, OUTPUT);
  pinMode(l21, OUTPUT);
  pinMode(empate, OUTPUT);
  pinMode(l12, OUTPUT);
  pinMode(l22, OUTPUT);
  pinMode(l32, OUTPUT);
  
  /* Se inicializan en LOW */
  digitalWrite(a, 0);
  digitalWrite(b, 0);
  digitalWrite(c, 0);
  digitalWrite(d, 0);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);

  /* Interrupciones por tiempo */
  Timer1.initialize(1000000); // Cada segundo
  Timer1.attachInterrupt(conteoRegresivo);
}

void loop() {
  if(!jugando){
    if(Serial.available()>0){
      opcion = Serial.read();
      empezar(opcion);
    }
  }


  if(jugando){
    if(n != contador){
      Serial.println(contador);
      umbral();
      n = contador;
    }  

    if(cambio != tiempo){
      cambiar(tiempo);
      cambio = tiempo;  
    }
  }
  
}

/**
 * Funcione que convierte un muero a binario
 */
void cambiar(int tiempo){
  
  int bin[4];
  for(int i = 3; i>=0;i--){
    bin[i] = tiempo %2;
    tiempo = tiempo/2;
  }
  digitalWrite( a, bin[3] );
  digitalWrite( b, bin[2] );
  digitalWrite( c, bin[1] );
  digitalWrite( d, bin[0] );  
  
}

void umbral(){
  if (contador == 0){
    digitalWrite(empate, 1);
    digitalWrite(l12, 0);
    digitalWrite(l22, 0);
    digitalWrite(l32, 0);
    digitalWrite(l11, 0);
    digitalWrite(l21, 0);
    digitalWrite(l31, 0);
  }else{
    digitalWrite(empate, LOW);
    if(contador > 0 && contador <=3){
       digitalWrite(l12, HIGH);
       digitalWrite(l22, LOW);
       digitalWrite(l32, LOW);
    }else if(contador>3 && contador <= 5){
       digitalWrite(l12, LOW);
       digitalWrite(l22, HIGH);
       digitalWrite(l32, LOW);
    }else if(contador > 5){
      digitalWrite(l12, LOW);
       digitalWrite(l22, LOW);
       digitalWrite(l32, HIGH);
    }else if(contador <0 && contador >=-3){
        digitalWrite(l11, HIGH);
       digitalWrite(l21, LOW);
       digitalWrite(l31, LOW);
    }else if(contador < -3 && contador >=-5){
       digitalWrite(l11, LOW);
       digitalWrite(l21, HIGH);
       digitalWrite(l31, LOW);
    }else if(contador < -5){
      digitalWrite(l11, LOW);
       digitalWrite(l21, LOW);
       digitalWrite(l31, HIGH);
    }
    
  }
}

void empezar(char opt){
  contar(3);
  Serial.println("empieza juego");
  jugando = true;
  Serial.println(opt); 
  
  tiempo = opt - '0';
  cambio = tiempo +1;
  contador = 0;
  n = contador +1;
  
  Serial.println(tiempo); 
}

void contar(int conteo){

  while(conteo>=0){
    cambiar(conteo);
    delay(1000);  
    conteo--;
  }
  
  
}


void jugador1(){
  if(millis()> T0 +250 && jugando){
    Serial.println("Jugador1");
    contador --;
    digitalWrite(LED1, digitalRead(LED1)^1);
    T0 = millis();

  }
}

void jugador2(){
  if(millis()> T1 +250 && jugando){
    Serial.println("Jugador2");
    contador ++;
    digitalWrite(LED2, digitalRead(LED2)^1);
    T1 = millis();
  }  
}

void conteoRegresivo(){
  if(tiempo>0){
    tiempo --;
  }else{
    jugando = false;  
  }

  if(conteo > 0){
    conteo --;  
  }
}


