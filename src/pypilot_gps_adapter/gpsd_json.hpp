#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ship_data_model.hpp>
#include <pypilot_syslib.hpp>
#include "gps_fix.hpp"
#include "gps_filter_math.hpp"

namespace pypilot_gps_adapter {

namespace detail {

inline const char* skip_json_ws(const char* p) {
    while (p && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')) ++p;
    return p;
}

inline const char* find_key(const char* json, const char* key) {
    if (!json || !key) return 0;
    const char* p = json;
    const size_t key_len = strlen(key);
    while ((p = strstr(p, key)) != 0) {
        if (p > json && p[-1] == '"' && p[key_len] == '"') {
            p += key_len + 1;
            p = skip_json_ws(p);
            if (*p == ':') return skip_json_ws(p + 1);
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

inline bool parse_int_field(const char* json, const char* key, int& out) {
    double value = 0.0;
    if (!parse_number_field(json, key, value)) return false;
    out = int(value);
    return true;
}

inline bool parse_string_field(const char* json, const char* key, char* out, size_t capacity) {
    if (!out || capacity == 0) return false;
    out[0] = 0;
    const char* p = find_key(json, key);
    if (!p || *p != '"') return false;
    ++p;
    size_t n = 0;
    while (*p && *p != '"') {
        if (*p == '\\' && p[1]) ++p;
        if (n + 1 < capacity) out[n++] = *p;
        ++p;
    }
    out[n] = 0;
    return *p == '"';
}

} // namespace detail

template<typename Real = float>
class GpsdJsonParser {
public:
    GpsdJsonParser() : logger_(0) { device_id_[0] = 0; }

    void set_logger(pypilot_syslib::Logger* logger) { logger_ = logger; }
    pypilot_syslib::Logger* logger() const { return logger_; }

    bool parse_tpv(const char* json,
                   uint64_t now_us,
                   GpsFixInput<Real>& out,
                   const char* default_device_id = "gpsd") {
        char class_name[12];
        if (!detail::parse_string_field(json, "class", class_name, sizeof(class_name))) {
            log_rejected(now_us, 1);
            return false;
        }
        if (strcmp(class_name, "TPV") != 0) {
            log_rejected(now_us, 2);
            return false;
        }

        int mode = 0;
        if (detail::parse_int_field(json, "mode", mode) && mode < 2) {
            log_rejected(now_us, 3);
            return false;
        }

        GpsFixInput<Real> fix;
        fix.time_us = now_us;
        fix.source = ship_data_model::SensorSource::gpsd;
        fix.device_id = default_device_id;

        if (detail::parse_string_field(json, "device", device_id_, sizeof(device_id_))) {
            fix.device_id = device_id_;
        }

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
        if (detail::parse_number_field(json, "altHAE", alt) ||
            detail::parse_number_field(json, "alt", alt)) {
            fix.has_altitude = true;
            fix.altitude_m = Real(alt);
        }

        out = fix;
        const bool accepted = fix.has_lat_lon || fix.has_speed || fix.has_track || fix.has_altitude;
        if (accepted) {
            pypilot_syslib::log_if(logger_, now_us,
                                   pypilot_syslib::LogLevel::Info,
                                   pypilot_syslib::LogModule::GpsAdapter,
                                   pypilot_syslib::LogEvent::GpsFixAccepted,
                                   "gps fix accepted");
        } else {
            log_rejected(now_us, 4);
        }
        return accepted;
    }

private:
    void log_rejected(uint64_t now_us, int32_t reason) const {
        pypilot_syslib::log_if(logger_, now_us,
                               pypilot_syslib::LogLevel::Warn,
                               pypilot_syslib::LogModule::GpsAdapter,
                               pypilot_syslib::LogEvent::GpsFixRejected,
                               "gps fix rejected",
                               reason);
    }

    char device_id_[80];
    pypilot_syslib::Logger* logger_;
};

} // namespace pypilot_gps_adapter
