// local includes
#include "modlcd_bus.h"
#include "spi_bus.h"
#include "i2c_bus.h"
#include "i80_bus.h"
#include "rgb_bus.h"

// micropython includes
#include "py/obj.h"
#include "py/runtime.h"
#include "py/objarray.h"


mp_obj_t mp_lcd_bus_get_lane_count(size_t n_args, const mp_obj_t *args)
{
    mp_lcd_i80_bus_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    uint8_t lane_count;
    mp_lcd_err_t ret = lcd_panel_io_get_lane_count(&self->panel_io_handle, &lane_count);

    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("%d(lcd_panel_io_init)"), ret);
    }

    return mp_obj_new_int(lane_count);
}

MP_DEFINE_CONST_FUN_OBJ_VAR(mp_lcd_bus_get_lane_count_obj, 1, mp_lcd_bus_get_lane_count);


mp_obj_t mp_lcd_bus_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    enum { ARG_self, ARG_width, ARG_height, ARG_bpp, ARG_buffer_size, ARG_rgb565_byte_swap };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_self,             MP_ARG_OBJ  | MP_ARG_REQUIRED  },
        { MP_QSTR_width,            MP_ARG_INT  | MP_ARG_REQUIRED  },
        { MP_QSTR_height,           MP_ARG_INT  | MP_ARG_REQUIRED  },
        { MP_QSTR_bpp,              MP_ARG_INT  | MP_ARG_REQUIRED  },
        { MP_QSTR_buffer_size,      MP_ARG_INT  | MP_ARG_REQUIRED  },
        { MP_QSTR_rgb565_byte_swap, MP_ARG_BOOL | MP_ARG_REQUIRED  },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_lcd_bus_obj_t *self = (mp_lcd_bus_obj_t *)args[ARG_self].u_obj;


    if (args[ARG_bpp].u_int == 16) {
        self->rgb565_byte_swap = (bool)args[ARG_rgb565_byte_swap].u_bool;
    } else {
        self->rgb565_byte_swap = false;
    }

    mp_lcd_err_t ret = lcd_panel_io_init(&self->panel_io_handle,  (uint16_t)args[ARG_width].u_int,
        (uint16_t)args[ARG_height].u_int, (uint8_t)args[ARG_bpp].u_int,
        (uint32_t)args[ARG_buffer_size].u_int
    );

    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("%d(lcd_panel_io_init)"), ret);
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(mp_lcd_bus_init_obj, 6, mp_lcd_bus_init);


mp_obj_t mp_lcd_bus_tx_param(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    enum { ARG_self, ARG_cmd, ARG_params };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_self,    MP_ARG_OBJ | MP_ARG_REQUIRED  },
        { MP_QSTR_cmd,     MP_ARG_INT | MP_ARG_REQUIRED  },
        { MP_QSTR_params,  MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_lcd_bus_obj_t *self = (mp_lcd_bus_obj_t *)args[ARG_self].u_obj;
    mp_lcd_err_t ret;

    if (args[ARG_params].u_obj != mp_const_none) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[ARG_params].u_obj, &bufinfo, MP_BUFFER_READ);
        ret = lcd_panel_io_tx_param(&self->panel_io_handle, (int)args[ARG_cmd].u_int, bufinfo.buf, (size_t)bufinfo.len);
    } else {
        ret = lcd_panel_io_tx_param(&self->panel_io_handle, (int)args[ARG_cmd].u_int, NULL, 0);
    }

    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("%d(lcd_panel_io_tx_param)"), ret);
    }

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(mp_lcd_bus_tx_param_obj, 2, mp_lcd_bus_tx_param);


mp_obj_t mp_lcd_bus_tx_color(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    enum { ARG_self, ARG_cmd, ARG_data, ARG_x_start, ARG_y_start, ARG_x_end, ARG_y_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_self,    MP_ARG_OBJ | MP_ARG_REQUIRED  },
        { MP_QSTR_cmd,     MP_ARG_INT | MP_ARG_REQUIRED  },
        { MP_QSTR_data,    MP_ARG_OBJ | MP_ARG_REQUIRED  },
        { MP_QSTR_x_start, MP_ARG_INT | MP_ARG_REQUIRED  },
        { MP_QSTR_y_start, MP_ARG_INT | MP_ARG_REQUIRED  },
        { MP_QSTR_x_end,   MP_ARG_INT | MP_ARG_REQUIRED  },
        { MP_QSTR_y_end,   MP_ARG_INT | MP_ARG_REQUIRED  },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_lcd_bus_obj_t *self = (mp_lcd_bus_obj_t *)args[ARG_self].u_obj;

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_data].u_obj, &bufinfo, MP_BUFFER_READ);

    mp_lcd_err_t ret = lcd_panel_io_tx_color(&self->panel_io_handle, (int)args[ARG_cmd].u_int, bufinfo.buf, (size_t)bufinfo.len);

    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("%d(lcd_panel_io_tx_color)"), ret);
    }

    if (self->callback == mp_const_none) {
        while (self->trans_done == false) {}
        self->trans_done = false;
    }

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(mp_lcd_bus_tx_color_obj, 7, mp_lcd_bus_tx_color);


