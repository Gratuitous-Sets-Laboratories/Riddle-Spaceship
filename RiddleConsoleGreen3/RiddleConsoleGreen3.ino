/*
 * GRATUITOUS SETS LABORATORIES
 * Dallas, TX, USA
 * 
 * Riddle Escapes
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
  const String myNameIs = "RiddleConsoleGreen3";              // name of sketch
  const String verNum = "Beta";                               // version of sketch
  const String lastUpdate = "2022 Sept 03";                   // last update

//............. Harware Installed ............................//
  #define numPISOregs 1                                       // total number of PISO shift registers (data in)
  #define numSIPOregs 1                                       // total number of SIPO shift registers (data out)

//.............. Game Tuning .................................//
  const byte correctKey = 4;                                  // 4 = green

  const int targetCrankCount = 40;
  const int minCrankTime = 100;
  const int frameDelay;
//  const int maxCrankTime = 2500;


//-------------- PIN DE0FINITIONS  ----------------------------//
/* Most of the I/O pins on the Arduino Nano are hard-wired to various components on the ARDNEX2.
 * Pins not used for their standard fuction have header pins for alternate uses.
 */

  #define xOutPin     A6          // 12V signal to higher control system
  #define xInPin      A7          // 12V signal from higher conrtol system

  #define analogInPin A0          // onboard 1K resistor voltage divider
  #define neoPixelPin 5           // data line to WS2812 (NeoPixel) via 470R resistor

  #define cradlePin 2
  const int crankPin[2] = {3,4};
  #define chargeLEDPin 6
  
  #define relay2Pin   8           // trigger pin for relay K2 via 1K resistor & 2N2222 or 2N7000 
  #define relay1Pin   9           // trigger pin for relay K1 via 1K resistor & 2N2222 or 2N7000
  #define buzzerPin  10           // trigger pin for piezoelectric buzzer via 1K resistor & 2N2222 or 2N7000
  #define audioTxPin 11           // data pin for DF Player Mini, pin 11
  #define audioRxPin 12           // data pin for DF Player Mini, pin 10
  #define progLEDPin 13           // trigger pin for PRG LED (if used)


//-------------- HARDWARE INITIALIZATION ---------------------//

//.............. WS2812B NeoPixels ...........................//
  #define keyLEDs 1                                           // number of pixels in the daisy chain (key)
  #define numLEDs 24                                          // number of pixels in the daisy chain (main)
  const int npxBright = 255;                                  // relative brighness of NeoPixels (0-255)
  Adafruit_NeoPixel keyLED = Adafruit_NeoPixel(               // neoPixel object name
    keyLEDs, neoPixelPin, NEO_GRB + NEO_KHZ800);              // # of pixels, signal pin, type
  Adafruit_NeoPixel chargeLEDs = Adafruit_NeoPixel(               // neoPixel object name
    numLEDs, chargeLEDPin, NEO_GRB + NEO_KHZ800);             // # of pixels, signal pin, type

//.............. Serial for DFPlayer Mini ....................//
  const int mp3vol = 30;                                      // confirm min/max of this paramater
  const int mp3eq = 0;
  SoftwareSerial mp3Serial(audioRxPin, audioTxPin);           // RX, TX on Arduino side

//============== GLOBAL VARIABLES ============================//
/* Decrlare variables used by various functions.
 */
//.............. Raw Input Data ..............................//
  byte PISOdata[numPISOregs];
  byte PISOprev[numPISOregs];
  bool somethingNew;
  
  bool masterGo;  
  bool masterOld;
  uint32_t masterGoTick;
 
  bool lockReady;
  byte spaceKey;
  byte spaceKeyOld;
  
  byte gameStage = 0;

  bool battInPlace;
  bool battInPlaceOld;
  bool lastCrankPos;
  bool tooFast;
  int crankCount;
  uint32_t tick;
  byte frame;
  uint32_t frameTick;

//============================================================//
//============== SETUP =======================================//
//============================================================//

