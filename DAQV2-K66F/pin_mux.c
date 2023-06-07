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

/*
 * TEXT BELOW IS USED AS SETTING FOR THE PINS TOOL *****************************
PinsProfile:
- !!product 'Pins v2.0'
- !!processor 'MK66FN2M0xxx18'
- !!package 'MK66FN2M0VLQ18'
- !!mcu_data 'ksdk2_0'
- !!processor_version '1.0.1'
- pin_labels:
  - {pin_num: '140', pin_signal: PTD11/LLWU_P25/SPI2_PCS0/SDHC0_CLKIN/LPUART0_CTS_b/FB_A19, label: SPI2_CS, identifier: SPI2_CS}
  - {pin_num: '141', pin_signal: PTD12/SPI2_SCK/FTM3_FLT0/SDHC0_D4/FB_A20, label: SPI2_SCK, identifier: SPI2_SCK}
  - {pin_num: '142', pin_signal: PTD13/SPI2_SOUT/SDHC0_D5/FB_A21, label: SPI2_MOSI, identifier: SPI2_MOSI}
  - {pin_num: '143', pin_signal: PTD14/SPI2_SIN/SDHC0_D6/FB_A22, label: SPI2_MISO, identifier: SPI2_MISO}
  - {pin_num: '59', pin_signal: ADC0_SE10/PTA7/FTM0_CH4/RMII0_MDIO/MII0_MDIO/TRACE_D3, label: RMII0_MDIO, identifier: RMII0_MDIO}
  - {pin_num: '60', pin_signal: ADC0_SE11/PTA8/FTM1_CH0/RMII0_MDC/MII0_MDC/FTM1_QD_PHA/TPM1_CH0/TRACE_D2, label: RMII0_MDC, identifier: RMII0_MDC}
  - {pin_num: '64', pin_signal: CMP2_IN0/PTA12/CAN0_TX/FTM1_CH0/RMII0_RXD1/MII0_RXD1/I2C2_SCL/I2S0_TXD0/FTM1_QD_PHA/TPM1_CH0, label: RMII0_RXD1, identifier: RMII0_RXD1}
  - {pin_num: '65', pin_signal: CMP2_IN1/PTA13/LLWU_P4/CAN0_RX/FTM1_CH1/RMII0_RXD0/MII0_RXD0/I2C2_SDA/I2S0_TX_FS/FTM1_QD_PHB/TPM1_CH1, label: RMII0_RXD0, identifier: RMII0_RXD0}
  - {pin_num: '66', pin_signal: PTA14/SPI0_PCS0/UART0_TX/RMII0_CRS_DV/MII0_RXDV/I2C2_SCL/I2S0_RX_BCLK/I2S0_TXD1, label: RMII0_CRS_DV, identifier: RMII0_CRS_DV}
  - {pin_num: '67', pin_signal: CMP3_IN1/PTA15/SPI0_SCK/UART0_RX/RMII0_TXEN/MII0_TXEN/I2S0_RXD0, label: RMII0_TXEN, identifier: RMII0_TXEN}
  - {pin_num: '68', pin_signal: CMP3_IN2/PTA16/SPI0_SOUT/UART0_CTS_b/UART0_COL_b/RMII0_TXD0/MII0_TXD0/I2S0_RX_FS/I2S0_RXD1, label: RMII0_TXD0, identifier: RMII0_TXD0}
  - {pin_num: '69', pin_signal: ADC1_SE17/PTA17/SPI0_SIN/UART0_RTS_b/RMII0_TXD1/MII0_TXD1/I2S0_MCLK, label: RMII0_TXD1, identifier: RMII0_TXD1}
  - {pin_num: '55', pin_signal: PTA5/USB0_CLKIN/FTM0_CH2/RMII0_RXER/MII0_RXER/CMP2_OUT/I2S0_TX_BCLK/JTAG_TRST_b, label: RMII0_RXER, identifier: RMII0_RXER}
  - {pin_num: '47', pin_signal: PTE26/ENET_1588_CLKIN/UART4_CTS_b/RTC_CLKOUT/USB0_CLKIN, label: ENET_1588_CLKIN, identifier: ENET_1588_CLKIN}
  - {pin_num: '137', pin_signal: PTD8/LLWU_P24/I2C0_SCL/LPUART0_RX/FB_A16, label: SCL, identifier: SCL}
  - {pin_num: '138', pin_signal: PTD9/I2C0_SDA/LPUART0_TX/FB_A17, label: SDA, identifier: SDA}
  - {pin_num: '1', pin_signal: ADC1_SE4a/PTE0/SPI1_PCS1/UART1_TX/SDHC0_D1/TRACE_CLKOUT/I2C1_SDA/RTC_CLKOUT, label: RS232_TX2, identifier: TX2}
  - {pin_num: '2', pin_signal: ADC1_SE5a/PTE1/LLWU_P0/SPI1_SOUT/UART1_RX/SDHC0_D0/TRACE_D3/I2C1_SCL/SPI1_SIN, label: RS232_RX2, identifier: RX2}
  - {pin_num: '63', pin_signal: PTA11/LLWU_P23/FTM2_CH1/MII0_RXCLK/I2C2_SDA/FTM2_QD_PHB/TPM2_CH1, label: LED_RUN, identifier: LED_RUN}
  - {pin_num: '62', pin_signal: PTA10/LLWU_P22/FTM2_CH0/MII0_RXD2/FTM2_QD_PHA/TPM2_CH0/TRACE_D0, label: BUTTON_RESET, identifier: BUTTON_RESET}
  - {pin_num: '46', pin_signal: ADC0_SE18/PTE25/LLWU_P21/CAN1_RX/UART4_RX/I2C0_SDA/EWM_IN, label: RS232_RX, identifier: RS232_RX}
  - {pin_num: '45', pin_signal: ADC0_SE17/PTE24/CAN1_TX/UART4_TX/I2C0_SCL/EWM_OUT_b, label: RS232_TX, identifier: RS232_TX}
  - {pin_num: '133', pin_signal: ADC0_SE7b/PTD6/LLWU_P15/SPI0_PCS3/UART0_RX/FTM0_CH6/FB_AD0/FTM0_FLT0/SPI1_SOUT, label: RS485_RX, identifier: RS485_RX}
  - {pin_num: '124', pin_signal: PTC17/CAN1_TX/UART3_TX/ENET0_1588_TMR1/FB_CS4_b/FB_TSIZ0/FB_BE31_24_BLS7_0_b/SDRAM_DQM3, label: RS485_TX, identifier: RS485_TX}
  - {pin_num: '58', pin_signal: PTA6/FTM0_CH3/CLKOUT/TRACE_CLKOUT, label: SDRAM_CLKOUT}
  - {pin_num: '139', pin_signal: PTD10/LPUART0_RTS_b/FB_A18, label: SPI2_WP, identifier: SPI2_WP}
  - {pin_num: '144', pin_signal: PTD15/SPI2_PCS1/SDHC0_D7/FB_A23, label: SPI2_HOLD, identifier: SPI2_HOLD}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR THE PINS TOOL ***
 */

