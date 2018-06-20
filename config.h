/*

  FM Receiver Project based on Arduino
  Copyright (c) 2017-2018 Michael A. Matveev. All right reserved.
  This firmware is licensed under a GNU GPL v.3 License.

  For more information see: https://www.gnu.org/licenses/gpl-3.0.en.html

*/

#include <Arduino.h>
#include "internal.h"

// Should backlight be ON or OFF when powering on
#define DEFAULT_BL ON
// Default volume when powering on, 0-10
#define DEFAULT_VOL 3
// Default frequency when powering on
#define DEFAULT_FREQ 10630
// Default band, either BAND_WEST, BAND_JAPAN, BAND_WORLD or BAND_EAST
#define DEFAULT_BAND BAND_WORLD
// Default band index (BAND_WEST=0, BAND_JAPAN=1, BAND_WORLD=2 or BAND_EAST=3)
#define DEFAULT_BAND_INDEX 2

// Define Arduino digital outputs for pins (display)
#define PIN_CLK 7
#define PIN_DIN 6
#define PIN_DC 5
#define PIN_RST 4
#define PIN_CE 3
#define PIN_BL 2

/*#define PIN_CLK  4
  #define PIN_DIN  5
  #define PIN_DC   2
  #define PIN_RST  0
  #define PIN_CE   1
  #define PIN_BL   3*/
