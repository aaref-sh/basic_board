
#include <avr/interrupt.h>
#include <PID_v1.h>
#include <avr/iom2560.h>

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
int desiredRPM = 2200;          // ENTER DESIRED RPM HERE

int dimming = 540;                    // this should be the same as maxoutputlimit
int tempcounter = 100;

byte relayState = LOW;                // the current state of the relay pin
byte buttonState;                     // the current reading from the input pin
byte lastButtonState = HIGH;          // the previous reading from the input pin

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

ISR(TIMER1_COMPA_vect) {       // comparator match
  if (startflag == true) {     // flag for start up delay
    digitalWrite(Triac, HIGH);  // set TRIAC gate to high
    TCNT1 = 65536 - PULSE;     // trigger pulse width
  }
}

ISR(TIMER1_OVF_vect) {         // timer1 overflow
  digitalWrite(Triac, LOW);     // turn off TRIAC gate
  TCCR1B = 0x00;               // disable timer stops unintended triggers
}

// RPM counting routine
void tacho() {
  count++;
  unsigned long time = micros() - lastflash;
  float time_in_sec  = ((float)time + rpmcorrection) / 1000000;
  float prerpm = 60 / time_in_sec;
  RPM = prerpm / TACHOPULSES;
  lastflash = micros();
}
void run_motor(int TimeInSecs,int desiredrpm,bool toright){
    desiredRPM = desiredrpm;
    if(toright){
      digitalWrite(TORIGHT,HIGH);
      delay(50);
    }
    turn_on();
    soft_start();
    speed_regulator(TimeInSecs);
    fault_protection();
  

    //reset rpm after motor stops
    if (count == 0 && relayState == LOW) RPM = 0;
    turn_off();
    digitalWrite(TORIGHT,LOW);
    delay(5000);
}
void turn_off(){
    relayState = !relayState;
    Setpoint = 200;
    Input = 200;
    runflag = false;
    startflag = false;
    delay (300);                     // delay to prevent sparks on relay contacts
    digitalWrite(RELAY, relayState); // set the Relay:
}
void turn_on(){
    relayState = !relayState;
    loopflag = true;
    digitalWrite(RELAY, relayState); // set the Relay:
    delay (300);                     // delay to prevent sparks on relay contacts
    startflag = true;                // flag to start motor
}
void speed_regulator(int TimeInSecs){
  if (relayState == HIGH && loopflag == false) {
    unsigned long start_time = millis();
    while(millis()-start_time < TimeInSecs * 1000){
      unsigned long piddelay = millis();

      if ((piddelay - lastpiddelay) > 1000) {     // delay to switch PID values. Prevents hard start
        myPID.SetTunings(rKp, rKi, rKd);          // Set the PID gain constants and start
        lastpiddelay = millis();
      }

      Input = RPM;
      Setpoint = desiredRPM;

      lcd.setCursor(0,0);
      lcd.print("desired RPM:");
      lcd.setCursor(12,0);
      lcd.print(desiredRPM);
      lcd.setCursor(0,1);
      lcd.print("Real RPM:   ");
      lcd.setCursor(12, 1);
      lcd.print(RPM);
      
      Serial.print(desiredRPM);
      Serial.print("   ");
      Serial.println(RPM);
      myPID.Compute();
      dimming = map(Output, minoutputlimit, maxoutputlimit, maxoutputlimit, minoutputlimit); // reverse the output
      dimming = constrain(dimming, mindimminglimit, maxdimminglimit);     // check that dimming is in 20-625 range
    }
  }
}
void soft_start(){

  //soft start
  while (loopflag == true) {
    myPID.SetTunings(sKp, sKi, sKd);        // Set the PID gain constants and start
    int i = (desiredRPM - tempcounter);
    for (int j = 1; j <= i; j++) {
      Input = RPM;
      Setpoint = tempcounter;
      myPID.Compute();
      dimming = map(Output, minoutputlimit, maxoutputlimit, maxoutputlimit, minoutputlimit); // inverse the output
      dimming = constrain(dimming, mindimminglimit, maxdimminglimit);     // check that dimming is in 20-625 range
      tempcounter++;
      delayMicroseconds (risetime);
    }
    if (tempcounter >= desiredRPM) {
      lastcounttime = millis();
      lastpiddelay = millis();
      loopflag = false;
      runflag = true;
      tempcounter = 100;
    }
  }

}
void fault_protection(){
    // diagnose a fault and turn on protection
  unsigned long counttime = millis();
  if (counttime - lastcounttime >= 1000) {
    if (count == 0 && relayState == HIGH && runflag == true) {
      startflag = false;            // flag to turn off triac before relay turns off
      delay (300);                  // delay to prevent sparks on relay contacts
      digitalWrite(RELAY, LOW);
      relayState = LOW;
      stuckerror();
    }
    lastcount = count;
    count = 0;
    lastcounttime = millis();
  }
}

void stuckerror() {
  lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("ERROR!");
    lcd.setCursor(2, 1);
    lcd.print("MOTOR STUCK!");
  while (1) {
  }
}