#include <Arduino.h>
#include "Tracker.h"
#include "Voice.h"

Tracker::Tracker() {
  patternLength = 32;
  isPlaying = true;
  bpms[0] = 120;
  bpms[1] = 140;
  bpms[2] = 95;
  bpms[3] = 180;
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

  noteTime += dbps;

  if (noteTime > 250) {
    barCount++;
    if (barCount > 3) {
      tempoBlink = 30;
      barCount = 0;
      if (!pressedOnce) {
        SetNote(7, 0);
      }
    }

    for (int i = 0; i < 4; i++) {
      int note = tracks[i][trackIndex];
      int optOctave = trackOctaves[i][trackIndex];
      int optInstrument = trackInstruments[i][trackIndex];

      if (note > 0) {
        heldNotes[i] = note;
        heldInsturments[i] = currentVoice;
        voices[i].SetNote(note - 1, false, optOctave, optInstrument);
      }
    }

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
  }

  sample = 0;

  for (int i = 0; i < 4; i++) {
    voices[i].bps = bps;
    int samp = voices[i].UpdateVoice() / (2 + masterVolume * 5);
    sample += samp;
    lastSamples[i] = samp;
  }

  return 0;
}

void Tracker::BuildOLEDHintString(String string) {
  hintTime = 120;
  string.toCharArray(hint, 15);
}

void Tracker::SetCommand(char command, int val) {
  switch (command) {
    case 'T':
      SetTrackNum(val);
      BuildOLEDHintString(String("Track: " + String(val + 1)));
      break;
    case 'B':
      SetBPM(val);
      BuildOLEDHintString(String("BPM: " + String(bpms[val])));
      break;
    case 'N':
      if (!pressedOnce) {
        noteTime = 0;
        trackIndex = 0;
        lastMillis = millis();
        barCount = 0;
        tempoBlink = 30;
      }
      pressedOnce = true;
      SetNote(val, selectedTrack);
      break;
    case 'O':
      SetOctave(val);
      BuildOLEDHintString(String("Octave: " + String(val)));
      break;
    case 'L':
      SetEnvelopeLength(val);
      BuildOLEDHintString(String("Note Len: " + String(val + 1)));
      break;
    case 'E':
      SetEnvelopeNum(val);
      switch (val) {
        case 0:
          BuildOLEDHintString(String("Fade Out"));
          break;
        case 1:
          BuildOLEDHintString(String("Fade In"));
          break;
        case 2:
          BuildOLEDHintString(String("No Fade"));
          break;
        case 3:
          BuildOLEDHintString(String("Loop"));
          break;
      }
      break;
    case 'V':
      if (val == 3) {
        SoloTrack(false);

      } else {
        SetVolume(val);
        if (val == 2) {
          if (voices[selectedTrack].overdrive) {
            BuildOLEDHintString(String("ODrv On"));
          }else{
            BuildOLEDHintString(String("ODrv Off"));
          }
        } else if (val == 0) {
          if (voices[selectedTrack].mute) {
            BuildOLEDHintString(String("Mute"));
          }else{
            BuildOLEDHintString(String("Unmute"));
          }
        } else {
          BuildOLEDHintString(String("Volume: " + String(voices[selectedTrack].volume)));
        }
      }
      break;
    case 'D':
      SetEffect(val + 4);
      switch (val) {
        case 0:
          BuildOLEDHintString(String("Echo: " + String(voices[selectedTrack].delayMult)));
          break;
        case 1:
          BuildOLEDHintString(String("ArpChord: " + String(voices[selectedTrack].chordMult)));
          break;
        case 2:
          BuildOLEDHintString(String("Whoosh: " + String(voices[selectedTrack].whooshMult)));
          break;
        case 3:
          BuildOLEDHintString(String("Pitchbend: " + String(voices[selectedTrack].pitchMult)));
          break;
      }
      break;
    case 'A':
      SetEffect(val);
      switch (val) {
        case 0:
          BuildOLEDHintString(String("Effects Off"));
          break;
        case 1:
          BuildOLEDHintString(String("Low Pass: " + String(voices[selectedTrack].lowPassMult)));
          break;
        case 2:
          BuildOLEDHintString(String("Retrig: " + String(voices[selectedTrack].reverbMult)));
          break;
        case 3:
          BuildOLEDHintString(String("Wobble: " + String(voices[selectedTrack].phaserMult)));
          break;
      }
      break;
    case '^':
      ClearTrackNum(val);
      BuildOLEDHintString(String("Clr Track: " + String(val + 1)));
      break;
    case '$':
      SetPatternNum(val);
      BuildOLEDHintString(String("Pattern: " + String(val + 1)));
      break;
    case '#':
      ClearPatternNum(val);
      BuildOLEDHintString(String("Clr Pattern: " + String(val + 1)));
      break;
    case 'X':
      ClearAll(val);
      BuildOLEDHintString(String("New Song: " + String((32 * (val + 1)))));
      break;
    case 'P':
      TogglePlayStop();
      if (isPlaying) {
        BuildOLEDHintString(String("Rec On"));
      } else {
        BuildOLEDHintString(String("Rec Off"));
      }
      break;
    case 'I':
      currentVoice = val;
      if (val > 1) {
        BuildOLEDHintString(String("Instrument: " + String(val)));
        String("INS" + String(val)).toCharArray(oledInstString, 8);
      } else if (val == 1) {
        BuildOLEDHintString(String("SFX Bank"));
        String("SFX").toCharArray(oledInstString, 6);
      } else {
        BuildOLEDHintString(String("Drum Bank"));
        String("DRUM").toCharArray(oledInstString, 6);
      }
      break;
    case 'H':
      masterVolume++;
      if (masterVolume > 1)
        masterVolume = 0;

      BuildOLEDHintString(String("Mstr Volume: " + String(2 - masterVolume)));
      break;
    case 'K':

      break;
    case 'C':
      allPatternPlay = !allPatternPlay;
      if (allPatternPlay) {
        BuildOLEDHintString(String("Song Mode"));
      } else {
        BuildOLEDHintString(String("Pattern Mode"));
      }
      break;
    case '*':
      if (val == 0) {
        BuildOLEDHintString(String("Copy Pattern"));
        CopyPattern();
      }
      if (val == 1) {
        BuildOLEDHintString(String("Paste Pattern"));
        PastePattern();
      }
      if (val == 2) {
        BuildOLEDHintString(String("Paste All Patt"));
        PastePatternAll();
      }
      if (val == 3) {
        voices[selectedTrack].samplerMode = !voices[selectedTrack].samplerMode;
        voices[selectedTrack].SetEnvelopeNum(2);
        BuildOLEDHintString(String("Samp Mode: " + String(voices[selectedTrack].samplerMode)));
      }
      break;
  }
}

