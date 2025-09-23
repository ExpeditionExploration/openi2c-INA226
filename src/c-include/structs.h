#ifndef STRUCTS_H
#define STRUCTS_H

#include <node_api.h>

#include "driver_ina226.h"

napi_value ina226_info_to_js(napi_env env, const ina226_info_t *info);

/**
 * @brief Make a Buffer object from an ina226_handle_t pointer.
 *
 * - From calling this function, the JavaScript side will garbage collect, free,
 * and deinit the ina226_handle_t pointer when the Buffer is no longer needed.
 * - This function assumes the `handle` to have been allocated using `malloc()`.
 *
 * @return A Buffer object convertible to `ina226_handle_t` in C.
 *
 * In case of an error, it throws `error creating napi value` error and returns
 * `NULL`.
 */
napi_value ina226_handle_t_to_js(napi_env env, ina226_handle_t *handle);

#endif
