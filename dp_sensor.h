#pragma once

#include <functional>

#include "uyat_datapoint_types.h"

namespace esphome::uyat
{

struct DpSensor
{
   static constexpr const char * TAG = "uyat.DpSensor";

   using OnValueCallback = std::function<void(const float)>;

   void init(DatapointHandler& handler)
   {
      handler.register_datapoint_listener(this->matching_dp_, [this](const UyatDatapoint &datapoint) {
         ESP_LOGV(DpSensor::TAG, "%s processing as sensor", datapoint.to_string());

         if (auto * dp_value = std::get_if<BoolDatapointValue>(&datapoint.value))
         {
            this->value_ = calculate(dp_value->value);
            callback_(value_.value());
         }
         else
         if (auto * dp_value = std::get_if<UIntDatapointValue>(&datapoint.value))
         {
            this->value_ = calculate(dp_value->value);
            callback_(value_.value());
         }
         else
         if (auto * dp_value = std::get_if<EnumDatapointValue>(&datapoint.value))
         {
            this->value_ = calculate(dp_value->value);
            callback_(value_.value());
         }
         else
         if (auto * dp_value = std::get_if<Bitmask8DatapointValue>(&datapoint.value))
         {
            this->value_ = calculate(dp_value->value);
            callback_(value_.value());
         }
         else
         if (auto * dp_value = std::get_if<Bitmask16DatapointValue>(&datapoint.value))
         {
            this->value_ = calculate(dp_value->value);
            callback_(value_.value());
         }
         else
         if (auto * dp_value = std::get_if<Bitmask32DatapointValue>(&datapoint.value))
         {
            this->value_ = calculate(dp_value->value);
            callback_(value_.value());
         }
         else
         {
            if (matching_dp_.type.has_value())
            {
               ESP_LOGW(DpSensor::TAG, "Unhandled datapoint type %s!", datapoint.get_type_name());
            }

            return;
         }
      });
   }

   std::optional<float> get_last_value() const
   {
      return value_;
   }

   std::string config_to_string() const
   {
      if (matching_dp_.type)
      {
         return str_sprintf("%s, offset=%d, multiplier=%.2f", this->matching_dp_.to_string().c_str(), this->offset_, this->multiplier_);
      }
      else
      {
         return str_sprintf("Datapoint %u: BITMAP, offset=%d, multiplier=%.2f", this->matching_dp_.number, this->offset_, this->multiplier_);
      }
   }

   static DpSensor create_for_any(const OnValueCallback& callback, const uint8_t dp_id, const int32_t offset = 0, const uint16_t scale = 0)
   {
      // todo: set type to multiple supported types once implemented
      return DpSensor(callback, MatchingDatapoint{dp_id, {}}, offset, scale);
   }

   static DpSensor create_for_bool(const OnValueCallback& callback, const uint8_t dp_id, const int32_t offset = 0, const uint16_t scale = 0)
   {
      return DpSensor(callback, MatchingDatapoint{dp_id, UyatDatapointType::BOOLEAN}, offset, scale);
   }

   static DpSensor create_for_uint(const OnValueCallback& callback, const uint8_t dp_id, const int32_t offset = 0, const uint16_t scale = 0)
   {
      return DpSensor(callback, MatchingDatapoint{dp_id, UyatDatapointType::INTEGER}, offset, scale);
   }

   static DpSensor create_for_enum(const OnValueCallback& callback, const uint8_t dp_id, const int32_t offset = 0, const uint16_t scale = 0)
   {
      return DpSensor(callback, MatchingDatapoint{dp_id, UyatDatapointType::ENUM}, offset, scale);
   }

   static DpSensor create_for_bitmap(const OnValueCallback& callback, const uint8_t dp_id, const int32_t offset = 0, const uint16_t scale = 0)
   {
      // todo: set matching to any bitmask type
      return DpSensor(callback, MatchingDatapoint{dp_id, {}}, offset, scale);
   }

   DpSensor(DpSensor&&) = default;
   DpSensor& operator=(DpSensor&&) = default;

private:

   static constexpr float scale2multiplier(const uint16_t scale)
   {
      switch (scale)
      {
         case 0:
            return 1.0f;
         case 1:
            return 0.1f;
         case 2:
            return 0.01f;
         case 3:
            return 0.001f;
         case 4:
            return 0.0001f;
         case 5:
            return 0.00001f;
         case 6:
            return 0.000001f;
         default:
            return 1.0f;
      }
   }

   DpSensor(const OnValueCallback& callback, MatchingDatapoint&& matching_dp, const int32_t offset, const uint16_t scale):
   callback_(callback),
   matching_dp_(std::move(matching_dp)),
   offset_(offset),
   multiplier_(scale2multiplier(scale))
   {}

   float calculate(const uint32_t value) const
   {
      return (float(value) + float(this->offset_)) * this->multiplier_;
   }

   OnValueCallback callback_;
   const MatchingDatapoint matching_dp_;
   const int32_t offset_;
   const float multiplier_;

   std::optional<float> value_;
};

}
