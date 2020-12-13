#pragma once
#include "AB.h"

// DS3231 in circuit?
#define RTC_ENABLED 0

// DS18 in circuit?
#define TEMPERATURE_ENABLED 0

// Multiple OLEDs in circuit?
#define HYDRA_ENABLED 0

// Multiple Arduinos communicating? 
#define MULTICONTROLLER_ENABLED 0

// 74165 in circuit?
#define ABI_ENABLED 0

// 74595 in circuit?
#define ABO_ENABLED 0

#define ABI_TEST 0
#define ABO_TEST 0

// 74HC165
// SHIFT_IN_LOAD <-  SH -|  1     16 |- VCC     -> +5v
//         CLOCK <- CLK -|  2     15 |- CLK_INH -> SHIFT_IN_READ
//         bit 4 <-   E -|  3     14 |- D       -> bit 3
//         bit 5 <-   F -|  4     13 |- C       -> bit 2
//         bit 6 <-   G -|  5     12 |- B       -> bit 1
//         bit 7 <-   H -|  6     11 |- A       -> bit 0
//           GND <-  ~Q -|  6     10 |- SER     -> 10kΩ -> +5v (or next highest SR)
//           GND <- GND -|  6      9 |- Q       -> SHIFT_IN_DATA

/*
ALKS
AMPERKA_WIFI_SLOT
AVR_ADK
AVR_BT
AVR_CIRCUITPLAY
AVR_DUEMILANOVE
AVR_ESPLORA
AVR_ETHERNET
AVR_FIO
AVR_GEMMA
AVR_INDUSTRIAL101
AVR_LEONARDO
AVR_LEONARDO_ETH
AVR_LILYPAD
AVR_LILYPAD_USB
AVR_LININO_ONE
AVR_MEGA
AVR_MEGA2560
AVR_MICRO
AVR_MINI
AVR_NANO
AVR_NANO_EVERY
AVR_NG
AVR_PRO
AVR_ROBOT_CONTROL
AVR_ROBOT_MOTOR
AVR_UNO
AVR_UNO_WIFI_DEV_ED
AVR_UNO_WIFI_REV2
AVR_YUN
AVR_YUNMINI
BPI-BIT
CoreESP32
D-duino-32
D1_MINI32
ESP320
ESP32_DEV
ESP32_DEVKIT_LIPO
ESP32_EVB
ESP32_GATEWAY
ESP32_PICO
ESP32_POE
ESP32_POE_ISO
ESP32_THING
ESP8266_ARDUINO
ESP8266_ARDUINO_PRIMO
ESP8266_ARDUINO_STAR_OTTO
ESP8266_ARDUINO_UNOWIFI
ESP8266_ESP01
ESP8266_ESP07
ESP8266_ESP12
ESP8266_ESP13
ESP8266_ESP210
ESP8266_ESPECTRO_CORE
ESP8266_ESPRESSO_LITE_V1
ESP8266_ESPRESSO_LITE_V2
ESP8266_GENERIC
ESP8266_NODEMCU
ESP8266_OAK
ESP8266_PHOENIX_V1
ESP8266_PHOENIX_V2
ESP8266_SCHIRMILABS_EDUINO_WIFI
ESP8266_SONOFF_BASIC
ESP8266_SONOFF_S20
ESP8266_SONOFF_SV
ESP8266_SONOFF_TH
ESP8266_THING
ESP8266_THING_DEV
ESP8266_WEMOS_D1MINI
ESP8266_WEMOS_D1MINILITE
ESP8266_WEMOS_D1MINIPRO
ESP8266_WEMOS_D1R1
ESP8266_WIO_LINK
ESPECTRO32
ESPea32
ESPino32
FEATHER_ESP32
FROG_ESP32
GEN4_IOD
GROVE_UI_WIRELESS
HELTEC_WIFI_KIT_32
HELTEC_WIFI_LORA_32
HELTEC_WIFI_LORA_32_V2
HELTEC_WIRELESS_STICK
HORNBILL_ESP32_DEV
HORNBILL_ESP32_MINIMA
INTOROBOT_ESP32_DEV
LOLIN32
LOLIN_D32
LOLIN_D32_PRO
LoPy
LoPy4
M5STACK_FIRE
M5Stack_Core_ESP32
M5Stick_C
MH_ET_LIVE_ESP32DEVKIT
MH_ET_LIVE_ESP32MINIKIT
MOD_WIFI_ESP8266
NANO32
Node32s
NodeMCU_32S
ODROID_ESP32
ONEHORSE_ESP32_DEV
OROCA_EDUBOT
PYCOM_GPY
Pocket32
QUANTUM
SEEED_FEMTO_M0
SEEED_LORAWAN
SEEED_XIAO_M0
SEEED_ZERO
T-Beam
T-Watch
TTGO_LoRa32_V1
TTGO_T1
UBLOX_NINA_W10
WESP32
WIDORA_AIR
WIFIDUINO_ESP8266
WIFINFO
WIO_GPS_BOARD
WIO_TERMINAL
WIPY3
Wio_Lite_MG126
esp32vn_iot_uno
fm-devkit
*/

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

