#include <avr/io.h>
#include <avr/interrupt.h>
#include <PID_v1.h>
#include <LiquidCrystal.h>

#define TACHO 3            // tacho signals input pin
#define DETECT 2           // zero cross detect pin
#define GATE 17            // TRIAC gate pin
#define BUTTON 4           // rottary encoder button pin
#define RELAY 5            // relay pin
#define PULSE 2            // number of triac trigger pulse width counts. One count is 16 microseconds
#define TACHOPULSES 8      // number of pulses per revolution 
#define TORIGHT 6
#define door_lock 7        // 7 lock the door
#define door_stat 8        // 8 is high when door is locked

const int rs = A2, en = A3, d4 = A4, d5 = A5, d6 = A6, d7 = A7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

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



void lock_door_switch(){
  digitalWrite(door_lock,HIGH);
}

void unlock_door_switch(){
  lcd.setCursor(0,1);
  lcd.print("Unlocking door  ");
  digitalWrite(door_lock,LOW);
}

bool door_is_locked(){
  return digitalRead(door_stat)==1;
}



void setup() {
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("press start");

  Serial.begin(115200);
  // set up pins
  pinMode(BUTTON, INPUT);             // set the button pin
  pinMode(TORIGHT, OUTPUT);
  pinMode(RELAY, OUTPUT);             // set the relay  pin
  pinMode(DETECT, INPUT);             // set the zero cross detect pin
  pinMode(GATE, OUTPUT);              // set the TRIAC gate control pin
  pinMode(TACHO, INPUT);              // set the tacho pulses detect pin
  digitalWrite(BUTTON, HIGH);         // turn on pullup resistors
  digitalWrite(RELAY, relayState);    // initialize relay output

  Input = 200;                        // asiign initial value for PID
  Setpoint = 200;                     // asiign initial value for PID

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(minoutputlimit, maxoutputlimit);
  myPID.SetSampleTime(sampleRate);    // Sets the sample rate

  // set up Timer1
  OCR1A = 100;                        // initialize the comparator
  TIMSK1 = 0x03;                      // enable comparator A and overflow interrupts
  TCCR1A = 0x00;                      // timer control registers set for
  TCCR1B = 0x00;                      // normal operation, timer disabled

  // set up zero crossing interrupt IRQ0 on pin 2.
  // set up tacho sensor interrupt IRQ1 on pin3
  attachInterrupt(0, zeroCrossingInterrupt, RISING);
  attachInterrupt(1, tacho, FALLING);
}

// Interrupt Service Routines
void zeroCrossingInterrupt() { // zero cross detect
  TCCR1B = 0x04;               // start timer with divide by 256 input
  TCNT1 = 0;                   // reset timer - count from zero
  OCR1A = dimming;             // set the compare register brightness desired.
}

ISR(TIMER1_COMPA_vect) {       // comparator match
  if (startflag == true) {     // flag for start up delay
    digitalWrite(GATE, HIGH);  // set TRIAC gate to high
    TCNT1 = 65536 - PULSE;     // trigger pulse width
  }
}

ISR(TIMER1_OVF_vect) {         // timer1 overflow
  digitalWrite(GATE, LOW);     // turn off TRIAC gate
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

bool is_on = false;
void loop() {

  while(!digitalRead(BUTTON));
  prog1();
}
void prog1(){

  lock_door();

  

  run_motor(15,2600,1);
  run_motor(15,2600,0);
  run_motor(15,1300,1);
  
  unlock_door_switch();
  while(door_is_locked());
  lcd.setCursor(0,1);
  lcd.print("    FINISHED!   ");
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
void lock_door(){
  int test_num = 0;
  lcd.setCursor(0, 1);
  lcd.print("locking the door");
  lock_door_switch();
  while(!door_is_locked()){ 
    if(test_num >50){
      door_error();
      test_num = 0;
    }
    test_num++;
    delay(2000);
  }
  lcd.setCursor(0, 1);
  lcd.print("door locked");
}
void door_error(){
  lcd.setCursor(0, 1);
  lcd.print("locking failed  ");
  while(!digitalRead(BUTTON));
  lcd.setCursor(0, 1);
  lcd.print("locking the door");
  // TODO: show DE ,, implement stuff
}