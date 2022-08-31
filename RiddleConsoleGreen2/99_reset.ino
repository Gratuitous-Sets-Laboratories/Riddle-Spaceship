/*
 * Cycle Reset
 */

void cycleReset(){

  if (!spaceKey) lockReady = true;
  spaceKeyOld = spaceKey;
  masterOld = masterGo;

  if (millis() >= frameTick + frameDelay) frame++;

}
