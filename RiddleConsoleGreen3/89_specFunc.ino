/*
 * 
 */

void querryMaster(){
/*
 * This function will use the following global variables:
 * bool masterGo;
 * uint32_t masterGoTick;
 */

  int debounceDelay = 3000;

  if (digitalRead(xInPin)){
    masterGo = true;
    masterGoTick = millis();
  }
  else{
    if (millis() >= masterGoTick + debounceDelay){
      masterGo = false;
    }
  }
}