#include "fsl_common.h"
#include "fsl_port.h"
#include "pin_mux.h"

/*
 * TEXT BELOW IS USED AS SETTING FOR THE PINS TOOL *****************************
BOARD_InitPins:
- options: {coreID: singlecore, enableClock: 'true'}
- pin_list: []
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR THE PINS TOOL ***
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitPins(void) {
}


#define PIN10_IDX                       10u   /*!< Pin number for pin 10 in a port */
#define PIN11_IDX                       11u   /*!< Pin number for pin 11 in a port */
#define PIN12_IDX                       12u   /*!< Pin number for pin 12 in a port */
#define PIN13_IDX                       13u   /*!< Pin number for pin 13 in a port */
#define PIN14_IDX                       14u   /*!< Pin number for pin 14 in a port */
#define PIN15_IDX                       15u   /*!< Pin number for pin 15 in a port */

/*
 * TEXT BELOW IS USED AS SETTING FOR THE PINS TOOL *****************************
BOARD_InitSPIs:
- options: {coreID: singlecore, enableClock: 'true'}
- pin_list:
  - {pin_num: '140', peripheral: SPI2, signal: PCS0_SS, pin_signal: PTD11/LLWU_P25/SPI2_PCS0/SDHC0_CLKIN/LPUART0_CTS_b/FB_A19, direction: OUTPUT}
  - {pin_num: '141', peripheral: SPI2, signal: SCK, pin_signal: PTD12/SPI2_SCK/FTM3_FLT0/SDHC0_D4/FB_A20, direction: OUTPUT}
  - {pin_num: '143', peripheral: SPI2, signal: SIN, pin_signal: PTD14/SPI2_SIN/SDHC0_D6/FB_A22}
  - {pin_num: '142', peripheral: SPI2, signal: SOUT, pin_signal: PTD13/SPI2_SOUT/SDHC0_D5/FB_A21}
  - {pin_num: '139', peripheral: GPIOD, signal: 'GPIO, 10', pin_signal: PTD10/LPUART0_RTS_b/FB_A18, direction: OUTPUT}
  - {pin_num: '144', peripheral: GPIOD, signal: 'GPIO, 15', pin_signal: PTD15/SPI2_PCS1/SDHC0_D7/FB_A23, direction: OUTPUT}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR THE PINS TOOL ***
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitSPIs
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitSPIs(void) {
  CLOCK_EnableClock(kCLOCK_PortD);                           /* Port D Clock Gate Control: Clock enabled */

  PORT_SetPinMux(PORTD, PIN10_IDX, kPORT_MuxAsGpio);         /* PORTD10 (pin 139) is configured as PTD10 */
  PORT_SetPinMux(PORTD, PIN11_IDX, kPORT_MuxAsGpio);          /* PORTD11 (pin 140) is configured as SPI2_PCS0 */
  PORT_SetPinMux(PORTD, PIN12_IDX, kPORT_MuxAlt2);           /* PORTD12 (pin 141) is configured as SPI2_SCK */
  PORT_SetPinMux(PORTD, PIN13_IDX, kPORT_MuxAlt2);           /* PORTD13 (pin 142) is configured as SPI2_SOUT */
  PORT_SetPinMux(PORTD, PIN14_IDX, kPORT_MuxAlt2);           /* PORTD14 (pin 143) is configured as SPI2_SIN */
  PORT_SetPinMux(PORTD, PIN15_IDX, kPORT_MuxAsGpio);         /* PORTD15 (pin 144) is configured as PTD15 */
}


#define PIN11_IDX                       11u   /*!< Pin number for pin 11 in a port */
/*
 * TEXT BELOW IS USED AS SETTING FOR THE PINS TOOL *****************************
BOARD_InitLEDs:
- options: {coreID: singlecore, enableClock: 'true'}
- pin_list:
  - {pin_num: '63', peripheral: GPIOA, signal: 'GPIO, 11', pin_signal: PTA11/LLWU_P23/FTM2_CH1/MII0_RXCLK/I2C2_SDA/FTM2_QD_PHB/TPM2_CH1, direction: OUTPUT}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR THE PINS TOOL ***
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitLEDs
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitLEDs(void) {
  CLOCK_EnableClock(kCLOCK_PortA);                           /* Port A Clock Gate Control: Clock enabled */
CLOCK_EnableClock(kCLOCK_PortE);                           /* Port A Clock Gate Control: Clock enabled */
  PORT_SetPinMux(PORTA, PIN11_IDX, kPORT_MuxAsGpio);         /* PORTA11 (pin 63) is configured as PTA11 */
  PORT_SetPinMux(PORTE, PIN11_IDX, kPORT_MuxAsGpio);         /* PORTA11 (pin 63) is configured as PTA11 */
}


