/*
  Gadget Factory
  Papilio One Board Under Test Sketch for Verification
 
  Used to test the I/O of new Papilio One boards. This is the sketch that runs on the board that is being tested by the Pogo Pin tester.

 created 2010
 by Jack Gassett from existing Arduino code snippets
 http://www.gadgetfactory.net
 
 This example code is in the public domain.
 */
 #define DDRC _SFR_IO8(0x14)
 #define PINC _SFR_IO8(0x13)
 #define DDRF _SFR_IO8(0x08) 
 #define PORTF _SFR_IO8(0x07)
 int incomingByte;
 int state = 0;
 int status;
  
void setup() {
     DDRA = 0x01;
     DDRB = 0x00;
     DDRC = 0x00;
     DDRD = 0x00;
     DDRE = 0x00;
     DDRF = 0x00;

    
     digitalWrite(0, state);
     Serial.begin(9600);  
      Serial.println("Press any key to begin I/O testing");     
}

void testport(volatile uint8_t* port, char pName[2], uint8_t check1, uint8_t check2) {
    status = 0;
    int temp;
    digitalWrite(0, LOW);
    delay(10);
      //Serial.println(*port, HEX);    
    if (*port == check1) {
      status = 1;
    }
    else
      temp = *port;
    digitalWrite(0, HIGH);
    delay(10);
      //Serial.println(*port, HEX);    
    if (*port == check2 && status==1){
      Serial.print(pName);
      Serial.println(" Passed");}
    else{
      Serial.print(pName);
      Serial.println(" Failed");
      Serial.print("Should be:");
      Serial.print(check1, HEX);
      Serial.print(" ");
      Serial.print(check2, HEX);
      Serial.print(" "); 
      Serial.print(check1, BIN);
      Serial.print(" ");
      Serial.println(check2, BIN);      

      Serial.print("Actual   :");
      Serial.print(temp, HEX);
      Serial.print(" ");
      Serial.print(*port, HEX);
      Serial.print(" "); 
      Serial.print(temp, BIN);
      Serial.print(" ");
      Serial.println(*port, BIN);       
      Serial.println();    
    }
}

void loop(){
  if (Serial.available() > 0) {  
    Serial.flush();
    
    testport(&PINA, "AL", 0x54, 0xAB);
    testport(&PINB, "AH", 0x55, 0xAA);
    testport(&PINC, "BL", 0x55, 0xAA);
    testport(&PIND, "BH", 0x55, 0xAA);
    testport(&PINE, "CL", 0x55, 0xAA);
    testport(&PINF, "CH", 0x55, 0xAA);     
     
    digitalWrite(0, LOW);
    delay(10);
    //Serial.print("PortA: ");
    Serial.print(PINA, HEX);
    Serial.print(" ");
    Serial.print(PINB, HEX);
    Serial.print(" ");
    Serial.print(PINC, HEX);
    Serial.print(" ");
    Serial.print(PIND, HEX);
    Serial.print(" ");
    Serial.print(PINE, HEX);
    Serial.print(" ");
    Serial.println(PINF, HEX); 

    digitalWrite(0, HIGH);
    delay(10);
    //Serial.print("PortA: ");
    Serial.print(PINA, HEX);
    Serial.print(" ");
    Serial.print(PINB, HEX);
    Serial.print(" ");
    Serial.print(PINC, HEX);
    Serial.print(" ");
    Serial.print(PIND, HEX);
    Serial.print(" ");
    Serial.print(PINE, HEX);
    Serial.print(" ");
    Serial.println(PINF, HEX); 
    Serial.println("Serial Testing is completed.");
    Serial.println();    
    Serial.println("Press any key to begin I/O testing");    
  } 

}
