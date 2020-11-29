/*
  DigitalReadSerial

  Reads a digital input on pin 2, prints the result to the Serial Monitor

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/DigitalReadSerial
*/

#define CONFIG_ARDUINO_SPI_ENABLE

#include "SwitchBox.h"
#include "ABI.h"
#include "ABO.h"
#include "Debounce.h"
#include "DebugLine.h"
#include "RealTimeClock.h"
#include "RotaryEncoder.h"
#include "SoftWireI2C.h"
#include "SwitchBoxStateMachine.h"
#include "Temperature.h"
#include <Multi_OLED.h>
#include <SoftWire.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <canvas/canvas_types.h>
#include <lcdgfx.h>

#ifndef PSTR
#define PSTR(A) A
#endif

#define PROGNAME "SwitchBox"
#define VERSION "0.4.1"

#define WHITE_OLED 1
#define COLOR_OLED 2

#define DISPLAY_TYPE COLOR_OLED
#define USE_ROTARY_INPUT 0
#define USE_BUTTON_NAV 1
#define DEBUG_COLOR_DRAW 0

#define MENU_MODE_SMOOTH 1

#if DISPLAY_TYPE == WHITE_OLED
DisplaySSD1306_128x64_I2C display(-1);
#define DISPLAY_WIDTH (128)
#define COLOR_BLACK BLACK
#define COLOR_RED WHITE
#define COLOR_GREEN WHITE
#define COLOR_BLUE WHITE
#elif DISPLAY_TYPE == COLOR_OLED
#define SPI_FREQ 0
SPlatformSpiConfig spiConfig = {-1, {SSD1331_CS}, SSD1331_DC, SPI_FREQ, -1, -1};
DisplaySSD1331_96x64x16_SPI display(SSD1331_RES, spiConfig); // 8, {-1, 10, 9});
#define DISPLAY_WIDTH (96)
#define COLOR_BLACK RGB_COLOR16(0, 0, 0)
#define COLOR_RED RGB_COLOR16(0xFFFF, 0, 0)
#define COLOR_GREEN RGB_COLOR16(0, 0xFFFF, 0)
#define COLOR_BLUE RGB_COLOR16(0, 0, 0xFFFF)
#define COLOR_YELLOW RGB_COLOR16(0xFFFF, 0xFFFF, 0)
#define COLOR_WHITE RGB_COLOR16(0xFFFF, 0xFFFF, 0xFFFF)
#define COLOR_MENU_TEXT 0x4160
#else
#error No display type specified
#endif

/**

 * bus id number. this parameter is valid for Linux, ESP32.
 * If -1 is pointed, it defaults to platform specific i2c bus (Linux i2c-dev0, esp32 I2C_NUM_0).
int8_t busId;

 * Address of i2c device to control. This is mandatory argument for all platforms
uint8_t addr;

 * Pin to use as i2c clock pin. This parameter is not used in Linux.
 * If -1 is pointed, the library uses default clock pin for specific platform.
int8_t scl;

 * Pin to use as i2c data pin. This parameter is not used in Linux.
 * If -1 is pointed, the library uses default data pin for specific platform.
int8_t sda;

 * Frequency in HZ to run spi bus at. Zero value defaults to platform optimal clock
 * speed (100kHz or 400kHz depending on platform).
uint32_t frequency;

 */
// #define SCL_1 SCL
// #define SCL_2 A1
// #define SCL_3 A1

// #define SDA_1 SDA
// #define SDA_2 A3
// #define SDA_3 A2

#define RTC_ADDR 0x68
#define DISPLAY_2_ADDR 0x3C
#define DISPLAY_3_ADDR 0x3D
#define DISPLAY_4_ADDR 0x3C
SPlatformI2cConfig i2cConfig2 = {-1, DISPLAY_2_ADDR, SCL, SDA, 0};
DisplaySSD1306_128x64_I2C display2(-1, i2cConfig2);
SPlatformI2cConfig i2cConfig3 = {-1, DISPLAY_3_ADDR, SCL, SDA, 0};
DisplaySSD1306_128x64_I2C display3(-1, i2cConfig3);
// DisplaySSD1306_128x64_CustomI2C<SoftWireI2C> display3(-1, A1, A3, DISPLAY_3_ADDR, 1200);
// DisplaySSD1306_128x64_CustomI2C<SoftWireI2C> display4(-1, SCL_3, SDA_3, DISPLAY_4_ADDR);

#include <Multi_BitBang.h>
#include <Multi_OLED.h>