#define PIN5_IDX                         5u   /*!< Pin number for pin 5 in a port */
#define PIN7_IDX                         7u   /*!< Pin number for pin 7 in a port */
#define PIN8_IDX                         8u   /*!< Pin number for pin 8 in a port */
#define PIN12_IDX                       12u   /*!< Pin number for pin 12 in a port */
#define PIN13_IDX                       13u   /*!< Pin number for pin 13 in a port */
#define PIN14_IDX                       14u   /*!< Pin number for pin 14 in a port */
#define PIN15_IDX                       15u   /*!< Pin number for pin 15 in a port */
#define PIN16_IDX                       16u   /*!< Pin number for pin 16 in a port */
#define PIN17_IDX                       17u   /*!< Pin number for pin 17 in a port */
#define PIN26_IDX                       26u   /*!< Pin number for pin 26 in a port */
#define SOPT2_RMIISRC_ENET            0x01u   /*!< RMII clock source select: External bypass clock (ENET_1588_CLKIN). */

/*
 * TEXT BELOW IS USED AS SETTING FOR THE PINS TOOL *****************************
BOARD_InitENET:
- options: {coreID: singlecore, enableClock: 'true'}
- pin_list:
  - {pin_num: '47', peripheral: ENET, signal: CLKIN_1588, pin_signal: PTE26/ENET_1588_CLKIN/UART4_CTS_b/RTC_CLKOUT/USB0_CLKIN}
  - {pin_num: '47', peripheral: ENET, signal: RMII_CLKIN, pin_signal: PTE26/ENET_1588_CLKIN/UART4_CTS_b/RTC_CLKOUT/USB0_CLKIN, identifier: ''}
  - {pin_num: '60', peripheral: ENET, signal: RMII_MDC, pin_signal: ADC0_SE11/PTA8/FTM1_CH0/RMII0_MDC/MII0_MDC/FTM1_QD_PHA/TPM1_CH0/TRACE_D2}
  - {pin_num: '59', peripheral: ENET, signal: RMII_MDIO, pin_signal: ADC0_SE10/PTA7/FTM0_CH4/RMII0_MDIO/MII0_MDIO/TRACE_D3}
  - {pin_num: '55', peripheral: ENET, signal: RMII_RXER, pin_signal: PTA5/USB0_CLKIN/FTM0_CH2/RMII0_RXER/MII0_RXER/CMP2_OUT/I2S0_TX_BCLK/JTAG_TRST_b}
  - {pin_num: '65', peripheral: ENET, signal: RMII_RXD0, pin_signal: CMP2_IN1/PTA13/LLWU_P4/CAN0_RX/FTM1_CH1/RMII0_RXD0/MII0_RXD0/I2C2_SDA/I2S0_TX_FS/FTM1_QD_PHB/TPM1_CH1}
  - {pin_num: '69', peripheral: ENET, signal: RMII_TXD1, pin_signal: ADC1_SE17/PTA17/SPI0_SIN/UART0_RTS_b/RMII0_TXD1/MII0_TXD1/I2S0_MCLK}
  - {pin_num: '67', peripheral: ENET, signal: RMII_TXEN, pin_signal: CMP3_IN1/PTA15/SPI0_SCK/UART0_RX/RMII0_TXEN/MII0_TXEN/I2S0_RXD0}
  - {pin_num: '64', peripheral: ENET, signal: RMII_RXD1, pin_signal: CMP2_IN0/PTA12/CAN0_TX/FTM1_CH0/RMII0_RXD1/MII0_RXD1/I2C2_SCL/I2S0_TXD0/FTM1_QD_PHA/TPM1_CH0}
  - {pin_num: '68', peripheral: ENET, signal: RMII_TXD0, pin_signal: CMP3_IN2/PTA16/SPI0_SOUT/UART0_CTS_b/UART0_COL_b/RMII0_TXD0/MII0_TXD0/I2S0_RX_FS/I2S0_RXD1}
  - {pin_num: '66', peripheral: ENET, signal: RMII_CRS_DV, pin_signal: PTA14/SPI0_PCS0/UART0_TX/RMII0_CRS_DV/MII0_RXDV/I2C2_SCL/I2S0_RX_BCLK/I2S0_TXD1}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR THE PINS TOOL ***
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitENET
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitENET(void) {
  CLOCK_EnableClock(kCLOCK_PortA);                           /* Port A Clock Gate Control: Clock enabled */
  CLOCK_EnableClock(kCLOCK_PortE);                           /* Port E Clock Gate Control: Clock enabled */

  PORT_SetPinMux(PORTA, PIN12_IDX, kPORT_MuxAlt4);           /* PORTA12 (pin 64) is configured as RMII0_RXD1 */
  PORT_SetPinMux(PORTA, PIN13_IDX, kPORT_MuxAlt4);           /* PORTA13 (pin 65) is configured as RMII0_RXD0 */
  PORT_SetPinMux(PORTA, PIN14_IDX, kPORT_MuxAlt4);           /* PORTA14 (pin 66) is configured as RMII0_CRS_DV */
  PORT_SetPinMux(PORTA, PIN15_IDX, kPORT_MuxAlt4);           /* PORTA15 (pin 67) is configured as RMII0_TXEN */
  PORT_SetPinMux(PORTA, PIN16_IDX, kPORT_MuxAlt4);           /* PORTA16 (pin 68) is configured as RMII0_TXD0 */
  PORT_SetPinMux(PORTA, PIN17_IDX, kPORT_MuxAlt4);           /* PORTA17 (pin 69) is configured as RMII0_TXD1 */
  PORT_SetPinMux(PORTA, PIN5_IDX, kPORT_MuxAlt4);            /* PORTA5 (pin 55) is configured as RMII0_RXER */
  PORT_SetPinMux(PORTA, PIN7_IDX, kPORT_MuxAlt5);            /* PORTA7 (pin 59) is configured as RMII0_MDIO */
  PORT_SetPinMux(PORTA, PIN8_IDX, kPORT_MuxAlt5);            /* PORTA8 (pin 60) is configured as RMII0_MDC */
  PORT_SetPinMux(PORTE, PIN26_IDX, kPORT_MuxAlt2);           /* PORTE26 (pin 47) is configured as ENET_1588_CLKIN */
  SIM->SOPT2 = ((SIM->SOPT2 &
    (~(SIM_SOPT2_RMIISRC_MASK)))                             /* Mask bits to zero which are setting */
      | SIM_SOPT2_RMIISRC(SOPT2_RMIISRC_ENET)                /* RMII clock source select: External bypass clock (ENET_1588_CLKIN). */
    );
}


