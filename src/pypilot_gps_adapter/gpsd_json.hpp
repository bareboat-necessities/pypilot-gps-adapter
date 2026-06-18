#pragma once

#include <stdlib.h>
#include <string.h>
#include "gps_fix.hpp"
#include "gps_filter_math.hpp"

namespace pypilot_gps_adapter {

namespace detail {

inline const char* find_key(const char* json, const char* key) {
    if (!json || !key) return 0;
    const char* p = json;
    while ((p = strstr(p, key)) != 0) {
        if (p > json && p[-1] == '"' && p[strlen(key)] == '"') {
            p += strlen(key) + 1;
            while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') ++p;
            if (*p == ':') return p + 1;
        }
        ++p;
    }
    return 0;
}

inline bool parse_number_field(const char* json, const char* key, double& out) {
    const char* p = find_key(json, key);
    if (!p) return false;
    char* end = 0;
    const double value = strtod(p, &end);
    if (end == p) return false;
    out = value;
    return true;
}

} // namespace detail

template<typename Real = float>
class GpsdJsonParser {
public:
    bool parse_tpv(const char* json,
                   uint64_t now_us,
                   GpsFixInput<Real>& out,
                   const char* device_id = "gpsd") const {
        if (!json || strstr(json, "\"TPV\"") == 0) return false;

        GpsFixInput<Real> fix;
        fix.time_us = now_us;
        fix.source = pypilot_data_model::SensorSource::gpsd;
        fix.device_id = device_id;

        double lat = 0.0;
        double lon = 0.0;
        if (detail::parse_number_field(json, "lat", lat) &&
            detail::parse_number_field(json, "lon", lon)) {
            fix.has_lat_lon = true;
            fix.latitude_deg = Real(lat);
            fix.longitude_deg = Real(lon);
        }

        double speed_m_s = 0.0;
        if (detail::parse_number_field(json, "speed", speed_m_s)) {
            fix.has_speed = true;
            fix.speed_kn = Real(meters_per_second_to_knots(speed_m_s));
        }

        double track = 0.0;
        if (detail::parse_number_field(json, "track", track)) {
            fix.has_track = true;
            fix.track_deg = Real(track);
        }

        double alt = 0.0;
        if (detail::parse_number_field(json, "alt", alt)) {
            fix.has_altitude = true;
            fix.altitude_m = Real(alt);
        }

        out = fix;
        return fix.has_lat_lon || fix.has_speed || fix.has_track || fix.has_altitude;
    }
};

} // namespace pypilot_gps_adapter
