#include <Arduino.h>

#include "telemetry.hpp"

class TelemetrySink {
public:
  virtual ~TelemetrySink() = default;
  virtual bool publish(const TelemetryFrame &frame) = 0;
};

class SerialTelemetrySink final : public TelemetrySink {
public:
  bool publish(const TelemetryFrame &frame) override {
    const auto data = serialize(frame) + "\n";
    return Serial.write(reinterpret_cast<const uint8_t *>(data.data()),
                        data.size()) == data.size();
  }
};

SignalNormalizer normalizer({0.0f, 4095.0f, 0.0f, 100.0f, "percent"});
TelemetryBuffer buffer(64);
SerialTelemetrySink sink;
std::uint64_t sequenceNumber = 0;
std::uint64_t lastPublish = 0;
std::uint64_t lastSample = 0;
std::uint32_t previousTick = 0;
std::uint64_t uptime = 1;
std::string deviceId;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  deviceId = "esp32-" + std::to_string(ESP.getEfuseMac()) + "-" +
             std::to_string(esp_random());
}

void loop() {
  const auto tick = millis();
  uptime += static_cast<std::uint32_t>(tick - previousTick);
  previousTick = tick;
  const auto now = uptime;
  if (now - lastSample >= 250) {
    buffer.push(normalizer.normalize(deviceId, "tank-level-01",
                                     ++sequenceNumber, now,
                                     static_cast<float>(analogRead(34))));
    lastSample = now;
  }
  if (now - lastPublish >= 1000) {
    while (const auto *frame = buffer.peek()) {
      if (!sink.publish(*frame))
        break;
      buffer.acknowledge(frame->sequence);
    }
    lastPublish = now;
  }
}
