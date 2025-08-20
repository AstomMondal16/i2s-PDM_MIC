#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_vfs_fat.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s_pdm.h"
#include "driver/gpio.h"
#include "driver/spi_common.h"
#include "sdmmc_cmd.h"
#include "pdm_mic.h"
#include "format_wav.h"

static const char *TAG = "PDM_MIC";

sdmmc_host_t host = SDSPI_HOST_DEFAULT();
sdmmc_card_t *card;
i2s_chan_handle_t rx_handle = NULL;

static int16_t i2s_readraw_buff[SAMPLE_SIZE];
size_t bytes_read;
const int WAVE_HEADER_SIZE = 44;

void mount_sdcard(void) {
    esp_err_t ret;
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 8 * 1024
    };

    ESP_LOGI(TAG, "Initializing SD card");

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = SPI_MOSI_GPIO,
        .miso_io_num = SPI_MISO_GPIO,
        .sclk_io_num = SPI_SCLK_GPIO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };

    ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus.");
        return;
    }

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = SPI_CS_GPIO;
    slot_config.host_id = SPI2_HOST;

    ret = esp_vfs_fat_sdspi_mount(SD_MOUNT_POINT, &host, &slot_config, &mount_config, &card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount filesystem: %s", esp_err_to_name(ret));
        return;
    }

    sdmmc_card_print_info(stdout, card);
}

void record_wav(uint32_t rec_time) {
    ESP_LOGI(TAG, "Opening file");

    uint32_t flash_rec_time = BYTE_RATE * rec_time;
    const wav_header_t wav_header = WAV_HEADER_PCM_DEFAULT(flash_rec_time, 16, SAMPLE_RATE, 1);

    struct stat st;
    if (stat(RECORDED_FILE_PATH, &st) == 0) {
        f_unlink(RECORDED_FILE_PATH);
    }

    FILE *f = fopen(RECORDED_FILE_PATH, "w");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }

    fwrite(&wav_header, sizeof(wav_header), 1, f);

    int flash_wr_size = 0;
    while (flash_wr_size < flash_rec_time) {
        if (i2s_channel_read(rx_handle, (char *)i2s_readraw_buff, SAMPLE_SIZE, &bytes_read, 1000) == ESP_OK) {
            for (int i = 0; i < SAMPLE_SIZE; i++) {
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////                
                i2s_readraw_buff[i] *= 1.5; // Volume Gain suggested to 1.5 for better quality 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////            
            }
            printf("[0] %d [1] %d [2] %d [3]%d ...\n", i2s_readraw_buff[0], i2s_readraw_buff[1], i2s_readraw_buff[2], i2s_readraw_buff[3]);

            fwrite(i2s_readraw_buff, bytes_read, 1, f);
            flash_wr_size += bytes_read;
        } else {
            ESP_LOGE(TAG, "I2S Read Failed!");
        }
    }

    fclose(f);
    ESP_LOGI(TAG, "Recording complete. File saved to SD card.");

    esp_vfs_fat_sdcard_unmount(SD_MOUNT_POINT, card);
    ESP_LOGI(TAG, "SD Card unmounted");

    spi_bus_free(SPI2_HOST);
}

void init_microphone(void) {
    gpio_set_level(GPIO_NUM_4, 0);

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, NULL, &rx_handle));

    i2s_pdm_rx_config_t pdm_rx_cfg = {
        .clk_cfg = I2S_PDM_RX_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_PDM_RX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .clk = I2S_CLK_GPIO,
            .din = I2S_DATA_GPIO,
            .invert_flags = {.clk_inv = false},
        },
    };

    ESP_ERROR_CHECK(i2s_channel_init_pdm_rx_mode(rx_handle, &pdm_rx_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(rx_handle));
    ESP_LOGI(TAG, "Microphone Initialized");
}

void test(void) {
    ESP_LOGI(TAG, "Testing microphone...");

    size_t bytes_read;
    int16_t buffer[SAMPLE_SIZE];

    if (i2s_channel_read(rx_handle, (char *)buffer, SAMPLE_SIZE, &bytes_read, 1000) == ESP_OK) {
        ESP_LOGI(TAG, "Read %d bytes", bytes_read);
        for (int i = 0; i < 10; i++) {
            ESP_LOGI(TAG, "Sample %d: %d", i, buffer[i]);
        }
    } else {
        ESP_LOGE(TAG, "I2S Read Failed!");
    }
}
