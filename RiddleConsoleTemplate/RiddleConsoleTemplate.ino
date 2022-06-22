

/*
 * GRATUITOUS SETS LABORATORIES
 * Dallas, TX, USA
 * 
 * Riddle Escapes
 * Myrtle Beach, SC
 * 15 June 2022
 * 
 */

//============== DEFINITIONS & DECLAIRATIONS =================//

//-------------- LIBRARIES -----------------------------------//
/* Call all functionlibraries required by the sketch. 
 */

  #include <Adafruit_NeoPixel.h>  // WS2812 (NeoPixel) addressable LEDs
  #include <SoftwareSerial.h>
  
//-------------- SETTINGS & GLOBAL CONSTANTS -----------------//
/* Define constraits used by various functions.
 */

  String myNameIs = "RiddleConsoleTemplate 22 Jun 2022";      // nametag for Serial monitor setup
  const byte correctKey = 4;                                  // 4 = green

  const int crankLimit;

  const int bright = 255;           // relative brightness of the Key's neoPixel (0-255)
  const int toneLength = 125;       // length of each tone from the buzzer (in ms)
  const int pulseDelay = 1;         // delay between color/intensity changes in the scan sequense (in ms)

  #define numLEDs 1
  #define numChargerLEDs 32
  #define numRegs 2

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

  #define chargerPixelPin 6
  const int chargerPhesPin = 2;
  const int crankPhesPin[2] = {3,4};

//-------------- HARDWARE INITIALIZATION ---------------------//

  Adafruit_NeoPixel keyLED = Adafruit_NeoPixel(
    numLEDs, neoPixelPin, NEO_GRB + NEO_KHZ800
    );                                                        // neoPixel object name, # of pixels, signal pin, type

  SoftwareSerial mp3Serial(audioRxPin, audioTxPin);           // RX, TX on Arduino side

//-------------- GLOBAL VARIABLES ----------------------------//
/* Decrlare variables used by various functions.
 */

  bool lockReady;
  byte spaceKey;
  byte spaceKeyOld;
  
  bool masterGo;
  bool masterGoOld;
  
  bool batteryInCharger;
  bool batteryInChargerOld;
  bool lastCrank;
  uint32_t crankTime;
  int crankSpeed;
  int chargeLevel;
  
  byte gameStage = 0;

  word mp3Status;

  uint32_t relayTime[2];

//============================================================//
//============== SETUP =======================================//
//============================================================//

void setup() {

//-------------- SERIAL MONITOR ------------------------------//

  
  Serial.begin(19200);
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
  pinMode (chargerPhesPin, INPUT);
  for (int p = 0; p < 2; p++){
    pinMode (crankPhesPin[p], INPUT);
  }

//-------------- NPX -----------------------------------------//

  keyLED.begin();
  keyLED.show();

  sendLightByte(0);

//-------------- MP3 ------------------------------------------//

  mp3Serial.begin(9600);
  sendAudioCommand(0x0D,0);         // start mp3
  sendAudioCommand(0x06,30);        // set volume to max
  sendAudioCommand(0x07,0);         // set EQ to normal

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

  checkMaster();                        // check for master signal
  readSpaceKey();                       // determine which key (if any) is in the lock


 

//-------------- Puzzle Flow ---------------------------------//
/*
 * Game flow 
 */

  if (!masterGo) gameStage = 0;         // if the masterGo signal is absent reset the gameStage to begin the puzzle
  
  switch (gameStage){

//-------------- 0: Master Go --------------------------------//
    case 0:
                                              // while in stage 0...
      digitalWrite (relay1Pin, LOW);          // disengage onboard relay 1
      digitalWrite (relay2Pin, LOW);          // disengage onboard relay 2
      digitalWrite (xOutPin, LOW);            // bring 12V output LOW
      sendLightByte(0);                       // send a null byte to SIPO register (when applicable)

      if (masterGo){
        gameStage++;
      }
      break;
      
//-------------- 1: Space Key --------------------------------//
    case 1:
                                              // while in stage 0...
      digitalWrite (relay1Pin, LOW);          // disengage onboard relay 1
      digitalWrite (relay2Pin, LOW);          // disengage onboard relay 2
      digitalWrite (xOutPin, LOW);            // bring 12V output LOW
      sendLightByte(0);                       // send a null byte to SIPO register (when applicable)
      
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
                                              // while in stage 1...
      digitalWrite (relay1Pin, HIGH);         // engage onboard relay 1 to fire up the consoles acrylic screens
      digitalWrite (relay2Pin, LOW);          // keep onboard relay 2 disengaged
      digitalWrite (xOutPin, LOW);            // keep 12V output LOW

      if (digitalRead(chargerPhesPin)){
        batteryInCharger = true;
        gameStage++;
      }
      else{
        batteryInCharger = false;
      }
      break;

    case 2:
      //stuff
      //animation
      break;
  }

  dbts();
  cycleReset();

}
