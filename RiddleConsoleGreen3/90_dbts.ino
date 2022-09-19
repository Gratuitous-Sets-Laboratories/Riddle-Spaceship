void dbts(){

//  Serial.println(digitalRead(cradlePin));

  bool somethingNew = false;
  if (spaceKey != spaceKeyOld){
    somethingNew = true;
  }
  if (masterGo != masterOld){
    somethingNew = true;
  }
  if (battInPlace != battInPlaceOld){
    somethingNew = true;
  }
  
  if (somethingNew){
    Serial.print("masterGo: ");
    Serial.print(masterGo);
    Serial.print("\tgameStage: ");
    Serial.print(gameStage);
    Serial.print("\tspaceKey: ");
    Serial.print(spaceKey);
    Serial.print("\tbattInPlace: ");
    Serial.print(battInPlace);
    Serial.print("\tlastCrankPos: ");
    Serial.print(lastCrankPos);
    Serial.print("\tcrankCount");
    Serial.print(crankCount);
    Serial.println();
  }
}
