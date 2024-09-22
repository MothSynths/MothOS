#include <Arduino.h>

//MothSynth led manager init
#include "LedManager.h"
LedManager ledManager = LedManager(9, 10, 11, 12);

//MothSynth input manager init
#include "InputManager.h"
InputManager inputManager = InputManager();

//tracker init
#include "Tracker.h"
Tracker tracker = Tracker();

//keypad setup
#include <Keypad.h>
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  { 'M', 'N', 'O', 'P' },
  { 'I', 'J', 'K', 'L' },
  { 'E', 'F', 'G', 'H' },
  { 'A', 'B', 'C', 'D' }
};

byte rowPins[ROWS] = { 4, 3, 8, 15 };
byte colPins[COLS] = { 16, 17, 18, 13 };
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//i2s sound
#include <ESP_I2S.h>
I2SClass i2s;
const int sampleRate = 44100;  // sample rate in Hz

//OLED init
#include "ScreenManager.h"
#include <U8g2lib.h>
#include <Wire.h>


U8G2_SSD1306_128X64_NONAME_1_HW_I2C screen = U8G2_SSD1306_128X64_NONAME_1_HW_I2C(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/47, /* data=*/48);
ScreenManager screenManager = ScreenManager();

//OLED Helers
TaskHandle_t Task2;  //OLED requires second core
char ledCommandOLED;
String noteChars[12];
int volumeBars[4];

void setup() {

  noteChars[0] = String("C_");
  noteChars[1] = String("C#");
  noteChars[2] = String("D_");
  noteChars[3] = String("D#");
  noteChars[4] = String("E_");
  noteChars[5] = String("F_");
  noteChars[6] = String("F#");
  noteChars[7] = String("G_");
  noteChars[8] = String("G#");
  noteChars[9] = String("A_");
  noteChars[10] = String("A#");
  noteChars[11] = String("B");

  xTaskCreatePinnedToCore(
    Task2Loop,
    "Core 0 task",
    10000,
    NULL,
    1,
    &Task2,
    0);

  keypad.setDebounceTime(0);

  i2s.setPins(6, 7, 5);
  if (!i2s.begin(I2S_MODE_STD, sampleRate, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO)) {
    Serial.println("Failed to initialize I2S!");
    while (1) {};
  }

  screen.begin();
}

void loop() {

  inputManager.UpdateInput(keypad.getKey());
  char note = inputManager.note;
  char trackCommand = inputManager.trackCommand;
  int trackCommandArgument = inputManager.trackCommandArgument;
  char ledCommand = inputManager.ledCommand;

  if (ledCommand != ' ') {
    ledCommandOLED = ledCommand;
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

void Task2Loop(void *parameter) {
  for (;;) {
    screen.firstPage();
    do {
      screenManager.Update(tracker, screen, ledCommandOLED, volumeBars, noteChars);
    } while (screen.nextPage());
  }
}
