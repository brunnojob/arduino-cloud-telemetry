# ESP32 Telemetry

Calibrated acquisition, signal quality, per-sensor sequence numbers, a bounded buffer, and acknowledged serial delivery.

## Run

Requirements: ESP32, C++17, and PlatformIO.

```sh
pio test -e native
pio run -e esp32dev
pio run -e esp32dev -t upload
python -m pip install -r cloud/requirements.txt
python cloud/serial_bridge.py /dev/ttyUSB0
```

## Behavior

ADC uses GPIO 34; serial runs at 115200 baud. Extended uptime handles `millis()` rollover. A boot identifier distinguishes restarts. The bridge retains batches in SQLite. Calibration and pin assignments must match the connected equipment.

## Result synchronization

The [operations archive](https://vercel-home-telemetry-api.vercel.app/laboratory.html?project=arduino-cloud-telemetry) stores execution results. Supabase migrations are in the [API repository](https://github.com/brunnojob/vercel-home-telemetry-api/tree/main/supabase/migrations).

```sh
python cloud/sync.py enqueue result.json --project arduino-cloud-telemetry
python cloud/sync.py sync
```

Set `BRUNNODEV_ACCESS_TOKEN` to your session token. The SQLite outbox retains reports until the server confirms persistence; identical content does not create duplicate records. Tokens are not stored in source code. To run the synchronization tests:

```sh
python -m unittest discover -s cloud
```
