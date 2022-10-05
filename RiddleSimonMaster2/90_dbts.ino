void dbts(){
/*
  Serial.print("MOSI: ");
  Serial.print(puzzleMOSI,BIN);
  Serial.print(" ");
  Serial.print("MISO: ");
  Serial.print(puzzleMISO,BIN);
  Serial.println();
*/

  if (somethingNew){
    printAllPISOdata();
  }
}

void printAllPISOdata(){
  for (int reg = 0; reg < numPISOregs; reg++){
    Serial.print("PSIO Register Data: ");
    Serial.print("reg #");
    Serial.print(reg);
    Serial.print(": ");
    Serial.print(PISOdata[reg],BIN);
    if (reg < numPISOregs -1){
      Serial.print(", ");
    }
    Serial.println();
  }
}
