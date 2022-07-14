/*
 * GRATUITOUS SETS LABORATORIES
 * Dallas, TX, USA
 * 
 * Riddle Escapes
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
  const String myNameIs = "RiddleWarpCore 13 Jul 2022";// nametag for Serial monitor setup

  #define numLEDs 1                                        // single pixel for the spaceKey
  const int bright = 255;                                  // relative brightness of the Key's neoPixel (0-255)
  const int timeToOpen = 2000;


//-------------- PIN DE0FINITIONS  ----------------------------//
/* Most of the I/O pins on the Arduino Nano are hard-wired to various components on the ARDNEX2.
 * Pins not used for their standard fuction have header pins for alternate uses.
 */
  #define neoPixelPin 5
  #define latchPin 4
  #define buttonPin 3
  #define batteryPin 2

  #define buttonLED 6
  #define iris1Relay 9
  #define iris2Relay 8
  #define solenoidRelay 7
  
  #define xOutPin     A6          // 12V signal to higher control system
  #define xInPin      A7          // 12V signal from higher conrtol system

  #define audioTxPin 11           // data pin for DF Player Mini, pin 11
  #define audioRxPin 12           // data pin for DF Player Mini, pin 10

//-------------- HARDWARE INITIALIZATION ---------------------//

  Adafruit_NeoPixel statusLED = Adafruit_NeoPixel(
    numLEDs, neoPixelPin, NEO_GRB + NEO_KHZ800
    );                                                        // neoPixel object name, # of pixels, signal pin, type

  SoftwareSerial mp3Serial(audioRxPin, audioTxPin);           // RX, TX on Arduino side

//-------------- GLOBAL VARIABLES ----------------------------//
/* Decrlare variables used by various functions.
 */

  bool masterGo;
  bool masterOld;

  bool latchIsClosed;
  bool buttonIsPressed;
  bool buttonWasPressed;
  bool batteryIsIn;

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

  pinMode (latchPin, INPUT_PULLUP);
  pinMode (buttonPin, INPUT_PULLUP);
  pinMode (batteryPin, INPUT);

  pinMode (buttonLED, OUTPUT);
  pinMode (iris1Relay, OUTPUT);
  pinMode (iris2Relay, OUTPUT);
  pinMode (solenoidRelay, OUTPUT);
  pinMode (audioRxPin, INPUT);
  pinMode (audioTxPin, OUTPUT);

//-------------- NPX -----------------------------------------//

  statusLED.begin();
  statusLED.setBrightness(bright);
  statusLED.show();

//-------------- MP3 ------------------------------------------//

  mp3Serial.begin(9600);
  sendAudioCommand(0x0D,0);         // start mp3
  sendAudioCommand(0x06,30);        // set volume to max
  sendAudioCommand(0x07,0);         // set EQ to normal

//-------------- A/V FEEDBACK --------------------------------//

  Serial.println("Setup complete.");
  Serial.println();
}

//============================================================//
//============== LOOP ========================================//
//============================================================//

void loop() {

//-------------- Update Inputs -------------------------------//

  tagInputs();

  switch (gameStage){
    case 0:                                                   // awaiting master signal (switches complete)
      // LOCK IRIS
      if (buttonIsPressed){
        statusLED.setPixelColor(0,0,255,0);
        statusLED.show();
      }
      else{
        statusLED.setPixelColor(0,0,0,0);
        statusLED.show();
      }
      if (masterGo){
        gameStage++;
      }
      break;

     case 1:                                                  // awaiting button press
      statusLED.setPixelColor(0,255,0,0);
      statusLED.show();

      if (buttonIsPressed){
        openIris();
        gameStage++;
      }
      break;

    case 2:                                                   // popalock
      if(latchIsClosed){
        digitalWrite(solenoidRelay, HIGH);
        delay (250);
        digitalWrite(solenoidRelay, LOW);
        delay (250);
      }
      else{
        gameStage++;
      }
      break;
      
    case 3:                                                   // await battery return
      if(batteryIsIn && latchIsClosed){
        if(buttonIsPressed){
          closeIris();
        }
      }
  }

//-------------- Routine Maintainance ------------------------//

  dbts();
  cycleReset();

}
