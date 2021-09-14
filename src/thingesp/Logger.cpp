#pragma once

#ifndef _LOG_DISABLE_
    #define LOG(type, text) Serial.print("["); Serial.print(F(type)); Serial.print("] "); Serial.println(F(text));
    #define LOG_VALUE(type, text, value) Serial.print("["); Serial.print(F(type)); Serial.print("] "); Serial.print(F(text)); Serial.println(value);
    #define LOG_WITHOUT_F(type, text) Serial.print("["); Serial.print(type); Serial.print("] "); Serial.println(text);
#else
    #define LOG(type, text) void();
    #define LOG_VALUE(type, text, value) void();
    #define LOG_WITHOUT_F(type, text) void();
#endif