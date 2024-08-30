#include <Arduino.h>
#include "Tracker.h"
#include "Voice.h"

Tracker::Tracker() {
  patternLength = 32;
  isPlaying = true;
  bpms[0] = 31000;
  bpms[1] = 31001;
  bpms[2] = 31002;
  bpms[3] = 31003;
  SetBPM(0);

  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < 256; i++) {
      tracks[j][i] = 0;
    }
  }

  ClearAll(0);
}

int Tracker::UpdateTracker() {
  float curTime = millis();
  float delta = curTime - lastMillis;
  tempoBlink = 0;
  lastMillis = curTime;


  float dbps = delta * bps;
  beatTime += dbps;
  noteTime += dbps;
  if (beatTime > 1000) {
    beatTime -= 1000;
    barCount++;
    tempoBlink = 20;
    if (barCount > 7) {
      tempoBlink = 100;
      barCount = 0;
    }
  }

  if (noteTime > 250) {
    noteTime -= 250;
    trackIndex++;

    if (trackIndex >= patternLength * (currentPattern + 1)) {
      trackIndex = patternLength * (currentPattern + 0);
      if (allPatternPlay) {
        currentPattern++;
        if (currentPattern > 3) {
          currentPattern -= 4;
        }
        trackIndex = patternLength * (currentPattern + 0);
      }
    }

    int trackIndexBehind = trackIndex - 1;
    if (trackIndexBehind < patternLength * (currentPattern + 0)) {
      trackIndexBehind = patternLength * (currentPattern + 1) + trackIndexBehind;
    }

    for (int i = 0; i < 4; i++) {
      int note = tracks[i][trackIndexBehind];
      int optOctave = trackOctaves[i][trackIndexBehind];
      int optInstrument = trackInstruments[i][trackIndexBehind];

      if (note > 0) {
        heldNotes[i] = note;
        heldInsturments[i] = currentVoice;
        voices[i].SetNote(note - 1, false, optOctave, optInstrument);
      }
    }
  }

  sample = 0;

  for (int i = 0; i < 4; i++) {
    voices[i].bps = bps;
    int samp = voices[i].UpdateVoice() / (3 + masterVolume * 5);
    sample += samp;
    lastSamples[i] = samp;
  }

  return 0;
}

