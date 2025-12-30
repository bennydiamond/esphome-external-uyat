#pragma once

#include "esphome/core/helpers.h"

#include <functional>

#include "uyat_datapoint_types.h"

namespace esphome::uyat
{

struct DpTextSensor
{
   static constexpr const char * TAG = "uyat.DpTextSensor";

   using OnValueCallback = std::function<void(const std::string&)>;

   void init(DatapointHandler& handler)
   {
      handler_ = &handler;
      this->handler_->register_datapoint_listener(this->matching_dp_, [this](const UyatDatapoint &datapoint) {
         ESP_LOGV(DpTextSensor::TAG, "%s processing as text_sensor", datapoint.to_string());

         if (auto * dp_value = std::get_if<RawDatapointValue>(&datapoint.value))
         {
            this->value_ = std::string(dp_value->value.begin(), dp_value->value.end());
            this->value_ = decode(this->value_);
            callback_(this->value_);
         }
         else
         if (auto * dp_value = std::get_if<StringDatapointValue>(&datapoint.value))
         {
            this->value_ = decode(dp_value->value);
            callback_(this->value_);
         }
         else
         {
            ESP_LOGW(DpTextSensor::TAG, "Unhandled datapoint type %s!", datapoint.get_type_name());
            return;
         }
      });
   }

   std::string get_last_value() const
   {
      return value_;
   }

   const std::string config_to_string() const
   {
      return str_sprintf("%s%s%s", this->base64_encoded_? "Base64 " : "", this->as_hex_? "Hex " : "", this->matching_dp_.to_string().c_str());
   }

   static DpTextSensor create_for_raw(const OnValueCallback& callback, const uint8_t dp_id, const bool base64_encoded, const bool as_hex)
   {
      return DpTextSensor(callback, MatchingDatapoint{dp_id, UyatDatapointType::RAW}, base64_encoded, as_hex);
   }

   static DpTextSensor create_for_string(const OnValueCallback& callback, const uint8_t dp_id, const bool base64_encoded, const bool as_hex)
   {
      return DpTextSensor(callback, MatchingDatapoint{dp_id, UyatDatapointType::STRING}, base64_encoded, as_hex);
   }

   DpTextSensor(DpTextSensor&&) = default;
   DpTextSensor& operator=(DpTextSensor&&) = default;

private:

   DpTextSensor(const OnValueCallback& callback, MatchingDatapoint&& matching_dp, const bool base64_encoded, const bool as_hex):
   callback_(callback),
   matching_dp_(std::move(matching_dp)),
   base64_encoded_(base64_encoded),
   as_hex_(as_hex)
   {}

   std::string decode(const std::string& input) const
   {
      if (input.empty())
      {
         return {};
      }

      if (this->base64_encoded_)
      {
         const auto decoded = base64_decode(input);
         if (decoded.empty())
         {
            return {};
         }
         if (this->as_hex_)
         {
            return format_hex_pretty(&decoded.front(), decoded.size());
         }

         return std::string(decoded.begin(), decoded.end());
      }

      if (this->as_hex_)
      {
         return format_hex_pretty(input.data(), input.size());
      }

      return input;
   }

   OnValueCallback callback_;
   const MatchingDatapoint matching_dp_;
   const bool base64_encoded_;
   const bool as_hex_;

   DatapointHandler* handler_;
   std::string value_;
};

}
