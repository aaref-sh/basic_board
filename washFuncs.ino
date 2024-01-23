
void rinse(int seconds){
    bool toright = 1;
    auto start = millis();
    while (millis() - start < seconds*1000)
    {
        run_motor(15,2000,toright);
        delay(5);
        toright ^= 1;
    }
}


void runPart(){

}

void preRun(){
    
  lock_door();

  requlateWaterLevel();

}