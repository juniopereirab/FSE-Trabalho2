#ifndef UART_H
#define UART_H

#include "uart_defs.h"

int initUart();
void requestToUart(int uart_filestream, unsigned char code);
void sendToUart(int uart_filestream, unsigned char code, int value);
void sendToUartByte(int uart_filestream, unsigned char code, char value);
Number_type readFromUart(int uart_filestream, unsigned char code);
void closeUart(int uart_filestream);

#endif