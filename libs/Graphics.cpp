#include <Arduino.h>
#include <Graphics.h>
#include <LCD5110_Graph.h>
#include <Tools.h>
#include <display.h>


extern unsigned char MediumNumbers[];
extern unsigned char IconsFont[];
extern unsigned char TinyFont[];
extern unsigned char MedvedFont[];

// CLK, DIN, DC, RST, CE
LCD5110 display(PIN_CLK, PIN_DIN, PIN_DC, PIN_RST, PIN_CE);

void Graphics::displayBasics() {
  switch (menu) {
    case 1 ... 4:
      display.setFont(IconsFont);
      display.print(ICON_MONO, 76, 0);
      display.print(ICON_FM, 64, 10);
      display.print(ICON_RDS, 2, 20);
      display.print(ICON_VOL, 2, 28);
      display.update();
      break;
  }
};

void Graphics::drawMenuItem(String item) {
  for (int x = 0; x < 83; x++) {
    for (int y = 37; y < 48; y++) {
      display.setPixel(x, y);
    }
  }
  display.setFont(MedvedFont);
  display.invertText(true);
  display.print(utf8rus(item), CENTER, 39);
  display.invertText(false);
  display.update();
}

void Graphics::resetOldValues() {
  old_strength = -1;
  old_stereo = false;
  old_station = "";
  old_freqText = "";
  old_volume = 0;
}

void Graphics::drawMenu() {
  display.clrScr();
  switch (menu) {
    case 1:
      drawMenuItem("Громкость");
      resetOldValues();
      displayBasics();
      break;
    case 2:
      drawMenuItem("Ручн. настр.");
      resetOldValues();
      displayBasics();
      break;
    case 3:
      drawMenuItem("Автонастр.");
      resetOldValues();
      displayBasics();
      break;
    case 4:
      drawMenuItem("Сигнал");
      signalCurrent = 9;
      display.setFont(TinyFont);
      display.update();
      break;
  }
}

void Graphics::init() { display.InitLCD(); };

void Graphics::updateState(int strength, bool stereo, int volume,
                           String freqText, String station) {
  switch (menu) {
    case 1 ... 3:
      bool needUpdate;
      needUpdate = false;
      if (freqText != old_freqText) {
        needUpdate = true;
        display.setFont(MediumNumbers);
        display.print("/////", 0, 0);
        if (freqText.substring(0, 1) == "1") {
          display.print(freqText.substring(0, 5), 0, 0);
        } else {
          display.print(freqText.substring(1, 5), 12, 0);
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
        /*digitalWrite(PIN_BL, HIGH);
        delay(300);
        digitalWrite(PIN_BL, LOW);*/
        display.update();
        delay(50);
        display.disableSleep();
      }
      break;
    case 4:
      if (checkMillis2(100)) {
        int strengthAbs = strength / 1.8;
        signalCurrent++;
        if (signalCurrent > 83) {
          display.clrScr();
          display.setFont(TinyFont);
          display.update();
          drawMenu();
          signalCurrent = 9;
        }
        display.print("  ", 0, 30);
        display.printNumI(strength, 0, 30);
        display.setPixel(signalCurrent, 35-strengthAbs);
        display.update();
        delay(50);
        display.disableSleep();
      }      
      break;
  }
}