/*
 * 
 */

void relayCommand(byte relay, bool state){
  int relayPin[2] = {relay1Pin, relay2Pin};
  digitalWrite(relayPin[relay-1], state);
  if (state){
    relayTime[relay-1] = millis();
  }
}

void relaySafety(){
  int maxTime = 5*60*1000;
  int relayPin[2] = {relay1Pin, relay2Pin};
  for (int relay = 0; relay < 2; relay++){
    if (millis() >= relayTime[relay-1] + maxTime){
      digitalWrite(relayPin[relay], LOW);
    }
  }
}
