/*

  FM Receiver Project based on Arduino
  Copyright (c) 2017-2018 Michael A. Matveev. All right reserved.
  This firmware is licensed under a GNU GPL v.3 License.

  For more information see: https://www.gnu.org/licenses/gpl-3.0.en.html

*/

#include "Graphics.h"
#include <Arduino.h>
#include "LCD5110_Graph.h"
#include "Tools.h"
#include "about.c"
#include "config.h"
#include "internal.h"
#include "logo.c"

const String Bands[5] PROGMEM = {"US/EUROPE", "JAPAN", "WORLDWIDE",
                                 "EAST EUROPE"
                                };

extern unsigned char MediumNumbers[];
extern unsigned char IconsFont[];
extern unsigned char TinyFont[];
extern unsigned char MedvedFont[];
// CLK, DIN, DC, RST, CE
LCD5110 display(PIN_CLK, PIN_DIN, PIN_DC, PIN_RST, PIN_CE);

void Graphics::showSplash() {
  display.clrScr();
  display.drawBitmap(0, 0, logo, 84, 48);
  display.update();
  delay(1500);
  display.clrScr();
  display.update();
  delay(50);
  display.disableSleep();
}

void Graphics::showTuningBox() {
  display.drawRect(10, 10, 74, 35);
  for (int x = 11; x < 74; x++) {
    for (int y = 11; y < 34; y++) {
      display.clrPixel(x, y);
    }
  }
  display.setFont(MedvedFont);
  display.print(utf8rus("Поиск..."), CENTER, 20);
  display.update();
  delay(50);
  display.disableSleep();
}

void Graphics::hideTuningBox() {
  display.clrScr();
  displayBasics();
  drawMenu();
  old_volume = 0;
}

void Graphics::displayBasics() {
  switch (menu) {
    case 1 ... 4:
      display.setFont(IconsFont);
      display.print(ICON_MONO, 76, 0);
      display.print(ICON_FM, 64, 10);
      display.print(ICON_RDS, 2, 20);
      display.print(ICON_VOL, 2, 28);
      display.update();
      delay(50);
      display.disableSleep();
      break;
  }
}

void Graphics::drawMenuItem(String item) {
  display.setFont(MedvedFont);
  display.print("            ", CENTER, 40);
  display.print(utf8rus(item), CENTER, 40);
  display.update();
  delay(50);
  display.disableSleep();
}

void Graphics::resetOldValues() {
  old_strength = -1;
  old_stereo = false;
  old_station = "";
  old_freqText = "";
  old_volume = 0;
}

void Graphics::toggleBL() {
  currentBL = !currentBL;
  digitalWrite(PIN_BL, currentBL ? HIGH : LOW);
  drawBLMenu(currentBL);
}

void Graphics::drawBLMenu(bool on) {
  display.setFont(MedvedFont);
  if (on) {
    display.drawRoundRect(9, 12, 36, 24);
    display.clrRoundRect(39, 12, 72, 24);
  } else {
    display.clrRoundRect(9, 12, 36, 24);
    display.drawRoundRect(39, 12, 72, 24);
  }
  display.update();
  delay(50);
  display.disableSleep();
}

void Graphics::drawBandSelect(int currentBand) {
  display.setFont(TinyFont);
  for (int i = 0; i < 4; i++) {
    if (currentBand == i) {
      display.invertText(true);
    }
    for (int x = 13; x < 72; x++) {
      for (int y = 2 + (8 * i); y < 9 + (8 * i); y++) {
        if (currentBand == i) {
          display.setPixel(x, y);
        } else {
          display.clrPixel(x, y);
        }
      }
    }
    display.print(Bands[i], 15, 3 + (8 * i));
    if (currentBand == i) {
      display.invertText(false);
    }
  }
  display.update();
  delay(50);
  display.disableSleep();
}

void Graphics::switchBass() {
  bass = !bass;
  drawBLMenu(bass);
  display.update();
}

void Graphics::drawMenu() {
  switch (menu) {
    case MENU_VOLUME:
      display.clrScr();
      drawMenuItem("Громкость");
      resetOldValues();
      displayBasics();
      break;
    case MENU_MANUAL:
      drawMenuItem("Ручн. настр.");
      break;
    case MENU_AUTO:
      drawMenuItem("Автонастр.");
      break;
    case MENU_SIGNAL:
      display.clrScr();
      drawMenuItem("Сигнал");
      signalCurrent = 9;
      strengthAbsSave = 0;
      break;
    case MENU_BL:
      display.clrScr();
      drawMenuItem("Подсветка");
      display.setFont(MedvedFont);
      display.print(utf8rus("Вкл. Выкл."), CENTER, 15);
      drawBLMenu(currentBL);
      break;
    case MENU_BASS:
      display.clrScr();
      drawMenuItem("Бас");
      display.setFont(MedvedFont);
      display.print(utf8rus("Вкл. Выкл."), CENTER, 15);
      drawBLMenu(bass);
      display.update();
      break;
    case MENU_BAND:
      display.clrScr();
      drawMenuItem("Диапазон");
      drawBandSelect(band);
      break;
    case MENU_VISUAL:
      display.clrScr();
      visualCurrent1 = 0;
      visualSave1 = 25;
      drawMenuItem("Визуализация");
      break;
    case MENU_ABOUT:
      display.clrScr();
      display.setFont(TinyFont);
      display.drawBitmap(0, 0, about, 84, 35);
      display.printNumI(HW_REVISION, 53, 26);
      display.print(".", 56, 26);
      display.printNumI(FW_VERSION, 60, 26);
      drawMenuItem("Прошивка");
      break;
  }
}

