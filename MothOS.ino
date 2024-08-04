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

//i2s sound
#include <I2S.h>
const int sampleRate = 22000;  // sample rate in Hz
bool comp;

void setup() {
  keypad.setDebounceTime(0);

  //setup I2S pins
  I2S.setDataPin(5);
  I2S.setSckPin(6);  //blc
  I2S.setFsPin(7);   //lrc

  if (!I2S.begin(I2S_PHILIPS_MODE, sampleRate, 16)) {
    Serial.println("Failed to initialize I2S!");
    while (1)
      ;
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

  I2S.write((tracker.sample));
  I2S.write((tracker.sample));

  int tempoBlink = tracker.tempoBlink;
  if (tempoBlink > 0)
    ledManager.SetLit(tempoBlink, tracker.selectedTrack);

  ledManager.SetPattern(tracker.allPatternPlay, tracker.currentPattern);
  ledManager.UpdateLed();
  inputManager.EndFrame();
}
