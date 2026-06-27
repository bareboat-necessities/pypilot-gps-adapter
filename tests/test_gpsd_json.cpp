#include <cassert>
#include <cmath>
#include <cstring>
#include <pypilot_gps_adapter.hpp>

static bool nearf(float a, float b) { return std::fabs(a - b) < 0.0001f; }

int main() {
    const char* tpv = "{\"class\":\"TPV\",\"mode\":3,\"device\":\"/dev/ttyUSB0\",\"lat\":40.1,\"lon\":-74.2,\"speed\":2.0,\"track\":123.4,\"altHAE\":7.5}";

    pypilot_gps_adapter::GpsdJsonParser<float> parser;
    pypilot_gps_adapter::GpsFixInput<float> fix;
    assert(parser.parse_tpv(tpv, 123456ULL, fix, "gpsd-test"));

    assert(fix.time_us == 123456ULL);
    assert(fix.source == ship_data_model::SensorSource::gpsd);
    assert(fix.device_id && std::strcmp(fix.device_id, "/dev/ttyUSB0") == 0);
    assert(fix.has_lat_lon);
    assert(nearf(fix.latitude_deg, 40.1f));
    assert(nearf(fix.longitude_deg, -74.2f));
    assert(fix.has_speed);
    assert(fix.speed_kn > 3.88f && fix.speed_kn < 3.89f);
    assert(fix.has_track);
    assert(nearf(fix.track_deg, 123.4f));
    assert(fix.has_altitude);
    assert(nearf(fix.altitude_m, 7.5f));

    const char* tpv_no_device = "{\"class\":\"TPV\",\"mode\":2,\"lat\":40.2,\"lon\":-74.3,\"alt\":8.5}";
    assert(parser.parse_tpv(tpv_no_device, 123457ULL, fix, "gpsd-default"));
    assert(fix.device_id && std::strcmp(fix.device_id, "gpsd-default") == 0);
    assert(fix.has_altitude);
    assert(nearf(fix.altitude_m, 8.5f));

    assert(!parser.parse_tpv("{\"class\":\"SKY\"}", 1, fix));
    assert(!parser.parse_tpv("{\"class\":\"TPV\",\"mode\":1,\"lat\":1,\"lon\":2}", 1, fix));
    return 0;
}
