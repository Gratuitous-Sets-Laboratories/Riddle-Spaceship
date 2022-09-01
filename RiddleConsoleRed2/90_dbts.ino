void dbts(){

  bool somethingNew = false;
  if (spaceKey != spaceKeyOld){
    somethingNew = true;
  }
  if (masterGo != masterOld){
    somethingNew = true;
  }
  
  if (somethingNew){
    Serial.print("gameStage: ");
    Serial.print(gameStage);
    Serial.print("\tspaceKey: ");
    Serial.print(spaceKey);
    Serial.print("\tmasterAnalog: ");
    Serial.print(analogRead(xInPin));
    Serial.print("\tmasterGo: ");
    Serial.print(masterGo);
    Serial.println();
  }

}
