#include "remote_log.h"
#include "esp_log.h"

#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"

static const char *TAG = "Remote log common";

static RingbufHandle_t ringbuffer_log_handle = NULL;


esp_err_t log_heap(void *out, size_t *len)
{
    size_t data = heap_caps_get_free_size(MALLOC_CAP_8BIT);

    // boilerplate   
    memcpy(out,&data,sizeof(data));
    *len = sizeof(data);
    return ESP_OK;
}

esp_err_t log_xring(void *out, size_t *len)
{
    size_t data = xRingbufferGetCurFreeSize(ringbuffer_log_handle);
    
    // boilerplate   
    memcpy(out,&data,sizeof(data));
    *len = sizeof(data);
    return ESP_OK;
}


void remote_log_common_init(uint8_t tx_pin, uint8_t rx_pin)
{
  remote_log_config cfg = {
    .transport_type = REMOTE_LOG_UART,
    .uart_num = 1,
    .baud_rate = 9600,
    .tx_pin = tx_pin,
    .rx_pin = rx_pin,
    .log_frequency_ms = 100
  };
  remote_log_init(&cfg);
  ESP_LOGI(TAG, "Remote Log Common initialized.");
}


void add_heap_tracking(uint8_t log_id, void *param)
{
  remote_log_register_t heap_log = {
    .log_id = log_id,
    .tag = "Heap tracking",
    .data_log_cb = log_heap
  };
  remote_log_register_log(heap_log);
  ESP_LOGI(TAG, "Heap tracking added");
}

void add_xring_tracking(uint8_t log_id, void *param)
{
  if(!param) {
    ESP_LOGE(TAG, "NULL passed as param");
  }

  ringbuffer_log_handle = *(RingbufHandle_t *)param;
  
  if(ringbuffer_log_handle != NULL) {
    ESP_LOGI(TAG, "got value");
  }
  else {
    ESP_LOGE(TAG, "Error NULL handle");
  }
  
  remote_log_register_t xring_log = {
    .log_id = log_id,
    .tag = "xRing free space",
    .data_log_cb = log_xring
  };
  remote_log_register_log(xring_log);
  ESP_LOGI(TAG, "xRingBuffer tracking added");
}

void remote_log_common_add(remote_log_common_t type, uint8_t log_id, void *param)
{
  switch (type) {
    case REMOTE_LOG_TRACK_HEAP_SIZE:
      add_heap_tracking(log_id, param);
      break;
    
    case REMOTE_LOG_TRACK_XRING_SIZE:
      add_xring_tracking(log_id, param);
      break;
    
    default:
      ESP_LOGE(TAG, "Error added tracking");
      break;
  }
}
