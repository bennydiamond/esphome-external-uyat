#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

#include "../uyat.h"
#include "../dp_vap.h"

#include <optional>

namespace esphome::uyat
{

enum class UyatVAPValueType {
  VOLTAGE,
  AMPERAGE,
  POWER,
};

class UyatSensorVAP : public sensor::Sensor, public Component {
 private:
  void on_value(const DpVAP::VAPValue&);
  std::string get_object_id() const;

 public:

  void setup() override;
  void dump_config() override;
  void configure_raw_dp(const uint8_t dp_id, const UyatVAPValueType value_type);

  void set_uyat_parent(Uyat *parent) { this->parent_ = parent; }

 protected:
  Uyat *parent_;
  std::optional<DpVAP> dp_vap_;
  UyatVAPValueType value_type_{UyatVAPValueType::VOLTAGE};
};

}  // namespace
