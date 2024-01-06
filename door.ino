

void unlock_door_switch(){
  lcd.setCursor(0,1);
  lcd.print("Unlocking door  ");
  digitalWrite(door_lock,LOW);
}


void lock_door_switch(){
  digitalWrite(door_lock,HIGH);
}

bool door_is_locked(){
  return digitalRead(door_stat)==1;
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