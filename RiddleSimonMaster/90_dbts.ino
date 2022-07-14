void dbts(){

  bool somethingNew = false;
  for (int reg = 0; reg < numPISOregs; reg++){
    if (regByteNew[reg] != regByteOld[reg]){
      somethingNew = true;
    }
  }
  if (somethingNew){
    Serial.print("puzzle: ");
    Serial.print(regByteNew[1],BIN);
    Serial.print("/tsimon: ");
    Serial.print(regByteNew[0],BIN);
    Serial.println();
  }
}
