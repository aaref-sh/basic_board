#include <avr/io.h>
#include <avr/interrupt.h>
#include <PID_v1.h>
//#include <avr/iom2560.h>
#include <avr/iom328.h>
#include <HeliOS.h>

#define BUTTON 4                // rottary encoder button pin

#define sampleRate 1             // Variable that determines how fast our PID loop
#define rpmcorrection 86         // sito kazkodel reikia, kad realus rpm atitiktu matuojamus
#define protection 2000          // protection will switch on when real rpm exceeds desired by value
#define debounceDelay 50         // the debounce time; increase if the output flickers
#define minoutputlimit 80        // limit of PID output
#define maxoutputlimit 540       // limit of PID output
#define mindimminglimit 80       // the shortest delay before triac fires
#define maxdimminglimit 625      // for 60Hz will be 520
#define risetime 100             // RPM rise time delay in microseconds (risetime x RPM)

#define sKp 0.1
#define sKi 0.2
#define sKd 0 // PID tuning parameters for starting motor
#define rKp 0.25
#define rKi 1
#define rKd 0  // PID tuning parameters for runnig motor


unsigned int RPM;                     // real rpm variable
unsigned int count;                   // tacho pulses count variable
unsigned int lastcount = 0;           // additional tacho pulses count variable
unsigned long lastcounttime = 0;
unsigned long lastflash;
unsigned long lastpiddelay = 0;
unsigned long previousMillis = 0;
unsigned long lastDebounceTime = 0;

int desiredRPM = 2200;                // ENTER DESIRED RPM HERE

int dimming = 540;                    // this should be the same as maxoutputlimit
int tempcounter = 100;

bool relayState = 0;                  // the current state of the relay pin

bool loopflag = false;                // flag for soft start
bool startflag = false;               // flag for motor start delay
bool runflag = false;                 // flag for motor running state

double Setpoint, Input, Output;       // define PID variables

PID myPID(&Input, &Output, &Setpoint, sKp, sKi, sKd, DIRECT); // define PID variables and parameters

// Interrupt Service Routines
void zeroCrossingInterrupt() { // zero cross detect
  TCCR1B = 0x04;               // start timer with divide by 256 input
  TCNT1 = 0;                   // reset timer - count from zero
  OCR1A = dimming;             // set the compare register brightness desired.
}

 