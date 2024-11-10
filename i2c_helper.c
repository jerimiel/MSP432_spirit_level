/*
 * i2c_helper.c
 *
 *  Created on: Feb 22, 2023
 *      Author: Silvano Cortesi
 *  University: ETH Zurich
 *   Institute: D-ITET Center for Project-Based Learning
 */

#include "i2c_helper.h"
#include "project.h"

/**
 * I2C interface configuration (with source clock frequency placeholder)
 */
eUSCI_I2C_MasterConfig i2c_config = {
  .selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK, // use the SMCLK as input clock
  .i2cClk = 0, // clock frequency will be set on i2c_init()
  .dataRate = EUSCI_B_I2C_SET_DATA_RATE_100KBPS, // configure the bus for 100kbps
  .byteCounterThreshold = 0, // Don't use the counter
  .autoSTOPGeneration = EUSCI_B_I2C_NO_AUTO_STOP, // Don't use the auto stop
};


/**
 * Initialize the I2C and the low-level peripherals used to access it.
 * @return Zero on success, non-zero error code on failure.
 */
uint8_t i2c_init(void)
{
  // initialize GPIO first
  i2c_init_gpio();

  // I2C master configuration
  // Set the i2cClk to be the current subsystem clock
  i2c_config.i2cClk = CS_getSMCLK();

  // Init I2C as master
  I2C_initMaster(I2C_INTERFACE, &i2c_config);

  // interrupt setup
  // -- none --

  // enable I2C interface
  I2C_enableModule(I2C_INTERFACE);

  // zero return value on success
  return I2C_OK;
}
/* ------------------------------------------------------------------------- */


/**
 * Initialize the GPIO pins used for interfacing the i2c.
 */
void i2c_init_gpio(void)
{
  // configure clock and data ports to connect to I2C peripheral
  GPIO_setAsPeripheralModuleFunctionOutputPin(
    I2C_SCL_PORT, I2C_SCL_PIN, I2C_GPIO_AF);
  GPIO_setAsPeripheralModuleFunctionOutputPin(
    I2C_SDA_PORT, I2C_SDA_PIN, I2C_GPIO_AF);

  // interrupt setup
  // -- none --
}
/* ------------------------------------------------------------------------- */


/**
 * Read data from one or multiple sensor register(s).
 * @param device_address The sensors I2C bus address
 * @param register_address The sensors register address to start reading from
 * @param data Pointer to the memory where the received data is stored
 * @param count Number of bytes to read
 * @return Status code, zero on success, non-zero in case of errors
 */
