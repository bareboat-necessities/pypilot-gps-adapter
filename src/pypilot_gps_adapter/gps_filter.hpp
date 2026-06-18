#pragma once

#include <pypilot_algorithms/gps_filter.hpp>
#include "gps_fix.hpp"

namespace pypilot_gps_adapter {

template<typename Real = float>
class GpsFilter {
public:
    GpsFilter() : has_fix_(false) {}

    void reset() {
        algorithm_.reset();
        has_fix_ = false;
        last_fix_ = GpsFixInput<Real>();
    }

    bool has_fix() const { return has_fix_; }
    const GpsFixInput<Real>& last_fix() const { return last_fix_; }

    const pypilot_algorithms::gps::GpsFilterOutput<Real>& output() const {
        return algorithm_.output();
    }

    GpsFixInput<Real> update(const GpsFixInput<Real>& fix) {
        pypilot_algorithms::gps::GpsFilterFix<Real> input;
        input.has_lat_lon = fix.has_lat_lon;
        input.latitude_deg = fix.latitude_deg;
        input.longitude_deg = fix.longitude_deg;
        input.has_speed = fix.has_speed;
        input.speed_kn = fix.speed_kn;
        input.has_track = fix.has_track;
        input.track_deg = fix.track_deg;
        input.timestamp_s = Real(fix.time_us) / Real(1000000);

        if (algorithm_.update(input, input.timestamp_s + algorithm_.gps_time_offset_s()) &&
            algorithm_.output().valid) {
            last_fix_ = fix;
            last_fix_.has_lat_lon = true;
            last_fix_.latitude_deg = algorithm_.output().latitude_deg;
            last_fix_.longitude_deg = algorithm_.output().longitude_deg;
            last_fix_.has_speed = true;
            last_fix_.speed_kn = algorithm_.output().speed_kn;
            last_fix_.has_track = true;
            last_fix_.track_deg = algorithm_.output().track_deg;
            has_fix_ = true;
            return last_fix_;
        }

        last_fix_ = fix;
        has_fix_ = fix.has_lat_lon || fix.has_speed || fix.has_track || fix.has_altitude;
        return last_fix_;
    }

    bool predict(const GpsPredictionInput<Real>& prediction) {
        pypilot_algorithms::gps::GpsFilterPrediction<Real> input;
        input.time_s = Real(prediction.time_us) / Real(1000000);
        if (prediction.has_accel) {
            input.accel_x_m_s2 = prediction.accel_x;
            input.accel_y_m_s2 = prediction.accel_y;
        }
        const bool ok = algorithm_.predict(input);
        if (ok && algorithm_.output().valid) {
            last_fix_.time_us = prediction.time_us;
            last_fix_.has_lat_lon = true;
            last_fix_.latitude_deg = algorithm_.output().latitude_deg;
            last_fix_.longitude_deg = algorithm_.output().longitude_deg;
            last_fix_.has_speed = true;
            last_fix_.speed_kn = algorithm_.output().speed_kn;
            last_fix_.has_track = true;
            last_fix_.track_deg = algorithm_.output().track_deg;
            has_fix_ = true;
        }
        return ok;
    }

private:
    pypilot_algorithms::gps::GpsFilter2D<Real> algorithm_;
    bool has_fix_;
    GpsFixInput<Real> last_fix_;
};

} // namespace pypilot_gps_adapter
