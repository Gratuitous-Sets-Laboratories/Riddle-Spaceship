void crankTheCrank() {

  int minTime = 250;
  int maxTime = 1000;

  for (int c = 0; c < 2; c++){
    if (digitalRead(crankPhesPin[c]) && lastCrank != c){
      if (millis() < crankTime + minTime){
        crankSpeed = crankTime - millis() + minTime;
      }
      else if (millis() > crankTime + maxTime){
        crankSpeed = millis() - crankTime - maxTime;
      }
      else {
        crankSpeed = 0;
        chargeLevel++;
      }
      crankTime = millis();
      lastCrank++;
    }
  }
}
