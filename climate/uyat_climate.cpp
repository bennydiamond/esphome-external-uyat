#include "esphome/core/log.h"
#include "esphome/core/entity_base.h"

#include "uyat_climate.h"

namespace esphome {
namespace uyat {

void UyatClimate::on_switch_value(const bool value)
{
  ESP_LOGV(UyatClimate::TAG, "Switch of %s is now %s", this->get_object_id().c_str(), ONOFF(value));
  this->mode = climate::CLIMATE_MODE_OFF;

  if (value)
  {
    if (this->supports_heat_ && this->supports_cool_) {
      this->mode = climate::CLIMATE_MODE_HEAT_COOL;
    } else if (this->supports_heat_) {
      this->mode = climate::CLIMATE_MODE_HEAT;
    } else if (this->supports_cool_) {
      this->mode = climate::CLIMATE_MODE_COOL;
    }
  }

  this->compute_state_();
  this->publish_state();
}

void UyatClimate::on_sleep_value(const bool value)
{
  ESP_LOGV(UyatClimate::TAG, "Sleep of %s is now %s", this->get_object_id().c_str(), ONOFF(value));
  this->preset = this->presets_.get_active_preset();
  this->compute_target_temperature_();
  this->publish_state();
}

void UyatClimate::on_eco_value(const bool value)
{
  ESP_LOGV(UyatClimate::TAG, "Eco of %s is now %s", this->get_object_id().c_str(), ONOFF(value));
  this->preset = this->presets_.get_active_preset();
  this->compute_target_temperature_();
  this->publish_state();
}

void UyatClimate::on_boost_value(const bool value)
{
  ESP_LOGV(UyatClimate::TAG, "Boost of %s is now %s", this->get_object_id().c_str(), ONOFF(value));
  this->preset = this->presets_.get_active_preset();
  this->compute_target_temperature_();
  this->publish_state();
}

void UyatClimate::on_target_temperature_value(const float value)
{
  this->manual_temperature_ = value;
  if (this->reports_fahrenheit_) {
    this->manual_temperature_ = (this->manual_temperature_ - 32) * 5 / 9;
  }

  ESP_LOGV(UyatClimate::TAG, "Manual Target Temperature of %s is now %.1f", this->get_object_id().c_str(), this->manual_temperature_);
  this->compute_target_temperature_();
  this->compute_state_();
  this->publish_state();
}

void UyatClimate::on_current_temperature_value(const float value)
{
  this->current_temperature = value;
  if (this->reports_fahrenheit_) {
    this->current_temperature = (this->current_temperature - 32) * 5 / 9;
  }

  ESP_LOGV(UyatClimate::TAG, "Current Temperature of %s is now %.1f", this->get_object_id().c_str(), this->current_temperature);
  this->compute_state_();
  this->publish_state();
}

void UyatClimate::on_active_state_value(const float value)
{
  ESP_LOGV(UyatClimate::TAG, "MCU reported active state is: %.0f", value);
  this->dp_active_state_->dp_number.get_last_received_value() = static_cast<uint32_t>(value);
  this->compute_state_();
  this->publish_state();
}

void UyatClimate::setup() {
  if (this->dp_switch_.has_value()) {
    this->dp_switch_->init(*(this->parent_));
  }
  if (this->active_state_pins_.heating != nullptr) {
    this->active_state_pins_.heating->setup();
    this->active_state_pins_.heating_state = this->active_state_pins_.heating->digital_read();
  }
  if (this->active_state_pins_.cooling != nullptr) {
    this->active_state_pins_.cooling->setup();
    this->active_state_pins_.cooling_state = this->active_state_pins_.cooling->digital_read();
  }
  if (this->dp_active_state_.has_value()) {
    this->dp_active_state_->dp_number.init(*(this->parent_));
  }
  if (this->dp_target_temperature_.has_value()) {
    this->dp_target_temperature_->init(*(this->parent_));
  }
  if (this->dp_current_temperature_.has_value()) {
    this->dp_current_temperature_->init(*(this->parent_));
  }

  this->presets_.init(*(this->parent_));

  if (this->swing_vertical_id_.has_value()) {
    this->parent_->register_datapoint_listener(*this->swing_vertical_id_, [this](const UyatDatapoint &datapoint) {
      auto * dp_value = std::get_if<BoolDatapointValue>(&datapoint.value);
      if (!dp_value)
      {
        ESP_LOGW(UyatClimate::TAG, "Unexpected datapoint type!");
        return;
      }

      this->swing_vertical_ = dp_value->value;
      ESP_LOGV(UyatClimate::TAG, "MCU reported vertical swing is: %s", ONOFF(dp_value->value));
      this->compute_swingmode_();
      this->publish_state();
    });
  }

  if (this->swing_horizontal_id_.has_value()) {
    this->parent_->register_datapoint_listener(*this->swing_horizontal_id_, [this](const UyatDatapoint &datapoint) {
      auto * dp_value = std::get_if<BoolDatapointValue>(&datapoint.value);
      if (!dp_value)
      {
        ESP_LOGW(UyatClimate::TAG, "Unexpected datapoint type!");
        return;
      }

      this->swing_horizontal_ = dp_value->value;
      ESP_LOGV(UyatClimate::TAG, "MCU reported horizontal swing is: %s", ONOFF(dp_value->value));
      this->compute_swingmode_();
      this->publish_state();
    });
  }

  if (this->fan_speed_id_.has_value()) {
    this->parent_->register_datapoint_listener(*this->fan_speed_id_, [this](const UyatDatapoint &datapoint) {
      auto * dp_value = std::get_if<EnumDatapointValue>(&datapoint.value);
      if (!dp_value)
      {
        ESP_LOGW(UyatClimate::TAG, "Unexpected datapoint type!");
        return;
      }

      ESP_LOGV(UyatClimate::TAG, "MCU reported Fan Speed Mode is: %u", dp_value->value);
      this->fan_state_ = dp_value->value;
      this->compute_fanmode_();
      this->publish_state();
    });
  }
}

void UyatClimate::loop() {
  bool state_changed = false;
  if (this->active_state_pins_.heating != nullptr) {
    bool heating_state = this->active_state_pins_.heating->digital_read();
    if (heating_state != this->active_state_pins_.heating_state) {
      ESP_LOGV(UyatClimate::TAG, "Heating state pin changed to: %s", ONOFF(heating_state));
      this->active_state_pins_.heating_state = heating_state;
      state_changed = true;
    }
  }
  if (this->active_state_pins_.cooling != nullptr) {
    bool cooling_state = this->active_state_pins_.cooling->digital_read();
    if (cooling_state != this->active_state_pins_.cooling_state) {
      ESP_LOGV(UyatClimate::TAG, "Cooling state pin changed to: %s", ONOFF(cooling_state));
      this->active_state_pins_.cooling_state = cooling_state;
      state_changed = true;
    }
  }

  if (state_changed) {
    this->compute_state_();
    this->publish_state();
  }
}

void UyatClimate::control(const climate::ClimateCall &call) {
  if (call.get_mode().has_value()) {
    const bool switch_state = *call.get_mode() != climate::CLIMATE_MODE_OFF;
    ESP_LOGV(UyatClimate::TAG, "Setting switch: %s", ONOFF(switch_state));
    this->dp_switch_->set_value(switch_state);
    const climate::ClimateMode new_mode = *call.get_mode();

    if (this->dp_active_state_.has_value()) {
      if (!this->dp_active_state_->apply_mode(new_mode))
      {
        ESP_LOGW(UyatClimate::TAG, "Failed to apply mode %d!", new_mode);
      }
    } else {
      ESP_LOGW(UyatClimate::TAG, "Active state (mode) datapoint not configured");
    }
  }

  control_swing_mode_(call);
  control_fan_mode_(call);

  if (call.get_target_temperature().has_value()) {
    float target_temperature = *call.get_target_temperature();
    if (this->reports_fahrenheit_)
      target_temperature = (target_temperature * 9 / 5) + 32;

    ESP_LOGV(UyatClimate::TAG, "Setting target temperature: %.1f", target_temperature);
    this->dp_target_temperature_->set_value(target_temperature);
  }

  if (call.get_preset().has_value()) {
    this->presets_.apply_preset(*call.get_preset());
  }
}

void UyatClimate::control_swing_mode_(const climate::ClimateCall &call) {
  bool vertical_swing_changed = false;
  bool horizontal_swing_changed = false;

  if (call.get_swing_mode().has_value()) {
    const auto swing_mode = *call.get_swing_mode();

    switch (swing_mode) {
      case climate::CLIMATE_SWING_OFF:
        if (swing_vertical_ || swing_horizontal_) {
          this->swing_vertical_ = false;
          this->swing_horizontal_ = false;
          vertical_swing_changed = true;
          horizontal_swing_changed = true;
        }
        break;

      case climate::CLIMATE_SWING_BOTH:
        if (!swing_vertical_ || !swing_horizontal_) {
          this->swing_vertical_ = true;
          this->swing_horizontal_ = true;
          vertical_swing_changed = true;
          horizontal_swing_changed = true;
        }
        break;

      case climate::CLIMATE_SWING_VERTICAL:
        if (!swing_vertical_ || swing_horizontal_) {
          this->swing_vertical_ = true;
          this->swing_horizontal_ = false;
          vertical_swing_changed = true;
          horizontal_swing_changed = true;
        }
        break;

      case climate::CLIMATE_SWING_HORIZONTAL:
        if (swing_vertical_ || !swing_horizontal_) {
          this->swing_vertical_ = false;
          this->swing_horizontal_ = true;
          vertical_swing_changed = true;
          horizontal_swing_changed = true;
        }
        break;

      default:
        break;
    }
  }

  if (vertical_swing_changed && this->swing_vertical_id_.has_value()) {
    ESP_LOGV(UyatClimate::TAG, "Setting vertical swing: %s", ONOFF(swing_vertical_));
    this->parent_->set_boolean_datapoint_value(this->swing_vertical_id_->number, swing_vertical_);
  }

  if (horizontal_swing_changed && this->swing_horizontal_id_.has_value()) {
    ESP_LOGV(UyatClimate::TAG, "Setting horizontal swing: %s", ONOFF(swing_horizontal_));
    this->parent_->set_boolean_datapoint_value(this->swing_horizontal_id_->number, swing_horizontal_);
  }

  // Publish the state after updating the swing mode
  this->publish_state();
}

void UyatClimate::control_fan_mode_(const climate::ClimateCall &call) {
  if (call.get_fan_mode().has_value()) {
    climate::ClimateFanMode fan_mode = *call.get_fan_mode();

    uint8_t uyat_fan_speed;
    switch (fan_mode) {
      case climate::CLIMATE_FAN_LOW:
        uyat_fan_speed = *fan_speed_low_value_;
        break;
      case climate::CLIMATE_FAN_MEDIUM:
        uyat_fan_speed = *fan_speed_medium_value_;
        break;
      case climate::CLIMATE_FAN_MIDDLE:
        uyat_fan_speed = *fan_speed_middle_value_;
        break;
      case climate::CLIMATE_FAN_HIGH:
        uyat_fan_speed = *fan_speed_high_value_;
        break;
      case climate::CLIMATE_FAN_AUTO:
        uyat_fan_speed = *fan_speed_auto_value_;
        break;
      default:
        uyat_fan_speed = 0;
        break;
    }

    if (this->fan_speed_id_.has_value()) {
      this->parent_->set_enum_datapoint_value(this->fan_speed_id_->number, uyat_fan_speed);
    }
  }
}

climate::ClimateTraits UyatClimate::traits() {
  auto traits = climate::ClimateTraits();
  traits.add_feature_flags(climate::CLIMATE_SUPPORTS_ACTION);
  if (this->dp_current_temperature_.has_value()) {
    traits.add_feature_flags(climate::CLIMATE_SUPPORTS_CURRENT_TEMPERATURE);
  }

  if (supports_heat_)
    traits.add_supported_mode(climate::CLIMATE_MODE_HEAT);
  if (supports_cool_)
    traits.add_supported_mode(climate::CLIMATE_MODE_COOL);
  if (this->dp_active_state_->mapping.drying_value.has_value())
    traits.add_supported_mode(climate::CLIMATE_MODE_DRY);
  if (this->dp_active_state_->mapping.fanonly_value.has_value())
    traits.add_supported_mode(climate::CLIMATE_MODE_FAN_ONLY);

  {
    const auto supported_presets = this->presets_.get_supported_presets();
    for (const auto& supported: supported_presets)
    {
      traits.add_supported_preset(supported);
    }
    if (!supported_presets.empty())
    {
      traits.add_supported_preset(climate::CLIMATE_PRESET_NONE);
    }
  }

  if (this->swing_vertical_id_.has_value() && this->swing_horizontal_id_.has_value()) {
    traits.set_supported_swing_modes({climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_BOTH,
                                      climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_SWING_HORIZONTAL});
  } else if (this->swing_vertical_id_.has_value()) {
    traits.set_supported_swing_modes({climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL});
  } else if (this->swing_horizontal_id_.has_value()) {
    traits.set_supported_swing_modes({climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_HORIZONTAL});
  }

  if (fan_speed_id_) {
    if (fan_speed_low_value_)
      traits.add_supported_fan_mode(climate::CLIMATE_FAN_LOW);
    if (fan_speed_medium_value_)
      traits.add_supported_fan_mode(climate::CLIMATE_FAN_MEDIUM);
    if (fan_speed_middle_value_)
      traits.add_supported_fan_mode(climate::CLIMATE_FAN_MIDDLE);
    if (fan_speed_high_value_)
      traits.add_supported_fan_mode(climate::CLIMATE_FAN_HIGH);
    if (fan_speed_auto_value_)
      traits.add_supported_fan_mode(climate::CLIMATE_FAN_AUTO);
  }
  return traits;
}

void UyatClimate::dump_config() {
  LOG_CLIMATE("", "Uyat Climate", this);
  if (this->dp_switch_.has_value()) {
    ESP_LOGCONFIG(UyatClimate::TAG, "  Switch is %s", this->dp_switch_->get_config().to_string().c_str());
  }
  if (this->dp_active_state_.has_value()) {
    ESP_LOGCONFIG(UyatClimate::TAG, "  Active state is %s", this->dp_active_state_->dp_number.get_config().to_string().c_str());
  }
  if (this->dp_target_temperature_.has_value()) {
    ESP_LOGCONFIG(UyatClimate::TAG, "  Target Temperature is %s", this->dp_target_temperature_->get_config().to_string().c_str());
  }
  if (this->dp_current_temperature_.has_value()) {
    ESP_LOGCONFIG(UyatClimate::TAG, "  Current Temperature is %s", this->dp_current_temperature_->get_config().to_string().c_str());
  }
  LOG_PIN("  Heating State Pin: ", this->active_state_pins_.heating);
  LOG_PIN("  Cooling State Pin: ", this->active_state_pins_.cooling);
  this->presets_.dump_config();
  if (this->swing_vertical_id_.has_value()) {
    ESP_LOGCONFIG(UyatClimate::TAG, "  Swing Vertical is %s", this->swing_vertical_id_->to_string().c_str());
  }
  if (this->swing_horizontal_id_.has_value()) {
    ESP_LOGCONFIG(UyatClimate::TAG, "  Swing Horizontal is %s", this->swing_horizontal_id_->to_string().c_str());
  }
  if (this->fan_speed_id_.has_value()) {
    ESP_LOGCONFIG(UyatClimate::TAG, "  Fan Speed is %s", this->fan_speed_id_->to_string().c_str());
  }
}

void UyatClimate::compute_swingmode_() {
  if (this->swing_vertical_ && this->swing_horizontal_) {
    this->swing_mode = climate::CLIMATE_SWING_BOTH;
  } else if (this->swing_vertical_) {
    this->swing_mode = climate::CLIMATE_SWING_VERTICAL;
  } else if (this->swing_horizontal_) {
    this->swing_mode = climate::CLIMATE_SWING_HORIZONTAL;
  } else {
    this->swing_mode = climate::CLIMATE_SWING_OFF;
  }
}

void UyatClimate::compute_fanmode_() {
  if (this->fan_speed_id_.has_value()) {
    // Use state from MCU datapoint
    if (this->fan_speed_auto_value_.has_value() && this->fan_state_ == this->fan_speed_auto_value_) {
      this->fan_mode = climate::CLIMATE_FAN_AUTO;
    } else if (this->fan_speed_high_value_.has_value() && this->fan_state_ == this->fan_speed_high_value_) {
      this->fan_mode = climate::CLIMATE_FAN_HIGH;
    } else if (this->fan_speed_medium_value_.has_value() && this->fan_state_ == this->fan_speed_medium_value_) {
      this->fan_mode = climate::CLIMATE_FAN_MEDIUM;
    } else if (this->fan_speed_middle_value_.has_value() && this->fan_state_ == this->fan_speed_middle_value_) {
      this->fan_mode = climate::CLIMATE_FAN_MIDDLE;
    } else if (this->fan_speed_low_value_.has_value() && this->fan_state_ == this->fan_speed_low_value_) {
      this->fan_mode = climate::CLIMATE_FAN_LOW;
    }
  }
}

void UyatClimate::compute_target_temperature_() {
  if (auto preset_temperature = this->presets_.get_active_preset_temperature())
  {
    this->target_temperature = *preset_temperature;
  }
  else
  {
    this->target_temperature = this->manual_temperature_;
  }
}

void UyatClimate::compute_state_() {
  if (std::isnan(this->current_temperature) || std::isnan(this->target_temperature)) {
    // if any control parameters are nan, go to OFF action (not IDLE!)
    this->switch_to_action_(climate::CLIMATE_ACTION_OFF);
    return;
  }

  if (this->mode == climate::CLIMATE_MODE_OFF) {
    this->switch_to_action_(climate::CLIMATE_ACTION_OFF);
    return;
  }

  climate::ClimateAction target_action = climate::CLIMATE_ACTION_IDLE;
  if (this->active_state_pins_.heating != nullptr || this->active_state_pins_.cooling != nullptr) {
    // Use state from input pins
    if (this->active_state_pins_.heating_state) {
      target_action = climate::CLIMATE_ACTION_HEATING;
      this->mode = climate::CLIMATE_MODE_HEAT;
    } else if (this->active_state_pins_.cooling_state) {
      target_action = climate::CLIMATE_ACTION_COOLING;
      this->mode = climate::CLIMATE_MODE_COOL;
    }

    if (this->dp_active_state_.has_value()) {
      const auto mode_from_dp = this->dp_active_state_->last_value_to_mode();
      if (mode_from_dp)
      {
        this->mode = *mode_from_dp;
      }
    }
  } else if (this->dp_active_state_.has_value()) {
    {
     // Use state & action from MCU datapoint
      const auto mode_from_dp = this->dp_active_state_->last_value_to_mode();
      if (mode_from_dp)
      {
        this->mode = *mode_from_dp;
      }
    }
    {
      const auto action_from_dp = this->dp_active_state_->last_value_to_action();
      if (action_from_dp)
      {
        target_action = *action_from_dp;
      }
    }
  } else {
    // Fallback to active state calc based on temp and hysteresis
    const float temp_diff = this->target_temperature - this->current_temperature;
    if (std::abs(temp_diff) > this->hysteresis_) {
      if (this->supports_heat_ && temp_diff > 0) {
        target_action = climate::CLIMATE_ACTION_HEATING;
        this->mode = climate::CLIMATE_MODE_HEAT;
      } else if (this->supports_cool_ && temp_diff < 0) {
        target_action = climate::CLIMATE_ACTION_COOLING;
        this->mode = climate::CLIMATE_MODE_COOL;
      }
    }
  }

  this->switch_to_action_(target_action);
}

void UyatClimate::switch_to_action_(climate::ClimateAction action) {
  // For now this just sets the current action but could include triggers later
  this->action = action;
}

}  // namespace uyat
}  // namespace esphome
