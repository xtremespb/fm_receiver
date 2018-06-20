/*

  FM Receiver Project based on Arduino
  Copyright (c) 2017-2018 Michael A. Matveev. All right reserved.
  This firmware is licensed under a GNU GPL v.3 License.

  For more information see: https://www.gnu.org/licenses/gpl-3.0.en.html

*/

#include "FM.h"
#include <Arduino.h>
#include <Wire.h>

word FM::getBandAndSpacing() {
  byte band = getRegister(RDA5807M_REG_TUNING) &
              (RDA5807M_BAND_MASK | RDA5807M_SPACE_MASK);
  // Separate channel spacing
  const byte space = band & RDA5807M_SPACE_MASK;
  if (band & RDA5807M_BAND_MASK == BAND_EAST &&
      !(getRegister(RDA5807M_REG_BLEND) & RDA5807M_FLG_EASTBAND65M))
    // Lower band limit is 50MHz
    band = (band >> RDA5807M_BAND_SHIFT) + 1;
  else
    band >>= RDA5807M_BAND_SHIFT;
  return word(space, band);
};

word FM::getFrequency(void) {
  const word spaceandband = getBandAndSpacing();
  return pgm_read_word(&RDA5807M_BandLowerLimits[lowByte(spaceandband)]) +
         (getRegister(RDA5807M_REG_STATUS) & RDA5807M_READCHAN_MASK) *
         pgm_read_byte(&RDA5807M_ChannelSpacings[highByte(spaceandband)]) /
         10;
};

bool FM::setFrequency(word frequency) {
  const word spaceandband = getBandAndSpacing();
  const word origin =
    pgm_read_word(&RDA5807M_BandLowerLimits[lowByte(spaceandband)]);
  // Check that specified frequency falls within our current band limits
  if (frequency < origin ||
      frequency >
      pgm_read_word(&RDA5807M_BandHigherLimits[lowByte(spaceandband)]))
    return false;
  // Adjust start offset
  frequency -= origin;
  const byte spacing =
    pgm_read_byte(&RDA5807M_ChannelSpacings[highByte(spaceandband)]);
  // Check that the given frequency can be tuned given current channel spacing
  if (frequency * 10 % spacing) return false;
  // Attempt to tune to the requested frequency
  updateRegister(
    RDA5807M_REG_TUNING, RDA5807M_CHAN_MASK | RDA5807M_FLG_TUNE,
    ((frequency * 10 / spacing) << RDA5807M_CHAN_SHIFT) | RDA5807M_FLG_TUNE);
  return true;
};

word FM::getRegister(byte reg) {
  word result;
  Wire.beginTransmission(RDA5807M_I2C_ADDR_RANDOM);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(RDA5807M_I2C_ADDR_RANDOM, 2, true);
  result = (word)Wire.read() << 8;
  result |= Wire.read();
  return result;
};

void FM::setRegister(byte reg, const word value) {
  Wire.beginTransmission(RDA5807M_I2C_ADDR_RANDOM);
  Wire.write(reg);
  Wire.write(highByte(value));
  Wire.write(lowByte(value));
  Wire.endTransmission(true);
};

void FM::updateRegister(byte reg, word mask, word value) {
  setRegister(reg, getRegister(reg) & ~mask | value);
};

void FM::writeRegister(byte reg, unsigned int value) {
  Wire.beginTransmission(0x11);
  Wire.write(reg);
  Wire.write(value >> 8);
  Wire.write(value & 0xFF);
  Wire.endTransmission();
}

void FM::setVolume() {
  writeRegister(5, 0x84D0 | volume);
}

void FM::setBand(word band) {
  updateRegister(RDA5807M_REG_TUNING, RDA5807M_BAND_MASK, band);
}

void FM::setBandByIndex(int idx) {
  switch (idx) {
    case 0:
      setBand(BAND_WEST);
      break;
    case 1:
      setBand(BAND_JAPAN);
      break;
    case 2:
      setBand(BAND_WORLD);
      break;
    case 3:
      setBand(BAND_EAST);
      break;
  }
  setFrequency(pgm_read_word(&RDA5807M_BandLowerLimits[idx]));
}

