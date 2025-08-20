#ifndef PDM_MIC_H
#define PDM_MIC_H

#include <stdio.h>
#include "esp_err.h"
#include "driver/i2s_pdm.h"
#include "driver/spi_common.h"
#include "sdmmc_cmd.h"

// Constants
#define SAMPLE_RATE         32000
#define BIT_SAMPLE          32
#define NUM_CHANNELS        1 // Mono Recording
#define SD_MOUNT_POINT      "/sdcard"
#define SAMPLE_SIZE         (BIT_SAMPLE * 1024)
#define BYTE_RATE           (SAMPLE_RATE * (BIT_SAMPLE / 8)) * NUM_CHANNELS

// SPI GPIOs for SD card
#define SPI_MOSI_GPIO       GPIO_NUM_17
#define SPI_SCLK_GPIO       GPIO_NUM_33
#define SPI_MISO_GPIO       GPIO_NUM_34
#define SPI_CS_GPIO         GPIO_NUM_18

// I2S GPIOs for PDM mic
#define I2S_CLK_GPIO        GPIO_NUM_2
#define I2S_DATA_GPIO       GPIO_NUM_1

// File path
#define RECORDED_FILE_PATH  SD_MOUNT_POINT "/test_1.wav"

// External Variables
extern sdmmc_host_t host;
extern i2s_chan_handle_t rx_handle;
extern const int WAVE_HEADER_SIZE;

// Function Declarations
void mount_sdcard(void);
void record_wav(uint32_t rec_time);
void init_microphone(void);
void test(void);

#endif // PDM_MIC_H
