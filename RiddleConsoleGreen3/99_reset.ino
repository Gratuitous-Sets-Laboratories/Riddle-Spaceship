/*
 * Cycle Reset
 */

void cycleReset(){

  if (!spaceKey) lockReady = true;
  spaceKeyOld = spaceKey;
  masterOld = masterGo;
  battInPlaceOld = battInPlace;

  if (millis() >= frameTick + frameDelay) frame++;

}
