/*
 * delay.c
 *
 *  Created on: Feb 22, 2023
 *      Author: Silvano Cortesi
 *  University: ETH Zurich
 *   Institute: D-ITET Center for Project-Based Learning
 */

#include "delay.h"
#include "project.h"

/* Function to delay for specific amount of ticks */
/* Uses SysTick */
/* Ticks at max 2^24 - 1 */
void delay_ticks(uint32_t ticks)
{
    // Set an overflow value for the SysTick timer
    SysTick->LOAD = ticks;
    // Reset the SysTick timer to 0
    SysTick->VAL = 0;
    // Enable the SysTick timer
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;

    // COUNTFLAG is a bit that is set to 1 when counter reaches 0 again (e.g. Overflow).
    // It's automatically cleared when read.
    while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);

    // Disable the SysTick timer
    SysTick->CTRL = 0;
}

/* Function to delay for specific amount of us */
/* Uses SysTick */
/* us at max 44'739'240 */
void delay_us(uint32_t us)
{
    // SysTick uses SMCLK
    delay_ticks((us * (CS_getSMCLK())) / 1000000);
}

/* Function to delay for specific amount of ms */
/* Uses SysTick */
/* ms at max 44'739 */
void delay_ms(uint32_t ms)
{
    // SysTick uses SMCLK
    delay_ticks((ms * (CS_getSMCLK())) / 1000);
}
