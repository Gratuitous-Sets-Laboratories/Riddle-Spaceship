/*
 * GRATUITOUS SETS LABORATORIES
 * Dallas, TX, USA
 * 
 * Riddle Escapes
 * Close Encounters Simon Says
 * Myrtle Beach, SC, USA
 * 
 * MABOB I (1.0) architecture
 */

//============== DEFINITIONS & DECLAIRATIONS =================//

//-------------- LIBRARIES -----------------------------------//
/* Call all function libraries required by the sketch. 
 */
  #include <Adafruit_NeoPixel.h>  // WS2812 (NeoPixel) addressable LEDs
  #include <SoftwareSerial.h>
  
//-------------- SETTINGS & GLOBAL CONSTANTS -----------------//
/* Define constraits used by various functions.
 * Variables using '#define' are defined by hardware, and should be left alone.
 * Variables using 'const' can be changed to tune the puzzle.
 */
 
  #define numPISOregs 2                                       // total number of PISO shift registers (data in)
  #define numLEDs 3                                           //

  const String myNameIs = "RiddleSimonMaster2 Aug 2022 (B)";  // nametag for Serial monitor setup
  
//.............. Simon Says Tuning ...........................//

  const int numberOfButtons = 5;
  const int answerLength = 8;
  const int startingPoint = 3;
  const int triggerWindow = 5000;
  const int playTime = 500;
  const int pauseTime = 250;
  
 
//-------------- PIN DE0FINITIONS  ----------------------------//
/* Most of the I/O pins on the Arduino Nano are hard-wired to various components on the ARDNEX2.
 * Pins not used for their standard fuction have header pins for alternate uses.
 */
//.............. Digital Pins ................................//
  #define loadPin     2           // parallel connection to all 74HC165 PISO shift registers, pin 1
  #define dataInPin   3           // serial connection to nearest 74HC165 PISO shift register, pin 9
  #define latchPin    4           // parallel connection to all 74HC595 SIPO shift registers, pin 12
  #define neoPixelPin 5           // data line to WS2812 (NeoPixel) via 470R resistor
  #define dataOutPin  6           // serial connection to nearest 74HC595 SIPO shift register, pin 14
  #define clockPin    7           // parallel connection to all shift registers (74HC165 pin 2 / 74HC595 pin 11)
  #define relay2Pin   8           // trigger pin for relay K2 via 1K resistor & 2N2222 or 2N7000 
  #define relay1Pin   9           // trigger pin for relay K1 via 1K resistor & 2N2222 or 2N7000
  #define buzzerPin  10           // trigger pin for piezoelectric buzzer via 1K resistor & 2N2222 or 2N7000
  #define audioTxPin 11           // data pin for DF Player Mini, pin 11
  #define audioRxPin 12           // data pin for DF Player Mini, pin 10
  #define progLEDPin 13           // trigger pin for PRG LED (if used)
//.............. Analog Pins .................................//
  #define analogInPin A0          // onboard 1K resistor voltage divider
  #define xOutPin     A6          // 12V signal to higher control system
  #define xInPin      A7          // 12V signal from higher conrtol system

//-------------- HARDWARE INITIALIZATION ---------------------//

  Adafruit_NeoPixel progressLED = Adafruit_NeoPixel(
    numLEDs, neoPixelPin, NEO_GRB + NEO_KHZ800
    );                                                        // neoPixel object name, # of pixels, signal pin, type

  SoftwareSerial mp3Serial(audioRxPin, audioTxPin);           // RX, TX on Arduino side

//============== GLOBAL VARIABLES ============================//
/* Decrlare variables used by various functions.
 */

  byte PISOdata[numPISOregs];
  byte PISOprev[numPISOregs];
  bool somethingNew;
//............................................................//
  byte gameStage;                                             // used to tack entry point into switch/case
  byte puzzleMISO;                                            // rename of PISOreg[0]

  byte puzzleMOSI;                                            // SIPO output byte from MCH to downstream puzzles
//............................................................//
  bool validAnswer;
  byte correctAnswer[answerLength];
  byte currentAnswer[answerLength];
  byte simonStep;
  byte entryStep;
//  bool soFarSoGood;
  byte buttonStates;                                          // rename of PISOreg[1]
  byte buttonLights;
  byte buttonNew;
  byte buttonOld;
  unsigned long tick;

//============================================================//
//============== SETUP =======================================//
//============================================================//

