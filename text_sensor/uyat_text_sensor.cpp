#include "esphome/core/log.h"
#include "esphome/core/entity_base.h"

#include "uyat_text_sensor.h"

namespace esphome {
namespace uyat {

static const char *const TAG = "uyat.text_sensor";

void UyatTextSensor::configure_raw_dp(const uint8_t dp_id, const bool base64_encoded , const bool as_hex)
{
  this->dp_text_sensor_.emplace(std::move(DpTextSensor::create_for_raw([this](const std::string& value){on_value(value);}, dp_id, base64_encoded, as_hex)));
}

void UyatTextSensor::configure_string_dp(const uint8_t dp_id, const bool base64_encoded, const bool as_hex)
{
  this->dp_text_sensor_.emplace(std::move(DpTextSensor::create_for_string([this](const std::string& value){on_value(value);}, dp_id, base64_encoded, as_hex)));
}

void UyatTextSensor::setup() {
  assert(this->parent_);
  this->dp_text_sensor_->init(*(this->parent_));
}

void UyatTextSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "Uyat Text Sensor:");
  ESP_LOGCONFIG(TAG, "  Text Sensor %s is %s", get_object_id().c_str(), this->dp_text_sensor_? this->dp_text_sensor_->config_to_string().c_str() : "misconfigured!");
}

void UyatTextSensor::on_value(const std::string& value)
{
  ESP_LOGV(TAG, "MCU reported %s is: %s", get_object_id().c_str(), value.c_str());
  this->publish_state(value);
}

std::string UyatTextSensor::get_object_id() const
{
  char object_id_buf[OBJECT_ID_MAX_LEN];
  return this->get_object_id_to(object_id_buf).str();
}


}  // namespace uyat
}  // namespace esphome
