#include "debounce.h"

#include <arduino.h>

bool debounce(ButtonState &buttonState, bool raw) {
  // read the state of the switch into a local variable:
  int reading = !digitalRead(buttonState.pin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != buttonState.last) {
    // reset the debouncing timer
    buttonState.last = reading;
    buttonState.debounce = millis();
  }
  int dt = (millis() - buttonState.debounce);
  if (dt > raw ? 1 : DEBOUNCE_INTERVAL) {
    //    char str[64];
    //    sprintf(str, "dt = %d", dt);
    //    Serial.println(str);
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState.value) {
      buttonState.value = reading;
      return true;
    }
  }
  return false;
}
