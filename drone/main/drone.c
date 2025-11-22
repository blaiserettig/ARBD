#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include <stdio.h>
// void run_on_event(void* handler_arg, esp_event_base_t base, int32_t id, void*
// event_data) {

void app_main(void) {
  gpio_config_t d2_config;
  d2_config.mode = GPIO_MODE_OUTPUT;
  d2_config.pin_bit_mask = 1UL << 23;
  d2_config.pull_up_en = GPIO_PULLUP_ENABLE;
  gpio_config(&d2_config);
}
