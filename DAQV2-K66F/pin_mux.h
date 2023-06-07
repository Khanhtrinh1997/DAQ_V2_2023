/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_


/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Direction type  */
typedef enum _pin_mux_direction
{
  kPIN_MUX_DirectionInput = 0U,         /* Input direction */
  kPIN_MUX_DirectionOutput = 1U,        /* Output direction */
  kPIN_MUX_DirectionInputOrOutput = 2U  /* Input or output direction */
} pin_mux_direction_t;

/*!
 * @addtogroup pin_mux
 * @{
 */

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void);

/* PORTD11 (number 140), SPI2_CS */
#define BOARD_INITSPIS_SPI2_CS_PERIPHERAL                                   SPI2   /*!< Device name: SPI2 */
#define BOARD_INITSPIS_SPI2_CS_SIGNAL                                    PCS0_SS   /*!< SPI2 signal: PCS0_SS */
#define BOARD_INITSPIS_SPI2_CS_PIN_NAME                                SPI2_PCS0   /*!< Pin name */
#define BOARD_INITSPIS_SPI2_CS_LABEL                                   "SPI2_CS"   /*!< Label */
#define BOARD_INITSPIS_SPI2_CS_NAME                                    "SPI2_CS"   /*!< Identifier name */
#define BOARD_INITSPIS_SPI2_CS_DIRECTION                kPIN_MUX_DirectionOutput   /*!< Direction */

/* PORTD12 (number 141), SPI2_SCK */
#define BOARD_INITSPIS_SPI2_SCK_PERIPHERAL                                  SPI2   /*!< Device name: SPI2 */
#define BOARD_INITSPIS_SPI2_SCK_SIGNAL                                       SCK   /*!< SPI2 signal: SCK */
#define BOARD_INITSPIS_SPI2_SCK_PIN_NAME                                SPI2_SCK   /*!< Pin name */
#define BOARD_INITSPIS_SPI2_SCK_LABEL                                 "SPI2_SCK"   /*!< Label */
#define BOARD_INITSPIS_SPI2_SCK_NAME                                  "SPI2_SCK"   /*!< Identifier name */
#define BOARD_INITSPIS_SPI2_SCK_DIRECTION               kPIN_MUX_DirectionOutput   /*!< Direction */

/* PORTD14 (number 143), SPI2_MISO */
#define BOARD_INITSPIS_SPI2_MISO_PERIPHERAL                                 SPI2   /*!< Device name: SPI2 */
#define BOARD_INITSPIS_SPI2_MISO_SIGNAL                                      SIN   /*!< SPI2 signal: SIN */
#define BOARD_INITSPIS_SPI2_MISO_PIN_NAME                               SPI2_SIN   /*!< Pin name */
#define BOARD_INITSPIS_SPI2_MISO_LABEL                               "SPI2_MISO"   /*!< Label */
#define BOARD_INITSPIS_SPI2_MISO_NAME                                "SPI2_MISO"   /*!< Identifier name */

/* PORTD13 (number 142), SPI2_MOSI */
#define BOARD_INITSPIS_SPI2_MOSI_PERIPHERAL                                 SPI2   /*!< Device name: SPI2 */
#define BOARD_INITSPIS_SPI2_MOSI_SIGNAL                                     SOUT   /*!< SPI2 signal: SOUT */
#define BOARD_INITSPIS_SPI2_MOSI_PIN_NAME                              SPI2_SOUT   /*!< Pin name */
#define BOARD_INITSPIS_SPI2_MOSI_LABEL                               "SPI2_MOSI"   /*!< Label */
#define BOARD_INITSPIS_SPI2_MOSI_NAME                                "SPI2_MOSI"   /*!< Identifier name */

