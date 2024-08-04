#include "Arduino.h"
#include "LedManager.h"

LedManager::LedManager(int pinA, int pinB, int pinC, int pinD) {

  outPinA = pinA;
  outPinB = pinB;
  outPinC = pinC;
  outPinD = pinD;
  pinMode(outPinA, OUTPUT);
  pinMode(outPinB, OUTPUT);
  pinMode(outPinC, OUTPUT);
  pinMode(outPinD, OUTPUT);
  lastMillis = millis();
  command = 'T';
}

void LedManager::SetPattern(bool pPlay, int p) {
  patternPlay = pPlay;
  pattern = p;
}

void LedManager::UpdateLed() {
  if (command == 'T') {
    float timeDelta = millis() - lastMillis;
    lastMillis = millis();

    if (timeLit > 0) {
      timeLit -= timeDelta;
      if (timeLit <= 0) {
        digitalWrite(outPinA, LOW);
        digitalWrite(outPinB, LOW);
        digitalWrite(outPinC, LOW);
        digitalWrite(outPinD, LOW);
      }
    }
  }
  if (patternPlay) {
    flipBlink = !flipBlink;
    if (flipBlink) {
      switch (pattern) {
        case 0:
          digitalWrite(outPinA, HIGH);
          break;
        case 1:
          digitalWrite(outPinB, HIGH);
          break;
        case 2:
          digitalWrite(outPinC, HIGH);
          break;
        case 3:
          digitalWrite(outPinD, HIGH);
          break;
      }
    } else {
      switch (pattern) {
        case 0:
          digitalWrite(outPinA, LOW);
          break;
        case 1:
          digitalWrite(outPinB, LOW);
          break;
        case 2:
          digitalWrite(outPinC, LOW);
          break;
        case 3:
          digitalWrite(outPinD, LOW);
          break;
      }
    }
  }
}

void LedManager::SetCommand(char com) {
  command = com;
  digitalWrite(outPinA, LOW);
  digitalWrite(outPinB, LOW);
  digitalWrite(outPinC, LOW);
  digitalWrite(outPinD, LOW);
  switch (command) {
    case 'A':
      digitalWrite(outPinA, HIGH);
      break;
    case 'B':
      digitalWrite(outPinB, HIGH);
      break;
    case 'C':
      digitalWrite(outPinC, HIGH);
      break;
    case 'D':
      digitalWrite(outPinD, HIGH);
      break;
    case 'T':
      break;
  }
}

void LedManager::SetLit(float time, int col) {

  if (command != 'T') {
    return;
  }

  digitalWrite(outPinA, LOW);
  digitalWrite(outPinB, LOW);
  digitalWrite(outPinC, LOW);
  digitalWrite(outPinD, LOW);
  if (col == 0) {
    digitalWrite(outPinA, HIGH);
  } else if (col == 1) {
    digitalWrite(outPinB, HIGH);
  } else if (col == 2) {
    digitalWrite(outPinC, HIGH);
  } else if (col == 3) {
    digitalWrite(outPinD, HIGH);
  }
  timeLit = time;
}