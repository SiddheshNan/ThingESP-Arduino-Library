#pragma once

#include "../ArduinoJson.h"
#include "./RateLimiter.cpp"
#include "./Logger.cpp"

#if defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)
    String HandleResponse(String query) __attribute__((weak));
#else
    String HandleResponse(String query);
#endif

namespace thing_esp {
    class Message : public RateLimiter {
    public:
        void sendMsg(String number, String msg) {
            if (!is_device_calls_enabled()) {
                LOG("ERR",
                    "Device-calls have not been enabled! Please go to ThingESP console and enable them from project settings.");
                return;
            }

            if (is_rate_limited()) {
                LOG_VALUE("ERR", "This device-call have been rate limited! for (seconds): ", get_rate_limit());
                LOG("ERR",
                    "to reduce the rate-limiting, email me <hello@siddhesh.me> with your username and your project use-case.");
                return;
            }

            DynamicJsonDocument data_out(1024);
            data_out["action"] = "device_call";
            data_out["to_number"] = number;
            data_out["msg"] = msg;
            sendResponse(data_out);
        }

        void setCallback(String(*callback_func)(String)) {
            this->callbackFunction = callback_func;
        }


    protected:
        void onMessage(String &data) {

            DynamicJsonDocument data_in(1024);
            DynamicJsonDocument data_out(1024);
            deserializeJson(data_in, data);

            String incoming_action = data_in["action"];

            if (incoming_action == "query") {

                if (!HandleResponse && !callbackFunction){
                    LOG("MSG", "Error! No callback was set! Please set HandleResponse or setCallback!");
                    return;
                }

                data_out["msg_id"] = (const char *) data_in["msg_id"];
                data_out["action"] = "returned_api_response";
                String query = (const char *) data_in["query"];

                #ifndef _DISABLE_LOWER_CASE_
                                query.toLowerCase();
                #endif

                LOG_VALUE("MSG", "Query: ", query);
                String resp = !!HandleResponse ? HandleResponse(query) : this->callbackFunction(query);
                LOG_VALUE("MSG", "Response: ", resp);
                data_out["returned_api_response"] = resp;
                sendResponse(data_out);


            } else if (incoming_action == "rate_limits_info") {

                unsigned int delay = (unsigned int) data_in["delay"];
                bool calls_enabled = (bool) data_in["calls_enabled"];

                LOG_VALUE("INFO", "Device calls: ", calls_enabled ? "enabled" : "disabled");
                LOG_VALUE("INFO", "Got rate-limits (sec): ", delay / 1000);

                set_rate_limit(delay);
                set_device_calls_status(calls_enabled);

            } else if (incoming_action == "server_msg") {

                String incoming_msg = (const char *) data_in["msg"];
                LOG_WITHOUT_F("MSG", incoming_msg);

            }
        };


        void sendResponse(DynamicJsonDocument &json_doc) {
            String out_str;
            serializeJson(json_doc, out_str);
            publishMSG(out_str.c_str());
        }


        /*
         * Overwritten in the client
         */

        String (*callbackFunction)(String);

        virtual void publishMSG(const char *msg) {};


    };

}