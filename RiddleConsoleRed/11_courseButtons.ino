/*
 * Check if a new button has been pressed, and what button that was.
 * Update the recorded button sequense.
 * Check the recorded sequense against the correct answer.
 */
//============== Read Course Buttons =========================//

void courseButtons(){
  
  byte newButton = 0;                                         // assume that no button has been pressed
  
  if (regByteNew[0] < regByteOld[0]){                         // if the first PISO register byte is lower than before, a button has been pressed...
    for (int btn = 0; btn < 5; btn++){                        // for each button on the console...
      if (bitRead(regByteNew[0],btn) == 0){                   // if its bit is LOW...
        newButton = btn+1;                                    // the newly pressed button is one greater than the count (non-zero indexed)
      }
    }
    playTrack(1);                                             // play the button press sound effect
  }
//------------------------------------------------------------//
  if (newButton && newButton != sequense[6]){                 // if there is a new button, and it's not the same as the last press...
    for (int seqPos = 0; seqPos < 6; seqPos++){               // for each postion in the sequense (other than the lst one)...
      sequense[seqPos] = sequense[seqPos+1];                  // scoot the last recorded press down one in the order
    }
    sequense[6] = newButton;                                  // record the latest button press into the last position
  }
//------------------------------------------------------------//
  courseIsCorrect = true;                                     // assume that the sequense is correct
  for (int seqPos = 0; seqPos < 7; seqPos++){                 // for each step in the sequense...
    if (sequense[seqPos] != correctCourse[seqPos]){           // if the recorded button doesn't match the correct sequense...
      courseIsCorrect = false;                                // the sequense is not correct
    }
  }
}
