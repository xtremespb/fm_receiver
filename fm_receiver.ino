/*
  fm_receiver.ino - FM Receiver Project based on Arduino
  Copyright (c) 2017-2018 Michael A. Matveev. All right reserved.
  This firmware is licensed under a MIT License.
  For more information see: https://opensource.org/licenses/MIT
*/

#include "lib/FM.h"
#include "lib/Graphics.h"
#include "lib/Tools.h"
#include "lib/config.h"

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
  // Draw menu
  graphics.drawMenu();
}

void loop() {
  // Read analog signal from buttons
  int inV = analogRead(A0);
  // Menu Button
  if (inV > 500 && inV < 524 && checkMillis(300)) {
    graphics.menu++;
    if (graphics.menu > 6) {
      graphics.menu = 1;
    }
    graphics.drawMenu();
  }
  // Left Button
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
    }
  }
  // Right Button
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
    }
  }
  // Read current state from FM Receiver
  fm.readState();
  // Update graphics accoring to the current data got from FM receiver
  graphics.updateState(fm.strength, fm.stereo, fm.volume, fm.freqText,
                       fm.station);
}
