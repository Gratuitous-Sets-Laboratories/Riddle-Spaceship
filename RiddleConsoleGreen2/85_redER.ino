/*
 * 
 */

void querryMaster(){
/*
 * This function will use the following global variables:
 * bool masterGo;
 * uint32_t masterTick;
 */

  int debounceDelay = 1000;

  if (digitalRead(xInPin)){
    masterGo = true;
    masterGoTick = millis();
  }
  else{
    if (millis() >= masterTick){
      masterGo = false;
    }
  }
}
