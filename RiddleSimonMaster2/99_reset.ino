/*
 * Cycle Reset
 */

void cycleReset(){

  for (int reg = 0; reg < numPISOregs; reg++){
    PISOprev[reg] = PISOdata[reg];
  }
  if (buttonNew){
    buttonOld = buttonNew;
  }

  somethingNew = false;

}
