#include "pdm_mic.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "APP_MAIN";

void app_main(void) {
    ESP_LOGI(TAG, "Starting application");
    
    // Initialize the microphone
    init_microphone();
    
    // Mount the SD card
    mount_sdcard();
    
    // Define recording time in seconds
    uint32_t recording_time = 10; // Adjust as needed
    
    
    ESP_LOGI(TAG, "Starting recording for %lu seconds", (unsigned long)recording_time);
    record_wav(recording_time);

    // test();  // Test the mic
    //vTaskDelay(pdMS_TO_TICKS(2000));
    
    ESP_LOGI(TAG, "Recording completed");
}
