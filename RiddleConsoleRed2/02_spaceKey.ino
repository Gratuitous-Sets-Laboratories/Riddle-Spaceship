/*
 * SPACE KEY
 * These functions deal with the value reading and AV animations of the Space Keys.
 * K#   color ohm  anlg hexDec
 * 0    none  ---  ---  --------
 * 1    RED   3k3  237  0x00FC00
 * 2    ORN   2k2  319  0x48CC00
 * 3    YEL   1k5  409  0x909000
 * 4    GRN   1k0  511  0xFC0000
 * 5    BLU   680  609  0x0000FC
 * 6    PRP   470  696  0x009090
 * 7    WTE   220  838  0x909090
 */
//-------------- Read Space Key ------------------------------//
/*
 * Check the analogInPin to update the spaceKey variable.
 */
void readSpaceKey(){

  int target[8] = {0,237,319,409,511,609,696,838};            // calculated analog read values for each key's chosen resistor
  int range = 10;                                             // the margin of error (+/-) to allow a key to read

  int keyValue = analogRead(analogInPin);                     // read the keyPin and set that as a local variable 

  spaceKey = 0;                                               // assume there is no key

  for (int sk=1; sk<8; sk++) {                                // for each of 7 possible keys...
    if (keyValue >= target[sk]-range                          // if the read is at or above the low end...
    && keyValue <= target[sk]+range) {                        // and it is at or below the high end...
      spaceKey = sk;                                          // that is the key number
    }
  }
}

//-------------- Light Space Key -----------------------------//
/*
 * Illuminate the Space Key's neoPixel.
 * keyColors are defined below
 * adjust those values to change the color associated with a given key
 */
void lightKey(byte key){

  uint32_t keyColor[8] = {0x000000, 0x00FC00, 0x48CC00, 0x909000, 0xFC0000, 0x0000FC, 0x009090, 0x909090};

  keyLED.setPixelColor(0,keyColor[key]);                      // set pixel color to one of the values defined above
  keyLED.show();                                              // illumiate the pixel
}

//-------------- AV Space Key Animations ---------------------//
/*
 * Makes the space key pulse with white light and sound as the lock "scans" the key
 */
void scanLoop(int scanDelay) {
  
  for (int x=0; x<255; x++) {                                 // for each ascending step in a byte...
    keyLED.setPixelColor (0,x,x,x);                           // set each color in the key's pixel to that value
    keyLED.show();                                            // illuminate the pixel
    analogWrite(buzzerPin, x);                                // sound the piezo buzzer at a matching volume
    delayMicroseconds(scanDelay);                             // delay a number of us defined by the function's paramater
  }
  for (int y=255; y>=0; y--) {                                // for each descending step in a byte...
    keyLED.setPixelColor (0,y,y,y);                           // set each color in the key's pixel to that value
    keyLED.show();                                            // illuminate the pixel
    analogWrite(buzzerPin, y);                                // sound the piezo buzzer at a matching volume
    delayMicroseconds(scanDelay);                             // delay a number of us defined by the function's paramater
  }
}

void yupTone() {

  const int toneLength = 125;                                 // length of each tone from the buzzer (in ms)

  tone(buzzerPin, 523, toneLength);
  delay(toneLength);
  tone(buzzerPin, 659, toneLength);
  delay(toneLength);
  tone(buzzerPin, 784, toneLength * 2);
  delay(toneLength * 2);
}

void nopeTone() {

  const int toneLength = 125;                                 // length of each tone from the buzzer (in ms)
    
  tone(buzzerPin, 330, toneLength * 2);
  delay(toneLength * 2);
  tone(buzzerPin, 262, toneLength * 4);
  delay(toneLength * 4);
}
