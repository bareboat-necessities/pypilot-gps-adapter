#pragma once

#include <pypilot_data_model.hpp>
#include <pypilot_sensors/samples.hpp>
#include <pypilot_sensors/sensors_manager.hpp>
#include "gps_fix.hpp"

namespace pypilot_gps_adapter {

template<typename Real = float>
inline pypilot_sensors::GpsSample<Real> make_gps_sample(const GpsFixInput<Real>& fix) {
    pypilot_sensors::GpsSample<Real> sample;
    sample.time_us = fix.time_us;
    sample.source = fix.source;
    sample.device_id = fix.device_id;

    if (fix.has_speed) {
        sample.speed_valid = true;
        sample.speed_kn = fix.speed_kn;
    }
    if (fix.has_track) {
        sample.track_valid = true;
        sample.track_deg = fix.track_deg;
    }
    if (fix.has_lat_lon) {
        sample.fix_valid = true;
        sample.latitude_deg = fix.latitude_deg;
        sample.longitude_deg = fix.longitude_deg;
    }
    if (fix.has_altitude) {
        sample.altitude_valid = true;
        sample.altitude_m = fix.altitude_m;
    }
    if (fix.has_declination) {
        sample.declination_valid = true;
        sample.declination_deg = fix.declination_deg;
    }
    return sample;
}

template<typename Real = float>
inline pypilot_sensors::SensorBatch<Real> make_gps_batch(const GpsFixInput<Real>& fix) {
    pypilot_sensors::SensorBatch<Real> batch;
    batch.has_gps = true;
    batch.gps = make_gps_sample(fix);
    return batch;
}

template<typename Real = float>
class GpsSensorBridge {
public:
    pypilot_sensors::GpsSample<Real> sample_from_fix(const GpsFixInput<Real>& fix) const {
        return make_gps_sample(fix);
    }

    pypilot_sensors::SensorBatch<Real> batch_from_fix(const GpsFixInput<Real>& fix) const {
        return make_gps_batch(fix);
    }

    bool apply_fix(pypilot_data_model::DataModel<Real>& model,
                   pypilot_sensors::SensorsManager<Real>& sensors,
                   const GpsFixInput<Real>& fix) const {
        return sensors.apply_batch(model, batch_from_fix(fix));
    }
};

} // namespace pypilot_gps_adapter
