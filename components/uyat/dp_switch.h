#pragma once

#include <functional>

#include "uyat_dp_retry.h"
#include "uyat_datapoint_types.h"

namespace esphome::uyat
{

struct DpSwitch
{
   static constexpr const char * TAG = "uyat.DpSwitch";

   using OnValueCallback = std::function<void(const bool)>;

   struct Config
   {
      MatchingDatapoint matching_dp;
      const bool inverted;

      const std::string to_string() const
      {
         return str_sprintf("%s%s", this->inverted? "Inverted " : "", this->matching_dp.to_string().c_str());
      }
   };

   void init(DatapointHandler& handler)
   {
      this->handler_ = &handler;
      this->retry_.init(this->handler_, [this]() {
         if (!this->set_value_.has_value()) {
            return false;
         }
         this->set_value(this->set_value_.value(), true);
         return true;
      });
      this->handler_->register_datapoint_listener(this->config_.matching_dp, [this](const UyatDatapoint &datapoint) {
         ESP_LOGV(DpSwitch::TAG, "%s processing as switch", datapoint.to_string().c_str());

         if (!this->config_.matching_dp.matches(datapoint.get_type()))
         {
            ESP_LOGW(DpSwitch::TAG, "Non-matching datapoint type %s!", datapoint.get_type_name());
            return;
         }

         bool received_val = false;
         if (auto * dp_value = std::get_if<BoolDatapointValue>(&datapoint.value))
         {
            if (!this->config_.matching_dp.allows_single_type())
            {
               this->config_.matching_dp.types = {UyatDatapointType::BOOLEAN};
               ESP_LOGI(DpSwitch::TAG, "Resolved %s", this->config_.matching_dp.to_string().c_str());
            }
            received_val = invert_if_needed(dp_value->value);
         }
         else
         if (auto * dp_value = std::get_if<UIntDatapointValue>(&datapoint.value))
         {
            if (!this->config_.matching_dp.allows_single_type())
            {
               this->config_.matching_dp.types = {UyatDatapointType::INTEGER};
               ESP_LOGI(DpSwitch::TAG, "Resolved %s", this->config_.matching_dp.to_string().c_str());
            }
            received_val = invert_if_needed(dp_value->value != 0);
         }
         else
         if (auto * dp_value = std::get_if<EnumDatapointValue>(&datapoint.value))
         {
            if (!this->config_.matching_dp.allows_single_type())
            {
               this->config_.matching_dp.types = {UyatDatapointType::ENUM};
               ESP_LOGI(DpSwitch::TAG, "Resolved %s", this->config_.matching_dp.to_string().c_str());
            }
            received_val = invert_if_needed(dp_value->value != 0);
         }
         else
         {
            ESP_LOGW(DpSwitch::TAG, "Unhandled datapoint type %s!", datapoint.get_type_name());
            return;
         }

         received_value_ = received_val;
         
         // Check if this is an ack for a pending retry
         if (this->set_value_.has_value() && this->set_value_.value() == received_val) {
            ESP_LOGD(DpSwitch::TAG, "[DP%u] MCU confirmed value, canceling retry", this->config_.matching_dp.number);
            this->retry_.cancel();
         }
         
         callback_(received_val);
      });
   }

   std::optional<bool> get_last_received_value() const
   {
      return received_value_;
   }

   std::optional<bool> get_last_set_value() const
   {
      return set_value_;
   }

   const Config& get_config() const
   {
      return config_;
   }

   void set_value(const bool value, bool already_in_retry_sequence = false)
   {
      if (this->handler_ == nullptr)
      {
         ESP_LOGE(DpSwitch::TAG, "DatapointHandler not initialized for %s", this->config_.to_string().c_str());
         return;
      }

      this->set_value_ = value;
      if (!this->config_.matching_dp.allows_single_type())
      {
         ESP_LOGW(DpSwitch::TAG, "Cannot set value, datapoint type not yet known for %s", this->config_.matching_dp.to_string().c_str());
         return;
      }
      
      // Send the datapoint value
      if (this->config_.matching_dp.matches(UyatDatapointType::BOOLEAN))
      {
         handler_->set_datapoint_value(UyatDatapoint{this->config_.matching_dp.number, BoolDatapointValue{invert_if_needed(value)}}, already_in_retry_sequence);
      }
      else
      if (this->config_.matching_dp.matches(UyatDatapointType::INTEGER))
      {
         handler_->set_datapoint_value(UyatDatapoint{this->config_.matching_dp.number, UIntDatapointValue{invert_if_needed(value)? 0x01u : 0x00u}}, already_in_retry_sequence);
      }
      else
      if (this->config_.matching_dp.matches(UyatDatapointType::ENUM))
      {
         handler_->set_datapoint_value(UyatDatapoint{this->config_.matching_dp.number, EnumDatapointValue{static_cast<decltype(EnumDatapointValue::value)>(invert_if_needed(value)? 0x01 : 0x00)}}, already_in_retry_sequence);
      }
      
      // Setup retry if enabled and not already in retry sequence
      // Only schedule retry if we're actually changing the value (value differs from last MCU confirmation)
      const auto last_value = this->get_last_received_value();
      const bool value_changed = !last_value.has_value() || last_value.value() != value;
      this->retry_.schedule_if_needed(already_in_retry_sequence, value_changed);
   }

   DpSwitch(DpSwitch&&) = default;
   DpSwitch& operator=(DpSwitch&&) = default;

   DpSwitch(const OnValueCallback& callback, MatchingDatapoint&& matching_dp, const bool inverted, const DatapointRetryConfig& retry_config):
   retry_(matching_dp.number, retry_config, TAG),
   config_{std::move(matching_dp), inverted},
   callback_(callback)
   {}

private:
   bool invert_if_needed(const bool logical) const
   {
      return this->config_.inverted? (!logical) : logical;
   }

   DatapointRetry retry_;
   Config config_;
   OnValueCallback callback_;

   DatapointHandler* handler_{nullptr};
   std::optional<bool> received_value_;
   std::optional<bool> set_value_;
};

}
