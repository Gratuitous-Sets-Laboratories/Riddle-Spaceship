/*
 * These functions interface with the DF Mini MP3 Player
 */

//-------------- PLAY TRACK BY NUMBER ------------------------//
/*
 * This is the only function you need to call for basic use of the onboard MP3 player.
 * Regardless of what the plater is currently doing,
 * this function will begin the track called in the trackNum parameter.
 * MP3 files should be on the MicroSD card's root directory,
 * and named as a 4-digit number (0001-9999).
 */
void playTrack(word trackNum){
  sendAudioCommand(0x03,trackNum);
}

//-------------- SEND AUDIO COMMAND --------------------------//

void sendAudioCommand(byte command, word parameter){
/*
 * The following byte values come from DF Robot
 */
  byte startByte     = 0x7E;
  byte versionByte   = 0xFF;
  byte commandLength = 0x06;
  byte acknowledge   = 0x00;
  byte endByte       = 0xEF;

  word checksum = -(
    versionByte +
    commandLength +
    command +
    acknowledge +
    highByte(parameter) +
    lowByte(parameter));
  
  byte commandLine[10] = {
    startByte,
    versionByte,
    commandLength,
    command,
    acknowledge,
    highByte(parameter),
    lowByte(parameter),
    highByte(checksum),
    lowByte(checksum),
    endByte
    };

  for (byte clb=0; clb<10; clb++){
    mp3Serial.write(commandLine[clb]);
  }
  
}



//--

void querryMp3(){
  sendAudioCommand(0x42,0);
  delay(100);
  if(mp3Serial.available()){
    mp3Status = mp3Serial.read();
  }
  Serial.println(mp3Status,HEX);
}
