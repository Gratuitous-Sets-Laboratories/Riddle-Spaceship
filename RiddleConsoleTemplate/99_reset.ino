/*
 * Cycle Reset
 */

void cycleReset(){

  for (int reg = 0; reg < numPISOregs; reg++){
    PISOprev[reg] = PISOdata[reg];
  }

  if (!spaceKey) lockReady = true;
  spaceKeyOld = spaceKey;
  masterOld = masterGo;

}
