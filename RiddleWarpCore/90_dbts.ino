void dbts(){

  bool somethingNew = false;
  if (masterGo != masterOld) somethingNew = true;
  if (buttonIsPressed != buttonWasPressed) somethingNew = true;
  if (batteryIsIn != batteryWasIn) somethingNew = true;

  if (somethingNew){
    Serial.print("master = ");
    Serial.print(masterGo);
    Serial.print("/tbattery = ");
    Serial.print(batteryIsIn);
    Serial.print("/tbutton = ");
    Serial.print(buttonIsPressed);
  }
}