/* PORTD10 (number 139), SPI2_WP */
#define BOARD_INITSPIS_SPI2_WP_GPIO                                        GPIOD   /*!< GPIO device name: GPIOD */
#define BOARD_INITSPIS_SPI2_WP_PORT                                        PORTD   /*!< PORT device name: PORTD */
#define BOARD_INITSPIS_SPI2_WP_GPIO_PIN                                      10U   /*!< PORTD pin index: 10 */
#define BOARD_INITSPIS_SPI2_WP_PIN_NAME                                    PTD10   /*!< Pin name */
#define BOARD_INITSPIS_SPI2_WP_LABEL                                   "SPI2_WP"   /*!< Label */
#define BOARD_INITSPIS_SPI2_WP_NAME                                    "SPI2_WP"   /*!< Identifier name */
#define BOARD_INITSPIS_SPI2_WP_DIRECTION                kPIN_MUX_DirectionOutput   /*!< Direction */

/* PORTD15 (number 144), SPI2_HOLD */
#define BOARD_INITSPIS_SPI2_HOLD_GPIO                                      GPIOD   /*!< GPIO device name: GPIOD */
#define BOARD_INITSPIS_SPI2_HOLD_PORT                                      PORTD   /*!< PORT device name: PORTD */
#define BOARD_INITSPIS_SPI2_HOLD_GPIO_PIN                                    15U   /*!< PORTD pin index: 15 */
#define BOARD_INITSPIS_SPI2_HOLD_PIN_NAME                                  PTD15   /*!< Pin name */
#define BOARD_INITSPIS_SPI2_HOLD_LABEL                               "SPI2_HOLD"   /*!< Label */
#define BOARD_INITSPIS_SPI2_HOLD_NAME                                "SPI2_HOLD"   /*!< Identifier name */
#define BOARD_INITSPIS_SPI2_HOLD_DIRECTION              kPIN_MUX_DirectionOutput   /*!< Direction */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitSPIs(void);

/* PORTA11 (number 63), LED_RUN */
#define BOARD_INITLEDS_LED_RUN_GPIO                                        GPIOA   /*!< GPIO device name: GPIOA */
#define BOARD_INITLEDS_LED_RUN_PORT                                        PORTA   /*!< PORT device name: PORTA */
#define BOARD_INITLEDS_LED_RUN_GPIO_PIN                                      11U   /*!< PORTA pin index: 11 */
#define BOARD_INITLEDS_LED_RUN_PIN_NAME                                    PTA11   /*!< Pin name */
#define BOARD_INITLEDS_LED_RUN_LABEL                                   "LED_RUN"   /*!< Label */
#define BOARD_INITLEDS_LED_RUN_NAME                                    "LED_RUN"   /*!< Identifier name */
#define BOARD_INITLEDS_LED_RUN_DIRECTION                kPIN_MUX_DirectionOutput   /*!< Direction */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitLEDs(void);

/* PORTE26 (number 47), ENET_1588_CLKIN */
#define BOARD_INITENET_ENET_1588_CLKIN_PERIPHERAL                           ENET   /*!< Device name: ENET */
#define BOARD_INITENET_ENET_1588_CLKIN_SIGNAL                         CLKIN_1588   /*!< ENET signal: CLKIN_1588 */
#define BOARD_INITENET_ENET_1588_CLKIN_PIN_NAME                  ENET_1588_CLKIN   /*!< Pin name */
#define BOARD_INITENET_ENET_1588_CLKIN_LABEL                   "ENET_1588_CLKIN"   /*!< Label */
#define BOARD_INITENET_ENET_1588_CLKIN_NAME                    "ENET_1588_CLKIN"   /*!< Identifier name */

/* PORTA8 (number 60), RMII0_MDC */
#define BOARD_INITENET_RMII0_MDC_PERIPHERAL                                 ENET   /*!< Device name: ENET */
#define BOARD_INITENET_RMII0_MDC_SIGNAL                                 RMII_MDC   /*!< ENET signal: RMII_MDC */
#define BOARD_INITENET_RMII0_MDC_PIN_NAME                              RMII0_MDC   /*!< Pin name */
#define BOARD_INITENET_RMII0_MDC_LABEL                               "RMII0_MDC"   /*!< Label */
#define BOARD_INITENET_RMII0_MDC_NAME                                "RMII0_MDC"   /*!< Identifier name */

