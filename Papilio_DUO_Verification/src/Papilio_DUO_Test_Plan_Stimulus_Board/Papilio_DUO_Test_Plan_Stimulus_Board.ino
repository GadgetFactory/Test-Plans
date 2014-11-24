/*
  Gadget Factory
  Papilio One Stimulus Sketch for Verification
 
  Used to test the I/O of new Papilio One boards. This is the sketch that runs on the board built into the Pogo Pin tester.

 created 2010
 by Jack Gassett from existing Arduino code snippets
 http://www.gadgetfactory.net
 
 This example code is in the public domain.
 */

int pinCount=54;
  
void setup() {     
  for (int thisPin = 1; thisPin < pinCount; thisPin++)  {
    pinMode(thisPin, OUTPUT);  
  }

  pinMode(0,INPUT);
  
  //Disable the ISP pins
  pinMode(11,INPUT);
  pinMode(12,INPUT);
  pinMode(13,INPUT);
   
     
}

void loop(){
  if (digitalRead(0))
  {
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
  } 

}
