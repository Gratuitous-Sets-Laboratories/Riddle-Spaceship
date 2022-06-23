/*
 * 
 */
void switchPuzzle(){

  bool switchFlip[8] = {0,0,0,0,0,0,0,0};

  for (int sw = 0; sw < 8; sw++){
    bool switchPosNew = bitRead(regByteNew[1],sw);
    bool switchPosOld = bitRead(regByteOld[1],sw);
    if (switchPosNew != switchPosOld){
      switchFlip[sw] = true;
      // maybe play a sound effect?
    }
  }
  for (int x=0; x<8; x++) {
    if (switchFlip[x]) {
      if (bitRead(lightByteOld,x-1)) bitWrite(lightByteNew,x-1,0);
      else bitWrite(lightByteNew,x-1,1);
      if (bitRead(lightByteOld,x)) bitWrite(lightByteNew,x,0);
      else bitWrite(lightByteNew,x,1);
      if (bitRead(lightByteOld,x+1)) bitWrite(lightByteNew,x+1,0);
      else bitWrite(lightByteNew,x+1,1);
    }
  }
}
