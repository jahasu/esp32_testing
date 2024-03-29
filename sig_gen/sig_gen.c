#include <stdio.h>
#include <stdint.h>
#include "math.h"
#include "sig_gen.h"
#include "esp_log.h"
#include "esp_random.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

static const char *TAG = "sig_gen";
sig_gen_t L_sig;
sig_gen_t R_sig;

SemaphoreHandle_t xTaskSem = NULL;
static int timer_initialized = 0;

static void _timer_cb(void* arg)
{
    if(xTaskSem != NULL) {
        xSemaphoreGive(xTaskSem);
    }
}

/**
 * @brief Initialize a stereo signal generator global instance
 * 
 * @param stereo_config 
 */
void sig_gen_stereo_init(sig_gen_stereo_config_t* stereo_config)
{
    sig_gen_config_t cfg = {
        .gen_source = SINE_LUT,
        .lut_freq = stereo_config->left_sine_freq,
        .sample_rate = stereo_config->sample_rate,
        .bytes_per_sample = stereo_config->bytes_per_sample,
        .enable_cb = stereo_config->enable_cb,
        .cb_interval = stereo_config->cb_interval_ms,
        .endianess = stereo_config->endianess
    };
    sig_gen_init(&L_sig, &cfg);
    cfg.lut_freq = stereo_config->right_sine_freq;
    sig_gen_init(&R_sig, &cfg);
}

/**
 * @brief Read samples out to a user buffer
 * 
 * @param out_data User supplied buffer to hold audio data
 * @param size Total size of audio data to get in bytes
 * @param bytes_read Number of bytes read out to user buffer
 * @return esp_err_t ESP_OK on successful output of audio data 
 */
esp_err_t sig_gen_stereo_read(uint8_t *out_data, size_t size, size_t *bytes_read)
{
    if((!L_sig.initialized) | (!R_sig.initialized)) {
        ESP_LOGE(TAG, "ERROR: Signal generator not initialized!");
        return ESP_FAIL;
    }

    size_t samples = size / (L_sig.bytes_per_sample * 2); // 2 channels

    size_t bytes = sig_gen_output_combine(&L_sig, &R_sig, out_data, samples);
    *bytes_read = bytes;

    return ESP_OK;
}

void sig_gen_init(sig_gen_t *sg, const sig_gen_config_t *cfg)
{
    sg->gen_source = cfg->gen_source;
    sg->lut_freq = cfg->lut_freq;
    sg->bytes_per_sample = cfg->bytes_per_sample;
    sg->sample_rate = cfg->sample_rate;
    sg->endianess = cfg->endianess;
    sg->_amplitude = floor((pow(2,24)-1)/2) * cfg->amplitude;
    sg->_freq = cfg->freq;
    sg->_deltaTime = (1.0 / cfg->sample_rate);
    sg->_phase = cfg->phase;
    sg->_time = 0.0;
    sg->_double_pi = 2.0*M_PI;
    sg->initialized = 1;
    sg->cb_enabled = cfg->enable_cb;

    // Generate signal from LUT - create obj on the heap
    if(sg->gen_source == SINE_LUT) {
        sg->lut_gen = (lut_gen_t *)malloc(sizeof(lut_gen_t));
        lut_gen_init(sg->lut_gen, sg->lut_freq);
        ESP_LOGI(TAG,"Signal Generator Initialized. LUT#%d. LUT size: %d. Bytes/sample: %d. Freq: %f", sg->lut_freq, sg->lut_gen->lut_size, sg->bytes_per_sample, (float)(sg->sample_rate)/(sg->lut_gen->lut_size));
    }
    // Generate signal from calculation
    else {
        ESP_LOGI(TAG,"Signal Generator Initialized. sampleRate:%d, ampl:%f, freq:%f, deltaT:%f, phase:%f", sg->sample_rate,sg->_amplitude,sg->_freq,sg->_deltaTime,sg->_phase);
    }
    // Spawn timer task to drive callback
    if(!timer_initialized) {
        if(sg->cb_enabled) {
            // create timer
            const esp_timer_create_args_t periodic_timer_args = {
                .callback = _timer_cb,
                .name = "periodic"
            };
            esp_timer_handle_t periodic_timer;
            ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
            esp_timer_start_periodic(periodic_timer, cfg->cb_interval*1000);
            ESP_LOGI(TAG, "Periodic audio callback started. Interval: %d ms", cfg->cb_interval);

            xTaskSem = xSemaphoreCreateBinary();
            if( xTaskSem != NULL ) {
                ESP_LOGD(TAG, "Semaphore created");
            }
            timer_initialized = 1;
        }
    }
}


