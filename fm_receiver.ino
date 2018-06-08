/*
  fm_receiver.ino - FM Receiver Project based on Arduino
  Copyright (c) 2017-2018 Michael A. Matveev. All right reserved.
  This library is licensed under a MIT License.
  For more information see: https://opensource.org/licenses/MIT
*/

#include "./libs/FM.h"
#include "./libs/Graphics.h"
#include "./libs/Tools.h"
#include "./libs/display.h"

Graphics graphics;
FM fm;

void setup() {
  // Set backlight
  pinMode(PIN_BL, OUTPUT);
  digitalWrite(PIN_BL, DEFAULT_BL);
  // Init FM Radio module
  fm.init();
  // Workaround for Display startup
  // delay(500);
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
    if (graphics.menu > 4) {
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
        break;
    }
  }
  // Read current state from FM Receiver
  fm.readState();
  // Update graphics accoring to the current data got from FM receiver
  graphics.updateState(fm.strength, fm.stereo, fm.volume, fm.freqText,
                       fm.station);
}
