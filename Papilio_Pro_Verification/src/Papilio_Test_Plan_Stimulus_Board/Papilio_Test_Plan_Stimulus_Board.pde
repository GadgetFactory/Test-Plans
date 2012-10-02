/*
  Gadget Factory
  Papilio One Stimulus Sketch for Verification
 
  Used to test the I/O of new Papilio One boards. This is the sketch that runs on the board built into the Pogo Pin tester.

 created 2010
 by Jack Gassett from existing Arduino code snippets
 http://www.gadgetfactory.net
 
 This example code is in the public domain.
 */
 #define DDRC _SFR_IO8(0x14)
 #define DDRF _SFR_IO8(0x08) 
 #define PORTF _SFR_IO8(0x07)
 int state;
  
void setup() {
     DDRA = 0xFE;
     DDRB = 0xFF;
     DDRC = 0xFF;
     DDRD = 0xFF;
     DDRE = 0xFF;
     DDRF = 0xFF;
}

void loop(){
   
  if (digitalRead(0))
  {
        PORTA = 0xAA;
        PORTB = 0xAA;
        PORTC = 0xAA;
        PORTD = 0xAA;
        PORTE = 0xAA;
        PORTF = 0xAA;     
  }
  else
  {
        PORTA = 0x55;
        PORTB = 0x55;
        PORTC = 0x55;
        PORTD = 0x55;
        PORTE = 0x55;
        PORTF = 0x55; 
  } 

}
