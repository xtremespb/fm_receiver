#include <Arduino.h>

class FM {
 private:
  unsigned int state[6];
  char segRDS[8];
  char segRDS1[64];
  char indexRDS1;
  void writeReg(byte reg, unsigned int valor);
  int noRDSCounter = 0;
  int freq = 193;  

 public:  
  int strength = 0;
  bool stereo = false;
  int volume = 5;
  String freqText = "";
  String station = "";
  String radiotext = "";
  bool isTuning = false;
  void init();
  void getRDS();
  int readState();
  void autoTune(byte direc);
  void setFrequency(int channel);
  void lowerFrequency();
  void higherFrequency();
  void lowerVolume();
  void higherVolume();
};