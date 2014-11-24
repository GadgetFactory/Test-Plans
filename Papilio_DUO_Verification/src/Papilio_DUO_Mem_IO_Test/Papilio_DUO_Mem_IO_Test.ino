/*
  Gadget Factory
  Papilio Pro QuickStart Example
 
 Pressing any of the 4 pushbuttons on the Button/LED Wing will light the corresponding LED. 
 
 BPW5007 Button/LED Wing Reference:
 http://www.gadgetfactory.net/gf/project/bpw5007-butnled/
 
 Hardware:
 * Connect a Button/LED Wing to any available Wing Slot

 created 2010
 by Jack Gassett from existing Arduino code snippets
 http://www.gadgetfactory.net
 
 11/19/2012 Jack Gassett
   -Added LED1 blinking
 
 This example code is in the public domain.
 */

// variables will change:
int thisPin;
int incomingByte = 0;
int pinCount=54;
boolean ioPass = true;
int previousPinState = LOW;

void setup() {

  for (int thisPin = 1; thisPin < pinCount; thisPin++)  {
    pinMode(thisPin, INPUT);
  }   
  
  pinMode(0, OUTPUT);   
  
  //Setup Serial port and send out Title
  Serial.begin(9600); 

  //Start with a Mem Test
  //memtest();
  //ioTest();
}

void ioTest()
{
  Serial.println("Starting I/O Test");
  Serial.println("The I/O Test will only be succesful if a stimulus board is connected.");
  Serial.println("Do not be alarmed by a failure if a stimulus board is not connected."); 
  
  for (int thisPin = 1; thisPin < 11; thisPin++)  {
    digitalWrite(0, HIGH);
    delay(10);
    previousPinState = digitalRead(thisPin);
    digitalWrite(0, LOW);
    delay(10);
    if (digitalRead(thisPin)==previousPinState){  //The pin should have toggled when we changed pin 0, if we read the same value then it did not toggle and is a failure
      Serial.print("I/O pin ");
      Serial.print(thisPin);
      Serial.println(" failed please check for any shorts or disconnects.");
      ioPass = false;
    }    
  }   
 
  for (int thisPin = 14; thisPin < pinCount; thisPin++)  {
    digitalWrite(0, HIGH);
    delay(10);
    previousPinState = digitalRead(thisPin);
    digitalWrite(0, LOW);
    delay(10);
    if (digitalRead(thisPin)==previousPinState){  //The pin should have toggled when we changed pin 0, if we read the same value then it did not toggle and is a failure
      Serial.print("I/O pin ");
      Serial.print(thisPin);
      Serial.println(" failed please check for any shorts or disconnects.");
      ioPass = false;
    }    
  }   
  
  if (ioPass)
    Serial.println("IO Test Passed!");
  else
    Serial.println("IO Test Failed, please check the pins that reported a failure!");  
  ioPass = true;
  
  Serial.println();
}

void printnibble(unsigned int c)
{
	c&=0xf;
	if (c>9)
		Serial.write(c+'a'-10);
	else
		Serial.write(c+'0');
}

void printhexbyte(unsigned int c)
{
	printnibble(c>>4);
	printnibble(c);
}


void printhex(unsigned int c)
{
	printhexbyte(c>>24);
	printhexbyte(c>>16);
	printhexbyte(c>>8);
	printhexbyte(c);
}

void memfill(volatile unsigned*end)
{
	volatile unsigned *c = (volatile unsigned*)0x2000;
	while (c<end) {
		*c = (unsigned)c ^ 0xffffbeef;
        c++;
	}
}

int memcheck(volatile unsigned *end)
{
	volatile unsigned *c = (volatile unsigned*)0x2000;
	while (c<end) {
		unsigned v = (unsigned)c ^ 0xffffbeef;
		if (v !=*c) {
			Serial.print("Error at address "); printhex((unsigned)c); Serial.println("");
			return -1;
		}
		c++;
	}
    return 0;
}

void memtest()
{
	volatile unsigned *ptr = (volatile unsigned *)0x2000;

	Serial.println("Starting memory test from 0x2000 and beyond");
	unsigned v = *((unsigned*)0);
	Serial.print("check value is 0x"); printhex(v); Serial.println("");

	int i;
	for (;;) {
		if (*ptr == v) {
			Serial.print("Found tag at "); printhex((unsigned)ptr); Serial.println("");
			Serial.println("Testing up to that. Filling...");
			memfill(ptr);
			Serial.println("Checking...");
			if (memcheck(ptr)==0) {
				Serial.println("All OK!");
			}
			break;
		}
		ptr++;
	}
        Serial.println();
}

void loop(){
  delay(1000);
  memtest();
  delay(20);
  ioTest();

}
