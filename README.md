# Arduino Cloud Telemetry

Embedded C++ telemetry pipeline for calibrated sensor readings, quality flags, monotonic sequences and bounded offline buffering.

## Build

```bash
pio test -e native
pio run -e esp32dev
```

The firmware example emits JSON telemetry over serial. `TelemetrySink` is the integration boundary for a cloud provider; credentials and live cloud delivery are intentionally not included.


#UPDATED FOR ME
