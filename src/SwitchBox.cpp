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
#include "SwitchBoxStateMachine.h"
#include "Temperature.h"
#include <SoftwareSerial.h>
#include <Wire.h>
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

#define SET_MENU_FONT()                      \
  display.setFixedFont(ssd1306xled_font5x7); \
  display.setColor(COLOR_MENU_TEXT)

SAppMenu menu;
#define MENU_ITEM_COUNT (kInteractiveTriggerCount)
char *menuItems[MENU_ITEM_COUNT] = {0};

NanoRect menuRect() {
  auto result = display.rect();
  result.p1.y = result.p2.y - 42;
  return result;
}

ButtonState buttonYellow = {kABIPinShiftRegister, kSinKeyH};
ButtonState buttonBlue = {kABIPinShiftRegister, kSinKeyG};
ButtonState buttonGreen = {kABIPinShiftRegister, kSinKeyF};
ButtonState buttonRed = {kABIPinShiftRegister, kSinKeyE};

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
SoftwareSerial ser(A1, A2);

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

  debounce(buttonRed);
  debounce(buttonGreen);
  debounce(buttonBlue);
  debounce(buttonYellow);

  sbsm_setup();

  for (size_t i = 0; i < MENU_ITEM_COUNT; i++) {
    menuItems[i] = (char *)calloc(64 + 1, sizeof(char));
    strcpy(menuItems[i], sbsm_trigger_name((Trigger)i).c_str());
  }

  display.createMenu(&menu, const_cast<const char **>(menuItems), sizeof(menuItems) / sizeof(char *), menuRect());
  SET_MENU_FONT();
#if MENU_MODE_SMOOTH
  display.showMenuSmooth(&menu);
#else
  display.showMenu(&menu);
#endif
}

int encoderPosCount = 0;
// bool menuDirty = true;

#define LABEL_STRING_BUFFER_SIZE (42)
char lastLabel0[LABEL_STRING_BUFFER_SIZE] = {0};
char lastLabel1[LABEL_STRING_BUFFER_SIZE] = {0};
char lastLabel2[LABEL_STRING_BUFFER_SIZE] = {0};
char lastLabel3[LABEL_STRING_BUFFER_SIZE] = {0};
char lastTime[TIME_STRING_BUFFER_SIZE] = {0};

// the loop routine runs over and over again forever:
float avgTick = 0;
unsigned long ts = micros();
unsigned long debug_ts = micros();

#define CHAR_WIDTH_LG (DISPLAY_WIDTH / 6)
#define CHAR_WIDTH_SM (DISPLAY_WIDTH / 5)

lcdint_t x = 0;
lcdint_t y = 0;

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
      ser.write(triggerName.c_str());
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
    Serial.println(triggerNames.find(event)->second.c_str());
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

  snprintf(label0, LABEL_STRING_BUFFER_SIZE, "%s", sbsm_input_label().c_str());
  snprintf(label1, LABEL_STRING_BUFFER_SIZE, "%s", sbsm_output_label().c_str());
  snprintf(label2, LABEL_STRING_BUFFER_SIZE, "%s", sbsm_subwoofer_label().c_str());
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

  if (debounce(buttonRed) && buttonRed.value) {
  }

  if (debounce(buttonYellow) && buttonYellow.value) {
    sbsm_trigger(kTriggerSelectInputAnalog);
    sbsm_trigger(kTriggerSelectOutputMonitor);
  }

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