#include <node_api.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "driver_ina226.h"
#include "errors.h"
#include "structs.h"
#include "driver_ina226_interface.h"

static ina226_handle_t ina226_iic_handle;

napi_value ina226_info_wrapper(napi_env env, napi_callback_info info) {
    size_t argc = 0; // No arguments expected
    napi_get_cb_info(env, info, &argc, NULL, NULL, NULL);

    // Create an instance of ina226_info_t
    ina226_info_t ina_info;

    // Call the ina226_info function
    uint8_t result = ina226_info(&ina_info);

    // Check success or failure
    if (result == 0) {
        // Convert ina_info to JavaScript object
        napi_value jsObj = ina226_info_to_js(env, &ina_info);
        return jsObj;
    } else if (result == 2) {
        napi_throw_error(env, NULL, "Failed to retrieve ina226 information");
        return NULL;
    }

    // In case of other unexpected values
    napi_throw_error(env, NULL, "Unexpected result from ina226_info");
    return NULL;
}

/**
 * Initialize the INA226.
 *
 * Takes configuration object with following keys when calling from JavaScript:
 * - `i2c_device`: The I2C bus number`.
 * - `addr`: The I2C address of the INA226 sensor. Possible values are
 *           enumerated in `I2CAddress` enum.
 * - `r`: The shunt resistor value in ohms.
 * - `conversion_time_shunt`: Conversion time for shunt voltage.
 * - `conversion_time_bus`: Conversion time for bus voltage.
 * - `averaging_mode`: Averaging mode.
 * - `mode`: Operating mode (triggered/continuous)
 */
