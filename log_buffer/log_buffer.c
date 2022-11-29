#include "log_buffer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"


const char *TB_TAG = "log_buffer";


void log_buffer_init(log_buffer_t *tb, uint8_t *buffer, size_t size, size_t delayed_start, char *tag)
{
    tb->buffer = buffer;
    tb->size = size;
    tb->write = 0;
    tb->is_printed = 0;
    tb->delayed_start = delayed_start;
    tb->tag = tag;
    memset(tb->buffer,0,size);
}

void log_buffer_add(log_buffer_t *tb, void *data, size_t bytes)
{   
    if(!tb->is_printed) {
        if(tb->delayed_start > 0)
            tb->delayed_start--;
        else {
            if(tb->write+bytes > tb->size) { // buffer will be overflown if a write happens
                log_buffer_print(tb);       
            }
            else {
                printf(".");
                // Add data to log buffer
                memcpy(tb->buffer + tb->write,data,bytes);
                tb->write += bytes;
            }
        }
    }
}


void log_buffer_add_byte(log_buffer_t *tb, uint8_t data)
{
    if(!tb->is_printed) {
        if(tb->delayed_start > 0)
            tb->delayed_start--;
        else {
            if((tb->write+1) > tb->size) {
                log_buffer_print(tb);      
            }
            else {
                // Add data byte to log buffer
                tb->buffer[tb->write] = data;
                tb->write++;
            }
        }
    }
}


void log_buffer_enable_global(uint8_t *buffer, size_t size, size_t delayed_start)
{
    log_buffer_init(&global_log_buf,buffer,size,delayed_start,"Global log buffer");
    ESP_LOGI(TB_TAG,"Global log_buffer initialized");
}


/*
FreeRTOS task to print the contents of the object buffer
*/
void _log_buffer_print_task(void *arg)
{
    log_buffer_t *tb = (log_buffer_t *)arg; // Get log_buffer_t obj
    ESP_LOGI("","-------------------------------------------------");
    ESP_LOGI("","%s",tb->tag);
    ESP_LOGI("","-------------------------------------------------");
    ESP_LOG_BUFFER_HEX("log_buffer",tb->buffer,tb->size);
    vTaskDelete(NULL);
}


/*
Creates a freeRTOS task to print the buffer
*/
void log_buffer_print(log_buffer_t *tb)
{
    xTaskCreatePinnedToCore(_log_buffer_print_task,"log_buffer print",8192,tb,10,NULL,APP_CPU_NUM);
    tb->is_printed = 1;
}


// -------------------------------------------------


void _log_reg_buffer_print_task(void *arg)
{
    log_reg_buffer_t *lr = (log_reg_buffer_t *)arg; // Get log_buffer_t obj
    uint32_t last = 0;
    ESP_LOGI("","-------------------------------------------------");
    ESP_LOGI("","%s",lr->tag);
    ESP_LOGI("","-------------------------------------------------");
    if(lr->incl_time) {
        for(int i=0;i<lr->size;i++) {
            //ESP_LOGI("","%s: \t 0x%.8X \t %d",(lr->buffer+i)->tag,(lr->buffer+i)->reg,(lr->buffer+i)->timestamp-last);
            last = (lr->buffer+i)->timestamp;
        }
    }
    else {
        for(int i=0;i<lr->size;i++) {
            ESP_LOGI("","%s: \t 0x%.8X",(lr->buffer+i)->tag,(lr->buffer+i)->reg);
        }
    }
    vTaskDelete(NULL);
}


/*
Creates a freeRTOS task to print the buffer
*/
void log_reg_buffer_print(log_reg_buffer_t *lr)
{
    xTaskCreatePinnedToCore(_log_reg_buffer_print_task,"log_reg_buffer print",8192,lr,10,NULL,APP_CPU_NUM);
    lr->is_printed = 1;
}


void log_reg_buffer_init(log_reg_buffer_t *lr, log_reg_t *buffer, size_t size, uint8_t incl_timestamps, char *tag)
{
    lr->buffer = buffer;
    lr->size = size;
    lr->is_printed = 0;
    lr->incl_time = incl_timestamps;
    lr->tag = tag;
    lr->write = 0;
    memset(lr->buffer,0,sizeof(log_reg_t)*lr->size);
}


void log_reg_buffer_add(log_reg_buffer_t *lr, uint32_t reg, char *tag)
{
    if(!lr->is_printed) {
        if(lr->write+1 > lr->size) { // buffer will be overflown if a new log happens
            log_reg_buffer_print(lr);       
        }
        else {
            // Add log to log buffer
            log_reg_t lreg = {
                .reg = reg,
                .tag = tag
            };
            if(lr->incl_time) {
                lreg.timestamp = (uint32_t)esp_timer_get_time();
            }
            lr->buffer[lr->write] = lreg;
            lr->write++;
        }
    }
}

void log_reg_buffer_enable_global(log_reg_t *buffer, size_t size, uint8_t incl_timestamps)
{
    log_reg_buffer_init(&global_reg_buf,buffer,size,incl_timestamps,"Global register buffer");
    ESP_LOGI(TB_TAG,"Global log_reg_buffer initialized");
}


