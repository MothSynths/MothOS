#include <Arduino.h>

#include "LedManager.h"
LedManager ledManager = LedManager(9, 10, 11, 12);

#include "InputManager.h"
InputManager inputManager = InputManager();

//tracker
#include "Tracker.h"
Tracker tracker = Tracker();

//Matrix keypad library
#include <Keypad.h>
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  { 'M', 'N', 'O', 'P' },
  { 'I', 'J', 'K', 'L' },
  { 'E', 'F', 'G', 'H' },
  { 'A', 'B', 'C', 'D' }
};

byte rowPins[ROWS] = { 4, 3, 8, 15 };     //connect to the row pinouts of the keypad
byte colPins[COLS] = { 16, 17, 18, 13 };  //connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//sample header 
//1,2,1,3,1,4,1,5

//i2s sound
#include <ESP_I2S.h>
I2SClass i2s;
const int sampleRate = 22050;  // sample rate in Hz


void setup() {
  keypad.setDebounceTime(0);

  //setup I2S pins
  // i2s.setPins(I2S_PIN_BCLK, I2S_PIN_WS, I2S_PIN_DOUT);
  i2s.setPins(6, 7, 5);

  // New i2s driver defaults to PHILIPS MODE when in STD mode, and will automatically
  // fill both slots (ie left + right) with the same sample when using I2S_SLOT_MODE_MONO
  if (!i2s.begin(I2S_MODE_STD, sampleRate, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO)) {
    Serial.println("Failed to initialize I2S!");
    while (1) { } ;
  }
}


void loop() {

  inputManager.UpdateInput(keypad.getKey());
  char note = inputManager.note;
  char trackCommand = inputManager.trackCommand;
  int trackCommandArgument = inputManager.trackCommandArgument;
  char ledCommand = inputManager.ledCommand;

  if (ledCommand != ' ') {
    ledManager.SetCommand(ledCommand);
  }

  if (trackCommand != ' ') {
    tracker.SetCommand(trackCommand, trackCommandArgument);
  }

  int sample = tracker.UpdateTracker();

  // New i2s only wants to write bytes out, so we need to split the sample before writing
  // Copy the high and low bytes of our 16bit sample into a buffer and write that
  byte outbuf[2];
  outbuf[0] = lowByte(tracker.sample);
  outbuf[1] = highByte(tracker.sample);
  i2s.write(outbuf, 2);

  int tempoBlink = tracker.tempoBlink;
  if (tempoBlink > 0)
    ledManager.SetLit(tempoBlink, tracker.selectedTrack);

  ledManager.SetPattern(tracker.allPatternPlay, tracker.currentPattern);
  ledManager.UpdateLed();
  inputManager.EndFrame();
}