#define NUM_DISPLAYS 3
#define NUM_BUSES 2
// I2C bus info
uint8_t scl_list[NUM_BUSES] = {A1, A1}; //{9,9,9,9};
uint8_t sda_list[NUM_BUSES] = {A2, A3}; //{5,6,7,8};
int32_t speed_list[NUM_BUSES] = {400000L, 400000L};
// OLED display info
uint8_t bus_list[NUM_DISPLAYS] = {0, 1}; // can be multiple displays per bus
uint8_t addr_list[NUM_DISPLAYS] = {0x3c, 0x3c};
uint8_t type_list[NUM_DISPLAYS] = {OLED_128x64, OLED_128x64};
uint8_t flip_list[NUM_DISPLAYS] = {0, 0};
uint8_t invert_list[NUM_DISPLAYS] = {0, 0};

void hydra_setup() {
  // put your setup code here, to run once:
  Multi_I2CInit(sda_list, scl_list, speed_list, NUM_BUSES);
  Multi_OLEDInit(bus_list, addr_list, type_list, flip_list, invert_list, NUM_DISPLAYS);
} // setup

#define SET_MENU_FONT()                      \
  display.setFixedFont(ssd1306xled_font5x7); \
  display.setColor(COLOR_MENU_TEXT)

SAppMenu menu;
#define MENU_ITEM_COUNT (0)
char *menuItems[MENU_ITEM_COUNT] = {};

NanoRect menuRect() {
  auto result = display.rect();
  result.p1.y = result.p2.y - 42;
  return result;
}

#if USE_ROTARY_INPUT
RotaryState rotaryState;
#endif
#if USE_BUTTON_NAV
ButtonState buttonUp = {kABIPinShiftRegister, kSinUp};
ButtonState buttonDown = {kABIPinShiftRegister, kSinDown};
ButtonState buttonEnter = {kABIPinShiftRegister, kSinEnter};
#endif

OneWire oneWire(ONE_WIRE_BUS);
Temperature temperature(oneWire);
RealTimeClock rtc;
// RX, TX
SoftwareSerial ser(A6, A7);

void printRTCTemperature() {
  // Serial_printf("%2d.%02d°\n", rtc.temp() / 100, rtc.temp() % 100);
}

void printRTCTime(bool aDoRefresh = true) {
  char tTimeString[13];
  rtc.format(tTimeString, 13, {true, kTF_amPm, true}, aDoRefresh);
  Serial.println(tTimeString);
  rtc.format(tTimeString, 13, {true, kTF_24H, true});
  Serial.println(tTimeString);
  rtc.format(tTimeString, 13, {true, kTF_12H, true});
  Serial.println(tTimeString);
  rtc.format(tTimeString, 13, {true, kTF_amPm, false});
  Serial.println(tTimeString);
  rtc.format(tTimeString, 13, {true, kTF_24H, false});
  Serial.println(tTimeString);
  rtc.format(tTimeString, 13, {true, kTF_12H, false});
  Serial.println(tTimeString);
  rtc.format(tTimeString, 13, {false, kTF_amPm, true});
  Serial.println(tTimeString);
  rtc.format(tTimeString, 13, {false, kTF_24H, true});
  Serial.println(tTimeString);
  rtc.format(tTimeString, 13, {false, kTF_12H, true});
  Serial.println(tTimeString);
  rtc.format(tTimeString, 13, {false, kTF_amPm, false});
  Serial.println(tTimeString);
  rtc.format(tTimeString, 13, {false, kTF_24H, false});
  Serial.println(tTimeString);
  rtc.format(tTimeString, 13, {false, kTF_12H, false});
  Serial.println(tTimeString);
}

TimeFormat headerTimeFormat = new TimeFormat();

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ; // delay for Leonardo

  Serial.println(F("START " __FILE__));
  Serial.println(F(PROGNAME " Version " VERSION " built on " __DATE__));

  ser.begin(9600);

  // Start the I2C Bus as Master
  Wire.begin();

  rtc.setup();
  temperature.setup();

  display.begin();
  display.clear();

  ABInit abinit = {0};
  abinit.clk = SHIFT_CLK;
  abinit.data = SHIFT_IN_DATA;
  abinit.load = SHIFT_IN_LOAD;
  abinit.read = SHIFT_IN_READ;
  abi_setup(abinit);

  ABOnit abonit = {0};
  abonit.clk = SHIFT_CLK;
  abonit.data = SHIFT_OUT_DATA;
  abonit.load = SHIFT_OUT_LATCH;
  abo_setup(abonit);

