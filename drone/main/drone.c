#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>

#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 18
#define PIN_NUM_CLK 4
#define PIN_NUM_CSN 2
#define PIN_NUM_CE 15

static spi_device_handle_t nrf;

// Low-level SPI transfer
uint8_t nrf_cmd(uint8_t cmd)
{
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &cmd
    };
    uint8_t rx = 0;
    t.rx_buffer = &rx;
    spi_device_transmit(nrf, &t);
    return rx;
}

// Read RX payload (up to 32 bytes)
void nrf_read_payload(uint8_t *data, uint8_t len)
{
    uint8_t cmd = 0x61;  // R_RX_PAYLOAD
    uint8_t tmp[33] = {0};

    spi_transaction_t t = {
        .length = (len + 1) * 8,
        .tx_buffer = &cmd,
        .rx_buffer = tmp,
    };
    spi_device_transmit(nrf, &t);

    memcpy(data, &tmp[1], len);
}

// Write register
void nrf_write_reg(uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = { (uint8_t)(0x20 | reg), value };

    spi_transaction_t t = {
        .length = 16,
        .tx_buffer = buf
    };
    spi_device_transmit(nrf, &t);
}

// Write RX address (5 bytes)
void nrf_write_reg_long(uint8_t reg, uint8_t *buf)
{
    uint8_t out[6];
    out[0] = 0x20 | reg;
    memcpy(&out[1], buf, 5);

    spi_transaction_t t = {
        .length = 48,
        .tx_buffer = out
    };
    spi_device_transmit(nrf, &t);
}

// Init RX mode
void nrf_init_rx()
{
    // CE low
    gpio_set_direction(PIN_NUM_CE, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_NUM_CE, 0);

    // CONFIG register
    // PWR_UP = 1
    // PRIM_RX = 1  (receiver mode)
    // CRC = 1 byte
    nrf_write_reg(0x00, 0x0B);

    // Enable data pipe 0
    nrf_write_reg(0x02, 0x01);

    // Disable auto-ack (simple mode)
    nrf_write_reg(0x01, 0x00);

    // RF channel (same as transmitter)
    nrf_write_reg(0x05, 40);

    // RF setup: 1 Mbps, 0 dBm
    nrf_write_reg(0x06, 0x06);

    // Set RX address (same as TX address)
    uint8_t addr[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
    nrf_write_reg_long(0x0A, addr);  // RX_ADDR_P0

    // Payload size: 1 byte
    nrf_write_reg(0x11, 1);

    // Flush RX
    uint8_t flush = 0xE2;
    nrf_cmd(flush);

    // Enter RX mode
    gpio_set_level(PIN_NUM_CE, 1);

    printf("nRF24L01+ Receiver Initialized.\n");
}

void app_main()
{
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .max_transfer_sz = 64
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 8 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = PIN_NUM_CSN,
        .queue_size = 1,
    };

    // Init SPI
    spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    spi_bus_add_device(SPI2_HOST, &devcfg, &nrf);

    // Init nRF24 in RX mode
    nrf_init_rx();

    uint8_t received = 0;

    while (1)
    {
        // Check FIFO status
        uint8_t status = nrf_cmd(0xFF);  // NOP (returns STATUS)

        if (status & 0x40)  // RX_DR flag
        {
            // Read one byte
            nrf_read_payload(&received, 1);

            printf("Received: %d\n", received);

            // Clear interrupt flags
            nrf_write_reg(0x07, 0x40);
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