/* PORTA7 (number 59), RMII0_MDIO */
#define BOARD_INITENET_RMII0_MDIO_PERIPHERAL                                ENET   /*!< Device name: ENET */
#define BOARD_INITENET_RMII0_MDIO_SIGNAL                               RMII_MDIO   /*!< ENET signal: RMII_MDIO */
#define BOARD_INITENET_RMII0_MDIO_PIN_NAME                            RMII0_MDIO   /*!< Pin name */
#define BOARD_INITENET_RMII0_MDIO_LABEL                             "RMII0_MDIO"   /*!< Label */
#define BOARD_INITENET_RMII0_MDIO_NAME                              "RMII0_MDIO"   /*!< Identifier name */

/* PORTA5 (number 55), RMII0_RXER */
#define BOARD_INITENET_RMII0_RXER_PERIPHERAL                                ENET   /*!< Device name: ENET */
#define BOARD_INITENET_RMII0_RXER_SIGNAL                               RMII_RXER   /*!< ENET signal: RMII_RXER */
#define BOARD_INITENET_RMII0_RXER_PIN_NAME                            RMII0_RXER   /*!< Pin name */
#define BOARD_INITENET_RMII0_RXER_LABEL                             "RMII0_RXER"   /*!< Label */
#define BOARD_INITENET_RMII0_RXER_NAME                              "RMII0_RXER"   /*!< Identifier name */

/* PORTA13 (number 65), RMII0_RXD0 */
#define BOARD_INITENET_RMII0_RXD0_PERIPHERAL                                ENET   /*!< Device name: ENET */
#define BOARD_INITENET_RMII0_RXD0_SIGNAL                               RMII_RXD0   /*!< ENET signal: RMII_RXD0 */
#define BOARD_INITENET_RMII0_RXD0_PIN_NAME                            RMII0_RXD0   /*!< Pin name */
#define BOARD_INITENET_RMII0_RXD0_LABEL                             "RMII0_RXD0"   /*!< Label */
#define BOARD_INITENET_RMII0_RXD0_NAME                              "RMII0_RXD0"   /*!< Identifier name */

/* PORTA17 (number 69), RMII0_TXD1 */
#define BOARD_INITENET_RMII0_TXD1_PERIPHERAL                                ENET   /*!< Device name: ENET */
#define BOARD_INITENET_RMII0_TXD1_SIGNAL                               RMII_TXD1   /*!< ENET signal: RMII_TXD1 */
#define BOARD_INITENET_RMII0_TXD1_PIN_NAME                            RMII0_TXD1   /*!< Pin name */
#define BOARD_INITENET_RMII0_TXD1_LABEL                             "RMII0_TXD1"   /*!< Label */
#define BOARD_INITENET_RMII0_TXD1_NAME                              "RMII0_TXD1"   /*!< Identifier name */

/* PORTA15 (number 67), RMII0_TXEN */
#define BOARD_INITENET_RMII0_TXEN_PERIPHERAL                                ENET   /*!< Device name: ENET */
#define BOARD_INITENET_RMII0_TXEN_SIGNAL                               RMII_TXEN   /*!< ENET signal: RMII_TXEN */
#define BOARD_INITENET_RMII0_TXEN_PIN_NAME                            RMII0_TXEN   /*!< Pin name */
#define BOARD_INITENET_RMII0_TXEN_LABEL                             "RMII0_TXEN"   /*!< Label */
#define BOARD_INITENET_RMII0_TXEN_NAME                              "RMII0_TXEN"   /*!< Identifier name */

/* PORTA12 (number 64), RMII0_RXD1 */
#define BOARD_INITENET_RMII0_RXD1_PERIPHERAL                                ENET   /*!< Device name: ENET */
#define BOARD_INITENET_RMII0_RXD1_SIGNAL                               RMII_RXD1   /*!< ENET signal: RMII_RXD1 */
#define BOARD_INITENET_RMII0_RXD1_PIN_NAME                            RMII0_RXD1   /*!< Pin name */
#define BOARD_INITENET_RMII0_RXD1_LABEL                             "RMII0_RXD1"   /*!< Label */
#define BOARD_INITENET_RMII0_RXD1_NAME                              "RMII0_RXD1"   /*!< Identifier name */

