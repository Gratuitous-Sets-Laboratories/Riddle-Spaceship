/*
 * Cycle Reset
 */

void cycleReset(){

  for (int rb = 0; rb < numPISOregs; rb++){
    regByteOld[rb] = regByteNew[rb];
  }
  buttonOld = buttonNew;

}
