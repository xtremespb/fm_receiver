#include <Arduino.h>

#define DEFAULT_BL LOW

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

#define MENU_VOLUME 1
#define MENU_MANUAL 2
#define MENU_AUTO 3
#define MENU_SIGNAL 4