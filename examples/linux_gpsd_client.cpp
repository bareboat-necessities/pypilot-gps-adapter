#include <iostream>
#include <pypilot_gps_adapter/linux/gpsd_client.hpp>

int main() {
    pypilot_gps_adapter::linux_gpsd::GpsdClient client;
    if (!client.open()) {
        std::cerr << "gpsd open failed: " << client.last_error() << "\n";
        return 1;
    }
    if (!client.send_watch(true)) {
        std::cerr << "gpsd WATCH failed: " << client.last_error() << "\n";
        return 1;
    }

    pypilot_gps_adapter::GpsFixInput<float> fix;
    for (int i = 0; i < 10; ++i) {
        if (client.read_tpv(fix, 0, 1000)) {
            std::cout << "gps fix";
            if (fix.has_lat_lon) std::cout << " lat=" << fix.latitude_deg << " lon=" << fix.longitude_deg;
            if (fix.has_speed) std::cout << " speed_kn=" << fix.speed_kn;
            if (fix.has_track) std::cout << " track=" << fix.track_deg;
            std::cout << "\n";
            return 0;
        }
    }

    std::cerr << "no TPV fix received\n";
    return 2;
}
