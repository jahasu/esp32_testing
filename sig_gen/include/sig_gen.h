#pragma once

#include <stdint.h>
#include <string.h>
#include "esp_err.h"

#include "sine_lut.h"


typedef enum {
    LUT_GEN,
    CALC_GEN
} gen_source_e;

typedef enum {
    SIG_GEN_LE,
    SIG_GEN_BE
} endianess_t;


typedef struct {
    gen_source_e gen_source;    // Signal source: LUT or CALCulation 
    lut_gen_t *lut_gen;         // Lookup generator obj
    lut_freq_e lut_freq;

    uint8_t bytes_per_sample;
    uint16_t sample_rate;
    endianess_t endianess;

    double _amplitude;
    double _freq;
    double _time;
    double _deltaTime;
    double _phase;
    double _double_pi;

    uint8_t initialized;
} sig_gen_t;


typedef struct {
    gen_source_e gen_source;
    lut_freq_e lut_freq;
    uint8_t bytes_per_sample;
    uint16_t sample_rate;
    endianess_t endianess;
    double amplitude;
    double freq;
    double phase;
} sig_gen_config_t;


void sig_gen_init(sig_gen_t *sg, const sig_gen_config_t *cfg);
size_t sig_gen_output( sig_gen_t *sg, uint8_t *out_data, size_t samples);
size_t sig_gen_output_combine(sig_gen_t *sg_l, sig_gen_t *sg_r, uint8_t *out_data, size_t samples);

// uint32_t _sig_gen_get_sample(sig_gen_t *sg);

