#include "telemetry.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>

SignalNormalizer::SignalNormalizer(Calibration calibration) : calibration_(std::move(calibration)) {
    if (calibration_.rawMin >= calibration_.rawMax || calibration_.valueMin >= calibration_.valueMax || calibration_.unit.empty())
        throw std::invalid_argument("invalid_calibration");
}

TelemetryFrame SignalNormalizer::normalize(std::string deviceId, std::string sensorId, std::uint64_t sequence,
                                           std::uint64_t timestampMs, float raw) {
    if (deviceId.empty() || sensorId.empty() || !std::isfinite(raw) || timestampMs == 0)
        throw std::invalid_argument("invalid_sensor_reading");
    if (hasSequence_ && sequence <= lastSequence_)
        throw std::invalid_argument("replayed_sequence");
    lastSequence_ = sequence;
    hasSequence_ = true;
    const bool inRange = raw >= calibration_.rawMin && raw <= calibration_.rawMax;
    const float ratio = (raw - calibration_.rawMin) / (calibration_.rawMax - calibration_.rawMin);
    const float value = calibration_.valueMin + ratio * (calibration_.valueMax - calibration_.valueMin);
    return {std::move(deviceId), std::move(sensorId), sequence, timestampMs, raw, value, calibration_.unit,
            inRange ? SignalQuality::Good : SignalQuality::Bad};
}

TelemetryBuffer::TelemetryBuffer(std::size_t capacity) : capacity_(capacity) {
    if (capacity_ == 0)
        throw std::invalid_argument("buffer_capacity_required");
}

void TelemetryBuffer::push(TelemetryFrame frame) {
    if (frames_.size() == capacity_) {
        frames_.pop_front();
        ++dropped_;
    }
    frames_.push_back(std::move(frame));
}

std::optional<TelemetryFrame> TelemetryBuffer::pop() {
    if (frames_.empty())
        return std::nullopt;
    auto frame = std::move(frames_.front());
    frames_.pop_front();
    return frame;
}

std::size_t TelemetryBuffer::size() const {
    return frames_.size();
}

std::uint64_t TelemetryBuffer::dropped() const {
    return dropped_;
}

std::string qualityName(SignalQuality quality) {
    switch (quality) {
        case SignalQuality::Good: return "good";
        case SignalQuality::Uncertain: return "uncertain";
        case SignalQuality::Bad: return "bad";
    }
    return "bad";
}

std::string serialize(const TelemetryFrame& frame) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(3)
           << "{\"device\":\"" << frame.deviceId << "\",\"sensor\":\"" << frame.sensorId
           << "\",\"sequence\":" << frame.sequence << ",\"timestamp_ms\":" << frame.timestampMs
           << ",\"raw\":" << frame.raw << ",\"value\":" << frame.value
           << ",\"unit\":\"" << frame.unit << "\",\"quality\":\"" << qualityName(frame.quality) << "\"}";
    return stream.str();
}
