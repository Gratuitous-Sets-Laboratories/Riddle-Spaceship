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
    Serial.print("gameStage: ");
    Serial.print(gameStage);
    Serial.print("\tspaceKey: ");
    Serial.print(spaceKey);
    Serial.print("\tmasterGo: ");
    Serial.print(masterGo);
    Serial.print("\tbattInPlace: ");
    Serial.print(battInPlace);
    Serial.println();
  }
}
