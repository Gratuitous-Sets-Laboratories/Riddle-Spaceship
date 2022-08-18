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
  const String myNameIs = "RiddleTestBed, Aug 2022";// nametag for Serial monitor setup
  const byte correctKey = 4;                                  // 4 = green

  #define numKeyLEDs 1                                        // single pixel for the spaceKey
  const int keyBright = 255;                                  // relative brightness of the Key's neoPixel (0-255)

  #define numPISOregs 1                                       // total number of PISO shift registers (data in)
  #define numSIPOregs 1                                       // total number of SIPO shift registers (data out)

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

  Adafruit_NeoPixel keyLED = Adafruit_NeoPixel(
    numKeyLEDs, neoPixelPin, NEO_GRB + NEO_KHZ800
    );                                                        // neoPixel object name, # of pixels, signal pin, type

  SoftwareSerial mp3Serial(audioRxPin, audioTxPin);           // RX, TX on Arduino side

//-------------- GLOBAL VARIABLES ----------------------------//
/* Decrlare variables used by various functions.
 */

  byte outputByte = 0;
  byte regByteNew[3];

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

  keyLED.begin();
  keyLED.setBrightness(keyBright);
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

  readRegisters(1,2);
  outputByte++;
  sendLightByte(outputByte);
  delay(250);

  dbts();
  cycleReset();

}
