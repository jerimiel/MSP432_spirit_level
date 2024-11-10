/*
 * main.c
 *
 *  Created on: May 30, 2024
 *      Author: Philipp Schilk
 *  University: ETH Zurich
 *   Institute: D-ITET Center for Project-Based Learning
 */
#include "bmi160.h"
#include "project.h"

//              ___ __  __ ____   ___  ____ _____  _    _   _ _____
//             |_ _|  \/  |  _ \ / _ \|  _ \_   _|/ \  | \ | |_   _|
//  =========   | || |\/| | |_) | | | | |_) || | / _ \ |  \| | | |   =========
//  =========   | || |  | |  __/| |_| |  _ < | |/ ___ \| |\  | | |   =========
//             |___|_|  |_|_|    \___/|_| \_\|_/_/   \_\_| \_| |_|
//
//  Please read the following carefully:
//    - You are permitted to solve problems in groups.
//    - You must write your own code.
//    - DO NOT modify any code outside of this file - it will not be submitted
//      or graded! If your code depends on modifications outside of this file 
//      it will not work correctly while grading and will cost you points!
//    - Please read the handout carefully. We recommend completing the steps 
//      in the order they are numbered/listed in the handout, which is not 
//      necessarily the order in which they appear in this file!
//
//  Please fill out the following:
//
//    Full Name: VICTOR APEH
//    Group Members/Collaborators:
//
//  ==========================================================================
//  ==========================================================================

//chip id address macro
#define CHIPID 0x00 //Chip-id address
#define DEVICE_ADDRESS 0x69 //I2C address for BMI160
#define PI 3.142
#define PWM_CCR0 100 //PWM CCR0 value

/* Port mapper configuration register */
const uint8_t port_mapping[] =
{
  PMAP_TA1CCR1A, PMAP_NONE, PMAP_NONE, PMAP_NONE, PMAP_NONE, PMAP_NONE, PMAP_NONE, PMAP_NONE
};


void check_for_err(int i, char *msg);
void error(char *msg);

