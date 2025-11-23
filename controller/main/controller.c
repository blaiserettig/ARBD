#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>
#include "driver/spi_master.h"

#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 18
#define PIN_NUM_CLK 4
#define PIN_NUM_CSN 2
#define PIN_NUM_CE 15

static spi_device_handle_t nrf_handle;

// (WRITE COMMAND | reg num) (val)
void nrf_write_reg(const uint8_t reg, const uint8_t data) {
    uint8_t buf[2] = {(uint8_t) (0x20 | reg), data};
    spi_transaction_t t = {
        .length = 16,
        .tx_buffer = buf,
    };
    spi_device_transmit(nrf_handle, &t);
}

void nrf_write_payload(const uint8_t *data, const uint8_t len) {
    uint8_t buf[33];
    buf[0] = 0xA0;
    memcpy(&buf[1], data, len);

    spi_transaction_t t = {
        .length = (len + 1) * 8,
        .tx_buffer = buf,
    };

    spi_device_transmit(nrf_handle, &t);
}

void nrf_init() {
    // CE -> chip enable
    // controls RADIO mode (TX, RX, stby)
    // radio will stay off until CE goes high
    gpio_set_direction(PIN_NUM_CE, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_NUM_CE, 0);

    // 0x00 = CONFIG
    // 0x0A = 00001010
    // power up, transmit mode
    nrf_write_reg(0x00, 0x0A);

    // diable auto ack
    nrf_write_reg(0x01, 0x00);

    // disable rx pipes
    nrf_write_reg(0x02, 0x00);

    // set radio freq
    // 2.4 Ghz + 40 Mhz
    nrf_write_reg(0x05, 40);

    // 0dBm transmit power
    // 1 Mbps bitrate
    nrf_write_reg(0x06, 0x06);

    // hardcoded pipe the transmitter transmits on
    // receiver has to listen on same addr
    uint8_t addr[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
    spi_transaction_t t = {
        .length = 8 * 6,
        .tx_buffer = (uint8_t[6]){0x20 | 0x10, addr[0], addr[1], addr[2], addr[3], addr[4]}
    };
    spi_device_transmit(nrf_handle, &t);

    // clear leftover data in radio
    uint8_t cmd = 0xE1;
    spi_transaction_t flush = {
        .length = 8,
        .tx_buffer = &cmd,
    };
    spi_device_transmit(nrf_handle, &flush);

    vTaskDelay(5 / portTICK_PERIOD_MS);

    printf("nrf initialized.\n");
}

void app_main(void) {
    esp_err_t ret;

    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 64
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 8 * 1000 * 1000, // 8mhz
        .mode = 0,
        .spics_io_num = PIN_NUM_CSN,
        .queue_size = 1
    };

    // power up spi hardware
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

    // nrf handle now represents radio on spi bus
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &nrf_handle));

    nrf_init();

    uint8_t msg = 0;

    while (1) {
        printf("sending %d\n", msg);

        gpio_set_level(PIN_NUM_CE, 1); // radio on
        vTaskDelay(1);
        nrf_write_payload(&msg, 1);
        gpio_set_level(PIN_NUM_CE, 0); // radio off

        msg++;

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
