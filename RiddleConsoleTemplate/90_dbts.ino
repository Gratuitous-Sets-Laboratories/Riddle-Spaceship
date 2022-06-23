void dbts(){

  bool somethingNew = false;
  if (spaceKey != spaceKeyOld){
    somethingNew = true;
  }
  if (masterGo != masterOld){
    somethingNew = true;
  }
  if (batteryInCharger != batteryInChargerOld){
    somethingNew = true;
  }
  
  if (somethingNew){
    Serial.print("gameStage: ");
    Serial.print(gameStage);
    Serial.print("\tspaceKey: ");
    Serial.print(spaceKey);
    Serial.print("\tmasterGo: ");
    Serial.print(masterGo);
    Serial.print("\tbatteryInCh: ");
    Serial.print(batteryInCharger);
    Serial.println();
  }

}
