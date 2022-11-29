set(srsc)
set(includes)

list(APPEND includes
  
    "circular_buffer/include"
    "sig_gen/include"

 
)

list(APPEND srsc
   
    "circular_buffer/circular_buffer.c"
    "sig_gen/sig_gen.c"
    "sig_gen/sine_lut.c"
  
   
)


idf_component_register(SRCS ${srsc}
                    INCLUDE_DIRS ${includes}
                    REQUIRES esp_timer
                    )
