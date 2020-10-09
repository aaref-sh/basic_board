int freq = 5;
int AC_LOAD = 3;
int to_right = 4;
int btn1 = A0;
int runing = 6;
int door_stat = 7;
int door_lock = 8;
int dimming, Htime, Ltime, lf, dims=256, calls=0;
float Ttime, frqu, ms;
bool incs = false , dics = false, is_on = false;
unsigned long call_time;

void setup(){
  //ms=1100;
  Serial.begin(4800);
  pinMode(door_stat, INPUT);
  pinMode(btn1, INPUT);
  pinMode(freq, INPUT);
  pinMode(runing, INPUT);
  pinMode(door_lock, OUTPUT);
  pinMode(to_right, OUTPUT);
  pinMode(AC_LOAD, OUTPUT);// Set AC Load pin as output
  attachInterrupt(0, zero_crosss_int, RISING);  // Choose the zero cross interrupt # from the table above
  //noInterrupts();
}

void zero_crosss_int(){  // function to be fired at the zero crossing to dim the light
  // Firing angle calculation : 1 full 50Hz wave =1/50=20ms
  // Every zerocrossing thus: (50Hz)-> 10ms (1/2 Cycle) For 60Hz => 8.33ms (10.000/120)
  // 10ms=10000us
  // (10000us - 10us) / 128 = 75 (Approx) For 60Hz =>65
  if (dims < 256 && is_on) {
    int dimtime = (39 * dims);  // For 60Hz =>65
    delayMicroseconds(dimtime);    // Wait till firing the TRIAC
    digitalWrite(AC_LOAD, HIGH);   // Fire the TRIAC
    delayMicroseconds(10);         // triac On propogation delay
    // (for 60Hz use 8.33) Some Triacs need a longer period
    digitalWrite(AC_LOAD, LOW);    // No longer trigger the TRIAC (the next zero crossing will swith it off) TRIAC
  }
}

void motor_stuck(){
  is_on =0 ;
  //noInterrupts();
  digitalWrite(to_right ,HIGH);
  delay(300);
  digitalWrite(to_right ,LOW);
  while(1){}
}

void get_hz1(){
  int stat = 0, changes = 0;
  //noInterrupts();
  unsigned long c_time = micros();
  while(micros() - c_time < 250000){
    int val = digitalRead(freq);
    if(val!=stat){
      stat = val;
      changes ++;
    }
  }
  if(changes < 100)dims = 190;
  //interrupts();
  frqu = changes*4;
}

void get_hz(){
  if(digitalRead(runing)==0){
    calls++;
    if (calls>2)motor_stuck();
  }
  Htime=pulseIn(freq, HIGH);      //read high time
  Ltime=pulseIn(freq, LOW);       //read low time
  Ttime = Htime+Ltime;
  if(Ttime ==0){
    frqu = 0;
    if(dims<250)dims-=10;else dims = 185;
    return;
  }
  calls = 0;
      //getting frequency with Ttime is in Micro seconds
  frqu=1000000/Ttime;
}
void speed_regulator(){
  //dimming = (analogRead(btn1)+1)/4;
  //ms = (256 - dimming) * 13.1;
  get_hz();
  if(frqu>3400){ frqu = lf;}
  lf = frqu;
  if(abs(frqu-ms)>75){
    if(frqu < ms){ 
      //if(dims>210)dims=210;
     // if(dims>230){dims = 220;}
      if(dims>10){
        dims --;
        incs = 1;
      }
    }
    else if(frqu > ms){
      if(dims<256){
        dims ++;
        delay(100);
        dics = 1;
      }
    }
    delay(40);
  }
  else {
    if(incs){dims +=2;incs = 0;}
    if(dics){dims--;dics = 0;}
  }
  Serial.print(frqu);
  Serial.print("   ");
  Serial.println(ms);
}

void motor_config(bool right, int Time){
  digitalWrite(to_right , right?HIGH:LOW);
  delay(20);
  ms = 1400;
  call_time = micros();
  is_on = 1;
  //interrupts();
  while(micros()<Time* 1000000+call_time)
    speed_regulator();
  is_on = 0;
  //noInterrupts();
  while(digitalRead(runing)!=0){}
  digitalWrite(to_right ,LOW);
  delay(5000);
}
void run_prog_1 (){
  motor_config(0,10);
  motor_config(1,10);
  //motor_config(0,10);
}
void loop(){
  //run_prog_1();
  while(digitalRead(door_stat)==0){
    digitalWrite(door_lock,HIGH);
  }
  run_prog_1 ();
  digitalWrite(door_lock,LOW);
  while(1);
}