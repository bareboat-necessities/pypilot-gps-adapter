#include <cassert>
#include <cmath>
#include <cstring>
#include <pypilot_gps_adapter.hpp>

static bool nearf(float a, float b) { return std::fabs(a - b) < 0.0001f; }

int main() {
    pypilot_gps_adapter::GpsFixInput<float> fix;
    fix.time_us = 5000;
    fix.source = pypilot_data_model::SensorSource::gpsd;
    fix.device_id = "gpsd0";
    fix.has_lat_lon = true;
    fix.latitude_deg = 41.0f;
    fix.longitude_deg = -73.0f;
    fix.has_speed = true;
    fix.speed_kn = 6.2f;
    fix.has_track = true;
    fix.track_deg = 275.0f;

    const auto sample = pypilot_gps_adapter::make_gps_sample(fix);
    assert(sample.time_us == 5000);
    assert(sample.source == pypilot_data_model::SensorSource::gpsd);
    assert(sample.device_id && std::strcmp(sample.device_id, "gpsd0") == 0);
    assert(sample.fix_valid);
    assert(nearf(sample.latitude_deg, 41.0f));
    assert(nearf(sample.longitude_deg, -73.0f));
    assert(sample.speed_valid);
    assert(nearf(sample.speed_kn, 6.2f));
    assert(sample.track_valid);
    assert(nearf(sample.track_deg, 275.0f));

    const auto batch = pypilot_gps_adapter::make_gps_batch(fix);
    assert(batch.has_gps);
    assert(batch.gps.source == pypilot_data_model::SensorSource::gpsd);

    return 0;
}
