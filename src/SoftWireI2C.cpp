/*
    MIT License

    Copyright (c) 2016-2019, Alexey Dynda

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
#include "SoftWireI2C.h"

static uint8_t s_bytesWritten = 0;

SoftWireI2C::SoftWireI2C(int8_t scl, int8_t sda, uint8_t sa, int freq) : m_wire(SoftWire(scl, sda)), m_sa(sa) {
  m_wire.setClock(freq);
}

SoftWireI2C::~SoftWireI2C() {
}

void SoftWireI2C::begin() {
  m_wire.begin();
}

void SoftWireI2C::end() {
  m_wire.end();
}

void SoftWireI2C::start() {
  m_wire.beginTransmission(m_sa);
  s_bytesWritten = 0;
}

void SoftWireI2C::stop() {
  m_wire.endTransmission();
}

void SoftWireI2C::send(uint8_t data) {
  if (s_bytesWritten == 0)
    m_mode = data;
  if (m_wire.write(data) != 0) {
    s_bytesWritten++;
    return;
  }
  {
    stop();
    start();
    send(m_mode);
    /* Commands never require many bytes. Thus assume that user tries to send data */
  }
  m_wire.write(data);
  s_bytesWritten++;
}

void SoftWireI2C::sendBuffer(const uint8_t *buffer, uint16_t size) {
  while (size--) {
    send(*buffer);
    buffer++;
  }
}
