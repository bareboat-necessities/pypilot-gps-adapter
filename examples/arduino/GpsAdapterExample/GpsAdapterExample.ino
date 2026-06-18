#include <Arduino.h>
#include <pypilot_algorithms.hpp>
#include <pypilot_sensors.hpp>
#include <pypilot_gps_adapter.hpp>

void setup() {
    Serial.begin(9600);

    pypilot_gps_adapter::GpsFixInput<float> fix;
    fix.time_us = 1000000UL;
    fix.source = pypilot_data_model::SensorSource::gpsd;
    fix.device_id = "gpsd-arduino";
    fix.has_lat_lon = true;
    fix.latitude_deg = 40.1f;
    fix.longitude_deg = -74.2f;
    fix.has_speed = true;
    fix.speed_kn = 5.5f;
    fix.has_track = true;
    fix.track_deg = 123.0f;

    pypilot_sensors::SensorBatch<float> batch = pypilot_gps_adapter::make_gps_batch(fix);
    if (batch.has_gps && batch.gps.speed_valid) {
        Serial.println(batch.gps.speed_kn);
    }
}

void loop() {
}
