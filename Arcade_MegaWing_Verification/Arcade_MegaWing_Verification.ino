#include "VGA.h"
#include "SmallFS.h"
#include "structures.h"
#include "cbuffer.h"

// AUDIO STUFF

#define YM2149BASE IO_SLOT(11)
#define YM2149REG(x) REGISTER(YM2149BASE,x)

//#define HAVE_POKEY
#define HAVE_YM

#define POKEYBASE IO_SLOT(12)
#define POKEYREG(x) REGISTER(POKEYBASE,x)

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

#define AUDIOPIN WING_A_15
#define AUDIOPINEXTRA WING_A_14

#define BUTTON_DOWN 26 /* WING_B_10 */
#define BUTTON_UP 25 /* WING_B_9 */
#define BUTTON_LEFT 24 /* WING_B_8 */
#define BUTTON_RIGHT 27 /* WING_B_11 */
#define BUTTON_RES 11 /* Reset */

#define PS2B_CLK 13
#define PS2A_CLK 33
#define PS2B_DAT 12
#define PS2A_DAT 32

#define JOYSTICKA_1 40
#define JOYSTICKA_2 42
#define JOYSTICKA_3 43
#define JOYSTICKA_4 45
#define JOYSTICKA_5 47
#define JOYSTICKA_6 41
#define JOYSTICKA_8 44
#define JOYSTICKA_9 46

#define JOYSTICKB_1 28
#define JOYSTICKB_2 30
#define JOYSTICKB_3 31
#define JOYSTICKB_4 1
#define JOYSTICKB_5 3
#define JOYSTICKB_6 29
#define JOYSTICKB_8 0
#define JOYSTICKB_9 2

#define LED1 7
#define LED2 6
#define LED3 5
#define LED4 4

int testOut[] = { 
    PS2A_CLK,PS2A_DAT,JOYSTICKA_1,JOYSTICKA_2,JOYSTICKA_3,JOYSTICKA_4,JOYSTICKA_5,JOYSTICKA_6,JOYSTICKA_8,JOYSTICKA_9, LED1, LED2, LED3, LED4};       // Test pins of joystick A and ps/2 A that act as outputs
int outCount = 14;           // the number of pins (i.e. the length of the array)

int testIn[] = { 
    BUTTON_RES,PS2B_CLK,PS2B_DAT,JOYSTICKB_1,JOYSTICKB_2,JOYSTICKB_3,JOYSTICKB_4,JOYSTICKB_5,JOYSTICKB_6,JOYSTICKB_8,JOYSTICKB_9};       // Test pins of joystick and ps/2 that act as inputs
int inCount = 11;           // the number of pins (i.e. the length of the array)

static int tick = 0;
static int tickmax = 10;

int ledState = HIGH;

unsigned char lval[5],cval[5];

/* This will hold the current game area */
unsigned char area[blocks_x][blocks_y];

/* This is used to save background image on the nextpiece area */
unsigned char nextpiecearea[piecesize_max * blocksize * piecesize_max * blocksize];

/* Current piece falling */
static struct piece currentpiece;

/* Next piece */
static struct piece nextpiece;

/* Whether the current piece is valid, or if we need to allocate a new one */
static bool currentpiecevalid=false;

/* Colors for current and next piece */
static color_type *currentcolor, *nextcolor;

static unsigned int timerTicks = 0;

const int OPERATION_CLEAR=0;
const int OPERATION_DRAW=1;

/* The score area definition */

#define SCORECHARS 6
#define SCOREX 90
#define SCOREY 40

/* The score area. We use this to save the BG image */

unsigned char scorearea[ 9 * ((8* SCORECHARS)+1) ];

typedef void (*loopfunc)(void);

enum {
	START,
	PLAY
} game_state = START;

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



#define DEBUG(x...)


