#pragma once

#include <functional>

#include "uyat_datapoint_types.h"

namespace esphome::uyat
{

class DatapointRetry
{
 public:
  using RetryCallback = std::function<bool()>;

  DatapointRetry(uint8_t datapoint_id, const DatapointRetryConfig &config, const char *log_tag)
      : datapoint_id_(datapoint_id), retry_config_(config), log_tag_(log_tag) {}

  void init(DatapointHandler *handler, RetryCallback retry_callback)
  {
    this->handler_ = handler;
    this->retry_callback_ = std::move(retry_callback);
    this->retries_left_ = 0;
  }

  void schedule_if_needed(const bool already_in_retry_sequence, const bool value_changed)
  {
    if (already_in_retry_sequence) {
      return;
    }

    if (!this->retry_config_.enabled || this->retry_config_.count == 0u || !value_changed) {
      return;
    }

    this->retries_left_ = this->retry_config_.count;
    start_retry_timeout_();
  }

  void cancel()
  {
    cancel_retry_timeout_();
  }

 private:
  void start_retry_timeout_()
  {
    if (this->handler_ == nullptr || this->retries_left_ == 0u) {
      return;
    }

    ESP_LOGD(this->log_tag_, "[DP%u] Scheduling retry timeout: %ums (%u retries left)",
             this->datapoint_id_, this->retry_config_.timeout_ms, this->retries_left_);

    this->handler_->schedule_datapoint_retry_timeout(
        this->datapoint_id_, this->retry_config_.timeout_ms, [this]() { this->on_retry_timeout_(); });
  }

  void cancel_retry_timeout_()
  {
    if (this->handler_ == nullptr) {
      return;
    }

    ESP_LOGV(this->log_tag_, "[DP%u] Canceled retry timeout", this->datapoint_id_);
    this->handler_->cancel_datapoint_retry_timeout(this->datapoint_id_);
  }

  void on_retry_timeout_()
  {
    if (this->retries_left_ == 0u) {
      ESP_LOGE(this->log_tag_, "[DP%u] Retry timeout: no retries left", this->datapoint_id_);
      return;
    }

    if (!this->retry_callback_) {
      ESP_LOGE(this->log_tag_, "[DP%u] Retry timeout: no retry callback", this->datapoint_id_);
      return;
    }

    if (!this->retry_callback_()) {
      ESP_LOGE(this->log_tag_, "[DP%u] Retry timeout: no value to resend", this->datapoint_id_);
      return;
    }

    this->retries_left_--;
    ESP_LOGW(this->log_tag_, "[DP%u] MCU did not confirm value, retrying send (%u retries left)",
             this->datapoint_id_, this->retries_left_);
  }

  DatapointRetryConfig retry_config_{};
  uint8_t retries_left_{0};
  uint8_t datapoint_id_{0};
  DatapointHandler *handler_{nullptr};
  const char *log_tag_{nullptr};
  RetryCallback retry_callback_;
};

}
