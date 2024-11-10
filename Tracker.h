#include "Voice.h"
#ifndef Tracker_h
#define Tracker_h
#include <U8g2lib.h>

class Tracker {
public:
  int lastNoteTrackIndex;
  int preKEnvNumber;
  int hintTime;
  char hint[15];
  int masterVolume;
  bool pressedOnce;
  int sample;
  int sample2;
  bool isPlaying;
  float tempoBlink;
  int selectedTrack;
  bool allPatternPlay;
  int currentPattern;
  int patternLength;
  int trackIndex;
  Voice voices[4];
  Tracker();
  int UpdateTracker();
  void SetCommand(char command, int val);
  int lastSamples[4];
  char oledInstString[8];
  char oledOctString[6];
  int currentVoice = 0;
  int tracks[4][512];
  int trackOctaves[4][512];
  bool solo;

private:
  int lastNoteTrack;
  float bpm;
  float bps;
  float beatTime;
  float noteTime;
  int barCount;
  float lastMillis;
  float soundVelocity;
  void SoloTrack(bool repeat);
  int bpms[4];
  int trackInstruments[4][512];
  int patternCopy[4][128];
  int patternCopyOctaves[4][128];
  int patternCopyInstruments[4][128];
  void BuildOLEDHintString(String string);
  void SetNote(int val, int track);
  void SetEffect(int val);
  void SetBPM(int val);
  void SetDelay(int val);
  void SetEnvelopeNum(int val);
  void SetVoice(int val);
  void SetEnvelopeLength(int val);
  void SetOctave(int val);
  void SetVolume(int val);
  void SetTrackNum(int val);
  void ClearTrackNum(int val);
  void SetPatternNum(int val);
  void ClearPatternNum(int val);
  void TogglePlayStop();
  void CopyTrack();
  void PasteTrack();
  void CopyPattern();
  void PastePattern();
  void PastePatternAll();
  void ClearAll(int val);
  int UpdateVoices();
  int heldNotes[4];
  int heldInsturments[4];
};

#endif