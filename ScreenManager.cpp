/*Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

*/

#include <Arduino.h>
#include "ScreenManager.h"
#include "Tracker.h"

ScreenManager::ScreenManager() {
  cursorX = 0;
  cursorY = 0;
}

void ScreenManager::Update(Tracker &tracker, U8G2_SSD1306_128X64_NONAME_1_HW_I2C &screen, char ledCommandOLED, int volumeBars[4], String noteChars[12]) {
  bool isShowingInstructions = UpdateInstructionsScreen(tracker, screen, ledCommandOLED, volumeBars, noteChars);
  if (!isShowingInstructions) {
    UpdateMainScreen(tracker, screen, ledCommandOLED, volumeBars, noteChars);
    if (tracker.hintTime > 0) {
      screen.setColorIndex(0);
      screen.drawBox(0, 0, 126, 22);
      screen.setColorIndex(1);
      screen.drawRFrame(0, 0, 126, 22, 2);
      tracker.hintTime--;
      screen.drawStr(4, 16, tracker.hint);
    }
  }
}

bool ScreenManager::UpdateInstructionsScreen(Tracker &tracker, U8G2_SSD1306_128X64_NONAME_1_HW_I2C &screen, char ledCommandOLED, int volumeBars[4], String noteChars[12]) {
  screen.setFont(u8g2_font_6x13_tf);
  int xs = 32;
  int marginleft = 3;
  int margintop = 13;
  int ys = 16;

  if (ledCommandOLED == 'D') {
    for (int x = 0; x < 4; x++) {
      for (int y = 0; y < 4; y++) {
        screen.drawRFrame(x * 32, y * 16, 31, 16, 2);
      }
    }
    screen.drawStr(xs * 0 + marginleft, ys * 0 + margintop, "NLNG");
    screen.drawStr(xs * 1 + marginleft, ys * 0 + margintop, "NMED");
    screen.drawStr(xs * 2 + marginleft, ys * 0 + margintop, "NSRT");
    screen.drawStr(xs * 3 + marginleft, ys * 0 + margintop, "REC");
    screen.drawStr(xs * 0 + marginleft, ys * 1 + margintop, "BPM1");
    screen.drawStr(xs * 1 + marginleft, ys * 1 + margintop, "BPM2");
    screen.drawStr(xs * 2 + marginleft, ys * 1 + margintop, "BPM3");
    screen.drawStr(xs * 3 + marginleft, ys * 1 + margintop, "BPM4");
    screen.drawStr(xs * 0 + marginleft, ys * 2 + margintop, "CPAT");
    screen.drawStr(xs * 1 + marginleft, ys * 2 + margintop, "PPAT");
    screen.drawStr(xs * 2 + marginleft, ys * 2 + margintop, "PALL");
    screen.drawStr(xs * 3 + marginleft, ys * 2 + margintop, "N/SP");
    screen.drawStr(xs * 0 + marginleft, ys * 3 + margintop, "RSTS");
    screen.drawStr(xs * 1 + marginleft, ys * 3 + margintop, "RSTL");
    screen.drawStr(xs * 2 + marginleft, ys * 3 + margintop, "MVOL");
    screen.drawStr(xs * 3 + marginleft, ys * 3 + margintop, "PTSN");
  } else if (ledCommandOLED == 'C') {
    for (int x = 0; x < 4; x++) {
      for (int y = 0; y < 4; y++) {
        screen.drawRFrame(x * 32, y * 16, 31, 16, 2);
      }
    }
    screen.drawStr(xs * 0 + marginleft, ys * 0 + margintop, "TRK1");
    screen.drawStr(xs * 1 + marginleft, ys * 0 + margintop, "TRK2");
    screen.drawStr(xs * 2 + marginleft, ys * 0 + margintop, "TRK3");
    screen.drawStr(xs * 3 + marginleft, ys * 0 + margintop, "TRK4");
    screen.drawStr(xs * 0 + marginleft, ys * 1 + margintop, "CLT1");
    screen.drawStr(xs * 1 + marginleft, ys * 1 + margintop, "CLT2");
    screen.drawStr(xs * 2 + marginleft, ys * 1 + margintop, "CLT3");
    screen.drawStr(xs * 3 + marginleft, ys * 1 + margintop, "CLT4");
    screen.drawStr(xs * 0 + marginleft, ys * 2 + margintop, "PAT1");
    screen.drawStr(xs * 1 + marginleft, ys * 2 + margintop, "PAT2");
    screen.drawStr(xs * 2 + marginleft, ys * 2 + margintop, "PAT3");
    screen.drawStr(xs * 3 + marginleft, ys * 2 + margintop, "PAT4");
    screen.drawStr(xs * 0 + marginleft, ys * 3 + margintop, "CLP1");
    screen.drawStr(xs * 1 + marginleft, ys * 3 + margintop, "CLP2");
    screen.drawStr(xs * 2 + marginleft, ys * 3 + margintop, "CLP3");
    screen.drawStr(xs * 3 + marginleft, ys * 3 + margintop, "CLP4");
  } else if (ledCommandOLED == 'B') {
    for (int x = 0; x < 4; x++) {
      for (int y = 0; y < 4; y++) {
        screen.drawRFrame(x * 32, y * 16, 31, 16, 2);
      }
    }
    screen.drawStr(xs * 0 + marginleft, ys * 0 + margintop, "MUTE");
    screen.drawStr(xs * 1 + marginleft, ys * 0 + margintop, "VOL");
    screen.drawStr(xs * 2 + marginleft, ys * 0 + margintop, "OVDR");
    screen.drawStr(xs * 3 + marginleft, ys * 0 + margintop, "SOLO");
    screen.drawStr(xs * 0 + marginleft, ys * 1 + margintop, "ENV1");
    screen.drawStr(xs * 1 + marginleft, ys * 1 + margintop, "ENV2");
    screen.drawStr(xs * 2 + marginleft, ys * 1 + margintop, "ENV3");
    screen.drawStr(xs * 3 + marginleft, ys * 1 + margintop, "LOOP");
    screen.drawStr(xs * 0 + marginleft, ys * 2 + margintop, "ECHO");
    screen.drawStr(xs * 1 + marginleft, ys * 2 + margintop, "CHRD");
    screen.drawStr(xs * 2 + marginleft, ys * 2 + margintop, "WOOS");
    screen.drawStr(xs * 3 + marginleft, ys * 2 + margintop, "PTCH");
    screen.drawStr(xs * 0 + marginleft, ys * 3 + margintop, "NOFX");
    screen.drawStr(xs * 1 + marginleft, ys * 3 + margintop, "LOWP");
    screen.drawStr(xs * 2 + marginleft, ys * 3 + margintop, "RTRG");
    screen.drawStr(xs * 3 + marginleft, ys * 3 + margintop, "WOBB");
  } else if (ledCommandOLED == 'A') {
    for (int x = 0; x < 4; x++) {
      for (int y = 0; y < 4; y++) {
        screen.drawRFrame(x * 32, y * 16, 31, 16, 2);
      }
    }
    screen.drawStr(xs * 0 + marginleft, ys * 0 + margintop, "OCT0");
    screen.drawStr(xs * 1 + marginleft, ys * 0 + margintop, "OCT1");
    screen.drawStr(xs * 2 + marginleft, ys * 0 + margintop, "OCT2");
    screen.drawStr(xs * 3 + marginleft, ys * 0 + margintop, "OCT3");
    screen.drawStr(xs * 0 + marginleft, ys * 1 + margintop, "INS6");
    screen.drawStr(xs * 1 + marginleft, ys * 1 + margintop, "INS7");
    screen.drawStr(xs * 2 + marginleft, ys * 1 + margintop, "INS8");
    screen.drawStr(xs * 3 + marginleft, ys * 1 + margintop, "INS9");
    screen.drawStr(xs * 0 + marginleft, ys * 2 + margintop, "INS2");
    screen.drawStr(xs * 1 + marginleft, ys * 2 + margintop, "INS3");
    screen.drawStr(xs * 2 + marginleft, ys * 2 + margintop, "INS4");
    screen.drawStr(xs * 3 + marginleft, ys * 2 + margintop, "INS5");
    screen.drawStr(xs * 0 + marginleft, ys * 3 + margintop, "DRMS");
    screen.drawStr(xs * 1 + marginleft, ys * 3 + margintop, "SFX");
    screen.drawStr(xs * 2 + marginleft, ys * 3 + margintop, "INS0");
    screen.drawStr(xs * 3 + marginleft, ys * 3 + margintop, "INS1");
  } else {
    return false;
  }
  return true;
}
void ScreenManager::OnNote(int input, Tracker &tracker) {

  int patternOffset = tracker.currentPattern * tracker.patternLength;
  int val = tracker.tracks[cursorY][cursorX];
  if (val > 0) {
    tracker.tracks[cursorY][cursorX] = 0;
  } else {
    int oldTrackIndex = tracker.trackIndex;
    tracker.trackIndex = cursorX + patternOffset;
    tracker.SetNote(input, cursorY);
    tracker.trackIndex = oldTrackIndex;
  }
  cursorX++;
  if (cursorX > 31) {
    cursorX = 0;
  }
}