#define PIN10_IDX                       10u   /*!< Pin number for pin 10 in a port */
/*
 * TEXT BELOW IS USED AS SETTING FOR THE PINS TOOL *****************************
BOARD_InitBUTTONs:
- options: {coreID: singlecore, enableClock: 'true'}
- pin_list:
  - {pin_num: '62', peripheral: GPIOA, signal: 'GPIO, 10', pin_signal: PTA10/LLWU_P22/FTM2_CH0/MII0_RXD2/FTM2_QD_PHA/TPM2_CH0/TRACE_D0, direction: INPUT}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR THE PINS TOOL ***
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitBUTTONs
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitBUTTONs(void) {
  CLOCK_EnableClock(kCLOCK_PortA);                           /* Port A Clock Gate Control: Clock enabled */

  PORT_SetPinMux(PORTA, PIN10_IDX, kPORT_MuxAsGpio);         /* PORTA10 (pin 62) is configured as PTA10 */
}


#define PIN8_IDX                         8u   /*!< Pin number for pin 8 in a port */
#define PIN9_IDX                         9u   /*!< Pin number for pin 9 in a port */
#define PORT_DFER_DFE_8_MASK        0x0100u   /*!< Digital Filter Enable Mask for item 8. */
#define PORT_DFER_DFE_9_MASK        0x0200u   /*!< Digital Filter Enable Mask for item 9. */

/*
 * TEXT BELOW IS USED AS SETTING FOR THE PINS TOOL *****************************
BOARD_InitI2C:
- options: {coreID: singlecore, enableClock: 'true'}
- pin_list:
  - {pin_num: '137', peripheral: I2C0, signal: SCL, pin_signal: PTD8/LLWU_P24/I2C0_SCL/LPUART0_RX/FB_A16, slew_rate: fast, open_drain: enable, pull_select: down,
    pull_enable: disable, digital_filter: disable}
  - {pin_num: '138', peripheral: I2C0, signal: SDA, pin_signal: PTD9/I2C0_SDA/LPUART0_TX/FB_A17, slew_rate: fast, open_drain: enable, pull_select: down, pull_enable: disable,
    digital_filter: disable}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR THE PINS TOOL ***
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitI2C
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitI2C(void) {
  CLOCK_EnableClock(kCLOCK_PortD);                           /* Port D Clock Gate Control: Clock enabled */

  PORT_EnablePinsDigitalFilter(                              /* Configure digital filter */
    PORTD,                                                   /* Digital filter is configured on port D */
      PORT_DFER_DFE_8_MASK                                   /* Digital filter is configured for PORTD0 */
    | PORT_DFER_DFE_9_MASK,                                  /* Digital filter is configured for PORTD1 */
    false                                                    /* Disable digital filter */
  );
  const port_pin_config_t portd8_pin137_config = {
    kPORT_PullDisable,                                       /* Internal pull-up/down resistor is disabled */
    kPORT_FastSlewRate,                                      /* Fast slew rate is configured */
    kPORT_PassiveFilterDisable,                              /* Passive filter is disabled */
    kPORT_OpenDrainEnable,                                   /* Open drain is enabled */
    kPORT_LowDriveStrength,                                  /* Low drive strength is configured */
    kPORT_MuxAlt2,                                           /* Pin is configured as I2C0_SCL */
    kPORT_UnlockRegister                                     /* Pin Control Register fields [15:0] are not locked */
  };
  PORT_SetPinConfig(PORTD, PIN8_IDX, &portd8_pin137_config); /* PORTD8 (pin 137) is configured as I2C0_SCL */
  const port_pin_config_t portd9_pin138_config = {
    kPORT_PullDisable,                                       /* Internal pull-up/down resistor is disabled */
    kPORT_FastSlewRate,                                      /* Fast slew rate is configured */
    kPORT_PassiveFilterDisable,                              /* Passive filter is disabled */
    kPORT_OpenDrainEnable,                                   /* Open drain is enabled */
    kPORT_LowDriveStrength,                                  /* Low drive strength is configured */
    kPORT_MuxAlt2,                                           /* Pin is configured as I2C0_SDA */
    kPORT_UnlockRegister                                     /* Pin Control Register fields [15:0] are not locked */
  };
  PORT_SetPinConfig(PORTD, PIN9_IDX, &portd9_pin138_config); /* PORTD9 (pin 138) is configured as I2C0_SDA */
}


#define PIN0_IDX                         0u   /*!< Pin number for pin 0 in a port */
#define PIN1_IDX                         1u   /*!< Pin number for pin 1 in a port */
#define PIN2_IDX                         2u   /*!< Pin number for pin 2 in a port */
#define PIN3_IDX                         3u   /*!< Pin number for pin 3 in a port */
#define PIN4_IDX                         4u   /*!< Pin number for pin 4 in a port */
#define PIN5_IDX                         5u   /*!< Pin number for pin 5 in a port */
#define PIN6_IDX                         6u   /*!< Pin number for pin 6 in a port */
#define PIN17_IDX                       17u   /*!< Pin number for pin 17 in a port */
#define PIN24_IDX                       24u   /*!< Pin number for pin 24 in a port */
#define PIN25_IDX                       25u   /*!< Pin number for pin 25 in a port */
#define SOPT5_UART1TXSRC_UART_TX      0x00u   /*!< UART 1 transmit data source select: UART1_TX pin */

