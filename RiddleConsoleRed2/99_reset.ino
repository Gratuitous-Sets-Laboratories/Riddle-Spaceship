/*
 * Cycle Reset
 */

void cycleReset(){

  masterOld = masterGo;
  if (!spaceKey) lockReady = true;
  spaceKeyOld = spaceKey;
  for (int reg = 0; reg < numPISOregs; reg++){
    PISOprev[reg] = PISOdata[reg];
  }
  lightByteOld = lightByteNew;

}
