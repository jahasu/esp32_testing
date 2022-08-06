set(srsc)
set(includes)

list(APPEND includes
    "code_timer/include"
    "circular_buffer/include"
    "sig_gen/include"
    "log_buffer/include"
    "remote_log/include"
)

list(APPEND srsc
    "code_timer/code_timer.c"
    "circular_buffer/circular_buffer.c"
    "sig_gen/sig_gen.c"
    "sig_gen/sine_lut.c"
    "log_buffer/log_buffer.c"
    "remote_log/remote_log.c"
    "remote_log/remote_log_udp.c"
    "remote_log/remote_log_uart.c"
    "remote_log/remote_log_common.c"
)


idf_component_register(SRCS ${srsc}
                    INCLUDE_DIRS ${includes}
                    REQUIRES esp_timer
                    )
