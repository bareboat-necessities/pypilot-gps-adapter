# pypilot-gps-adapter

C++ GPS adapter and filter layer for modular pypilot.

This project converts GPS-specific inputs such as gpsd TPV messages and filtered GPS state into `pypilot-sensors` samples. It is intentionally above generic sensor arbitration and below the runtime daemon.

## Dependencies

`pypilot-gps-adapter` depends on:

* `pypilot-data-model`
* `pypilot-algorithms`
* `pypilot-sensors`

The dependency direction is:

```text
gpsd / raw GPS / filtered GPS
              │
              v
      pypilot-gps-adapter ───> pypilot-sensors ───> pypilot-data-model
              │
              └──────────────> pypilot-algorithms
```

`pypilot-sensors` does **not** depend on `pypilot-gps-adapter`.

## Responsibilities

This module owns:

* GPS fix input structs
* GPS coordinate math from original `pypilot/gps_filter.py`
* gpsd TPV JSON parsing into GPS fix input
* GPS fix to `pypilot_sensors::GpsSample` conversion
* GPS fix to `pypilot_sensors::SensorBatch` conversion
* GPS fix application through `pypilot_sensors::SensorsManager`
* future portable GPSFilter predict/update state
* optional future Linux gpsd socket client

This module does **not** own:

* generic source/device arbitration
* generic sensor timeout/lostdevice behavior
* NMEA 0183 parsing
* Signal K parsing
* autopilot mode selection
* pilot command computation
* servo command output runtime
* persistent settings storage
* user interface/server API

## Build

```bash
cmake -S . -B build \
  -DPYPILOT_DATA_MODEL_DIR=$PWD/../pypilot-data-model/src \
  -DPYPILOT_ALGORITHMS_DIR=$PWD/../pypilot-algorithms/src \
  -DPYPILOT_SENSORS_DIR=$PWD/../pypilot-sensors/src
cmake --build build
ctest --test-dir build --output-on-failure
```

## Arduino

Use the Arduino Library Manager or local `--libraries` paths for all dependencies:

```bash
arduino-cli compile --fqbn arduino:avr:mega \
  --libraries pypilot-gps-adapter \
  --libraries pypilot-sensors \
  --libraries pypilot-data-model \
  --libraries pypilot-algorithms \
  --libraries pypilot-nmea0183-connector \
  --libraries pypilot-signalk-connector \
  --libraries pypilot-servo-protocol \
  pypilot-gps-adapter/examples/arduino/GpsAdapterExample
```

Linux-only gpsd socket code should stay behind a CMake/compile-time option and should not be required for Arduino builds.

## Current scope

Completed in this module so far:

* Phase 4.1: portable `ll_to_xy` / `xy_to_ll` coordinate math, including dateline wrapping parity with original PyPilot
* Phase 4.3: GPS fix to `GpsSample`, `SensorBatch`, and `SensorsManager` bridge APIs
* early gpsd TPV JSON parsing scaffold

Still pending:

* Phase 4.2: full original PyPilot GPSFilter Kalman predict/update port
* Phase 4.5: Linux gpsd socket client
