/*
 * uart.c
 *
 *  Created on: Feb 20, 2023
 *      Author: Silvano Cortesi
 *  University: ETH Zurich
 *   Institute: D-ITET Center for Project-Based Learning
 */

#include "project.h"
#include "uart.h"

/**
 * Send string via UART peripheral
 * @param str Pointer to the message to send
 * @param ... printf-style parameters
 */
void uart_println(const char* str, ...)
{
  static char print_buffer[256];

  va_list lst;
  va_start(lst, str);
  vsnprintf(print_buffer, 256, str, lst);
  str = print_buffer;
  while (*str) 
  {
    while (!(EUSCI_A_CMSIS(UART_INTERFACE)->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A_CMSIS(UART_INTERFACE)->TXBUF = *str;
    str++;
  }
  while (!(EUSCI_A_CMSIS(UART_INTERFACE)->IFG & EUSCI_A_IFG_TXIFG));
  EUSCI_A_CMSIS(UART_INTERFACE)->TXBUF = '\r';
  while (!(EUSCI_A_CMSIS(UART_INTERFACE)->IFG & EUSCI_A_IFG_TXIFG));
  EUSCI_A_CMSIS(UART_INTERFACE)->TXBUF = '\n';
}


/**
 * If UART_PRINTF is enabled, override required system calls.
 */
#if UART_PRINTF > 0
int fputc(int c, register FILE* stream)
{
  UART_transmitData(UART_INTERFACE, (uint8_t)c);
  return c;
}

int fputs(const char* s, register FILE* stream)
{
  uart_println(s);
  return 0;
}
#endif /* UART_PRINTF > 0 */
