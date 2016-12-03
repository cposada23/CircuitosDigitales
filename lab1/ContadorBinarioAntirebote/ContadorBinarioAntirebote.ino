volatile int contador = 0;
int l1 = 4;
int l2 = 5;
int l3 = 6;
int l4 = 7; 

int n = contador ;
long T0 = 0;
void setup()
   {   Serial.begin(9600);
       attachInterrupt( 0 , sumar, FALLING);
       attachInterrupt( 1 , sumar, FALLING);
       pinMode(l1, OUTPUT);
       pinMode(l2, OUTPUT);
       pinMode(l3, OUTPUT);
       pinMode(l4, OUTPUT);
       digitalWrite(l1, LOW);
       digitalWrite(l2, LOW);
       digitalWrite(l3, LOW);
       digitalWrite(l4, LOW);
   }
void loop()
   {
      if (n != contador)
          {     
                Serial.println(contador);
                digitalWrite( l4, digitalRead( l4 ) ^ 1 );
                if(contador %2 == 0){
                  digitalWrite( l3, digitalRead( l3 ) ^ 1 );
                  
                }
                if(contador % 4 == 0){
                  digitalWrite( l2, digitalRead( l2 ) ^ 1 );
                }

                if(contador %8 == 0){
                  digitalWrite( l1, digitalRead( l1 ) ^ 1 );
                }
                n = contador ;
          }
   }

void sumar() 
   {    
        contador = (contador +1 )%16;  
        Serial.println("sumar " + contador);
   }

   void restar() 
   {    
        
        if(millis()>T0 +250){
          contador = (contador - 1);
          if(contador < 0 ){
              contador = 15;
            }
          
          T0 = millis();
        }
        
        
   }