// Calculate or look up sine sample
uint32_t _sig_gen_get_sample(sig_gen_t *sg)
{
    if(sg->gen_source == SINE_LUT) {
        return lut_gen_get_sample(sg->lut_gen);
    }
    else if(sg->gen_source == SINE_CALC) {
        double angle = sg->_double_pi * sg->_freq * sg->_time + sg->_phase;
        double result = sg->_amplitude * sin(angle);
        sg->_time += sg->_deltaTime;
        return (uint32_t)result;
    }
    else if(sg->gen_source == WHITE_NOISE) {
        return esp_random();
    }
    else {
        ESP_LOGE(TAG,"Signal Generator Error - no generator source selected: [%d]", sg->gen_source);
        return 0;
    }
}

// Outputs a mono sine
// @return number of bytes
size_t sig_gen_output(sig_gen_t *sg, uint8_t *out_data, size_t samples)
{
    if(!sg->initialized) {
        ESP_LOGE(TAG, "ERROR: Signal generator not initialized!");
        return 0;
    }

    // Blocks here if callback is enabled
    if(L_sig.cb_enabled | R_sig.cb_enabled) {
        if(xTaskSem != NULL) {
            if(xSemaphoreTake(xTaskSem, pdMS_TO_TICKS(1000)) != pdTRUE) {
                ESP_LOGE(TAG, "ERROR: No callback received");
                return 0;
            }
        }
    }

    size_t out_index = 0;

    switch (sg->bytes_per_sample) {
        case SIG_GEN_16BIT: // 16bit LE
            if(sg->endianess == SIG_GEN_LE) {
                for(int i=0;i<samples;i++) {
                    uint16_t sample = (uint16_t)(_sig_gen_get_sample(sg)>>8);
                    out_data[out_index++] = (sample & 0xff);
                    out_data[out_index++] = (sample >> 8) & 0xff;
                }
            }
            else {  // BE
                for(int i=0;i<samples;i++) {
                    uint16_t sample = (uint16_t)(_sig_gen_get_sample(sg)>>8);
                    out_data[out_index++] = (sample >> 8) & 0xff;
                    out_data[out_index++] = (sample & 0xff);
                }
            }
            break;

        case SIG_GEN_24BIT: // 24bit LE
            if(sg->endianess == SIG_GEN_LE) {
                for(int i=0;i<samples;i++) {
                    uint32_t sample = _sig_gen_get_sample(sg);
                    out_data[out_index++] = (sample & 0xff);
                    out_data[out_index++] = (sample >> 8) & 0xff;
                    out_data[out_index++] = (sample >> 16) & 0xff;
                }
            }
            else {  // BE
                for(int i=0;i<samples;i++) {
                    uint32_t sample = _sig_gen_get_sample(sg);
                    out_data[out_index++] = (sample >> 16) & 0xff;
                    out_data[out_index++] = (sample >> 8) & 0xff;
                    out_data[out_index++] = (sample & 0xff);
                }
            }
            break;

        case SIG_GEN_24_32BIT: // 24/32bit LE
            if(sg->endianess == SIG_GEN_LE) {
                for(int i=0;i<samples;i++) {
                    uint32_t sample = _sig_gen_get_sample(sg);
                    out_data[out_index++] = (sample & 0xff);
                    out_data[out_index++] = (sample >> 8) & 0xff;
                    out_data[out_index++] = (sample >> 16) & 0xff;
                    out_data[out_index++] = (sample >> 24) & 0xff;
                }
            }
            else {  // BE
                for(int i=0;i<samples;i++) {
                    uint32_t sample = _sig_gen_get_sample(sg);
                    out_data[out_index++] = (sample >> 24) & 0xff;
                    out_data[out_index++] = (sample >> 16) & 0xff;
                    out_data[out_index++] = (sample >> 8) & 0xff;
                    out_data[out_index++] = (sample & 0xff);
                }
            }
            break;


        default:
            ESP_LOGE(TAG,"ERROR: bytes per sample %d", sg->bytes_per_sample);
            break;
        }
    return out_index;
}

