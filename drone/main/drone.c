#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/adc_types.h"
#include "hal/gpio_types.h"
#include "pid_controller.h"

#include "driver/dac_oneshot.h"
#include "esp_adc/adc_oneshot.h"

#include "utils.h"

void app_main(void) {

  float lastTime = getHRTimeF();
  while (1) {
    float currentTime = getHRTimeF();
    float delta = currentTime - lastTime;
    // ######################################

    // ######################################
    lastTime = currentTime;
    vTaskDelay(32 / portTICK_PERIOD_MS);
  }
}
