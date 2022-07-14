/*
 * 
 */
void tagInputs(){
  if (digitalRead (xInPin)) masterGo = true;
  else masterGo = false;

  if (digitalRead (latchPin == 0)) latchIsClosed = true;
  else latchIsClosed = false;
  
  if (digitalRead (buttonPin == 0)) buttonIsPressed = true;
  else buttonIsPressed = false;
  
  if (digitalRead (batteryPin)) batteryIsIn = true;
  else batteryIsIn = false;
  
}
