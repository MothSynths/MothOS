#ifndef ScreenManager_h
#define ScreenManager_h
#include "Tracker.h"

class ScreenManager {
public:
  ScreenManager();
  void Update(Tracker &tracker, U8G2_SSD1306_128X64_NONAME_1_HW_I2C &screen, char ledCommandOLED, int volumeBars[4], String noteChars[12]);
  void OnInput(int input, Tracker &tracker);
private:
  int lastNote;
  int cursorX;
  int cursorY;
  int cursorMode;
  int lastNoteBeat;
  int noteBeatTime;
  void UpdateMainScreen(Tracker &tracker, U8G2_SSD1306_128X64_NONAME_1_HW_I2C &screen, char ledCommandOLED, int volumeBars[4], String noteChars[12]);
  bool UpdateInstructionsScreen(Tracker &tracker, U8G2_SSD1306_128X64_NONAME_1_HW_I2C &screen, char ledCommandOLED, int volumeBars[4], String noteChars[12]);
};

#endif