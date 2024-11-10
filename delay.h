/*
 * delay.c
 *
 *  Created on: Feb 22, 2023
 *      Author: Silvano Cortesi
 *  University: ETH Zurich
 *   Institute: D-ITET Center for Project-Based Learning
 */

#ifndef DELAY_H_
#define DELAY_H_

#include <stdint.h>
#include "msp.h"

/* Function to delay for specific amount of ticks */
/* Uses SysTick */
/* Ticks at max 2^24 - 1 */
void delay_ticks(uint32_t ticks);

/* Function to delay for specific amount of us */
/* Uses SysTick */
/* us at max 44'739'240 */
void delay_us(uint32_t us);

/* Function to delay for specific amount of ms */
/* Uses SysTick */
/* ms at max 44'739 */
void delay_ms(uint32_t ms);

#endif /* DELAY_H_ */