#if USE_ROTARY_INPUT
  rotaryState.pinA.pin = {kABIPinShiftRegister, kSinRotaryA};
  rotaryState.pinB.pin = {kABIPinShiftRegister, kSinRotaryB};
  rotaryState.pinSwitch.pin = {kABIPinShiftRegister, kSinRotaryButton};
  rotary_setup(rotaryState);
#endif
#if USE_BUTTON_NAV
  debounce(buttonUp);
  debounce(buttonDown);
  debounce(buttonEnter);
#endif

  // debounce(buttonRed);
  // debounce(buttonGreen);
  // debounce(buttonBlue);
  // debounce(buttonYellow);

  sbsm_setup();

  // for (size_t i = 0; i < MENU_ITEM_COUNT; i++) {
  //   menuItems[i] = (char *)calloc(64 + 1, sizeof(char));
  //   strcpy(menuItems[i], sbsm_trigger_name((Trigger)i));
  // }

  display.createMenu(&menu, const_cast<const char **>(menuItems), sizeof(menuItems) / sizeof(char *), menuRect());
  SET_MENU_FONT();
#if MENU_MODE_SMOOTH
  display.showMenuSmooth(&menu);
#else
  display.showMenu(&menu);
#endif
  // }

  // Multi_OLEDWriteCommand2(0x81, ucContrast);

  // void extended_display_start() {
  display2.begin();
  display2.clear();
  display2.setFreeFont(free_calibri11x12);
  display2.setTextCursor(10, 0);
  display2.write("Display 2");

  display3.begin();
  display3.clear();
  display3.setFreeFont(free_calibri11x12);
  display3.setTextCursor(10, 0);
  display3.write("Display 3");

  // display4.begin();
  // display4.clear();
  // display4.setFreeFont(free_calibri11x12);
  // display4.setTextCursor(10, 0);
  // display4.write("Display 4");

  hydra_setup();
}

int encoderPosCount = 0;
// bool menuDirty = true;

#define LABEL_STRING_BUFFER_SIZE (42)
char hydraLabel[LABEL_STRING_BUFFER_SIZE] = {0};
char lastLabel0[LABEL_STRING_BUFFER_SIZE] = {0};
char lastLabel1[LABEL_STRING_BUFFER_SIZE] = {0};
char lastLabel2[LABEL_STRING_BUFFER_SIZE] = {0};
char lastLabel3[LABEL_STRING_BUFFER_SIZE] = {0};
char lastTime[TIME_STRING_BUFFER_SIZE] = {0};

void hydra_loop() {
  uint8_t i, j, bit, map[16];
  uint8_t addr;
  char szTemp[16];
  for (i = 0; i < NUM_BUSES; i++) {
    Serial.print("Scanning I2C bus ");
    Serial.println(i);
    Multi_I2CScan(i, map);
    for (j = 0; j < 16; j++) {
      if (map[j] != 0) // device(s) found
      {
        for (bit = 0; bit < 8; bit++) {
          if (map[j] & (1 << bit)) // device here
          {
            addr = (j * 8) + bit;
            Serial.print("Device found at address 0x");
            Serial.println(addr, HEX);
          }
        }
      }
    }
  } // for each bus

  for (int i = 0; i < NUM_DISPLAYS; i++) {
    Multi_OLEDFill(i, 0);
    Multi_OLEDSetContrast(i, 255);
    Multi_OLEDWriteString(i, 0, 0, (char *)"Display", FONT_SMALL, 0);
    sprintf(hydraLabel, "Num: %d", i + 3);
    Multi_OLEDWriteString(i, 0, 2, hydraLabel, FONT_NORMAL, 0);
  }
}

// the loop routine runs over and over again forever:
float avgTick = 0;
unsigned long ts = micros();
unsigned long debug_ts = micros();

#define CHAR_WIDTH_LG (DISPLAY_WIDTH / 6)
#define CHAR_WIDTH_SM (DISPLAY_WIDTH / 5)

lcdint_t x = 0;
lcdint_t y = 0;

// void i2cScan() {
//   byte error, address;
//   int bus;

//   for (bus = 3; bus <= 4; bus++) {
//     SoftWire wire(A0 + bus, SCL);
//     // wire.setClock(1200);
//     wire.begin();
//     Serial_printf("Scanning bus %d...\n", bus);
//     for (address = 1; address < 127; address++) {
//       // The i2c_scanner uses the return value of
//       // the Write.endTransmisstion to see if
//       // a device did acknowledge to the address.
//       wire.beginTransmission(address);
//       error = wire.endTransmission();

//       if (error == 0) {
//         Serial_printf("I2C address %d.%02x : device found!\n", bus, address);
//       } else if (error == 4) {
//         Serial_printf("I2C address %d.%02x : timeout\n", bus, address);
//       }
//     }
//     wire.end();
//   }
// }
/*
 EEPROM
 nano (every) has 256 bytes
*/

