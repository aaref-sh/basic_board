
int temp_sensor = 2;
int motor = 3;          // high to fire the trick
int to_right = 4;       // change relays status to make motor running revirce
int freq = 5;           // get the frequency (motor speed)
int runing = 6;         // high while motor is rotating
int door_stat = 7;
int door_lock = 8;
int water_1 = 9;   // water in level 1
int water_2 = 10;  // water reached level 2
int pumb = 11;
int valve = 12;
int heater = 13;
int btn_start = A0;     // debug stuff
int dimming, Htime, Ltime, lf, dims=256, calls=0;
float Ttime, frqu, ms;
bool incs = false , dics = false, is_on = false;
unsigned long call_time;


#pragma region hardware_part
void lock_door(){
  digitalWrite(door_lock,HIGH);
}

void unlock_door(){
  digitalWrite(door_lock,LOW);
}

void open_valve(){
  digitalWrite(valve,HIGH);
}

void close_valve(){
  digitalWrite(valve,LOW);
}

bool door_is_locked(){
  return digitalRead(door_stat)==1;
}

void start_pumb(){
  digitalWrite(pumb,HIGH);
}

bool water_in_level_1(){
  return digitalRead(water_1)==1;
}

bool water_in_level_2(){
  return digitalRead(water_2)==1;
}

void start_heating(){
  digitalWrite(heater,HIGH);
}

void stop_heating(){
  digitalWrite(heater,LOW);
}

int tempreture(){
  // TODO: 
}

void stop_pumb(){
  digitalWrite(pumb,LOW);
}
#pragma endregion


void setup(){
  //ms=1100;
  Serial.begin(4800);
  pinMode(door_stat, INPUT);
  pinMode(btn_start, INPUT);
  pinMode(freq, INPUT);
  pinMode(runing, INPUT);
  pinMode(water_1, INPUT);
  pinMode(water_2, INPUT);
  pinMode(pumb, OUTPUT);
  pinMode(valve, OUTPUT);
  pinMode(door_lock, OUTPUT);
  pinMode(to_right, OUTPUT);
  pinMode(motor, OUTPUT);                 // Set AC Load pin as output
  attachInterrupt(0, zero_cross, RISING);   // Choose the zero cross interrupt # from the table above
  //noInterrupts();
}

void zero_cross(){  
  // Firing angle calculation : 1 full 50Hz wave =1/50=20ms
  // Every zerocrossing thus: (50Hz)-> 10ms (1/2 Cycle) For 60Hz => 8.33ms (10.000/120)
  // 10ms=10000us
  // (10000us - 10us) / 128 = 75 (Approx) For 60Hz =>65
  if (dims < 256 && is_on) {
    int dimtime = (39 * dims);  // For 60Hz =>65
    delayMicroseconds(dimtime);    // Wait till firing the TRIAC
    digitalWrite(motor, HIGH);   // Fire the TRIAC
    delayMicroseconds(10);         // triac On propogation delay
    // (for 60Hz use 8.33) Some Triacs need a longer period
    digitalWrite(motor, LOW);    // No longer trigger the TRIAC (the next zero crossing will swith it off) TRIAC
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

void motor_config(bool right, int Time, int speed){
  digitalWrite(to_right , right?HIGH:LOW);
  delay(10);
  ms = speed;
  call_time = micros();
  is_on = 1;
  //interrupts();
  while(micros()<Time* 1000000+call_time){
    speed_regulator();
  }
  is_on = 0;
  //noInterrupts();
  while(digitalRead(runing)!=0){}
  digitalWrite(to_right ,LOW);
  delay(5000);
}

void run_prog_1 (){
  H1: 
  while(digitalRead(btn_start)==0){} // wait start button

  while(!door_is_locked()){ 
    lock_door();
  }
  
  open_valve();
  while (!water_in_level_1()){}

  start_heating();
  motor_config(0,10,900);

  if(water_in_level_2())
    close_valve();

  stop_heating();
  motor_config(1,10,900);
  motor_config(1,30,3300);
  motor_config(0,30,3300);
  motor_config(1,10,900);
  unlock_door();
  while (door_is_locked()){}
  // some tone to alert at end
}

void loop(){
  run_prog_1 ();
  while(1);
}