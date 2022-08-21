#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <string.h>


/*
  Timestamp data
*/
typedef struct {
  char *tag;
  uint32_t timestamp;
} timings_t;


/*
  code_timer class
*/
typedef struct {
  timings_t *buffer;
  size_t size;
  size_t _idx;
  size_t size_trigger;
  uint8_t active;
  char *timer_tag;
} code_timer_t;


// Global object
code_timer_t global_code_timer;


void code_timer_init(code_timer_t *ct, char *timer_tag, size_t size, size_t size_trigger);
void code_timer_deinit(code_timer_t *ct);
void code_timer_enable_all(void);
void code_timer_disable_all(void);
void code_timer_activate(code_timer_t *ct);
void code_timer_deactivate(code_timer_t *ct);
void code_timer_take_timestamp(code_timer_t *ct, char *tag);
void code_timer_print_timestamps(code_timer_t *ct, size_t number);

void code_timer_enable_global(size_t size);
