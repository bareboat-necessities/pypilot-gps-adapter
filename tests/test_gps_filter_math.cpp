#include <cassert>
#include <cmath>
#include <pypilot_gps_adapter.hpp>

static bool near(double a, double b, double eps) { return std::fabs(a - b) < eps; }

int main() {
    const double lat0 = 40.0;
    const double lon0 = -74.0;
    const double lat = 40.001;
    const double lon = -73.998;

    const auto xy = pypilot_gps_adapter::ll_to_xy(lat, lon, lat0, lon0);
    assert(std::fabs(xy.x_m) > 100.0);
    assert(std::fabs(xy.y_m) > 100.0);

    const auto ll = pypilot_gps_adapter::xy_to_ll(xy.x_m, xy.y_m, lat0, lon0);
    assert(near(ll.latitude_deg, lat, 1e-9));
    assert(near(ll.longitude_deg, lon, 1e-9));

    assert(near(pypilot_gps_adapter::wrap_180_degrees(181.0), -179.0, 1e-12));
    assert(near(pypilot_gps_adapter::meters_per_second_to_knots(1.0), 1.9438444924406048, 1e-12));

    return 0;
}
