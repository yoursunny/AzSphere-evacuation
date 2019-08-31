#include "D6T.hpp"

namespace d6t {

static constexpr uint8_t I2C_ADDR = 0x0A;
static constexpr uint8_t CMD_READ = 0x4C;

Sensor::Sensor(TwoWire& wire)
  : m_wire(wire)
{
}

bool
Sensor::begin()
{
  m_wire.beginTransmission(I2C_ADDR);
  return m_wire.endTransmission() == 0;
}

bool
Sensor::readImpl(uint8_t raw[], size_t sz)
{
  m_wire.beginTransmission(I2C_ADDR);
  m_wire.write(CMD_READ);
  m_wire.endTransmission(false);
  size_t n = m_wire.requestFrom(I2C_ADDR, sz);
  m_wire.readBytes(raw, sz);
  return n == sz;
}

} // namespace d6t
