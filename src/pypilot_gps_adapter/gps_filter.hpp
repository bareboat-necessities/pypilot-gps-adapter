#pragma once

#include "gps_fix.hpp"

namespace pypilot_gps_adapter {

template<typename Real = float>
class GpsFilter {
public:
    GpsFilter() : has_fix_(false) {}

    void reset() { has_fix_ = false; }

    bool has_fix() const { return has_fix_; }
    const GpsFixInput<Real>& last_fix() const { return last_fix_; }

    GpsFixInput<Real> update(const GpsFixInput<Real>& fix) {
        last_fix_ = fix;
        has_fix_ = true;
        return last_fix_;
    }

    bool predict(const GpsPredictionInput<Real>&) {
        return has_fix_;
    }

private:
    bool has_fix_;
    GpsFixInput<Real> last_fix_;
};

} // namespace pypilot_gps_adapter
