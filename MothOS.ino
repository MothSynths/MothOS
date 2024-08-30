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
const int sampleRate = 44100;  // sample rate in Hz


#include <U8g2lib.h>
#include <Wire.h>
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/47, /* data=*/48);

TaskHandle_t Task2;

String noteChars[12];

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

  //setup I2S pins
  // i2s.setPins(I2S_PIN_BCLK, I2S_PIN_WS, I2S_PIN_DOUT);
  i2s.setPins(6, 7, 5);

  // New i2s driver defaults to PHILIPS MODE when in STD mode, and will automatically
  // fill both slots (ie left + right) with the same sample when using I2S_SLOT_MODE_MONO
  if (!i2s.begin(I2S_MODE_STD, sampleRate, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO)) {
    Serial.println("Failed to initialize I2S!");
    while (1) {};
  }
  u8g2.begin();
}

char ledCommandOLED;
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
    u8g2.firstPage();
    do {
      UpdateInstructionsScreen();
      //UpdateMainScreen();
    } while (u8g2.nextPage());
  }
}

void UpdateInstructionsScreen() {
  u8g2.setFont(u8g2_font_6x13_tf);
  int xs = 32;
  int marginleft = 3;
  int margintop = 13;
  int ys = 16;


  int showF = 1;

  if (ledCommandOLED == 'D') {
    for (int x = 0; x < 4; x++) {
      for (int y = 0; y < 4; y++) {
        u8g2.drawRFrame(x * 32, y * 16, 31, 16, 2);
      }
    }
    u8g2.drawStr(xs * 0 + marginleft, ys * 0 + margintop, "NLNG");
    u8g2.drawStr(xs * 1 + marginleft, ys * 0 + margintop, "NMED");
    u8g2.drawStr(xs * 2 + marginleft, ys * 0 + margintop, "NSRT");
    u8g2.drawStr(xs * 3 + marginleft, ys * 0 + margintop, "REC");
    u8g2.drawStr(xs * 0 + marginleft, ys * 1 + margintop, "BPM1");
    u8g2.drawStr(xs * 1 + marginleft, ys * 1 + margintop, "BPM2");
    u8g2.drawStr(xs * 2 + marginleft, ys * 1 + margintop, "BPM3");
    u8g2.drawStr(xs * 3 + marginleft, ys * 1 + margintop, "BPM4");
    u8g2.drawStr(xs * 0 + marginleft, ys * 2 + margintop, "CPAT");
    u8g2.drawStr(xs * 1 + marginleft, ys * 2 + margintop, "PPAT");
    u8g2.drawStr(xs * 2 + marginleft, ys * 2 + margintop, "PALL");
    u8g2.drawStr(xs * 3 + marginleft, ys * 2 + margintop, "N/SP");
    u8g2.drawStr(xs * 0 + marginleft, ys * 3 + margintop, "RSTS");
    u8g2.drawStr(xs * 1 + marginleft, ys * 3 + margintop, "RSTL");
    u8g2.drawStr(xs * 2 + marginleft, ys * 3 + margintop, "MVOL");
    u8g2.drawStr(xs * 3 + marginleft, ys * 3 + margintop, "PTSN");
  } else if (ledCommandOLED == 'C') {
    for (int x = 0; x < 4; x++) {
      for (int y = 0; y < 4; y++) {
        u8g2.drawRFrame(x * 32, y * 16, 31, 16, 2);
      }
    }
    u8g2.drawStr(xs * 0 + marginleft, ys * 0 + margintop, "TRK1");
    u8g2.drawStr(xs * 1 + marginleft, ys * 0 + margintop, "TRK2");
    u8g2.drawStr(xs * 2 + marginleft, ys * 0 + margintop, "TRK3");
    u8g2.drawStr(xs * 3 + marginleft, ys * 0 + margintop, "TRK4");
    u8g2.drawStr(xs * 0 + marginleft, ys * 1 + margintop, "CLT1");
    u8g2.drawStr(xs * 1 + marginleft, ys * 1 + margintop, "CLT2");
    u8g2.drawStr(xs * 2 + marginleft, ys * 1 + margintop, "CLT3");
    u8g2.drawStr(xs * 3 + marginleft, ys * 1 + margintop, "CLT4");
    u8g2.drawStr(xs * 0 + marginleft, ys * 2 + margintop, "PAT1");
    u8g2.drawStr(xs * 1 + marginleft, ys * 2 + margintop, "PAT2");
    u8g2.drawStr(xs * 2 + marginleft, ys * 2 + margintop, "PAT3");
    u8g2.drawStr(xs * 3 + marginleft, ys * 2 + margintop, "PAT4");
    u8g2.drawStr(xs * 0 + marginleft, ys * 3 + margintop, "CLP1");
    u8g2.drawStr(xs * 1 + marginleft, ys * 3 + margintop, "CLP2");
    u8g2.drawStr(xs * 2 + marginleft, ys * 3 + margintop, "CLP3");
    u8g2.drawStr(xs * 3 + marginleft, ys * 3 + margintop, "CLP4");
  } else if (ledCommandOLED == 'B') {
    for (int x = 0; x < 4; x++) {
      for (int y = 0; y < 4; y++) {
        u8g2.drawRFrame(x * 32, y * 16, 31, 16, 2);
      }
    }
    u8g2.drawStr(xs * 0 + marginleft, ys * 0 + margintop, "VOL1");
    u8g2.drawStr(xs * 1 + marginleft, ys * 0 + margintop, "VOL2");
    u8g2.drawStr(xs * 2 + marginleft, ys * 0 + margintop, "VOL3");
    u8g2.drawStr(xs * 3 + marginleft, ys * 0 + margintop, "OVDR");
    u8g2.drawStr(xs * 0 + marginleft, ys * 1 + margintop, "ENV1");
    u8g2.drawStr(xs * 1 + marginleft, ys * 1 + margintop, "ENV2");
    u8g2.drawStr(xs * 2 + marginleft, ys * 1 + margintop, "ENV3");
    u8g2.drawStr(xs * 3 + marginleft, ys * 1 + margintop, "LOOP");
    u8g2.drawStr(xs * 0 + marginleft, ys * 2 + margintop, "ECHO");
    u8g2.drawStr(xs * 1 + marginleft, ys * 2 + margintop, "CHRD");
    u8g2.drawStr(xs * 2 + marginleft, ys * 2 + margintop, "WOOS");
    u8g2.drawStr(xs * 3 + marginleft, ys * 2 + margintop, "PTCH");
    u8g2.drawStr(xs * 0 + marginleft, ys * 3 + margintop, "NOFX");
    u8g2.drawStr(xs * 1 + marginleft, ys * 3 + margintop, "LOWP");
    u8g2.drawStr(xs * 2 + marginleft, ys * 3 + margintop, "RTRG");
    u8g2.drawStr(xs * 3 + marginleft, ys * 3 + margintop, "WOBB");
  } else if (ledCommandOLED == 'A') {
    for (int x = 0; x < 4; x++) {
      for (int y = 0; y < 4; y++) {
        u8g2.drawRFrame(x * 32, y * 16, 31, 16, 2);
      }
    }
    u8g2.drawStr(xs * 0 + marginleft, ys * 0 + margintop, "OCT0");
    u8g2.drawStr(xs * 1 + marginleft, ys * 0 + margintop, "OCT1");
    u8g2.drawStr(xs * 2 + marginleft, ys * 0 + margintop, "OCT2");
    u8g2.drawStr(xs * 3 + marginleft, ys * 0 + margintop, "OCT3");
    u8g2.drawStr(xs * 0 + marginleft, ys * 1 + margintop, "INS6");
    u8g2.drawStr(xs * 1 + marginleft, ys * 1 + margintop, "INS7");
    u8g2.drawStr(xs * 2 + marginleft, ys * 1 + margintop, "INS8");
    u8g2.drawStr(xs * 3 + marginleft, ys * 1 + margintop, "INS9");
    u8g2.drawStr(xs * 0 + marginleft, ys * 2 + margintop, "INS2");
    u8g2.drawStr(xs * 1 + marginleft, ys * 2 + margintop, "INS3");
    u8g2.drawStr(xs * 2 + marginleft, ys * 2 + margintop, "INS4");
    u8g2.drawStr(xs * 3 + marginleft, ys * 2 + margintop, "INS5");
    u8g2.drawStr(xs * 0 + marginleft, ys * 3 + margintop, "DRMS");
    u8g2.drawStr(xs * 1 + marginleft, ys * 3 + margintop, "SFX");
    u8g2.drawStr(xs * 2 + marginleft, ys * 3 + margintop, "INS0");
    u8g2.drawStr(xs * 3 + marginleft, ys * 3 + margintop, "INS1");
  } else {
    UpdateMainScreen();
    if (tracker.hintTime > 0) {
      u8g2.setColorIndex(0);
      u8g2.drawBox(0, 0, 126, 22);
      u8g2.setColorIndex(1);
      u8g2.drawRFrame(0, 0, 126, 22, 2);
      tracker.hintTime--;
      u8g2.drawStr(4, 16, tracker.hint);
    }
  }
}