/* PORTA16 (number 68), RMII0_TXD0 */
#define BOARD_INITENET_RMII0_TXD0_PERIPHERAL                                ENET   /*!< Device name: ENET */
#define BOARD_INITENET_RMII0_TXD0_SIGNAL                               RMII_TXD0   /*!< ENET signal: RMII_TXD0 */
#define BOARD_INITENET_RMII0_TXD0_PIN_NAME                            RMII0_TXD0   /*!< Pin name */
#define BOARD_INITENET_RMII0_TXD0_LABEL                             "RMII0_TXD0"   /*!< Label */
#define BOARD_INITENET_RMII0_TXD0_NAME                              "RMII0_TXD0"   /*!< Identifier name */

/* PORTA14 (number 66), RMII0_CRS_DV */
#define BOARD_INITENET_RMII0_CRS_DV_PERIPHERAL                              ENET   /*!< Device name: ENET */
#define BOARD_INITENET_RMII0_CRS_DV_SIGNAL                           RMII_CRS_DV   /*!< ENET signal: RMII_CRS_DV */
#define BOARD_INITENET_RMII0_CRS_DV_PIN_NAME                        RMII0_CRS_DV   /*!< Pin name */
#define BOARD_INITENET_RMII0_CRS_DV_LABEL                         "RMII0_CRS_DV"   /*!< Label */
#define BOARD_INITENET_RMII0_CRS_DV_NAME                          "RMII0_CRS_DV"   /*!< Identifier name */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitENET(void);

/* PORTA10 (number 62), BUTTON_RESET */
#define BOARD_INITBUTTONS_BUTTON_RESET_GPIO                                GPIOA   /*!< GPIO device name: GPIOA */
#define BOARD_INITBUTTONS_BUTTON_RESET_PORT                                PORTA   /*!< PORT device name: PORTA */
#define BOARD_INITBUTTONS_BUTTON_RESET_GPIO_PIN                              10U   /*!< PORTA pin index: 10 */
#define BOARD_INITBUTTONS_BUTTON_RESET_PIN_NAME                            PTA10   /*!< Pin name */
#define BOARD_INITBUTTONS_BUTTON_RESET_LABEL                      "BUTTON_RESET"   /*!< Label */
#define BOARD_INITBUTTONS_BUTTON_RESET_NAME                       "BUTTON_RESET"   /*!< Identifier name */
#define BOARD_INITBUTTONS_BUTTON_RESET_DIRECTION         kPIN_MUX_DirectionInput   /*!< Direction */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitBUTTONs(void);

/* PORTD8 (number 137), SCL */
#define BOARD_INITI2C_SCL_PERIPHERAL                                        I2C0   /*!< Device name: I2C0 */
#define BOARD_INITI2C_SCL_SIGNAL                                             SCL   /*!< I2C0 signal: SCL */
#define BOARD_INITI2C_SCL_PIN_NAME                                      I2C0_SCL   /*!< Pin name */
#define BOARD_INITI2C_SCL_LABEL                                            "SCL"   /*!< Label */
#define BOARD_INITI2C_SCL_NAME                                             "SCL"   /*!< Identifier name */

/* PORTD9 (number 138), SDA */
#define BOARD_INITI2C_SDA_PERIPHERAL                                        I2C0   /*!< Device name: I2C0 */
#define BOARD_INITI2C_SDA_SIGNAL                                             SDA   /*!< I2C0 signal: SDA */
#define BOARD_INITI2C_SDA_PIN_NAME                                      I2C0_SDA   /*!< Pin name */
#define BOARD_INITI2C_SDA_LABEL                                            "SDA"   /*!< Label */
#define BOARD_INITI2C_SDA_NAME                                             "SDA"   /*!< Identifier name */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitI2C(void);

