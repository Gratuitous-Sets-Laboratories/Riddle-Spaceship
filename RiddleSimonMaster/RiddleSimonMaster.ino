/*
 * GRATUITOUS SETS LABORATORIES
 * Dallas, TX, USA
 * 
 * Riddle Escapes
 * Close Encounters Simon Says
 * Myrtle Beach, SC, USA
 * 22 June 2022
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
  const String myNameIs = "RiddleSimonMaster 12 Jul 2022";    // nametag for Serial monitor setup

  const int numberOfButtons = 5;
  const int answerLength = 8;
  const int startingPoint = 3;
  const int triggerWindow = 500000;
  byte correctAnswer[answerLength];
  const int playTime = 500;
  const int pauseTime = 250;

  #define numLEDs 3                                           // single pixel for the spaceKey
  const int bright = 255;                                     // relative brightness of the Key's neoPixel (0-255)

  #define numPISOregs 2                                       // total number of PISO shift registers (data in)
  #define numSIPOregs 2                                       // total number of SIPO shift registers (data out)

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

//-------------- GLOBAL VARIABLES ----------------------------//
/* Decrlare variables used by various functions.
 */
//  bool lockReady;
//  byte spaceKey;
//  byte spaceKeyOld;
  
//  bool masterGo;
//  bool masterOld;

  byte regByteNew[numPISOregs];
  byte regByteOld[numPISOregs];

  byte puzzState;
  byte buttonByte;
  byte progressByte;
  byte lightByte;

  bool validAnswer = false;
  byte buttonNew;
  byte buttonOld;
  int entryStep;
  int gameStep;
  unsigned long tick;
  unsigned long progTick;
  
  byte gameStage = 0;

  word mp3Status;

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


//-------------- NPX -----------------------------------------//

  progressLED.begin();
  progressLED.setBrightness(bright);
  progressLED.show();

//  sendLightByte(0);

//-------------- MP3 ------------------------------------------//

  mp3Serial.begin(9600);
  sendAudioCommand(0x0D,0);         // start mp3
  sendAudioCommand(0x06,30);        // set volume to max
  sendAudioCommand(0x07,0);         // set EQ to normal

//-------------- SIMON SAYS SETUP ----------------------------//

  randomSeed(analogRead(A0));
  while (!validAnswer) createAnswer();
  gameStep = startingPoint;
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

  readRegisters(0,1);
  
  puzzState = regByteNew[1];
  buttonByte = regByteNew[0];
  progressByte = 0;
  lightByte = 0;

  bool redConComp   = bitRead(puzzState,0);
  bool bluConComp   = bitRead(puzzState,1);
  bool grnConComp   = bitRead(puzzState,2);
  bool battInPlace  = bitRead(puzzState,3);
  bool wiresPatched = bitRead(puzzState,4);
  bool switchesComp = bitRead(puzzState,5);
  bool cardsInPlace = bitRead(puzzState,6);

  switch(gameStage){
//------------------------------------------------------------//    
    case 0:                                                   // the main game is in progress, the master will pass signals between puzzles

      bitWrite(progressByte,0,1);                             // allow the red console to be in play
      bitWrite(progressByte,1,1);                             // allow the blue console to be in play
      bitWrite(progressByte,2,1);                             // allow the green console to be in play
      
      if (switchesComp) bitWrite(progressByte,3,1);           // if the switch puzzle is complete, allow the warp core to open
      if (wiresPatched) bitWrite(progressByte,6,1);           // if the wire puzzle is complete, allow the memory card puzzle to begin

      sendRegBytes();                                         // update the SIPO rregisters

      // NEOPIXEL STATUS UPDATES

      if (puzzState%16 == 15){                                // if the first four puzzles read as complete...
        gameStage++;                                          // advance the game stage
      }
      break;                                                  // EXIT CASE
//------------------------------------------------------------//    
    case 1:                                                   // begin a Simon Says step by...
      demonstrate(gameStep,playTime);                         // deomstrating the current partial sequense 
      gameStage++;                                            // advance the game stage
//------------------------------------------------------------//          
    case 2:                                                   // play Simon Says
      readButtons();                                          // identify if and what button has been pressed

      if (buttonNew && (buttonNew != buttonOld)){             // if a button has been pressed and it's different from the last one...
        tick = millis();                                      // record the timestamp of the press
        if (buttonNew == correctAnswer[entryStep]){           // if the new button is correct in the current entry sequense...
          beepLight(buttonNew,playTime);                      // animate the button (light & sound)
          entryStep++;                                        // and advance the entry step
        }
        else{                                                 // otherwise (if the button pressed was wrong)...
          fail();                                             // execute failure protocols!
        }
        if (entryStep == gameStep){
          entryStep = 0;
          gameStep++;
          delay(playTime);
          demonstrate(gameStep,playTime);
        }
      }
      if (millis() >= tick + triggerWindow){
        fail();
      }
      if(gameStep >= answerLength){
        gameStep++;
      }
      break;
//------------------------------------------------------------//    
    case 3:
      bool filler = false;
  }
//-------------- Routine Maintainance ------------------------//

  dbts();
  cycleReset();

}