void FM::init() {
  Wire.begin();
  setRegister(RDA5807M_REG_CONFIG, RDA5807M_FLG_DHIZ | RDA5807M_FLG_DMUTE |
              RDA5807M_FLG_BASS | RDA5807M_FLG_SEEKUP |
              RDA5807M_FLG_RDS | RDA5807M_FLG_NEW |
              RDA5807M_FLG_ENABLE);
  setBand(DEFAULT_BAND);
  setVolume();
  setFrequency(freq);
}

void FM::moreBass() {
  writeRegister(0x02, 0xD00D);
}

void FM::lessBass() {
  writeRegister(0x02, 0xC00D);
}

void FM::getRDS() {
  Wire.beginTransmission(0x11);
  Wire.write(0x0C);
  Wire.endTransmission(0);
  Wire.requestFrom(0x11, 8, 1);
  unsigned int RDS[4];
  char grp, ver;
  for (int i = 0; i < 4; i++) {
    RDS[i] = 256 * Wire.read() + Wire.read();
  }
  Wire.endTransmission();
  int i;
  grp = (RDS[1] >> 12) & 0xf;
  if (RDS[1] & 0x0800) {
    ver = 1;
  } else {
    switch (grp) {
      case 0:
        i = (RDS[1] & 3) << 1;
        segRDS[i] = (RDS[3] >> 8);
        segRDS[i + 1] = (RDS[3] & 0xFF);
        station = "";
        for (i = 0; i < 8; i++) {
          if (segRDS[i] > 31 && segRDS[i] < 127)
            station += segRDS[i];
          else
            station += "";
        }
        station = station.substring(0, 9);
        break;
      case 2:
        i = (RDS[1] & 15) << 2;
        segRDS1[i] = (RDS[2] >> 8);
        segRDS1[i + 1] = (RDS[2] & 0xFF);
        segRDS1[i + 2] = (RDS[3] >> 8);
        segRDS1[i + 3] = (RDS[3] & 0xFF);
        radiotext = "";
        for (i = 0; i < 42; i++) {
          radiotext += segRDS1[i];
        }
        break;
      default:;
    }
  }
}

int FM::readState() {
  Wire.requestFrom(0x10, 12);
  for (int i = 0; i < 6; i++) {
    state[i] = 256 * Wire.read() + Wire.read();
  }
  Wire.endTransmission();
  freq = getFrequency();
  strength = state[1] >> 10;
  stereo = (state[0] & 0x0400) != 0;
  char buffer[30];
  sprintf(buffer, "%04d ", freq);
  freqText = "";
  for (int i = 0; i < 5; i++) {
    freqText += buffer[i];
  }
  if ((state[0] & 0x8000) != 0) {
    noRDSCounter = 0;
    getRDS();
  } else {
    noRDSCounter++;
    if (noRDSCounter > 100) {
      station = "RDS";
      radiotext = "";
      noRDSCounter = 0;
    }
  }
}

void FM::autoTune(byte direc) {
  byte i;
  isTuning = true;
  if (!direc)
    writeRegister(0x02, 0xC30d);
  else
    writeRegister(0x02, 0xC10d);
  for (i = 0; i < 10; i++) {
    delay(200);
    readState();
    if (state[0] & 0x4000) {
      freq = state[0] & 0x03ff;
      isTuning = false;
      break;
    }
  }
}

void FM::lowerFrequency() {
  const word spaceandband = getBandAndSpacing();
  freq = freq - 10;
  if (freq < pgm_read_word(&RDA5807M_BandLowerLimits[lowByte(spaceandband)])) {
    freq = pgm_read_word(&RDA5807M_BandHigherLimits[lowByte(spaceandband)]);
  }
  setFrequency(freq);
}

void FM::higherFrequency() {
  const word spaceandband = getBandAndSpacing();
  freq = freq + 10;
  if (freq > pgm_read_word(&RDA5807M_BandHigherLimits[lowByte(spaceandband)])) {
    freq = pgm_read_word(&RDA5807M_BandLowerLimits[lowByte(spaceandband)]);
  }
  setFrequency(freq);
}

void FM::lowerVolume() {
  volume = volume - 1;
  if (volume < 0) volume = 0;
  setVolume();
}

void FM::higherVolume() {
  volume = volume + 1;
  if (volume > 15) volume = 15;
  setVolume();
}
