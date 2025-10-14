#include <node_api.h>
#include <string.h>

#include "fn-bindings.h"

static uint8_t register_fn(napi_env env, napi_value exports, const char* name,
                           napi_callback cb, void* context) {
    napi_value fn;
    napi_status status;
    status = napi_create_function(env, name, strlen(name), cb, context, &fn);
    status |= napi_set_named_property(env, exports, name, fn);
    if (status != napi_ok) {
        napi_throw_error(env, "init c extension error",
                         "couldn't register an fn");
        return 1;
    }

    return 0;
}

// Initialize the module
napi_value Init(napi_env env, napi_value exports) {
    napi_value info_fn, basic_init_fn;
    uint8_t status;

    // register_fn already throws, so no need to re-throw on error.
    status = register_fn(env, exports, "getSensorInfo", ina226_info_wrapper, NULL);
    if (status) return NULL;
    status = register_fn(env, exports, "init", basic_init, NULL);
    if (status) return NULL;
    status = register_fn(env, exports, "getShuntVoltage", ina226_read_shunt_voltage_wrapper, NULL);
    if (status) return NULL;
    status = register_fn(env, exports, "getBusVoltage", ina226_read_bus_voltage_wrapper, NULL);
    if (status) return NULL;
    status = register_fn(env, exports, "getCurrent", ina226_read_current_wrapper, NULL);
    if (status) return NULL;
    status = register_fn(env, exports, "getPower", ina226_read_power_wrapper, NULL);
    if (status) return NULL;
    status = register_fn(env, exports, "softReset", ina226_soft_reset_wrapper, NULL);
    if (status) return NULL;
    status = register_fn(env, exports, "setMode", ina226_set_mode_wrapper, NULL);
    if (status) return NULL;

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