napi_value basic_init(napi_env env, napi_callback_info info) {

    size_t argc = 7;
    napi_value argv[argc], this;
    napi_get_cb_info(env, info, &argc, argv, &this, NULL);
    if (argc != 7) {
        napi_throw_error(env, WRONG_NUMBER_OF_ARGUMENTS,
                         "Check number of arguments for fn: basic_init(..)");
        return NULL;
    }


    // Parse the first argument (bus nr)
    uint32_t i2c_bus;
    napi_status status = napi_get_value_uint32(env, argv[0], &i2c_bus);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to create NAPI value for i2c_bus");
        return NULL;
    }
    set_iic_bus(i2c_bus);

    // Parse the second argument (i2c addr)
    uint32_t i2c_addr;
    status = napi_get_value_uint32(env, argv[1], &i2c_addr);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to create NAPI value for i2c_addr");
        return NULL;
    }

    // Parse the third argument (r)
    double r;
    status = napi_get_value_double(env, argv[2], &r);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to create NAPI value for r");
        return NULL;
    }

    // Parse the fourth argument (conversion_time_shunt)
    int32_t conversion_time_shunt;
    status = napi_get_value_int32(env, argv[3], &conversion_time_shunt);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to create NAPI value for conversion_time_shunt");
        return NULL;
    }

    // Parse the fifth argument (converion_time_bus)
    int32_t conversion_time_bus;
    status = napi_get_value_int32(env, argv[4], &conversion_time_bus);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to create NAPI value for conversion_time_bus");
        return NULL;
    }

    // Parse the sixth argument (averaging mode)
    int32_t averaging_mode;
    status = napi_get_value_int32(env, argv[5], &averaging_mode);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to create NAPI value for averaging_mode");
        return NULL;
    }

    // Parse the seventh argument (operating mode)
    int32_t mode;
    status = napi_get_value_int32(env, argv[6], &mode);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to create NAPI value for mode");
        return NULL;
    }

    uint8_t res;
    uint16_t calibration;
    
    /* link interface function */
    DRIVER_INA226_LINK_INIT(&ina226_iic_handle, ina226_handle_t);
    DRIVER_INA226_LINK_IIC_INIT(&ina226_iic_handle, ina226_interface_iic_init);
    DRIVER_INA226_LINK_IIC_DEINIT(&ina226_iic_handle, ina226_interface_iic_deinit);
    DRIVER_INA226_LINK_IIC_READ(&ina226_iic_handle, ina226_interface_iic_read);
    DRIVER_INA226_LINK_IIC_WRITE(&ina226_iic_handle, ina226_interface_iic_write);
    DRIVER_INA226_LINK_DELAY_MS(&ina226_iic_handle, ina226_interface_delay_ms);
    DRIVER_INA226_LINK_DEBUG_PRINT(&ina226_iic_handle, ina226_interface_debug_print);
    DRIVER_INA226_LINK_RECEIVE_CALLBACK(&ina226_iic_handle, ina226_interface_receive_callback);
    
    /* set addr pin */
    res = ina226_set_addr_pin(&ina226_iic_handle, i2c_addr);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: set addr pin failed.\n");
        napi_throw_error(env, INIT_ERROR, "Failed to set addr pin");
        return NULL;
    }

    /* set the r */
    res = ina226_set_resistance(&ina226_iic_handle, r);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: set resistance failed.\n");
        napi_throw_error(env, INIT_ERROR,
            "Failed to set shunt resistance");
        return NULL;
    }
    
    /* init */
    res = ina226_init(&ina226_iic_handle);
    switch (res)
    {
        case 0:
            // Success
            break;
        case 1:
            ina226_interface_debug_print("ina226: IIC initialization failed.\n");
            napi_throw_error(env, INIT_ERROR, "IIC initialization failed");
            return NULL;
        case 2:
            ina226_interface_debug_print("ina226: handle is NULL.\n");
            napi_throw_error(env, INIT_ERROR, "Handle is NULL");
            return NULL;
        case 3:
            ina226_interface_debug_print("ina226: iic init failed.\n");
            napi_throw_error(env, INIT_ERROR, "Linked functions is NULL");
            return NULL;
        case 4:
            ina226_interface_debug_print("ina226: id is invalid.\n");
            napi_throw_error(env, INIT_ERROR, "ID is invalid");
            return NULL;
        case 5:
            ina226_interface_debug_print("ina226: soft reset failed.\n");
            napi_throw_error(env, INIT_ERROR, "Soft reset failed");
            return NULL;
        default:
            ina226_interface_debug_print("ina226: unknown error.\n");
            napi_throw_error(env, INIT_ERROR, "Unknown error");
            return NULL;
    }
    
    /* set average mode */
    res = ina226_set_average_mode(&ina226_iic_handle,
        averaging_mode);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: set average mode failed.\n");
        (void)ina226_deinit(&ina226_iic_handle);
        napi_throw_error(env, INIT_ERROR, "Failed to set INA226 average mode");
        return NULL;
    }
    
    /* set default bus voltage conversion time */
    res = ina226_set_bus_voltage_conversion_time(&ina226_iic_handle,
        conversion_time_bus);
    if (res != 0)
    {
        ina226_interface_debug_print(
            "ina226: set bus voltage conversion time failed.\n");
        (void)ina226_deinit(&ina226_iic_handle);
        napi_throw_error(env, INIT_ERROR,
            "Failed to set INA226 bus voltage conversion time");
        return NULL;
    }
    
    /* set default shunt voltage conversion time */
    res = ina226_set_shunt_voltage_conversion_time(
        &ina226_iic_handle, conversion_time_shunt);
    if (res != 0)
    {
        ina226_interface_debug_print(
            "ina226: set shunt voltage conversion time failed.\n");
        (void)ina226_deinit(&ina226_iic_handle);
        napi_throw_error(env, INIT_ERROR,
            "Failed to set INA226 shunt voltage conversion time");
        return NULL;
    }
    
    /* calculate calibration */
    res = ina226_calculate_calibration(&ina226_iic_handle, (uint16_t *)&calibration);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: calculate calibration failed.\n");
        (void)ina226_deinit(&ina226_iic_handle);
        napi_throw_error(env, INIT_ERROR,
            "Failed to calculate INA226 calibration");
        return NULL;
    }
    res = ina226_set_calibration(&ina226_iic_handle, calibration);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: set calibration failed.\n");
        (void)ina226_deinit(&ina226_iic_handle);
        napi_throw_error(env, INIT_ERROR,
            "Failed to set INA226 calibration");
        return NULL;
    }
    
    /* set operating mode */
    res = ina226_set_mode(&ina226_iic_handle, (ina226_mode_t)mode);
    if (res != 0)
    {
        ina226_interface_debug_print("ina226: set mode failed.\n");
        (void)ina226_deinit(&ina226_iic_handle);
        napi_throw_error(env, INIT_ERROR, "Failed to set INA226 mode");
        return NULL;
    }
    
    return NULL;
}

napi_value ina226_read_shunt_voltage_wrapper(napi_env env,
                                             napi_callback_info info) {
    size_t argc = 0; // No arguments expected
    napi_get_cb_info(env, info, &argc, NULL, NULL, NULL);

    if (argc != 0) {
        napi_throw_error(env, WRONG_NUMBER_OF_ARGUMENTS,
                         "Check number of arguments for fn:"
                         " ina226_read_shunt_voltage_wrapper(..)");
        return NULL;
    }

    int16_t raw;
    float mV;
    uint8_t res = ina226_read_shunt_voltage(&ina226_iic_handle,
        &raw, &mV);
    if (res != 0) {
        napi_throw_error(env, INIT_ERROR, "Failed to read shunt voltage");
        return NULL;
    }

    napi_value jsMV;
    napi_status status = napi_create_double(env, mV, &jsMV);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to create NAPI value for shunt voltage");
        return NULL;
    }

    return jsMV;
}