void Tracker::SoloTrack(bool repeat) {
  if (!repeat) {
    solo = !solo;
  }

  if (solo) {
    for (int i = 0; i < 4; i++) {
      if (i == selectedTrack) {
        voices[i].soloMute = false;
      } else {
        voices[i].soloMute = true;
      }
    }
    BuildOLEDHintString(String("Solo On"));
  } else {
    for (int i = 0; i < 4; i++) {
      voices[i].soloMute = false;
    }
    BuildOLEDHintString(String("Solo Off"));
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
  if (isPlaying && pressedOnce) {
    //one behind trick
    tracks[track][trackIndex] = val + 1;
    trackOctaves[track][trackIndex] = voices[selectedTrack].octave;
    trackInstruments[track][trackIndex] = currentVoice;
    lastNoteTrackIndex = trackIndex % patternLength;
  } else {
    voices[track].SetNote(val, false, -1, currentVoice);
  }
};

void Tracker::SetTrackNum(int val) {
  selectedTrack = val;
  SoloTrack(true);
};

void Tracker::ClearTrackNum(int val) {
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

void Tracker::ClearAll(int val) {
  selectedTrack = 0;
  currentPattern = 0;
  isPlaying = true;
  pressedOnce = false;
  allPatternPlay = false;
  currentVoice = 0;
  String("DRUMS").toCharArray(oledInstString, 6);
  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < 512; i++) {
      tracks[j][i] = 0;
      trackInstruments[j][i] = 0;
    }
    voices[j].SetDelay(0);
    voices[j].ResetEffects();
    voices[j].SetEnvelopeNum(0);
    voices[j].volume = 2;
    voices[j].SetOctave(1);
  }
  patternLength = 32 + (32 * val);
};