int volumeBars[4];

void UpdateMainScreen() {

  u8g2.setFont(u8g2_font_6x13_tf);

  for (int i = 0; i < 4; i++) {
    int val = tracker.lastSamples[i] / 100;
    val = abs(val);
    if (val > 23) {
      val = 23;
    }
    if (volumeBars[i] < val) {
      volumeBars[i] = val;
    } else {
      volumeBars[i] -= 1;
      if (volumeBars[i] < 0)
        volumeBars[i] = 0;
    }
    u8g2.drawBox(i * 20 + 52, 24 - volumeBars[i], 12, volumeBars[i]);
    u8g2.drawRFrame(i * 20 + 50, 0, 16, 24, 2);


    char buff[4];
    bool showNote = false;
    int dnote = 0;
    int doct = 0;
    for (int r = 0; r < 3; r++) {
      int trackIndex = tracker.trackIndex - r;
      if (trackIndex < 0) {
        trackIndex += tracker.patternLength;
      }
      int note = tracker.tracks[i][trackIndex] - 1;
      int oct = tracker.trackOctaves[i][trackIndex];
      if (note < 0) {

      } else {
        dnote = note;
        doct = oct;
        showNote = true;
      }
    }
    if (showNote) {
      u8g2.setFont(u8g2_font_5x7_tf);
      String(noteChars[dnote] + String(doct)).toCharArray(buff, 4);
      u8g2.drawStr(i * 20 + 51, 34, buff);
    }
  }

  u8g2.setFont(u8g2_font_logisoso16_tf);
  String s = "";
  if (tracker.trackIndex % tracker.patternLength < 10) {
    s = "0" + String(tracker.trackIndex % tracker.patternLength);
  } else {
    s = String(tracker.trackIndex % tracker.patternLength);
  }
  s += "/" + String(tracker.patternLength);
  char buf[6];

  s.toCharArray(buf, 6);
  u8g2.drawStr(4, 63, buf);

  s = String(tracker.currentPattern + 1);
  s += "/4";
  s.toCharArray(buf, 6);
  u8g2.drawStr(100, 63, buf);
  u8g2.setFont(u8g2_font_6x13_tf);
  u8g2.drawStr(76, 63, "PAT:");


  u8g2.drawStr(4, 10, tracker.oledInstString);
  char buff[6];
  String("OC:" + String(tracker.voices[tracker.selectedTrack].octave)).toCharArray(buff, 6);
  u8g2.drawStr(4, 22, buff);
  if (!tracker.voices[tracker.selectedTrack].samplerMode)
    String("NOTE").toCharArray(buff, 6);
  else
    String("SAMP").toCharArray(buff, 6);
  u8g2.drawStr(4, 34, buff);
}