void ScreenManager::MoveCursor(int dir) {
  cursorX += dir;
  if (cursorX > 31) {
    cursorX = 0;
  }
  if (cursorX < 0) {
    cursorX = 0;
  }
}

void ScreenManager::OnInput(int input, Tracker &tracker) {
  int patternOffset = tracker.currentPattern * tracker.patternLength;
  if (cursorMode == 1) {
    return;
  }
  if (input == 6) {
    int oldTrackIndex = tracker.trackIndex;
    tracker.trackIndex = cursorX + patternOffset;
    tracker.SetNote(lastNote, cursorY);
    tracker.trackIndex = oldTrackIndex;
    return;
  }
  if (input == 7) {
    int oldTrackIndex = tracker.trackIndex;
    tracker.tracks[cursorY][cursorX] = 0;
    tracker.trackIndex = oldTrackIndex;
    return;
  }

  if (input == 2) {
    cursorX++;
  }
  if (input == 0) {
    cursorX--;
  }
  if (input == 1) {
    cursorY++;
    if (cursorY > 3)
      cursorY = 3;
    tracker.selectedTrack = cursorY;
  }
  if (input == 5) {
    cursorY--;
    if (cursorY < 0)
      cursorY = 0;
    tracker.selectedTrack = cursorY;
  }
  if (input == 3) {
    cursorMode = 1;
  }
}

