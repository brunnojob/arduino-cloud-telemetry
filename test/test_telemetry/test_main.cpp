#include <unity.h>
#include <stdexcept>

#include "telemetry.hpp"

void test_calibration_and_quality() {
    SignalNormalizer normalizer({0.0f, 4095.0f, 0.0f, 100.0f, "percent"});
    auto frame = normalizer.normalize("edge", "level", 1, 100, 2047.5f);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 50.0f, frame.value);
    TEST_ASSERT_EQUAL_STRING("good", qualityName(frame.quality).c_str());
    auto bad = normalizer.normalize("edge", "level", 2, 200, 4200.0f);
    TEST_ASSERT_EQUAL_STRING("bad", qualityName(bad.quality).c_str());
}

void test_replay_is_rejected() {
    SignalNormalizer normalizer({0.0f, 10.0f, 0.0f, 1.0f, "ratio"});
    normalizer.normalize("edge", "sensor", 5, 10, 2.0f);
    bool rejected = false;
    try {
        normalizer.normalize("edge", "sensor", 5, 11, 2.0f);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    TEST_ASSERT_TRUE(rejected);
}

void test_serializer_escapes_json_identifiers() {
    TelemetryFrame frame{R"(edge"one)", "sensor", 1, 100, 1.0f, 2.0f, "bar", SignalQuality::Good};
    const auto output = serialize(frame);
    TEST_ASSERT_TRUE(output.find(R"(edge\"one)") != std::string::npos);
}

void test_buffer_is_bounded_and_fifo() {
    TelemetryBuffer buffer(2);
    auto frame = [](int sequence) { return TelemetryFrame{"edge", "s", static_cast<unsigned>(sequence), 100, 1, 1, "bar", SignalQuality::Good}; };
    buffer.push(frame(1));
    buffer.push(frame(2));
    buffer.push(frame(3));
    TEST_ASSERT_EQUAL_UINT32(1, buffer.dropped());
    TEST_ASSERT_EQUAL_UINT32(2, buffer.size());
    TEST_ASSERT_EQUAL_UINT32(2, buffer.pop()->sequence);
    TEST_ASSERT_EQUAL_UINT32(3, buffer.pop()->sequence);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_calibration_and_quality);
    RUN_TEST(test_replay_is_rejected);
    RUN_TEST(test_serializer_escapes_json_identifiers);
    RUN_TEST(test_buffer_is_bounded_and_fifo);
    return UNITY_END();
}
