#include <stdint.h>
#include <stdbool.h>
#include <string.h> 

#include "Header.h"


#define RS2321_UART_IRQHandler          UART1_RX_TX_IRQHandler 
#define RS2322_UART_IRQHandler 		UART4_RX_TX_IRQHandler
//tUART sUART1; 
uart_config_t config;


void Init_RS232_UART (void)
{
  
  UART_GetDefaultConfig(&config);
  config.baudRate_Bps = 14400;
  config.enableTx = true;
  config.enableRx = true;

  UART_Init(RS2321_UART, &config, CLOCK_GetFreq(RS2321_UART_CLKSRC));
  /* Enable RX interrupt. */
  UART_EnableInterrupts(RS2321_UART, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
  EnableIRQ(RS2321_UART_IRQn);
//
//  UART_Init(RS2322_UART, &config, CLOCK_GetFreq(RS2322_UART_CLKSRC));
//  /* Enable RX interrupt. */
//  UART_EnableInterrupts(RS2322_UART, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
//  EnableIRQ(RS2322_UART_IRQn);  
}

void RS2321_UART_IRQHandler(void)
{
    unsigned char ucChar;
    if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(RS2321_UART))
    {
        ucChar=UART_ReadByte(RS2321_UART);
        g_UARTRxBuf[RecvCntStart]=  ucChar;
        RecvCntStart++;
        
        if((RecvCntStart>=300)||(RecvCntStart<0))
        {
          RecvCntStart=0;
        }
        PMUConnectCount =0;
//        RS2321Buff[RX2321Count] = ucChar;
//        RX2321Count++;
       
        switch(InterruptMesgState)
        {
        case CHECKBYTE1:
          if (ucChar == 0x7E)
          {
            sInterruptMesg.u8RecvByte[0] = ucChar;
            InterruptMesgState = CHECKBYTE2;
          } else 
          {
            sInterruptMesg.u8RecvByte[0] = 0;
            sInterruptMesg.u8RecvByte[1] = 0;
            sInterruptMesg.u8RecvByte[2] = 0;
          }
          break;
        case CHECKBYTE2:
          if (ucChar == 0x7B)
            {
              sInterruptMesg.u8RecvByte[1] = ucChar;
              InterruptMesgState = CHECKBYTE3;
            } else 
            {
              sInterruptMesg.u8RecvByte[0] = 0;
              sInterruptMesg.u8RecvByte[1] = 0;
              sInterruptMesg.u8RecvByte[2] = 0;
              InterruptMesgState = CHECKBYTE1;
            }
          break;
        case CHECKBYTE3:
          if (ucChar == 0x7C)
            {
              sInterruptMesg.u8RecvByte[2] = 0x7C;
            } else 
            {
              sInterruptMesg.u8RecvByte[0] = 0;
              sInterruptMesg.u8RecvByte[1] = 0;
              sInterruptMesg.u8RecvByte[2] = 0;
            }
          InterruptMesgState = CHECKBYTE1;         
          break;          
        }
      if ((sInterruptMesg.u8RecvByte[1] == 0x7B) && (sInterruptMesg.u8RecvByte[2] == 0x7C))
      {
              sInterruptMesg.u8Checkbit = 1;
              g_UARTRxBuf2[0] = 0x7E;
              g_UARTRxBuf2[1] = 0x7B;
              g_UARTRxBuf2[2] = 0x7C;
              RecvCntStart2 = 2;

              sInterruptMesg.u8RecvByte[0] = 0;
              sInterruptMesg.u8RecvByte[1] = 0;
              sInterruptMesg.u8RecvByte[2] = 0;
      }

      if (sInterruptMesg.u8Checkbit == 1)
      {
              g_UARTRxBuf2[RecvCntStart2] = ucChar;
              RecvCntStart2++;
      }
    
    }  
}
void RS2322_UART_IRQHandler(void)
{

    if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(RS2322_UART))
    {
        RS2322Buff[RX2322Count]=UART_ReadByte(RS2322_UART);
        RX2322Count++;  
       
    }
}