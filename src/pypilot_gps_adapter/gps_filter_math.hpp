#pragma once

#include <pypilot_algorithms/gps_math.hpp>

namespace pypilot_gps_adapter {

using pypilot_algorithms::gps::GpsLatLon;
using pypilot_algorithms::gps::GpsXY;
using pypilot_algorithms::gps::earth_m_per_degree;
using pypilot_algorithms::gps::earth_radius_m;
using pypilot_algorithms::gps::ll_to_xy;
using pypilot_algorithms::gps::meters_per_second_to_knots;
using pypilot_algorithms::gps::wrap_180_degrees;
using pypilot_algorithms::gps::xy_to_ll;

} // namespace pypilot_gps_adapter
