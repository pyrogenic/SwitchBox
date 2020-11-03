#include "RealTimeClock.h"

#include "DebugLine.h"

void RealTimeClock::setup() {
#ifdef ARDUINO_ARCH_ESP8266
  URTCLIB_WIRE.begin(0, 2); // D3 and D4 on ESP8266
#else
  URTCLIB_WIRE.begin();
#endif

  rtc.set_rtc_address(0x68);
  rtc.set_model(URTCLIB_MODEL_DS3231);
  rtc.refresh();
  Serial_printf("Lost power? %s\n", rtc.lostPower() ? "Yes" : "No");
  Serial_printf("Year: 20%02d\n", rtc.year());
}

void RealTimeClock::setHour(uint8_t hour) {
  uint8_t second = rtc.second();
  uint8_t minute = rtc.minute();
  uint8_t dayOfWeek = rtc.dayOfWeek();
  uint8_t day = rtc.day();
  uint8_t month = rtc.month();
  uint8_t year = rtc.year();
  rtc.set(second, minute, hour, dayOfWeek, day, month, year);
}

void RealTimeClock::moveHourHand(int8_t d) {
  uint8_t second = rtc.second();
  uint8_t minute = rtc.minute();
  uint8_t hour = rtc.hour();
  uint8_t dayOfWeek = rtc.dayOfWeek();
  uint8_t day = rtc.day();
  uint8_t month = rtc.month();
  uint8_t year = rtc.year();
  hour += d;
  rtc.set(second, minute, hour, dayOfWeek, day, month, year);
}

void RealTimeClock::fallBack() {
  moveHourHand(-1);
}

void RealTimeClock::springForward() {
  moveHourHand(1);
}

void RealTimeClock::format(char *buffer, size_t bufferSize, const TimeFormat &options, bool refresh) {
  if (refresh) {
    rtc.refresh();
  }
  format(buffer, bufferSize, options);
}

void RealTimeClock::format(char *buffer, size_t bufferSize, const TimeFormat &options) const {
  const char *amOrPm;
  uRTCLib rtc = const_cast<uRTCLib &>(this->rtc);
  auto hour = rtc.hour();
  if (options.amPm != kTF_24H) {
    if (hour >= 12) {
      amOrPm = " pm";
      if (hour > 12) {
        hour -= 12;
      }
    } else {
      amOrPm = " am";
      if (hour == 0) {
        hour = 12;
      }
    }
  } else {
    amOrPm = "";
  }
  if (options.amPm != kTF_amPm) {
    amOrPm = "";
  }
  if (options.seconds) {
    snprintf_P(buffer, 13, options.pad ? PSTR("%02u:%02u:%02u%s") : PSTR("%u:%02u:%02u%s"), hour, rtc.minute(), rtc.second(), amOrPm);
  } else {
    snprintf_P(buffer, 13, options.pad ? PSTR("%02u:%02u%s") : PSTR("%u:%02u%s"), hour, rtc.minute(), amOrPm);
  }
}