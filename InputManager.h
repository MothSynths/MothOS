#include <Keypad.h>
#ifndef InputManager_h
#define InputManager_h
class InputManager {
public:
  char note;
  char trackCommand;
  int trackCommandArgument;
  char ledCommand;
  InputManager();
  void UpdateInput(char rawInput);
  void ProcessFunctionClick(int input);
  void ProcessClick(int input);
  void EndFrame();

private:
  bool fnc[4];
  void ClearFunctions();
};
#endif