/*

  FM Receiver Project based on Arduino
  Copyright (c) 2017-2018 Michael A. Matveev. All right reserved.
  This firmware is licensed under a GNU GPL v.3 License.

  For more information see: https://www.gnu.org/licenses/gpl-3.0.en.html

*/

#include "FM.h"
#include "Graphics.h"
#include "Tools.h"

Graphics graphics;
FM fm;

void setup() {
  // Set backlight
  pinMode(PIN_BL, OUTPUT);
  digitalWrite(PIN_BL, DEFAULT_BL);
  // Init FM Radio module
  fm.init();
  // Init Graphics module
  graphics.init();
  // Show splash
  graphics.showSplash();
  // Draw menu
  graphics.drawMenu();
}

void loop() {
  // Read analog signal from buttons
  int inV = analogRead(A0);
  // Menu Button presseded
  if (inV > 500 && inV < 524 && checkMillis(300)) {
    graphics.menu++;
    if (graphics.menu > MENU_ITEMS_COUNT) {
      graphics.menu = 1;
    }
    graphics.drawMenu();
  }
  // Left Button presseded
  if (inV < 50 && checkMillis(150)) {
    switch (graphics.menu) {
      case MENU_VOLUME:
        fm.lowerVolume();
        break;
      case MENU_MANUAL:
        fm.lowerFrequency();
        break;
      case MENU_AUTO:
        graphics.showTuningBox();
        fm.autoTune(1);
        delay(500);
        graphics.hideTuningBox();
        break;
      case MENU_BL:
        if (checkMillis2(450)) {
          graphics.toggleBL();
        }
        break;
      case MENU_BASS:
        if (checkMillis2(450)) {
          graphics.switchBass();
          if (graphics.bass) {
            fm.moreBass();
          } else {
            fm.lessBass();
          }
        }
        break;
      case MENU_BAND:
        if (checkMillis2(250)) {
          graphics.band--;
          if (graphics.band < 0) {
            graphics.band = 3;
          }
          graphics.drawBandSelect(graphics.band);
          fm.setBandByIndex(graphics.band);
        }
        break;
    }
  }
  // Right Button pressed
  if (inV < 700 && inV > 660 && checkMillis(150)) {
    switch (graphics.menu) {
      case MENU_VOLUME:
        fm.higherVolume();
        break;
      case MENU_MANUAL:
        fm.higherFrequency();
        break;
      case MENU_AUTO:
        graphics.showTuningBox();
        fm.autoTune(0);
        delay(500);
        graphics.hideTuningBox();
        break;
      case MENU_BL:
        if (checkMillis2(450)) {
          graphics.toggleBL();
        }
        break;
      case MENU_BASS:
        if (checkMillis2(450)) {
          graphics.switchBass();
          if (graphics.bass) {
            fm.moreBass();
          } else {
            fm.lessBass();
          }
        }
        break;
      case MENU_BAND:
        if (checkMillis2(250)) {
          graphics.band++;
          if (graphics.band > 3) {
            graphics.band = 0;
          }
          graphics.drawBandSelect(graphics.band);
          fm.setBandByIndex(graphics.band);
        }
        break;
    }
  }
  // Read current state from FM Receiver
  fm.readState();
  // Update graphics accoring to the current data got from FM receiver
  graphics.updateState(fm.strength, fm.stereo, fm.volume, fm.freqText,
                       fm.station);
}
