#include "code_timer.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"


/* Global */
const char *CT_TAG = "code_timer";
int timers_active = 1;



void code_timer_enable_global(size_t size)
{
    code_timer_init(&global_code_timer,"Global timer",size,size);
    ESP_LOGI(CT_TAG,"Global code timer initialized");
    code_timer_enable_all();    
}


/*
FreeRTOS task to print the contents of the time code object buffer
*/
void _code_timer_print_task(void *arg)
{
    code_timer_t *ct = (code_timer_t *)arg; // Get code_timer obj
    uint32_t last = 0;
    ESP_LOGI("","-------------------------------------------------");
    ESP_LOGI("","%s",ct->timer_tag);
    ESP_LOGI("","-------------------------------------------------");
    for(int i=0; i<ct->size_trigger; i++) {
        ESP_LOGI("","%3d: %s \t%8d\t%d",i,ct->buffer[i].tag, ct->buffer[i].timestamp, ct->buffer[i].timestamp-last);
        last = ct->buffer[i].timestamp;
        vTaskDelay(1);
    }
    vTaskDelete(NULL);
}


/*
Enable code timers
*/
void code_timer_enable_all(void)
{
    timers_active = 1;
    ESP_LOGI(CT_TAG,"Timers activated");
}

/*
Disable code timers
*/
void code_timer_disable_all(void)
{
    timers_active = 0;
    ESP_LOGI(CT_TAG,"Timers deactivated");
}

/*
Activate timimg
*/
void code_timer_activate(code_timer_t *ct)
{
    ct->active = true;
}

/*
Deactivate timimg
*/
void code_timer_deactivate(code_timer_t *ct)
{
    ct->active = false;
}

/*
Initialize code_time object
@ct: code_time obj
@size: number of timestamps in buffer
@size_trigger: number of timestamps taken before printing them to the console
*/
void code_timer_init(code_timer_t *ct, char *timer_tag, size_t size, size_t size_trigger)
{
    size_t _size = size;
    if(_size == 0) {
        ESP_LOGW(CT_TAG,"Buffer size = 0, setting to 10");
        _size = 10;
    }
    if(_size < size_trigger) {
        ESP_LOGW(CT_TAG,"Buffer size set to size_trigger");
        _size = size_trigger;
    }
    ct->buffer = (timings_t *)malloc(sizeof(timings_t)*_size);
    ct->size = _size;
    ct->_idx = 0;
    ct->size_trigger = size_trigger;
    ct->active = true;
    ct->timer_tag = timer_tag;
}


void code_timer_deinit(code_timer_t *ct)
{
    free(ct->buffer);
    ct->buffer = NULL;
}

/*
Adds a timestamp obj to the buffer
*/
void _ct_add(code_timer_t *ct, timings_t timestamp)
{
    ct->buffer[ct->_idx] = timestamp;
    ct->_idx++;
}

/*
Takes a timestamp
@str: name to be printet along timestamp
*/
void code_timer_take_timestamp(code_timer_t *ct, char *tag)
{   
    if(timers_active) {
        if(ct->active) {
            timings_t timing = {
            .tag = tag,
            .timestamp = (uint32_t)esp_timer_get_time()
            };
            _ct_add(ct,timing);
            if(ct->_idx >= ct->size_trigger) {
                ct->active = false;
                code_timer_print_timestamps(ct, 0);
            }
        }
    }
}

/*
Creates a freeRTOS task to print the timestamps
*/
void code_timer_print_timestamps(code_timer_t *ct, size_t number)
{
    if(number!=0) {
        ct->size_trigger = number;
    }
    xTaskCreatePinnedToCore(_code_timer_print_task,"code_timer print",8192,ct,10,NULL,APP_CPU_NUM);
}

