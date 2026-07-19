#pragma once

#include "../uyat_datapoint_types.h"
#include "../dp_color.h"
#include <cstdint>
#include <optional>

namespace esphome::uyat
{

struct ConfigSwitch
{
   MatchingDatapoint switch_dp;
   bool inverted;
   DatapointRetryConfig retry_config;
};

struct ConfigDimmer
{
   MatchingDatapoint dimmer_dp;
   uint32_t min_value;
   uint32_t max_value;
   bool inverted;
   std::optional<MatchingDatapoint> min_value_dp;
   DatapointRetryConfig retry_config;
};

struct ConfigWhiteTemperature
{
   MatchingDatapoint white_temperature_dp;
   uint32_t min_value;
   uint32_t max_value;
   bool inverted;
   float cold_white_temperature;
   float warm_white_temperature;
   DatapointRetryConfig retry_config;
};

struct ConfigColor
{
   MatchingDatapoint color_dp;
   UyatColorType color_type;
   DatapointRetryConfig retry_config;
};

}
