

ISR(TIMER1_COMPA_vect) {        // comparator match
  if (startflag ) {             // flag for start up delay
    digitalWrite(Triac, HIGH);  // set TRIAC gate to high
    TCNT1 = 65536 - PULSE;      // trigger pulse width
  }
}

ISR(TIMER1_OVF_vect) {          // timer1 overflow
  digitalWrite(Triac, LOW);     // turn off TRIAC gate
  TCCR1B = 0x00;                // disable timer stops unintended triggers
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
        setOutValue(TORIGHT,HIGH);
      delay(50);
    }
    turn_on();
    soft_start();
    speed_regulator(TimeInSecs);
    fault_protection();
  

    //reset rpm after motor stops
    if (!count && !relayState) RPM = 0;
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
    setOutValue(RELAY, relayState); // set the Relay:
}
void turn_on(){
    relayState = !relayState;
    loopflag = true;
    setOutValue(RELAY, relayState); // set the Relay:
    delay (300);                     // delay to prevent sparks on relay contacts
    startflag = true;                // flag to start motor
}
void speed_regulator(int TimeInSecs){
  if (relayState && !loopflag) {
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
  while (loopflag) {
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
    if (!count && relayState && runflag) {
      startflag = false;            // flag to turn off triac before relay turns off
      delay (300);                  // delay to prevent sparks on relay contacts
      setOutValue(RELAY, LOW);
      relayState = 0;
      stuckerror();
    }
    lastcount = count;
    count = 0;
    lastcounttime = millis();
  }
}

void stuckerror() {
  display.set("E5");
  while (1) {
  }
}