void ScreenManager::UpdateMainScreen(Tracker &tracker, U8G2_SSD1306_128X64_NONAME_1_HW_I2C &screen, char ledCommandOLED, int volumeBars[4], String noteChars[12]) {
  screen.setFont(u8g2_font_6x13_tf);

  char buf[6];
  char buf32[22];
  String s = String(tracker.currentPattern + 1);
  s += "/4";
  s.toCharArray(buf, 6);
  screen.drawStr(0, 44, buf);
  if (!tracker.voices[tracker.selectedTrack].samplerMode) {
    s = String(tracker.oledInstString);
    s.toCharArray(buf, 6);
    screen.drawStr(30, 44, buf);
  } else {
    s = String("SAMP");
    s.toCharArray(buf, 6);
    screen.drawStr(30, 44, buf);
  }

  if (cursorMode == 0) {
    s = String("D=Notes, A,B,C,E=Move");
  } else {
    s = String("F1=Exit, F2,F3=Move");
  }
  s.toCharArray(buf32, 22);
  screen.drawStr(0, 56, buf32);

  int patternOffset = tracker.currentPattern * tracker.patternLength;
  int note = tracker.tracks[cursorY][patternOffset + cursorX] - 1;
  int oct = tracker.trackOctaves[cursorY][patternOffset + cursorX];
  String(noteChars[note] + String(oct)).toCharArray(buf, 4);

  if (note >= 0)
    screen.drawStr(64, 44, buf);

  for (int i = 0; i < 4; i++) {

    char trackChars[32];
    for (int j = 0; j < 32; j++) {
      int jp = j + patternOffset;
      int val = tracker.tracks[i][jp];

      if (j % 4 == 0) {
        screen.drawBox(j * 4 + 1, i * 8 + 4, 1, 1);
      } else {
      }
      if (val == 0) {

      } else {
        screen.drawBox(j * 4 + 1, i * 8 + 1, 2, 6);
      }
      if (jp == tracker.trackIndex) {
        screen.drawBox(j * 4, i * 8, 1, 8);
      }
      if (cursorX == j && cursorY == i) {
        if (cursorMode == 0) {
          screen.drawFrame(j * 4, i * 8, 4, 8);
        } else {
          screen.drawBox(j * 4, i * 8, 4, 8);
        }
      }
    }
    //screen.drawStr(2, 12 + (i * 12), trackChars);
  }
}