void setup() {

//-------------- SERIAL MONITOR ------------------------------//

  Serial.begin(19200);                                        // !! Serial monitor must be set to 19200 baud to read feedback !!
  Serial.println();
  Serial.println("Setup initialized.");
  Serial.println(myNameIs);

//-------------- PINMODES ------------------------------------//

//.............. Shift Registers .............................//
  pinMode (clockPin, OUTPUT);
  pinMode (loadPin, OUTPUT);
  pinMode (dataInPin, INPUT);
  pinMode (latchPin, OUTPUT);
  pinMode (dataOutPin, OUTPUT);
//.............. Onboard Relays ..............................//
  pinMode (relay1Pin, OUTPUT);
  pinMode (relay2Pin, OUTPUT);
//.............. Offboard Communication ......................//
  pinMode (xOutPin, OUTPUT);
  pinMode (xInPin, INPUT);
//.............. Other Pin Modes .............................//
  pinMode (audioRxPin, INPUT);
  pinMode (audioTxPin, OUTPUT);
  pinMode (progLEDPin, OUTPUT);


//-------------- HARDWARE SETUP ------------------------------//

  progressLED.begin();
  progressLED.setBrightness(255);
  progressLED.show();

  mp3Serial.begin(9600);
  sendAudioCommand(0x0D,0);         // start mp3
  sendAudioCommand(0x06,30);        // set volume to max
  sendAudioCommand(0x07,0);         // set EQ to normal

//-------------- SIMON SAYS SETUP ----------------------------//

  randomSeed(analogRead(A0));
  while (!validAnswer) createAnswer();
  simonStep = startingPoint;
  entryStep = 0;

//-------------- A/V FEEDBACK --------------------------------//

  tone(buzzerPin,1000,500);
  digitalWrite(progLEDPin, HIGH);
  delay(500);
  digitalWrite(progLEDPin,LOW);
  Serial.println("Setup complete.");
  Serial.println();
}

//============================================================//
//============== LOOP ========================================//
//============================================================//

void loop() {

//-------------- Update Inputs -------------------------------//

  readPISO(0,1);                                              // read both PISO registers

  for (int reg = 0; reg < numPISOregs; reg++){                // for each register...
    if (PISOdata[reg] != PISOprev[reg]){                      // if its byte has changed since last cycle...
      somethingNew = true;                                    // raise the somethingNew flag
    }
  }
  
  puzzleMISO = PISOdata[1];                                   // globally rename the 2nd byte
  buttonStates = PISOdata[0];                                 // globally rename the 1st byte                                                           
  bool redConComp   = bitRead(puzzleMISO,0);                  // locally rename various bits of the 2nd byte
  bool bluConComp   = bitRead(puzzleMISO,1);
  bool grnConComp   = bitRead(puzzleMISO,2);
  bool battInPlace  = bitRead(puzzleMISO,3);
  bool wiresPatched = bitRead(puzzleMISO,4);
  bool switchesComp = bitRead(puzzleMISO,5);
  bool cardsInPlace = bitRead(puzzleMISO,6);

  /* puzzleMOSI pin, MCN pin, & puzzle corolation
   *  bit ltr puz
   *  0   G  red
   *  1   F  blu
   *  2   E  grn
   *  3   D  bat
   *  4   C  wir
   *  5   B  swt
   *  6   A  crd
   *  7   
   */

//============== GAME FLOW ===================================//

  switch(gameStage){

//-------------- Main Game in Progress -----------------------//
    case 0:
      puzzleMOSI = 7;
      if (switchesComp) bitWrite(puzzleMOSI,3,1);             // if the switch puzzle is complete, allow the warp core to open
      if (wiresPatched) bitWrite(puzzleMOSI,6,1);             // if the wire puzzle is complete, allow the memory card puzzle to begin
      
      sendSIPO(puzzleMOSI);                                   // send the puzzleMOSI data to the master hub
      sendSIPO(255);                                          // send a blank byte for the button LEDs
      pulsePin(latchPin);                                     // latch both SIPO registers
      
      if (puzzleMISO%16 == 15){                               // if all 3 consoles and the warp core are reporting completion...
        simonStep = startingPoint;                            // put the players at the designated step in the Simon game
        entryStep = 0;
        gameStage++;                                          // advance the gameStage
      }
      break;                                                  // END CASE

//-------------- Simon Demonstration--------------------------//
    case 1:
      demonstrate(simonStep, 1000);                           // demonstrate the correct sequense up to the current simonStep
      gameStage++;                                            // advance the gameStage

//-------------- Simon Play ----------------------------------//
    case 2:
      buttonRead();                                           // read the latest button press
      if (buttonNew && (buttonNew != buttonOld)){             // if there is a press and it's different from the last one...
        currentAnswer[entryStep] = buttonNew;                 // record that as the current entryStep
        
        if(soFarSoGood){                                      // if the sequense is looking good...
          entryStep++;                                        // advance the entryStep
        }
        else{                                                 // otherwise...
          fail();                                             // failure protocols
          break;
        }
        if (entryStep > simonStep){                           // if the (newly advanced) entryStep would put the players past the simonStep...
          entryStep = 0;                                      // reset the entryStep
          simonStep++;                                        // increment the simonStep
          gameStage = 1;                                      // set gameStage to play the demonstration
        }
        if (simonStep > answerLength){                        // if the (newly advanced simonStep would put the players past the finish line...
          gameStage = 3;                                      // trigger victory
        }
      }
      break;                                                  // END CASE

//-------------- Victory -------------------------------------//
    case 3:
      // VICTORY CONDITIONS
      break;
  }                                                           // final switch case brace

//-------------- Routine Maintainance ------------------------//

  dbts();
  cycleReset();

}
