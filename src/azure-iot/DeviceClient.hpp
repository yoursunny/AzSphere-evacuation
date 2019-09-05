#ifndef AZURE_IOT_DEVICE_CLIENT_HPP
#define AZURE_IOT_DEVICE_CLIENT_HPP

namespace azure_iot {

class DeviceClient
{
public:
  ~DeviceClient();

  bool
  begin(const char* connstr);

  bool
  end();

  void
  loop();

  bool
  isConnected() const
  {
    return m_isConnected;
  }

  bool
  send(const char* message);

private:
  bool
  beginImpl(const char* connstr);

private:
  class Impl;
  friend Impl;

  void* m_hdl = nullptr;
  bool m_isConnected = false;
};

} // namespace azure_iot

#endif // AZURE_IOT_DEVICE_CLIENT_HPP