#if defined(SEEED_XIAO_M0)

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

#elif defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_NANO_EVERY)
#define ONE_WIRE_BUS 2

// SSD1331 / ST7735 Nano/Atmega328 PINS: connect LCD to D5 (D/C), D4 (CS), D3 (RES), D11(SDA), D13(SCL)
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
// trying A0 (14) b/c D12 didn't seem to work
#define SHIFT_OUT_LATCH A0

#elif defined(ESP32)
// 12,13,14,15 used for JLINK

//       27  26  25 33 32 35 34 VN VP EN
// D2 D4 RX2 TX2 D5 18 19 21 RX0 TX0 22 24
#define ONE_WIRE_BUS 2

// SSD1331 / ST7735 Nano/Atmega328 PINS: connect LCD to D5 (D/C), D4 (CS), D3 (RES), D11(SDA), D13(SCL)
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
// trying A0 (14) b/c D12 didn't seem to work
#define SHIFT_OUT_LATCH A0
#endif

typedef enum {
  // // Connected to CLK on KY-040
  // // Connected to A (11) on 74165
  kSinRotaryA = 0x00,
  kSinUp = kSinRotaryA,
  // // Connected to DT on KY-040
  // // Connected to B (12) on 74165
  kSinRotaryB = 0x01,
  kSinDown = kSinRotaryB,
  // // Connected to SW on KY-040
  // // Connected to C (13) on 74165
  kSinRotaryButton = 0x02,
  kSinEnter = kSinRotaryButton,

  kSinBack = 0x03,
  kSinDay = 0x04,
  kSinNight = 0x05,
  kSinOff = 0x06,
  kSinMute = 0x07,

  kSinInDigital = 0x08,
  kSinInAnalog = 0x09,
  kSinInAux = 0x0A,

  kSin0x0B = 0x0B,
  kSin0x0C = 0x0C,

  kSinPreEQ = 0x0D,
  kSinPreBellari = 0x0E,
  kSinPreValhalla = 0x0F,

  kSin0x10 = 0x10,
  kSin0x11 = 0x11,

  kSinOutGeshelli = 0x12,
  kSinOutMonolith = 0x12,
  kSinOutValhalla = 0x14,
  kSinOutSpeakers = 0x15,
  kSinOutSubwoofer = 0x16,
  kSinOutADC = 0x17,

} ShiftInBit;

typedef enum {
  kSout0x00 = 0x00,

  kSout_input_a = 0x01,
  kSout_input_b = 0x02,

  kSout0x03 = 0x03,

  kSout_engage_loki = 0x04,
  kSout_engage_bellari = 0x05,
  kSout_engage_valhalla = 0x06,
  kSout_engage_level = 0x07,

  kSout0x08 = 0x08,
  kSout0x09 = 0x09,

  kSout_output_a = 0x0A,
  kSout_output_b = 0x0B,
  kSout_output_c = 0x0C,
  kSout_disable_subwoofer = 0x0D,
  kSout_engage_mute = 0x0E,
  kSout_headphones = 0x0F,

} ShiftOutBit;

#define INVERT_RELAY_CONTROL 1

#if INVERT_RELAY_CONTROL
#define SOUT_LOW HIGH
#define SOUT_HIGH LOW
#else
#define SOUT_LOW LOW
#define SOUT_HIGH HIGH
#endif
