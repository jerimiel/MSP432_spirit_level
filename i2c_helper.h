/*
 * i2c_helper.h
 *
 *  Created on: Feb 22, 2023
 *      Author: Silvano Cortesi
 *  University: ETH Zurich
 *   Institute: D-ITET Center for Project-Based Learning
 */

#ifndef I2C_HELPER_H_
#define I2C_HELPER_H_

#include <stdint.h>

#include "ti/devices/msp432p4xx/driverlib/driverlib.h"
#include "msp.h"

#define I2C_INTERFACE    EUSCI_B1_BASE
#define I2C_TIMEOUT      ((uint32_t) 10000)

#define I2C_SCL_PORT     GPIO_PORT_P6
#define I2C_SCL_PIN      GPIO_PIN5
#define I2C_SDA_PORT     GPIO_PORT_P6
#define I2C_SDA_PIN      GPIO_PIN4
#define I2C_GPIO_AF      GPIO_PRIMARY_MODULE_FUNCTION

#define I2C_OK               0
#define I2C_INIT_ERROR       1
#define I2C_CONFIG_ERROR     2
#define I2C_INTERUPT_ERROR   3

/**
 * Initialize the I2C and the low-level peripherals used to access it.
 * @return Zero on success, non-zero error code on failure.
 */
uint8_t i2c_init(void);

/**
 * Initialize the GPIO pins used for interfacing the i2c.
 */
void i2c_init_gpio(void);

/**
 * Read data from one or multiple sensor register(s).
 * @param device_address The sensors I2C bus address
 * @param register_address The sensors register address to start reading from
 * @param data Pointer to the memory where the received data is stored
 * @param count Number of bytes to read
 * @return Status code, zero on success, non-zero in case of errors
 */
int8_t i2c_read(uint8_t device_address, uint8_t register_address, uint8_t *data,
                uint16_t count);

/**
 * Write data to one or multiple sensor register(s).
 * @param device_address The sensors I2C bus address
 * @param register_address The sensors register address to write
 * @param data Pointer to the data to be written
 * @param count Number of bytes to write
 * @return Status code, zero on success, non-zero in case of errors
 */
int8_t i2c_write(uint8_t device_address, uint8_t register_address,
                 uint8_t *data, uint16_t count);

#endif /* I2C_HELPER_H_ */
