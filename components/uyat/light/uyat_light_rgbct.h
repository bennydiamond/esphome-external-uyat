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

class UyatLightRGBCT : public EntityBase, public Component, public light::LightOutput {
 public:

  struct Config
  {
    ConfigSwitch switch_config;
    ConfigDimmer dimmer_config;
    ConfigColor color_config;
    ConfigWhiteTemperature wt_config;
    bool color_interlock;
  };

  explicit UyatLightRGBCT(Uyat *parent, Config config);
  void setup() override;
  void dump_config() override;

  light::LightTraits get_traits() override;
  void setup_state(light::LightState *state) override;
  void write_state(light::LightState *state) override;

 private:

  static constexpr const char* TAG = "uyat.light.rgbct";

  struct Dimmer
  {
    DpDimmer dimmer;
    std::optional<DpNumber> min_value_number;
  };

  void on_dimmer_value(const float);
  void on_white_temperature_value(const float);
  void on_switch_value(const bool);
  void on_color_value(const DpColor::Value&);

  Uyat& parent_;
  DpSwitch dp_switch_;
  DpDimmer dp_dimmer_;
  std::optional<DpNumber> dimmer_min_value_;
  DpColor dp_color_;
  DpDimmer dp_white_temperature_;
  const float cold_white_temperature_;
  const float warm_white_temperature_;
  const bool color_interlock_{false};
  light::LightState *state_{nullptr};
};

}  // namespace esphome::uyat
