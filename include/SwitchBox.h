#pragma once
#include "AB.h"

// 74HC165
// SHIFT_IN_LOAD <-  SH -|  1     16 |- VCC     -> +5v
//         CLOCK <- CLK -|  2     15 |- CLK_INH -> SHIFT_IN_READ
//         bit 4 <-   E -|  3     14 |- D       -> bit 3
//         bit 5 <-   F -|  4     13 |- C       -> bit 2
//         bit 6 <-   G -|  5     12 |- B       -> bit 1
//         bit 7 <-   H -|  6     11 |- A       -> bit 0
//           GND <-  ~Q -|  6     10 |- SER     -> 10kΩ -> +5v (or next highest SR)
//           GND <- GND -|  6      9 |- Q       -> SHIFT_IN_DATA

// #define BUTTON_PIN_A 0
// #define BUTTON_PIN_B 1

// // 0, 1 don't seem to work with UNO
// // Connected to CLK on KY-040
// #define ROTARY_PIN_A 2
// // Connected to DT on KY-040
// #define ROTARY_PIN_B 3
// // 4, 5 used by OLED on XIAO as A4/A5

// // RELAY_INPUT switches from the DAC to the PHONO input
// #define RELAY_INPUT 6
// // RELAY_MONITOR engages the preamp bypass (direct to speakers)
// #define RELAY_MONITOR 7
// // RELAY_AMP switches from GESHELLI to VALHALLA output
// #define RELAY_AMP 8
// // RELAY_SUB engages the subwoofer output
// #define RELAY_SUB 9

// // Connected to SW on KY-040
// #define ROTARY_PIN_BUTTON 10

// SR_IN CLK, SH_OUT CLK
#define SHIFT_CLK 0

// SR_IN Q
#define SHIFT_IN_DATA 1
// SR_IN SH (take low to load)
#define SHIFT_IN_LOAD 2
// SR_IN CLK_INH (take low to read)
#define SHIFT_IN_READ 3

// 4, 5 used by OLED on XIAO as A4/A5

// Need a 74595 for the latched output, otherwise the serially-loaded bits will ghost over the relays
// !OE should be held low
// SR_OUT SER
#define SHIFT_OUT_DATA 6
// SR_OUT RCLK
#define SHIFT_OUT_LATCH 7

// 8, 9, 10 used by OLED on XIAO as A4/A5

// from ADA           -- seeed
// #define cs   10    -- MOSI
// #define dc   9     -- MISO
// #define rst  8     -- SCK

typedef enum {
  // // Connected to CLK on KY-040
  // // Connected to A (11) on 74165
  kSinRotaryA = 0,
  kSinUp = kSinRotaryA,
  // // Connected to DT on KY-040
  // // Connected to B (12) on 74165
  kSinRotaryB = 1,
  kSinDown = kSinRotaryB,
  // // Connected to SW on KY-040
  // // Connected to C (13) on 74165
  kSinRotaryButton = 2,
  kSinEnter = kSinRotaryButton,

  // // Connected to D (14) on 74165
  kSinKeyD = 3,
  // // Connected to E (3) on 74165
  kSinKeyE = 4,
  // // Connected to F (4) on 74165
  kSinKeyF = 5,
  // // Connected to G (5) on 74165
  kSinKeyG = 6,
  // // Connected to H (6) on 74165
  kSinKeyH = 7,
} ShiftInBit;

typedef enum {
  kSoutRelayInput = 0,
  kSoutRelayMonitor = 1,
  kSoutRelayAmp = 2,
  kSoutRelaySub = 3,
  kSoutUnusedA = 4,
  kSoutUnusedB = 5,
  kSoutUnusedC = 6,
  kSoutUnusedD = 7,
} ShiftOutBit;
