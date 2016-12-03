int led = 13;
volatile int contador = 0;
int n = contador; 

void setup() {
  Serial.begin(9600);
  attachInterrupt(0,cambiar , RISING);
  attachInterrupt(1,cambiar, RISING);
  pinMode(led, OUTPUT);
  digitalWrite(led, 1);
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

  if(n!=contador){
    Serial.println(contador);
    n = contador;
  }
}

void cambiar(){
  Serial.println("cambiar");
  contador ++;  
}
