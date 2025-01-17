#include <MIDI.h>
#define RX_PIN 40  // GPIO 40 for MIDI input
#define TX_PIN -1  // TX pin not used
// Create a MIDI object
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);

//IMPORTANT: COMMENT OUT ISOLED BELOW IF NOT USING ACTUAL OLED
#define ISOLED

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
int sampleRate = 22050;  // sample rate in Hz

//OLED init
#include "ScreenManager.h"
#include <U8g2lib.h>
#include <Wire.h>
#include "FSManager.h"


U8G2_SSD1306_128X64_NONAME_1_HW_I2C screen = U8G2_SSD1306_128X64_NONAME_1_HW_I2C(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/47, /* data=*/48);
ScreenManager screenManager = ScreenManager();

//OLED Helpers
TaskHandle_t Task2;  //OLED requires second core
char ledCommandOLED;
String noteChars[12];
int volumeBars[4];
int lastVol;
bool trackerUI = true;
bool debounce;
bool selectingMode = true;
int saveCount = 0;
int loadCount = 0;
bool midiMode = false;

//file system
FSManager fsManager = FSManager(tracker);


void SetupFS() {
}

void setup() {
  keypad.setHoldTime(1000);
  keypad.addEventListener(keypadEvent);
  fsManager.init(tracker);
#ifdef ISOLED
  xTaskCreatePinnedToCore(
    Task2Loop,
    "Core 0 task",
    10000,
    NULL,
    1,
    &Task2,
    0);
#endif
  screen.begin();
  while (selectingMode) {
    char trackerInput = keypad.getKey();
    trackerUI = false;

    if (trackerInput == 'M') {
      selectingMode = false;
      sampleRate = 44100;
    }
    if (trackerInput == 'N') {
      selectingMode = false;
      sampleRate = 22050;
      midiMode = true;
      tracker.SetMidiMode();
    }
    delay(1);
  }

  screenManager.showIntro = false;

  tracker.trackerUI = trackerUI;
  screenManager.trackerUI = trackerUI;
  tracker.ClearAll(0);
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


  keypad.setDebounceTime(0);

  i2s.setPins(6, 7, 5);
  if (!i2s.begin(I2S_MODE_STD, sampleRate, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO)) {
    while (1) {};
  }

  if (midiMode) {
    Serial2.begin(31250, SERIAL_8N1, RX_PIN, TX_PIN);
    // Set up the MIDI library with Serial2 as the input interface
    MIDI.begin(MIDI_CHANNEL_OMNI);  // Listen to all channels
    MIDI.setHandleClock(handleClock);
    MIDI.setHandleStart(handleStart);
    MIDI.setHandleNoteOn(handleNoteOn);
  }
}

int noteMidi = -1;
int octMidi = 0;

void handleNoteOn(byte channel, byte note, byte velocity) {
  if (noteMidi == -1) {
    int n = (int)(note % 12);
    if (n<1)
    {
      n=1;
    }
    if (n>13){
      n=13;
    }
    int o = (int)(note / 12) - 5;
    if (o < 0) {
      o = 0;
    }
    if (o > 2) {
      o = 2;
    }
    noteMidi = n;
    octMidi = o;
  }
}

void handleClock() {
  tracker.OnMidiTick();
}

void handleStart() {
  tracker.OnMidiStart();
}

