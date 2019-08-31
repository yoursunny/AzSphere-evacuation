#include <Arduino.h>
#include <Wire.h>

#include <D6T.hpp>

d6t::Sensor d6tSensor;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Wire.begin();
  Wire.changeBufferLimits(35, 1);

  if (!d6tSensor.begin()) {
    Serial.println("d6t not detected");
  }
}

void loop()
{
  delay(1000);

  d6t::Output44 output;
  if (!d6tSensor.read(output)) {
    Serial.println("d6t read error");
    return;
  }

  d6t::Temperature ref = output.getRef();
  Serial.println(ref);
  for (int x = 0; x < 4; ++x) {
    for (int y = 0; y < 4; ++y) {
      Serial.printf("%4d ", output.getPixel(x, y));
    }
    Serial.println();
  }
  for (int x = 0; x < 4; ++x) {
    for (int y = 0; y < 4; ++y) {
      Serial.print(output.getPixel(x, y) >= ref ? "x" : " ");
    }
    Serial.println();
  }
  Serial.println();
}
