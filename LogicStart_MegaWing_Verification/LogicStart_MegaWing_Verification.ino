//#include <stdio.h>
//#include <stdlib.h>
#include "VGA.h"
#include "SmallFS.h"
#include "structures.h"
#include "cbuffer.h"

// AUDIO STUFF

#define YM2149BASE IO_SLOT(11)
#define YM2149REG(x) REGISTER(YM2149BASE,x)

#define HAVE_YM

struct ymframe {
	unsigned char regval[16];
};
struct pokeyframe {
	unsigned char regval[9];
};
CircularBuffer<ymframe,2> YMaudioBuffer;
CircularBuffer<pokeyframe,2> POKEYaudioBuffer;

SmallFSFile ymaudiofile;
SmallFSFile pokeyaudiofile;

#define AUDIOPIN WING_B_10

#define BUTTON_DOWN  WING_B_13
#define BUTTON_UP WING_B_14
#define BUTTON_LEFT WING_B_12
#define BUTTON_RIGHT WING_B_11
#define BUTTON_RES WING_B_15 /* Reset */

//For SPI ADC
#define SELPIN WING_A_12 //Selection Pin
#define DATAOUT WING_A_14//MOSI
#define DATAIN  WING_A_13//MISO
#define SPICLOCK WING_A_15 //Clock

byte segmentPins[] = {WING_A_7, WING_A_10, WING_A_5, WING_A_6, WING_A_3, WING_A_4, WING_A_9}; // seg a to g

byte digitPins[] = {WING_A_0,WING_A_2,WING_A_8,WING_A_11}; // LSB to MSB

int ledPins[] = { 
   WING_C_8,WING_C_9,WING_C_10,WING_C_11,WING_C_12,WING_C_13,WING_C_14,WING_C_15 };       // an array of pin numbers to which LEDs are attached
int ledCount = 8;           // the number of pins (i.e. the length of the array)

int switchPins[] = { 
   WING_C_0,WING_C_1,WING_C_2,WING_C_3,WING_C_4,WING_C_5,WING_C_6,WING_C_7};       // an array of pin numbers to which Buttons are attached
int switchCount = 8;           // the number of pins (i.e. the length of the array)

char adc;
int val;
char charBuf[12];
char *ts = charBuf;
int count = 1000;

static int tick = 0;
static int tickmax = 10000;

int ledState = HIGH;
int switchState = 0;         // variable for reading the pushbutton status
int thisPin;

unsigned char lval[5],cval[5];

/* This will hold the current game area */
unsigned char area[blocks_x][blocks_y];

static unsigned int timerTicks = 0;

enum event_t hasEvent()
{
	enum event_t ret = event_none;

	cval[0] = digitalRead( BUTTON_DOWN );
	cval[1] = digitalRead( BUTTON_UP );
	cval[2] = digitalRead( BUTTON_LEFT );
	cval[3] = digitalRead( BUTTON_RIGHT );
        cval[4] = digitalRead( BUTTON_RES );

	if (!lval[0] && cval[0] ) {
		ret = event_down;
	} else if (!lval[1] && cval[1]) {
		ret = event_up;
	} else if (!lval[2] && cval[2]) {
		ret = event_left;
	} else if (!lval[3] && cval[3]) {
		ret=  event_right;
	} else if (cval[4] == 0) {
		ret = event_res;
	} 
	*((unsigned*)lval)=*((unsigned*)cval);

	return ret;
}

void getCurrentValue(char *value)
        // subroutine for translation of value into string
{  
     if(val>=1000)        // for values > 999 the first digit is always 1
       {
         value[0]= 49;      // the first digit is 1, ASCII value 49
         val = val - 1000;  
        // from now on we work with the last three digits only
       }
       else
       {
         value[0]= 32;      // the first digit is a space, ASCII value 32
       }
       
       int val_1;       // second digit
       int val_2;       // third digit
       int val_3;       // fourth digit
       
       val_1 = val/100;
         // from values 100-999 only the first digit remains
         // integers forget digits behind the decimal point
         // f.e. 123/100 = 1,23 => 1 remains
       val_2 = (val-(100*val_1))/10;        
         // f.e. (123-(100*1))/10 = 2,3 => 2 remains
       val_3 = val-(100*val_1)-(10*val_2);  
         // f.e. 123-(100*1)-(10*2)= 3
       value[1]= val_1+48;                  
         // character value of 0 = 48
         // f.e. ASCII value of 1 = 49
       value[2]= val_2+48;             // f.e. ASCII value of 2 = 50
     value[3]= val_3+48;             // f.e. ASCII value of 3 = 51      
}     

