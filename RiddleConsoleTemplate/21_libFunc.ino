void pulseClockPin(){
  digitalWrite(clockPin, LOW);
  delayMicroseconds(10);
  digitalWrite(clockPin, HIGH);
  delayMicroseconds(10);
}

void pulseLoadPin(){
  digitalWrite(loadPin, LOW);
  delayMicroseconds(10);
  digitalWrite(loadPin, HIGH);
  delayMicroseconds(10);
}

void pulseLatchPin(){
  digitalWrite(latchPin, LOW);
  delayMicroseconds(10);
  digitalWrite(latchPin, HIGH);
  delayMicroseconds(10);
}

void sendLightByte(byte lightByte){
  for (int dump = 0; dump < 8; dump++){
    pulseClockPin();
  }
  for (int pos=0; pos<8; pos++){
    bool bitVal = bitRead(lightByte, pos);
    digitalWrite(dataOutPin, bitVal);
    pulseClockPin();
  }
  pulseLatchPin();
}
