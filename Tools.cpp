/*

  FM Receiver Project based on Arduino
  Copyright (c) 2017-2018 Michael A. Matveev. All right reserved.
  This firmware is licensed under a GNU GPL v.3 License.

  For more information see: https://www.gnu.org/licenses/gpl-3.0.en.html

*/

#include <Arduino.h>

long __tstamp;
long __tstamp2;

String utf8rus(String source) {
  int i, k;
  String target;
  unsigned char n;
  char m[2] = {'0', '\0'};
  k = source.length();
  i = 0;
  while (i < k) {
    n = source[i];
    i++;
    if (n >= 0xBF) {
      switch (n) {
        case 0xD0: {
            n = source[i];
            i++;
            if (n == 0x81) {
              n = 0xA8;
              break;
            }
            if (n >= 0x90 && n <= 0xBF) n = n + 0x2F;
            break;
          }
        case 0xD1: {
            n = source[i];
            i++;
            if (n == 0x91) {
              n = 0xB7;
              break;
            }
            if (n >= 0x80 && n <= 0x8F) n = n + 0x6F;
            break;
          }
      }
    }
    m[0] = n;
    target = target + String(m);
  }
  return target;
};

boolean checkMillis(int m) {
  if (millis() - __tstamp > m) {
    __tstamp = millis();
    return true;
  } else {
    return false;
  }
};

boolean checkMillis2(int m) {
  if (millis() - __tstamp2 > m) {
    __tstamp2 = millis();
    return true;
  } else {
    return false;
  }
};
