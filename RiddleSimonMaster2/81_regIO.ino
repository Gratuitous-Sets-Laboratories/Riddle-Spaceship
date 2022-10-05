//============================================================//
//============== SHIFT REGISTER I/O ==========================//
//============================================================//

/* These functions will read and write via PISO & SIOP shift registers.
 * They require the global variable byte arrays of:
 * byte PISOdata[numPISOregs];
 * 
 * Devices use 74HC165 PISO registers and 74HC595 SIPO registers.
 * 
 * This section updated  Aug 2022
 */

//-------------- RISING EDGE PUSLE ---------------------------//

/* This function will send a rising edge signal to the designated output pin.
 * Paramaters required are:
 * pinName (loadPin, latchPin, clockPin)
 * pulseTime (given in microseconds)
 */

void pulsePin(int pinName){
  int pulseTime = 10;
  digitalWrite(pinName, LOW);
  delayMicroseconds(pulseTime);
  digitalWrite(pinName, HIGH);
  delayMicroseconds(pulseTime);
}

//-------------- READ FROM PISO REGISTERS --------------------//

/* This function will querry the current status of all requested PISO shift registers.
 * Paramater required are:
 * fReg = the zero-indexed number of the first register to querry
 * lReg = the zero-indexed number of the last register to querry
 * 
 * Note that this function requires the global variable array called:
 * byte PISOdata[numPISOregs];
 */
 
void readPISO(int fReg, int lReg){

  pulsePin(loadPin);                                          // rend a rising edge signal to load the current data into the registers
  
  for (int dumpReg = 0; dumpReg < fReg; dumpReg++){           // for each register before the first one called for...
    for (int dumpBit = 0; dumpBit < 8; dumpBit++){            // for each bit in that register's byte...
      pulsePin(clockPin);                                     // pulse the clock pin to skip that bit
    }
  }
  for (int readReg = fReg; readReg < lReg+1; readReg++){      // for each requested register...
    for (int pos = 0; pos < 8; pos++){                        // for each bit in that register's byte...
      bool val = digitalRead(dataInPin);                      // read the bit's value from the input pin
      bitWrite(PISOdata[readReg], pos, val);                  // write it to the approriate position in the appropriate byte
      pulsePin(clockPin);                                     // pulse the clock pin to advance the next bit
    }
  }
}

//-------------- WRITE TO SIPO REGISTERS ---------------------//

/* This function will load a byte of data into a chain of SIPO shift registers.
 * The sr_byte parameter is the bye of data being sent.
 * 
 * Note that this function should be called seperately for each register in the chain,
 * beginning with the furthest register from the Arduino.
 * 
 * After each register has been updated, use the command
 * pulsePin(latchPin,10);
 * in order to activate the chain of registers.
 * 
 */
void sendSIPO(byte sr_data){

  for (int pos=0; pos<8; pos++){                              // for each bit in the byte...
    bool bitVal = bitRead(sr_data, pos);                      // note its value from the paramater
    digitalWrite(dataOutPin, bitVal);                         // write it to the data pin
    pulsePin(clockPin);                                       // pulse the clock pin
  }
}

//-------------- WRITE TO BOTH PISO REGISTERS ----------------//
 void sendToBothPISOs(){
  sendSIPO(puzzleMOSI);
  sendSIPO(buttonLights);
  pulsePin(latchPin);
 }
