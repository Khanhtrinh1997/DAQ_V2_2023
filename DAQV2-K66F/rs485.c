#include <stdint.h>
#include <stdbool.h>
#include <string.h> 

#include "Header.h"

#if (USERDEF_RS485_AGISSON == ENABLED)

#define RS485_UART_IRQHandler           UART0_RX_TX_IRQHandler 
//#define RS485T_UART_IRQHandler 		UART0_RX_TX_IRQHandler
status_t UART_9Bit_Init(UART_Type *base, const uart_config_t *config, uint32_t srcClock_Hz);
//tUART sUART1; 
uint32_t RX485Count=0;
extern uint32_t iCounter;
VERTIV_T vertiv_t;
sMODBUSRTU_struct Modbus_vertiv;
void Init_RS485_UART(void)
{
  uint8_t temp = 0;
  UART_GetDefaultConfig(&config);
  config.baudRate_Bps = 14400;
  config.enableTx = true;
  config.enableRx = true;
  UART_9Bit_Init(RS485T_UART, &config, CLOCK_GetFreq(RS485T_UART_CLKSRC));
//  UART_Init(RS485T_UART, &config, CLOCK_GetFreq(RS485T_UART_CLKSRC));
  UART_9Bit_Init(RS485R_UART, &config, CLOCK_GetFreq(RS485R_UART_CLKSRC));
//  temp = UART_C1_M_MASK;
//  RS485T_UART->C1 |=  temp;

  /* Enable RX interrupt. */
  UART_EnableInterrupts(RS485R_UART, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
  EnableIRQ(RS485R_UART_IRQn);
 
}

void Init_RS485_UART_TEST(void)
{
  uint8_t temp = 0;
  UART_GetDefaultConfig(&config);
  config.baudRate_Bps = 14400;
  config.enableTx = true;
  config.enableRx = true;
  UART_Init(RS485T_UART, &config, CLOCK_GetFreq(RS485T_UART_CLKSRC));
//  UART_Init(RS485T_UART, &config, CLOCK_GetFreq(RS485T_UART_CLKSRC));
  UART_Init(RS485R_UART, &config, CLOCK_GetFreq(RS485R_UART_CLKSRC));
//  temp = UART_C1_M_MASK;
//  RS485T_UART->C1 |=  temp;

  /* Enable RX interrupt. */
  UART_EnableInterrupts(RS485R_UART, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
  EnableIRQ(RS485R_UART_IRQn);
}

//uint8_t thao =0;
uint8_t Recv03 =0;
uint8_t RecvComplete = 1;
  unsigned char thaouchar;
void Looptest(uint8_t ucChar)
{
   if(Modbus.runningStep ==_TEST_MODE)
     {
        sTestRS485.CabBuff[sTestRS485.CabRecvCnt++]=  ucChar;
     }
     if(sTestRS485.CabRecvCnt>14)   sTestRS485.CabRecvCnt=0;
}
uint8_t u8An;
void RS485_UART_IRQHandler(void)
{
  unsigned char ucChar,csum;

    if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(RS485R_UART))
    {
      if((TYPE==HW)||(TYPE==EMER)){
        Cab_Connection =0;
        ucChar=UART_ReadByte(RS485R_UART);
        Looptest(ucChar);
        thaouchar = ucChar;
        g_UARTRxBuf485[RecvCntStart485]=  ucChar;
        RecvCntStart485++;
        RecvComplete = 1;
        if( (g_UARTRxBuf485[0] == 0x03)&&(RecvCntStart485>2) )
        {
          RecvComplete = 0;
          RecvCntEnd485 = g_UARTRxBuf485[2]+3;
          if(iSet!=0)RecvComplete =1;
        }
        else  RecvCntEnd485=0;
        if( RecvCntStart485>  RecvCntEnd485)
        {
          csum = ChecksumH(&g_UARTRxBuf485[0],RecvCntEnd485);
          if(csum==  g_UARTRxBuf485[RecvCntEnd485])
          {
            RecvOK =1; 
            RecvComplete = 1;
            if(g_UARTRxBuf485[4]== AcmdCodeTable[RS485Command][3])
            {
              RecvCodeOK=1;
            }
            else
            {
              RecvCodeOK=0;
            }
          }
          else RecvOK =0;
        }
        if((Program_Status ==0))        //&&(RecvCntStart485>4)
          {   
//            if(g_UARTRxBuf485[4]==0x01)
//              {
            if(( RecvOK==1)&&((g_UARTRxBuf485[4]== 0x01)||(g_UARTRxBuf485[2]== 0xEF) ))
            {
                  Program_Status=1;
                  RS485Command = 1;
                  RS485Timer = 2;
                   iCounter =1;
            }
            if(g_UARTRxBuf485[0]== 0x73)
            {
                  Program_Status=1;
                  RS485Command = 1;
                  RS485Timer = 2;
                    iCounter =1;
            }
             if(g_UARTRxBuf485[0]== 0x03)
            {
                Recv03=1;
            }
//                  iCounter =1;
//              }
          }

        if(RecvCntStart485>299)RecvCntStart485=0;
      }
      else if (TYPE ==DKD51_BDP){
        ucChar=UART_ReadByte(RS485R_UART);
        dpc_t.uart_rx.buff_485[dpc_t.uart_rx.cnt] = ucChar;
        dpc_t.uart_rx.cnt++;
        if((dpc_t.uart_rx.cnt>=500)||(dpc_t.uart_rx.cnt<0)) dpc_t.uart_rx.cnt=0;
        PMUConnectCount =0;
      }
      else if(TYPE == VERTIV_M830B){ //2022 modbus standar 
        ucChar=UART_ReadByte(RS485R_UART);
        u8An=ucChar;
        PMUConnectCount =0;         
        if(Modbus_vertiv.u8MosbusEn==1)
            {
                Modbus_vertiv.u8BuffRead[Modbus_vertiv.u8DataPointer++] = ucChar;
                if(Modbus_vertiv.u8DataPointer == Modbus_vertiv.u8ByteCount)
                {
                    Modbus_vertiv.u8MosbusEn = 2;
                }
            }

            if((Modbus_vertiv.u8BuffRead[1] >= 1)&&(Modbus_vertiv.u8BuffRead[1] <= 50)) //Kiem tra Function Code
            {
                switch(Modbus_vertiv.u8BuffRead[1])
                {
                case _READ_COIL_STATUS:
                  Modbus_vertiv.u8ByteCount = Modbus_vertiv.u8BuffRead[2] + 5;
                    break;
                case _READ_INPUT_STATUS:
                  Modbus_vertiv.u8ByteCount = Modbus_vertiv.u8BuffRead[2] + 5;
                    break;
                case _READ_HOLDING_REGS:
                    Modbus_vertiv.u8ByteCount = Modbus_vertiv.u8BuffRead[2] + 5;      // 5 byte: slave addr, function, byteCount, 2 byteCRC 
                    break;
                case _READ_INPUT_REG:
                    Modbus_vertiv.u8ByteCount = Modbus_vertiv.u8BuffRead[2] + 5;
                    break;
                case _PRESET_SINGLE_REG:
                    Modbus_vertiv.u8ByteCount = 8;
                    break;
                case _PRESET_MULTIPLE_REGS:
                    Modbus_vertiv.u8ByteCount = 8;
                    break;
                case _SETUP_REGISTER:

                    break;
                case _USER_REGISTER:
                    Modbus_vertiv.u8ByteCount = 8;
                    break;
                case _REPORT_SLAVE_ID:
                    Modbus_vertiv.u8ByteCount = Modbus_vertiv.u8BuffRead[2] + 5;
                    break;   
                default:
                    Modbus_vertiv.u8ByteCount = 8;
                    break;
                }
            }
            else
            {
                Modbus_vertiv.u8MosbusEn = 0;
                Modbus_vertiv.u8DataPointer = 0;
            }

            if(ucChar==Modbus_vertiv.u8SlaveID && Modbus_vertiv.u8MosbusEn == 0)     // bat dau ghi khi nhan dc dia slave chnh xac
            {
                Modbus_vertiv.u8MosbusEn = 1;
                Modbus_vertiv.u8DataPointer = 0;

                Modbus_vertiv.u8BuffRead[Modbus_vertiv.u8DataPointer++] = ucChar;
            }
      } 
    }
}

#endif














