/*

  FM Receiver Project based on Arduino
  Copyright (c) 2017-2018 Michael A. Matveev. All right reserved.
  This firmware is licensed under a GNU GPL v.3 License.

  For more information see: https://www.gnu.org/licenses/gpl-3.0.en.html

*/

#ifndef FW_VERSION
#define FW_VERSION 1
#endif
#ifndef HW_REVISION
#define HW_REVISION 3
#endif

#define ON HIGH
#define OFF LOW
#define BAND_WEST (0x0 << 2)
#define BAND_JAPAN (0x1 << 2)
#define BAND_WORLD (0x2 << 2)
#define BAND_EAST (0x3 << 2)

// Graphical definitions for font icons
#define ICON_SIGNAL_1 "'"
#define ICON_SIGNAL_2 "("
#define ICON_SIGNAL_3 ")"
#define ICON_SIGNAL_4 "*"
#define ICON_SIGNAL_5 "+"
#define ICON_FM "/"
#define ICON_VOL ","
#define ICON_MONO "-"
#define ICON_STEREO "."
#define ICON_VOL_PART "0"
#define ICON_BLANK "1"
#define ICON_RDS "2"

// Number of items in menu
#if HW_REVISION > 3
#define MENU_ITEMS_COUNT 9
#else
#define MENU_ITEMS_COUNT 8
#endif
// Menu items
#define MENU_VOLUME 1
#define MENU_MANUAL 2
#define MENU_AUTO 3
#define MENU_SIGNAL 4
#define MENU_BL 5
#define MENU_BASS 6
#define MENU_BAND 7
#if HW_REVISION > 3
#define MENU_VISUAL 8
#else
#define MENU_VISUAL -1
#endif
#if HW_REVISION > 3
#define MENU_ABOUT 9
#else
#define MENU_ABOUT 8
#endif