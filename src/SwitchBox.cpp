/*
  DigitalReadSerial

  Reads a digital input on pin 2, prints the result to the Serial Monitor

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/DigitalReadSerial
*/

#include <lcdgfx.h>
#include "DebugLine.h"
#include "Debounce.h"
#include "RotaryEncoder.h"
#include "SwitchBox.h"

DisplaySSD1306_128x64_I2C display(-1); // This line is suitable fo

SAppMenu menu;
#define MENU_ITEM_COUNT (9)
char * menuItems[MENU_ITEM_COUNT] = {0};

#define BAR_TOP (58)

RotaryState rotaryState;
int activeOutput = 0;

// the setup routine runs once when you press reset:
void setup() {
  rotaryState.pinA.pin = ROTARY_PIN_A;
  rotaryState.pinB.pin = ROTARY_PIN_B;
  rotaryState.pinSwitch.pin = ROTARY_PIN_BUTTON;

  display.begin();
  display.clear();
  NanoRect menuRect = {0, 16, 128, BAR_TOP - 2};
  
  for (size_t i = 0; i < MENU_ITEM_COUNT; i++) {
    menuItems[i] = (char *)calloc(64 + 1, sizeof(char));
  }
  strcpy(menuItems[0], "  Analog");
  strcpy(menuItems[1], "  Digital");
  strcpy(menuItems[2], "  ");
  strcpy(menuItems[3], "  Loki");
  strcpy(menuItems[4], "  Valhalla");
  strcpy(menuItems[5], "  ");
  strcpy(menuItems[6], "  Speakers");
  strcpy(menuItems[7], "  Geshelli");
  
  display.createMenu( &menu, const_cast<const char **>(menuItems), sizeof(menuItems) / sizeof(char *), menuRect);

  rotary_setup(rotaryState);

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
}

int encoderPosCount = 0;
bool menuDirty = true;
bool menuUpdate = true;

char lastDebug[256] = {0};
// the loop routine runs over and over again forever:
void loop() {
  display.setFixedFont( ssd1306xled_font6x8 );
  display.showMenuSmooth(&menu);
  RotaryAction action = rotary_loop(rotaryState);
  if (action) {
    // debug_set("Action: %d %s", action,
    //           action == kRotaryActionWiddershinsUp ? "WiddershinsUp" :
    //           action == kRotaryActionWiddershinsDown ? "WiddershinsDown" :
    //           action == kRotaryActionNone ? "None" :
    //           action == kRotaryActionClockwiseDown ? "ClockwiseDown" :
    //           action == kRotaryActionClockwiseUp ? "ClockwiseUp" :
    //           action == kRotaryActionClick ? "Click" : "?");
    switch (action) {
      case kRotaryActionClick:
        if (activeOutput != menu.selection) {
          activeOutput = menu.selection;
          menuDirty = true;
        }
        break;
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
// 
  if (menuDirty) {
    for (int i = 0; i < MENU_ITEM_COUNT; ++i) {
      char c = i == activeOutput ? '*' : ' ';
      menuItems[i][0] = c;
    }
    //    NanoRect rect = {0, 0, 128, 32};
    //    display.setColor(BLACK);
    //    display.fillRect(rect);
    //    display.setFixedFont( ssd1306xled_font8x16);
    //    int len = strlen(debug_get());
    //    int x = (16 - len) / 2;
    //    display.printFixed(x * 8, 0, debug_get(), STYLE_NORMAL);
    display.setFixedFont( ssd1306xled_font6x8 );
    display.showMenuSmooth(&menu);
  } else if (menuUpdate) {
    display.setFixedFont( ssd1306xled_font6x8 );
    display.updateMenuSmooth(&menu);
  }

  menuDirty = false;
  menuUpdate = false;

  if (strcmp(lastDebug, debug_get())) {
    strcpy(lastDebug, debug_get());
    // NanoRect rect = {0, 0, 128, 16};
    // display.setColor(BLACK);
    // display.fillRect(rect);
    display.setFixedFont(ssd1306xled_font6x8);
    display.printFixed(0, 0, debug_get(), STYLE_NORMAL);
  }
  digitalWrite(RELAY_1, activeOutput == 1 ? HIGH : LOW);
  digitalWrite(RELAY_2, activeOutput == 2 ? HIGH : LOW);
}
