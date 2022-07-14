/*
 * 
 */
//-------------- READ PSIO REGISTER INPUTS -------------------//
/*
 * Read the designated PISO shift registers.
 * Registers are zero-indexed, so begin with 0 for the register nearest to the Arduino.
 */
void readRegisters(int fReg, int lReg){

  pulsePin(latchPin,10);                                      // rend a rising edge signal to load the current data into the registers
  
  for (int dumpReg = 0; dumpReg < fReg; dumpReg++){           // for each register before the first one called for...
    for (int dumpBit = 0; dumpBit < 8; dumpBit++){            // for each bit in that register's byte...
      pulsePin(clockPin,10);                                  // pulse the clock pin to skip that bit
    }
  }
  for (int readReg = fReg; readReg < lReg+1; readReg++){      // for each requested register...
    for (int pos = 0; pos < 8; pos++){                        // for each bit in that register's byte...
      bool val = digitalRead(dataInPin);                      // read the bit's value from the input pin
      bitWrite(regByteNew[readReg], pos, val);                // write it to the approriate position in the appropriate byte
      pulsePin(clockPin,10);                                  // pulse the clock pin to advance the next bit
    }
  }
}

//==
/*
 * Rising edge
 */
void pulsePin(int pinName, int pulseTime){
  digitalWrite(pinName, LOW);
  delayMicroseconds(pulseTime);
  digitalWrite(pinName, HIGH);
  delayMicroseconds(pulseTime);
}

//==

void sendLightByte(byte lightByte){
  for (int dump = 0; dump < 8; dump++){
    pulsePin(clockPin,10);
  }
  for (int pos=0; pos<8; pos++){
    bool bitVal = bitRead(lightByte, pos);
    digitalWrite(dataOutPin, bitVal);
    pulsePin(clockPin,10);
  }
  pulsePin(latchPin,10);
}
