/*
    MIT License

    Copyright (c) 2018-2019, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

/*
 * @file hal/arduino/arduino_i2c.h LCDGFX ARDUINO Interface communication functions
 */

#pragma once

#include <SoftWire.h>

/**
 * Class implements i2c support via Wire library for Arduino platforms
 */
class SoftWireI2C {
public:
  /**
   * Creates i2c implementation instance for Arduino platform.
   * @param scl clock pin to use for i2c
   * @param sda data pin to use for i2c
   * @param sa i2c address of the device to control over i2c
   */
  SoftWireI2C(int8_t scl, int8_t sda, uint8_t sa, int freq);
  ~SoftWireI2C();

  /**
   * Initializes i2c interface
   */
  void begin();

  /**
   * Closes i2c interface
   */
  void end();

  /**
   * Starts communication with SSD1306 display.
   */
  void start();

  /**
   * Ends communication with SSD1306 display.
   */
  void stop();

  /**
   * Sends byte to SSD1306 device
   * @param data - byte to send
   */
  void send(uint8_t data);

  /**
   * @brief Sends bytes to SSD1306 device
   *
   * Sends bytes to SSD1306 device. This functions gives
   * ~ 30% performance increase than ssd1306_intf.send.
   *
   * @param buffer - bytes to send
   * @param size - number of bytes to send
   */
  void sendBuffer(const uint8_t *buffer, uint16_t size);

  /**
   * Sets i2c address for communication
   * This API is required for some led displays having multiple
   * i2c addresses for different types of data.
   *
   * @param addr i2c address to set (7 bits)
   */
  void setAddr(uint8_t addr) { m_sa = addr; }

private:
  SoftWire m_wire;
  uint8_t m_sa;
  uint8_t m_mode;
};
