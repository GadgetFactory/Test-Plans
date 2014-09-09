/*
  Gadget Factory
  Papilio One Stimulus Sketch for Verification
 
  Used to test the I/O of new Papilio One boards. This is the sketch that runs on the board built into the Pogo Pin tester.

 created 2010
 by Jack Gassett from existing Arduino code snippets
 http://www.gadgetfactory.net
 
 This example code is in the public domain.
 */

 int state;
 int pinCount=53;
  
void setup() {
//     DDRA = 0xFE;
//     DDRB = 0xFF;
//     DDRC = 0xFF;
//     DDRD = 0xFF;
//     DDRE = 0xFF;
//     DDRF = 0xFF;

//pinMode(13, OUTPUT); 
//pinMode(5, OUTPUT); 
     
  for (int thisPin = 1; thisPin < pinCount; thisPin++)  {
    pinMode(thisPin, OUTPUT);  
  }
  
//  for (int thisPin = 1; thisPin < pinCount; thisPin++)  {  
//    digitalWrite(thisPin, HIGH);
//  }  
//  
  pinMode(0,INPUT);
   
     
}

void loop(){
   
  
//  digitalWrite(13, HIGH);
//  digitalWrite(5, HIGH);
  
//  for (int thisPin = 1; thisPin < pinCount; thisPin++)  {  
//    digitalWrite(thisPin, HIGH);
//  }    
  
  if (digitalRead(0))
  {
//        PORTA = 0xAA;
//        PORTB = 0xAA;
//        PORTC = 0xAA;
//        PORTD = 0xAA;
//        PORTE = 0xAA;
//        PORTF = 0xAA; 
          for (int thisPin = 1; thisPin < pinCount; thisPin=thisPin+2)  {
            digitalWrite(thisPin, HIGH);  
            digitalWrite(thisPin-1, LOW);
          }
  }
  else
  {
          for (int thisPin = 1; thisPin < pinCount; thisPin=thisPin+2)  {
            digitalWrite(thisPin, LOW);  
            digitalWrite(thisPin-1, HIGH);
          }
//        PORTA = 0x55;
//        PORTB = 0x55;
//        PORTC = 0x55;
//        PORTD = 0x55;
//        PORTE = 0x55;
//        PORTF = 0x55; 
  } 

}