mp_obj_t mp_lcd_bus_deinit(mp_obj_t self_in)
{
    mp_lcd_bus_obj_t *self = self_in;

    mp_lcd_err_t ret = lcd_panel_io_del(&self->panel_io_handle);
    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("%d(lcd_panel_io_del)"), ret);
    }

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(mp_lcd_bus_deinit_obj, mp_lcd_bus_deinit);


mp_obj_t mp_lcd_bus_rx_param(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    enum { ARG_self, ARG_cmd, ARG_data };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_self,  MP_ARG_OBJ | MP_ARG_REQUIRED  },
        { MP_QSTR_cmd,   MP_ARG_INT | MP_ARG_REQUIRED  },
        { MP_QSTR_data,  MP_ARG_OBJ | MP_ARG_REQUIRED  },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_lcd_bus_obj_t *self = (mp_lcd_bus_obj_t *)args[ARG_self].u_obj;

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_data].u_obj, &bufinfo, MP_BUFFER_WRITE);

    mp_lcd_err_t ret = lcd_panel_io_rx_param(&self->panel_io_handle, (int)args[ARG_cmd].u_int, bufinfo.buf, (size_t)bufinfo.len);

    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("%d(lcd_panel_io_rx_param)"), ret);
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(mp_lcd_bus_rx_param_obj, 3, mp_lcd_bus_rx_param);


mp_obj_t mp_lcd_bus_register_callback(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    enum { ARG_self, ARG_callback };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_self,         MP_ARG_OBJ | MP_ARG_REQUIRED  },
        { MP_QSTR_callback,     MP_ARG_OBJ | MP_ARG_REQUIRED  },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_lcd_bus_obj_t *self = (mp_lcd_bus_obj_t *)args[ARG_self].u_obj;

    self->callback = args[ARG_callback].u_obj;

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(mp_lcd_bus_register_callback_obj, 2, mp_lcd_bus_register_callback);


STATIC const mp_rom_map_elem_t mp_lcd_bus_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_get_lane_count),    MP_ROM_PTR(&mp_lcd_bus_get_lane_count_obj)     },
    { MP_ROM_QSTR(MP_QSTR_register_callback), MP_ROM_PTR(&mp_lcd_bus_register_callback_obj)  },
    { MP_ROM_QSTR(MP_QSTR_tx_param),          MP_ROM_PTR(&mp_lcd_bus_tx_param_obj)           },
    { MP_ROM_QSTR(MP_QSTR_tx_color),          MP_ROM_PTR(&mp_lcd_bus_tx_color_obj)           },
    { MP_ROM_QSTR(MP_QSTR_rx_param),          MP_ROM_PTR(&mp_lcd_bus_rx_param_obj)           },
    { MP_ROM_QSTR(MP_QSTR_init),              MP_ROM_PTR(&mp_lcd_bus_init_obj)               },
    { MP_ROM_QSTR(MP_QSTR_deinit),            MP_ROM_PTR(&mp_lcd_bus_deinit_obj)             },
    { MP_ROM_QSTR(MP_QSTR___del__),           MP_ROM_PTR(&mp_lcd_bus_deinit_obj)             }
};

MP_DEFINE_CONST_DICT(mp_lcd_bus_locals_dict, mp_lcd_bus_locals_dict_table);


STATIC const mp_map_elem_t mp_module_lcd_bus_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),           MP_OBJ_NEW_QSTR(MP_QSTR_lcd_bus)      },
    { MP_ROM_QSTR(MP_QSTR_RGBBus),             (mp_obj_t)&mp_lcd_rgb_bus_type        },
    { MP_ROM_QSTR(MP_QSTR_SPIBus),             (mp_obj_t)&mp_lcd_spi_bus_type        },
    { MP_ROM_QSTR(MP_QSTR_I2CBus),             (mp_obj_t)&mp_lcd_i2c_bus_type        },
    { MP_ROM_QSTR(MP_QSTR_I80Bus),             (mp_obj_t)&mp_lcd_i80_bus_type        }
};

STATIC MP_DEFINE_CONST_DICT(mp_module_lcd_bus_globals, mp_module_lcd_bus_globals_table);


const mp_obj_module_t mp_module_lcd_bus = {
    .base    = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&mp_module_lcd_bus_globals,
};

MP_REGISTER_MODULE(MP_QSTR_lcd_bus, mp_module_lcd_bus);