/*
 * TEXT BELOW IS USED AS SETTING FOR THE PINS TOOL *****************************
BOARD_InitUARTs:
- options: {coreID: singlecore, enableClock: 'true'}
- pin_list:
  - {pin_num: '46', peripheral: UART4, signal: RX, pin_signal: ADC0_SE18/PTE25/LLWU_P21/CAN1_RX/UART4_RX/I2C0_SDA/EWM_IN}
  - {pin_num: '45', peripheral: UART4, signal: TX, pin_signal: ADC0_SE17/PTE24/CAN1_TX/UART4_TX/I2C0_SCL/EWM_OUT_b, direction: OUTPUT}
  - {pin_num: '2', peripheral: UART1, signal: RX, pin_signal: ADC1_SE5a/PTE1/LLWU_P0/SPI1_SOUT/UART1_RX/SDHC0_D0/TRACE_D3/I2C1_SCL/SPI1_SIN}
  - {pin_num: '1', peripheral: UART1, signal: TX, pin_signal: ADC1_SE4a/PTE0/SPI1_PCS1/UART1_TX/SDHC0_D1/TRACE_CLKOUT/I2C1_SDA/RTC_CLKOUT, direction: OUTPUT}
  - {pin_num: '124', peripheral: UART3, signal: TX, pin_signal: PTC17/CAN1_TX/UART3_TX/ENET0_1588_TMR1/FB_CS4_b/FB_TSIZ0/FB_BE31_24_BLS7_0_b/SDRAM_DQM3, direction: OUTPUT}
  - {pin_num: '133', peripheral: UART0, signal: RX, pin_signal: ADC0_SE7b/PTD6/LLWU_P15/SPI0_PCS3/UART0_RX/FTM0_CH6/FB_AD0/FTM0_FLT0/SPI1_SOUT}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR THE PINS TOOL ***
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitUARTs
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitUARTs(void) {
  CLOCK_EnableClock(kCLOCK_PortA);                           /* Port C Clock Gate Control: Clock enabled */
  CLOCK_EnableClock(kCLOCK_PortC);                           /* Port C Clock Gate Control: Clock enabled */
  CLOCK_EnableClock(kCLOCK_PortD);                           /* Port D Clock Gate Control: Clock enabled */
  CLOCK_EnableClock(kCLOCK_PortE);                           /* Port E Clock Gate Control: Clock enabled */
  PORT_SetPinMux(PORTD, PIN1_IDX, kPORT_MuxAsGpio);
  PORT_SetPinMux(PORTD, PIN0_IDX, kPORT_MuxAsGpio);
  PORT_SetPinMux(PORTE, PIN2_IDX, kPORT_MuxAsGpio);
  PORT_SetPinMux(PORTE, PIN3_IDX, kPORT_MuxAsGpio);
  PORT_SetPinMux(PORTA, PIN2_IDX, kPORT_MuxAlt2);           /* PORTA2 (pin 52) is configured as UART0_TX */
//  PORT_SetPinMux(PORTC, PIN17_IDX, kPORT_MuxAlt3);
  PORT_SetPinMux(PORTD, PIN6_IDX, kPORT_MuxAlt3);            /* PORTD6 (pin 133) is configured as UART0_RX */
  PORT_SetPinMux(PORTE, PIN0_IDX, kPORT_MuxAlt3);            /* PORTE0 (pin 1) is configured as UART1_TX */
  PORT_SetPinMux(PORTE, PIN1_IDX, kPORT_MuxAlt3);            /* PORTE1 (pin 2) is configured as UART1_RX */
  PORT_SetPinMux(PORTE, PIN2_IDX, kPORT_MuxAsGpio);            /* PORTE2 (pin 2) is configured as GPIO */
  PORT_SetPinMux(PORTE, PIN3_IDX, kPORT_MuxAsGpio);            /* PORTE3 (pin 3) is configured as GPIO */
  PORT_SetPinMux(PORTE, PIN4_IDX, kPORT_MuxAlt3);            /* PORTE4 (pin 4) is configured as UART3_TX */
  PORT_SetPinMux(PORTE, PIN5_IDX, kPORT_MuxAlt3); 
  PORT_SetPinMux(PORTE, PIN24_IDX, kPORT_MuxAlt3);           /* PORTE24 (pin 45) is configured as UART4_TX */
  PORT_SetPinMux(PORTE, PIN25_IDX, kPORT_MuxAlt3);           /* PORTE25 (pin 46) is configured as UART4_RX */
  SIM->SOPT5 = ((SIM->SOPT5 &
    (~(SIM_SOPT5_UART1TXSRC_MASK)))                          /* Mask bits to zero which are setting */
      | SIM_SOPT5_UART1TXSRC(SOPT5_UART1TXSRC_UART_TX)       /* UART 1 transmit data source select: UART1_TX pin */
    );
}


#define PIN0_IDX                         0u   /*!< Pin number for pin 0 in a port */
#define PIN1_IDX                         1u   /*!< Pin number for pin 1 in a port */
#define PIN2_IDX                         2u   /*!< Pin number for pin 2 in a port */
#define PIN3_IDX                         3u   /*!< Pin number for pin 3 in a port */
#define PIN4_IDX                         4u   /*!< Pin number for pin 4 in a port */
#define PIN5_IDX                         5u   /*!< Pin number for pin 5 in a port */
#define PIN6_IDX                         6u   /*!< Pin number for pin 6 in a port */
#define PIN7_IDX                         7u   /*!< Pin number for pin 7 in a port */
#define PIN8_IDX                         8u   /*!< Pin number for pin 8 in a port */
#define PIN9_IDX                         9u   /*!< Pin number for pin 9 in a port */
#define PIN10_IDX                       10u   /*!< Pin number for pin 10 in a port */
#define PIN11_IDX                       11u   /*!< Pin number for pin 11 in a port */
#define PIN12_IDX                       12u   /*!< Pin number for pin 12 in a port */
#define PIN13_IDX                       13u   /*!< Pin number for pin 13 in a port */
#define PIN14_IDX                       14u   /*!< Pin number for pin 14 in a port */
#define PIN15_IDX                       15u   /*!< Pin number for pin 15 in a port */
#define PIN16_IDX                       16u   /*!< Pin number for pin 16 in a port */
#define PIN17_IDX                       17u   /*!< Pin number for pin 17 in a port */
#define PIN18_IDX                       18u   /*!< Pin number for pin 18 in a port */
#define PIN19_IDX                       19u   /*!< Pin number for pin 19 in a port */
#define PIN20_IDX                       20u   /*!< Pin number for pin 20 in a port */
#define PIN21_IDX                       21u   /*!< Pin number for pin 21 in a port */
#define PIN22_IDX                       22u   /*!< Pin number for pin 22 in a port */
#define PIN23_IDX                       23u   /*!< Pin number for pin 23 in a port */

