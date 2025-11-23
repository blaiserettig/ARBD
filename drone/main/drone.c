#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "pid_controller.h"
#include <sys/time.h>

#include "esp_adc/adc_oneshot.h"
/**
 * Time in nano seconds
 */
uint64_t getHighResTime() {
  static struct timeval tv_now;
  gettimeofday(&tv_now, NULL);

  return (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
}

void app_main(void) {
  gpio_config_t d2_config;
  d2_config.mode = GPIO_MODE_OUTPUT;
  d2_config.pin_bit_mask = 1ULL << GPIO_NUM_2;
  d2_config.pull_up_en = GPIO_PULLUP_ENABLE;
  d2_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
  d2_config.intr_type = GPIO_INTR_DISABLE;
  gpio_config(&d2_config);
  gpio_set_level(GPIO_NUM_2, 1);

  pid_controller_t led_pid = {.kp = 1.0, .ki = 0, .kd = 0, .integral = 0};

  adc_oneshot_unit_handle_t adc1_handle;
  adc_oneshot_unit_init_cfg_t init_config1 = {
      .unit_id = ADC_UNIT_1,
      .ulp_mode = ADC_ULP_MODE_DISABLE,
  };
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

  uint64_t lastTime = getHighResTime();
  float currentLEDBrightness = 0;
  while (1) {
    uint64_t currentTime = getHighResTime();
    float delta = (currentTime - lastTime) / 1000.0;
    // ######################################
    int expectedPotVal = 0;
    adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &expectedPotVal);
    gpio_set_level(GPIO_NUM_2, expectedPotVal > 50 ? 1 : 0);

    float err = expectedPotVal - currentLEDBrightness;
    float correction = update_pid(&led_pid, err, delta);
    currentLEDBrightness += correction * delta;

    // ######################################
    lastTime = currentTime;
  }
}
