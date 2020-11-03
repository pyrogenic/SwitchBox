#pragma once

#include <Arduino.h>
#include <uRTCLib.h>

// HH:MM:SS am
#define TIME_STRING_BUFFER_SIZE 12

typedef enum {
  kTF_24H,
  kTF_12H,
  kTF_amPm,
} HourFormat;

typedef struct TimeFormat {
  TimeFormat(bool seconds = false, HourFormat amPm = kTF_24H, bool pad = false) : seconds(seconds), amPm(amPm), pad(pad) {}
  bool seconds;
  HourFormat amPm;
  bool pad;
} tTimeFormat;

class RealTimeClock {
public:
  RealTimeClock(bool verbose);

  void setup();

  void setHour(uint8_t hour);
  void fallBack();
  void springForward();

  void format(char *buffer, size_t bufferSize, const TimeFormat &format, bool refresh);
  void format(char *buffer, size_t bufferSize, const TimeFormat &format) const;

private:
  void moveHourHand(int8_t d);

  uRTCLib rtc;
};
