#pragma once

#include "esphome/core/component.h"
#include "esphome/components/light/light_output.h"

#include "../uyat.h"
#include "../dp_switch.h"
#include "../dp_dimmer.h"
#include "uyat_light_common.h"

namespace esphome::uyat
{

class UyatLightRGB : public EntityBase, public Component, public light::LightOutput {
 public:

  struct Config
  {
    ConfigSwitch switch_config;
    ConfigColor color_config;
  };

  explicit UyatLightRGB(Uyat *parent, Config config);
  void setup() override;
  void dump_config() override;

  light::LightTraits get_traits() override;
  void setup_state(light::LightState *state) override;
  void write_state(light::LightState *state) override;

 private:

  static constexpr const char* TAG = "uyat.light.rgb";

  void on_switch_value(const bool);
  void on_color_value(const DpColor::Value&);

  Uyat& parent_;
  DpSwitch dp_switch_;
  DpColor dp_color_;
  light::LightState *state_{nullptr};
};

}  // namespace esphome::uyat
