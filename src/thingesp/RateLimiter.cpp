#pragma once

#include <Arduino.h>
#include <stdint.h>

#include "../ArduinoJson.h"
#include "./Logger.cpp"

class RateLimiter{
public:

    bool is_rate_limited() {
        unsigned int current_millis = millis();
        if (last_called_millis && (last_called_millis+RATE_LIMIT < current_millis)){
            last_called_millis = current_millis;
            return false;
        }
        else {
            LOG("RL", "You are getting Rate Limited!")
            return true;
        }
    }

    void set_rate_limit(unsigned int _limit){
        RATE_LIMIT = _limit;
    }

    const char* get_rate_limits_msg(){

//        DynamicJsonDocument data_out(1024);
//        data_out["action"] = "GET_RATE_LIMITS";
//        String out_msg;
//        serializeJson(data_out, out_msg);
//        return out_msg.c_str();

        return "{\"action\": \"GET_RATE_LIMITS\"}";
    }


private:
    unsigned int last_called_millis = 0;
    unsigned int RATE_LIMIT = 0;
};