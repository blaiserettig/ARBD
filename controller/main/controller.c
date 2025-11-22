#include "driver/gpio.h"
#include "freertos/FreeRTOS.h" // Needed for vTaskDelay
#include "freertos/task.h"     // Needed for vTaskDelay
#include <stdio.h>

void app_main(void) {
  // ... (Your gpio_config code remains here) ...
  gpio_config_t d2_config = {.mode = GPIO_MODE_OUTPUT,
                             .pin_bit_mask = 1ULL << GPIO_NUM_2,
                             .pull_up_en = GPIO_PULLUP_DISABLE,
                             .pull_down_en = GPIO_PULLDOWN_DISABLE,
                             .intr_type = GPIO_INTR_DISABLE};
  gpio_config(&d2_config);

  while (1) {
    // Turn LED ON (Set HIGH)
    gpio_set_level(GPIO_NUM_2, 1);
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Wait 1 second

    // Turn LED OFF (Set LOW)
    gpio_set_level(GPIO_NUM_2, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Wait 1 second
  }
}
