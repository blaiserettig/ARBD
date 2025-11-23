#include "driver/gpio.h"
#include "freertos/FreeRTOS.h" // Needed for vTaskDelay
#include "freertos/task.h"     // Needed for vTaskDelay
#include <stdio.h>
#include <esp_adc/adc_oneshot.h>

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

    adc_oneshot_unit_handle_t adc2_unit;
    adc_oneshot_unit_init_cfg_t adc2_init = {
        .unit_id = ADC_UNIT_2,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc2_init, &adc2_unit));

    adc_oneshot_chan_cfg_t chan_cfg = {
    .atten = ADC_ATTEN_DB_12,
    .bitwidth = ADC_BITWIDTH_12};

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc2_unit, ADC_CHANNEL_2, &chan_cfg));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc2_unit, ADC_CHANNEL_0, &chan_cfg));

    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Wait 1 second
        int x_out = 0;
        int y_out = 0;

        adc_oneshot_read(adc2_unit, ADC_CHANNEL_2, &x_out);
        adc_oneshot_read(adc2_unit, ADC_CHANNEL_0, &y_out);

        printf("x-axis: %d  y-axis: %d\n",
            x_out, y_out);
    }
}
