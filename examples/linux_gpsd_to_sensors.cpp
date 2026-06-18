#include <iostream>
#include <pypilot_gps_adapter.hpp>

int main() {
    const char* tpv = "{\"class\":\"TPV\",\"lat\":40.1,\"lon\":-74.2,\"speed\":2.0,\"track\":123.4}";
    pypilot_gps_adapter::GpsdJsonParser<float> parser;
    pypilot_gps_adapter::GpsFixInput<float> fix;
    if (!parser.parse_tpv(tpv, 1000000ULL, fix, "gpsd-demo")) {
        std::cerr << "failed to parse gpsd TPV\n";
        return 1;
    }
    const auto batch = pypilot_gps_adapter::make_gps_batch(fix);
    std::cout << "has_gps=" << batch.has_gps << " speed_kn=" << batch.gps.speed_kn << "\n";
    return 0;
}
