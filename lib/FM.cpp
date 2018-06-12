#include <Arduino.h>
#include <FM.h>
#include <Wire.h>

void FM::writeReg(byte reg, unsigned int valor) {
  Wire.beginTransmission(0x11);
  Wire.write(reg);
  Wire.write(valor >> 8);
  Wire.write(valor & 0xFF);
  Wire.endTransmission();
}

void FM::init() {
  Wire.begin();
  writeReg(0x02, 0xC00d);  // Soft reset, enable RDS
  writeReg(0x05, 0x84d8);
  writeReg(5, 0x84D0 | volume);
  setFrequency(freq);
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
  freq = state[0] & 0x03ff;
  strength = state[1] >> 10;
  stereo = (state[0] & 0x0400) != 0;
  char buffer[30];
  sprintf(buffer, "%04d ", 870 + freq);
  freqText = "";
  for (int i = 0; i < 5; i++) {
    if (i == 3) {
      freqText += ".";
    }
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
    writeReg(0x02, 0xC30d);
  else
    writeReg(0x02, 0xC10d);
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

void FM::setFrequency(int channel) {
  byte nH, nL;
  nH = channel >> 2;
  nL = ((channel & 3) << 6 | 0x10);
  Wire.beginTransmission(0x11);
  Wire.write(0x03);
  Wire.write(nH);
  Wire.write(nL);
  Wire.endTransmission();
}

void FM::lowerFrequency() {
  freq--;
  if (freq < 0) {
    freq = 205;
  }
  setFrequency(freq);
}

void FM::higherFrequency() {
  freq++;
  if (freq > 205) {
    freq = 0;
  }
  setFrequency(freq);
}

void FM::lowerVolume() {
  volume = volume - 1;
  if (volume < 0) volume = 0;
  writeReg(5, 0x84D0 | volume);
}

void FM::higherVolume() {
  volume = volume + 1;
  if (volume > 15) volume = 15;
  writeReg(5, 0x84D0 | volume);
}