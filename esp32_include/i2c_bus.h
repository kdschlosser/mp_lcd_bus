#ifndef _ESP32_I2C_BUS_H_
    #define _ESP32_I2C_BUS_H_

    //local_includes
    #include "lcd_types.h"

    // esp-idf includes
    #include "esp_lcd_panel_io.h"
    #include "driver/i2c.h"

    // micropython includes
    #include "mphalport.h"
    #include "py/obj.h"
    #include "py/objarray.h"

    typedef struct _mp_lcd_i2c_bus_obj_t {
        mp_obj_base_t base;

        mp_obj_t callback;

        bool trans_done;
        bool rgb565_byte_swap;

        lcd_panel_io_t panel_io_handle;
        esp_lcd_panel_io_i2c_config_t panel_io_config;
        i2c_config_t bus_config;
        esp_lcd_i2c_bus_handle_t bus_handle;

        int host;

    } mp_lcd_i2c_bus_obj_t;

    extern const mp_obj_type_t mp_lcd_i2c_bus_type;
#endif /* _ESP32_I2C_BUS_H_ */