// Takes two sig_gen objs and produce a stereo sine
size_t sig_gen_output_combine(sig_gen_t *sg_l, sig_gen_t *sg_r, uint8_t *out_data, size_t samples)
{
    if((!sg_l->initialized) | (!sg_r->initialized)) {
        ESP_LOGE(TAG, "ERROR: Signal generator not initialized!");
        return 0;
    }

    // Blocks here if callback is enabled
    if(L_sig.cb_enabled | R_sig.cb_enabled) {
        if(xTaskSem != NULL) {
            if(xSemaphoreTake(xTaskSem, pdMS_TO_TICKS(1000)) != pdTRUE) {
                ESP_LOGE(TAG, "ERROR: No callback received");
                return 0;
            }
        }
    }

    if(sg_l->bytes_per_sample != sg_r->bytes_per_sample) {
        sg_r->bytes_per_sample = sg_l->bytes_per_sample;
    }
    if(sg_l->endianess != sg_r->endianess) {
        sg_r->endianess = sg_l->endianess;
    }

    uint8_t bps_combined = sg_l->bytes_per_sample;
    uint8_t end_combined = sg_l->endianess;
    size_t out_index = 0;

    switch (bps_combined) {
        case SIG_GEN_16BIT: // 16bit LE
            if(end_combined == SIG_GEN_LE) {
                uint16_t l_sample;
                uint16_t r_sample;

                for(int i=0;i<samples;i++) {
                    l_sample = (uint16_t)(_sig_gen_get_sample(sg_l)>>8);
                    r_sample = (uint16_t)(_sig_gen_get_sample(sg_r)>>8);
                    // Combine l & r
                    uint32_t lr_combined = (l_sample<<16)|(r_sample & 0xffff);

                    out_data[out_index++] = (lr_combined >> 24) & 0xff;
                    out_data[out_index++] = (lr_combined >> 16) & 0xff;
                    out_data[out_index++] = (lr_combined >> 8) & 0xff;
                    out_data[out_index++] = (lr_combined & 0xff);
                }
            }
            else { // BE
                uint16_t l_sample;
                uint16_t r_sample;

                for(int i=0;i<samples;i++) {
                    l_sample = (uint16_t)(_sig_gen_get_sample(sg_l)>>8);
                    r_sample = (uint16_t)(_sig_gen_get_sample(sg_r)>>8);
                    // Combine l & r
                    uint32_t lr_combined = (l_sample<<16)|(r_sample & 0xffff);

                    out_data[out_index++] = (lr_combined >> 16) & 0xff;
                    out_data[out_index++] = (lr_combined >> 24) & 0xff;
                    out_data[out_index++] = (lr_combined & 0xff);
                    out_data[out_index++] = (lr_combined >> 8) & 0xff;
                }
            }
            break;

        case SIG_GEN_24BIT: // 24bit LE
            if(end_combined == SIG_GEN_LE) {
                uint32_t l_sample;
                uint32_t r_sample;

                for(int i=0;i<samples;i++) {
                    l_sample = _sig_gen_get_sample(sg_l);
                    r_sample = _sig_gen_get_sample(sg_r);

                    // l
                    out_data[out_index++] = (l_sample >> 16) & 0xff;
                    out_data[out_index++] = (l_sample >> 8) & 0xff;
                    out_data[out_index++] = (l_sample & 0xff);

                    // r
                    out_data[out_index++] = (r_sample >> 16) & 0xff;
                    out_data[out_index++] = (r_sample >> 8) & 0xff;
                    out_data[out_index++] = (r_sample & 0xff);
                }
            }
            else { // BE
                uint32_t l_sample;
                uint32_t r_sample;

                for(int i=0;i<samples;i++) {
                    l_sample = _sig_gen_get_sample(sg_l);
                    r_sample = _sig_gen_get_sample(sg_r);

                    // l
                    out_data[out_index++] = (l_sample & 0xff);
                    out_data[out_index++] = (l_sample >> 8) & 0xff;
                    out_data[out_index++] = (l_sample >> 16) & 0xff;

                    // r
                    out_data[out_index++] = (r_sample & 0xff);
                    out_data[out_index++] = (r_sample >> 8) & 0xff;
                    out_data[out_index++] = (r_sample >> 16) & 0xff;
                }
            }
            break;

        case SIG_GEN_24_32BIT: // 24/32bit LE
            if(end_combined == SIG_GEN_LE) {
                uint32_t l_sample;
                uint32_t r_sample;

                for(int i=0;i<samples;i++) {
                    l_sample = _sig_gen_get_sample(sg_l);
                    r_sample = _sig_gen_get_sample(sg_r);

                    // l
                    out_data[out_index++] = (l_sample >> 24) & 0xff;
                    out_data[out_index++] = (l_sample >> 16) & 0xff;
                    out_data[out_index++] = (l_sample >> 8) & 0xff;
                    out_data[out_index++] = (l_sample & 0xff);

                    // r
                    out_data[out_index++] = (r_sample >> 24) & 0xff;
                    out_data[out_index++] = (r_sample >> 16) & 0xff;
                    out_data[out_index++] = (r_sample >> 8) & 0xff;
                    out_data[out_index++] = (r_sample & 0xff);
                }
            }
            else { // BE
                uint32_t l_sample;
                uint32_t r_sample;

                for(int i=0;i<samples;i++) {
                    l_sample = _sig_gen_get_sample(sg_l);
                    r_sample = _sig_gen_get_sample(sg_r);

                    // l
                    out_data[out_index++] = (l_sample & 0xff);
                    out_data[out_index++] = (l_sample >> 8) & 0xff;
                    out_data[out_index++] = (l_sample >> 16) & 0xff;
                    out_data[out_index++] = (l_sample >> 24) & 0xff;

                    // r
                    out_data[out_index++] = (r_sample & 0xff);
                    out_data[out_index++] = (r_sample >> 8) & 0xff;
                    out_data[out_index++] = (r_sample >> 16) & 0xff;
                    out_data[out_index++] = (r_sample >> 24) & 0xff;
                }
            }
            break;

        default:
            ESP_LOGE(TAG,"bytes per sample L; %d // bytes per sample R; %d", sg_l->bytes_per_sample, sg_r->bytes_per_sample);
            break;
    }

    return out_index;
}
