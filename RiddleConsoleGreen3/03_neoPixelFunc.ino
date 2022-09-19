/*
 * pixels 0-23 are the ring
 * pixels 24-31 are the strip
 *
 * NOTE, colors are in GRB order
 */
 

void redFlash(int frameNum){
/*
 * This animation will make the ring pulse with red light.
 */

  for (int pxl = 0; pxl < 24; pxl++){
    if (frameNum < 128){
      chargeLEDs.setPixelColor(pxl,0,frameNum*2,0);
    }
    else{
      chargeLEDs.setPixelColor(pxl,0,256-(frameNum*2),0);
    }
//    chargeLEDs.show();
  }
}

void cycleAnimation(int frameNum, bool overSpeed){

  uint32_t color = 0xDD9900;
  if (overSpeed) color = 0x00FF00;

  for (int pxl = 0; pxl < 24; pxl++){
    if (frameNum/24 == pxl){
      chargeLEDs.setPixelColor(pxl,color);
    }
    else{
      chargeLEDs.setPixelColor(pxl,0);
    }
    if (pxl > 0){
      chargeLEDs.setPixelColor(pxl-1,color);
    }
    if (pxl == 0){
      chargeLEDs.setPixelColor(23,color);
    }
  }
}

void greenFlash(int frameNum){
  for (int pxl = 0; pxl < 24; pxl++){
    if (frameNum < 128){
      chargeLEDs.setPixelColor(pxl,frameNum*2,0,0);
    }
    else{
      chargeLEDs.setPixelColor(pxl,256-(frameNum*2),0,0);
    }
    chargeLEDs.show();
  }
}

/*
void chargeAnimation(int frameNum){
  float chargePercent = crankCount/targetCrankCount;
  for (int pxl = 24; pxl < 32; pxl++){
    if (pxl <= chargePercent*8){
      chargeLEDs.setPixelColor(pxl,255*chargePercent,255-(255*chargePercent),0);
    }
  }
  if(!tooFast){
    chargeLEDs.setPixelColor(frameNum/16,0,0,255);
  }
  else{
    chargeLEDs.setPixelColor(frameNum/16,255,0,0);
  }
  chargeLEDs.show();
}
*/
void showChargeLevel(int charge){
  
  byte barPxl = 0;
  byte red = (255 - crankCount) / 2;
  byte grn = (crankCount / 2);

  for (int pxl = 24; pxl < 32; pxl++){
    if (barPxl <= crankCount/32){
      chargeLEDs.setPixelColor(pxl,red,grn,0);
    }
    else {
      chargeLEDs.setPixelColor(pxl,0);
    }
  }
//  chargeLED.show();
}
