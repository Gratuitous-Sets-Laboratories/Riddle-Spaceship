/*
 * pixels 0-23 are the ring
 * pixels 24-31 are the strip
 */

void redFlash(int frameNum){

  for (int pxl = 0; pxl < 24; pxl++){
    if (frameNum < 128){
      chargeLEDs.setPixelColor(pxl,0,frameNum*2,0);
    }
    else{
      chargeLEDs.setPixelColor(pxl,0,256-(frameNum*2),0);
    }
    chargeLEDs.show();
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
 * NOTE, colors are in GRB order
 */
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
