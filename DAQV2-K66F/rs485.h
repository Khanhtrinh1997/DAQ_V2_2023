#include "board.h"
#include "fsl_gpio.h"
#include "fsl_uart.h"
#include "pin_mux.h"
#include "clock_config.h"

#if ((USERDEF_RS485_AGISSON == ENABLED)||((USERDEF_RS485_DKD51_BDP == ENABLED))||(USERDEF_RS485_VERTIV_M830B == ENABLED))
#define RS485R_UART 	                UART0
#define RS485R_UART_CLKSRC 		SYS_CLK
#define DKD51_RS485R_UART_CLKSRC        BUS_CLK
#define RS485R_UART_IRQn 		UART0_RX_TX_IRQn
//#define RS485_UART_IRQHandler  	UART0_RX_TX_IRQHandler
#define RS485T_UART 			UART0
#define RS485T_UART_CLKSRC 		SYS_CLK
#define DKD51_RS485T_UART_CLKSRC        BUS_CLK
#define RS485T_UART_IRQn 		UART0_RX_TX_IRQn
#define RS485T_UART_IRQHandler 		UART0_RX_TX_IRQHandler

#define RS485R_UART_BAUDRATE 14400//28800
#define RS485T_UART_BAUDRATE 14400//28800

extern uint8_t Recv03;
void Init_RS485_UART(void);
void Init_RS485_UART_TEST(void);
void Looptest(uint8_t ucChar);
void RS485_UART_IRQHandler(void);
extern uint32_t RX485Count;
extern uint8_t RecvComplete;
#endif