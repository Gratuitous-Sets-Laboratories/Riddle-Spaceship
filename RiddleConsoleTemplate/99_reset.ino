/*
 * Cycle Reset
 */

void cycleReset(){

  if (!spaceKey) lockReady = true;
  spaceKeyOld = spaceKey;
  masterGoOld = masterGo;
  batteryInChargerOld = batteryInCharger;
  relaySafety();
}
