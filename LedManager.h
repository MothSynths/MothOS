#ifndef LedManager_h
#define LedManager_h
class LedManager {
public:
  int outPinA;
  int outPinB;
  int outPinC;
  int outPinD;
  float timeLit;
  float lastMillis;
  int command;
  bool flipBlink;
  
  LedManager(int pinA,int pinB, int pinC, int pinD);
  void UpdateLed();
  void SetCommand(char command);
  void SetLit(float time,int col);
  void SetPattern(bool patternPlay,int pattern);
private:
  int pattern;
  bool patternPlay;
};
#endif