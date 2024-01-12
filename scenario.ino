
void prog1(){

  lock_door();

  openValve1();
  openValve2();

  while (waterUnder50())
  {
    delay(1000);
  }
  
  rinse(60);

  run_motor(15,2600,1);
  run_motor(15,2600,0);
  run_motor(15,1300,1);
  
  unlock_door_switch();
  while(door_is_locked());
  lcd.setCursor(0,1);
  lcd.print("    FINISHED!   ");
}