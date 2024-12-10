#include "Arduino.h"
#include "Tracker.h"
#include "FSManager.h"
#include <LittleFS.h>

FSManager::FSManager(Tracker &tracker) {
}

void FSManager::init(Tracker &tracker) {
  if (!LittleFS.begin()) {
    if (LittleFS.format()) {
      String("FS Format!").toCharArray(tracker.fsState, 11);
      if (LittleFS.begin()) {
        String("FS Fmt Rdy!").toCharArray(tracker.fsState, 11);
      }
    } else {
      String("FS Failed").toCharArray(tracker.fsState, 11);
      return;
    }
  } else {
    String("FS Ready!").toCharArray(tracker.fsState, 11);
  }
}

int FSManager::save(Tracker &tracker) {
  File file = LittleFS.open("/track1", "w");
  if (!file) {
    return 0;
  }

  uint8_t arr[512 * 4];
  for (int i = 0; i < 512; i++) {
    arr[i * 4] = (uint8_t)tracker.tracks[0][i];
    arr[i * 4 + 1] = (uint8_t)tracker.tracks[1][i];
    arr[i * 4 + 2] = (uint8_t)tracker.tracks[2][i];
    arr[i * 4 + 3] = (uint8_t)tracker.tracks[3][i];
  }
  int written = file.write((uint8_t *)arr, 512 * 4 * sizeof(uint8_t));
  file.close();

  file = LittleFS.open("/track1oct", "w");
  if (!file) {
    return 0;
  }

  for (int i = 0; i < 512; i++) {
    arr[i * 4] = (uint8_t)tracker.trackOctaves[0][i];
    arr[i * 4 + 1] = (uint8_t)tracker.trackOctaves[1][i];
    arr[i * 4 + 2] = (uint8_t)tracker.trackOctaves[2][i];
    arr[i * 4 + 3] = (uint8_t)tracker.trackOctaves[3][i];
  }

  written = file.write((uint8_t *)arr, 512 * 4 * sizeof(uint8_t));
  file.close();

  file = LittleFS.open("/track1inst", "w");
  if (!file) {
    return 0;
  }

  for (int i = 0; i < 512; i++) {
    arr[i * 4] = (uint8_t)tracker.trackInstruments[0][i];
    arr[i * 4 + 1] = (uint8_t)tracker.trackInstruments[1][i];
    arr[i * 4 + 2] = (uint8_t)tracker.trackInstruments[2][i];
    arr[i * 4 + 3] = (uint8_t)tracker.trackInstruments[3][i];
  }

  written = file.write((uint8_t *)arr, 512 * 4 * sizeof(uint8_t));
  file.close();

  uint8_t arr2[12 * 4];
  file = LittleFS.open("/prf", "w");
  if (!file) {
    return 0;
  }

  for (int i = 0; i < 4; i++) {
    arr2[i * 12 + 0] = (uint8_t)tracker.voices[i].volume;
    arr2[i * 12 + 1] = (uint8_t)tracker.voices[i].envelopeNum;
    arr2[i * 12 + 2] = (uint8_t)tracker.voices[i].envelopeLength;
    arr2[i * 12 + 3] = (uint8_t)tracker.voices[i].phaserMult;
    arr2[i * 12 + 4] = (uint8_t)tracker.voices[i].lowPassMult;
    arr2[i * 12 + 5] = (uint8_t)tracker.voices[i].reverbMult;
    arr2[i * 12 + 6] = (uint8_t)tracker.voices[i].chordMult;
    arr2[i * 12 + 7] = (uint8_t)tracker.voices[i].pitchMult;
    arr2[i * 12 + 8] = (uint8_t)tracker.voices[i].delayMult;
    arr2[i * 12 + 9] = (uint8_t)tracker.voices[i].whooshMult;
    arr2[i * 12 + 10] = (uint8_t)tracker.bmpChoice;
    arr2[i * 12 + 11] = (uint8_t)tracker.voices[i].samplerMode;
  }
  written = file.write((uint8_t *)arr2, 12 * 4 * sizeof(uint8_t));
  file.close();

  return written;
}

int FSManager::load(Tracker &tracker) {
  File file = LittleFS.open("/track1", "r");
  if (!file) {
    return 0;
  }

  // Read the binary data into the array
  uint8_t arr[512 * 4];
  int written = file.read((uint8_t *)arr, 512 * 4 * sizeof(uint8_t));
  for (int i = 0; i < 512; i++) {
    tracker.tracks[0][i] = arr[i * 4];
    tracker.tracks[1][i] = arr[i * 4 + 1];
    tracker.tracks[2][i] = arr[i * 4 + 2];
    tracker.tracks[3][i] = arr[i * 4 + 3];
  }
  file.close();

  file = LittleFS.open("/track1oct", "r");
  if (!file) {
    return 0;
  }

  // Read the binary data into the array

  written = file.read((uint8_t *)arr, 512 * 4 * sizeof(uint8_t));
  for (int i = 0; i < 512; i++) {
    tracker.trackOctaves[0][i] = arr[i * 4];
    tracker.trackOctaves[1][i] = arr[i * 4 + 1];
    tracker.trackOctaves[2][i] = arr[i * 4 + 2];
    tracker.trackOctaves[3][i] = arr[i * 4 + 3];
  }
  file.close();

  file = LittleFS.open("/track1inst", "r");
  if (!file) {
    return 0;
  }

  // Read the binary data into the array

  written = file.read((uint8_t *)arr, 512 * 4 * sizeof(uint8_t));
  for (int i = 0; i < 512; i++) {
    tracker.trackInstruments[0][i] = arr[i * 4];
    tracker.trackInstruments[1][i] = arr[i * 4 + 1];
    tracker.trackInstruments[2][i] = arr[i * 4 + 2];
    tracker.trackInstruments[3][i] = arr[i * 4 + 3];
  }
  file.close();


  uint8_t arr2[12 * 4];
  file = LittleFS.open("/prf", "r");
  if (!file) {
    return 0;
  }

  written = file.read((uint8_t *)arr2, 12 * 4 * sizeof(uint8_t));
  for (int i = 0; i < 4; i++) {
    tracker.voices[i].volume = (uint8_t)arr2[i * 12 + 0];
    tracker.voices[i].envelopeNum = (uint8_t)arr2[i * 12 + 1];
    tracker.voices[i].envelopeLength = (uint8_t)arr2[i * 12 + 2];
    tracker.voices[i].phaserMult = (uint8_t)arr2[i * 12 + 3];
    tracker.voices[i].lowPassMult = (uint8_t)arr2[i * 12 + 4];
    tracker.voices[i].reverbMult = (uint8_t)arr2[i * 12 + 5];
    tracker.voices[i].chordMult = (uint8_t)arr2[i * 12 + 6];
    tracker.voices[i].pitchMult = (uint8_t)arr2[i * 12 + 7];
    tracker.voices[i].delayMult = (uint8_t)arr2[i * 12 + 8];
    tracker.voices[i].whooshMult = (uint8_t)arr2[i * 12 + 9];
    tracker.voices[i].samplerMode = (uint8_t)arr2[i * 12 + 11];
    tracker.SetBPM((uint8_t)arr2[i * 12 + 10]);
  
  }

  file.close();
  return 44;
}