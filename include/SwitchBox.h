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

#define XIAO 1
#define MINI 2
#define NANO 3

#if DEVICE == XIAO

// SR_IN CLK, SH_OUT CLK
#define SHIFT_CLK 0

// SR_IN Q
#define SHIFT_IN_DATA 1
// SR_IN SH (take low to load)
#define SHIFT_IN_LOAD 2
// SR_IN CLK_INH (take low to read)
#define SHIFT_IN_READ 3

// 4, 5 used by OLED on XIAO as A4/A5
#define SSD1306_SDA 4
#define SSD1306_SCL 5
#define SSD1331_DC 4
#define SSD1331_RES 5

// Need a 74595 for the latched output, otherwise the serially-loaded bits will ghost over the relays
// !OE should be held low
// SR_OUT SER
#define SHIFT_OUT_DATA 6
// SR_OUT RCLK
#define SHIFT_OUT_LATCH 7

// 8, 9, 10 used by OLED
//   XIAO       SSD1331    lcdgfx
//  8 - SCK       SCL       SCLK
//  9 - MISO
// 10 - MOSI      SDA       MOSI
#define SSD1331_SCL 8
#define SSD1331_CS 9
#define SSD1331_SDA 10

#endif

#if DEVICE == NANO
// SSD1331 Nano/Atmega328 PINS: connect LCD to D5 (D/C), D4 (CS), D3 (RES), D11(SDA), D13(SCL)
#define SSD1331_SCL 13
#define SSD1331_SDA 11
#define SSD1331_RES 3
#define SSD1331_DC 5
#define SSD1331_CS 4

// SR_IN CLK, SH_OUT CLK
#define SHIFT_CLK 6

// SR_IN Q
#define SHIFT_IN_DATA 7
// SR_IN SH (take low to load)
#define SHIFT_IN_LOAD 8
// SR_IN CLK_INH (take low to read)
#define SHIFT_IN_READ 9

// Need a 74595 for the latched output, otherwise the serially-loaded bits will ghost over the relays
// !OE should be held low
// SR_OUT SER
#define SHIFT_OUT_DATA 10
// SR_OUT RCLK
// trying A0 b/c D12 didn't seem to work
#define SHIFT_OUT_LATCH 14

#endif

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
