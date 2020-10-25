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
#include "RotaryEncoder.h"
#include "SwitchBoxStateMachine.h"
#include <lcdgfx.h>

#define WHITE_OLED 1
#define COLOR_OLED 2

#define DISPLAY_TYPE COLOR_OLED

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
DisplaySSD1331_96x64x8_SPI display(SSD1331_RES, spiConfig); // 8, {-1, 10, 9});
#define DISPLAY_WIDTH (96)
#define COLOR_BLACK RGB_COLOR8(0, 0, 0)
#define COLOR_RED RGB_COLOR8(0xFFFF, 0, 0)
#define COLOR_GREEN RGB_COLOR8(0, 0xFFFF, 0)
#define COLOR_BLUE RGB_COLOR8(0, 0, 0xFFFF)
#define COLOR_WHITE RGB_COLOR8(0xFFFF, 0xFFFF, 0xFFFF)
#else
#error No display type specified
#endif

SAppMenu menu;
#define MENU_ITEM_COUNT (kInteractiveTriggerCount)
char *menuItems[MENU_ITEM_COUNT] = {0};

NanoRect menuRect() {
  //{0, 16, DISPLAY_WIDTH, DISPLAY_HEIGHT};
  auto result = display.rect();
  result.p1.y += 16;
  return result;
}

ButtonState button7 = {kABIPinShiftRegister, kSinKeyH};
ButtonState button6 = {kABIPinShiftRegister, kSinKeyG};
ButtonState button5 = {kABIPinShiftRegister, kSinKeyF};
ButtonState button4 = {kABIPinShiftRegister, kSinKeyE};

#ifdef USE_ROTARY_INPUT
RotaryState rotaryState;
#else
ButtonState buttonUp = {kABIPinShiftRegister, kSinUp};
ButtonState buttonDown = {kABIPinShiftRegister, kSinDown};
ButtonState buttonEnter = {kABIPinShiftRegister, kSinEnter};
#endif

void setup() {
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

#ifdef USE_ROTARY_INPUT
  rotaryState.pinA.pin = {kABIPinShiftRegister, kSinRotaryA};
  rotaryState.pinB.pin = {kABIPinShiftRegister, kSinRotaryB};
  rotaryState.pinSwitch.pin = {kABIPinShiftRegister, kSinRotaryButton};
  rotary_setup(rotaryState);
#else
  debounce(buttonUp);
  debounce(buttonDown);
  debounce(buttonEnter);
#endif

  debounce(button4);
  debounce(button5);
  debounce(button6);
  debounce(button7);

  sbsm_setup();

  for (size_t i = 0; i < MENU_ITEM_COUNT; i++) {
    menuItems[i] = (char *)calloc(64 + 1, sizeof(char));
    strcpy(menuItems[i], sbsm_trigger_name((Trigger)i).c_str());
  }

  /*
    NanoRect rect = menuRect();
    display.createMenu(&menu, const_cast<const char **>(menuItems), sizeof(menuItems) / sizeof(char *), rect);
    display.setFreeFont(free_calibri11x12_latin);
    display.setFontSpacing(1);
    display.showMenuSmooth(&menu);
  */
  display.createMenu(&menu, const_cast<const char **>(menuItems), sizeof(menuItems) / sizeof(char *), menuRect());
  display.setFixedFont(ssd1306xled_font6x8);
  display.showMenuSmooth(&menu);

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

int encoderPosCount = 0;
// bool menuDirty = true;
bool menuUpdate = true;

char lastDebug[256] = {0};
char lastRow0[256] = {0};
char lastRow1[256] = {0};
// the loop routine runs over and over again forever:
float avgTick = 0;
unsigned long ts = micros();
unsigned long debug_ts = micros();

#define CHAR_WIDTH_LG (DISPLAY_WIDTH / 6)
#define CHAR_WIDTH_SM (DISPLAY_WIDTH / 5)

lcdint_t x = 0;
lcdint_t y = 0;

#define TICK_LENGTH 100
#define DEBUG_INTERVAL 1000000
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

#ifdef USE_ROTARY_INPUT
  RotaryAction action = rotary_loop(rotaryState);
  if (action) {
    switch (action) {
    case kRotaryActionClick: {
      Trigger event = (Trigger)menu.selection;
      sbsm_trigger(event);
      Serial.print("Select: ");
      Serial.println(triggerNames.find(event)->second.c_str());
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
#else
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
    display.setFixedFont(ssd1306xled_font6x8);
    display.updateMenuSmooth(&menu);
  }

  char row0[CHAR_WIDTH_LG + 1] = {0};
  char row1[CHAR_WIDTH_SM + 1] = {0};
  snprintf(row0, CHAR_WIDTH_LG, "%s > %s", sbsm_input_label().c_str(), sbsm_output_label().c_str());
  snprintf(row1, CHAR_WIDTH_SM, "%s", sbsm_subwoofer_label().c_str());
  if (strcmp(lastRow0, row0)) {
    strcpy(lastRow0, row0);
    NanoRect rect = {0, 0, DISPLAY_WIDTH, 8};
    display.setColor(COLOR_BLACK);
    display.fillRect(rect);
    display.setColor(COLOR_RED);
    display.setFixedFont(ssd1306xled_font6x8);
    display.printFixed(0, 0, row0, STYLE_NORMAL);
  }

  if (strcmp(lastRow1, row1)) {
    strcpy(lastRow1, row1);
    NanoRect rect = {0, 9, DISPLAY_WIDTH, 14};
    display.setColor(COLOR_BLACK);
    display.fillRect(rect);
    display.setColor(COLOR_GREEN);
    display.setFixedFont(ssd1306xled_font5x7);
    display.printFixed(DISPLAY_WIDTH - strlen(row1) * 6, 9, row1, STYLE_NORMAL);
  }

  /*
    // menuUpdate = false;

    // if (strcmp(lastDebug, debug_get())) {
    //   strcpy(lastDebug, debug_get());
    //   NanoRect rect = {0, 8, DISPLAY_WIDTH, 16};
    //   display.setColor(COLOR_BLACK);
    //   display.fillRect(rect);
    //   display.setColor(COLOR_WHITE);
    //   display.setFixedFont(ssd1306xled_font6x8);
    //   display.printFixed(0, 8, debug_get(), STYLE_NORMAL);
    // }


    */

  abo_loop();

#ifdef DEBUG_DRAW
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
    abi_debug();
    abo_debug();
    debug_ts = micros();
    Serial_printf("Avg Tick: %dµs, (x, y) = (%d, %d)\n", (int)(avgTick), display.rect().p2.x, display.rect().p2.y);
#ifdef DEBUG_DRAW
    NanoRect rect = {x, y, x + 8, y + 8};
    display.setColor(ts);
    display.fillRect(rect);
#endif
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