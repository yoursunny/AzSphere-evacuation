#include "DeviceClient.hpp"
#include "cacerts.hpp"

#include <azureiot/iothub_device_client_ll.h>
#include <azureiot/iothubtransportmqtt.h>

#include <Arduino.h>

#define HDL(self) (static_cast<IOTHUB_DEVICE_CLIENT_LL_HANDLE>(self->m_hdl))

#define CHECK_OK(expr) \
  do { \
    if ((expr) != IOTHUB_CLIENT_OK) { \
      return false; \
    } \
  } while(false)

namespace azure_iot {

class DeviceClient::Impl
{
public:
  static void
  updateConnStatus(IOTHUB_CLIENT_CONNECTION_STATUS result,
                   IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason,
                   void* ctx)
  {
    DeviceClient* self = static_cast<DeviceClient*>(ctx);
    self->m_isConnected = result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED;
    Serial.printf("reason=%d\n", reason);
  }

private:
  Impl() = delete;
};

DeviceClient::~DeviceClient()
{
  end();
}

bool
DeviceClient::begin(const char* connstr)
{
  end();
  if (!beginImpl(connstr)) {
    end();
    return false;
  }
  return true;
}

bool
DeviceClient::beginImpl(const char* connstr)
{
  m_hdl = IoTHubDeviceClient_LL_CreateFromConnectionString(connstr, MQTT_Protocol);
  if (HDL(this) == nullptr) {
    return false;
  }
  CHECK_OK(IoTHubDeviceClient_LL_SetOption(HDL(this),
           "TrustedCerts", CACERTS));
  CHECK_OK(IoTHubDeviceClient_LL_SetConnectionStatusCallback(HDL(this),
           Impl::updateConnStatus, this));
  CHECK_OK(IoTHubDeviceClient_LL_SetRetryPolicy(HDL(this),
           IOTHUB_CLIENT_RETRY_INTERVAL, 0));
  return true;
}

bool
DeviceClient::end()
{
  if (HDL(this) == nullptr) {
    return true;
  }
  IoTHubDeviceClient_LL_Destroy(HDL(this));
  m_hdl = nullptr;
  m_isConnected = false;
  return true;
}

void
DeviceClient::loop()
{
  if (HDL(this) == nullptr) {
    return;
  }
  IoTHubDeviceClient_LL_DoWork(HDL(this));
}

bool
DeviceClient::send(const char* message)
{
  if (HDL(this) == nullptr) {
    return false;
  }
  auto m = IoTHubMessage_CreateFromString(message);
  if (m == nullptr) {
    return false;
  }
  auto res = IoTHubDeviceClient_LL_SendEventAsync(HDL(this), m, nullptr, nullptr);
  IoTHubMessage_Destroy(m);
  CHECK_OK(res);
  return true;
}

} // namespace azure_iot