void Tracker::SetCommand(char command, int val) {
  switch (command) {
    case 'T':
      SetTrackNum(val);
      hintTime = 120;
      String("Track: " + String(val + 1)).toCharArray(hint, 15);
      break;
    case 'B':
      hintTime = 120;
      String("BPM: " + String(bpms[val])).toCharArray(hint, 15);
      SetBPM(val);
      break;
    case 'N':
      if (!pressedOnce) {
        noteTime = 0;
        beatTime = 0;
        trackIndex = 0;
      }
      pressedOnce = true;
      SetNote(val, selectedTrack);
      break;
    case 'O':
      hintTime = 120;
      String("Octave: " + String(val)).toCharArray(hint, 15);
      SetOctave(val);
      break;
    case 'L':
      hintTime = 120;
      SetEnvelopeLength(val);
      String("Note Len: " + String(val + 1)).toCharArray(hint, 15);
      break;
    case 'E':
      hintTime = 120;
      switch (val) {
        case 0:
          String("Fade In").toCharArray(hint, 15);
          break;
        case 1:
          String("Fade Out").toCharArray(hint, 15);
          break;
        case 2:
          String("No Fade").toCharArray(hint, 15);
          break;
        case 3:
          String("Loop").toCharArray(hint, 15);
          break;
      }
      SetEnvelopeNum(val);
      break;
    case 'V':
      hintTime = 120;
      String("Volume: " + String(val + 1)).toCharArray(hint, 15);
      if (val == 3) {
        String("Overdrive").toCharArray(hint, 15);
      }
      SetVolume(val);
      break;
    case 'D':
      hintTime = 120;
      SetEffect(val + 4);
      switch (val) {
        case 0:
          String("Echo: " + String(voices[selectedTrack].delayMult)).toCharArray(hint, 15);
          break;
        case 1:
          String("ArpChord: " + String(voices[selectedTrack].chordMult)).toCharArray(hint, 15);
          break;
        case 2:
          String("Whoosh: " + String(voices[selectedTrack].whooshMult)).toCharArray(hint, 15);
          break;
        case 3:
          String("Pitchbend: " + String(voices[selectedTrack].pitchMult)).toCharArray(hint, 15);
          break;
      }


      break;
    case 'A':
      hintTime = 120;
      SetEffect(val);
      switch (val) {
        case 0:
          String("Effects Off").toCharArray(hint, 15);
          break;
        case 1:
          String("Low Pass: " + String(voices[selectedTrack].lowPassMult)).toCharArray(hint, 15);
          break;
        case 2:
          String("Retrig: " + String(voices[selectedTrack].reverbMult)).toCharArray(hint, 15);
          break;
        case 3:
          String("Wobble: " + String(voices[selectedTrack].phaserMult)).toCharArray(hint, 15);
          break;
      }
      break;
    case '^':
      hintTime = 120;
      String("Clr Track: " + String(val + 1)).toCharArray(hint, 15);
      ClearTrackNum(val);
      break;
    case '$':
      hintTime = 120;
      String("Pattern: " + String(val + 1)).toCharArray(hint, 15);
      SetPatternNum(val);
      break;
    case '#':
      hintTime = 120;
      String("Clr Pattern: " + String(val + 1)).toCharArray(hint, 15);
      ClearPatternNum(val);
      break;
    case 'X':
      hintTime = 120;
      String("New Song: " + String((32 * (val + 1)))).toCharArray(hint, 15);
      ClearAll(val);
      break;
    case 'P':
      hintTime = 120;
      String("Recording: " + String((bool)isPlaying)).toCharArray(hint, 15);
      TogglePlayStop();
      break;
    case 'I':
      currentVoice = val;
      hintTime = 120;
      if (val > 1) {
        String("Instrument: " + String(val)).toCharArray(hint, 15);
        String("INS" + String(val)).toCharArray(oledInstString, 8);
      } else if (val == 1) {
        String("SFX Bank").toCharArray(hint, 15);
        String("SFX").toCharArray(oledInstString, 6);
      } else {
        String("Drum Bank").toCharArray(hint, 15);
        String("DRUM").toCharArray(oledInstString, 6);
      }

      break;
    case 'H':

      masterVolume++;
      if (masterVolume > 1)
        masterVolume = 0;

      hintTime = 120;
      String("Mstr Volume: " + String(2 - masterVolume)).toCharArray(hint, 15);
      break;
    case 'K':

      break;
    case 'C':
      allPatternPlay = !allPatternPlay;
      hintTime = 120;
      if (allPatternPlay) {
        String("Song Mode").toCharArray(hint, 15);
      } else {
        String("Pattern Mode").toCharArray(hint, 15);
      }
      break;
    case '*':
      hintTime = 120;
      if (val == 0) {
        String("Copy Pattern").toCharArray(hint, 15);
        CopyPattern();
      }
      if (val == 1) {
        String("Paste Pattern").toCharArray(hint, 15);
        PastePattern();
      }
      if (val == 2) {
        String("Paste All Patt").toCharArray(hint, 15);
        PastePatternAll();
      }
      if (val == 3) {
        hintTime = 120;
        voices[selectedTrack].samplerMode = !voices[selectedTrack].samplerMode;
        voices[selectedTrack].SetEnvelopeNum(2);
        String("Samp Mode: " + String(voices[selectedTrack].samplerMode)).toCharArray(hint, 15);
      }

      break;
  }
}

void Tracker::SetEffect(int val) {
  voices[selectedTrack].SetEffectNum(val);
};

void Tracker::SetBPM(int val) {
  bpm = bpms[val];
  bps = bpm / 60;
};

void Tracker::SetDelay(int val) {
  if (val > 0)
    val += 1;
  voices[selectedTrack].delay = val;
};
void Tracker::SetEnvelopeNum(int val) {
  voices[selectedTrack].SetEnvelopeNum(val);
};

