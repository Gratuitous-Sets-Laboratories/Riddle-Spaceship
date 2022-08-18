void dbts(){

  for (int r = 0; r < 3; r++){
    Serial.print(regByteNew[r],BIN);
    Serial.print("\t");
  }
  Serial.println();
}