/* PORTE25 (number 46), RS232_RX */
#define BOARD_INITUARTS_RS232_RX_PERIPHERAL                                UART4   /*!< Device name: UART4 */
#define BOARD_INITUARTS_RS232_RX_SIGNAL                                       RX   /*!< UART4 signal: RX */
#define BOARD_INITUARTS_RS232_RX_PIN_NAME                               UART4_RX   /*!< Pin name */
#define BOARD_INITUARTS_RS232_RX_LABEL                                "RS232_RX"   /*!< Label */
#define BOARD_INITUARTS_RS232_RX_NAME                                 "RS232_RX"   /*!< Identifier name */

/* PORTE24 (number 45), RS232_TX */
#define BOARD_INITUARTS_RS232_TX_PERIPHERAL                                UART4   /*!< Device name: UART4 */
#define BOARD_INITUARTS_RS232_TX_SIGNAL                                       TX   /*!< UART4 signal: TX */
#define BOARD_INITUARTS_RS232_TX_PIN_NAME                               UART4_TX   /*!< Pin name */
#define BOARD_INITUARTS_RS232_TX_LABEL                                "RS232_TX"   /*!< Label */
#define BOARD_INITUARTS_RS232_TX_NAME                                 "RS232_TX"   /*!< Identifier name */
#define BOARD_INITUARTS_RS232_TX_DIRECTION              kPIN_MUX_DirectionOutput   /*!< Direction */

/* PORTE1 (number 2), RS232_RX2 */
#define BOARD_INITUARTS_RX2_PERIPHERAL                                     UART1   /*!< Device name: UART1 */
#define BOARD_INITUARTS_RX2_SIGNAL                                            RX   /*!< UART1 signal: RX */
#define BOARD_INITUARTS_RX2_PIN_NAME                                    UART1_RX   /*!< Pin name */
#define BOARD_INITUARTS_RX2_LABEL                                    "RS232_RX2"   /*!< Label */
#define BOARD_INITUARTS_RX2_NAME                                           "RX2"   /*!< Identifier name */

/* PORTE0 (number 1), RS232_TX2 */
#define BOARD_INITUARTS_TX2_PERIPHERAL                                     UART1   /*!< Device name: UART1 */
#define BOARD_INITUARTS_TX2_SIGNAL                                            TX   /*!< UART1 signal: TX */
#define BOARD_INITUARTS_TX2_PIN_NAME                                    UART1_TX   /*!< Pin name */
#define BOARD_INITUARTS_TX2_LABEL                                    "RS232_TX2"   /*!< Label */
#define BOARD_INITUARTS_TX2_NAME                                           "TX2"   /*!< Identifier name */
#define BOARD_INITUARTS_TX2_DIRECTION                   kPIN_MUX_DirectionOutput   /*!< Direction */

/* PORTC17 (number 124), RS485_TX */
#define BOARD_INITUARTS_RS485_TX_PERIPHERAL                                UART3   /*!< Device name: UART3 */
#define BOARD_INITUARTS_RS485_TX_SIGNAL                                       TX   /*!< UART3 signal: TX */
#define BOARD_INITUARTS_RS485_TX_PIN_NAME                               UART3_TX   /*!< Pin name */
#define BOARD_INITUARTS_RS485_TX_LABEL                                "RS485_TX"   /*!< Label */
#define BOARD_INITUARTS_RS485_TX_NAME                                 "RS485_TX"   /*!< Identifier name */
#define BOARD_INITUARTS_RS485_TX_DIRECTION              kPIN_MUX_DirectionOutput   /*!< Direction */

/* PORTD6 (number 133), RS485_RX */
#define BOARD_INITUARTS_RS485_RX_PERIPHERAL                                UART0   /*!< Device name: UART0 */
#define BOARD_INITUARTS_RS485_RX_SIGNAL                                       RX   /*!< UART0 signal: RX */
#define BOARD_INITUARTS_RS485_RX_PIN_NAME                               UART0_RX   /*!< Pin name */
#define BOARD_INITUARTS_RS485_RX_LABEL                                "RS485_RX"   /*!< Label */
#define BOARD_INITUARTS_RS485_RX_NAME                                 "RS485_RX"   /*!< Identifier name */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitUARTs(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitSDRAM(void);

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/*******************************************************************************
 * EOF
 ******************************************************************************/

