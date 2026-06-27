#include <cassert>
#include <cmath>
#include <pypilot_gps_adapter.hpp>

static bool nearf(float a, float b, float eps) { return std::fabs(a - b) < eps; }

int main() {
    pypilot_gps_adapter::GpsFilter<float> filter;
    assert(!filter.has_fix());

    pypilot_gps_adapter::GpsFixInput<float> fix;
    fix.time_us = 100000000UL;
    fix.source = ship_data_model::SensorSource::gpsd;
    fix.device_id = "gpsd0";
    fix.has_lat_lon = true;
    fix.latitude_deg = 40.0f;
    fix.longitude_deg = -74.0f;
    fix.has_speed = true;
    fix.speed_kn = 10.0f;
    fix.has_track = true;
    fix.track_deg = 90.0f;

    const auto filtered = filter.update(fix);
    assert(filter.has_fix());
    assert(filtered.has_lat_lon);
    assert(nearf(filtered.latitude_deg, 40.0f, 0.00001f));
    assert(nearf(filtered.longitude_deg, -74.0f, 0.00001f));
    assert(filtered.has_speed);
    assert(filtered.has_track);
    assert(nearf(filtered.track_deg, 90.0f, 0.01f));

    pypilot_gps_adapter::GpsPredictionInput<float> prediction;
    prediction.time_us = 100200000UL;
    prediction.has_accel = true;
    prediction.accel_x = 0.0f;
    prediction.accel_y = 0.0f;
    prediction.accel_z = 0.0f;
    assert(filter.predict(prediction));
    assert(filter.last_fix().has_lat_lon);
    assert(filter.last_fix().longitude_deg > -74.0f);

    pypilot_gps_adapter::GpsFixInput<float> partial;
    partial.time_us = 100300000UL;
    partial.has_lat_lon = true;
    partial.latitude_deg = 41.0f;
    partial.longitude_deg = -73.0f;
    const auto partial_out = filter.update(partial);
    assert(partial_out.has_lat_lon);
    assert(nearf(partial_out.latitude_deg, 41.0f, 0.00001f));
    assert(nearf(partial_out.longitude_deg, -73.0f, 0.00001f));

    return 0;
}
