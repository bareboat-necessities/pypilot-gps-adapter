#pragma once

#ifndef ARDUINO

#include <stdint.h>
#include "../gps_fix.hpp"
#include "../gpsd_json.hpp"

namespace pypilot_gps_adapter {
namespace linux_gpsd {

class GpsdClient {
public:
    GpsdClient();
    ~GpsdClient();

    bool open(const char* host = "127.0.0.1", const char* port = "2947");
    void close();
    bool connected() const;

    bool send_watch(bool enable = true);
    int read_line(char* out, unsigned capacity, int timeout_ms);
    bool read_tpv(GpsFixInput<float>& out, uint64_t now_us, int timeout_ms = 1000);

    const char* last_error() const;

private:
    int fd_;
    char last_error_[128];
    GpsdJsonParser<float> parser_;

    void set_error(const char* message);
};

} // namespace linux_gpsd
} // namespace pypilot_gps_adapter

#endif // ARDUINO
