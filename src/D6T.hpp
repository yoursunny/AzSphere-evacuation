#ifndef D6T_HPP
#define D6T_HPP

#include <Wire.h>

namespace d6t {

/** \brief Surface temperature in 1/100 of degrees Celsius.
 */
using Temperature = int16_t;

/** \brief Measurement output.
 *  \tparam N number of pixels.
 */
template<int N>
class Output
{
public:
  Output()
  {
    memset(m_raw, 0, sizeof(m_raw));
  }

  /** \brief Get reference temperature (PTAT).
   */
  Temperature
  getRef() const
  {
    return getValueAt(0);
  }

  /** \brief Get measured value at pixel i (P[i]).
   */
  Temperature
  getPixel(int i) const
  {
    if (i >= N) {
      return INT16_MIN;
    }
    return getValueAt(2 + 2 * i);
  }

  /** \brief Count how many pixels are hotter than reference.
   */
  int
  countHots() const
  {
    Temperature ref = this->getRef();
    int result = 0;
    for (int i = 0; i < N; ++i) {
      if (this->getPixel(i) > ref) {
        ++result;
      }
    }
    return result;
  }

private:
  Temperature
  getValueAt(int offset) const
  {
    uint16_t v = (static_cast<uint16_t>(m_raw[offset + 1]) << 8) | m_raw[offset];
    int16_t t = 0;
    memcpy(&t, &v, sizeof(t));
    return t;
  }

private:
  uint8_t m_raw[2 + 2 * N + 1];
  friend class Sensor;
};

/** \brief Measurement output of D6T-1A.
 */
using Output1 = Output<1>;

/** \brief Measurement output of D6T-8L.
 */
using Output8 = Output<8>;

/** \brief Measurement output of D6T-44L.
 */
class Output44 : public Output<16>
{
public:
  using Output::getPixel;

  /** \brief Get measured value at pixel row=x col=y.
   */
  Temperature
  getPixel(int x, int y) const
  {
    return getPixel(4 * x + y);
  }
};

/** \brief D6T sensor series.
 */
class Sensor
{
public:
  explicit
  Sensor(TwoWire& wire = Wire);

  bool
  begin();

  template<typename OutputT>
  bool
  read(OutputT& output)
  {
    return readImpl(output.m_raw, sizeof(output.m_raw));
  }

private:
  bool
  readImpl(uint8_t raw[], size_t sz);

private:
  TwoWire& m_wire;
};

} // namespace d6t

#endif // D6T_HPP
