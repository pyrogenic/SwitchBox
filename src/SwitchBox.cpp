/*
  DigitalReadSerial

  Reads a digital input on pin 2, prints the result to the Serial Monitor

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/DigitalReadSerial
*/

#include "SwitchBox.h"
#include "Debounce.h"
#include "DebugLine.h"
#include "RotaryEncoder.h"
#include "SwitchBoxStateMachine.h"
#include <lcdgfx.h>

#ifdef WHITE_OLED
DisplaySSD1306_128x64_I2C display(-1);
#else
DisplaySSD1331_96x64x16_SPI display(-1);
#endif

SAppMenu menu;
#define MENU_ITEM_COUNT (kInteractiveTriggerCount)
char *menuItems[MENU_ITEM_COUNT] = {0};
//#define BAR_TOP (58)
NanoRect menuRect = {0, 16, 128, 64};

ButtonState buttonA = {BUTTON_PIN_A};
ButtonState buttonB = {BUTTON_PIN_B};

int read = 0;
int clear = 0;

// the setup routine runs once when you press reset
RotaryState rotaryState;
void setup() {
  display.begin();
  display.clear();

  pinMode(BUTTON_PIN_A, INPUT);
  pinMode(BUTTON_PIN_B, INPUT);

  pinMode(RELAY_INPUT, OUTPUT);
  pinMode(RELAY_MONITOR, OUTPUT);
  pinMode(RELAY_AMP, OUTPUT);
  pinMode(RELAY_SUB, OUTPUT);

  rotaryState.pinA.pin = ROTARY_PIN_A;
  rotaryState.pinB.pin = ROTARY_PIN_B;
  rotaryState.pinSwitch.pin = ROTARY_PIN_BUTTON;
  rotary_setup(rotaryState);

  debounce(buttonA);
  debounce(buttonB);

  sbsm_setup();

  for (size_t i = 0; i < MENU_ITEM_COUNT; i++) {
    menuItems[i] = (char *)calloc(64 + 1, sizeof(char));
    strcpy(menuItems[i], sbsm_trigger_name((Trigger)i).c_str());
  }

  display.createMenu(&menu, const_cast<const char **>(menuItems), sizeof(menuItems) / sizeof(char *), menuRect);
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
int nextTick = 0;
void loop() {
  // digitalWrite(RELAY_INPUT, HIGH);
  // delay(1000);
  // digitalWrite(RELAY_INPUT, LOW);
  // digitalWrite(RELAY_MONITOR, HIGH);
  // delay(1000);
  // digitalWrite(RELAY_MONITOR, LOW);
  // digitalWrite(RELAY_AMP, HIGH);
  // delay(1000);
  // digitalWrite(RELAY_AMP, LOW);
  // digitalWrite(RELAY_SUB, HIGH);
  // delay(1000);
  // digitalWrite(RELAY_SUB, LOW);

  // if (millis() > nextTick) {
  //   Serial.printf("...read/clear: %d/%d  last/value: A:%d/%d, B:%d/%d\n", read, clear, buttonA.last, buttonA.value, buttonB.last, buttonB.value);
  //   nextTick = millis() + 1000;
  // }

  if (debounce(buttonA)) {
    if (buttonA.value) {
      read |= 1;
    } else {
      clear |= 1;
    }
    // Serial.printf("A) read/clear: %d/%d\n", read, clear);
  }

  if (debounce(buttonB)) {
    if (buttonB.value) {
      read |= 2;
    } else {
      clear |= 2;
    }
    // Serial.printf("B) read/clear: %d/%d\n", read, clear);
  }

  // all downs have ups
  if (read && read == clear) {
    Serial.printf("TSU: %d\n", read);
    read = clear = 0;
  }

  // if (debounce_steady(buttonA) && debounce_steady(buttonB) ) {
  //   read = (1 & buttonA.value) | (2 & buttonB.value);
  //   clear = ~(1 & buttonA.value) | (2 & buttonB.value);
  //   Serial.printf("!! read/clear: %d/%d !!\n", read, clear);
  // }

  sbsm_loop();
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
  // if (menuDirty) {
  // for (int i = 0; i < MENU_ITEM_COUNT; ++i) {
  //   char c = i == activeOutput ? '*' : ' ';
  //   menuItems[i][0] = c;
  // }
  //    NanoRect rect = {0, 0, 128, 32};
  //    display.setColor(BLACK);
  //    display.fillRect(rect);
  //    display.setFixedFont( ssd1306xled_font8x16);
  //    int len = strlen(debug_get());
  //    int x = (16 - len) / 2;
  //    display.printFixed(x * 8, 0, debug_get(), STYLE_NORMAL);
  //   display.setFixedFont(ssd1306xled_font6x8);
  //   display.showMenuSmooth(&menu);
  // } else
  if (menuUpdate) {
    display.setFixedFont(ssd1306xled_font6x8);
    display.updateMenuSmooth(&menu);
  }
  menuUpdate = false;
  char row0[33] = {0};
  char row1[33] = {0};
  snprintf(row0, 32, "%s > %s", sbsm_input_label().c_str(), sbsm_output_label().c_str());
  snprintf(row1, 32, "%s", sbsm_subwoofer_label().c_str());
  if (strcmp(lastRow0, row0)) {
    strcpy(lastRow0, row0);
    NanoRect rect = {0, 0, 128, 8};
    display.setColor(BLACK);
    display.fillRect(rect);
    display.setFixedFont(ssd1306xled_font6x8);
    display.printFixed(0, 0, row0, STYLE_NORMAL);
  }
  if (strcmp(lastRow1, row1)) {
    strcpy(lastRow1, row1);
    NanoRect rect = {0, 9, 128, 14};
    display.setColor(BLACK);
    display.fillRect(rect);
    display.setFixedFont(ssd1306xled_font5x7);
    display.printFixed(strlen(row1) * 6, 9, row1, STYLE_NORMAL);
  }
  // if (strcmp(lastDebug, debug_get())) {
  //   strcpy(lastDebug, debug_get());
  //   NanoRect rect = {0, 8, 128, 16};
  //   display.setColor(BLACK);
  //   display.fillRect(rect);
  //   display.setFixedFont(ssd1306xled_font6x8);
  //   display.printFixed(0, 8, debug_get(), STYLE_NORMAL);
  // }
}
