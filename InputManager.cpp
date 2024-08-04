#include "Arduino.h"
#include "InputManager.h"

InputManager::InputManager() {
}

void InputManager::UpdateInput(char rawInput) {
  note = ' ';
  trackCommand = ' ';
  ledCommand = ' ';

   if (rawInput) {
    switch (rawInput) {
      case 'A':
        ProcessClick(0);
        break;
      case 'B':
        ProcessClick(1);
        break;
      case 'C':
        ProcessClick(2);
        break;
      case 'D':
        ProcessClick(3);
        break;

      case 'E':
        ProcessClick(4);
        break;
      case 'F':
        ProcessClick(5);
        break;
      case 'G':
        ProcessClick(6);
        break;
      case 'H':
        ProcessClick(7);
        break;

      case 'I':
        ProcessClick(8);
        break;
      case 'J':
        ProcessClick(9);
        break;
      case 'K':
        ProcessClick(10);
        break;
      case 'L':
        ProcessClick(11);
        break;

      case 'M':
        ProcessFunctionClick(0);
        break;
      case 'N':
        ProcessFunctionClick(1);
        break;
      case 'O':
        ProcessFunctionClick(2);
        break;
      case 'P':
        ProcessFunctionClick(3);
        break;
    }
  }
}

void InputManager::ClearFunctions() {
  for (int i = 0; i < 4; i++)
    fnc[i] = false;
}

void InputManager::ProcessClick(int input) {
  //if another function is on, exectute command
  for (int i = 0; i < 4; i++) {
    if (fnc[i]) {

      switch (i) {
        case 0:
          trackCommand = 'I';  //instrument selection
          trackCommandArgument = input;
          break;
        case 1:
          if (input >= 0 && input < 4) {
            trackCommand = 'A';  //arp
            trackCommandArgument = input;
          }
          if (input >= 4 && input < 8) {
            trackCommand = 'D';  //delay
            trackCommandArgument = input - 4;
          }
          if (input >= 8 && input < 12) {
            trackCommand = 'E';  //envelope
            trackCommandArgument = input - 8;
          }
          break;
        case 2:
          if (input >= 0 && input < 4) {
            trackCommand = '#';  //clear pattern
            trackCommandArgument = input;
          }
          if (input >= 4 && input < 8) {
            trackCommand = '$';  //select pattern
            trackCommandArgument = input - 4;
          }
          if (input >= 8 && input < 12) {
            trackCommand = '^';  //clear track
            trackCommandArgument = input - 8;
          }
          break;
        case 3:
          if (input >= 0 && input < 4) {
            if (input < 2) {
              trackCommand = 'X';  //pattern length
              trackCommandArgument = input;
            } else if (input < 3) {
              trackCommand = 'H';  // Line / Headphone
              trackCommandArgument = 0;
            } else {
               trackCommand = 'C';
            }
          }
          if (input >= 4 && input < 8) {

            trackCommand = '*';  //copy paste options 0 = copy track 1=paste track 2 = copy pattern 3 = paste pattern
            trackCommandArgument = input - 4;
          }
          if (input >= 8 && input < 12) {
            trackCommand = 'B';  //clear track
            trackCommandArgument = input - 8;
          }
          break;
      }
      //don't continue, execute command
      ledCommand = 'T';
      ClearFunctions();
      return;
    }
  }

  trackCommand = 'N';
  trackCommandArgument = input;
}

void InputManager::ProcessFunctionClick(int input) {
  //if another function is on, exectute command
  for (int i = 0; i < 4; i++) {
    if (fnc[i]) {

      switch (i) {
        case 0:
          trackCommand = 'O';  //instrument octave
          trackCommandArgument = input;
          break;
        case 1:
          trackCommand = 'V';  //track volume
          trackCommandArgument = input;
          break;
        case 2:
          trackCommand = 'T';  //selct track
          trackCommandArgument = input;
          break;
        case 3:
          if (input == 3) {
            trackCommand = 'P';  //stop play
            trackCommandArgument = 0;
          } else {
            trackCommand = 'L';  //note length
            trackCommandArgument = input;
          }
          break;
      }
      //don't continue, execute command
      ledCommand = 'T';
      ClearFunctions();
      return;
    }
  }

  //enable function
  if (fnc[input] == false) {
    fnc[input] = true;
    switch (input) {
      case 0:
        ledCommand = 'A';
        break;
      case 1:
        ledCommand = 'B';
        break;
      case 2:
        ledCommand = 'C';
        break;
      case 3:
        ledCommand = 'D';
        break;
    }
  }
}

void InputManager::EndFrame() {
  ledCommand = ' ';
  trackCommand = ' ';
  trackCommandArgument = 0;
}
