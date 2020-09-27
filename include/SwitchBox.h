#pragma once

#define BUTTON_PIN_A 0
#define BUTTON_PIN_B 1

// 0, 1 don't seem to work with UNO
// Connected to CLK on KY-040
#define ROTARY_PIN_A 2
// Connected to DT on KY-040
#define ROTARY_PIN_B 3
// 4, 5 used by OLED on XIAO as A4/A5

// 4 -- SDA
// 5 -- SDC

// RELAY_INPUT switches from the DAC to the PHONO input
#define RELAY_INPUT 6
// RELAY_MONITOR engages the preamp bypass (direct to speakers)
#define RELAY_MONITOR 7
// RELAY_AMP switches from GESHELLI to VALHALLA output
#define RELAY_AMP 8
// RELAY_SUB engages the subwoofer output
#define RELAY_SUB 9

// Connected to SW on KY-040
#define ROTARY_PIN_BUTTON 10