int8_t i2c_read(uint8_t device_address,
                uint8_t register_address,
                uint8_t* data,
                uint16_t count)
{
  /*
   * Data on the bus should be like
   * |------------+---------------------|
   * | I2C action | Data                |
   * |------------+---------------------|
   * | Start      | [device_address]    |
   * | Write      | (register_address)  |
   * | Start      | [device_address]    |
   * | Read       | (data[0])           |
   * | Read       | (....)              |
   * | Read       | (data[count - 1])   |
   * | Stop       | -                   |
   * |------------+---------------------|
   *
   * return 0 for success, non-zero for failure
   */

  // wait for I2C to be ready to read
  while (I2C_isBusBusy(I2C_INTERFACE))
  {
    continue;
  }

  // configure slave address
  I2C_setSlaveAddress(I2C_INTERFACE, device_address);

  // initiate read with START and writing register address
  I2C_masterSendMultiByteStart(I2C_INTERFACE, register_address);

  // wait for transmission to complete, clear interrupt flag
  while (!I2C_getInterruptStatus(I2C_INTERFACE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0))
  {
    continue;
  }
  I2C_clearInterruptFlag(I2C_INTERFACE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);

  // check for NACK, on NACK stop and exit with failure
  if (I2C_getInterruptStatus(I2C_INTERFACE, EUSCI_B_I2C_NAK_INTERRUPT))
  {
    I2C_masterSendMultiByteStop(I2C_INTERFACE);
    return -1;
  }

  // start receive transaction with RE-START
  I2C_masterReceiveStart(I2C_INTERFACE);

  // wait for RE-START to complete send
  while (I2C_masterIsStartSent(I2C_INTERFACE))
  {
    continue;
  }

  // receive all but last data byte
  while (count > 1)
  {
    // wait for next byte transmission to complete
    while (!I2C_getInterruptStatus(I2C_INTERFACE, EUSCI_B_I2C_RECEIVE_INTERRUPT0))
    {
      continue;
    }

    // read received byte, clears RX interrupt automatically
    *data = I2C_masterReceiveMultiByteNext(I2C_INTERFACE);
    data = data + 1;

    count = count - 1;
  }

  // --- broken driverlib implementation to handle reception of last byte and STOPing ---
  // *data = I2C_masterReceiveMultiByteFinish(I2C_INTERFACE);
  // return 0;
  // --- alternative handling using driverlib API below ---

  // receive last byte and generate stop condition
  I2C_masterReceiveMultiByteStop(I2C_INTERFACE);

  // wait for stop interrupt
  while (!I2C_getInterruptStatus(I2C_INTERFACE, EUSCI_B_I2C_STOP_INTERRUPT))
  {
    continue;
  }

  // wait for next byte transmission to complete
  while (!I2C_getInterruptStatus(I2C_INTERFACE, EUSCI_B_I2C_RECEIVE_INTERRUPT0))
  {
    continue;
  }

  // actually read last received value
  *data = I2C_masterReceiveMultiByteNext(I2C_INTERFACE);

  return 0;
}
/* ------------------------------------------------------------------------- */


/**
 * Write data to one or multiple sensor register(s).
 * @param device_address The sensors I2C bus address
 * @param register_address The sensors register address to write
 * @param data Pointer to the data to be written
 * @param count Number of bytes to write
 * @return Status code, zero on success, non-zero in case of errors
 */
int8_t i2c_write(uint8_t device_address,
                 uint8_t register_address,
                 uint8_t* data,
                 uint16_t count)
{
  /*
   * Data on the bus should be like
   * |------------+---------------------|
   * | I2C action | Data                |
   * |------------+---------------------|
   * | Start      | [device_address]    |
   * | Write      | (register_address)  |
   * | Write      | (data[0])           |
   * | Write      | (...)               |
   * | Write      | (data[count - 1])   |
   * | Stop       | -                   |
   * |------------+---------------------|
   *
   * return 0 for success, non-zero for failure
   */

  // wait for I2C to be ready to write
  while (I2C_isBusBusy(I2C_INTERFACE))
  {
    continue;
  }

  // configure slave address
  I2C_setSlaveAddress(I2C_INTERFACE, device_address);

  // initiate read with START and writing register address
  I2C_masterSendMultiByteStart(I2C_INTERFACE, register_address);

  // wait for transmission to complete
  while (!I2C_getInterruptStatus(I2C_INTERFACE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0))
  {
    continue;
  }

  // check for NACK, on NACK stop and exit with failure
  if (I2C_getInterruptStatus(I2C_INTERFACE, EUSCI_B_I2C_NAK_INTERRUPT))
  {
    I2C_masterSendMultiByteStop(I2C_INTERFACE);
    return -1;
  }

  // send data
  while (count > 0)
  {
    // wait for next byte transmission to complete
    while (!I2C_getInterruptStatus(I2C_INTERFACE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0))
    {
      continue;
    }

    // send next byte, clears TX interrupt automatically
    I2C_masterSendMultiByteNext(I2C_INTERFACE, *data);
    data = data + 1;

    count = count - 1;
  }

  // wait for last transmission to complete, send STOP and finally clear flag
  while (!I2C_getInterruptStatus(I2C_INTERFACE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0))
  {
    continue;
  }
  I2C_masterSendMultiByteStop(I2C_INTERFACE);
  I2C_clearInterruptFlag(I2C_INTERFACE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);

  return 0;
}
/* ------------------------------------------------------------------------- */
