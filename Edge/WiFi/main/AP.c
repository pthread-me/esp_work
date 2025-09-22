//
//

#define LOCAL_LOG_LEVEL ESP_LOG_VERBOSE
static const char* TAG = "MyModule";

#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_wifi_types_generic.h"
#include "nvs_flash.h"

void app_main(void){
	// wifi modules will be stored in non-volatile mem
	if(nvs_flash_init() != ESP_OK){
		ESP_LOGE(TAG, "Could not init non-volatile memory partition\n");
		return;
	}

	// default wifi module init
	wifi_init_config_t wifi_config_param = WIFI_INIT_CONFIG_DEFAULT();
	if(esp_wifi_init(&wifi_config_param) != ESP_OK){
		ESP_LOGE(TAG, "Could not initialize wifi modules\n");
		return; 
	} 

	if(esp_wifi_set_mode(WIFI_MODE_STA) != ESP_OK){
		
	}


	if(esp_wifi_deinit() != ESP_OK){
		ESP_LOGE(TAG, "Could not free wifi resources, might want to erase flash\n");
		return; 
	}
	ESP_LOGI(TAG, "Complete\n");
}
