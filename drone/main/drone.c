#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/adc_types.h"
#include "hal/gpio_types.h"
#include "pid_controller.h"
#include <sys/time.h>

#include "driver/dac_oneshot.h"
#include "esp_adc/adc_oneshot.h"

/**
 * Time in nano seconds (High Resolution Time)
 */
uint64_t getHRTime() {
  static struct timeval tv_now;
  gettimeofday(&tv_now, NULL);

  return (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
}

/**
 * Time in seconds (High Resolution Time)
 */
float getHRTimeF() { return (float)(getHRTime() / 1000000.0); }

void app_main(void) {
  gpio_config_t d2_config;
  d2_config.mode = GPIO_MODE_OUTPUT;
  d2_config.pin_bit_mask = 1ULL << GPIO_NUM_2;
  d2_config.pull_up_en = GPIO_PULLUP_ENABLE;
  d2_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
  d2_config.intr_type = GPIO_INTR_DISABLE;
  gpio_config(&d2_config);
  gpio_set_level(GPIO_NUM_2, 1);

  pid_controller_t led_pid = {.kp = .8, .ki = .01, .kd = 0.2, .integral = 0};

  adc_oneshot_unit_handle_t adc1_handle;
  adc_oneshot_unit_init_cfg_t init_config1 = {
      .unit_id = ADC_UNIT_1,
      .ulp_mode = ADC_ULP_MODE_DISABLE,
  };
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));
  adc_oneshot_chan_cfg_t config = {
      .bitwidth = ADC_BITWIDTH_12,
      .atten = ADC_ATTEN_DB_12,
  };
  ESP_ERROR_CHECK(
      adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &config));

  dac_oneshot_handle_t chan0_handle;
  dac_oneshot_config_t chan0_cfg = {
      .chan_id = DAC_CHAN_0,
  };
  ESP_ERROR_CHECK(dac_oneshot_new_channel(&chan0_cfg, &chan0_handle));

  float lastTime = getHRTimeF();
  float currentLEDBrightness = 0;
  while (1) {
    float currentTime = getHRTimeF();
    float delta = currentTime - lastTime;
    // ######################################
    int expectedPotVal = 0;
    adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &expectedPotVal);
    expectedPotVal = 120 + (expectedPotVal / 128); // x/2^6
    // gpio_set_level(GPIO_NUM_2, expectedPotVal > 50 ? 1 : 0);

    float err = expectedPotVal - currentLEDBrightness;
    float correction = update_pid(&led_pid, err, delta);
    currentLEDBrightness += correction * delta;
    dac_oneshot_output_voltage(chan0_handle, (uint8_t)currentLEDBrightness);

    // ######################################
    lastTime = currentTime;
    vTaskDelay(32 / portTICK_PERIOD_MS);
  }
}
