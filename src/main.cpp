#include <Arduino.h>

#include "telemetry.hpp"

class TelemetrySink {
public:
    virtual ~TelemetrySink() = default;
    virtual bool publish(const TelemetryFrame& frame) = 0;
};

class SerialTelemetrySink final : public TelemetrySink {
public:
    bool publish(const TelemetryFrame& frame) override {
        Serial.println(serialize(frame).c_str());
        return true;
    }
};

SignalNormalizer normalizer({0.0f, 4095.0f, 0.0f, 100.0f, "percent"});
TelemetryBuffer buffer(64);
SerialTelemetrySink sink;
std::uint64_t sequenceNumber = 0;
std::uint64_t lastPublish = 0;

void setup() {
    Serial.begin(115200);
    analogReadResolution(12);
}

void loop() {
    const auto now = static_cast<std::uint64_t>(millis());
    const auto raw = static_cast<float>(analogRead(34));
    buffer.push(normalizer.normalize("edge-01", "tank-level-01", ++sequenceNumber, now, raw));
    if (now - lastPublish >= 1000) {
        while (auto frame = buffer.pop())
            sink.publish(*frame);
        lastPublish = now;
    }
    delay(250);
}
