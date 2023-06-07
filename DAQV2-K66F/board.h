 /* This is a template file for board configuration */

#ifndef _BOARD_H_
#define _BOARD_H_


#include "clock_config.h"
#include "fsl_gpio.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* The board name */
#define BOARD_NAME "###-not-specified-###"

#ifndef DEBUG_UART
#define DEBUG_UART 4
#endif
/*! @brief The UART to use for debug messages. */
#if (DEBUG_UART == 0)
#define BOARD_DEBUG_UART_TYPE DEBUG_CONSOLE_DEVICE_TYPE_UART
#define BOARD_DEBUG_UART_BASEADDR (uint32_t) UART0
#define BOARD_DEBUG_UART_CLKSRC SYS_CLK
#define BOARD_DEBUG_UART_CLK_FREQ CLOCK_GetCoreSysClkFreq()
#define BOARD_UART_IRQ UART0_RX_TX_IRQn
#define BOARD_UART_IRQ_HANDLER UART0_RX_TX_IRQHandler
#elif (DEBUG_UART == 4)
#define BOARD_DEBUG_UART_TYPE DEBUG_CONSOLE_DEVICE_TYPE_UART
#define BOARD_DEBUG_UART_BASEADDR (uint32_t) UART4
#define BOARD_DEBUG_UART_CLKSRC SYS_CLK//UART4_CLK_SRC//
#define BOARD_DEBUG_UART_CLK_FREQ CLOCK_GetCoreSysClkFreq()//CLOCK_GetBusClkFreq()//
#define BOARD_UART_IRQ UART4_RX_TX_IRQn
#define BOARD_UART_IRQ_HANDLER UART4_RX_TX_IRQHandler
#endif
#ifndef BOARD_DEBUG_UART_BAUDRATE
#define BOARD_DEBUG_UART_BAUDRATE 518400//115200
#endif /* BOARD_DEBUG_UART_BAUDRATE */
/* Board led color mapping */

#define LED_RUN_INIT(output)                                                 \
    GPIO_WritePinOutput(BOARD_INITLEDS_LED_RUN_GPIO, BOARD_INITLEDS_LED_RUN_GPIO_PIN, output); \
    BOARD_INITLEDS_LED_RUN_GPIO->PDDR |= (1U << BOARD_INITLEDS_LED_RUN_GPIO_PIN) /*!< Enable target LED_RUN */
#define LED_RUN_ON() \
    GPIO_ClearPinsOutput(BOARD_INITLEDS_LED_RUN_GPIO, 1U << BOARD_INITLEDS_LED_RUN_GPIO_PIN) /*!< Turn on target LED_RUN */
#define LED_RUN_OFF() \
    GPIO_SetPinsOutput(BOARD_INITLEDS_LED_RUN_GPIO, 1U << BOARD_INITLEDS_LED_RUN_GPIO_PIN) /*!< Turn off target LED_RUN */
#define LED_RUN_TOGGLE() \
    GPIO_TogglePinsOutput(BOARD_INITLEDS_LED_RUN_GPIO, 1U << BOARD_INITLEDS_LED_RUN_GPIO_PIN) /*!< Toggle on target LED_RUN */

      
#define BUTTON_RESET_READ() \
	GPIO_ReadPinInput(BOARD_INITBUTTONS_BUTTON_RESET_GPIO, BOARD_INITBUTTONS_BUTTON_RESET_GPIO_PIN)
          
/* Flash DSPI configuration */

#define FLASH_SPI_MASTER SPI2
#define FLASH_SPI_MASTER_BAUDRATE 500000U
#define FLASH_SPI_MASTER_SOURCE_CLOCK DSPI2_CLK_SRC
#define FLASH_SPI_MASTER_INT_GPIO GPIOD
#define FLASH_SPI_MASTER_INT_PORT PORTD
#define FLASH_SPI_MASTER_INT_PIN 11U
#define FLASH_SPI_MASTER_IRQ PORTB_IRQn
#define FLASH_SPI_MASTER_IRQ_HANDLER PORTB_IRQHandler
/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/* Initialize debug console. */
void BOARD_InitDebugConsole(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_H_ */