void Tracker::SetEnvelopeLength(int val) {
  voices[selectedTrack].SetEnvelopeLength((val));
};

void Tracker::SetOctave(int val) {
  voices[selectedTrack].SetOctave(val);
};

void Tracker::SetVolume(int val) {
  voices[selectedTrack].SetVolume(val);
};

void Tracker::SetNote(int val, int track) {
  if (isPlaying) {
    //one behind trick
    if (noteTime > 200) {
      tracks[track][trackIndex + 1] = val + 1;
      trackOctaves[track][trackIndex + 1] = voices[selectedTrack].octave;
      trackInstruments[track][trackIndex + 1] = currentVoice;
    } else {

      tracks[track][trackIndex] = val + 1;
      trackOctaves[track][trackIndex] = voices[selectedTrack].octave;
      trackInstruments[track][trackIndex] = currentVoice;
    }
  } else {
    voices[track].SetNote(val, false, -1, currentVoice);
  }
};

void Tracker::SetTrackNum(int val) {
  selectedTrack = val;
};

void Tracker::ClearTrackNum(int val) {
  Serial.println(val);
  for (int i = patternLength * (currentPattern); i < patternLength * (currentPattern + 1); i++) {
    tracks[val][i] = 0;
  }
  voices[val].arpNum = 0;
};

void Tracker::SetPatternNum(int val) {

  trackIndex = trackIndex - (patternLength * currentPattern);
  currentPattern = val;
  trackIndex += (patternLength * currentPattern);
};

void Tracker::ClearPatternNum(int val) {
  for (int j = 0; j < 4; j++) {
    for (int i = patternLength * (currentPattern); i < patternLength * (currentPattern + 1); i++) {
      tracks[j][i] = 0;
    }
    voices[j].arpNum = 0;
  }
};

void Tracker::TogglePlayStop() {
  isPlaying = !isPlaying;
};

//TBD
void Tracker::CopyTrack(){};
void Tracker::PasteTrack(){};

void Tracker::CopyPattern() {
  for (int j = 0; j < 4; j++) {
    int c = 0;
    for (int i = patternLength * (currentPattern); i < patternLength * (currentPattern + 1); i++) {
      patternCopy[j][c] = tracks[j][i];
      patternCopyInstruments[j][c] = trackInstruments[j][i];
      patternCopyOctaves[j][c] = trackOctaves[j][i];
      c++;
    }
  }
};

void Tracker::PastePattern() {
  for (int j = 0; j < 4; j++) {
    int c = 0;
    for (int i = patternLength * (currentPattern); i < patternLength * (currentPattern + 1); i++) {
      tracks[j][i] = patternCopy[j][c];
      trackInstruments[j][i] = patternCopyInstruments[j][c];
      trackOctaves[j][i] = patternCopyOctaves[j][c];
      c++;
    }
  }
};

void Tracker::PastePatternAll() {
  for (int r = 0; r < 4; r++) {
    for (int j = 0; j < 4; j++) {
      int c = 0;
      for (int i = patternLength * (r); i < patternLength * (r + 1); i++) {
        tracks[j][i] = patternCopy[j][c];
        trackInstruments[j][i] = patternCopyInstruments[j][c];
        trackOctaves[j][i] = patternCopyOctaves[j][c];
        c++;
      }
    }
  }
};

//TBD
void Tracker::SetPatternLength(int val){};
void Tracker::SaveDefaultSong(){};

void Tracker::ClearAll(int val) {
  selectedTrack = 0;
  currentPattern = 0;
  isPlaying = true;
  pressedOnce = false;
  allPatternPlay = false;
  String("DRUMS").toCharArray(oledInstString, 6);
  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < 512; i++) {

      tracks[j][i] = 0;
      trackInstruments[j][i] = 0;
    }
    voices[j].SetDelay(0);
    voices[j].ResetEffects();
    voices[j].SetEnvelopeNum(0);
    voices[j].SetVolume(2);
    voices[j].SetOctave(1);
  }
  patternLength = 32 + (32 * val);
};