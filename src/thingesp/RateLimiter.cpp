#pragma once

#include <Arduino.h>
#include <stdint.h>

#include "../ArduinoJson.h"
#include "./Logger.cpp"

namespace thing_esp {
    class RateLimiter {
    protected:

        bool is_rate_limited() {
            unsigned int current_millis = millis();
            if (current_millis - last_called_millis >= RATE_LIMIT) {
                last_called_millis = current_millis;
                return false;
            } else {
                LOG("INFO", "You are getting Rate Limited!")
                return true;
            }
        }

        void set_rate_limit(unsigned int _limit) {
            RATE_LIMIT = _limit;
        }

        const char *get_rate_limits_msg() {

//        DynamicJsonDocument data_out(1024);
//        data_out["action"] = "get_rate_limits";
//        String out_msg;
//        serializeJson(data_out, out_msg);
//        return out_msg.c_str();

            return "{\"action\": \"get_rate_limits\"}";
        }

        bool is_device_calls_enabled() {
            return device_calls_enabled;
        }

        void set_device_calls_status(bool is_enabled) {
            device_calls_enabled = is_enabled;
        }

        unsigned int get_rate_limit() {
            return RATE_LIMIT / 1000;
        }


    private:
        bool device_calls_enabled = false;
        unsigned int last_called_millis = 0;
        unsigned int RATE_LIMIT = 0;
    };
}