int read_adc(int channel){
  int adcvalue = 0;
  int temp;
  byte commandbits = 00000000; //command bits - dont care (2), chn (3), dont care (3)

  //allow channel selection
  commandbits=((channel)<<3);
    //Serial.print("CommandBits: ");
  //Serial.print(commandbits, BIN);
  //Serial.print(" ");

  digitalWrite(SELPIN,LOW); //Select adc
  // setup bits to be written
  for (int i=7; i>=3; i--){
    temp = commandbits&1<<i;
    digitalWrite(DATAOUT,temp);
//    Serial.print(temp, DEC);
//    Serial.println(" ");
    //cycle clock
    digitalWrite(SPICLOCK,HIGH);
    digitalWrite(SPICLOCK,LOW);
  }


  //read bits from adc
  for (int i=11; i>=0; i--){
    adcvalue+=digitalRead(DATAIN)<<i;
    //cycle clock
    digitalWrite(SPICLOCK,HIGH);
    digitalWrite(SPICLOCK,LOW);
  }
  
  digitalWrite(SPICLOCK,HIGH);    //ignores 2 null bits
  digitalWrite(SPICLOCK,LOW);
  digitalWrite(SPICLOCK,HIGH);
  digitalWrite(SPICLOCK,LOW);
  
  digitalWrite(SELPIN, HIGH); //turn off device
  return adcvalue;
}

void _zpu_interrupt()
{
	// Play
	if (YMaudioBuffer.hasData()) {
		int i;
		ymframe f = YMaudioBuffer.pop();
		for (i=0;i<16; i++) {
			YM2149REG(i) = f.regval[i];
		}
	}
	timerTicks++;

	TMR0CTL &= ~(BIT(TCTLIF));
}

void clear_area()
{
	memset(&area,sizeof(area),0);
}

void blitImage(const char *name)
{
	VGA.blitStreamInit(0, 0, VGA.getHSize());

	SmallFSFile entry = SmallFS.open(name);
	if (entry.valid()) {
		entry.readCallback( entry.size(), &VGA.blitStream, (void*)&VGA );
	}
}

void entryImage(const char *name)
{
	blitImage(name);
	// Wait for upper key
	event_t ev;
	do {
		ev = hasEvent();
	} while (ev==event_none);

}

void waitTick()
{
#ifdef __linux__
	sync();
	usleep(100000);
#endif
	timerTicks = 0;
	while ( timerTicks < 5 ) {
		audiofill();
	}
}



void processEvent( enum event_t ev )
{

        VGA.setColor(GREEN);

	if (ev==event_up) {
                VGA.printtext(10,10, "Up");
	}
	if (ev==event_left) {
                VGA.printtext(10,20, "Left");
	}
	if (ev==event_right) {
                VGA.printtext(10,30, "Right");
	}
	if (ev==event_res) {
                //VGA.printtext(10,30, "RESET");
                setup();
	}
	if (ev==event_down || tick==tickmax) {
                VGA.printtext(10,40, "Down");
	}
}


void audiofill()
{
	// Check audio
	int r;

#ifdef HAVE_YM
	ymframe f;
	while (!YMaudioBuffer.isFull()) {
		r = ymaudiofile.read(&f.regval[0], 16);
		if (r==0) {
			ymaudiofile.seek(0,SEEK_SET);
			ymaudiofile.read(&f.regval[0], 16);
		}
		YMaudioBuffer.push(f);
	}
#endif
}

void setStatus(int value, int i)
{
        if (value < 300)
          VGA.setColor(GREEN);         
        else
          VGA.setColor(RED);
        getCurrentValue(charBuf);
        VGA.printtext(110,i*10, ts);          
}

