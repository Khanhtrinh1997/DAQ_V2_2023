
//#include "fsl_uart.h"
//#include "pin_mux.h"
//#include "clock_config.h"
#include "Header.h"

#define RS2321_UART 	                UART1
#define RS2321_UART_CLKSRC 		SYS_CLK
#define RS2321_UART_IRQn 		UART1_RX_TX_IRQn
#define RS2321_UART_IRQHandler  	UART1_RX_TX_IRQHandler

#define RS2322_UART 			UART4
#define RS2322_UART_CLKSRC 		BUS_CLK
#define RS2322_UART_IRQn 		UART4_RX_TX_IRQn
#define RS2322_UART_IRQHandler 		UART4_RX_TX_IRQHandler

#define RS2321_UART_BAUDRATE 14400//28800
#define RS2322_UART_BAUDRATE 14400//28800

void Init_RS232_UART (void);
extern uint32_t RX2321Count,RX2322Count;
extern uart_config_t config;