void main(void) {

  // ==== STEP 0: BASIC GPIO ===================================================
  // Configure the pin connect to button S1 as an input (with pull up) and
  // the output connected to LED1 as an output.
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);//setting up button S1
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);//setting red led as output

  // TODO

  // ==== STEP 1: UART CONFIG ==================================================
  // Configure UART to run at a baud rate of 115200 with the standard 8N1 config.
  // Hint: SMCLK Runs at 3MHz.
  // Remember to initialize and start the uart peripheral!
    static const eUSCI_UART_Config uart_config = {
        // SMCLK Clock Source Selected
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,
        // Baudrate Divider UCBRx
        1,
        // Clock Compensation Factor UCxBRF
        10,
        // Clock Compensation Factor UCxBRS
        0,
        // No Parity
        EUSCI_A_UART_NO_PARITY,
        // LSB First
        EUSCI_A_UART_LSB_FIRST,
        // One stop bit
        EUSCI_A_UART_ONE_STOP_BIT,
        // UART mode
        EUSCI_A_UART_MODE,
        // Oversampling Mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
    };
    //Initialization of UART
    UART_initModule(EUSCI_A0_BASE, &uart_config);
    //Enabling UART
    UART_enableModule(EUSCI_A0_BASE);


  // Config UART GPIOs:
  GPIO_setAsPeripheralModuleFunctionOutputPin(UART_TX_PORT, UART_TX_PIN, UART_GPIO_AF);
  GPIO_setAsPeripheralModuleFunctionOutputPin(UART_RX_PORT, UART_RX_PIN, UART_GPIO_AF);

  // TODO

  // We have implemented the familiar uart_println() function that accepts formatting
  // specifiers like printf. Test that it works:
  uart_println("Hello World!");

  // ==== STEP 2: SENSOR COMMUNICATION CHECK ===================================
  // Now, use the sensor board schematic and BMI160 datasheet to determine
  // the I2C address of the BMI160 IMU (Inertial Measurement Unit).
  //
  // Then identify the address and value of the BMI160's "CHIPID" register.
  // Use the i2c read function provided in 'i2c_helper.c' to read from that register.
  // Validate that the value is correct. If so, print "CHIPID OK". Otherwise, call
  // the "error" function with an error message.

  // We have already prepared the I2C port for you in i2c_helper.c
  // First, run the init code:
  check_for_err(i2c_init(), "i2c_init");

  // TODO
  //Initialize a variable to hold value retrieved from i2_c read
  uint8_t val;

  i2c_read(DEVICE_ADDRESS,CHIPID, &val, 1); //Reading chip id value

  if(val == 0xD1){//Checking to see if value read is correct
      uart_println("CHIPID OK\n\r");
  }
  else
  {
      uart_println("CHIP ID incorrect, check code!!\n\r");
  }

  // ==== STEP 3: SENSOR CONFIGURATION =========================================
  // Next, use the provided BMI160 driver to configure the sensor.
  // Use the README of the sensor driver (libraries/bosch/bmi160) to understand how
  // to use the driver.
  //
  // Hint: The i2c_read/i2c_write functions given in i2c_helper.c are already
  //       compatible with the driver. You can pass them to the driver directly.
  //
  //       Also the delay_ms function defined in delay.c is directly compatible.
  //
  //       Make sure to inform the driver of the correct I2C address!
  //
  // We require the following configuration:
  // - Gyroscope: Disabled
  // - Accelerometer: Normal Mode.
  //   - sample rate: 25hz
  //   - range: 2g
  //   - bandwidth: 'ACCEL_BW_NORMAL_AVG4'
  //
  // Furthermore, we need a way of detecting when a new sample is ready. The BMI160
  // has two outputs/channels (INT1 and INT2) that can be configured to output a signal
  // when some internal event has happened.
  //
  // Use the driver (and it's documentation) to configure the output pin/channel 1:
  //   - A push-pull output.
  //   - Active high.
  //   - Not latched.
  //   - To signal when accelerometer or gyroscope data  is ready.
  //
  // Also configure the microcontroller GPIO pin connected to this signal to be
  // an input. We will poll this signal, so you do not need to configure any
  // microcontroller interrupts.

  // === BASE SENSOR CONFIG ===
  struct bmi160_dev device; //BMI160 sensor struct

  device.chip_id = CHIPID; //Chip id
  device.id = DEVICE_ADDRESS; //Device I2C address
  device.interface = BMI160_I2C_INTF; //Picking I2C interface
  device.read = i2c_read; //read function
  device.write = i2c_write; //write function
  device.delay_ms = delay_ms; //delay function
  bmi160_init(&device); //line to initialize the device

    // TODO

  // === ACCEL/GYRO SENSOR CONFIG ===
  device.gyro_cfg.power = BMI160_GYRO_SUSPEND_MODE; //Putting the gyroscope in suspend mode to disable it
  device.accel_cfg.power = BMI160_ACCEL_NORMAL_MODE; //Normal power mode for accelerometer
  device.accel_cfg.odr = BMI160_ACCEL_ODR_25HZ; //25Hz sample rate
  device.accel_cfg.range = BMI160_ACCEL_RANGE_2G; //2G acceleration range
  device.accel_cfg.bw = BMI160_ACCEL_BW_NORMAL_AVG4; //Acceleration Bandwith
  bmi160_set_sens_conf(&device); //Setting configurations for the device

  // TODO

  // === DATA READY INTERRUPT ===
  struct bmi160_int_settg int_config;
  int_config.int_channel = BMI160_INT_CHANNEL_1; //Choosing INT1 as the interrupt channel
  int_config.int_pin_settg.output_en = 1; //Enable output
  int_config.int_pin_settg.output_mode = 0; //Selecting push-pull as output mode
  int_config.int_pin_settg.latch_dur = BMI160_LATCH_DUR_NONE; //Setting to not latched
  int_config.int_type = BMI160_ACC_GYRO_DATA_RDY_INT; //Setting data ready interrupt signal
  bmi160_set_int_config(&int_config, &device); // Setting interrupt configurations for the device

  GPIO_setAsInputPin(GPIO_PORT_P4, GPIO_PIN1); //setting Pin 1 port 4 as an input pin.


  // TODO

  // ==== STEP 7: PWM SETUP ===============================================================
  // Finally, configure Timer A1 to generate a PWM signal that controls the brightness
  // of the red LED inside LED2.
  //
  // Note that you have to:
  // - Configure the GPIO connected to that LED to be connected to the capture compare
  //   output of the timer.
  // - Configure the timer to run at an appropriate speed.
  // - Configure the timer compare register.
  // - Start the timer.
  /* Remapping TA1CCR1 output pin to P2.0 (red LED of LED2) */

  // TODO
  //Port mapping to port 2
  PMAP_configurePorts((const uint8_t *) port_mapping,
                          PMAP_P2MAP,
                          1,
                          PMAP_DISABLE_RECONFIGURATION);

      /* Set pin P2.0 as output pin */
      GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,
                                                  GPIO_PIN0,
                                                  GPIO_PRIMARY_MODULE_FUNCTION);
    const Timer_A_UpModeConfig upConfigA1 =
      {
        // SMCLK clock source
       TIMER_A_CLOCKSOURCE_SMCLK,
        // SMCLK/1 = 3MHz
       TIMER_A_CLOCKSOURCE_DIVIDER_1,
       // Value in CCR0; It determines period
       PWM_CCR0,
        // Disable Timer interrupt
       TIMER_A_TAIE_INTERRUPT_DISABLE,
        // Disable CCR0 interrupt
       TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,
        // Clear value
       TIMER_A_DO_CLEAR,
      };

    //Configure capture compare register
    Timer_A_CompareModeConfig compareConfigA1 =
      {
       // Use CCR1 as compare register
       TIMER_A_CAPTURECOMPARE_REGISTER_1,
        // Disable  CCR interrupt
       TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,
        // Toggle-reset output mode
       TIMER_A_OUTPUTMODE_TOGGLE_RESET,
      };
    /* Initialize compare mode for Timer_A1 */
    Timer_A_initCompare(TIMER_A1_BASE, &compareConfigA1);
    /* Configure Timer_A1 for Up Mode */
    Timer_A_configureUpMode(TIMER_A1_BASE, &upConfigA1);
    /* Start counter of Timer_A1 */
    Timer_A_startCounter(TIMER_A1_BASE,TIMER_A_UP_MODE);

  // ==== MAIN LOOP ============================================================

  // Zero vector.
  // Your firmware should calculate the angle between the latest accelerometer
  // reading and this vector:
  float zerovec_x = 0;
  float zerovec_y = 0;
  float zerovec_z = 1;

  // Acceleration data struct
  struct bmi160_sensor_data accel;

  //Initializing variables to hold new vector
  float new_x;
  float new_y;
  float new_z;

  float angle_deg; // Variable to hold angle in degrees
  float abs_zer; // Variable to hold result of |z|
  float abs_norm; // Variable to hold result of |r|
  uint8_t compare_value; // Variable to hold changing compare values
  uint8_t holder;

  while (1) {
    // ==== STEP 4: RETRIEVING SENSOR DATA =====================================
    // First, read the state of the GPIO pin connected to the 'DATA READY' signal
    // of the BMI160 you configured above. If it indicates that *no* new data is
    // available, continue to the next while-loop iteration to keep polling.
    //
    // If new data is available, use the bmi160 driver to retrieve the accelerometer
    // reading and print it to the computer via UART.

      //Polling to see if data is ready
      if (GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN1))
      {
          bmi160_get_sensor_data(BMI160_ACCEL_SEL, &accel, NULL, &device); //Reading sensor data from device

          uart_println("X = %d  Y = %d  Z = %d", accel.x, accel.y,accel.z);
      }

    // TODO

    // ==== STEP 5: DATA PROCESSING ============================================
    // Next, convert the raw accelerometer reading to floating point values and
    // normalize it.

    // Dividing by 16384 to normalize because normalized = LSB/typical value which for 2g range is 16384 from datasheet
      new_x = (float)accel.x/16384;
      new_y = (float)accel.y/16384;
      new_z = (float)accel.z/16384;

    // Then, check if the button S1 is pressed. If so, set zerovec to the current
    // (normalized) accelerometer reading.
      //Polling for when button is pressed
      holder = GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1);
      if (!holder)
      {
          zerovec_x = new_x;
          zerovec_y = new_y;
          zerovec_z = new_z;
      }
    //
    // Finally calculate the angle between the current normalized accelerometer
    // reading and the zerovec vector (in degrees). Print this value via UART.
      abs_zer = sqrt((zerovec_x*zerovec_x) + (zerovec_y*zerovec_y) + (zerovec_z*zerovec_z));
      abs_norm = sqrt((new_x*new_x) + (new_y*new_y) + (new_z*new_z));

      angle_deg = round(acosl((new_x*zerovec_x + new_y*zerovec_y + new_z*zerovec_z)/(abs_zer * abs_norm)) * 180/PI);

      uart_println("angle = %.2f\r\n", angle_deg);

    // TODO

    // ==== STEP 6: SIMPLE LED =================================================
    // Next, check if the angle you calculated is within +- 5 degrees. If so,
    // turn on LED1. Otherwise, turn off LED1.
      if (angle_deg <= 5)
      {
          GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
      }
      else
      {
          GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
      }

    // TODO

    // ==== STEP 7: PWM ========================================================
    // Finally, control the brightness of the LED connected to the timer based
    // on the angle you calculated. The LED should be off for any angle outside of
    // +-45deg, at its lowest brightness at an angle of +-45 degrees, and at its
    // brightest at an angle of 0deg.

    // TODO
      /*To get the new compare value, I calculated the percentage
       * of total period which current angle represents
       * and then subtracted it out of total period to find the CCR1*/
      compare_value = PWM_CCR0 - (PWM_CCR0 * (45-angle_deg)/45); // new compare value
      if (angle_deg <= 45)
      {
          Timer_A_setCompareValue(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1,compare_value);
      }
      else
      {
          Timer_A_setCompareValue(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1,PWM_CCR0+1);/*Because Timer will never count to CCR0+1*/
      }
  }
}

void check_for_err(int i, char *msg) {
  if (i != 0) {
    error(msg);
  }
}

void error(char *msg) {
  uart_println("Error during step: %s!", msg);
  while (1) {
  }
}
