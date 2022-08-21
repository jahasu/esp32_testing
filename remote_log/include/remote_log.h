#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "esp_err.h"

#include "remote_log_common.h"


#define MAX_LOGS 10
#define MAX_DATA_SIZE 100
#define MAX_LOG_SIZE 150

#define ID_SEND_INTERVAL 100
#define EVENT_QUEUE_SZ 30


typedef enum {
  REMOTE_LOG_UDP = 1,
  REMOTE_LOG_UART
} remote_log_transport_type;

typedef enum {
  REMOTE_LOG_ID,
  REMOTE_LOG_DATA,
  REMOTE_LOG_EVENT
} remote_log_type;

typedef struct {
  remote_log_transport_type transport_type;
  uint32_t log_frequency_ms;
  // for UDP
  char *ip;
  uint32_t port;
  // for UART
  uint8_t uart_num;
  uint32_t baud_rate;
  uint8_t tx_pin;
  uint8_t rx_pin;
} remote_log_config;

typedef struct {
  remote_log_type log_type;
  uint8_t log_id;
  uint32_t timestamp;
  uint8_t log_data_len;
  uint8_t *log_data;
  uint16_t total_len;
} remote_log_t;

typedef struct {
  uint8_t log_id;
  char tag[30];
  esp_err_t (*data_log_cb)(void *out_data, size_t *len);
  uint16_t call_counter;
  uint16_t total_times_called;
  uint8_t call_interval_multiplier;
  uint8_t call_interval_counter;
} remote_log_register_t;

typedef struct {
  uint8_t event_id;
  char tag[30];
  uint16_t call_counter;
  uint16_t total_times_called;
} remote_log_event_register_t;

typedef struct {
  uint8_t event_id;
  uint32_t timestamp;
} remote_log_event_t;

esp_err_t remote_log_init(remote_log_config *cfg);
esp_err_t remote_log_register_log(remote_log_register_t log);

esp_err_t remote_log_register_event(remote_log_event_register_t event);
esp_err_t remote_log_record_event(uint8_t event_id);

void remote_log_common_init(uint8_t tx_pin, uint8_t rx_pin);
void remote_log_common_add(remote_log_common_t type, uint8_t log_id, void *param);