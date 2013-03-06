#include<avr/io.h>
#include<avr/interrupt.h>

#include <stdint.h>
#include <stdio.h>

void UART_Init(void);//инициализация uart
int	uart_putchar(char c, FILE *stream);

/*
 * Size of internal line buffer used by uart_getchar().
 */
#define RX_BUFSIZE 80

/*
 * Receive one character from the UART.  The actual reception is
 * line-buffered, and one character is returned from the buffer at
 * each invokation.
 */
int	uart_getchar(FILE *stream);