void Graphics::init() {
  display.InitLCD();
  currentBL = DEFAULT_BL;
};

void Graphics::updateState(int strength, bool stereo, int volume,
                           String freqText, String station) {
  switch (menu) {
    case MENU_VOLUME ... MENU_AUTO:
      bool needUpdate;
      needUpdate = false;
      if (freqText != old_freqText) {
        needUpdate = true;
        display.setFont(MediumNumbers);
        display.print("/////", 0, 0);
        if (freqText.substring(0, 1) == "1") {
          display.print(freqText.substring(0, 3), 6, 0);
        } else {
          display.print(freqText.substring(0, 2), 18, 0);
        }
        display.setPixel(44, 14);
        display.setPixel(45, 14);
        display.setPixel(44, 15);
        display.setPixel(45, 15);
        if (freqText.substring(0, 1) == "1") {
          display.print(freqText.substring(3, 4), 48, 0);
        } else {
          display.print(freqText.substring(2, 3), 48, 0);
        }
        old_freqText = freqText;
      }
      if (strength != old_strength && checkMillis2(300)) {
        display.setFont(IconsFont);
        switch (strength) {
          case 0 ... 2:
            if (old_strength_idx != 1) {
              needUpdate = true;
              old_strength_idx = 1;
            }
            display.print(ICON_SIGNAL_1, 64, 0);
            break;
          case 3 ... 15:
            if (old_strength_idx != 2) {
              needUpdate = true;
              old_strength_idx = 2;
            }
            display.print(ICON_SIGNAL_2, 64, 0);
            break;
          case 16 ... 20:
            if (old_strength_idx != 3) {
              needUpdate = true;
              old_strength_idx = 3;
            }
            display.print(ICON_SIGNAL_3, 64, 0);
            break;
          case 21 ... 35:
            if (old_strength_idx != 4) {
              needUpdate = true;
              old_strength_idx = 4;
            }
            display.print(ICON_SIGNAL_4, 64, 0);
            break;
          case 36 ... 63:
            if (old_strength_idx != 5) {
              needUpdate = true;
              old_strength_idx = 5;
            }
            display.print(ICON_SIGNAL_5, 64, 0);
            break;
        }
        old_strength = strength;
      }
      if (old_stereo != stereo && checkMillis2(300)) {
        needUpdate = true;
        display.setFont(IconsFont);
        if (stereo) {
          display.print(ICON_STEREO, 76, 0);
        } else {
          display.print(ICON_MONO, 76, 0);
        }
        old_stereo = stereo;
      }
      if (old_station != station && checkMillis2(300) && station.length() > 0) {
        display.setFont(TinyFont);
        display.print("          ", 15, 20);
        display.print(station, 15, 20);
        old_station = station;
        needUpdate = true;
      }
      if (old_volume != volume) {
        needUpdate = true;
        display.setFont(IconsFont);
        for (int i = 0; i < volume; i++) {
          display.print(ICON_VOL_PART, 15 + (i * 4), 28);
        }
        for (int i = volume; i < 15; i++) {
          display.print(ICON_BLANK, 15 + (i * 4), 28);
        }
        old_volume = volume;
      }
      if (needUpdate) {
        display.update();
        delay(50);
        display.disableSleep();
      }
      break;
    case MENU_SIGNAL:
      if (checkMillis2(100)) {
        int strengthAbs = 35 - (strength / 1.8);
        signalCurrent++;
        if (signalCurrent > 83) {
          for (int x = 9; x < 84; x++) {
            for (int y = 0; y < 36; y++) {
              display.clrPixel(x, y);
            }
          }
          signalCurrent = 9;
        }
        display.setFont(TinyFont);
        display.print("  ", 0, 30);
        display.printNumI(strength, 0, 30);
        if (strengthAbs == strengthAbsSave) {
          display.setPixel(signalCurrent, strengthAbs);
        } else {
          if (strengthAbs > strengthAbsSave) {
            display.drawLine(signalCurrent, strengthAbsSave - 1, signalCurrent,
                             strengthAbs);
          } else {
            display.drawLine(signalCurrent, strengthAbsSave + 1, signalCurrent,
                             strengthAbs);
          }
        }
        strengthAbsSave = strengthAbs;
        display.update();
        delay(50);
        display.disableSleep();
      }
      break;
    case MENU_VISUAL:
      int v1 = (1023 - analogRead(A2)) / 30;
      if (visualCurrent1 > 83) {
        for (int x = 0; x < 84; x++) {
          for (int y = 0; y < 37; y++) {
            display.clrPixel(x, y);
          }
        }
        visualCurrent1 = 0;
        display.update();
        delay(50);
        display.disableSleep();
      }
      if (visualSave1 == v1) {
        display.setPixel(visualCurrent1, v1);
      } else {
        if (v1 > visualSave1) {
          display.drawLine(visualCurrent1, visualSave1 - 1, visualCurrent1, v1);
        } else {
          display.drawLine(visualCurrent1, visualSave1 + 1, visualCurrent1, v1);
        }
      }
      visualCurrent1++;
      visualSave1 = v1;
      display.update();
      delay(50);
      display.disableSleep();
      break;
  }
}
