#ifndef FSManager_h
#define FSManager_h
#include "Tracker.h"
class FSManager {
public:
  FSManager(Tracker &tracker);
  void init(Tracker &tracker);
  int save(Tracker &tracker);
  int load(Tracker &tracker);
private:
};
#endif