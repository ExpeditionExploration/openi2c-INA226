/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      driver_ina226_interface_template.c
 * @brief     driver ina226 interface template source file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2025-01-29
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2025/01/29  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#include "driver_ina226_interface.h"
#include <stdint.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <endian.h>

char IIC_BUS[50];
static int iic_fd = -1;
void set_iic_bus(const uint32_t iic_bus) {
    if (iic_bus != 0) {
        snprintf(IIC_BUS, sizeof(IIC_BUS), "/dev/i2c-%d", iic_bus);
    } else {
        fprintf(stderr,
            "Warning: I2C bus not set, using default /dev/i2c-1\n");
        snprintf(IIC_BUS, sizeof(IIC_BUS), "/dev/i2c-1");
    }
}

/**
 * @brief  interface iic bus init
 * @return status code
 *         - 0 success
 *         - 1 iic init failed
 * @note   none
 */
uint8_t ina226_interface_iic_init(void)
{
    if ((iic_fd = open(IIC_BUS, O_RDWR)) < 0) {
        perror("Failed to open i2c bus");
        return 1;
    }
    return 0;
}

/**
 * @brief  interface iic bus deinit
 * @return status code
 *         - 0 success
 *         - 1 iic deinit failed
 * @note   none
 */
uint8_t ina226_interface_iic_deinit(void)
{
    if (close(iic_fd) < 0) {
        perror("Failed to close i2c bus");
        return 1;
    }
    iic_fd = -1; // Reset the file descriptor
    return 0;
}

/**
 * @brief      interface iic bus read
 * @param[in]  addr iic device write address
 * @param[in]  reg iic register address
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len length of the data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t ina226_interface_iic_read(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (iic_fd < 0) {
        fprintf(stderr, "iic: device not opened\n");
        return 1;
    }
    if (buf == NULL || len == 0) {
        fprintf(stderr, "iic: invalid read buffer/len\n");
        return 1;
    }

    /* Set 7-bit slave address */
    if (ioctl(iic_fd, I2C_SLAVE, addr) < 0) {
        perror("iic: set slave address failed");
        return 1;
    }

    struct i2c_rdwr_ioctl_data rdwr;
    struct i2c_msg msgs[2];
    uint8_t reg_byte = reg;

    msgs[0].addr  = addr;
    msgs[0].flags = 0;
    msgs[0].len   = 1;
    msgs[0].buf   = &reg_byte;

    msgs[1].addr  = addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len   = len;
    msgs[1].buf   = buf;

    rdwr.msgs  = msgs;
    rdwr.nmsgs = 2;

    if (ioctl(iic_fd, I2C_RDWR, &rdwr) < 0) {
        perror("iic: read transaction failed");
        return 1;
    }

#if __BYTE_ORDER == __LITTLE_ENDIAN
    /* INA226 provides registers big-endian. Convert in-place to host order
       if caller likely expects little-endian 16-bit words (even length). */
    if ((len & 1) == 0) {
        for (uint16_t i = 0; i < len; i += 2) {
            uint16_t be = ((uint16_t)buf[i] << 8) | buf[i + 1];
            uint16_t le = be16toh(be);
            buf[i]     = (uint8_t)(le & 0xFF);
            buf[i + 1] = (uint8_t)((le >> 8) & 0xFF);
        }
    }
#endif

    return 0;
}

/**
 * @brief     interface iic bus write
 * @param[in] addr iic device write address
 * @param[in] reg iic register address
 * @param[in] *buf pointer to a data buffer
 * @param[in] len length of the data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t ina226_interface_iic_write(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (iic_fd < 0) {
        fprintf(stderr, "iic: device not opened\n");
        return 1;
    }

    /* Set the slave address (7-bit) */
    if (ioctl(iic_fd, I2C_SLAVE, addr) < 0) {
        perror("iic: set slave address failed");
        return 1;
    }

    /* Build transmit buffer: register pointer + payload */
    uint8_t tx[len + 1];
    tx[0] = reg;
    if (len && buf) {
        memcpy(&tx[1], buf, len);
    }

    ssize_t w = write(iic_fd, tx, len + 1);
    if (w != (ssize_t)(len + 1)) {
        if (w < 0) perror("iic: write failed");
        else fprintf(stderr, "iic: short write (%zd/%u)\n", w, len + 1);
        return 1;
    }

    return 0;
}

/**
 * @brief     interface delay ms
 * @param[in] ms time
 * @note      none
 */
void ina226_interface_delay_ms(uint32_t ms)
{
    usleep(ms * 1000);
}

/**
 * @brief     interface print format data
 * @param[in] fmt format data
 * @note      none
 */
void ina226_interface_debug_print(const char *const fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

/**
 * @brief     interface receive callback
 * @param[in] type irq type
 * @note      none
 */
void ina226_interface_receive_callback(uint8_t type)
{
    switch (type)
    {
        case INA226_STATUS_SHUNT_VOLTAGE_OVER_VOLTAGE :
        {
            ina226_interface_debug_print("ina226: irq shunt voltage over voltage.\n");
            
            break;
        }
        case INA226_STATUS_SHUNT_VOLTAGE_UNDER_VOLTAGE :
        {
            ina226_interface_debug_print("ina226: irq shunt voltage under voltage.\n");
            
            break;
        }
        case INA226_STATUS_BUS_VOLTAGE_OVER_VOLTAGE :
        {
            ina226_interface_debug_print("ina226: irq bus voltage over voltage.\n");
            
            break;
        }
        case INA226_STATUS_BUS_VOLTAGE_UNDER_VOLTAGE :
        {
            ina226_interface_debug_print("ina226: irq bus voltage under voltage.\n");
            
            break;
        }
        case INA226_STATUS_POWER_OVER_LIMIT :
        {
            ina226_interface_debug_print("ina226: irq power over limit.\n");
            
            break;
        }
        default :
        {
            ina226_interface_debug_print("ina226: unknown code.\n");
            
            break;
        }
    }
}