/* Random function (sorta) */
#ifndef __linux__
unsigned xrand() {
	CRC16APP=TIMERTSC;
	return CRC16ACC;
}
#else
#define xrand rand
#endif

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


void board_draw()
{
	int x,y;
	VGA.setColor(BLACK);
	VGA.drawRect(board_x0,board_y0,board_width,board_height);

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

void loop()
{
	enum event_t ev = hasEvent();
	tick++;  
  
	audiofill();
	//loop_functions[game_state]();
	if (ev!=event_none) {
		processEvent(ev);
	}

        setStatus(PS2B_CLK,10, 70, "PS2 CLK");
        setStatus(PS2B_DAT,10, 80, "PS2 DAT");
        
        setStatus(JOYSTICKB_1, 100, 10, "J1");
        setStatus(JOYSTICKB_2, 100, 20, "J2");
        setStatus(JOYSTICKB_3, 100, 30, "J3");
        setStatus(JOYSTICKB_4, 100, 40, "J4");
        setStatus(JOYSTICKB_5, 100, 50, "J5");
        setStatus(JOYSTICKB_6, 100, 60, "J6");
        setStatus(JOYSTICKB_8, 100, 70, "J8");
        //setStatus(JOYSTICKB_9, 100, 80, "J9");  
        
        //Blink LED's

//        if (digitalRead( PS2B_CLK ) == 1){
//          VGA.setColor(GREEN);
//          VGA.printtext(10,40, "PS2 CLK");}
//        else {
//          VGA.setColor(RED);
//          VGA.printtext(10,40, "PS2 CLK");
//        }
//        
//        if (digitalRead( PS2B_DAT ) == 1){
//          VGA.setColor(GREEN);
//          VGA.printtext(10,50, "PS2 DAT");}
//        else {
//          VGA.setColor(RED);
//          VGA.printtext(10,50, "PS2 DAT");
//        }       
             

	delay(20);
        ledState = !ledState;
        digitalWrite(LED1, ledState);
        digitalWrite(LED2, ledState);
        digitalWrite(LED3, ledState);
        digitalWrite(LED4, ledState);        
}

void setup()
{

	pinMode(AUDIOPIN,OUTPUT);
	digitalWrite(AUDIOPIN,HIGH);
	outputPinForFunction(AUDIOPIN, 8);
	pinModePPS(AUDIOPIN, HIGH);

#ifdef AUDIOPINEXTRA
	pinMode(AUDIOPINEXTRA,OUTPUT);
        digitalWrite(AUDIOPINEXTRA,HIGH);
	outputPinForFunction(AUDIOPINEXTRA, 8);
	pinModePPS(AUDIOPINEXTRA, HIGH);
#endif

        // initialize ps2 A and joystick A as HIGH outputs:
        for (int thisPin = 0; thisPin < outCount; thisPin++)  {
          pinMode(testOut[thisPin], OUTPUT); 
          digitalWrite(testOut[thisPin],HIGH);     
        }
        
        // initialize ps2 B, joystick B and pushbutton pins as inputs:
        for (int thisPin = 0; thisPin < inCount; thisPin++)
          pinMode(testIn[thisPin], INPUT); 

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
	VGA.setColor(RED);
        VGA.printtext(10,0, "Up");
        VGA.printtext(10,10, "Left");
        VGA.printtext(10,20, "Right");
        VGA.printtext(10,30, "Down");
        
        VGA.printtext(0, 110, "Arcade MegaWing Test");

	INTRMASK = BIT(INTRLINE_TIMER0); // Enable Timer0 interrupt

	INTRCTL=1;

	// Start timer, 50Hz (prescaler 64)
	TMR0CMP = (CLK_FREQ/(50U*64))-1;
	TMR0CNT = 0x0;
	TMR0CTL = BIT(TCTLENA)|BIT(TCTLCCM)|BIT(TCTLDIR)|BIT(TCTLCP2)|BIT(TCTLCP0)|BIT(TCTLIEN);
}