void keypadEvent(KeypadEvent key) {
  switch (keypad.getState()) {
    case PRESSED:

      break;

    case RELEASED:

      break;

    case HOLD:
      if (key == 'M') {
        trackerUI = !trackerUI;
        tracker.trackerUI = trackerUI;
        screenManager.trackerUI = trackerUI;
        inputManager.ledCommand = ' ';
        ledCommandOLED = ' ';
        inputManager.trackCommand = ' ';
        inputManager.ClearFunctions();
        ledManager.SetCommand('T');
      }
      if (key == 'N') {
        tracker.playThrough = !tracker.playThrough;
        if (!tracker.playThrough) {
          tracker.BuildOLEDHintString("Rec On");
        } else {
          tracker.BuildOLEDHintString("Rec Off");
        }
        inputManager.ledCommand = ' ';
        ledCommandOLED = ' ';
        inputManager.trackCommand = ' ';
        inputManager.ClearFunctions();
        ledManager.SetCommand('T');
      }
      if (key == 'O') {
        int loaded = fsManager.load(tracker);

        if (loaded == 0) {
          tracker.BuildOLEDHintString("Load Fail");
        } else {
          tracker.pressedOnce = true;
          tracker.BuildOLEDHintString("Loaded...");
          tracker.isPlaying = true;
        }
        inputManager.ledCommand = ' ';
        ledCommandOLED = ' ';
        inputManager.trackCommand = ' ';
        inputManager.ClearFunctions();
        ledManager.SetCommand('T');
      }
      if (key == 'P') {
        int saved = fsManager.save(tracker);
        tracker.BuildOLEDHintString("Saved...");

        if (saved == 0) {
          tracker.BuildOLEDHintString("Save Failed");
        }
        inputManager.ledCommand = ' ';
        ledCommandOLED = ' ';
        inputManager.trackCommand = ' ';
        inputManager.ClearFunctions();
        ledManager.SetCommand('T');
      }

      break;
  }
}

void loop() {
  if (midiMode) {
    MIDI.read();
  }
  if (screenManager.cursorMode == 0) {
    inputManager.UpdateInput(keypad.getKey());
    char note = inputManager.note;
    char trackCommand = inputManager.trackCommand;
    int trackCommandArgument = inputManager.trackCommandArgument;
    char ledCommand = inputManager.ledCommand;

    if (noteMidi > -1) {
      note = noteMidi;
      trackCommand = 'N';
      trackCommandArgument = note;
      tracker.voices[tracker.selectedTrack].SetOctave(octMidi);
      noteMidi = -1;
    }
    if (ledCommand != ' ') {
      ledCommandOLED = ledCommand;
      ledManager.SetCommand(ledCommand);
    }
    if (trackCommand != ' ') {

      if (trackCommand == 'N' && trackerUI) {
        screenManager.OnInput(trackCommandArgument, tracker);
      } else {
        tracker.SetCommand(trackCommand, trackCommandArgument);
      }
    }

    ledManager.UpdateLed();
  } else {

    char trackerInput = keypad.getKey();
    if (trackerInput && !debounce) {
      if (trackerInput == 'M') {
        screenManager.cursorMode = 0;
      } else if (trackerInput == 'N') {
        screenManager.MoveCursor(-1);
      } else if (trackerInput == 'O') {
        screenManager.MoveCursor(1);
      } else if (trackerInput == 'P') {
        tracker.voices[tracker.selectedTrack].octave++;
        if (tracker.voices[tracker.selectedTrack].octave > 2) {
          tracker.voices[tracker.selectedTrack].octave = 0;
        }
      } else {
        screenManager.OnNote(inputManager.ConvertToInt(trackerInput), tracker);
      }

      debounce = true;
    } else {
      debounce = false;
    }
  }

  int sample = tracker.UpdateTracker();

  // New i2s only wants to write bytes out, so we need to split the sample before writing
  // Copy the high and low bytes of our 16bit sample into a buffer and write that
  byte outbuf[2];
  int outVol = (tracker.sample + (lastVol / 2));
  lastVol = tracker.sample;
  outbuf[0] = lowByte(outVol);
  outbuf[1] = highByte(outVol);
  i2s.write(outbuf, 2);

  int tempoBlink = tracker.tempoBlink;
  if (tempoBlink > 0)
    ledManager.SetLit(tempoBlink, tracker.selectedTrack);

  ledManager.SetPattern(tracker.allPatternPlay, tracker.currentPattern);

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
