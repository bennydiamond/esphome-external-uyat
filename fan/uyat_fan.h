#pragma once

#include "esphome/core/component.h"
#include "esphome/components/fan/fan.h"

#include "../uyat.h"
#include "../dp_switch.h"
#include "../dp_number.h"

namespace esphome {
namespace uyat {

class UyatFan : public Component, public fan::Fan {
 private:
  void on_speed_value(const float);
  void on_switch_value(const bool);
  void on_oscillation_value(const bool);
  void on_direction_value(const bool);

 public:
  UyatFan(Uyat *parent) : parent_(parent) {}
  void setup() override;
  void dump_config() override;
  void configure_speed(MatchingDatapoint matching_dp, const uint32_t min_value, const uint32_t max_value)
  {
    this->speed_.emplace(
      DpNumber{
        [this](const float value){ this->on_speed_value(value); },
        std::move(matching_dp),
        0.0f, 1.0f
      },
      min_value,
      max_value
    );
  }
  void configure_switch(MatchingDatapoint matching_dp, const bool inverted)
  {
    this->dp_switch_.emplace(
      [this](const bool value){ this->on_switch_value(value); },
      std::move(matching_dp),
      inverted
    );
  }
  void configure_oscillation(MatchingDatapoint matching_dp, const bool inverted)
  {
    this->dp_oscillation_.emplace(
      [this](const bool value){ this->on_oscillation_value(value); },
      std::move(matching_dp),
      inverted
    );
  }
  void configure_direction(MatchingDatapoint matching_dp, const bool inverted)
  {
    this->dp_direction_.emplace(
      [this](const bool value){ this->on_direction_value(value); },
      std::move(matching_dp),
      inverted
    );
  }

  fan::FanTraits get_traits() override;

 protected:

  struct SpeedConfig
  {
    DpNumber dp_speed;
    uint32_t min_value;
    uint32_t max_value;

    int get_number_of_speeds() const
    {
      return max_value - min_value + 1;
    }
  };

  void control(const fan::FanCall &call) override;

  Uyat *parent_;

  std::optional<SpeedConfig> speed_{};
  std::optional<DpSwitch> dp_switch_{};
  std::optional<DpSwitch> dp_oscillation_{};
  std::optional<DpSwitch> dp_direction_{};
};

}  // namespace uyat
}  // namespace esphome