/*
 * TEXT BELOW IS USED AS SETTING FOR THE PINS TOOL *****************************
BOARD_InitSDRAM:
- options: {coreID: singlecore, enableClock: 'true'}
- pin_list:
  - {pin_num: '113', peripheral: SDRAM, signal: A15, pin_signal: ADC1_SE4b/CMP0_IN2/PTC8/FTM3_CH4/I2S0_MCLK/FB_AD7/SDRAM_A15}
  - {pin_num: '114', peripheral: SDRAM, signal: A14, pin_signal: ADC1_SE5b/CMP0_IN3/PTC9/FTM3_CH5/I2S0_RX_BCLK/FB_AD6/SDRAM_A14/FTM2_FLT0}
  - {pin_num: '115', peripheral: SDRAM, signal: A13, pin_signal: ADC1_SE6b/PTC10/I2C1_SCL/FTM3_CH6/I2S0_RX_FS/FB_AD5/SDRAM_A13}
  - {pin_num: '129', peripheral: SDRAM, signal: A12, pin_signal: PTD2/LLWU_P13/SPI0_SOUT/UART2_RX/FTM3_CH2/FB_AD4/SDRAM_A12/I2C0_SCL}
  - {pin_num: '130', peripheral: SDRAM, signal: A11, pin_signal: PTD3/SPI0_SIN/UART2_TX/FTM3_CH3/FB_AD3/SDRAM_A11/I2C0_SDA}
  - {pin_num: '131', peripheral: SDRAM, signal: A10, pin_signal: PTD4/LLWU_P14/SPI0_PCS1/UART0_RTS_b/FTM0_CH4/FB_AD2/SDRAM_A10/EWM_IN/SPI1_PCS0}
  - {pin_num: '132', peripheral: SDRAM, signal: A9, pin_signal: ADC0_SE6b/PTD5/SPI0_PCS2/UART0_CTS_b/UART0_COL_b/FTM0_CH5/FB_AD1/SDRAM_A9/EWM_OUT_b/SPI1_SCK}
  - {pin_num: '111', peripheral: SDRAM, signal: A17, pin_signal: CMP0_IN0/PTC6/LLWU_P10/SPI0_SOUT/PDB0_EXTRG/I2S0_RX_BCLK/FB_AD9/SDRAM_A17/I2S0_MCLK}
  - {pin_num: '110', peripheral: SDRAM, signal: A18, pin_signal: PTC5/LLWU_P9/SPI0_SCK/LPTMR0_ALT2/I2S0_RXD0/FB_AD10/SDRAM_A18/CMP0_OUT/FTM0_CH2}
  - {pin_num: '109', peripheral: SDRAM, signal: A19, pin_signal: PTC4/LLWU_P8/SPI0_PCS0/UART1_TX/FTM0_CH3/FB_AD11/SDRAM_A19/CMP1_OUT}
  - {pin_num: '105', peripheral: SDRAM, signal: A20, pin_signal: ADC0_SE4b/CMP1_IN0/TSI0_CH15/PTC2/SPI0_PCS2/UART1_CTS_b/FTM0_CH1/FB_AD12/SDRAM_A20/I2S0_TX_FS}
  - {pin_num: '104', peripheral: SDRAM, signal: A21, pin_signal: ADC0_SE15/TSI0_CH14/PTC1/LLWU_P6/SPI0_PCS3/UART1_RTS_b/FTM0_CH0/FB_AD13/SDRAM_A21/I2S0_TXD0}
  - {pin_num: '103', peripheral: SDRAM, signal: A22, pin_signal: ADC0_SE14/TSI0_CH13/PTC0/SPI0_PCS4/PDB0_EXTRG/USB0_SOF_OUT/FB_AD14/SDRAM_A22/I2S0_TXD1}
  - {pin_num: '97', peripheral: SDRAM, signal: A23, pin_signal: TSI0_CH11/PTB18/CAN0_TX/FTM2_CH0/I2S0_TX_BCLK/FB_AD15/SDRAM_A23/FTM2_QD_PHA/TPM2_CH0}
  - {pin_num: '81', peripheral: SDRAM, signal: CAS, pin_signal: ADC0_SE8/ADC1_SE8/TSI0_CH0/PTB0/LLWU_P5/I2C0_SCL/FTM1_CH0/RMII0_MDIO/MII0_MDIO/SDRAM_CAS_b/FTM1_QD_PHA/TPM1_CH0}
  - {pin_num: '136', peripheral: SDRAM, signal: CKE, pin_signal: PTD7/CMT_IRO/UART0_TX/FTM0_CH7/SDRAM_CKE/FTM0_FLT1/SPI1_SIN}
  - {pin_num: '58', peripheral: SDRAM, signal: CLKOUT, pin_signal: PTA6/FTM0_CH3/CLKOUT/TRACE_CLKOUT}
  - {pin_num: '126', peripheral: SDRAM, signal: DQM0, pin_signal: PTC19/UART3_CTS_b/ENET0_1588_TMR3/FB_CS3_b/FB_BE7_0_BLS31_24_b/SDRAM_DQM0/FB_TA_b}
  - {pin_num: '125', peripheral: SDRAM, signal: DQM1, pin_signal: PTC18/UART3_RTS_b/ENET0_1588_TMR2/FB_TBST_b/FB_CS2_b/FB_BE15_8_BLS23_16_b/SDRAM_DQM1}
  - {pin_num: '84', peripheral: SDRAM, signal: CS0, pin_signal: ADC0_SE13/TSI0_CH8/PTB3/I2C0_SDA/UART0_CTS_b/UART0_COL_b/ENET0_1588_TMR1/SDRAM_CS0_b/FTM0_FLT0}
  - {pin_num: '83', peripheral: SDRAM, signal: WE, pin_signal: ADC0_SE12/TSI0_CH7/PTB2/I2C0_SCL/UART0_RTS_b/ENET0_1588_TMR0/SDRAM_WE/FTM0_FLT3}
  - {pin_num: '82', peripheral: SDRAM, signal: RAS, pin_signal: ADC0_SE9/ADC1_SE9/TSI0_CH6/PTB1/I2C0_SDA/FTM1_CH1/RMII0_MDC/MII0_MDC/SDRAM_RAS_b/FTM1_QD_PHB/TPM1_CH1}
  - {pin_num: '96', peripheral: SDRAM, signal: D16, pin_signal: TSI0_CH10/PTB17/SPI1_SIN/UART0_TX/FTM_CLKIN1/FB_AD16/SDRAM_D16/EWM_OUT_b/TPM_CLKIN1}
  - {pin_num: '95', peripheral: SDRAM, signal: D17, pin_signal: TSI0_CH9/PTB16/SPI1_SOUT/UART0_RX/FTM_CLKIN0/FB_AD17/SDRAM_D17/EWM_IN/TPM_CLKIN0}
  - {pin_num: '92', peripheral: SDRAM, signal: D18, pin_signal: ADC1_SE15/PTB11/SPI1_SCK/UART3_TX/FB_AD18/SDRAM_D18/FTM0_FLT2}
  - {pin_num: '91', peripheral: SDRAM, signal: D19, pin_signal: ADC1_SE14/PTB10/SPI1_PCS0/UART3_RX/FB_AD19/SDRAM_D19/FTM0_FLT1}
  - {pin_num: '87', peripheral: SDRAM, signal: D23, pin_signal: ADC1_SE12/PTB6/FB_AD23/SDRAM_D23}
  - {pin_num: '88', peripheral: SDRAM, signal: D22, pin_signal: ADC1_SE13/PTB7/FB_AD22/SDRAM_D22}
  - {pin_num: '89', peripheral: SDRAM, signal: D21, pin_signal: PTB8/UART3_RTS_b/FB_AD21/SDRAM_D21}
  - {pin_num: '90', peripheral: SDRAM, signal: D20, pin_signal: PTB9/SPI1_PCS1/UART3_CTS_b/FB_AD20/SDRAM_D20}
  - {pin_num: '120', peripheral: SDRAM, signal: D24, pin_signal: PTC15/UART4_TX/FB_AD24/SDRAM_D24}
  - {pin_num: '119', peripheral: SDRAM, signal: D25, pin_signal: PTC14/UART4_RX/FB_AD25/SDRAM_D25}
  - {pin_num: '118', peripheral: SDRAM, signal: D26, pin_signal: PTC13/UART4_CTS_b/FTM_CLKIN1/FB_AD26/SDRAM_D26/TPM_CLKIN1}
  - {pin_num: '117', peripheral: SDRAM, signal: D27, pin_signal: PTC12/UART4_RTS_b/FTM_CLKIN0/FB_AD27/SDRAM_D27/FTM3_FLT0/TPM_CLKIN0}
  - {pin_num: '102', peripheral: SDRAM, signal: D28, pin_signal: PTB23/SPI2_SIN/SPI0_PCS5/FB_AD28/SDRAM_D28/CMP3_OUT}
  - {pin_num: '101', peripheral: SDRAM, signal: D29, pin_signal: PTB22/SPI2_SOUT/FB_AD29/SDRAM_D29/CMP2_OUT}
  - {pin_num: '100', peripheral: SDRAM, signal: D30, pin_signal: PTB21/SPI2_SCK/FB_AD30/SDRAM_D30/CMP1_OUT}
  - {pin_num: '99', peripheral: SDRAM, signal: D31, pin_signal: PTB20/SPI2_PCS0/FB_AD31/SDRAM_D31/CMP0_OUT}
  - {pin_num: '112', peripheral: SDRAM, signal: A16, pin_signal: CMP0_IN1/PTC7/SPI0_SIN/USB0_SOF_OUT/I2S0_RX_FS/FB_AD8/SDRAM_A16}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR THE PINS TOOL ***
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitSDRAM
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitSDRAM(void) {
  CLOCK_EnableClock(kCLOCK_PortA);                           /* Port A Clock Gate Control: Clock enabled */
  CLOCK_EnableClock(kCLOCK_PortB);                           /* Port B Clock Gate Control: Clock enabled */
  CLOCK_EnableClock(kCLOCK_PortC);                           /* Port C Clock Gate Control: Clock enabled */
  CLOCK_EnableClock(kCLOCK_PortD);                           /* Port D Clock Gate Control: Clock enabled */

  PORT_SetPinMux(PORTA, PIN6_IDX, kPORT_MuxAlt5);            /* PORTA6 (pin 58) is configured as CLKOUT */
  PORT_SetPinMux(PORTB, PIN0_IDX, kPORT_MuxAlt5);            /* PORTB0 (pin 81) is configured as SDRAM_CAS_b */
  PORT_SetPinMux(PORTB, PIN1_IDX, kPORT_MuxAlt5);            /* PORTB1 (pin 82) is configured as SDRAM_RAS_b */
  PORT_SetPinMux(PORTB, PIN10_IDX, kPORT_MuxAlt5);           /* PORTB10 (pin 91) is configured as SDRAM_D19 */
  PORT_SetPinMux(PORTB, PIN11_IDX, kPORT_MuxAlt5);           /* PORTB11 (pin 92) is configured as SDRAM_D18 */
  PORT_SetPinMux(PORTB, PIN16_IDX, kPORT_MuxAlt5);           /* PORTB16 (pin 95) is configured as SDRAM_D17 */
  PORT_SetPinMux(PORTB, PIN17_IDX, kPORT_MuxAlt5);           /* PORTB17 (pin 96) is configured as SDRAM_D16 */
  PORT_SetPinMux(PORTB, PIN18_IDX, kPORT_MuxAlt5);           /* PORTB18 (pin 97) is configured as SDRAM_A23 */
  PORT_SetPinMux(PORTB, PIN2_IDX, kPORT_MuxAlt5);            /* PORTB2 (pin 83) is configured as SDRAM_WE */
  PORT_SetPinMux(PORTB, PIN20_IDX, kPORT_MuxAlt5);           /* PORTB20 (pin 99) is configured as SDRAM_D31 */
  PORT_SetPinMux(PORTB, PIN21_IDX, kPORT_MuxAlt5);           /* PORTB21 (pin 100) is configured as SDRAM_D30 */
  PORT_SetPinMux(PORTB, PIN22_IDX, kPORT_MuxAlt5);           /* PORTB22 (pin 101) is configured as SDRAM_D29 */
  PORT_SetPinMux(PORTB, PIN23_IDX, kPORT_MuxAlt5);           /* PORTB23 (pin 102) is configured as SDRAM_D28 */
  PORT_SetPinMux(PORTB, PIN3_IDX, kPORT_MuxAlt5);            /* PORTB3 (pin 84) is configured as SDRAM_CS0_b */
  PORT_SetPinMux(PORTB, PIN6_IDX, kPORT_MuxAlt5);            /* PORTB6 (pin 87) is configured as SDRAM_D23 */
  PORT_SetPinMux(PORTB, PIN7_IDX, kPORT_MuxAlt5);            /* PORTB7 (pin 88) is configured as SDRAM_D22 */
  PORT_SetPinMux(PORTB, PIN8_IDX, kPORT_MuxAlt5);            /* PORTB8 (pin 89) is configured as SDRAM_D21 */
  PORT_SetPinMux(PORTB, PIN9_IDX, kPORT_MuxAlt5);            /* PORTB9 (pin 90) is configured as SDRAM_D20 */
  PORT_SetPinMux(PORTC, PIN0_IDX, kPORT_MuxAlt5);            /* PORTC0 (pin 103) is configured as SDRAM_A22 */
  PORT_SetPinMux(PORTC, PIN1_IDX, kPORT_MuxAlt5);            /* PORTC1 (pin 104) is configured as SDRAM_A21 */
  PORT_SetPinMux(PORTC, PIN10_IDX, kPORT_MuxAlt5);           /* PORTC10 (pin 115) is configured as SDRAM_A13 */
  PORT_SetPinMux(PORTC, PIN12_IDX, kPORT_MuxAlt5);           /* PORTC12 (pin 117) is configured as SDRAM_D27 */
  PORT_SetPinMux(PORTC, PIN13_IDX, kPORT_MuxAlt5);           /* PORTC13 (pin 118) is configured as SDRAM_D26 */
  PORT_SetPinMux(PORTC, PIN14_IDX, kPORT_MuxAlt5);           /* PORTC14 (pin 119) is configured as SDRAM_D25 */
  PORT_SetPinMux(PORTC, PIN15_IDX, kPORT_MuxAlt5);           /* PORTC15 (pin 120) is configured as SDRAM_D24 */
