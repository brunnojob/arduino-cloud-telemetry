#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <optional>
#include <string>

enum class SignalQuality { Good, Uncertain, Bad };

struct Calibration {
    float rawMin;
    float rawMax;
    float valueMin;
    float valueMax;
    std::string unit;
};

struct TelemetryFrame {
    std::string deviceId;
    std::string sensorId;
    std::uint64_t sequence;
    std::uint64_t timestampMs;
    float raw;
    float value;
    std::string unit;
    SignalQuality quality;
};

class SignalNormalizer {
public:
    explicit SignalNormalizer(Calibration calibration);
    TelemetryFrame normalize(std::string deviceId, std::string sensorId, std::uint64_t sequence,
                             std::uint64_t timestampMs, float raw);
private:
    Calibration calibration_;
    std::uint64_t lastSequence_ = 0;
    bool hasSequence_ = false;
};

class TelemetryBuffer {
public:
    explicit TelemetryBuffer(std::size_t capacity);
    void push(TelemetryFrame frame);
    std::optional<TelemetryFrame> pop();
    std::size_t size() const;
    std::uint64_t dropped() const;
private:
    std::size_t capacity_;
    std::deque<TelemetryFrame> frames_;
    std::uint64_t dropped_ = 0;
};

std::string serialize(const TelemetryFrame& frame);
std::string qualityName(SignalQuality quality);
