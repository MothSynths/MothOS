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

  beatTime += delta * bps;
  noteTime += delta * bps;
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
      int noteDelay = tracks[i][trackIndexBehind];
      if (note > 0) {
        heldNotes[i] = note;
        heldInsturments[i] = currentVoice;
        voices[i].SetNote(note - 1, false, optOctave, optInstrument);
        //} else if (voices[i].arpNum > 0) {
        // voices[i].SetNote(heldNotes[i] - 1, false, -1, heldInsturments[i] - 1);
      } else if (voices[i].delay > 0) {
        int delayIndex = trackIndexBehind - voices[i].delay * 2;
        if (delayIndex < patternLength * (currentPattern + 0)) {
          delayIndex = patternLength * (currentPattern + 1) + delayIndex;
        }
        int delayNote = tracks[i][delayIndex];
        if (delayNote > 0) {
          optInstrument = trackInstruments[i][delayIndex];
          voices[i].SetNote(delayNote - 1, true, -1, optInstrument);
        }
      }
    }
  }

  sample = 0;
  sample2 = 0;
  for (int i = 0; i < 4; i++) {
    sample += voices[i].UpdateVoice() / (4 + masterVolume * 5);
  }

  return 0;
}

void Tracker::SetCommand(char command, int val) {
  switch (command) {
    case 'T':
      SetTrackNum(val);
      break;
    case 'B':
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
      SetOctave(val);
      break;
    case 'L':
      SetEnvelopeLength(val);
      break;
    case 'E':
      SetEnvelopeNum(val);
      break;
    case 'V':
      SetVolume(val);
      break;
    case 'D':
      SetDelay(val);
      break;
    case 'A':
      SetArp(val);
      break;
    case '^':
      ClearTrackNum(val);
      break;
    case '$':
      SetPatternNum(val);
      break;
    case '#':
      ClearPatternNum(val);
      break;
    case 'X':
      ClearAll(val);
      break;
    case 'P':
      TogglePlayStop();
      break;
    case 'I':
      currentVoice = val;
      break;
    case 'H':
      masterVolume++;
      if (masterVolume > 1)
        masterVolume = 0;
      break;
    case 'C':
      allPatternPlay = !allPatternPlay;
      break;
    case '*':
      if (val == 0)
        CopyPattern();
      if (val == 1) {
        PastePattern();
      }
      if (val == 2) {
        PastePatternAll();
      }
      break;
  }
}

void Tracker::SetArp(int val) {
  voices[selectedTrack].SetArpNum(val);
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
  isPlaying =true;
  pressedOnce = false;
  allPatternPlay = false;

  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < 512; i++) {

      tracks[j][i] = 0;
      trackInstruments[j][i] = 0;
    }
    voices[j].SetDelay(0);
    voices[j].SetArpNum(0);
    voices[j].SetEnvelopeNum(0);
    voices[j].SetVolume(2);
    voices[j].SetOctave(1);
  }
  patternLength = 32 + (32 * val);
};