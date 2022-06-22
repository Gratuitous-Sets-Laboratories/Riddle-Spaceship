


//-------------- CHECK MASTER --------------------------------//
/*
 * Check and update the status of the 12V input line
 * (if applicable)
 */
void checkMaster(){
  int readVal = analogRead(xInPin);
  if (readVal > 512) masterGo = true;
  else masterGo = false;

}

//--
