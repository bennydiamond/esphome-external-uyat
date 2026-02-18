#pragma once

#include "esphome/core/component.h"
#include "esphome/components/light/light_output.h"

#include "../uyat.h"
#include "../dp_number.h"
#include "../dp_switch.h"
#include "../dp_color.h"
#include "../dp_dimmer.h"
#include "uyat_light_common.h"

namespace esphome::uyat
{

class UyatLightDimmer : public EntityBase, public Component, public light::LightOutput {
 public:

  struct Config
  {
    ConfigSwitch switch_config;
    ConfigDimmer dimmer_config;
  };

  explicit UyatLightDimmer(Uyat *parent, Config config);
  void setup() override;
  void dump_config() override;

  light::LightTraits get_traits() override;
  void setup_state(light::LightState *state) override;
  void write_state(light::LightState *state) override;

 private:

  static constexpr const char* TAG = "uyat.light.dimmer";

  void on_dimmer_value(const float);
  void on_switch_value(const bool);
  Uyat& parent_;
  DpSwitch dp_switch_;
  DpDimmer dp_dimmer_;
  std::optional<DpNumber> dimmer_min_value_;
  light::LightState *state_{nullptr};
};

}  // namespace esphome::uyat