#define TICK_LENGTH 100
#define DEBUG_INTERVAL 1000000

bool menuUpdate = true;
uint32_t loopCount(0);

void loop() {
  auto dt = micros() - ts;
  if (dt < TICK_LENGTH) {
    delayMicroseconds(TICK_LENGTH - dt);
  }
  dt = micros() - ts;
  avgTick = ((29.0f * avgTick) + dt) / 30.0f;
  ts += dt;

  abi_loop();
  sbsm_loop();

#if USE_ROTARY_INPUT
  RotaryAction action = rotary_loop(rotaryState);
  // if (action != kRotaryActionNone) {
  //   Serial_printf("RotaryAction: %d\n", action);
  // }
  if (action) {
    switch (action) {
    case kRotaryActionClick: {
      Trigger event = (Trigger)menu.selection;
      sbsm_trigger(event);
      auto triggerName = triggerNames.find(event)->second;
      ser.write(triggerName);
      ser.write('\n ');
      break;
    }
    case kRotaryActionWiddershinsUp:
      display.menuUp(&menu);
      menuUpdate = true;
      break;
    case kRotaryActionClockwiseUp:
      display.menuDown(&menu);
      menuUpdate = true;
      break;
    default:
      break;
    }
  }
#endif
#if USE_BUTTON_NAV
  if (debounce(buttonUp) && buttonUp.value) {
    display.menuUp(&menu);
    menuUpdate = true;
  }
  if (debounce(buttonDown) && buttonDown.value) {
    display.menuDown(&menu);
    menuUpdate = true;
  }
  if (debounce(buttonEnter) && buttonEnter.value) {
    Trigger event = (Trigger)menu.selection;
    sbsm_trigger(event);
    Serial.print("Select: ");
    Serial.println(triggerNames.find(event)->second);
  }
#endif

  if (menuUpdate) {
    menuUpdate = false;
    SET_MENU_FONT();
#if MENU_MODE_SMOOTH
    display.updateMenuSmooth(&menu);
#else
    display.updateMenu(&menu);
#endif
    lastLabel0[0] = 0;
    lastLabel1[0] = 0;
    lastTime[0] = 0;
  }

  // char row0[CHAR_WIDTH_LG + 1] = {0};
  // char row1[CHAR_WIDTH_SM + 1] = {0};
  char label0[LABEL_STRING_BUFFER_SIZE] = {0};
  char label1[LABEL_STRING_BUFFER_SIZE] = {0};
  char label2[LABEL_STRING_BUFFER_SIZE] = {0};
  char label3[LABEL_STRING_BUFFER_SIZE] = {0};
  char time[TIME_STRING_BUFFER_SIZE] = {0};

  snprintf(label0, LABEL_STRING_BUFFER_SIZE, "%s", sbsm_input_label());
  snprintf(label1, LABEL_STRING_BUFFER_SIZE, "%s", sbsm_output_label());
  snprintf(label2, LABEL_STRING_BUFFER_SIZE, "%s", sbsm_subwoofer_label());
  headerTimeFormat.amPm = kTF_12H;
  headerTimeFormat.pad = false;
  headerTimeFormat.seconds = false;
  rtc.format(time, TIME_STRING_BUFFER_SIZE, headerTimeFormat, true);

  if (strncmp(lastLabel0, label0, LABEL_STRING_BUFFER_SIZE) || strncmp(lastLabel1, label1, LABEL_STRING_BUFFER_SIZE) || strncmp(lastLabel2, label2, LABEL_STRING_BUFFER_SIZE) ||
      strncmp(lastLabel3, label3, LABEL_STRING_BUFFER_SIZE) || strncmp(lastTime, time, TIME_STRING_BUFFER_SIZE)) {
    strncpy(lastLabel0, label0, LABEL_STRING_BUFFER_SIZE);
    strncpy(lastLabel1, label1, LABEL_STRING_BUFFER_SIZE);
    strncpy(lastLabel2, label2, LABEL_STRING_BUFFER_SIZE);
    strncpy(lastLabel3, label3, LABEL_STRING_BUFFER_SIZE);
    strncpy(lastTime, time, TIME_STRING_BUFFER_SIZE);

    // INPUT  |_10:20_|  OUTPUT
    auto textWidth = display.getFont().getTextSize(time);
    auto left = display.width() / 2 - textWidth / 2;
    auto right = left + textWidth;
    NanoRect timeRect = {left, 0, right, 9};
    display.setColor(0x31C8);
    display.fillRect(timeRect);
    NanoRect rect = timeRect;
    rect = {left, 0, right, 8};
    display.setColor(COLOR_BLACK);
    display.fillRect(rect);
    display.setColor(0x5371);
    display.setFixedFont(digital_font5x7_AB);
    display.printFixed(rect.p1.x, 0, time, STYLE_NORMAL);

    textWidth = display.getFont().getTextSize("IN");
    left = timeRect.p1.x / 2 - textWidth / 2;
    right = left + textWidth;
    rect = {left, 0, right, 8};
    display.setColor(COLOR_BLACK);
    display.fillRect(rect);
    display.setColor(COLOR_WHITE);
    display.printFixed(rect.p1.x, rect.p1.y + 2, "IN");

    textWidth = display.getFont().getTextSize("OUT");
    Serial_printf("timeRect.p2.x = %d\n", timeRect.p2.x);
    Serial_printf("textWidth = %d\n", textWidth);
    Serial_printf("display.width() = %d\n", display.width());
    left = timeRect.p2.x + (display.width() - timeRect.p2.x - textWidth) / 2;
    right = left + textWidth;
    Serial_printf("left, right = %d, %d\n", left, right);
    rect = {left, 0, right, 8};
    display.setColor(COLOR_BLACK);
    display.fillRect(rect);
    display.setColor(COLOR_WHITE);
    display.printFixed(rect.p1.x, rect.p1.y + 2, "OUT");

    rect = {0, timeRect.p2.y + 1, display.width(), timeRect.p2.y + 11};
    display.setColor(COLOR_BLACK);
    display.fillRect(rect);
    display.setColor(COLOR_WHITE);
    display.setFreeFont(free_calibri11x12);
    display.printFixed(rect.p1.x, rect.p1.y, label0);
    textWidth = display.getFont().getTextSize(label1);
    rect = {display.width() - textWidth, rect.p1.y, display.width(), rect.p2.y};
    display.printFixed(rect.p1.x, rect.p1.y, label1);

    /*

        rect = {rect.p1.x, rect.p2.y + 1, rect.p2.x, rect.p2.y + 1 + 11};
        display.setColor(COLOR_BLACK);
        display.fillRect(rect);

        display.setColor(COLOR_WHITE);
        display.setFixedFont(free_calibri11x12_latin);
        display.printFixed(rect.p1.x, rect.p1.y, label0, STYLE_NORMAL);
        display.printFixed(rect.p2.x - display.getFont().getTextSize(label1), rect.p1.y, label1, STYLE_NORMAL);
      */
  }

  abo_loop();

  // if (debounce(buttonRed) && buttonRed.value) {
  // }

  // if (debounce(buttonYellow) && buttonYellow.value) {
  //   sbsm_trigger(kTriggerSelectInputAnalog);
  //   sbsm_trigger(kTriggerSelectOutputMonitor);
  // }

#if DEBUG_COLOR_DRAW
  if (debounce(button4) && !button4.value) {
    x--;
  }
  if (debounce(button5) && !button5.value) {
    x++;
  }
  if (debounce(button6) && !button6.value) {
    y--;
  }
  if (debounce(button7) && !button7.value) {
    y++;
  }
  x = x % (DISPLAY_WIDTH - 8);
  y = y % (display.height() - 8);
#endif
  dt = micros() - debug_ts;
  if (dt > DEBUG_INTERVAL) {
    // abi_debug();
    // abo_debug();

    // Serial_printf("Avg Tick: %dµs, (x, y) = (%d, %d)\n", (int)(avgTick), display.rect().p2.x, display.rect().p2.y);
    ser.println(time);

    // i2cScan();
    hydra_loop();
    // printRTCTime();
    // printRTCTemperature();

#if DEBUG_COLOR_DRAW
    NanoRect rect = {x, y, x + 8, y + 8};
    display.setColor(ts);
    display.fillRect(rect);
#endif

    debug_ts = micros();
  }
}

/*

keyboard-matrix style input

  if (debounce(buttonA)) {
    if (buttonA.value) {
      read |= 1;
    } else {
      clear |= 1;
    }
    // Serial_printf("A) read/clear: %d/%d\n", read, clear);
  }

  if (debounce(buttonB)) {
    if (buttonB.value) {
      read |= 2;
    } else {
      clear |= 2;
    }
    // Serial_printf("B) read/clear: %d/%d\n", read, clear);
  }

  // all downs have ups
  if (read && read == clear) {
    Serial_printf("TSU: %d\n", read);
    read = clear = 0;
  }

*/