void setup() {

//-------------- SERIAL MONITOR ------------------------------//

  Serial.begin(19200);                                        // !! Serial monitor must be set to 19200 baud to read feedback !!
  Serial.println();
  Serial.println("Setup initialized.");
  Serial.print(myNameIs);                                     // report the sketch name and last update
  Serial.print(" ver ");
  Serial.println(verNum);
  Serial.print("Last updated on ");
  Serial.println(lastUpdate);

//-------------- PINMODES ------------------------------------//

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
  pinMode (cradlePin, INPUT);
  for (int p=0; p<2; p++) pinMode(crankPin[p],INPUT);

//-------------- HARDWARE SETUP ------------------------------//

//.............. NeoPixels ...................................//
  keyLED.begin();
  keyLED.setBrightness(npxBright);
  keyLED.show();
  
  chargeLEDs.begin();
  chargeLEDs.setBrightness(npxBright);
  chargeLEDs.show();

//.............. MP3 .........................................//
  mp3Serial.begin(9600);            // begin software serial comm (DFPlayer must use the 9600 baud)
  sendAudioCommand(0x0D,0);         // start mp3
  sendAudioCommand(0x06,mp3vol);    // set volume
  sendAudioCommand(0x07,mp3eq);     // set EQ

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

  querryMaster();                       // check for master signal
  readSpaceKey();                       // determine which key (if any) is in the lock

//============== PUZZLE FLOW =================================//

  if (!masterGo) gameStage = 0;         // if the masterGo signal is absent reset the gameStage to begin the puzzle
  
  switch (gameStage){

//-------------- 0: Await Master Go --------------------------//
    case 0:
                                              // while in stage 0...
      digitalWrite (relay1Pin, LOW);          // disengage onboard relay 1
      digitalWrite (relay2Pin, LOW);          // disengage onboard relay 2
      digitalWrite (xOutPin, LOW);            // bring 12V output LOW

      if (masterGo){
        gameStage++;
      }
      break;
      
//-------------- 1: Space Key --------------------------------//
    case 1:
                                              // while in stage 1...
      digitalWrite (relay1Pin, LOW);          // disengage onboard relay 1
      digitalWrite (relay2Pin, LOW);          // disengage onboard relay 2
      digitalWrite (xOutPin, LOW);            // bring 12V output LOW
      
      if (lockReady && spaceKey){             // if there was no key in the lock & now there is...
        lockReady = false;                    // the lock is no longer flagged as ready
        scanLoop(1200);                       // run the scanLoop animation...
        scanLoop(900);                        // (this delay serves to allow the key to be fully inserted
        scanLoop(600);                        // before giving a final update to the spaceKey value)
        readSpaceKey();                       // reread the spaceKey after the scanLoop
        lightKey(spaceKey);                   // illuminate the neoPixel with the key's color

        if (spaceKey == correctKey){          // if the newly read key is correct...
          yupTone();                          // play the happy sound
          gameStage++;                        // and advance the gameStage
        }
        else{                                 // otherwise...
          nopeTone();                         // play the fail sound
          lightKey(0);                        // and de-illumiate the key
        }   
      }
      break;                                  // EXIT SWITCH CASE
      
//-------------- 2: Main Console Game ------------------------//
    case 2:
                                              // while in stage 2...
      digitalWrite (relay1Pin, HIGH);         // engage onboard relay 1 to fire up the consoles acrylic screens
      digitalWrite (relay2Pin, LOW);          // keep onboard relay 2 disengaged
      digitalWrite (xOutPin, LOW);            // keep 12V output LOW

      //
/*
 * looking for:
 * battery
 * crank phesiis
 * outuptting:
 * sounds (MP3)
 * NPX
 */
      battInPlace = digitalRead(cradlePin);           // reading the battPin determines if the battery is present
      if (!battInPlace){                              // if the battery isn't there...
        redFlash(frame);                              // run an animaiton
      }
      else{                                           // otherwise (if there IS a battery)...
        if (digitalRead(crankPin[lastCrankPos])){     // if the crank is in the oposite position as it was last recorded...
          if (millis() >= tick + minCrankTime){       // and if the player isn't cranking too fast...
            tick = millis();                          // update the crank timestamp
            tooFast = false;
            lastCrankPos++;                           // flop the expected crank position
            crankCount++;                             // credit the player with a crank
          }
          else{
            tooFast = true;
          }
        }
      chargeAnimation(frame);
      }
      if (crankCount >= targetCrankCount){
        playTrack(1);
        gameStage++;
      }

      break;
      
//-------------- 3: Puzzle Solved ----------------------------//
    case 3:
                                              // while in stage 3...
      digitalWrite (relay1Pin, HIGH);         // engage onboard relay 1 to fire up the consoles acrylic screens
      digitalWrite (relay2Pin, HIGH);         // engage onboard relay 1
      digitalWrite (xOutPin, HIGH);           // send 12V output HIGH

      greenFlash(frame);

      break;
  }

//-------------- Routine Maintainance ------------------------//

  dbts();
  cycleReset();

}