void loop()
{
	enum event_t ev = hasEvent();
	tick++;  
  
	audiofill();
	//loop_functions[game_state]();
	if (ev!=event_none) {
		processEvent(ev);
	}

        //Slides
        for (int thisPin = 0; thisPin < switchCount; thisPin++)  {
          switchState = digitalRead(switchPins[thisPin]);
          //switchState = HIGH;
          digitalWrite(ledPins[thisPin], switchState);
        }

        if(tick == tickmax){
          tick = 0;
          for (int i = 1; i < 9; i++){
            val = read_adc(i);
            setStatus(val, i);
          }       
        }
}

 

void setup()
{

	pinMode(AUDIOPIN,OUTPUT);
	digitalWrite(AUDIOPIN,HIGH);
	outputPinForFunction(AUDIOPIN, 8);
	pinModePPS(AUDIOPIN, HIGH);

#ifdef AUDIOPINEXTRA
	pinMode(AUDIOPINEXTRA,OUTPUT);
	outputPinForFunction(AUDIOPINEXTRA, 8);
	pinModePPS(AUDIOPINEXTRA, HIGH);
#endif

        //Setup Joystick
        //pinMode(BUTTON_SEL, INPUT); 
        pinMode(BUTTON_UP, INPUT); 
        pinMode(BUTTON_DOWN, INPUT); 
        pinMode(BUTTON_LEFT, INPUT); 
        pinMode(BUTTON_RIGHT, INPUT);
        pinMode(BUTTON_RES, INPUT); 
       
        //Setup SPI ADC
        //set pin modes
        pinMode(SELPIN, OUTPUT);
        pinMode(DATAOUT, OUTPUT);
        pinMode(DATAIN, INPUT);
        pinMode(SPICLOCK, OUTPUT);
        //disable device to start with
        digitalWrite(SELPIN,HIGH);
        digitalWrite(DATAOUT,LOW);
        digitalWrite(SPICLOCK,LOW); 
        
        // initialize the LED pins as an output:
        for (int thisPin = 0; thisPin < ledCount; thisPin++)  {
          pinMode(ledPins[thisPin], OUTPUT);  
        }
        
        // initialize the slides as inputs:
        for (int thisPin = 0; thisPin < ledCount; thisPin++)  {
          pinMode(switchPins[thisPin], INPUT);      
        }  
        
//        Multiplex7Seg::set(1, 4, digitPins, segmentPins); // initialize 
//        Multiplex7Seg::loadValue(0001); // load any int value          

	VGA.clear();

	Serial.begin(115200);
	Serial.println("Starting");

	if (SmallFS.begin()<0) {
		Serial.println("No SmalLFS found, aborting.");
		//while(1);
	}


//	entryImage("eimage.dat");
//	entryImage("eimage2.dat");


	blitImage("PapilioImage.dat");
	ymaudiofile = SmallFS.open("audio.dat");

	clear_area();
	//board_draw();
	VGA.setColor(WHITE);
        VGA.printtext(0,0, "Joystick");
        VGA.printtext(0, 110, "LogicStart Test Plan");
        VGA.printtext(100,0, "ADC");
        VGA.printtext(80,10, "Ch0:");
        VGA.printtext(80,20, "Ch1:");
        VGA.printtext(80,30, "Ch2:");
        VGA.printtext(80,40, "Ch3:");
        VGA.printtext(80,50, "Ch4:");
        VGA.printtext(80,60, "Ch5:");
        VGA.printtext(80,70, "Ch6:");
        VGA.printtext(80,80, "Ch7:");       
        VGA.setColor(RED);
        VGA.printtext(10,10, "Up");
        VGA.printtext(10,20, "Left");
        VGA.printtext(10,30, "Right");
        VGA.printtext(10,40, "Down");
       
	INTRMASK = BIT(INTRLINE_TIMER0); // Enable Timer0 interrupt

	INTRCTL=1;

	// Start timer, 50Hz (prescaler 64)
	TMR0CMP = (CLK_FREQ/(50U*64))-1;
	TMR0CNT = 0x0;
	TMR0CTL = BIT(TCTLENA)|BIT(TCTLCCM)|BIT(TCTLDIR)|BIT(TCTLCP2)|BIT(TCTLCP0)|BIT(TCTLIEN);
}
