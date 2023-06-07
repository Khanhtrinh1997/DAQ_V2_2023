#include "fsl_uart.h"

status_t UART_9Bit_Init(UART_Type *base, const uart_config_t *config, uint32_t srcClock_Hz);
void UART_9Bit_Send(UART_Type *base, uint8_t data, uint8_t addrvalue);