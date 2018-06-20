/*

  FM Receiver Project based on Arduino
  Copyright (c) 2017-2018 Michael A. Matveev. All right reserved.
  This firmware is licensed under a GNU GPL v.3 License.

  For more information see: https://www.gnu.org/licenses/gpl-3.0.en.html

*/

#include <Arduino.h>
#include "config.h"

const int RDA5807M_BandLowerLimits[5] PROGMEM = {8700, 7600, 7600, 6500, 5000};
const int RDA5807M_BandHigherLimits[5] PROGMEM = {10800, 9100, 10800, 7600,
                                                  6500
                                                 };
const int RDA5807M_ChannelSpacings[4] PROGMEM = {100, 200, 50, 25};

#define RDA5807M_I2C_ADDR_RANDOM (0x22 >> 1)
#define RDA5807M_REG_CONFIG 0x02
#define RDA5807M_FLG_DHIZ 0x8000
#define RDA5807M_FLG_DMUTE 0x4000
#define RDA5807M_FLG_BASS 0x1000
#define RDA5807M_FLG_SEEKUP 0x0200
#define RDA5807M_FLG_RDS word(0x0008)
#define RDA5807M_FLG_NEW word(0x0004)
#define RDA5807M_REG_TUNING 0x03
#define RDA5807M_BAND_MASK word(0x000C)
#define RDA5807M_FLG_ENABLE word(0x0001)
#define RDA5807M_SPACE_MASK word(0x0003)
#define RDA5807M_REG_BLEND 0x07
#define RDA5807M_FLG_EASTBAND65M 0x0200
#define RDA5807M_BAND_SHIFT 2
#define RDA5807M_REG_STATUS 0x0A
#define RDA5807M_READCHAN_MASK 0x03FF
#define RDA5807M_CHAN_MASK 0xFFC0
#define RDA5807M_FLG_TUNE word(0x0010)
#define RDA5807M_CHAN_SHIFT 6

class FM {
  private:
    unsigned int state[6];
    char segRDS[8];
    char segRDS1[64];
    char indexRDS1;
    word FM::getRegister(byte reg);
    void setRegister(byte reg, const word value);
    void updateRegister(byte reg, word mask, word value);
    void writeRegister(byte reg, unsigned int value);
    word getBandAndSpacing();
    int noRDSCounter = 0;
    word freq = DEFAULT_FREQ;
    void setVolume();
    void setBand(word band);

  public:
    int strength = 0;
    bool stereo = false;
    int volume = DEFAULT_VOL;
    String freqText = "";
    String station = "";
    String radiotext = "";
    bool isTuning = false;
    void init();
    void getRDS();
    int readState();
    void autoTune(byte direc);
    void lowerFrequency();
    void higherFrequency();
    void lowerVolume();
    void higherVolume();
    void moreBass();
    void lessBass();
    word getFrequency(void);
    bool setFrequency(word frequency);
    void setBandByIndex(int idx);
};