napi_value ina226_read_bus_voltage_wrapper(napi_env env,
                                           napi_callback_info info) {
    size_t argc = 0; // No arguments expected
    napi_get_cb_info(env, info, &argc, NULL, NULL, NULL);

    if (argc != 0) {
        napi_throw_error(env, WRONG_NUMBER_OF_ARGUMENTS,
                         "Check number of arguments for fn:"
                         " ina226_read_bus_voltage_wrapper(..)");
        return NULL;
    }

    uint16_t raw;
    float mV;
    uint8_t res = ina226_read_bus_voltage(&ina226_iic_handle, &raw, &mV);
    if (res != 0) {
        napi_throw_error(env, INIT_ERROR, "Failed to read bus voltage");
        return NULL;
    }

    napi_value jsMV;
    napi_status status = napi_create_double(env, mV, &jsMV);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to create NAPI value for bus voltage");
        return NULL;
    }

    return jsMV;
}

napi_value ina226_read_current_wrapper(napi_env env, napi_callback_info info) {
    size_t argc = 0; // No arguments expected
    napi_get_cb_info(env, info, &argc, NULL, NULL, NULL);

    if (argc != 0) {
        napi_throw_error(env, WRONG_NUMBER_OF_ARGUMENTS,
                         "Check number of arguments for fn:"
                         " ina226_read_current_wrapper(..)");
        return NULL;
    }

    int16_t raw;
    float mA;
    uint8_t res = ina226_read_current(&ina226_iic_handle, &raw, &mA);
    if (res != 0) {
        napi_throw_error(env, INIT_ERROR, "Failed to read current");
        return NULL;
    }

    napi_value jsMA;
    napi_status status = napi_create_double(env, mA, &jsMA);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to create NAPI value for current");
        return NULL;
    }

    return jsMA;
}

napi_value ina226_read_power_wrapper(napi_env env, napi_callback_info info) {
    size_t argc = 0; // No arguments expected
    napi_get_cb_info(env, info, &argc, NULL, NULL, NULL);

    if (argc != 0) {
        napi_throw_error(env, WRONG_NUMBER_OF_ARGUMENTS,
                         "getPower() takes no arguments");
        return NULL;
    }

    uint16_t raw;
    float mW;
    uint8_t res = ina226_read_power(&ina226_iic_handle, &raw, &mW);
    if (res != 0) {
        napi_throw_error(env, INIT_ERROR, "Failed to read power");
        return NULL;
    }

    napi_value jsMW;
    napi_status status = napi_create_double(env, mW, &jsMW);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to create NAPI value for power");
        return NULL;
    }

    return jsMW;
}

napi_value ina226_soft_reset_wrapper(napi_env env, napi_callback_info info) {
    size_t argc = 0; // No arguments expected
    napi_get_cb_info(env, info, &argc, NULL, NULL, NULL);
    if (argc != 0) {
        napi_throw_error(env, WRONG_NUMBER_OF_ARGUMENTS,
                         "Check number of arguments for fn:"
                         " ina226_soft_reset_wrapper(..)");
        return NULL;
    }
    uint8_t res = ina226_soft_reset(&ina226_iic_handle);
    switch (res) {
        case 0:
            // Success
            break;
        case 1:
            napi_throw_error(env, ERROR_RESETTING_DEVICE, "Soft reset failed");
            return NULL;
        case 2:
            napi_throw_error(env, ERROR_RESETTING_DEVICE, "Handle is NULL");
            return NULL;
        case 3:
            napi_throw_error(env, ERROR_RESETTING_DEVICE,
                             "Handle is not initialized");
            return NULL;
        default:
            napi_throw_error(env, ERROR_RESETTING_DEVICE,
                             "Unexpected error during soft reset");
            return NULL;
    }

    return NULL; // No return value needed for soft reset
}

/**
 * Set INA226 mode.
 *
 * Possible modes are enumerated in `INA226Mode` enum.
 *
 * Parameters when calling from JavaScript:
 * - `mode`: The mode to set, which is described in the `INA226Mode` enum.
 *
 * Returns nothing. Throws on error.
 *
 * JS errors that can be thrown:
 * - `INIT_ERROR`: If the INA226 handle is not initialized or if setting the
 *                 mode fails.
 * - `ERROR_CREATING_NAPI_VALUE`: If there is an error creating the NAPI value
 *                                for the mode.
 * - `WRONG_NUMBER_OF_ARGUMENTS`: If the number of arguments passed is not
 *                                correct.
 */
napi_value ina226_set_mode_wrapper(napi_env env, napi_callback_info info) {
    size_t argc = 1; // One argument expected
    napi_value argv[argc], this;

    napi_get_cb_info(env, info, &argc, argv, &this, NULL);

    if (argc != 1) {
        napi_throw_error(
            env, WRONG_NUMBER_OF_ARGUMENTS,
            "Check number of arguments for fn: ina226_set_mode_wrapper(..)");
        return NULL;
    }

    int32_t mode;
    napi_status status = napi_get_value_int32(env, argv[0], &mode);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to create NAPI value for mode");
        return NULL;
    }

    uint8_t res = ina226_set_mode(&ina226_iic_handle,
        (ina226_mode_t)mode);
    if (res != 0) {
        napi_throw_error(env, INIT_ERROR, "Failed to set INA226 mode");
        return NULL;
    }

    return NULL;
}
