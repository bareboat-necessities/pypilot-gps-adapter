#pragma once

#include <stdint.h>
#include <ship_data_model.hpp>

namespace pypilot_gps_adapter {

template<typename Real = float>
struct GpsFixInput {
    uint64_t time_us = 0;

    bool has_lat_lon = false;
    Real latitude_deg = Real(0);
    Real longitude_deg = Real(0);

    bool has_speed = false;
    Real speed_kn = Real(0);

    bool has_track = false;
    Real track_deg = Real(0);

    bool has_altitude = false;
    Real altitude_m = Real(0);

    bool has_declination = false;
    Real declination_deg = Real(0);

    ship_data_model::SensorSource source = ship_data_model::SensorSource::gpsd;
    const char* device_id = 0;
};

template<typename Real = float>
struct GpsPredictionInput {
    uint64_t time_us = 0;

    bool has_accel = false;
    Real accel_x = Real(0);
    Real accel_y = Real(0);
    Real accel_z = Real(0);

    bool has_attitude = false;
    Real q0 = Real(1);
    Real q1 = Real(0);
    Real q2 = Real(0);
    Real q3 = Real(0);
};

} // namespace pypilot_gps_adapter
