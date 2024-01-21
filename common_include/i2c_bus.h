#ifndef _I2C_BUS_H_
    #define _I2C_BUS_H_

    //local_includes
    #include "modlcd_bus.h"

    // micropython includes
    #include "py/obj.h"
    #include "py/runtime.h"


    typedef struct _mp_lcd_i2c_bus_obj_t {
        mp_obj_base_t base;

        mp_obj_t callback;

        bool trans_done;
        bool rgb565_byte_swap;

        lcd_panel_io_t panel_io_handle;
        void *panel_io_config;
        void *bus_config;
        void *bus_handle;

        int host;

    } mp_lcd_i2c_bus_obj_t;

    extern const mp_obj_type_t mp_lcd_i2c_bus_type;
#endif /* _I2C_BUS_H_ */