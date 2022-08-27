void dbts(){

  Serial.print("MOSI");
  Serial.print(puzzleMISO,BIN);
  Serial.print("MISO");
  Serial.print(puzzleMISO,BIN);
  Serial.println();


  bool somethingNew = false;

  for (int reg = 0; reg < numPISOregs; reg++){
    if (PISOdata != PISOprev) somethingNew = true;
  }

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
