#include "driver/gpio.h"
#include "freertos/FreeRTOS.h" // Needed for vTaskDelay
#include "freertos/task.h"     // Needed for vTaskDelay
#include <stdio.h>

void app_main(void) {
    // ... (Your gpio_config code remains here) ...
    gpio_config_t d2_config = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = 1ULL << GPIO_NUM_2,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config_t d4_config = {
        .pin_bit_mask = 1ULL << GPIO_NUM_4,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config(&d2_config);
    gpio_config(&d4_config);

    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Wait 1 second
        printf("x-axis: %d  y-axis: %d\n",
            gpio_get_level(GPIO_NUM_2),
            gpio_get_level(GPIO_NUM_4));
    }
}