/*
void ScreenManager::UpdateMainScreen(Tracker &tracker, U8G2_SSD1306_128X64_NONAME_1_HW_I2C &screen, char ledCommandOLED, int volumeBars[4], String noteChars[12]) {
  char buffa[4];
  if (tracker.lastNoteTrackIndex == lastNoteBeat) {
    if (noteBeatTime > 0) {
      noteBeatTime--;
    }
  } else {
    lastNoteBeat = tracker.lastNoteTrackIndex;
    noteBeatTime = 200;
  }
  if (noteBeatTime > 0) {
    String(tracker.lastNoteTrackIndex).toCharArray(buffa, 4);
    screen.drawStr(56, 63, buffa);
  }



  screen.setFont(u8g2_font_6x13_tf);
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
    screen.drawBox(i * 20 + 52, 24 - volumeBars[i], 12, volumeBars[i]);
    screen.drawRFrame(i * 20 + 50, 0, 16, 24, 2);

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
      screen.setFont(u8g2_font_5x7_tf);
      String(noteChars[dnote] + String(doct)).toCharArray(buff, 4);
      screen.drawStr(i * 20 + 51, 34, buff);
    }
  }

  screen.setFont(u8g2_font_logisoso16_tf);

  if (!tracker.pressedOnce) {
    char buf[6];
    String s = "READY";
    s.toCharArray(buf, 6);
    screen.drawStr(4, 63, buf);
  } else {
    String s = "";
    if (tracker.trackIndex % tracker.patternLength < 10) {
      s = "0" + String(tracker.trackIndex % tracker.patternLength);
    } else {
      s = String(tracker.trackIndex % tracker.patternLength);
    }
    s += "/" + String(tracker.patternLength);
    char buf[6];
    s.toCharArray(buf, 6);
    screen.drawStr(4, 63, buf);
  }
  
  char buf[6];
  String s = String(tracker.currentPattern + 1);
  s += "/4";
  s.toCharArray(buf, 6);
  screen.drawStr(100, 63, buf);
  screen.setFont(u8g2_font_6x13_tf);
  screen.drawStr(76, 63, "PAT:");
  screen.drawStr(4, 10, tracker.oledInstString);
  char buff[6];
  String("OC:" + String(tracker.voices[tracker.selectedTrack].octave)).toCharArray(buff, 6);
  screen.drawStr(4, 22, buff);
  if (!tracker.voices[tracker.selectedTrack].samplerMode)
    String("NOTE").toCharArray(buff, 6);
  else
    String("SAMP").toCharArray(buff, 6);
  screen.drawStr(4, 34, buff);
}
*/