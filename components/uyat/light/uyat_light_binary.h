#pragma once

#include "esphome/core/component.h"
#include "esphome/components/light/light_output.h"

#include "../uyat.h"
#include "../dp_switch.h"
#include "uyat_light_common.h"

namespace esphome::uyat
{

class UyatLightBinary : public EntityBase, public Component, public light::LightOutput {
 public:

  struct Config
  {
    ConfigSwitch switch_config;
  };

  explicit UyatLightBinary(Uyat *parent, Config config);
  void setup() override;
  void dump_config() override;

  light::LightTraits get_traits() override;
  void setup_state(light::LightState *state) override;
  void write_state(light::LightState *state) override;

 private:

  static constexpr const char* TAG = "uyat.light.binary";

  void on_switch_value(const bool);
  Uyat& parent_;
  DpSwitch dp_switch_;
  light::LightState *state_{nullptr};
};

}  // namespace esphome::uyat
