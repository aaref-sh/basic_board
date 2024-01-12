#include <avr/io.h>
#include <LiquidCrystal.h>
#include <avr/interrupt.h>
#include <PID_v1.h>
#include <avr/iom2560.h>
#include <HeliOS.h>
#include <ShiftDisplay2.h>
 


#define BUTTON 4                // rottary encoder button pin



const int rs = A2, en = A3, d4 = A4, d5 = A5, d6 = A6, d7 = A7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

ShiftDisplay2 display(COMMON_CATHODE, 4);

unsigned int RPM;                     // real rpm variable
unsigned int count;                   // tacho pulses count variable
unsigned int lastcount = 0;           // additional tacho pulses count variable
unsigned long lastcounttime = 0;
unsigned long lastflash;
unsigned long lastpiddelay = 0;
unsigned long previousMillis = 0;
unsigned long lastDebounceTime = 0;

const int sampleRate = 1;             // Variable that determines how fast our PID loop
const int rpmcorrection = 86;         // sito kazkodel reikia, kad realus rpm atitiktu matuojamus
const int protection = 2000;          // protection will switch on when real rpm exceeds desired by value
const int debounceDelay = 50;         // the debounce time; increase if the output flickers
const int minoutputlimit = 80;        // limit of PID output
const int maxoutputlimit = 540;       // limit of PID output
const int mindimminglimit = 80;       // the shortest delay before triac fires
const int maxdimminglimit = 625;      // for 60Hz will be 520
const int risetime = 100;             // RPM rise time delay in microseconds (risetime x RPM)
int desiredRPM = 2200;                // ENTER DESIRED RPM HERE

int dimming = 540;                    // this should be the same as maxoutputlimit
int tempcounter = 100;

bool relayState = 0;                  // the current state of the relay pin

bool loopflag = false;                // flag for soft start
bool startflag = false;               // flag for motor start delay
bool runflag = false;                 // flag for motor running state

double Setpoint, Input, Output;       // define PID variables
double sKp = 0.1, sKi = 0.2, sKd = 0; // PID tuning parameters for starting motor
double rKp = 0.25, rKi = 1, rKd = 0;  // PID tuning parameters for runnig motor

PID myPID(&Input, &Output, &Setpoint, sKp, sKi, sKd, DIRECT); // define PID variables and parameters

// Interrupt Service Routines
void zeroCrossingInterrupt() { // zero cross detect
  TCCR1B = 0x04;               // start timer with divide by 256 input
  TCNT1 = 0;                   // reset timer - count from zero
  OCR1A = dimming;             // set the compare register brightness desired.
}

 