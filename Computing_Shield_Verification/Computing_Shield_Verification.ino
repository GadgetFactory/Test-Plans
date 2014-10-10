#define circuit Shield_Computing

#include "HQVGA.h"
#include "SmallFS.h"
#include "structures.h"

#define FREQ 17000          //Freq for ymplayer
 
#include <SD.h>
#include "SmallFS.h"
#include "modplayer.h"
#include "ramFS.h"
#include "cbuffer.h"

MODPLAYER modplayer;

#define BUTTON_DOWN 20 /* WING_B_10 */
#define BUTTON_UP 18 /* WING_B_9 */
#define BUTTON_LEFT 17 /* WING_B_8 */
#define BUTTON_RIGHT 21 /* WING_B_11 */
#define BUTTON_RES 19 /* Reset */

//#define BUTTON_DOWN WING_B_1 /* WING_B_10 */
//#define BUTTON_UP WING_B_3 /* WING_B_9 */
//#define BUTTON_LEFT WING_B_4 /* WING_B_8 */
//#define BUTTON_RIGHT WING_B_0 /* WING_B_11 */
//#define BUTTON_RES WING_B_2 /* Reset */

#define PS2B_CLK 39
#define PS2A_CLK 5
#define PS2B_DAT 41
#define PS2A_DAT 4

#define JOYSTICKA_1 13
#define JOYSTICKA_2 11
#define JOYSTICKA_3 9
#define JOYSTICKA_4 8
#define JOYSTICKA_5 6
#define JOYSTICKA_6 12
#define JOYSTICKA_7 10
#define JOYSTICKA_9 7

#define JOYSTICKB_1 31
#define JOYSTICKB_2 29
#define JOYSTICKB_3 27
#define JOYSTICKB_4 25
#define JOYSTICKB_5 23
#define JOYSTICKB_6 33
#define JOYSTICKB_7 35
#define JOYSTICKB_9 37

#define LED1 47
#define LED2 49
#define LED3 51
#define LED4 53

int testOut[] = { 
    PS2A_CLK,PS2A_DAT,JOYSTICKA_1,JOYSTICKA_2,JOYSTICKA_3,JOYSTICKA_4,JOYSTICKA_5,JOYSTICKA_6,JOYSTICKA_7,JOYSTICKA_9, LED1, LED2, LED3, LED4};       // Test pins of joystick A and ps/2 A that act as outputs
int outCount = 14;           // the number of pins (i.e. the length of the array)

int testIn[] = { 
    BUTTON_RES,PS2B_CLK,PS2B_DAT,JOYSTICKB_1,JOYSTICKB_2,JOYSTICKB_3,JOYSTICKB_4,JOYSTICKB_5,JOYSTICKB_6,JOYSTICKB_7,JOYSTICKB_9};       // Test pins of joystick and ps/2 that act as inputs
int inCount = 11;           // the number of pins (i.e. the length of the array)

static int tick = 0;
static int tickmax = 10;

unsigned char lval[5],cval[5];
static unsigned long timerTicks = 0;
static int timerMax = 10000;

int ledState = HIGH;

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
	} else if (!lval[4] && cval[4]) {
		ret = event_res;
	}
	*((unsigned*)lval)=*((unsigned*)cval);

	return ret;
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

void processEvent( enum event_t ev )
{

        VGA.setColor(GREEN);

	if (ev==event_up) {
                VGA.printtext(10,0, "Up");
	}
	if (ev==event_left) {
                VGA.printtext(10,10, "Left");
	}
	if (ev==event_right) {
                VGA.printtext(10,20, "Right");
	}
	if (ev==event_res) {
                //VGA.printtext(10,30, "RESET");
                setup();
	}
	if (ev==event_down || tick==tickmax) {
                VGA.printtext(10,30, "Down");
	}
}



void setStatus(int pin, int x, int y,const char *name)
{
        if (digitalRead( pin ) == 1){
          VGA.setColor(GREEN);
          VGA.printtext(x,y, name);}
        else {
          VGA.setColor(RED);
          VGA.printtext(x,y, name);
        }  
}

void _zpu_interrupt()
{
  modplayer.zpu_interrupt();
  timerTicks++;
}


void loop()
{
  enum event_t ev = hasEvent();

  
  if (modplayer.getPlaying() == 1)
    modplayer.audiofill();
  
  if (ev!=event_none) {
    processEvent(ev);
  }

  if (timerTicks > timerMax) {
    timerTicks = 0;
    ledState = !ledState;
    digitalWrite(LED1, ledState);
    digitalWrite(LED2, ledState);
    digitalWrite(LED3, ledState);
    digitalWrite(LED4, ledState); 
    
    setStatus(PS2B_CLK,10, 70, "PS2 CLK");
    setStatus(PS2B_DAT,10, 80, "PS2 DAT");  
  
    setStatus(JOYSTICKB_1, 100, 10, "J1");
    setStatus(JOYSTICKB_2, 100, 20, "J2");
    setStatus(JOYSTICKB_3, 100, 30, "J3");
    setStatus(JOYSTICKB_4, 100, 40, "J4");
    setStatus(JOYSTICKB_5, 100, 50, "J5");
    setStatus(JOYSTICKB_6, 100, 60, "J6");
    setStatus(JOYSTICKB_7, 100, 70, "J7");    
  }      
}

void setup()
{


  // initialize ps2 A and joystick A as HIGH outputs:
  for (int thisPin = 0; thisPin < outCount; thisPin++)  {
    pinMode(testOut[thisPin], OUTPUT); 
    digitalWrite(testOut[thisPin],HIGH);     
  }
  
  // initialize ps2 B, joystick B and pushbutton pins as inputs:
  for (int thisPin = 0; thisPin < inCount; thisPin++)
    pinMode(testIn[thisPin], INPUT); 
  
  VGA.begin(VGAWISHBONESLOT(9),CHARMAPWISHBONESLOT(10));
  	VGA.clear();
  
  	Serial.begin(115200);
  	Serial.println("Starting");
  
  	if (SmallFS.begin()<0) {
  		Serial.println("No SmalLFS found, aborting.");
  		//while(1);
  	}
  
  	blitImage("PapilioImage.dat");
  
  	VGA.setColor(RED);
  VGA.printtext(10,0, "Up");
  VGA.printtext(10,10, "Left");
  VGA.printtext(10,20, "Right");
  VGA.printtext(10,30, "Down");
  
  VGA.printtext(0, 110, "Comp Shield Test");
  
  //Setup timer for YM and mod players, this generates an interrupt at 1700hz
  TMR0CTL = 0;
  TMR0CNT = 0;
  TMR0CMP = ((CLK_FREQ/2) / FREQ )- 1;
  TMR0CTL = _BV(TCTLENA)|_BV(TCTLCCM)|_BV(TCTLDIR)|
  	_BV(TCTLCP0) | _BV(TCTLIEN);
  INTRMASK = BIT(INTRLINE_TIMER0); // Enable Timer0 interrupt
  INTRCTL=1;     
   
  //Set what wishbone slot the audio passthrough device is connected to.
  modplayer.setup(5);
  
  modplayer.loadFile("music.mod");
  modplayer.play(true);

}