//  PORT_SetPinMux(PORTC, PIN18_IDX, kPORT_MuxAlt5);           /* PORTC18 (pin 125) is configured as SDRAM_DQM1 */
//  PORT_SetPinMux(PORTC, PIN19_IDX, kPORT_MuxAlt5);           /* PORTC19 (pin 126) is configured as SDRAM_DQM0 */
  PORT_SetPinMux(PORTC, PIN16_IDX, kPORT_MuxAlt5);           /* PORTC18 (pin 125) is configured as SDRAM_DQM2 */
  PORT_SetPinMux(PORTC, PIN17_IDX, kPORT_MuxAlt5);           /* PORTC19 (pin 126) is configured as SDRAM_DQM3*/
  PORT_SetPinMux(PORTC, PIN2_IDX, kPORT_MuxAlt5);            /* PORTC2 (pin 105) is configured as SDRAM_A20 */
  PORT_SetPinMux(PORTC, PIN4_IDX, kPORT_MuxAlt5);            /* PORTC4 (pin 109) is configured as SDRAM_A19 */
  PORT_SetPinMux(PORTC, PIN5_IDX, kPORT_MuxAlt5);            /* PORTC5 (pin 110) is configured as SDRAM_A18 */
  PORT_SetPinMux(PORTC, PIN6_IDX, kPORT_MuxAlt5);            /* PORTC6 (pin 111) is configured as SDRAM_A17 */
  PORT_SetPinMux(PORTC, PIN7_IDX, kPORT_MuxAlt5);            /* PORTC7 (pin 112) is configured as SDRAM_A16 */
  PORT_SetPinMux(PORTC, PIN8_IDX, kPORT_MuxAlt5);            /* PORTC8 (pin 113) is configured as SDRAM_A15 */
  PORT_SetPinMux(PORTC, PIN9_IDX, kPORT_MuxAlt5);            /* PORTC9 (pin 114) is configured as SDRAM_A14 */
  PORT_SetPinMux(PORTD, PIN2_IDX, kPORT_MuxAlt5);            /* PORTD2 (pin 129) is configured as SDRAM_A12 */
  PORT_SetPinMux(PORTD, PIN3_IDX, kPORT_MuxAlt5);            /* PORTD3 (pin 130) is configured as SDRAM_A11 */
  PORT_SetPinMux(PORTD, PIN4_IDX, kPORT_MuxAlt5);            /* PORTD4 (pin 131) is configured as SDRAM_A10 */
  PORT_SetPinMux(PORTD, PIN5_IDX, kPORT_MuxAlt5);            /* PORTD5 (pin 132) is configured as SDRAM_A9 */
  PORT_SetPinMux(PORTD, PIN7_IDX, kPORT_MuxAlt5);            /* PORTD7 (pin 136) is configured as SDRAM_CKE */
}

/*******************************************************************************
 * EOF
 ******************************************************************************/

