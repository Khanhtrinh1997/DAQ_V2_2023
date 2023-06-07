#include "board.h"
#include "fsl_gpio.h"
#include "fsl_uart.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "rs485RTU.h"
#include "variables.h"
#include "string.h"
#include "ftp_handler.h"
//#include "rs485.h"
#include "DriverFunc.h"
#include "stdio.h"

//#include "Header.h"
static const unsigned char CRCHighTable[] =
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

static const unsigned char CRCLowTable[] =
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
    0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
    0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
    0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
    0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
    0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
    0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
    0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
    0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

sMODBUSRTU_struct Modbus;

VAC_V2UPDATE_RECEIVE_struct vac_v2_update_receive;
tConfigHandler configHandle;
//tGenState eGenState;
extern TimeFormat SyncTime;
extern uint32_t RS485Command;
extern uint8_t Program_Status;
extern uint32_t RecvCntStart485;
extern unsigned char g_UARTRxBuf485[300];
extern uint32_t Cab_Connection;
extern uint8_t TYPE;
extern uint8_t thao;
uart_config_t config1;
uint32_t TxInterrupt_cnt = 0;
uint32_t RxInterrupt_cnt = 0;
uint32_t uartStatus = 0;

uint32_t u32eastronSDM72DPM1; 
uint16_t u16IVY=0;
uint32_t u32IVY=0;
float val1;
//check sum message for fan---------------------------------------------------------------------------
uint8_t DKD51_check_sum_fan(FAN_DPC_SWITCH_UART_T* p){
  for(uint16_t i=0;i<p->uart_rx.cnt;i++){
    p->rx.buff_485[i]=p->uart_rx.buff_485[i];
    p->uart_rx.buff_485[i]=0;
  }
  p->rx.cnt =p->uart_rx.cnt;
  if(p->rx.cnt>6){
    p->check_sum_read = hex2byte2(&p->rx.buff_485[0],p->rx.cnt-5);
    p->check_sum_calculator = Checksum16(&p->rx.buff_485[0],p->rx.cnt);
  }else{
    p->check_sum_calculator = 0;
    p->check_sum_read       = 0xFFFF;
  }
  if(p->check_sum_read==p->check_sum_calculator) return NO_ERR;
  else return ERR;
}
//check return----------------------------------------------------------------------------------------
uint8_t DKD51_check_RTN_fan(FAN_DPC_SWITCH_UART_T* p){
  if(hex2byte(&p->rx.buff_485[0],7)== 0) return NO_ERR;
  else return ERR;
}
//clean fan-----------------------------------------------------------------------------------------
void DKD51_clean_buff_fan(FAN_DPC_SWITCH_UART_T* p){
  for(uint16_t i=0; i<500;i++){
    p->uart_rx.buff_485[i] =0;
    p->rx.buff_485[i]      =0;        
  }
  p->uart_rx.cnt = 0;
  p->rx.cnt      = 0;
}
//-----------------------------------------------------------------------------------------------------
void Init_RS485_MODBUS_UART (void)      // ko co parity bit
{
    UART_GetDefaultConfig(&config1);
    config1.baudRate_Bps = RS4851R_UART_BAUDRATE;
    config1.enableTx = true;
    config1.enableRx = true;

    UART_Init(RS4851R_UART, &config1, CLOCK_GetFreq(RS4851R_UART_CLKSRC));
    UART_Init(RS4851T_UART, &config1, CLOCK_GetFreq(RS4851T_UART_CLKSRC));
    /* Enable RX interrupt. */
    UART_EnableInterrupts(RS4851R_UART, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
    EnableIRQ(RS4851R_UART_IRQn);
//    /* Enable TX interrupt. */
//    UART_EnableInterrupts(RS4851T_UART, kUART_TransmissionCompleteInterruptEnable);
//    EnableIRQ(RS4851T_UART_IRQn);
}

void ReInit_RS485_MODBUS_UART (void)    // co parity bit
{
  UART_Deinit(RS4851R_UART);
  UART_GetDefaultConfig(&config1);    
  config1.parityMode = kUART_ParityEven;
  config1.baudRate_Bps = RS4851R_UART_BAUDRATE;
  config1.enableTx = true;
  config1.enableRx = true;

  UART_Init(RS4851R_UART, &config1, CLOCK_GetFreq(RS4851R_UART_CLKSRC));
  UART_Init(RS4851T_UART, &config1, CLOCK_GetFreq(RS4851T_UART_CLKSRC));
  /* Enable RX interrupt. */
  UART_EnableInterrupts(RS4851R_UART, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
  EnableIRQ(RS4851R_UART_IRQn);
  /* Enable TX interrupt. */
}
//for yada dpc-------------------------------------------------------------------
void ReInit_RS485_MODBUS_UART_yada_dpc (void)    // co parity bit-baud rate 2400
{
  UART_Deinit(RS4851R_UART);
  UART_GetDefaultConfig(&config1);    
  config1.parityMode = kUART_ParityEven;
  config1.baudRate_Bps = RS4851R_UART_BAUDRATE_2400;
  config1.enableTx = true;
  config1.enableRx = true;

  UART_Init(RS4851R_UART, &config1, CLOCK_GetFreq(RS4851R_UART_CLKSRC));
  UART_Init(RS4851T_UART, &config1, CLOCK_GetFreq(RS4851T_UART_CLKSRC));
  /* Enable RX interrupt. */
  UART_EnableInterrupts(RS4851R_UART, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
  EnableIRQ(RS4851R_UART_IRQn);
  /* Enable TX interrupt. */
}
//-------------------------------------------------------------------------------
//An 2022
void ReInit_RS485_MODBUS_UART_2022 (uint32_t baud, uart_parity_mode_t parity)    
{
  UART_Deinit(RS4851R_UART);
  UART_GetDefaultConfig(&config1);
  config1.baudRate_Bps = baud;
  config1.parityMode = parity;
  config1.baudRate_Bps = RS4851R_UART_BAUDRATE;
  config1.enableTx = true;
  config1.enableRx = true;

  UART_Init(RS4851R_UART, &config1, CLOCK_GetFreq(RS4851R_UART_CLKSRC));
  UART_Init(RS4851T_UART, &config1, CLOCK_GetFreq(RS4851T_UART_CLKSRC));
  /* Enable RX interrupt. */
  UART_EnableInterrupts(RS4851R_UART, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
  EnableIRQ(RS4851R_UART_IRQn);
  /* Enable TX interrupt. */
}


void ReInit2_RS485_MODBUS_UART (uint32_t baud)
{
  UART_Deinit(RS4851R_UART);
  UART_GetDefaultConfig(&config1);    
  config1.baudRate_Bps = baud;
  config1.enableTx = true;
  config1.enableRx = true;

  UART_Init(RS4851R_UART, &config1, CLOCK_GetFreq(RS4851R_UART_CLKSRC));
  UART_Init(RS4851T_UART, &config1, CLOCK_GetFreq(RS4851T_UART_CLKSRC));
  /* Enable RX interrupt. */
  UART_EnableInterrupts(RS4851R_UART, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
  EnableIRQ(RS4851R_UART_IRQn);
  /* Enable TX interrupt. */
}

void RS4851T_MODBUS_UART_IRQHandler(void)
{
//    if (kUART_TransmissionCompleteFlag & UART_GetStatusFlags(RS4851T_UART))
//    {
//      TxInterrupt_cnt++;
//      GPIO_ClearPinsOutput(GPIOD,1u << 1u);
//    }
//    UART_ClearStatusFlags(RS4851T_UART,kUART_TransmissionCompleteFlag);

}
void Looptest2(uint8_t ucChar)
{
   if(Modbus.runningStep ==_TEST_MODE)
     {
        sTestRS485.GenBuff[sTestRS485.GenRecvCnt++]=  ucChar;
     }
   if(sTestRS485.GenRecvCnt>14)   sTestRS485.GenRecvCnt=0;
}
uint8_t testcnt=0;
void RS4851R_MODBUS_UART_IRQHandler(void)
{
    uint8_t ucChar;

    if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(RS4851R_UART))
    {
        ucChar = UART_ReadByte(RS4851R_UART);
        testcnt=ucChar;
        Looptest2(ucChar);
        //update vac V2========================================================
        if((configHandle.is_vac_v2_update==1)
              &&(configHandle.Check_vac_firm_version>=VAC_CHECK_FIRM_VERSION)
              &&(configHandle.Check_vac_firm_version!=0))
        {
          vac_v2_update_receive.buff[vac_v2_update_receive.rxcnt++]= ucChar;
          if(vac_v2_update_receive.rxcnt>=50){
             vac_v2_update_receive.rxcnt=0;
          }
          
        }
        //======================================================================
        if((fan_dpc_switch_uart_t.rx_enable == STATE_ON)&&(TYPE == DKD51_BDP)){
          // for modbus china----------------------------------------------------
            ucChar = UART_ReadByte(RS4851R_UART);
            fan_dpc_switch_uart_t.uart_rx.buff_485[fan_dpc_switch_uart_t.uart_rx.cnt] = ucChar;
            fan_dpc_switch_uart_t.uart_rx.cnt++;
            if((fan_dpc_switch_uart_t.uart_rx.cnt>=500)||(fan_dpc_switch_uart_t.uart_rx.cnt<0)) fan_dpc_switch_uart_t.uart_rx.cnt=0;
        }
        else
        {
          // for modbus standard------------------------------------------------
            if(Modbus.u8MosbusEn==1)
            {
                Modbus.u8BuffRead[Modbus.u8DataPointer++] = ucChar;
                if(Modbus.u8DataPointer == Modbus.u8ByteCount)
                {
                    Modbus.u8MosbusEn = 2;
                }
            }

            if((Modbus.u8BuffRead[1] >= 1)&&(Modbus.u8BuffRead[1] <= 50)) //Kiem tra Function Code
            {
                switch(Modbus.u8BuffRead[1])
                {
                case _READ_COIL_STATUS:
                  Modbus.u8ByteCount = Modbus.u8BuffRead[2] + 5;
                    break;
                case _READ_INPUT_STATUS:
                  Modbus.u8ByteCount = Modbus.u8BuffRead[2] + 5;
                    break;
                case _READ_HOLDING_REGS:
                    Modbus.u8ByteCount = Modbus.u8BuffRead[2] + 5;      // 5 byte: slave addr, function, byteCount, 2 byteCRC 
                    break;
                case _READ_INPUT_REG:
                    Modbus.u8ByteCount = Modbus.u8BuffRead[2] + 5;
                    break;
                case _PRESET_SINGLE_REG:
                    Modbus.u8ByteCount = 8;
                    break;
                case _PRESET_MULTIPLE_REGS:
                    Modbus.u8ByteCount = 8;
                    break;
                case _SETUP_REGISTER:

                    break;
                case _USER_REGISTER:
                    Modbus.u8ByteCount = 8;
                    break;
                case _REPORT_SLAVE_ID:
                    Modbus.u8ByteCount = Modbus.u8BuffRead[2] + 5;
                    break;   
                default:
                    Modbus.u8ByteCount = 8;
                    break;
                }
            }
            else
            {
                Modbus.u8MosbusEn = 0;
                Modbus.u8DataPointer = 0;
            }

            if(ucChar==Modbus.u8SlaveID && Modbus.u8MosbusEn == 0)     // bat dau ghi khi nhan dc dia slave chnh xac
            {
                Modbus.u8MosbusEn = 1;
                Modbus.u8DataPointer = 0;
                Modbus.u8BuffRead[Modbus.u8DataPointer++] = ucChar;
            }
        }
    }
}

#if (USERDEF_MONITOR_GEN == ENABLED)
int8_t RS4851_Check_Respond_Data_GEN (void)
{
    uint16_t	mTemp = 0;

    if(Modbus.u8MosbusEn==2)
    {
        Modbus.u8CRCHighRead = Modbus.u8BuffRead[Modbus.u8ByteCount-2];
        Modbus.u8CRCLowRead  = Modbus.u8BuffRead[Modbus.u8ByteCount-1];

        ModbusCRC(&Modbus.u8BuffRead[0],Modbus.u8ByteCount-2);

        if(Modbus.u8CRCHigh == Modbus.u8CRCHighRead && Modbus.u8CRCLow==Modbus.u8CRCLowRead)
        {
            Modbus.u8FunctionCode 	= Modbus.u8BuffRead[1];

            switch(Modbus.u8FunctionCode)
            {
            case _READ_INPUT_STATUS:         
            case _READ_COIL_STATUS:
            {
              mTemp = (Modbus.u8NumberRegHigh<<8)|(Modbus.u8NumberRegLow);
              if (Modbus.u8BuffRead[2] == mTemp)
              {
                  switch(Modbus.u8SlaveID - sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenSlaveOffset)
                  {
                  case 1:
                  case 2:
                  {
                      Extract_Holding_Regs_Data_GEN();
                  }
                  break;

                  default:
                      break;
                  }
              }
              else
              {
                  Modbus.u8MosbusEn = 0;
                  return -3;
              }
          }
          break;
            case _READ_HOLDING_REGS:
            case _READ_INPUT_REG:
            {
                mTemp = (Modbus.u8NumberRegHigh<<8)|(Modbus.u8NumberRegLow);
                if (Modbus.u8BuffRead[2] == (mTemp*2))
                {
                    switch(Modbus.u8SlaveID - sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenSlaveOffset)
                    {
                    case 1:
                    case 2:
                    {
                        Extract_Holding_Regs_Data_GEN();
                    }
                    break;

                    default:
                        break;
                    }
                }
                else
                {
                    Modbus.u8MosbusEn = 0;
                    return -3;
                }
            }
            break;
            case _PRESET_SINGLE_REG:
            {
              Modbus.u8MosbusEn = 0;
              return 6;
            }            
            case _PRESET_MULTIPLE_REGS:
            {
              Modbus.u8MosbusEn = 0;
              return 16;
            }
            case _EXCEPTION_READ_HOLDING:
            {
              Modbus.u8MosbusEn = 0;
              return -4;
            }
            break;
            default:
                break;
            }

            Modbus.u8MosbusEn = 0;
        }
        else
        {
            Modbus.u8MosbusEn = 0;
            return -2;
        }
    }
    else
    {
        Modbus.u8MosbusEn = 0;
        return -1;
    }
    return 1;
}
#endif
#if (USERDEF_MONITOR_PM == ENABLED)
int8_t RS4851_Check_Respond_Data_PM (void)
{
    uint16_t	mTemp = 0;

    if(Modbus.u8MosbusEn==2)
    {
        Modbus.u8CRCHighRead = Modbus.u8BuffRead[Modbus.u8ByteCount-2];
        Modbus.u8CRCLowRead  = Modbus.u8BuffRead[Modbus.u8ByteCount-1];

        ModbusCRC(&Modbus.u8BuffRead[0],Modbus.u8ByteCount-2);

        if(Modbus.u8CRCHigh == Modbus.u8CRCHighRead && Modbus.u8CRCLow==Modbus.u8CRCLowRead)
        {
            Modbus.u8FunctionCode 	= Modbus.u8BuffRead[1];

            switch(Modbus.u8FunctionCode)
            {
            case _READ_HOLDING_REGS:
            case _READ_INPUT_REG:
            {
                mTemp = (Modbus.u8NumberRegHigh<<8)|(Modbus.u8NumberRegLow);
                if (Modbus.u8BuffRead[2] == (mTemp*2))
                {
                    switch(Modbus.u8SlaveID - sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMSlaveOffset)      // 
                    {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    case 9:
                    case 10:
                    {
                        Extract_Holding_Regs_Data_PM();
                    }
                    break;

                    default:
                        break;
                    }
                }
                else
                {
                    Modbus.u8MosbusEn = 0;
                    return -3;
                }
            }
            break;
            case _EXCEPTION_READ_HOLDING:
            {
              Modbus.u8MosbusEn = 0;
              return -4;
            }
            break;
            default:
                break;
            }

            Modbus.u8MosbusEn = 0;
        }
        else
        {
            Modbus.u8MosbusEn = 0;
            return -2;
        }
    }
    else
    {
        Modbus.u8MosbusEn = 0;
        return -1;
    }
    return 1;
}
#endif
#if (USERDEF_MONITOR_VAC == ENABLED)
int8_t RS4851_Check_Respond_Data_VAC (void)
{
    uint16_t	mTemp = 0;

    if(Modbus.u8MosbusEn==2)
    {
        Modbus.u8CRCHighRead = Modbus.u8BuffRead[Modbus.u8ByteCount-2];
        Modbus.u8CRCLowRead  = Modbus.u8BuffRead[Modbus.u8ByteCount-1];

        ModbusCRC(&Modbus.u8BuffRead[0],Modbus.u8ByteCount-2);

        if(Modbus.u8CRCHigh == Modbus.u8CRCHighRead && Modbus.u8CRCLow==Modbus.u8CRCLowRead)
        {
            Modbus.u8FunctionCode 	= Modbus.u8BuffRead[1];

            switch(Modbus.u8FunctionCode)
            {
            case _READ_HOLDING_REGS:
            case _READ_INPUT_REG:
            {
                mTemp = (Modbus.u8NumberRegHigh<<8)|(Modbus.u8NumberRegLow);
                if (Modbus.u8BuffRead[2] == (mTemp*2))
                {
                    switch(Modbus.u8SlaveID - sModbusManager.sVACManager[sModbusManager.u8VACCurrentIndex].u32VACSlaveOffset)
                    {
                    case 1:
                    case 2:
                    {
                        Extract_Holding_Regs_Data_VAC();
                    }
                    break;

                    default:
                        break;
                    }
                }
                else
                {
                    Modbus.u8MosbusEn = 0;
                    return -3;
                }
            }
            break;
            case _EXCEPTION_READ_HOLDING:
            {
              Modbus.u8MosbusEn = 0;
              return -4;
            }
            break;
            case _PRESET_MULTIPLE_REGS:
              {
                Modbus.u8MosbusEn = 0;
                return 16; 
              }
              break;
            default:
                break;
            }

            Modbus.u8MosbusEn = 0;
        }
        else
        {
            Modbus.u8MosbusEn = 0;
            return -2;
        }
    }
    else
    {
        Modbus.u8MosbusEn = 0;
        return -1;
    }
    return 1;
}
#endif
#if (USERDEF_MONITOR_SMCB == ENABLED) // smcb
int8_t RS4851_Check_Respond_Data_SMCB (void)
{
  uint16_t	mTemp = 0;
  
  if(Modbus.u8MosbusEn==2)    // finish reading
    {
        Modbus.u8CRCHighRead = Modbus.u8BuffRead[Modbus.u8ByteCount-2];
        Modbus.u8CRCLowRead  = Modbus.u8BuffRead[Modbus.u8ByteCount-1];

        ModbusCRC(&Modbus.u8BuffRead[0],Modbus.u8ByteCount-2);

        if(Modbus.u8CRCHigh == Modbus.u8CRCHighRead && Modbus.u8CRCLow==Modbus.u8CRCLowRead)
        {
            Modbus.u8FunctionCode 	= Modbus.u8BuffRead[1];

            switch(Modbus.u8FunctionCode)
            {
            case _READ_HOLDING_REGS:       
            {
                mTemp = (Modbus.u8NumberRegHigh<<8)|(Modbus.u8NumberRegLow);
                if (Modbus.u8BuffRead[2] == (mTemp*2))  // ByteCount == 2*NumberReg
                {
                    switch(Modbus.u8SlaveID - sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u32SMCBSlaveOffset)   
                    {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    {
                        Extract_Holding_Regs_Data_SMCB();
                    }
                    break;

                    default:
                        break;
                    }
                }
                else
                {
                    Modbus.u8MosbusEn = 0;
                    return -3;
                }
            }
                break; 
            case _EXCEPTION_READ_HOLDING:

            break;
            case _PRESET_SINGLE_REG:
                Modbus.u8MosbusEn = 0;
                return 6;
                break;
            case _PRESET_MULTIPLE_REGS:
                Modbus.u8MosbusEn = 0;
                return 16;
                break;
            case _USER_REGISTER:

                break;
            case _REPORT_SLAVE_ID:
            
                break;
            default:
                break;
            }

            Modbus.u8MosbusEn = 0;
        }
        else
        {
            Modbus.u8MosbusEn = 0;
            return -2;
        }
    }
    else
    {
        Modbus.u8MosbusEn = 0;
        return -1;
    }
    return 1;
}
#endif
//=======================fuel=========================//
#if (USERDEF_MONITOR_FUEL == ENABLED) // fuel
int8_t RS4851_Check_Respond_Data_FUEL (void)
{
  uint16_t	mTemp = 0;
  
  if(Modbus.u8MosbusEn==2)    // finish reading
    {
        Modbus.u8CRCHighRead = Modbus.u8BuffRead[Modbus.u8ByteCount-2];
        Modbus.u8CRCLowRead  = Modbus.u8BuffRead[Modbus.u8ByteCount-1];

        ModbusCRC(&Modbus.u8BuffRead[0],Modbus.u8ByteCount-2);

        if(Modbus.u8CRCHigh == Modbus.u8CRCHighRead && Modbus.u8CRCLow==Modbus.u8CRCLowRead)
        {
            Modbus.u8FunctionCode 	= Modbus.u8BuffRead[1];

            switch(Modbus.u8FunctionCode)
            {
            case _READ_HOLDING_REGS:       
            {
                mTemp = (Modbus.u8NumberRegHigh<<8)|(Modbus.u8NumberRegLow);
                if (Modbus.u8BuffRead[2] == (mTemp*2))  // ByteCount == 2*NumberReg
                {
                    switch(Modbus.u8SlaveID - sModbusManager.sFUELManager[sModbusManager.u8FUELCurrentIndex].u32FUELSlaveOffset)   
                    {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    {
                        Extract_Holding_Regs_Data_FUEL();
                    }
                    break;

                    default:
                        break;
                    }
                }
                else
                {
                    Modbus.u8MosbusEn = 0;
                    return -3;
                }
            }
                break; 
            case _EXCEPTION_READ_HOLDING:

            break;
            case _PRESET_SINGLE_REG:
                Modbus.u8MosbusEn = 0;
                return 6;
                break;
            case _PRESET_MULTIPLE_REGS:
                Modbus.u8MosbusEn = 0;
                return 16;
                break;
            case _USER_REGISTER:

                break;
            case _REPORT_SLAVE_ID:
            
                break;
            default:
                break;
            }

            Modbus.u8MosbusEn = 0;
        }
        else
        {
            Modbus.u8MosbusEn = 0;
            return -2;
        }
    }
    else
    {
        Modbus.u8MosbusEn = 0;
        return -1;
    }
    return 1;
}
#endif
//=======================fuel=========================//

#if (USERDEF_MONITOR_ISENSE == ENABLED)
int8_t RS4851_Check_Respond_Data_ISENSE (void)
{
    uint16_t	mTemp = 0;

    if(Modbus.u8MosbusEn==2)
    {
        Modbus.u8CRCHighRead = Modbus.u8BuffRead[Modbus.u8ByteCount-2];
        Modbus.u8CRCLowRead  = Modbus.u8BuffRead[Modbus.u8ByteCount-1];

        ModbusCRC(&Modbus.u8BuffRead[0],Modbus.u8ByteCount-2);

        if(Modbus.u8CRCHigh == Modbus.u8CRCHighRead && Modbus.u8CRCLow==Modbus.u8CRCLowRead)
        {
            Modbus.u8FunctionCode 	= Modbus.u8BuffRead[1];

            switch(Modbus.u8FunctionCode)
            {
            case _READ_HOLDING_REGS:
            case _READ_INPUT_REG:
            {
                mTemp = (Modbus.u8NumberRegHigh<<8)|(Modbus.u8NumberRegLow);
                if (Modbus.u8BuffRead[2] == (mTemp*2))
                {
                    switch(Modbus.u8SlaveID - sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u32ISENSESlaveOffset)      // 
                    {
                    case 1:
                    {
                        Extract_Holding_Regs_Data_ISENSE();
                    }
                    break;

                    default:
                        break;
                    }
                }
                else
                {
                    Modbus.u8MosbusEn = 0;
                    return -3;
                }
            }
            break;
            case _EXCEPTION_READ_HOLDING:
            {
              Modbus.u8MosbusEn = 0;
              return -4;
            }
            break;
            default:
                break;
            }

            Modbus.u8MosbusEn = 0;
        }
        else
        {
            Modbus.u8MosbusEn = 0;
            return -2;
        }
    }
    else
    {
        Modbus.u8MosbusEn = 0;
        return -1;
    }
    return 1;
}
#endif
//check response PM DC=======================================================
#if (USERDEF_MONITOR_PM_DC == ENABLED)
int8_t RS4851_Check_Respond_Data_PM_DC (void){
    uint16_t	mTemp = 0;

    if(Modbus.u8MosbusEn==2)
    {
        Modbus.u8CRCHighRead = Modbus.u8BuffRead[Modbus.u8ByteCount-2];
        Modbus.u8CRCLowRead  = Modbus.u8BuffRead[Modbus.u8ByteCount-1];

        ModbusCRC(&Modbus.u8BuffRead[0],Modbus.u8ByteCount-2);

        if(Modbus.u8CRCHigh == Modbus.u8CRCHighRead && Modbus.u8CRCLow==Modbus.u8CRCLowRead)
        {
            Modbus.u8FunctionCode 	= Modbus.u8BuffRead[1];

            switch(Modbus.u8FunctionCode)
            {
            case _READ_HOLDING_REGS:
            case _READ_INPUT_REG:
            {
                mTemp = (Modbus.u8NumberRegHigh<<8)|(Modbus.u8NumberRegLow);
                if (Modbus.u8BuffRead[2] == (mTemp*2))
                {
                    switch(Modbus.u8SlaveID - sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u32_pm_dc_slave_offset)      // 
                    {
                    case 1:
                    {
                        Extract_Holding_Regs_Data_PM_DC();
                    }
                    break;

                    default:
                        break;
                    }
                }
                else
                {
                    Modbus.u8MosbusEn = 0;
                    return -3;
                }
            }
            break;
            case _EXCEPTION_READ_HOLDING:
            {
              Modbus.u8MosbusEn = 0;
              return -4;
            }
            break;
            default:
                break;
            }

            Modbus.u8MosbusEn = 0;
        }
        else
        {
            Modbus.u8MosbusEn = 0;
            return -2;
        }
    }
    else
    {
        Modbus.u8MosbusEn = 0;
        return -1;
    }
    return 1;
}
#endif
//===========================================================================
#if (USERDEF_MONITOR_LIB == ENABLED)
//=== Gui ma lenh 16 cho LIB
void SetVar_i32LIB(uint8_t slaveAddr, uint16_t regAddr, int32_t val)
{
  uint8_t u8DataBuff[4];
  u8DataBuff[0] = (uint8_t) (val >> 24);
  u8DataBuff[1] = (uint8_t) (val >> 16);
  u8DataBuff[2] = (uint8_t) (val >> 8);
  u8DataBuff[3] = (uint8_t) (val);
  Write_Multi_Reg (slaveAddr, regAddr, &u8DataBuff[0], 2);
  vTaskDelay(500);
  if(RS4851_Check_Respond_Data_LIB() != 16){
      Write_Multi_Reg (slaveAddr, regAddr, &u8DataBuff[0], 2);
      vTaskDelay(500);
      if(RS4851_Check_Respond_Data_LIB() != 16){
      Write_Multi_Reg (slaveAddr, regAddr, &u8DataBuff[0], 2);
    }
  }
}
void SetVar_u16LIB(uint8_t slaveAddr, uint16_t regAddr, uint16_t val){
  uint8_t u8DataBuff[2];
  u8DataBuff[0] = (uint8_t) (val >> 8);
  u8DataBuff[1] = (uint8_t) (val);
  Write_Multi_Reg (slaveAddr, regAddr, &u8DataBuff[0], 1);  
  vTaskDelay(500);
  if(RS4851_Check_Respond_Data_LIB() != 16){
      Write_Multi_Reg (slaveAddr, regAddr, &u8DataBuff[0], 1);
      vTaskDelay(500);
      if(RS4851_Check_Respond_Data_LIB() != 16){
      Write_Multi_Reg (slaveAddr, regAddr, &u8DataBuff[0], 1);
    }
  }
}

int8_t RS4851_Check_Respond_Data_LIB (void)
{
    uint16_t	mTemp = 0;

    if(Modbus.u8MosbusEn==2)    // finish reading
    {
        Modbus.u8CRCHighRead = Modbus.u8BuffRead[Modbus.u8ByteCount-2];
        Modbus.u8CRCLowRead  = Modbus.u8BuffRead[Modbus.u8ByteCount-1];

        ModbusCRC(&Modbus.u8BuffRead[0],Modbus.u8ByteCount-2);

        if(Modbus.u8CRCHigh == Modbus.u8CRCHighRead && Modbus.u8CRCLow==Modbus.u8CRCLowRead)
        {
            Modbus.u8FunctionCode 	= Modbus.u8BuffRead[1];

            switch(Modbus.u8FunctionCode)
            {
            case _READ_COIL_STATUS:
            case _READ_INPUT_STATUS:
            {
                mTemp = (Modbus.u8NumberRegHigh<<8)|(Modbus.u8NumberRegLow);
                if (Modbus.u8BuffRead[2] == mTemp)  
                {
                    switch(Modbus.u8SlaveID - sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBSlaveOffset)   
                    {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    case 9:
                    case 10:
                    case 11:
                    case 12:
                    case 13:
                    case 14:
                    case 15:
                    case 16:
                    {
                        Extract_Holding_Regs_Data_LIB();
                    }
                    break;

                    default:
                        break;
                    }
                }
                else
                {
                    Modbus.u8MosbusEn = 0;
                    return -3;
                }
            }
            break;
            case _READ_HOLDING_REGS:
            case _READ_INPUT_REG:       
            {
                mTemp = (Modbus.u8NumberRegHigh<<8)|(Modbus.u8NumberRegLow);
                if (Modbus.u8BuffRead[2] == (mTemp*2))  // ByteCount == 2*NumberReg
                {
                    switch(Modbus.u8SlaveID - sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBSlaveOffset)   // ???????????????
                    {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    case 9:
                    case 10:
                    case 11:
                    case 12:
                    case 13:
                    case 14:
                    case 15:
                    case 16:
                    {
                        Extract_Holding_Regs_Data_LIB();
                    }
                    break;

                    default:
                        break;
                    }
                }
                else
                {
                    Modbus.u8MosbusEn = 0;
                    return -3;
                }
            }
            break;
            
            case _EXCEPTION_READ_HOLDING:
            {
              Modbus.u8MosbusEn = 0;
              return -4;
            }
            break;
            case _PRESET_SINGLE_REG:

                break;
            case _USER_REGISTER:

                break;
                           
            case _PRESET_MULTIPLE_REGS:
            {
              Modbus.u8MosbusEn = 0;
              return 16;
            }
            case _REPORT_SLAVE_ID:
            {
              switch(Modbus.u8SlaveID - sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBSlaveOffset)   
              {
              case 1:
              case 2:
              case 3:
              case 4:
              case 5:
              case 6:
              case 7:
              case 8:
              case 9:
              case 10:
              case 11:
              case 12:
              case 13:
              case 14:
              case 15:
              case 16:
              {
                  Extract_Holding_Regs_Data_LIB();
              }
              break;

              default:
                  break;
              }

            }
            break;
            default:
                break;
            }

            Modbus.u8MosbusEn = 0;
        }
        else
        {
            Modbus.u8MosbusEn = 0;
            return -2;
        }
    }
    else
    {
        Modbus.u8MosbusEn = 0;
        return -1;
    }
    return 1;
}
#endif
#if (USERDEF_MONITOR_BM == ENABLED)
int8_t RS4851_Check_Respond_Data_BM (void)
{
    uint16_t	mTemp = 0;

    if(Modbus.u8MosbusEn==2)
    {
        Modbus.u8CRCHighRead = Modbus.u8BuffRead[Modbus.u8ByteCount-2];
        Modbus.u8CRCLowRead  = Modbus.u8BuffRead[Modbus.u8ByteCount-1];

        ModbusCRC(&Modbus.u8BuffRead[0],Modbus.u8ByteCount-2);

        if(Modbus.u8CRCHigh == Modbus.u8CRCHighRead && Modbus.u8CRCLow==Modbus.u8CRCLowRead)
        {
            Modbus.u8FunctionCode 	= Modbus.u8BuffRead[1];

            switch(Modbus.u8FunctionCode)
            {
            case _READ_HOLDING_REGS:
            case _READ_INPUT_REG:
            {
                mTemp = (Modbus.u8NumberRegHigh<<8)|(Modbus.u8NumberRegLow);
                if (Modbus.u8BuffRead[2] == (mTemp*2))
                {
                    switch(Modbus.u8SlaveID - sModbusManager.sBMManager[sModbusManager.u8BMCurrentIndex].u32BMSlaveOffset)
                    {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    case 9:
                    case 10:
                    case 11:
                    case 12:
                    case 13:
                    case 14:
                    case 15:
                    case 16:
                    {
                        Extract_Holding_Regs_Data_BM();
                    }
                    break;

                    default:
                        break;
                    }
                }
                else
                {
                    Modbus.u8MosbusEn = 0;
                    return -3;
                }
            }
            break;
            case _EXCEPTION_READ_HOLDING:
            {
              Modbus.u8MosbusEn = 0;
              return -4;
            }
            break;
            default:
                break;
            }

            Modbus.u8MosbusEn = 0;
        }
        else
        {
            Modbus.u8MosbusEn = 0;
            return -2;
        }
    }
    else
    {
        Modbus.u8MosbusEn = 0;
        return -1;
    }
    return 1;
}
#endif

int8_t RS4851_Check_Respond_Setting_Data (void)
{
    uint16_t	mTemp = 0;

    if(Modbus.u8MosbusEn==2)
    {
        Modbus.u8CRCHighRead = Modbus.u8BuffRead[Modbus.u8ByteCount-2];
        Modbus.u8CRCLowRead  = Modbus.u8BuffRead[Modbus.u8ByteCount-1];

        ModbusCRC(&Modbus.u8BuffRead[0],Modbus.u8ByteCount-2);

        if(Modbus.u8CRCHigh == Modbus.u8CRCHighRead && Modbus.u8CRCLow==Modbus.u8CRCLowRead)
        {
            Modbus.u8FunctionCode 	= Modbus.u8BuffRead[1];

            switch(Modbus.u8FunctionCode)
            {
            case _PRESET_MULTIPLE_REGS:
            {
                mTemp = (Modbus.u8NumberRegHigh<<8)|(Modbus.u8NumberRegLow);
                if (Modbus.u8BuffRead[2] == (mTemp*2))
                {
                    
                }
                else
                {
                    Modbus.u8MosbusEn = 0;
                    return -3;
                }
            }
            break;
            case _EXCEPTION_WRITE_MULTI_REG:
            {
              Modbus.u8MosbusEn = 0;
              return -4;
            }
            break;
            default:
                break;
            }

            Modbus.u8MosbusEn = 0;
        }
        else
        {
            Modbus.u8MosbusEn = 0;
            return -2;
        }
    }
    else
    {
        Modbus.u8MosbusEn = 0;
        return -1;
    }
    return 1;
}

// lenh 01, doc nhieu bit (0x)
void Read_Coil_Status_Query (uint8_t slaveAddr, uint16_t startingAddr, uint16_t noPoint)
{
    Modbus.u8SlaveID = slaveAddr;
    Modbus.u8FunctionCode = 0x01;
    Modbus.u8StartHigh = (uint8_t)(startingAddr>>8);
    Modbus.u8StartLow = (uint8_t)(startingAddr);
    Modbus.u8NumberRegHigh = (uint8_t)(noPoint>>8);
    Modbus.u8NumberRegLow = (uint8_t)(noPoint);

    Modbus.u8BuffWrite[0] = Modbus.u8SlaveID;
    Modbus.u8BuffWrite[1] = Modbus.u8FunctionCode;
    Modbus.u8BuffWrite[2] = Modbus.u8StartHigh;
    Modbus.u8BuffWrite[3] = Modbus.u8StartLow;
    Modbus.u8BuffWrite[4] = Modbus.u8NumberRegHigh;
    Modbus.u8BuffWrite[5] = Modbus.u8NumberRegLow;
    
    if((noPoint%8)==0)
    {
      noPoint = noPoint/8;
    }
    else 
    {
      noPoint = (noPoint/8) + 1;
    }
    Modbus.u8NumberRegHigh = (uint8_t)(noPoint>>8);
    Modbus.u8NumberRegLow = (uint8_t)(noPoint);

    ModbusCRC(&Modbus.u8BuffWrite[0],6);

    Modbus.u8BuffWrite[6] = Modbus.u8CRCHigh;
    Modbus.u8BuffWrite[7] = Modbus.u8CRCLow;
    GPIO_SetPinsOutput(GPIOE,1u << 3u);
    GPIO_SetPinsOutput(GPIOE,1u << 2u);

    UART_WriteBlocking(RS4851T_UART, Modbus.u8BuffWrite, 8);
//    thao++;
    delay_deepsea();
    GPIO_ClearPinsOutput(GPIOE,1u << 3u);
    GPIO_ClearPinsOutput(GPIOE,1u << 2u);
}

// lenh 02, doc nhieu bit (1x), nhanh hon
void Read_Input_Status_Query_fast (uint8_t slaveAddr, uint16_t startingAddr, uint16_t noPoint)
{
    Modbus.u8SlaveID = slaveAddr;
    Modbus.u8FunctionCode = 0x02;
    Modbus.u8StartHigh = (uint8_t)(startingAddr>>8);
    Modbus.u8StartLow = (uint8_t)(startingAddr);
    Modbus.u8NumberRegHigh = (uint8_t)(noPoint>>8);
    Modbus.u8NumberRegLow = (uint8_t)(noPoint);

    Modbus.u8BuffWrite[0] = Modbus.u8SlaveID;
    Modbus.u8BuffWrite[1] = Modbus.u8FunctionCode;
    Modbus.u8BuffWrite[2] = Modbus.u8StartHigh;
    Modbus.u8BuffWrite[3] = Modbus.u8StartLow;
    Modbus.u8BuffWrite[4] = Modbus.u8NumberRegHigh;
    Modbus.u8BuffWrite[5] = Modbus.u8NumberRegLow;
    
    if((noPoint%8)==0)
    {
      noPoint = noPoint/8;
    }
    else 
    {
      noPoint = (noPoint/8) + 1;
    }
    Modbus.u8NumberRegHigh = (uint8_t)(noPoint>>8);
    Modbus.u8NumberRegLow = (uint8_t)(noPoint);

    ModbusCRC(&Modbus.u8BuffWrite[0],6);

    Modbus.u8BuffWrite[6] = Modbus.u8CRCHigh;
    Modbus.u8BuffWrite[7] = Modbus.u8CRCLow;
    GPIO_SetPinsOutput(GPIOE,1u << 3u);
    GPIO_SetPinsOutput(GPIOE,1u << 2u);

    UART_WriteBlocking(RS4851T_UART, Modbus.u8BuffWrite, 8);
//    thao++;
    delay_deepsea();
    GPIO_ClearPinsOutput(GPIOE,1u << 3u);
    GPIO_ClearPinsOutput(GPIOE,1u << 2u);
}
void Read_Input_Status_Query_coslight (uint8_t slaveAddr, uint16_t startingAddr, uint16_t noPoint)
{
    Modbus.u8SlaveID = slaveAddr;
    Modbus.u8FunctionCode = 0x02;
    Modbus.u8StartHigh = (uint8_t)(startingAddr>>8);
    Modbus.u8StartLow = (uint8_t)(startingAddr);
    Modbus.u8NumberRegHigh = (uint8_t)(noPoint>>8);
    Modbus.u8NumberRegLow = (uint8_t)(noPoint);

    Modbus.u8BuffWrite[0] = Modbus.u8SlaveID;
    Modbus.u8BuffWrite[1] = Modbus.u8FunctionCode;
    Modbus.u8BuffWrite[2] = Modbus.u8StartHigh;
    Modbus.u8BuffWrite[3] = Modbus.u8StartLow;
    Modbus.u8BuffWrite[4] = Modbus.u8NumberRegHigh;
    Modbus.u8BuffWrite[5] = Modbus.u8NumberRegLow;
    
    if((noPoint%8)==0)
    {
      noPoint = noPoint/8;
    }
    else 
    {
      noPoint = (noPoint/8) + 1;
    }
    Modbus.u8NumberRegHigh = (uint8_t)(noPoint>>8);
    Modbus.u8NumberRegLow = (uint8_t)(noPoint);

    ModbusCRC(&Modbus.u8BuffWrite[0],6);

    Modbus.u8BuffWrite[6] = Modbus.u8CRCHigh;
    Modbus.u8BuffWrite[7] = Modbus.u8CRCLow;
    GPIO_SetPinsOutput(GPIOE,1u << 3u);
    GPIO_SetPinsOutput(GPIOE,1u << 2u);

    UART_WriteBlocking(RS4851T_UART, Modbus.u8BuffWrite, 8);
//    thao++;
    delay_deepsea_coslight();
    GPIO_ClearPinsOutput(GPIOE,1u << 3u);
    GPIO_ClearPinsOutput(GPIOE,1u << 2u);
}
// lenh 02, doc nhieu bit (1x)
void Read_Input_Status_Query (uint8_t slaveAddr, uint16_t startingAddr, uint16_t noPoint)
{
    Modbus.u8SlaveID = slaveAddr;
    Modbus.u8FunctionCode = 0x02;
    Modbus.u8StartHigh = (uint8_t)(startingAddr>>8);
    Modbus.u8StartLow = (uint8_t)(startingAddr);
    Modbus.u8NumberRegHigh = (uint8_t)(noPoint>>8);
    Modbus.u8NumberRegLow = (uint8_t)(noPoint);

    Modbus.u8BuffWrite[0] = Modbus.u8SlaveID;
    Modbus.u8BuffWrite[1] = Modbus.u8FunctionCode;
    Modbus.u8BuffWrite[2] = Modbus.u8StartHigh;
    Modbus.u8BuffWrite[3] = Modbus.u8StartLow;
    Modbus.u8BuffWrite[4] = Modbus.u8NumberRegHigh;
    Modbus.u8BuffWrite[5] = Modbus.u8NumberRegLow;
    
    if((noPoint%8)==0)
    {
      noPoint = noPoint/8;
    }
    else 
    {
      noPoint = (noPoint/8) + 1;
    }
    Modbus.u8NumberRegHigh = (uint8_t)(noPoint>>8);
    Modbus.u8NumberRegLow = (uint8_t)(noPoint);

    ModbusCRC(&Modbus.u8BuffWrite[0],6);

    Modbus.u8BuffWrite[6] = Modbus.u8CRCHigh;
    Modbus.u8BuffWrite[7] = Modbus.u8CRCLow;
    GPIO_SetPinsOutput(GPIOE,1u << 3u);
    GPIO_SetPinsOutput(GPIOE,1u << 2u);

    UART_WriteBlocking(RS4851T_UART, Modbus.u8BuffWrite, 8);
//    thao++;
    delay2();
    GPIO_ClearPinsOutput(GPIOE,1u << 3u);
    GPIO_ClearPinsOutput(GPIOE,1u << 2u);
}
//for yada======================================================================
void Read_Holding_Regs_Query_yada (uint8_t slaveAddr, uint16_t startingAddr, uint16_t noPoint)
{
    Modbus.u8SlaveID = slaveAddr;
    Modbus.u8FunctionCode = 0x03;
    Modbus.u8StartHigh = (uint8_t)(startingAddr>>8);
    Modbus.u8StartLow = (uint8_t)(startingAddr);
    Modbus.u8NumberRegHigh = (uint8_t)(noPoint>>8);
    Modbus.u8NumberRegLow = (uint8_t)(noPoint);

    Modbus.u8BuffWrite[0] = Modbus.u8SlaveID;
    Modbus.u8BuffWrite[1] = Modbus.u8FunctionCode;
    Modbus.u8BuffWrite[2] = Modbus.u8StartHigh;
    Modbus.u8BuffWrite[3] = Modbus.u8StartLow;
    Modbus.u8BuffWrite[4] = Modbus.u8NumberRegHigh;
    Modbus.u8BuffWrite[5] = Modbus.u8NumberRegLow;

    ModbusCRC(&Modbus.u8BuffWrite[0],6);

    Modbus.u8BuffWrite[6] = Modbus.u8CRCHigh;
    Modbus.u8BuffWrite[7] = Modbus.u8CRCLow;
    GPIO_SetPinsOutput(GPIOE,1u << 3u);
    GPIO_SetPinsOutput(GPIOE,1u << 2u);

    UART_WriteBlocking(RS4851T_UART, Modbus.u8BuffWrite, 8);
//    thao++;
    //delay2();// old
    delay2_yada(); //new 
    GPIO_ClearPinsOutput(GPIOE,1u << 3u);
    GPIO_ClearPinsOutput(GPIOE,1u << 2u);
}
//===============================================================================
void Read_Holding_Regs_Query_coslight (uint8_t slaveAddr, uint16_t startingAddr, uint16_t noPoint)
{
    Modbus.u8SlaveID = slaveAddr;
    Modbus.u8FunctionCode = 0x03;
    Modbus.u8StartHigh = (uint8_t)(startingAddr>>8);
    Modbus.u8StartLow = (uint8_t)(startingAddr);
    Modbus.u8NumberRegHigh = (uint8_t)(noPoint>>8);
    Modbus.u8NumberRegLow = (uint8_t)(noPoint);

    Modbus.u8BuffWrite[0] = Modbus.u8SlaveID;
    Modbus.u8BuffWrite[1] = Modbus.u8FunctionCode;
    Modbus.u8BuffWrite[2] = Modbus.u8StartHigh;
    Modbus.u8BuffWrite[3] = Modbus.u8StartLow;
    Modbus.u8BuffWrite[4] = Modbus.u8NumberRegHigh;
    Modbus.u8BuffWrite[5] = Modbus.u8NumberRegLow;

    ModbusCRC(&Modbus.u8BuffWrite[0],6);

    Modbus.u8BuffWrite[6] = Modbus.u8CRCHigh;
    Modbus.u8BuffWrite[7] = Modbus.u8CRCLow;
    GPIO_SetPinsOutput(GPIOE,1u << 3u);
    GPIO_SetPinsOutput(GPIOE,1u << 2u);

    UART_WriteBlocking(RS4851T_UART, Modbus.u8BuffWrite, 8);
//    thao++;
    delay_deepsea_coslight();// old
    //delay2_yada(); //new 
    GPIO_ClearPinsOutput(GPIOE,1u << 3u);
    GPIO_ClearPinsOutput(GPIOE,1u << 2u);
}
// lenh 03, doc nhieu byte (4x)
void Read_Holding_Regs_Query (uint8_t slaveAddr, uint16_t startingAddr, uint16_t noPoint)
{
    Modbus.u8SlaveID = slaveAddr;
    Modbus.u8FunctionCode = 0x03;
    Modbus.u8StartHigh = (uint8_t)(startingAddr>>8);
    Modbus.u8StartLow = (uint8_t)(startingAddr);
    Modbus.u8NumberRegHigh = (uint8_t)(noPoint>>8);
    Modbus.u8NumberRegLow = (uint8_t)(noPoint);

    Modbus.u8BuffWrite[0] = Modbus.u8SlaveID;
    Modbus.u8BuffWrite[1] = Modbus.u8FunctionCode;
    Modbus.u8BuffWrite[2] = Modbus.u8StartHigh;
    Modbus.u8BuffWrite[3] = Modbus.u8StartLow;
    Modbus.u8BuffWrite[4] = Modbus.u8NumberRegHigh;
    Modbus.u8BuffWrite[5] = Modbus.u8NumberRegLow;

    ModbusCRC(&Modbus.u8BuffWrite[0],6);

    Modbus.u8BuffWrite[6] = Modbus.u8CRCHigh;
    Modbus.u8BuffWrite[7] = Modbus.u8CRCLow;
    GPIO_SetPinsOutput(GPIOE,1u << 3u);
    GPIO_SetPinsOutput(GPIOE,1u << 2u);

    UART_WriteBlocking(RS4851T_UART, Modbus.u8BuffWrite, 8);
//    thao++;
    delay2();// old
    //delay2_yada(); //new 
    GPIO_ClearPinsOutput(GPIOE,1u << 3u);
    GPIO_ClearPinsOutput(GPIOE,1u << 2u);
}

// lenh 03, doc nhieu byte (4x), thoi gian delay nho hon
void Read_Holding_Regs_Query_Deepsea (uint8_t slaveAddr, uint16_t startingAddr, uint16_t noPoint)
{
    Modbus.u8SlaveID = slaveAddr;
    Modbus.u8FunctionCode = 0x03;
    Modbus.u8StartHigh = (uint8_t)(startingAddr>>8);
    Modbus.u8StartLow = (uint8_t)(startingAddr);
    Modbus.u8NumberRegHigh = (uint8_t)(noPoint>>8);
    Modbus.u8NumberRegLow = (uint8_t)(noPoint);

    Modbus.u8BuffWrite[0] = Modbus.u8SlaveID;
    Modbus.u8BuffWrite[1] = Modbus.u8FunctionCode;
    Modbus.u8BuffWrite[2] = Modbus.u8StartHigh;
    Modbus.u8BuffWrite[3] = Modbus.u8StartLow;
    Modbus.u8BuffWrite[4] = Modbus.u8NumberRegHigh;
    Modbus.u8BuffWrite[5] = Modbus.u8NumberRegLow;

    ModbusCRC(&Modbus.u8BuffWrite[0],6);

    Modbus.u8BuffWrite[6] = Modbus.u8CRCHigh;
    Modbus.u8BuffWrite[7] = Modbus.u8CRCLow;
    GPIO_SetPinsOutput(GPIOE,1u << 3u);
    GPIO_SetPinsOutput(GPIOE,1u << 2u);

    UART_WriteBlocking(RS4851T_UART, Modbus.u8BuffWrite, 8);
//    thao++;
    delay_deepsea();
    GPIO_ClearPinsOutput(GPIOE,1u << 3u);
    GPIO_ClearPinsOutput(GPIOE,1u << 2u);
}

// lenh 04, doc nhieu byte (3x), nhanh hon
void Read_Input_Regs_Query_fast (uint8_t slaveAddr, uint16_t startingAddr, uint16_t noPoint)
{
    Modbus.u8SlaveID = slaveAddr;
    Modbus.u8FunctionCode = 0x04;
    Modbus.u8StartHigh = (uint8_t)(startingAddr>>8);
    Modbus.u8StartLow = (uint8_t)(startingAddr);
    Modbus.u8NumberRegHigh = (uint8_t)(noPoint>>8);
    Modbus.u8NumberRegLow = (uint8_t)(noPoint);

    Modbus.u8BuffWrite[0] = Modbus.u8SlaveID;
    Modbus.u8BuffWrite[1] = Modbus.u8FunctionCode;
    Modbus.u8BuffWrite[2] = Modbus.u8StartHigh;
    Modbus.u8BuffWrite[3] = Modbus.u8StartLow;
    Modbus.u8BuffWrite[4] = Modbus.u8NumberRegHigh;
    Modbus.u8BuffWrite[5] = Modbus.u8NumberRegLow;

    ModbusCRC(&Modbus.u8BuffWrite[0],6);

    Modbus.u8BuffWrite[6] = Modbus.u8CRCHigh;
    Modbus.u8BuffWrite[7] = Modbus.u8CRCLow;
    GPIO_SetPinsOutput(GPIOE,1u << 3u);
    GPIO_SetPinsOutput(GPIOE,1u << 2u);
    
    UART_WriteBlocking(RS4851T_UART, Modbus.u8BuffWrite, 8);
    
    delay_deepsea();
    GPIO_ClearPinsOutput(GPIOE,1u << 3u);
    GPIO_ClearPinsOutput(GPIOE,1u << 2u);
}
//thanhcm3 fix===================================================================
void Read_Input_Regs_Query_coslight (uint8_t slaveAddr, uint16_t startingAddr, uint16_t noPoint)
{
    Modbus.u8SlaveID = slaveAddr;
    Modbus.u8FunctionCode = 0x04;
    Modbus.u8StartHigh = (uint8_t)(startingAddr>>8);
    Modbus.u8StartLow = (uint8_t)(startingAddr);
    Modbus.u8NumberRegHigh = (uint8_t)(noPoint>>8);
    Modbus.u8NumberRegLow = (uint8_t)(noPoint);

    Modbus.u8BuffWrite[0] = Modbus.u8SlaveID;
    Modbus.u8BuffWrite[1] = Modbus.u8FunctionCode;
    Modbus.u8BuffWrite[2] = Modbus.u8StartHigh;
    Modbus.u8BuffWrite[3] = Modbus.u8StartLow;
    Modbus.u8BuffWrite[4] = Modbus.u8NumberRegHigh;
    Modbus.u8BuffWrite[5] = Modbus.u8NumberRegLow;

    ModbusCRC(&Modbus.u8BuffWrite[0],6);

    Modbus.u8BuffWrite[6] = Modbus.u8CRCHigh;
    Modbus.u8BuffWrite[7] = Modbus.u8CRCLow;
    GPIO_SetPinsOutput(GPIOE,1u << 3u);
    GPIO_SetPinsOutput(GPIOE,1u << 2u);
    
    UART_WriteBlocking(RS4851T_UART, Modbus.u8BuffWrite, 8);
    
    delay_deepsea_coslight();
    GPIO_ClearPinsOutput(GPIOE,1u << 3u);
    GPIO_ClearPinsOutput(GPIOE,1u << 2u);
    //delay_deepsea();
    
}
// lenh 04, doc nhieu byte (3x)
void Read_Input_Regs_Query (uint8_t slaveAddr, uint16_t startingAddr, uint16_t noPoint)
{
    Modbus.u8SlaveID = slaveAddr;
    Modbus.u8FunctionCode = 0x04;
    Modbus.u8StartHigh = (uint8_t)(startingAddr>>8);
    Modbus.u8StartLow = (uint8_t)(startingAddr);
    Modbus.u8NumberRegHigh = (uint8_t)(noPoint>>8);
    Modbus.u8NumberRegLow = (uint8_t)(noPoint);

    Modbus.u8BuffWrite[0] = Modbus.u8SlaveID;
    Modbus.u8BuffWrite[1] = Modbus.u8FunctionCode;
    Modbus.u8BuffWrite[2] = Modbus.u8StartHigh;
    Modbus.u8BuffWrite[3] = Modbus.u8StartLow;
    Modbus.u8BuffWrite[4] = Modbus.u8NumberRegHigh;
    Modbus.u8BuffWrite[5] = Modbus.u8NumberRegLow;

    ModbusCRC(&Modbus.u8BuffWrite[0],6);

    Modbus.u8BuffWrite[6] = Modbus.u8CRCHigh;
    Modbus.u8BuffWrite[7] = Modbus.u8CRCLow;
    GPIO_SetPinsOutput(GPIOE,1u << 3u);
    GPIO_SetPinsOutput(GPIOE,1u << 2u);
    
    UART_WriteBlocking(RS4851T_UART, Modbus.u8BuffWrite, 8);
    
    delay_deepsea();
    GPIO_ClearPinsOutput(GPIOE,1u << 3u);
    GPIO_ClearPinsOutput(GPIOE,1u << 2u);
    //delay_deepsea();
    
}

// lenh 17, doc thong tin san xuat cua acqui narada
void Read_Product_Query (uint8_t slaveAddr)
{
    Modbus.u8SlaveID = slaveAddr;
    Modbus.u8FunctionCode = 0x11;
    
    Modbus.u8BuffWrite[0] = Modbus.u8SlaveID;
    Modbus.u8BuffWrite[1] = Modbus.u8FunctionCode;
    Modbus.u8BuffWrite[2] = 0x00;
    Modbus.u8BuffWrite[3] = 0x00;
    Modbus.u8BuffWrite[4] = 0x00;
    Modbus.u8BuffWrite[5] = 0x00;

    ModbusCRC(&Modbus.u8BuffWrite[0],6);

    Modbus.u8BuffWrite[6] = Modbus.u8CRCHigh;
    Modbus.u8BuffWrite[7] = Modbus.u8CRCLow;
    GPIO_SetPinsOutput(GPIOE,1u << 3u);
    GPIO_SetPinsOutput(GPIOE,1u << 2u);
    
    UART_WriteBlocking(RS4851T_UART, Modbus.u8BuffWrite, 8);
    
    delay2();
    GPIO_ClearPinsOutput(GPIOE,1u << 3u);
    GPIO_ClearPinsOutput(GPIOE,1u << 2u);
}

void Write_Single_Reg (uint8_t slaveAddr, uint16_t regAddr, uint16_t writeVal) // lenh 06, ghi 1 thanh ghi
{
    Modbus.u8SlaveID = slaveAddr;
    Modbus.u8FunctionCode = 0x06;
    Modbus.u8StartHigh = (uint8_t)(regAddr>>8);
    Modbus.u8StartLow = (uint8_t)(regAddr);
    Modbus.u8NumberRegHigh = (uint8_t)(writeVal>>8);
    Modbus.u8NumberRegLow = (uint8_t)(writeVal);

    Modbus.u8BuffWrite[0] = Modbus.u8SlaveID;
    Modbus.u8BuffWrite[1] = Modbus.u8FunctionCode;
    Modbus.u8BuffWrite[2] = Modbus.u8StartHigh;
    Modbus.u8BuffWrite[3] = Modbus.u8StartLow;
    Modbus.u8BuffWrite[4] = Modbus.u8NumberRegHigh;
    Modbus.u8BuffWrite[5] = Modbus.u8NumberRegLow;

    ModbusCRC(&Modbus.u8BuffWrite[0],6);

    Modbus.u8BuffWrite[6] = Modbus.u8CRCHigh;
    Modbus.u8BuffWrite[7] = Modbus.u8CRCLow;

    GPIO_SetPinsOutput(GPIOE,1u << 3u);
    GPIO_SetPinsOutput(GPIOE,1u << 2u);
    
    UART_WriteBlocking(RS4851T_UART, Modbus.u8BuffWrite, 8);
    delay2();
    GPIO_ClearPinsOutput(GPIOE,1u << 3u);
    GPIO_ClearPinsOutput(GPIOE,1u << 2u);
}

void Write_Multi_Reg (uint8_t slaveAddr, uint16_t regAddr, uint8_t *writeBuff, uint8_t size)    // size: so thanh ghi 16bit can set gia tri
{
  uint8_t i,temp[256];
  uint8_t numOfByte;
  if (size <= 127)
  {
    numOfByte = size * 2;       // so thanh ghi 8 bit
  }
  else
  {
    numOfByte = 2;
  }
    Modbus.u8SlaveID = slaveAddr;
    Modbus.u8FunctionCode = 0x10;
    Modbus.u8StartHigh = (uint8_t)(regAddr>>8); // Address high
    Modbus.u8StartLow = (uint8_t)(regAddr);     // Address Low
    Modbus.u8NumberRegHigh = (uint8_t)(size>>8);        
    Modbus.u8NumberRegLow = (uint8_t)(size);

    Modbus.u8BuffWrite[0] = Modbus.u8SlaveID;
    Modbus.u8BuffWrite[1] = Modbus.u8FunctionCode;
    Modbus.u8BuffWrite[2] = Modbus.u8StartHigh;
    Modbus.u8BuffWrite[3] = Modbus.u8StartLow;
    Modbus.u8BuffWrite[4] = Modbus.u8NumberRegHigh;
    Modbus.u8BuffWrite[5] = Modbus.u8NumberRegLow;
    Modbus.u8BuffWrite[6] = numOfByte;
    
    for(i = 0; i < numOfByte; i++)
    {
      temp[i] = *(writeBuff+i);
      Modbus.u8BuffWrite[7 + i] = temp[i];
    }  

    ModbusCRC(&Modbus.u8BuffWrite[0],7 + numOfByte);

    Modbus.u8BuffWrite[7 + numOfByte] = Modbus.u8CRCHigh;
    Modbus.u8BuffWrite[8 + numOfByte] = Modbus.u8CRCLow;    

    switch(sModbusManager.SettingCommand)
    {
    default:
      {        
        UART_Deinit(RS4851R_UART);
        Init_RS485_MODBUS_UART();
            
        GPIO_SetPinsOutput(GPIOE,1u << 3u);
        GPIO_SetPinsOutput(GPIOE,1u << 2u);

        UART_WriteBlocking(RS4851T_UART, Modbus.u8BuffWrite, 9 + numOfByte); 
        delay2();
//        delay2();
        GPIO_ClearPinsOutput(GPIOE,1u << 3u);
        GPIO_ClearPinsOutput(GPIOE,1u << 2u);   
      }break;
    };
}


void SetVar_Time(uint8_t slaveAddr, uint16_t regAddr,TimeFormat setTime)
{
  uint8_t u8DataBuff[16];
  u8DataBuff[0] = (uint8_t) (setTime.min);
  u8DataBuff[1] = (uint8_t) (setTime.sec);
  u8DataBuff[2] = (uint8_t) (setTime.date);// - 1); 
  u8DataBuff[3] = (uint8_t) (setTime.hour);
  u8DataBuff[4] = 0;
  u8DataBuff[5] = (uint8_t) (setTime.month);// - 1);
  u8DataBuff[6] = (uint8_t) ((setTime.year) >> 8);
  u8DataBuff[7] = (uint8_t) (setTime.year);
  Write_Multi_Reg (slaveAddr, regAddr, &u8DataBuff[0], 4);
}

void SetVar_f32(uint8_t slaveAddr, uint16_t regAddr, float val)
{
  uint8_t i,u8DataBuff[4];
  uint8_t u8DataBuff_inverse[4];
  memcpy(&u8DataBuff_inverse[0],&val,4);
  for(i = 0; i < 4; i++)
  {
    u8DataBuff[i] = u8DataBuff_inverse[3-i];
  }
  Write_Multi_Reg (slaveAddr, regAddr, &u8DataBuff[0], 2);
}

// ma lenh 16: gui nhieu byte (4x), data 32 bit
void SetVar_i32(uint8_t slaveAddr, uint16_t regAddr, int32_t val)
{
  uint8_t u8DataBuff[4];
  u8DataBuff[0] = (uint8_t) (val >> 24);
  u8DataBuff[1] = (uint8_t) (val >> 16);
  u8DataBuff[2] = (uint8_t) (val >> 8);
  u8DataBuff[3] = (uint8_t) (val);
  Write_Multi_Reg (slaveAddr, regAddr, &u8DataBuff[0], 2);      
}

void SetVar_i16(uint8_t slaveAddr, uint16_t regAddr, uint16_t val)
{
  uint8_t u8DataBuff[2];
  u8DataBuff[0] = (uint8_t) (val >> 8);
  u8DataBuff[1] = (uint8_t) (val);
  Write_Multi_Reg (slaveAddr, regAddr, &u8DataBuff[0], 1);      
}


void ModbusCRC(unsigned char *str, unsigned char len)
{
    unsigned char temp;
    Modbus.u8CRCHigh = 0xFF;
    Modbus.u8CRCLow = 0xFF;

    while( len--)
    {
        temp = *str++ ^ Modbus.u8CRCHigh;
        Modbus.u8CRCHigh = CRCHighTable[temp] ^ Modbus.u8CRCLow;
        Modbus.u8CRCLow  = CRCLowTable[temp];
    }
}

void delay2(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < 4500; ++i)//20000
    {
        __asm("NOP"); /* delay */
    }
}

void delay2_yada(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < 20000; ++i)//20000
    {
        __asm("NOP"); /* delay */
    }
}

void delay_deepsea(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < 2500; ++i)//20000
    {
        __asm("NOP"); /* delay */
    }
}
void delay_deepsea_coslight(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < 2000; ++i)//20000
    {
        __asm("NOP"); /* delay */
    }
}

void modbusParsebit(uint8_t *buf, uint8_t *varType1, uint32_t pos, uint8_t coil)
{
    uint8_t temp;
    switch(coil)
    {
    case 0:
    {
        temp = buf[3 + pos];
        temp = temp&0x01;
        *varType1 = temp;
    }
    break;
    case 1:
    {
        temp = buf[3 + pos];
        temp = (temp>>1)&0x01;
        *varType1 = temp;
    }
    break;
    case 2:
    {
        temp = buf[3 + pos];
        temp = (temp>>2)&0x01;
        *varType1 = temp;
    }
    break;
    case 3:
    {
        temp = buf[3 + pos];
        temp = (temp>>3)&0x01;
        *varType1 = temp;
    }
    break;
    case 4:
    {
        temp = buf[3 + pos];
        temp = (temp>>4)&0x01;
        *varType1 = temp;
    }
    break;
    case 5:
    {
        temp = buf[3 + pos];
        temp = (temp>>5)&0x01;
        *varType1 = temp;
    }
    break;
    case 6:
    {
        temp = buf[3 + pos];
        temp = (temp>>6)&0x01;
        *varType1 = temp;
    }
    break;
    case 7:
    {
        temp = buf[3 + pos];
        temp = (temp>>7)&0x01;
        *varType1 = temp;
    }
    break;
    }
    
}

void modbusParseInfo(uint8_t *buf, uint8_t *varType1,uint16_t *varType2,uint32_t *varType3,uint32_t pos, uint8_t type)
{
    uint8_t u8Var;
    uint16_t u16Var;
    uint32_t u32Var;
    switch(type)
    {
    case 8:// 8-byte Integer
    {
        u8Var = buf[3 + pos * 2];
        *varType1 = u8Var;
    }
    break;
    case 16:// 16-byte Integer
    {
        u16Var = (buf[3 + pos * 2] << 8) | buf[3 + pos * 2 + 1];
        *varType2 = u16Var;
    }
    break;
    case 32:// 32-byte Integer
    {
        u32Var = (buf[3 + pos * 2] << 24) | (buf[3 + pos * 2 + 1] << 16) | (buf[3 + pos * 2 + 2] << 8) |(buf[3 + pos * 2 + 3]);
        *varType3 = u32Var;
        
    }
    break;
    };
}

void modbusParseInfoInverse(uint8_t *buf, uint8_t *varType1,uint16_t *varType2,uint32_t *varType3,uint32_t pos, uint8_t type)
{
    uint8_t u8Var;
    uint16_t u16Var;
    uint32_t u32Var;
    switch(type)
    {
    case 8:// 8-byte Integer
    {
        u8Var = buf[3 + pos * 2];
        *varType1 = u8Var;
    }
    break;
    case 16:// 16-byte Integer
    {
        u16Var = (buf[3 + pos * 2 + 1] << 8) | buf[3 + pos * 2];
        *varType2 = u16Var;
    }
    break;
    case 32:// 32-byte Integer
    {
        u32Var = (buf[3 + pos * 2 + 2] << 24) | (buf[3 + pos * 2 + 3] << 16) | (buf[3 + pos * 2 + 0] << 8) |(buf[3 + pos * 2 + 1]);
        *varType3 = u32Var;
    }
    break;
    };
}

void modbusParseInfoSign(uint8_t *buf, int8_t *varType1,int16_t *varType2,int32_t *varType3,uint32_t pos, uint8_t type)
{
    int8_t u8Var;
    int16_t u16Var;
    int32_t u32Var;
    switch(type)
    {
    case 8:// 8-byte Integer
    {
        u8Var = buf[3 + pos * 2];
        *varType1 = u8Var;
    }
    break;
    case 16:// 16-byte Integer
    {
        u16Var = (buf[3 + pos * 2] << 8) | buf[3 + pos * 2 + 1];
        *varType2 = u16Var;
    }
    break;
    case 32:// 32-byte Integer
    {
        u32Var = (buf[3 + pos * 2] << 24) | (buf[3 + pos * 2 + 1] << 16) | (buf[3 + pos * 2 + 2] << 8) |(buf[3 + pos * 2 + 3]);
        *varType3 = u32Var;
    }
    break;
    };
}

void modbusParseInfoSignInverse(uint8_t *buf, int8_t *varType1,int16_t *varType2,int32_t *varType3,uint32_t pos, uint8_t type)
{
    int8_t u8Var;
    int16_t u16Var;
    int32_t u32Var;
    switch(type)
    {
    case 8:// 8-byte Integer
    {
        u8Var = buf[3 + pos * 2];
        *varType1 = u8Var;
    }
    break;
    case 16:// 16-byte Integer
    {
        u16Var = (buf[3 + pos * 2 + 1] << 8) | buf[3 + pos * 2];
        *varType2 = u16Var;
    }
    break;
    case 32:// 32-byte Integer
    {
        u32Var = (buf[3 + pos * 2 + 2] << 24) | (buf[3 + pos * 2 + 3] << 16) | (buf[3 + pos * 2 + 0] << 8) |(buf[3 + pos * 2 + 1]);
        *varType3 = u32Var;
    }
    break;
    };
}

void modbusParseInfof(uint8_t *buf, float *varType4, uint32_t pos)
{
  uint32_t u32Temp = 0;
  float fVar;
  modbusParseInfo(buf, 0, 0, &u32Temp, pos, 32);
  memcpy(&fVar,&u32Temp,4);
  *varType4 = fVar;
}


void modbusParseInfofInverse(uint8_t *buf, float *varType4, uint32_t pos)
{
  uint32_t u32Temp = 0;
  float fVar;
  
  u32Temp = (buf[3 + pos * 2 + 2] << 24) | (buf[3 + pos * 2 + 3] << 16) | (buf[3 + pos * 2 + 0] << 8) |(buf[3 + pos * 2 + 1]);
  memcpy(&fVar,&u32Temp,4);
  *varType4 = fVar;
}

void modbusParseInfofInverse2(uint8_t *buf, float *varType4, uint32_t pos)
{
  uint32_t u32Temp = 0;
  float fVar;
  
  u32Temp = (buf[3 + pos * 2] << 24) | (buf[3 + pos * 2 + 1] << 16) | (buf[3 + pos * 2 + 2] << 8) |(buf[3 + pos * 2 + 3]);
  memcpy(&fVar,&u32Temp,4);
  *varType4 = fVar;
}

void modbusParseTime(uint8_t *buf, TimeFormat *setTime, uint32_t pos)
{
  setTime->min = buf[3 + pos * 2];
  setTime->sec = buf[3 + pos * 2 + 1];
  setTime->date = buf[3 + pos * 2 + 2];
  setTime->hour = buf[3 + pos * 2 + 3];
  setTime->month = buf[3 + pos * 2 + 5];
  setTime->year = (buf[3 + pos * 2 + 6] << 8) | (buf[3 + pos * 2 + 7]) ;  
}

void modbusParseInfoString(uint8_t *buf, uint8_t *dstBuf, uint8_t numOfByte, uint32_t pos)
{
  uint16_t temp;
  for (uint16_t i = 0; i < numOfByte; i++)
  {
    modbusParseInfo(buf, 0, &temp,0,pos + i,16);
    dstBuf[i] = (uint8_t) temp;
    if(dstBuf[i] == 0x20)
    {
      dstBuf[i] = 0;
      break;
    }
  }
}

void modbusParseInfoString2(uint8_t *buf, uint8_t *dstBuf, uint8_t numOfByte, uint32_t pos)     // doc cac thanh ghi 16bit chua 2 ky tu ASCII
{
  uint16_t temp;
  for (uint16_t i = 0; i < numOfByte; i++)
  {
    modbusParseInfo(buf, 0, &temp,0,pos + i,16);
    dstBuf[i*2+1] = (uint8_t) temp;
    dstBuf[i*2] = (uint8_t) (temp>>8);
    if(dstBuf[i] == 0x20)
    {
      dstBuf[i] = 0;
      break;
    }
  }
}
void modbusParseInfobcd(uint8_t *buf, uint8_t *varType1,uint16_t *varType2,uint32_t *varType3,uint32_t pos, uint8_t type){
    uint8_t u8Var;
    uint16_t u16Var;
    uint32_t u32Var;
    uint8_t u8temp1,u8temp2,u8temp3,u8temp4;
    switch(type)
    {
    case 8:// 8-byte bcd
    {
      u8Var = (buf[3 + pos * 2]/0x10*0x0A)+(buf[3 + pos * 2]%0x10);
      *varType1 = u8Var;
    }
    break;
    case 16:// 16-byte bcd
    {
      u8temp1 = (buf[3 + pos * 2]/0x10*0x0A)+(buf[3 + pos * 2]%0x10);
      u8temp2 = (buf[3 + pos * 2 + 1]/0x10*0x0A)+(buf[3 + pos * 2 + 1]%0x10);
      
      u16Var = u8temp1*100 + u8temp2;
      *varType2 = u16Var;
    }
    break;
    case 32:// 32-byte bcd
    {
      u8temp1 = (buf[3 + pos * 2]/0x10*0x0A)+(buf[3 + pos * 2]%0x10);
      u8temp2 = (buf[3 + pos * 2 + 1]/0x10*0x0A)+(buf[3 + pos * 2 + 1]%0x10);
      u8temp3 = (buf[3 + pos * 2 + 2]/0x10*0x0A)+(buf[3 + pos * 2 + 2]%0x10);
      u8temp4 = (buf[3 + pos * 2 + 3]/0x10*0x0A)+(buf[3 + pos * 2 + 3]%0x10);
      
      u32Var = u8temp1*1000000 + u8temp2*10000 + u8temp3*100 + u8temp4;
      *varType3 = u32Var;
    }
    break;
    };
}
#if (USERDEF_MONITOR_LIB == ENABLED)
uint16_t LoadCurr, ACFaultM1, PMUConnectM1;

float TempSaft = 0;
void Extract_Holding_Regs_Data_LIB (void)
{
    uint8_t i,j;
    switch(Modbus.u8SlaveID - sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBSlaveOffset)
    {
    case 1:
        i = 0;
        break;
    case 2:
        i = 1;
        break;
    case 3:
        i = 2;
        break;
    case 4:
        i = 3;
        break;
    case 5:
        i = 4;
        break;
    case 6:
        i = 5;
        break;
    case 7:
        i = 6;
        break;
    case 8:
        i = 7;
        break;
    case 9:
        i = 8;
        break;
    case 10:
        i = 9;
        break;
    case 11:
        i = 10;
        break;
    case 12:
        i = 11;
        break;
    case 13:
        i = 12;
        break;
    case 14:
        i = 13;
        break;
    case 15:
        i = 14;
        break;
    case 16:
        i = 15;
        break;
    };
    switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBType)
    {
    case 13:// COSLIGHT_CF4850T  
    case 1:// COSLIGHT
    {
        switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep)
        {
        case LIB_COSLIGHT_INFO_1:
        {
          uint16_t CH, DCH;
            for (j = 0; j < 16; j++)
            {
                modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16CellVolt[j],0,j,16);
            }
            for (j = 0; j < 4; j++)
            {
                modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16CellTemp[j],0,16+j,16);
            }
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u16AverTempCell,0,23,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u16EnvTemp,0,26,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32PackVolt,0,33,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&CH,0,34,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&DCH,0,35,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32SOC,0,36,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32SOH,0,37,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32CapRemain,39,32);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32DSGCapacity,43,32);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32DischargeTime,0,53,16);
//            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32Serial,0,55,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16ModuleMode,0,57,16);
            if(sLiionBattInfo[i].u16ModuleMode == 1) sLiionBattInfo[i].u32PackCurr = CH;
            else if (sLiionBattInfo[i].u16ModuleMode == 2) sLiionBattInfo[i].u32PackCurr = 0 - (int32_t)DCH;
            else sLiionBattInfo[i].u32PackCurr = 0;
            
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_COSLIGHT_INFO_2;
        }
        break;
        case LIB_COSLIGHT_INFO_2:
        {
          uint8_t mode;
          uint8_t alarm_CellOverVolt, alarm_PackOverVolt, alarm_VoltDiff, alarm_CellLowVolt, alarm_PackLowVolt; 
          uint8_t alarm_CharOverCurrent, alarm_DisCharOverCurrent, alarm_BattHighTemp1, alarm_BattHighTemp2, alarm_BattLowTemp1, alarm_BattLowTemp2, alarm_LowCapacity;
          uint8_t alarm_pcb_high_temp,alarm_env_high_temp,alarm_env_low_temp;
          uint8_t protect_CellOverVolt, protect_PackOverVolt, protect_CellLowVolt, protect_PackLowVolt, protect_OverCurr1, protect_OverCurr2; 
          uint8_t protect_ShortCircuit, protect_CharHighTemp, protect_DisCharHighTemp, protect_CharLowTemp, protect_DisCharLowTemp, protect_lost_sensor;
          uint8_t protect_pcb_high_temp,protect_env_high_temp,protect_env_low_temp;
          //thanhcm3 fix-------------------------------------------------
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_env_high_temp,0,4);
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_env_low_temp,0,5);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_env_high_temp,0,6);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_env_low_temp,0,7);
          //thanhcm3 fix-------------------------------------------------
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_PackOverVolt,1,0);
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_PackLowVolt,1,1);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_PackOverVolt,1,2);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_PackLowVolt,1,3);
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_CharOverCurrent,1,4);
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_DisCharOverCurrent,1,5);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_OverCurr1,1,6);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_OverCurr2,1,7);
          
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_LowCapacity,2,0);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_ShortCircuit,2,1);
          //thanhcm3 fix ------------------------------------------------
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_pcb_high_temp,2,3);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_pcb_high_temp,2,4);
          //thanhcm3 fix-------------------------------------------------
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_CellOverVolt,3,0);
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_CellLowVolt,3,1);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_CellOverVolt,3,2);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_CellLowVolt,3,3);
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_BattHighTemp1,3,4);
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_BattLowTemp1,3,5);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_CharHighTemp,3,6);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_CharLowTemp,3,7);
          
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_BattHighTemp2,4,0);
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_BattLowTemp2,4,1);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_DisCharHighTemp,4,2);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_DisCharLowTemp,4,3);
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_VoltDiff,4,4);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_lost_sensor,4,5);
          
          if(alarm_BattHighTemp2 == 1) alarm_BattHighTemp1 = 1;
          if(alarm_BattLowTemp2 == 1) alarm_BattLowTemp1 = 1;
          if(protect_OverCurr2 == 1) protect_OverCurr1 = 1;
          //thanhcm3 fix -----------------------------------------------------------------------
          if((protect_CharHighTemp ==1)||(protect_DisCharHighTemp==1)) protect_CharHighTemp =1;
          if((protect_CharLowTemp==1)||(protect_DisCharLowTemp==1))    protect_CharLowTemp  =1;
          
          sLiionBattInfo[i].u32AlarmStatus = alarm_CellOverVolt|(alarm_CellLowVolt<<1)|(alarm_PackOverVolt<<2)|(alarm_PackLowVolt<<3)|(alarm_CharOverCurrent<<4)|
                                              (alarm_DisCharOverCurrent<<5)|(alarm_BattHighTemp1<<6)|(alarm_BattLowTemp1<<7)|(alarm_env_high_temp<<8)|(alarm_env_low_temp<<9)|(alarm_pcb_high_temp<<10)|(alarm_LowCapacity<<11)|(alarm_VoltDiff<<12)|
                                              (protect_CellOverVolt<<16)|(protect_CellLowVolt<<17)|(protect_PackOverVolt<<18)|(protect_PackLowVolt<<19)|
                                              (protect_ShortCircuit<<20)|(protect_OverCurr1<<21)|(protect_CharHighTemp<<22)|(protect_CharLowTemp<<23)|(protect_env_high_temp<<24)|(protect_env_low_temp<<25)|
                                              (protect_pcb_high_temp<<26)|(protect_lost_sensor<<29);
          //thanhcm3 fix -----------------------------------------------------------------------
          sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_COSLIGHT_INFO_3;
        }
        break;
        case LIB_COSLIGHT_INFO_3:
        {
            uint16_t seri1, seri2;
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&seri1,0,0,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&seri2,0,1,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16SoftwareVersion,0,3,16);
            sLiionBattInfo[i].u32Serial = ((uint32_t)seri1<<16)|seri2;
            
            sLiionBattInfo[sModbusManager.u8LIBCurrentIndex].u8ErrorFlag = 0;
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_COSLIGHT_INFO_1;
        }
        break;
        default:
        break;
        };
    }
    break;
    case 2:// COSLIGHT_OLD_V1.1
    {
        switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep)
        {
        case LIB_COSLIGHT_OLD_INFO_1:
        {
          uint16_t CH, DCH;
          int16_t temp1, temp2;
                        
            modbusParseInfoSign(&Modbus.u8BuffRead[0],0,&temp1,0,0,16);
            modbusParseInfoSign(&Modbus.u8BuffRead[0],0,&temp2,0,1,16);
            sLiionBattInfo[i].u16AverTempCell = (temp1+temp2) / 2;
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32PackVolt,0,3,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&CH,0,4,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&DCH,0,5,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32SOC,0,6,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32SOH,0,7,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32CapRemain,9,32);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32DischargeTime,0,23,16);            
            sLiionBattInfo[i].u32PackCurr = (int32_t)CH - (int32_t)DCH;
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_COSLIGHT_OLD_INFO_2;
        }
        break;
        case LIB_COSLIGHT_OLD_INFO_2:
        {
          uint8_t mode;
          uint8_t alarm_CellOverVolt, alarm_PackOverVolt, alarm_VoltDiff, alarm_CellLowVolt, alarm_PackLowVolt; 
          uint8_t alarm_CharOverCurrent, alarm_DisCharOverCurrent, alarm_BattHighTemp1, alarm_BattHighTemp2, alarm_BattLowTemp1, alarm_BattLowTemp2, alarm_LowCapacity;
          uint8_t alarm_pcb_high_temp;
          uint8_t protect_CellOverVolt, protect_PackOverVolt, protect_CellLowVolt, protect_PackLowVolt, protect_OverCurr1, protect_OverCurr2; 
          uint8_t protect_ShortCircuit, protect_CharHighTemp, protect_DisCharHighTemp, protect_CharLowTemp, protect_DisCharLowTemp, protect_lost_sensor;
          uint8_t protect_pcb_high_temp;
          
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_PackOverVolt,0,0);
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_PackLowVolt,0,1);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_PackOverVolt,0,2);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_PackLowVolt,0,3);
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_CharOverCurrent,0,4);
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_DisCharOverCurrent,0,5);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_OverCurr1,0,6);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_OverCurr2,0,7);
          
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_LowCapacity,1,0);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_ShortCircuit,1,1);
          //thanhcm3 fix-----------------------------------------------------
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_pcb_high_temp,1,3);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_pcb_high_temp,1,4);
          //thanhcm3 fix-----------------------------------------------------
          
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_CellOverVolt,2,0);
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_CellLowVolt,2,1);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_CellOverVolt,2,2);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_CellLowVolt,2,3);
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_BattHighTemp1,2,4);
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_BattLowTemp1,2,5);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_CharHighTemp,2,6);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_CharLowTemp,2,7);
          
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_BattHighTemp2,3,0);
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_BattLowTemp2,3,1);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_DisCharHighTemp,3,2);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_DisCharLowTemp,3,3);
          modbusParsebit(&Modbus.u8BuffRead[0],&alarm_VoltDiff,3,4);
          modbusParsebit(&Modbus.u8BuffRead[0],&protect_lost_sensor,3,5);
          
          if(alarm_BattHighTemp2 == 1) alarm_BattHighTemp1 = 1;
          if(alarm_BattLowTemp2 == 1) alarm_BattLowTemp1 = 1;
          if(protect_OverCurr2 == 1) protect_OverCurr1 = 1;
          
          //thanhcm3 fix ------------------------------------------------------------------------------------------------
          if((protect_CharHighTemp == 1)||(protect_DisCharHighTemp ==1))  protect_CharHighTemp =1;
          if((protect_CharLowTemp ==1 )||(protect_DisCharLowTemp == 1))   protect_CharLowTemp =1;
          sLiionBattInfo[i].u32AlarmStatus = alarm_CellOverVolt|(alarm_CellLowVolt<<1)|(alarm_PackOverVolt<<2)|(alarm_PackLowVolt<<3)|(alarm_CharOverCurrent<<4)|
                                              (alarm_DisCharOverCurrent<<5)|(alarm_BattHighTemp1<<6)|(alarm_BattLowTemp1<<7)|(alarm_pcb_high_temp<<10)|(alarm_LowCapacity<<11)|(alarm_VoltDiff<<12)|
                                              (protect_CellOverVolt<<16)|(protect_CellLowVolt<<17)|(protect_PackOverVolt<<18)|(protect_PackLowVolt<<19)|
                                              (protect_ShortCircuit<<20)|(protect_OverCurr1<<21)|(protect_CharHighTemp<<22)|(protect_CharLowTemp<<23)|
                                              (protect_pcb_high_temp<<26)|(protect_lost_sensor<<29);
          //thanhcm3 fix ------------------------------------------------------------------------------------------------
          sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_COSLIGHT_OLD_INFO_3;
        }
        break;
        case LIB_COSLIGHT_OLD_INFO_3:
        {
            uint16_t seri1, seri2;
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&seri1,0,0,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&seri2,0,1,16);
//            modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16SoftwareVersion,0,3,16);
            sLiionBattInfo[i].u32Serial = ((uint32_t)seri1<<16)|seri2;
            
            sLiionBattInfo[sModbusManager.u8LIBCurrentIndex].u8ErrorFlag = 0; 
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_COSLIGHT_OLD_INFO_1;
        }
        break;
        default:
        break;
        };
    }
    break;
    case 12:// SHOTO_SDA10_48100
    case 3:// SHOTO_2019
    {
        switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep)
        {
        case LIB_SHOTO_INFO_1:
        {
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32PackVolt,0,0,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32PackCurr,0,1,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32CapRemain,0,2,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32DSGCapacity,0,4,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32SOC,0,5,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32SOH,0,6,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32DischargeTime,0,7,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u16AverTempCell,0,9,16);
            sLiionBattInfo[i].u16AverTempCell = (uint16_t)((float)sLiionBattInfo[i].u16AverTempCell - 2731.5);

            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_SHOTO_INFO_2;
        }
        break;
        case LIB_SHOTO_INFO_2:
        {
            for (j = 0; j < 16; j++)
            {
                modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16CellVolt[j],0,j,16);
            }
            for (j = 0; j < 8; j++)
            {
                modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16CellTemp[j],0,16+j,16);
                sLiionBattInfo[i].u16CellTemp[j] = (uint16_t)((float)sLiionBattInfo[i].u16CellTemp[j] - 2731.5);
            }
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16EnvTemp,0,24,16);
            sLiionBattInfo[i].u16EnvTemp = (uint16_t)((float)sLiionBattInfo[i].u16EnvTemp - 2731.5);
        
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_SHOTO_INFO_3;
        }
        break;
        case LIB_SHOTO_INFO_3:
        {
            uint8_t mode_DisChar, mode_Char, mode_FloatingChar, mode_FullChar, mode_Standby, mode_Turn_off;
            uint8_t alarm_CellOverVolt, alarm_PackOverVolt, alarm_CellLowVolt, alarm_PackLowVolt; 
            uint8_t alarm_CharOverCurrent, alarm_DisCharOverCurrent, alarm_CharHighTemp, alarm_DisCharHighTemp, alarm_CharLowTemp, alarm_DisCharLowTemp, alarm_LowCapacity;
            uint8_t alarm_env_high_temp,alarm_env_low_temp;
            uint8_t protect_CellOverVolt, protect_PackOverVolt, protect_CellLowVolt, protect_PackLowVolt, protect_CharOverCurrent, protect_DisCharOverCurrent; 
            uint8_t protect_ShortCircuit, protect_CharHighTemp, protect_DisCharHighTemp, protect_CharLowTemp, protect_DisCharLowTemp;
            uint8_t protect_env_high_temp,protect_env_low_temp,protect_pcb_high_temp;
                
            modbusParsebit(&Modbus.u8BuffRead[0],&mode_DisChar,0,0);
            modbusParsebit(&Modbus.u8BuffRead[0],&mode_Char,0,1);
            modbusParsebit(&Modbus.u8BuffRead[0],&mode_FloatingChar,0,2);
            modbusParsebit(&Modbus.u8BuffRead[0],&mode_FullChar,0,3);
            modbusParsebit(&Modbus.u8BuffRead[0],&mode_Standby,0,4);
            modbusParsebit(&Modbus.u8BuffRead[0],&mode_Turn_off,0,5);
            
            if(mode_FloatingChar==1) sLiionBattInfo[i].u16ModuleMode = 1;
            else if(mode_FullChar==1) sLiionBattInfo[i].u16ModuleMode = 1;
            else if(mode_Standby==1) sLiionBattInfo[i].u16ModuleMode = 3;
            else if(mode_DisChar==1) sLiionBattInfo[i].u16ModuleMode = 2;
            else if(mode_Char==1) sLiionBattInfo[i].u16ModuleMode = 1;
            else sLiionBattInfo[i].u16ModuleMode = 4;

            
            
            
            modbusParsebit(&Modbus.u8BuffRead[0],&alarm_CellOverVolt,1,0);
            modbusParsebit(&Modbus.u8BuffRead[0],&protect_CellOverVolt,1,1);
            modbusParsebit(&Modbus.u8BuffRead[0],&alarm_CellLowVolt,1,2);
            modbusParsebit(&Modbus.u8BuffRead[0],&protect_CellLowVolt,1,3);
            modbusParsebit(&Modbus.u8BuffRead[0],&alarm_PackOverVolt,1,4);
            modbusParsebit(&Modbus.u8BuffRead[0],&protect_PackOverVolt,1,5);
            modbusParsebit(&Modbus.u8BuffRead[0],&alarm_PackLowVolt,1,6);
            modbusParsebit(&Modbus.u8BuffRead[0],&protect_PackLowVolt,1,7);
            
            modbusParsebit(&Modbus.u8BuffRead[0],&alarm_CharHighTemp,2,0);
            modbusParsebit(&Modbus.u8BuffRead[0],&protect_CharHighTemp,2,1);
            modbusParsebit(&Modbus.u8BuffRead[0],&alarm_CharLowTemp,2,2);
            modbusParsebit(&Modbus.u8BuffRead[0],&protect_CharLowTemp,2,3);
            modbusParsebit(&Modbus.u8BuffRead[0],&alarm_DisCharHighTemp,2,4);
            modbusParsebit(&Modbus.u8BuffRead[0],&protect_DisCharHighTemp,2,5);
            modbusParsebit(&Modbus.u8BuffRead[0],&alarm_DisCharLowTemp,2,6);
            modbusParsebit(&Modbus.u8BuffRead[0],&protect_DisCharLowTemp,2,7);
            
            //thanhcm3 fix------------------------------------------------------
            modbusParsebit(&Modbus.u8BuffRead[0],&alarm_env_high_temp,3,0);
            modbusParsebit(&Modbus.u8BuffRead[0],&protect_env_high_temp,3,1);
            modbusParsebit(&Modbus.u8BuffRead[0],&alarm_env_low_temp,3,2);
            modbusParsebit(&Modbus.u8BuffRead[0],&protect_env_low_temp,3,3);
            modbusParsebit(&Modbus.u8BuffRead[0],&protect_pcb_high_temp,3,4);
            //thanhcm3 fix------------------------------------------------------
            
            modbusParsebit(&Modbus.u8BuffRead[0],&alarm_CharOverCurrent,4,0);
            modbusParsebit(&Modbus.u8BuffRead[0],&protect_CharOverCurrent,4,1);
            modbusParsebit(&Modbus.u8BuffRead[0],&alarm_DisCharOverCurrent,4,3);
            modbusParsebit(&Modbus.u8BuffRead[0],&protect_DisCharOverCurrent,4,4);
            modbusParsebit(&Modbus.u8BuffRead[0],&protect_ShortCircuit,4,6);
            
            modbusParsebit(&Modbus.u8BuffRead[0],&alarm_LowCapacity,6,2);
            
            if(alarm_DisCharHighTemp == 1) alarm_CharHighTemp = 1;
            if(alarm_DisCharLowTemp == 1) alarm_CharLowTemp = 1;
            if(protect_DisCharOverCurrent == 1) protect_CharOverCurrent = 1;
            //thanhcm3 fix ---------------------------------------------------------
            if((protect_CharHighTemp == 1)||(protect_DisCharHighTemp == 1))  protect_CharHighTemp = 1;
            if((protect_CharLowTemp == 1)||(protect_DisCharLowTemp ==1 ))    protect_CharLowTemp  = 1;
            sLiionBattInfo[i].u32AlarmStatus = alarm_CellOverVolt|(alarm_CellLowVolt<<1)|(alarm_PackOverVolt<<2)|(alarm_PackLowVolt<<3)|(alarm_CharOverCurrent<<4)|
                                              (alarm_DisCharOverCurrent<<5)|(alarm_CharHighTemp<<6)|(alarm_CharLowTemp<<7)|(alarm_env_high_temp<<8)|(alarm_env_low_temp<<9)|(alarm_LowCapacity<<11)|
                                              (protect_CellOverVolt<<16)|(protect_CellLowVolt<<17)|(protect_PackOverVolt<<18)|(protect_PackLowVolt<<19)|
                                              (protect_ShortCircuit<<20)|(protect_CharOverCurrent<<21)|(protect_CharHighTemp<<22)|(protect_CharLowTemp<<23)|
                                              (protect_env_high_temp<<24)|(protect_env_low_temp<<25)|(protect_pcb_high_temp<<26);
            //thanhcm3 fix ---------------------------------
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_SHOTO_INFO_4;
        }
        break;
        case LIB_SHOTO_INFO_4:
        {
            uint16_t temp; 
            modbusParseInfo(&Modbus.u8BuffRead[0], 0, &temp,0,0,16);
            sLiionBattInfo[i].u8swversion[1] =  (uint8_t) temp;
            sLiionBattInfo[i].u8swversion[0] = (uint8_t) (temp>>8);
            modbusParseInfoString2(&Modbus.u8BuffRead[0],&sLiionBattInfo[i].u8mode[0], 20, 1);
            
            sLiionBattInfo[sModbusManager.u8LIBCurrentIndex].u8ErrorFlag = 0;
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_SHOTO_INFO_1;
        }
        break;
        default:
        break;
        };
    }
    break;
    case 4:// HUAWEI
    {
        switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep)
        {
        case LIB_HUAWEI_INFO_1:
        {
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32PackVolt,0,0,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32BattVolt,0,1,16);            
            modbusParseInfoSign(&Modbus.u8BuffRead[0],0,(int16_t *)&sLiionBattInfo[i].u16PackCurr,0,2,16);
//            if (sLiionBattInfo[i].u16PackCurr > 0xF000)
//            {
//              sLiionBattInfo[i].u16PackCurr = ~sLiionBattInfo[i].u16PackCurr + 1;
//            }
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32SOC,0,3,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32SOH,0,4,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u16AverTempCell,0,5,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u16EnvTemp,0,6,16);
            
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16ModuleMode,0,14,16);
            
            for (j = 0; j < 16; j++)
            {
                modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16CellTemp[j],0,18+j,16);
            }
            for (j = 0; j < 16; j++)
            {
                modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16CellVolt[j],0,34+j,16);
            }
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_HUAWEI_INFO_2;
        }
        break;
        case LIB_HUAWEI_INFO_2:
        {
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32DischargeTime,0,0,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16Protect1,0,3,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16Protect2,0,4,16);            
            sLiionBattInfo[i].u32ProtectStatus = (sLiionBattInfo[i].u16Protect1 << 16) | sLiionBattInfo[i].u16Protect2;
            
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16MajorAlarm,0,5,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16MinorAlarm,0,6,16);            
            sLiionBattInfo[i].u32AlarmStatus = (sLiionBattInfo[i].u16MajorAlarm << 16) | sLiionBattInfo[i].u16MinorAlarm;
            
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16ModuleAlarm,0,7,16);
            sLiionBattInfo[i].u32FaultStatus = (sLiionBattInfo[i].u16ModuleMode << 16) | sLiionBattInfo[i].u16ModuleAlarm;
        
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_HUAWEI_INFO_3;
        }
        break;
        case LIB_HUAWEI_INFO_3:
        {
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16SoftwareVersion,0,0,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16HardwareVersion,0,1,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32CapRemain,0,6,16);
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_HUAWEI_INFO_4;
        }
        break;
        case LIB_HUAWEI_INFO_4:
        {
            modbusParseInfoString2(&Modbus.u8BuffRead[0],&sLiionBattInfo[i].u8mode[0], 12, 0);
            
            sLiionBattInfo[sModbusManager.u8LIBCurrentIndex].u8ErrorFlag = 0;
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_HUAWEI_INFO_1;
        }
        break;
        default:
        break;
        };
    }
    break;
    case 5:// M1Viettel50
    {
        switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep)
        {
        case LIB_VIETTEL_INFO_1:
        {
          uint16_t tempsw, BattType; 
          
          modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32ADCPackVolt,0,32);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32ADCBattVolt,2,32);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32PackVolt,4,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32ADCPackCurr,6,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32ADCBattCurr,8,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32PackCurr,10,32);
          
          for (j = 0; j < 15; j++)
          {
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16CellVolt[j],0,12+j,16);
          }
          
          for (j = 0; j < 4; j++)
          {
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16CellTemp[j],0,27+j,16);
          }
          modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16EnvTemp,0,31,16); 
          modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16AverTempCell,0,35,16); 
          modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32AlarmStatus,36,32);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32ProtectStatus,38,32);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32FaultStatus,40,32);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16ModuleMode,0,42,16);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32SOC,43,32);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32SOH,45,32);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32DischargeTime,47,32);
          
          modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16ConverterState,0,51,16); 
          modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16ErrCode,0,52,16); 
          modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16BalanceStatus,0,53,16); 
          modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16MosfetMode,0,55,16); 
          modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32Mcu2McuErr,58,32);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32CHGCapacity,62,32);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32DSGCapacity,64,32);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32Efficiency,66,32);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16NumberOfCan,0,68,16);
          
          modbusParseInfo(&Modbus.u8BuffRead[0], 0, &sLiionBattInfo[i].u16BattType,0,80,16);
          modbusParseInfo(&Modbus.u8BuffRead[0], 0, &tempsw,0,81,16);
          sLiionBattInfo[i].sw_ver         = tempsw;// thanhcm3 fix
          sLiionBattInfo[i].u8swversion[1] =  (uint8_t) tempsw;
          sLiionBattInfo[i].u8swversion[0] = (uint8_t) (tempsw>>8);  
          modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32Serial,82,32);
            
          sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_VIETTEL_INFO_2;
        }
        break;
        case LIB_VIETTEL_INFO_2:
        {
          modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16SOCMaxim,0,2,16);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16SOHMaxim,0,3,16);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16FullCapRepMaxim,0,4,16);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16VoltMaxim,0,5,16);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16CurrMaxim,0,6,16);
          if(sLiionBattInfo[i].sw_ver == 110){
            modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32IKalamn,8,32);
            //
            modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32SOCKalamn,12,32);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32VpriKalamn,14,32);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32VmesKalamn,16,32);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32CapKalamn,18,32);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32InternalR0Kalamn,20,32);
            sLiionBattInfo[i].u32CapKalamn = sLiionBattInfo[i].u32CapKalamn*10;
            
          }else{
            modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32IKalamn,8,32);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32SOCKalamn,10,32);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32VpriKalamn,12,32);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32VmesKalamn,14,32);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32CapKalamn,16,32);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32InternalR0Kalamn,18,32);
            //modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32InternalR0Kalamn,20,32);//thanhcm3 fix
            float temp_cap_kalamn;
            temp_cap_kalamn = (float) sLiionBattInfo[i].u32CapKalamn;
            temp_cap_kalamn = temp_cap_kalamn/10000.0;
            sLiionBattInfo[i].u32CapKalamn =(uint32_t)temp_cap_kalamn;
          }

          sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_VIETTEL_INFO_3;
        }
        break;
        case LIB_VIETTEL_INFO_3:
        {
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32PackHighVolt_A,0,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32BattHighVolt_A,2,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32CellHighVolt_A,4,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32BattLowVolt_A,6,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32CellLowVolt_A,8,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32CharHighCurr_A,10,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32DisCharHighCurr_A,12,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32CharHighTemp_A,14,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32DisCharHighTemp_A,16,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32CharLowTemp_A,18,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32DisCharLowtemp_A,20,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32LowCap_A,22,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32BMSHighTemp_A,24,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32BMSLowTemp_A,26,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32PackOverVolt_P,28,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32BattOverVolt_P,30,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32CellOverVolt_P,32,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32BattUnderVolt_P,34,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32CellUnderVolt_P,36,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32CharOverCurr_P,38,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32DisCharOverCurr_P,40,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32CharOverTemp_P,42,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32DisCharOverTemp_P,44,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32CharUnderTemp_P,46,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32DisCharUnderTemp_P,48,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32UnderCap_P,50,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32BMSOverTemp_P,52,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32BMSUnderTemp_P,54,32);
          modbusParseInfoSign(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].i32DifferentVolt_P,56,32);
          
          
          modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32VoltBalance,60,32);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32DeltaVoltBalance,62,32);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32DisCharCurrLimit,64,32);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32CharCurrLimit,66,32);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32VoltDisCharRef,72,32);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32VoltDisCharCMD,74,32);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16SystemByte,0,76,16);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16KeyTime,0,77,16);
          
          sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_VIETTEL_INFO_4;
        }
        break;
        case LIB_VIETTEL_INFO_4:
        {
          modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16IniMaxim,0,0,16);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16EnableKCS,0,1,16);
          modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16_liionCapInit,0,2,16);

//          sLiionBattInfo[sModbusManager.u8LIBCurrentIndex].u8ErrorFlag = 0;
          sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_VIETTEL_INFO_5;
        }
        break;
        case LIB_VIETTEL_INFO_5:
        {
          uint8_t u8Key[10];
          uint32_t Key2 = 0;
          uint16_t u16Temp;
          modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16EnableLock,0,5,16);
          for(j=0;j<10;j++)
            u8Key[j] = Modbus.u8BuffRead[3+j];
          modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&Key2,3,32);
          Key2 += 95;
          u8Key[6] = (uint8_t)(Key2>>24);
          u8Key[7] = (uint8_t)(Key2>>16);
          u8Key[8] = (uint8_t)(Key2>>8);
          u8Key[9] = (uint8_t)Key2;
          vTaskDelay(500);
          Write_Multi_Reg (sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID, 116, &u8Key[0], 5);
          vTaskDelay(500);
          if(PMUConnectM1 == 1)
            u8Key[1] = 0x00;
          else {
            if(ACFaultM1 == 0)
              u8Key[1] = 0x19; 
            else
              u8Key[1] = 0x11;
          }
          u8Key[0] = 0;
          Write_Multi_Reg (sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID, 76, &u8Key[0], 1); // System Byte
          vTaskDelay(500);
          u16Temp = (uint16_t)LoadCurr;
          u8Key[0] = (uint8_t)(u16Temp>>8);
          u8Key[1] = (uint8_t)(u16Temp);
          Write_Multi_Reg (sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID, 78, &u8Key[0], 1); // Total Curr
          vTaskDelay(500);
          sLiionBattInfo[sModbusManager.u8LIBCurrentIndex].u8ErrorFlag = 0;
          sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_VIETTEL_INFO_1;
        }
        break;
        default:
        break;
        };
        
    }
    break;
    case 14:// HUAFU_HF48100C
    case 6:// ZTT_2020
    {
        switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep)
        {
        case LIB_ZTT_2020_INFO_1:
        {
            modbusParseInfoSign(&Modbus.u8BuffRead[0],0,(int16_t *)&sLiionBattInfo[i].u16PackCurr,0,0,16);      
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32PackVolt,0,1,16);        
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32SOC,0,2,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32SOH,0,3,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32CapRemain,0,4,16);       
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32DischargeTime,0,7,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32AlarmStatus,0,9,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32ProtectStatus,0,10,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32FaultStatus,0,11,16);
            for (j = 0; j < 16; j++)
            {
                modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16CellVolt[j],0,15+j,16);
            }
            for (j = 0; j < 4; j++)
            {
                modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16CellTemp[j],0,31+j,16);
            }
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u16EnvTemp,0,36,16);      
            sLiionBattInfo[i].u16AverTempCell = (sLiionBattInfo[i].u16CellTemp[0] + sLiionBattInfo[i].u16CellTemp[1] + sLiionBattInfo[i].u16CellTemp[2] + sLiionBattInfo[i].u16CellTemp[3])/4; 
            //thanhcm3 fix------------------------------------------------------------------------------
            if(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBType == 14){
              sLiionBattInfo[i].u32DSGCapacity = 0;
            }else if(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBType == 6){
              modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32DSGCapacity,40,32);
            }
            //thanhcm3 fix------------------------------------------------------------------------------
            
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_ZTT_2020_INFO_2;
        }
        break;
        case LIB_ZTT_2020_INFO_2:
        {
            modbusParseInfoString2(&Modbus.u8BuffRead[0],&sLiionBattInfo[i].u8swversion[0], 20, 0);
            
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_ZTT_2020_INFO_3;
        }
        break;
        case LIB_ZTT_2020_INFO_3:
        {
            modbusParseInfoString2(&Modbus.u8BuffRead[0],&sLiionBattInfo[i].u8mode[0], 20, 0);
            
            sLiionBattInfo[sModbusManager.u8LIBCurrentIndex].u8ErrorFlag = 0;
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_ZTT_2020_INFO_1;
        }
        break;
        default:
        break;
        };
    }
    break;
    case 9:// SAFT
    {
        switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep)
        {
        case LIB_SAFT_INFO_1:
        {
            uint16_t alarm1, alarm2, alarm3, tempSaft;
            
            uint8_t protect_CellOverVolt, protect_PackOverVolt, protect_CellLowVolt, protect_PackLowVolt, protect_CharOverCurrent, protect_DisCharOverCurrent; 
            uint8_t protect_ShortCircuit, protect_CharHighTemp, protect_DisCharHighTemp, protect_CharLowTemp, protect_DisCharLowTemp;
            uint8_t protect_charge_or_discharge_high_temp,protect_charge_or_discharge_low_temp;
            
            modbusParseInfoInverse(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32PackVolt,0,32);
            modbusParseInfoInverse(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32PackCurr,2,32);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&alarm1,0,8,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&alarm2,0,9,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&alarm3,0,10,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&tempSaft,0,12,16);
            sLiionBattInfo[i].u32SOH = tempSaft & 0x00FF;
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&tempSaft,0,14,16);
            sLiionBattInfo[i].u32SOC = tempSaft & 0x00FF;
            sLiionBattInfo[i].u16AverTempCell = (tempSaft & 0xFF00) >> 8;
            
            protect_ShortCircuit = (uint8_t)((alarm1&0x0002) >> 1); 
            if((alarm1&0x000C) != 0x0000){
              if(sLiionBattInfo[i].u32PackCurr > 0){
                protect_CharHighTemp = 1;
                protect_DisCharHighTemp = 0;
              } else {
                protect_CharHighTemp = 0;
                protect_DisCharHighTemp = 1;
              }
            } else {
              protect_CharHighTemp = 0;
              protect_DisCharHighTemp = 0;
            } 
            if((alarm1&0x0030) != 0x0000){
              if(sLiionBattInfo[i].u32PackCurr > 0){
                protect_CharLowTemp = 1;
                protect_DisCharLowTemp = 0;
              } else {
                protect_CharLowTemp = 0;
                protect_DisCharLowTemp = 1;
              }
            } else {
              protect_CharLowTemp = 0;
              protect_DisCharLowTemp = 0;
            }
            if((alarm1&0x00C0) != 0x0000)
              protect_CellOverVolt = 1;
            else
              protect_CellOverVolt = 0;
            if((alarm1&0x0300) != 0x0000)
              protect_CellLowVolt = 1;
            else
              protect_CellLowVolt = 0;
            if(((alarm1&0x8000) != 0x0000)|((alarm2&0x0001) != 0x0000))
              protect_CharOverCurrent = 1;
            else
              protect_CharOverCurrent = 0;
            if((alarm2&0x0080) != 0x0000)
              protect_PackOverVolt = 1;
            else
              protect_PackOverVolt = 0;
            if((alarm2&0x0100) != 0x0000)
              protect_PackLowVolt = 1;
            else
              protect_PackLowVolt = 0;
            
            
            if((protect_CharHighTemp ==1)||(protect_DisCharHighTemp == 1)){
              protect_charge_or_discharge_high_temp = 1;
            }else{
              protect_charge_or_discharge_high_temp = 0;
            }
            
            if((protect_CharLowTemp== 1)||(protect_DisCharLowTemp == 1)){
              protect_charge_or_discharge_low_temp = 1;
            }else{
              protect_charge_or_discharge_low_temp = 0;
            }
            
            sLiionBattInfo[i].u32AlarmStatus = (protect_CellOverVolt<<16)|(protect_CellLowVolt<<17)|(protect_PackOverVolt<<18)|(protect_PackLowVolt<<19)|
                                              (protect_ShortCircuit<<20)|(protect_CharOverCurrent<<21)|(protect_charge_or_discharge_high_temp<<22)|(protect_charge_or_discharge_low_temp<<23);
            
            sLiionBattInfo[sModbusManager.u8LIBCurrentIndex].u8ErrorFlag = 0;
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_SAFT_INFO_1;
        }
        break;
        default:
          break;
        }
    }
    break;
    case 10:// Narada75
    {
        switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep)
        {
        case LIB_NARADA75_INFO_1:
        {
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32PackVolt,0,0,16);  
            modbusParseInfoSign(&Modbus.u8BuffRead[0],0,(int16_t *)&sLiionBattInfo[i].u32PackCurr,0,1,16);      
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32CapRemain,0,2,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u16AverTempCell,0,3,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u16EnvTemp,0,4,16); 
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32AlarmStatus,0,5,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32ProtectStatus,0,6,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32FaultStatus,0,7,16);           
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32SOC,0,8,16);
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_NARADA75_INFO_2;
        }
        break;
        case LIB_NARADA75_INFO_2:
        {
            char str1[] = "Product Style";
            char str2[] = "Serial Number Version";
            char serial[20] = {0};
            char *pdest1, *pdest2;
            pdest1 = strstr((const char*)Modbus.u8BuffRead,str1);
            int j = 0;
            while(*pdest1 != 'S')
            {
              sLiionBattInfo[i].u8mode[j++] =  *pdest1;
              pdest1++;
            }
            sLiionBattInfo[i].u8mode[j] = '\0';
            j = 0;
            pdest2 = strstr((const char *)Modbus.u8BuffRead,str2);
            while(*pdest2 != '\0')
            {
              serial[j++] =  *pdest2;
              pdest2++;
            }       
            
            sLiionBattInfo[sModbusManager.u8LIBCurrentIndex].u8ErrorFlag = 0;
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_NARADA75_INFO_1;
        }
        break;
        default:
        break;
        };
      
    }
    break;
    case 7:// ZTT50
    {
      switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep)
        {
        case LIB_ZTT_INFO_1:
        {
        modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32PackVolt,0,0,16);
        modbusParseInfoSign(&Modbus.u8BuffRead[0],0,(int16_t *)&sLiionBattInfo[i].u32PackCurr,0,1,16);
        modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32CapRemain,0,2,16);
        modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u16AverTempCell,0,3,16);
        modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u16EnvTemp,0,4,16);
        modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32AlarmStatus,0,5,16);
        modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32ProtectStatus,0,6,16);
        modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32FaultStatus,0,7,16);
        modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32SOC,0,8,16);

        sLiionBattInfo[i].fPackVolt = sLiionBattInfo[i].u32PackVolt * 0.01;
        sLiionBattInfo[i].fPackCurr = (sLiionBattInfo[i].u32PackCurr - 10000) * 0.1;
        sLiionBattInfo[i].fCapRemain = sLiionBattInfo[i].u32CapRemain * 0.1;
        sLiionBattInfo[i].fAverTempCell = (sLiionBattInfo[i].u16AverTempCell - 400) * 0.1;
        sLiionBattInfo[i].fEnvTemp = (sLiionBattInfo[i].u16EnvTemp -400) * 0.1;
        sLiionBattInfo[i].fSOC = sLiionBattInfo[i].u32SOC * 0.01;
        
        sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_ZTT_INFO_2;
        }
        break;
        case LIB_ZTT_INFO_2:
        {
          uint8_t sw1, sw2, hw1, hw2, hw3, hw4, hw5;
          uint8_t *s_temp;
          s_temp = (uint8_t*)strchr((const char*)Modbus.u8BuffRead,'*');
          sw1 = s_temp[1];
          sw2 = s_temp[2];
          hw1 = s_temp[4];
          hw2 = s_temp[5];
          hw3 = s_temp[6];
          hw4 = s_temp[7];
          hw5 = s_temp[8];
          for(int j=0;j<20;j++)
          {
            sLiionBattInfo[i].u8mode[j] = (uint8_t)s_temp[j+10];
          }
          sLiionBattInfo[i].u8mode[j+1] = '\0';
          
          sprintf((char*)sLiionBattInfo[i].u8swversion,"V%2d.%2d",sw1,sw2);
          sprintf((char*)sLiionBattInfo[i].u8hwversion,"V%2d.%2d.%2d.%2d.%2d",hw1,hw2,hw3,hw4,hw5);
          
          sLiionBattInfo[sModbusManager.u8LIBCurrentIndex].u8ErrorFlag = 0;
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_ZTT_INFO_1;
        }
        break;
        default:
        break;
        };
    }
    break;
    case 8:// HUAWEI_A1
    {
        switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep)
        {
        case LIB_HUAWEI_A1_INFO_1:
        {
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32PackVolt,0,0,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32BattVolt,0,1,16);            
            modbusParseInfoSign(&Modbus.u8BuffRead[0],0,(int16_t *)&sLiionBattInfo[i].u16PackCurr,0,2,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32SOC,0,3,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32SOH,0,4,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u16AverTempCell,0,5,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u16EnvTemp,0,6,16);
            
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16ModuleMode,0,14,16);
            
            for (j = 0; j < 16; j++)
            {
                modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16CellTemp[j],0,18+j,16);
            }
            for (j = 0; j < 16; j++)
            {
                modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16CellVolt[j],0,34+j,16);
            }
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_HUAWEI_A1_INFO_2;
        }
        break;
        case LIB_HUAWEI_A1_INFO_2:
        {
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32DischargeTime,0,0,16);
//            modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16Protect1,0,3,16);
//            modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16Protect2,0,4,16);            
//            sLiionBattInfo[i].u32ProtectStatus = (sLiionBattInfo[i].u16Protect1 << 16) | sLiionBattInfo[i].u16Protect2;
//            
//            modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16MajorAlarm,0,5,16);
//            modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16MinorAlarm,0,6,16);            
//            sLiionBattInfo[i].u32AlarmStatus = (sLiionBattInfo[i].u16MajorAlarm << 16) | sLiionBattInfo[i].u16MinorAlarm;
//            
//            modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16ModuleAlarm,0,7,16);
//            sLiionBattInfo[i].u32FaultStatus = (sLiionBattInfo[i].u16ModuleMode << 16) | sLiionBattInfo[i].u16ModuleAlarm;
        
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_HUAWEI_A1_INFO_3;
        }
        break;
        case LIB_HUAWEI_A1_INFO_3:
        {
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16SoftwareVersion,0,0,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16HardwareVersion,0,1,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32CapRemain,0,6,16);
            
            sLiionBattInfo[sModbusManager.u8LIBCurrentIndex].u8ErrorFlag = 0;
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_HUAWEI_A1_INFO_1;
        }
        break;
        default:
        break;
        };
    }
    break;
    case 11:// EVE
    {
        switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep)
        {
        case LIB_EVE_INFO_1:
        {
            modbusParseInfoSign(&Modbus.u8BuffRead[0],0,(int16_t *)&sLiionBattInfo[i].u16PackCurr,0,0,16);      // x100
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32PackVolt,0,1,16);         // x100
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32SOC,0,2,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32SOH,0,3,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32CapRemain,0,4,16);        // x100
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32DischargeTime,0,7,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32AlarmStatus,0,9,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32ProtectStatus,0,10,16);
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u32FaultStatus,0,11,16);
            for (j = 0; j < 16; j++)
            {
                modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16CellVolt[j],0,15+j,16);
            }
            for (j = 0; j < 4; j++)
            {
                modbusParseInfo(&Modbus.u8BuffRead[0],0,&sLiionBattInfo[i].u16CellTemp[j],0,31+j,16);
            }
            modbusParseInfo(&Modbus.u8BuffRead[0],0,(uint16_t *)&sLiionBattInfo[i].u16EnvTemp,0,36,16);      
            sLiionBattInfo[i].u16AverTempCell = (sLiionBattInfo[i].u16CellTemp[0] + sLiionBattInfo[i].u16CellTemp[1] + sLiionBattInfo[i].u16CellTemp[2] + sLiionBattInfo[i].u16CellTemp[3])/4; 
            
            modbusParseInfo(&Modbus.u8BuffRead[0],0,0,&sLiionBattInfo[i].u32DSGCapacity,40,32);
            
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_EVE_INFO_2;
        }
        break;
        case LIB_EVE_INFO_2:
        {
            modbusParseInfoString2(&Modbus.u8BuffRead[0],&sLiionBattInfo[i].u8swversion[0], 20, 0);
            
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_EVE_INFO_3;
        }
        break;
        case LIB_EVE_INFO_3:
        {
            modbusParseInfoString2(&Modbus.u8BuffRead[0],&sLiionBattInfo[i].u8mode[0], 20, 0);
            
            sLiionBattInfo[sModbusManager.u8LIBCurrentIndex].u8ErrorFlag = 0;
            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_EVE_INFO_1;
        }
        break;
        default:
        break;
        };
    }
    break;
    };
}
#endif


#if (USERDEF_MONITOR_GEN == ENABLED)
void Extract_Holding_Regs_Data_GEN ()
{
    uint8_t i,j;
    uint8_t *ptr = &Modbus.u8BuffRead[0];
    switch(Modbus.u8SlaveID - sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenSlaveOffset)
    {
    case 1:
        i = 0;
        break;
    case 2:
        i = 1;
        break;
    };
    switch(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenType)
    {
    case 1:// KUBOTA
    {
        switch(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep)
        {
        case GEN_KUBOTA_INFO_1:
        {
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32GenFrequency,0,0,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32MainFrequency,0,1,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32EngineSpeed,0,2,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32OilPressure,0,6,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32CoolantTemp,0,7,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32FuelLevel,0,8,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32BattVolt,0,9,16);
            
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32EngineWorkingHour,14,32);

            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNCurr1,0,16,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNCurr2,0,17,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNCurr3,0,18,16);

            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNVolt1,0,26,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNVolt2,0,27,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNVolt3,0,28,16);
            
            modbusParseInfofInverse(ptr, &sGenInfo[i].fPF1, 41);
            modbusParseInfofInverse(ptr, &sGenInfo[i].fPF2, 43);
            modbusParseInfofInverse(ptr, &sGenInfo[i].fPF3, 45);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32EngineWorkingMin,0,48,16);            
            
            modbusParseInfoInverse(ptr,0,0, &sGenInfo[i].u32ActiveEnergyLow,77,32);
            modbusParseInfoInverse(ptr,0,0, &sGenInfo[i].u32ActiveEnergyHigh,79,32);

//            sGenInfo[i].u32LNPower1 = sGenInfo[i].u32LNVolt1 * sGenInfo[i].u32LNCurr1 * sGenInfo[i].fPF1 * 0.01;
//            sGenInfo[i].u32LNPower2 = sGenInfo[i].u32LNVolt2 * sGenInfo[i].u32LNCurr2 * sGenInfo[i].fPF2 * 0.01;
//            sGenInfo[i].u32LNPower3 = sGenInfo[i].u32LNVolt3 * sGenInfo[i].u32LNCurr3 * sGenInfo[i].fPF3 * 0.01;
            
            modbusParseInfoInverse(ptr,0,0,&sGenInfo[i].u32LNSPower1,53,32);   
            modbusParseInfoInverse(ptr,0,0,&sGenInfo[i].u32LNSPower2,55,32);
            modbusParseInfoInverse(ptr,0,0,&sGenInfo[i].u32LNSPower3,57,32);
            
            modbusParseInfoInverse(ptr,0,0,&sGenInfo[i].u32LNPower1,61,32);   
            modbusParseInfoInverse(ptr,0,0,&sGenInfo[i].u32LNPower2,63,32);
            modbusParseInfoInverse(ptr,0,0,&sGenInfo[i].u32LNPower3,65,32);
                       
            modbusParseInfo(ptr,0,&sGenInfo[i].u16Error1Status,0,32,16);
            modbusParseInfo(ptr,0,&sGenInfo[i].u16Error2Status,0,35,16);
            
            modbusParseInfo(ptr,0,&sGenInfo[i].u16StatusBits1,0,37,16);
            modbusParseInfo(ptr,0,&sGenInfo[i].u16StatusBits2,0,38,16);
            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_KUBOTA_INFO_2;
        }
        break;
        case GEN_KUBOTA_INFO_2:
        {             
            uint16_t u16temp1, u16temp2 ;
            modbusParseInfo(ptr,0,&u16temp1,0,0,16);
            u16temp2 = u16temp1;
            sGenInfo[i].u16ErrorBattVolt = 0;
            if(((u16temp1 >> 1) & 0x01) == 1) sGenInfo[i].u16ErrorBattVolt = 1;
            if(((u16temp2 >> 2) & 0x01) == 1) sGenInfo[i].u16ErrorBattVolt = 2;   
            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_KUBOTA_INFO_1;
        }
        break;
        default:
            break;
        };
    }
    break;
    case 2:// BE142
    {
        switch(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep)
        {
        case GEN_BE142_INFO_1:
        {

            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNVolt1,0,0,16); 
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNCurr1,0,6,16);             
//            
//            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32MainFrequency,0,6,16);         
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32GenFrequency,0,9,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32BattVolt,0,30,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32EngineSpeed,0,32,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32OilPressure,0,34,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32CoolantTemp,0,35,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32FuelLevel,0,36,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32EngineWorkingHour,0,37,16);
                                   
//            modbusParseInfo(ptr,0,&sGenInfo[i].u16Error1Status,0,32,16);
//            modbusParseInfo(ptr,0,&sGenInfo[i].u16Error2Status,0,35,16);
            
            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_BE142_INFO_2;
        }
        break;
        case GEN_BE142_INFO_2:
        {             
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNCurr2,0,0,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNCurr3,0,1,16); 
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNVolt2,0,2,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNVolt3,0,3,16); 
            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_BE142_INFO_3;
        }
        break;
        case GEN_BE142_INFO_3:
        {         
            uint16_t gen_high_frq, gen_low_frq, gen_high_vol, gen_low_vol, high_curr, high_curr1, high_curr2, low_oil_press, low_oil_press1, low_oil_press2, high_coolant_temp,
              high_coolant_temp1, high_coolant_temp2, low_batt_vol, high_bat_vol, low_fuel;
            
            modbusParseInfo(ptr,0,&gen_low_frq,0,0,16);
            modbusParseInfo(ptr,0,&gen_high_frq,0,1,16);
            modbusParseInfo(ptr,0,&gen_high_vol,0,3,16);
            modbusParseInfo(ptr,0,&gen_low_vol,0,4,16);
            modbusParseInfo(ptr,0,&high_curr1,0,10,16);
            modbusParseInfo(ptr,0,&high_curr2,0,11,16);
            if((high_curr1)||(high_curr2))
              high_curr = 1;
            else
              high_curr = 0;
            modbusParseInfo(ptr,0,&low_oil_press1,0,14,16);
            modbusParseInfo(ptr,0,&low_oil_press2,0,15,16);
            if((low_oil_press1)||(low_oil_press2))
              low_oil_press = 1;
            else
              low_oil_press = 0;
            modbusParseInfo(ptr,0,&high_coolant_temp1,0,18,16);
            modbusParseInfo(ptr,0,&high_coolant_temp2,0,21,16);
            if((high_coolant_temp1)||(high_coolant_temp2))
              high_coolant_temp = 1;
            else
              high_coolant_temp = 0;
            modbusParseInfo(ptr,0,&low_batt_vol,0,23,16);
            modbusParseInfo(ptr,0,&high_bat_vol,0,24,16);
            modbusParseInfo(ptr,0,&low_fuel,0,26,16);
            
            sGenInfo[i].u32AlarmStatus = low_fuel|(gen_low_frq<<1)|(gen_high_frq<<2)|(high_coolant_temp<<3)|(gen_low_vol<<4)|
                                          (gen_high_vol<<5)|(low_oil_press<<6)|(high_bat_vol<<7)|(low_batt_vol<<8)|(high_curr<<27);
            
            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_BE142_INFO_1;
        }
        break;
        default:
            break;
        };
    }
    break;
    case 3:// DEEPSEA
    {
        switch(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep)
        {
        case GEN_DEEPSEA_INFO_1:
        {
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32OilPressure,0,0,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32CoolantTemp,0,1,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32FuelLevel,0,3,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32BattVolt,0,5,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32EngineSpeed,0,6,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32GenFrequency,0,7,16);
            
//            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32EngineWorkingHour,14,32);   
                        
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32LNVolt1,8,32);
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32LNVolt2,10,32);
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32LNVolt3,12,32);
            
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32LNCurr1,20,32);
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32LNCurr2,22,32);
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32LNCurr3,24,32);
            
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32LNPower1,28,32);
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32LNPower2,30,32);
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32LNPower3,32,32);
            
            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_DEEPSEA_INFO_2;
        }
        break;
        case GEN_DEEPSEA_INFO_2:
        {      
            
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32EngineWorkingMin,6,32);  
            sGenInfo[i].u32EngineWorkingMin = sGenInfo[i].u32EngineWorkingMin / 60;
            sGenInfo[i].u32EngineWorkingHour = sGenInfo[i].u32EngineWorkingMin / 60;
            sGenInfo[i].u32EngineWorkingMin = sGenInfo[i].u32EngineWorkingMin % 60;
            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_DEEPSEA_INFO_3;
        }
        break;
        case GEN_DEEPSEA_INFO_3:
        {             
            uint16_t u16_temp;
            uint8_t gen_high_vol, gen_low_vol, gen_low_frq, gen_high_frq, high_curr, low_fuel, high_coolant_temp, low_oil_press, low_batt_vol, high_bat_vol,emergency_stop; 
            
            modbusParseInfo(ptr,0,&u16_temp,0,1,16);
            emergency_stop = (uint8_t)((u16_temp>>12)&0x0F);           // emergency_stop
            if((emergency_stop==2)||(emergency_stop==3)) emergency_stop = 1;
            else emergency_stop = 0;
            low_oil_press = (uint8_t)((u16_temp>>8)&0x0F);     // Low_oil_press
            if((low_oil_press==2)||(low_oil_press==3)) low_oil_press = 1;
            else low_oil_press = 0;
            high_coolant_temp = (uint8_t)((u16_temp>>4)&0x0F); // high_coolant_temp
            if((high_coolant_temp==2)||(high_coolant_temp==3)) high_coolant_temp = 1; 
            else high_coolant_temp = 0; 
            
            modbusParseInfo(ptr,0,&u16_temp,0,3,16);
            gen_low_vol = (uint8_t)((u16_temp>>8)&0x0F);       // gen_low_vol
            if((gen_low_vol==2)||(gen_low_vol==3)) gen_low_vol = 1;
            else gen_low_vol = 0;
            gen_high_vol = (uint8_t)((u16_temp>>4)&0x0F);      // gen_high_vol
            if((gen_high_vol==2)||(gen_high_vol==3)) gen_high_vol = 1;
            else gen_high_vol = 0;
            gen_low_frq = (uint8_t)(u16_temp&0x0F);      // gen_low_frq
            if((gen_low_frq==2)||(gen_low_frq==3)) gen_low_frq = 1;     
            else gen_low_frq = 0;
            
            modbusParseInfo(ptr,0,&u16_temp,0,4,16);
            gen_high_frq = (uint8_t)((u16_temp>>12)&0x0F);           // gen_high_frq
            if((gen_high_frq==2)||(gen_high_frq==3)) gen_high_frq = 1;
            else gen_high_frq = 0;
            high_curr = (uint8_t)((u16_temp>>8)&0x0F);         // high_curr
            if((high_curr==2)||(high_curr==3)) high_curr = 1;
            else high_curr = 0;
            
            modbusParseInfo(ptr,0,&u16_temp,0,7,16);
            low_batt_vol = (uint8_t)((u16_temp>>12)&0x0F);           // low_batt_vol
            if((low_batt_vol==2)||(low_batt_vol==3)) low_batt_vol = 1;
            else low_batt_vol = 0;      
            high_bat_vol = (uint8_t)((u16_temp>>8)&0x0F);      // high_bat_vol
            if((high_bat_vol==2)||(high_bat_vol==3)) high_bat_vol = 1;
            else high_bat_vol = 0;
            low_fuel = (uint8_t)((u16_temp>>4)&0x0F);          // low_fuel
            if((low_fuel==2)||(low_fuel==3)) low_fuel = 1;
            else low_fuel = 0;
            
            sGenInfo[i].u32AlarmStatus = low_fuel|(gen_low_frq<<1)|(gen_high_frq<<2)|(high_coolant_temp<<3)|(gen_low_vol<<4)|
                                          (gen_high_vol<<5)|(low_oil_press<<6)|(high_bat_vol<<7)|(low_batt_vol<<8)|(emergency_stop<<16)|(high_curr<<27);
            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_DEEPSEA_INFO_4;
        }
        break;
        case GEN_DEEPSEA_INFO_4:
        {      
            modbusParseInfo(ptr,0,&sGenInfo[i].u16StatusBits1,0,0,16);
            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_DEEPSEA_INFO_1;
        }
        break;
        default:
            break;
        };
    }
    break;
    case 4:// LR2057
    {
        switch(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep)
        {
        case GEN_LR2057_INFO_1:
        {
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32MainFrequency,0,0,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNVolt1,0,1,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNVolt2,0,2,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNVolt3,0,3,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32GenFrequency,0,7,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNCurr1,0,8,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNCurr2,0,9,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNCurr3,0,10,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32CoolantTemp,0,11,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32OilPressure,0,13,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32FuelLevel,0,15,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32EngineSpeed,0,17,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32BattVolt,0,18,16);
          
            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_LR2057_INFO_2;
        }
        break;
        case GEN_LR2057_INFO_2:
        {      
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32EngineWorkingHour,0,32);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32EngineWorkingMin,0,2,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNPower1,0,10,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNPower2,0,11,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNPower3,0,12,16);
            
            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_LR2057_INFO_3;
        }
        break;
        case GEN_LR2057_INFO_3:
        {      
            uint8_t gen_high_vol, gen_low_vol, gen_low_frq, gen_high_frq, high_curr, low_fuel, low_oil_press, low_batt_vol, high_bat_vol,high_temp; 
            uint8_t gen_high_vol_1, gen_low_vol_1, gen_low_frq_1, gen_high_frq_1, high_curr_1, high_curr_2, low_fuel_1, low_oil_press_1,high_temp1; 
            modbusParsebit(ptr,&gen_high_frq,0,0);
            modbusParsebit(ptr,&gen_low_frq,0,1);
            modbusParsebit(ptr,&gen_high_vol,0,2);
            modbusParsebit(ptr,&gen_low_vol,0,3);
            modbusParsebit(ptr,&high_curr,0,4);
            modbusParsebit(ptr,&high_temp,0,6);
            modbusParsebit(ptr,&low_oil_press,0,7);
            modbusParsebit(ptr,&low_fuel,1,2);
            modbusParsebit(ptr,&high_temp1,1,4);
            modbusParsebit(ptr,&low_oil_press_1,1,5);
            modbusParsebit(ptr,&high_curr_1,1,6);
            modbusParsebit(ptr,&low_fuel_1,2,0);
            modbusParsebit(ptr,&low_batt_vol,2,2);
            modbusParsebit(ptr,&high_bat_vol,2,3);
            modbusParsebit(ptr,&gen_high_vol_1,7,5);
            modbusParsebit(ptr,&gen_low_vol_1,7,6);
            modbusParsebit(ptr,&gen_high_frq_1,7,7);
            modbusParsebit(ptr,&gen_low_frq_1,8,0);
            modbusParsebit(ptr,&high_curr_2,8,1);
            
            if(gen_high_vol_1 == 1) gen_high_vol = 1;
            if(gen_low_vol_1 == 1) gen_low_vol = 1;
            if(gen_low_frq_1 == 1) gen_low_frq = 1;
            if(gen_high_frq_1 == 1) gen_high_frq = 1;
            if(high_temp1 == 1) high_temp = 1;
            if(low_fuel_1 == 1) low_fuel = 1;
            if(low_oil_press_1 == 1) low_oil_press = 1;
            if((high_curr_1 == 1) || (high_curr_2 == 1)) high_curr = 1;
            
            sGenInfo[i].u32AlarmStatus = low_fuel|(gen_low_frq<<1)|(gen_high_frq<<2)|(high_temp<<3)|(gen_low_vol<<4)|(gen_high_vol<<5)|
                                          (low_oil_press<<6)|(high_bat_vol<<7)|(low_batt_vol<<8)|(high_curr<<27);        
            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_LR2057_INFO_1;
        }
        break;
        
        default:
            break;
        };
    }
    case 5:// HIMOINSA
    {
        switch(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep)
        {
        case GEN_HIMOINSA_INFO_1:
        {
            uint8_t high_coolant_temp, low_oil_press, emergency_stop, gen_high_vol, gen_high_frq, low_batt_vol, low_fuel, gen_low_vol, gen_low_frq;
            
            modbusParsebit(ptr,&high_coolant_temp,0,0);
            modbusParsebit(ptr,&low_oil_press,0,1);
            modbusParsebit(ptr,&emergency_stop,0,2);
            
            modbusParsebit(ptr,&gen_high_vol,1,3);
            modbusParsebit(ptr,&gen_high_frq,1,4);
            modbusParsebit(ptr,&low_batt_vol,1,7);
            
            modbusParsebit(ptr,&low_fuel,2,2);
            modbusParsebit(ptr,&gen_low_vol,2,4);
            modbusParsebit(ptr,&gen_low_frq,2,5);       
          
            sGenInfo[i].u32AlarmStatus = low_fuel|(gen_low_frq<<1)|(gen_high_frq<<2)|(high_coolant_temp<<3)|(gen_low_vol<<4)|(gen_high_vol<<5)|
                                          (low_oil_press<<6)|(low_batt_vol<<8)|(emergency_stop<<16);
            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_HIMOINSA_INFO_2;
        }
        break;
        case GEN_HIMOINSA_INFO_2:
        {      
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32GenFrequency,0,0,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNVolt1,0,4,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNVolt2,0,5,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNVolt3,0,6,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNCurr1,0,7,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNCurr2,0,8,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNCurr3,0,9,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNPower1,0,15,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32EngineSpeed,0,18,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32FuelLevel,0,19,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32BattVolt,0,21,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32CoolantTemp,0,22,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32OilPressure,0,23,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32EngineWorkingHour,0,35,16);
            
//            sGenInfo[i].u32EngineWorkingHour = sGenInfo[i].u32EngineWorkingMin / 60;
//            sGenInfo[i].u32EngineWorkingMin = sGenInfo[i].u32EngineWorkingMin % 60;
            
            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_HIMOINSA_INFO_1;
        }
        break;
        default:
            break;
        };
    }
    break;
    
    case 6:// QC315
    {
        switch(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep)
        {
        case GEN_QC315_INFO_1:
        { 
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32LNCurr1,0,32);
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32LNCurr2,2,32);
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32LNCurr3,4,32);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32GenFrequency,0,8,16);
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32LNPower1,10,32);
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32LNPower2,20,32);
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32LNPower3,30,32);
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32LNSPower1,14,32);
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32LNSPower2,24,32);
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32LNSPower3,34,32);
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32EngineWorkingHour,51,32);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32BattVolt,0,53,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32OilPressure,0,56,16);
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32CoolantTemp,57,32);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32FuelLevel,0,59,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32EngineSpeed,0,60,16);
            
            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_QC315_INFO_2;
        }
        break;
        case GEN_QC315_INFO_2:
        {    
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32LNVolt1,0,32);
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32LNVolt2,2,32);
            modbusParseInfo(ptr,0,0,&sGenInfo[i].u32LNVolt3,4,32);
            
            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_QC315_INFO_3;
        }
        break;
        case GEN_QC315_INFO_3:
        {             
            uint16_t alarmTemp0, alarmTemp1, alarmTemp3, alarmTemp4, alarmTemp5, alarmTemp6;
            uint8_t gen_low_vol, gen_high_vol, gen_high_frq, gen_low_frq, over_curr, low_fuel, high_coolant_temp, low_oil_press, low_batt_vol, high_batt_volt;
            
            modbusParseInfo(ptr,0,(uint16_t*) &alarmTemp0,0,0,16);
            modbusParseInfo(ptr,0,(uint16_t*) &alarmTemp1,0,1,16);
            modbusParseInfo(ptr,0,(uint16_t*) &alarmTemp3,0,3,16);
            modbusParseInfo(ptr,0,(uint16_t*) &alarmTemp4,0,4,16);
            modbusParseInfo(ptr,0,(uint16_t*) &alarmTemp5,0,5,16);
            modbusParseInfo(ptr,0,(uint16_t*) &alarmTemp6,0,6,16);
            
            if((alarmTemp1&0x200) || (alarmTemp6&0x04))
              gen_low_vol = 1;
            else
              gen_low_vol = 0;
            if((alarmTemp1&0x800) || (alarmTemp6&0x08))
              gen_high_vol = 1;
            else
              gen_high_vol = 0;
            if((alarmTemp1&0x1000) || (alarmTemp6&0x02))
              gen_high_frq = 1;
            else
              gen_high_frq = 0;
            if((alarmTemp1&0x400) || (alarmTemp6&0x01))
              gen_low_frq = 1;
            else
              gen_low_frq = 0;
            if((alarmTemp1&0x4000) || (alarmTemp1&0x2000))
              over_curr = 1;
            else
              over_curr = 0;
            if((alarmTemp0&0x40) || (alarmTemp0&0x80) || (alarmTemp0&0x100) || (alarmTemp0&0x200)) 
              low_fuel = 1; 
            else
              low_fuel = 0;
            if((alarmTemp0&0x1000) || (alarmTemp0&0x2000) || (alarmTemp0&0x4000) || (alarmTemp0&0x8000)|| (alarmTemp4&0x1000)|| (alarmTemp4&0x2000))
              high_coolant_temp = 1;
            else
              high_coolant_temp = 0;
            if((alarmTemp1&0x1) || (alarmTemp1&0x2) || (alarmTemp1&0x4) || (alarmTemp1&0x8)|| (alarmTemp4&0x100)|| (alarmTemp4&0x200))
              low_oil_press = 1;
            else
              low_oil_press = 0;
            if((alarmTemp3&0x20) || (alarmTemp5&0x2))
              low_batt_vol = 1;
            else
              low_batt_vol = 0;
            if(alarmTemp3&0x40)
              high_batt_volt = 1;
            else
              high_batt_volt = 0;
            
            sGenInfo[i].u32AlarmStatus = low_fuel|(gen_low_frq<<1)|(gen_high_frq<<2)|(high_coolant_temp<<3)|(gen_low_vol<<4)|(gen_high_vol<<5)|
                                          (low_oil_press<<6)|(high_batt_volt<<7)|(low_batt_vol<<8)|(over_curr<<27);
            
            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_QC315_INFO_1;
        }
        break;
        default:
            break;
        };
    }
    break;
    case 7:// CUMMIN
    {
        switch(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep)
        {
        case GEN_CUMMIN_INFO_1:
        { 
//            uint16_t GenErr = 0;
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32AlarmStatus ,0,0,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNVolt1,0,6,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNVolt2,0,7,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNVolt3,0,8,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNCurr1,0,14,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNCurr2,0,15,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNCurr3,0,16,16);
//            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNPower1,0,19,16);
//            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNPower2,0,20,16);
//            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNPower3,0,21,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNSPower1,0,28,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNSPower2,0,29,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32LNSPower3,0,30,16);
            
            //thanhcm3 fix -----------------------------------------------------------------------------------
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32GenFrequency,0,32,16);
            //thanhcm3 fix -----------------------------------------------------------------------------------
            
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32BattVolt,0,49,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32OilPressure,0,50,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32CoolantTemp,0,52,16);
            modbusParseInfo(ptr,0,(uint16_t*) &sGenInfo[i].u32EngineSpeed,0,56,16);
            modbusParseInfo(ptr,0,0, &sGenInfo[i].u32EngineWorkingHour,58,32);
            
            
            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_CUMMIN_INFO_1;
        }
        break;
        default:
            break;
        };
    }
    break;
    default:
        break;
    };
}
// ma lenh 16: GEN
void SetVar_i16GEN(uint8_t slaveAddr, uint16_t regAddr, int16_t val)
{
  uint8_t u8DataBuff[4];
  u8DataBuff[0] = (uint8_t) (val >> 8);
  u8DataBuff[1] = (uint8_t) (val);
  Write_Multi_Reg (slaveAddr, regAddr, &u8DataBuff[0], 1);
  vTaskDelay(500);
  if(RS4851_Check_Respond_Data_GEN() != 16){
      Write_Multi_Reg (slaveAddr, regAddr, &u8DataBuff[0], 1);
      vTaskDelay(500);
      if(RS4851_Check_Respond_Data_GEN() != 16){
      Write_Multi_Reg (slaveAddr, regAddr, &u8DataBuff[0], 1);
    }
  }
}
// ma lenh 06
void SetVar_FC06GEN(uint8_t slaveAddr, uint16_t regAddr, uint16_t Val)
{
  Write_Single_Reg(slaveAddr, regAddr, Val);
  vTaskDelay(500);
  if(RS4851_Check_Respond_Data_GEN() != 6){
      Write_Single_Reg(slaveAddr, regAddr, Val);
      vTaskDelay(500);
      if(RS4851_Check_Respond_Data_GEN() != 6){
      Write_Single_Reg(slaveAddr, regAddr, Val);
    }
  }
}
#endif


#if (USERDEF_MONITOR_PM == ENABLED)
void Extract_Holding_Regs_Data_PM(void)
{
    uint8_t i,j;
    uint8_t *ptr = &Modbus.u8BuffRead[0];
    switch(Modbus.u8SlaveID - sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMSlaveOffset)
    {
    case 1:
        i = 0;
        break;
    case 2:
        i = 1;
        break;
    };
    switch(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMType)
    {
    case 1:// FINECO
    {
        switch(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep)
        {
        case PM_FINECO_INFO_1:
        {
          for(j = 0; j < 3; j++)
          {
            modbusParseInfof(ptr, &sPMInfo[i].sPhaseInfo[j].fVoltage, 0 + j*2);
          }
          for(j = 0; j < 3; j++)
          {
            modbusParseInfof(ptr, &sPMInfo[i].sPhaseInfo[j].fCurrent, 6 + j*2);
          }
          modbusParseInfof(ptr, &sPMInfo[i].fNeutralCurrent, 12);
          modbusParseInfof(ptr, &sPMInfo[i].fTotalCurrent, 14);
          for(j = 0; j < 3; j++)
          {
            modbusParseInfof(ptr, &sPMInfo[i].sPhaseInfo[j].fActivePower, 16 + j*2);
          }
          modbusParseInfof(ptr, &sPMInfo[i].fRealPower, 22);
          for(j = 0; j < 3; j++)
          {
            modbusParseInfof(ptr, &sPMInfo[i].sPhaseInfo[j].fReactivePower, 24 + j*2);
          }
          modbusParseInfof(ptr, &sPMInfo[i].fReactivePower, 30);          
          for(j = 0; j < 3; j++)
          {
            modbusParseInfof(ptr, &sPMInfo[i].sPhaseInfo[j].fApparentPower, 32 + j*2);
          }
          modbusParseInfof(ptr, &sPMInfo[i].fApparentPower, 38); 
          for(j = 0; j < 3; j++)
          {
            modbusParseInfof(ptr, &sPMInfo[i].sPhaseInfo[j].fPowerFactor, 40 + j*2);
          }
          modbusParseInfof(ptr, &sPMInfo[i].fPowerFactor, 46);
          modbusParseInfof(ptr, &sPMInfo[i].fFrequency, 48);
           
           sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_FINECO_INFO_2;
        }
        break;
        case PM_FINECO_INFO_2:
        {
          modbusParseInfof(ptr, &sPMInfo[i].fRealEnergy, 0);
           sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_FINECO_INFO_1;
        }
        break;
        default:
            break;
        };
    }
    break;
    case 2:// ASCENT
    {
        switch(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep)
        {
        case PM_ASCENT_INFO_1:
        {   
           modbusParseInfof(ptr, &sPMInfo[i].fRealEnergy, 0);     
           sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_ASCENT_INFO_2;
        }
        break;
        case PM_ASCENT_INFO_2:
        {
           modbusParseInfof(ptr, &sPMInfo[i].sPhaseInfo[0].fVoltage, 0);
           sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_ASCENT_INFO_3;
        }
        break;
        case PM_ASCENT_INFO_3:
        {
          modbusParseInfof(ptr, &sPMInfo[i].sPhaseInfo[0].fCurrent, 0);
           sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_ASCENT_INFO_4;
        }
        break;
        case PM_ASCENT_INFO_4:
        {
          modbusParseInfof(ptr, &sPMInfo[i].sPhaseInfo[0].fActivePower, 0);
           sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_ASCENT_INFO_5;
        }
        break;
        case PM_ASCENT_INFO_5:
        {
          modbusParseInfof(ptr, &sPMInfo[i].sPhaseInfo[0].fPowerFactor, 0);
           sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_ASCENT_INFO_6;
        }
        break;
        case PM_ASCENT_INFO_6:
        {
          modbusParseInfof(ptr, &sPMInfo[i].fFrequency, 0);
           sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_ASCENT_INFO_1;
        }
        break;
        default:
            break;
        };
    }
    break;
    case 3:// EASTRON
    {
        switch(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep)
        {
        case PM_EASTRON_INFO_1:
        {   
           modbusParseInfof(ptr, &sPMInfo[i].sPhaseInfo[0].fVoltage,0);   
           modbusParseInfof(ptr, &sPMInfo[i].sPhaseInfo[0].fCurrent,6);   
           modbusParseInfof(ptr, &sPMInfo[i].sPhaseInfo[0].fActivePower,12);   
           modbusParseInfof(ptr, &sPMInfo[i].sPhaseInfo[0].fApparentPower,18);   
           modbusParseInfof(ptr, &sPMInfo[i].sPhaseInfo[0].fReactivePower,24);   
           modbusParseInfof(ptr, &sPMInfo[i].sPhaseInfo[0].fPowerFactor,30);   
           sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_EASTRON_INFO_2;
        }
        break;
        case PM_EASTRON_INFO_2:
        {
          float u32ImportActiveE_PM, u32ExportActiveE_PM, u32ImportReActiveE_PM, u32ExportReActiveE_PM;
          
           modbusParseInfof(ptr, &sPMInfo[i].fFrequency, 0);
           modbusParseInfof(ptr, &u32ImportActiveE_PM, 2);
           modbusParseInfof(ptr, &u32ExportActiveE_PM, 4);
           modbusParseInfof(ptr, &u32ImportReActiveE_PM, 6);
           modbusParseInfof(ptr, &u32ExportReActiveE_PM, 8);  
           
           sPMInfo[i].u32ImportActiveE = (int32_t)(u32ImportActiveE_PM*100);
           sPMInfo[i].u32ExportActiveE = (int32_t)(u32ExportActiveE_PM*100);
           sPMInfo[i].u32ImportReActiveE = (int32_t)(u32ImportReActiveE_PM*100);
           sPMInfo[i].u32ExportReActiveE = (int32_t)(u32ExportReActiveE_PM*100);
           sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_EASTRON_INFO_3;
        }
        break;
        case PM_EASTRON_INFO_3:
        {       
           float u32TotalActiveE_PM, u32TotalReActiveE_PM;
           
           modbusParseInfof(ptr, &u32TotalActiveE_PM, 0);
           modbusParseInfof(ptr, &u32TotalReActiveE_PM, 2);           
           
           sPMInfo[i].u32TotalActiveE = (int32_t)(u32TotalActiveE_PM*100);
           sPMInfo[i].u32TotalReActiveE = (int32_t)(u32TotalReActiveE_PM*100);
           sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_EASTRON_INFO_4;
        }
        break;
        case PM_EASTRON_INFO_4:
        {       
           modbusParseInfo(ptr,0,0,&sPMInfo[i].u32SerialNumber,0,32);
           sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_EASTRON_INFO_1;
        }
        break;
        default:
            break;
        };
    }
    break;
    case 4:// CET1
    {
        switch(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep)
        {
        case PM_CET1_INFO_1:
        {   
           modbusParseInfof(ptr, &sPMInfo[i].sPhaseInfo[0].fVoltage, 0);    
           modbusParseInfof(ptr, &sPMInfo[i].sPhaseInfo[0].fCurrent, 2);    
           modbusParseInfof(ptr, &sPMInfo[i].sPhaseInfo[0].fActivePower, 4);    
           modbusParseInfof(ptr, &sPMInfo[i].sPhaseInfo[0].fReactivePower, 6);    
           modbusParseInfof(ptr, &sPMInfo[i].sPhaseInfo[0].fApparentPower, 8);    
           modbusParseInfof(ptr, &sPMInfo[i].sPhaseInfo[0].fPowerFactor, 10);    
           modbusParseInfof(ptr, &sPMInfo[i].sPhaseInfo[0].fFrequency, 12); 
           
           sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_CET1_INFO_2;
        }
        break;
        case PM_CET1_INFO_2:
        {
          modbusParseInfoSign(ptr, 0,0,&sPMInfo[i].u32ImportActiveE,0, 32);
          modbusParseInfoSign(ptr, 0,0,&sPMInfo[i].u32ExportActiveE,2, 32);
          modbusParseInfoSign(ptr, 0,0,&sPMInfo[i].u32NetActiveE,4, 32);
          modbusParseInfoSign(ptr, 0,0,&sPMInfo[i].u32TotalActiveE,6, 32);
          modbusParseInfoSign(ptr, 0,0,&sPMInfo[i].u32ImportReActiveE,8, 32);
          modbusParseInfoSign(ptr, 0,0,&sPMInfo[i].u32ExportReActiveE,10, 32);
          modbusParseInfoSign(ptr, 0,0,&sPMInfo[i].u32NetReActiveE,12, 32);
          modbusParseInfoSign(ptr, 0,0,&sPMInfo[i].u32TotalReActiveE,14, 32);
          
           sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_CET1_INFO_3;
        }
        break;
        case PM_CET1_INFO_3:
        {
          modbusParseInfoString(ptr,&sPMInfo[i].u8Model[0], 20, 0);
          modbusParseInfo(ptr, 0,0,&sPMInfo[i].u32SerialNumber,25, 32);
          
           sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_CET1_INFO_1;
        }
        break;
        default:
            break;
        };
    }
    break;
    case 5:// PILOT
    {
      switch(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep)
      {
        case PM_PILOT_INFO_1:
        {
          uint16_t u16Voltage_PM, u16Frequency_PM, u16PowerFactor_PM;
          uint32_t u32Current_PM;
          int32_t u32ActivePower, u32ApparentPower, u32ReactivePower;
          modbusParseInfoInverse(ptr, 0,0,&sPMInfo[i].u32TotalActiveE,0, 32);
          modbusParseInfo(ptr,0, &u16Voltage_PM,0,2,16);
          modbusParseInfoInverse(ptr,0,0,&u32Current_PM,3,32);
          modbusParseInfoSignInverse(ptr,0,0,&u32ActivePower,5,32);
          modbusParseInfoSignInverse(ptr,0,0,&u32ApparentPower,7,32);
          modbusParseInfoSignInverse(ptr,0,0,&u32ReactivePower,9,32);          
          modbusParseInfo(ptr,0, &u16Frequency_PM,0,11,16);
          modbusParseInfo(ptr,0,&u16PowerFactor_PM,0,12,16);
          modbusParseInfoInverse(ptr, 0,0,&sPMInfo[i].u32ImportActiveE,13, 32);
          modbusParseInfoInverse(ptr, 0,0,&sPMInfo[i].u32ExportActiveE,15, 32);
          
          sPMInfo[i].sPhaseInfo[0].fVoltage = (float)u16Voltage_PM;
          sPMInfo[i].sPhaseInfo[0].fCurrent = (float)u32Current_PM;
          sPMInfo[i].sPhaseInfo[0].fActivePower = (float)u32ActivePower;
          sPMInfo[i].sPhaseInfo[0].fApparentPower = (float)u32ApparentPower;
          sPMInfo[i].sPhaseInfo[0].fReactivePower = (float)u32ReactivePower;
          sPMInfo[i].fFrequency = (float)u16Frequency_PM;
          sPMInfo[i].sPhaseInfo[0].fPowerFactor = (float)u16PowerFactor_PM;                                              
                    
          sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_PILOT_INFO_2;
        }
        break;
        case PM_PILOT_INFO_2:
        {

          modbusParseInfoSignInverse(ptr, 0,0,&sPMInfo[i].u32ImportReActiveE,0, 32);
          modbusParseInfoSignInverse(ptr, 0,0,&sPMInfo[i].u32ExportReActiveE,2, 32);
          modbusParseInfoSignInverse(ptr, 0,0,&sPMInfo[i].u32TotalReActiveE,4, 32);

          sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_PILOT_INFO_1;
        }
        break;
        default:
            break;
      };
    }
    break;
    case 15:// PILOT_3PHASE
    {
      switch(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep)
      {
        case PM_PILOT_3PHASE_INFO_1:
        {          
          uint16_t u16tempPM = 0;
          uint32_t u32tempPM = 0;
          int32_t i32tempPM = 0;
          int16_t i16tempPM = 0;
          modbusParseInfo(ptr,0, &u16tempPM,0,0,16);
          sPMInfo[i].sPhaseInfo[0].fVoltage = (float)u16tempPM;
          modbusParseInfo(ptr,0, &u16tempPM,0,1,16);
          sPMInfo[i].sPhaseInfo[1].fVoltage = (float)u16tempPM;
          modbusParseInfo(ptr,0, &u16tempPM,0,2,16);
          sPMInfo[i].sPhaseInfo[2].fVoltage = (float)u16tempPM;
          
          modbusParseInfoInverse(ptr,0, 0,&u32tempPM,6,32);
          sPMInfo[i].sPhaseInfo[0].fCurrent = (float)(u32tempPM/10);
          modbusParseInfoInverse(ptr,0, 0,&u32tempPM,8,32);
          sPMInfo[i].sPhaseInfo[1].fCurrent = (float)(u32tempPM/10);
          modbusParseInfoInverse(ptr,0, 0,&u32tempPM,10,32);
          sPMInfo[i].sPhaseInfo[2].fCurrent = (float)(u32tempPM/10);
          modbusParseInfoInverse(ptr,0, 0,&u32tempPM,12,32);
          sPMInfo[i].fTotalCurrent = (float)(u32tempPM/10);
          
          modbusParseInfoSignInverse(ptr,0, 0,&i32tempPM,14,32);
          sPMInfo[i].sPhaseInfo[0].fActivePower = (float)(i32tempPM/1000);
          modbusParseInfoSignInverse(ptr,0, 0,&i32tempPM,16,32);
          sPMInfo[i].sPhaseInfo[1].fActivePower = (float)(i32tempPM/1000);
          modbusParseInfoSignInverse(ptr,0, 0,&i32tempPM,18,32);
          sPMInfo[i].sPhaseInfo[2].fActivePower = (float)(i32tempPM/1000);
          modbusParseInfoSignInverse(ptr,0, 0,&i32tempPM,20,32);
          sPMInfo[i].fRealPower = (float)(i32tempPM/1000);
          
          modbusParseInfoSignInverse(ptr,0, 0,&i32tempPM,22,32);
          sPMInfo[i].sPhaseInfo[0].fReactivePower = (float)(i32tempPM/1000);
          modbusParseInfoSignInverse(ptr,0, 0,&i32tempPM,24,32);
          sPMInfo[i].sPhaseInfo[1].fReactivePower = (float)(i32tempPM/1000);
          modbusParseInfoSignInverse(ptr,0, 0,&i32tempPM,26,32);
          sPMInfo[i].sPhaseInfo[2].fReactivePower = (float)(i32tempPM/1000);
          modbusParseInfoSignInverse(ptr,0, 0,&i32tempPM,28,32);
          sPMInfo[i].fReactivePower = (float)(i32tempPM/1000);
          
          modbusParseInfoInverse(ptr,0, 0,&u32tempPM,30,32);
          sPMInfo[i].sPhaseInfo[0].fApparentPower = (float)(u32tempPM/1000);
          modbusParseInfoInverse(ptr,0, 0,&u32tempPM,32,32);
          sPMInfo[i].sPhaseInfo[1].fApparentPower = (float)(u32tempPM/1000);
          modbusParseInfoInverse(ptr,0, 0,&u32tempPM,34,32);
          sPMInfo[i].sPhaseInfo[2].fApparentPower = (float)(u32tempPM/1000);
          modbusParseInfoInverse(ptr,0, 0,&u32tempPM,36,32);
          sPMInfo[i].fApparentPower = (float)(u32tempPM/1000);
          
          modbusParseInfoSign(ptr,0,&i16tempPM, 0,38,16);
          sPMInfo[i].sPhaseInfo[0].fPowerFactor = (float)(i16tempPM/10);
          modbusParseInfoSign(ptr,0,&i16tempPM, 0,39,16);
          sPMInfo[i].sPhaseInfo[1].fPowerFactor = (float)(i16tempPM/10);
          modbusParseInfoSign(ptr,0,&i16tempPM, 0,40,16);
          sPMInfo[i].sPhaseInfo[2].fPowerFactor = (float)(i16tempPM/10);
          modbusParseInfoSign(ptr,0,&i16tempPM, 0,41,16);
          sPMInfo[i].fPowerFactor = (float)(i16tempPM/10);
          
          modbusParseInfo(ptr,0,&u16tempPM, 0,42,16);
          sPMInfo[i].fFrequency = (float)(u16tempPM/10);
          
          sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_PILOT_3PHASE_INFO_2;
        }
        break;
        case PM_PILOT_3PHASE_INFO_2:
        {
          uint32_t u32tempPM2 = 0;
          
          modbusParseInfoInverse(ptr,0, 0,&u32tempPM2,0,32);
          sPMInfo[i].u32ImportActiveE = (int32_t)(u32tempPM2*10);
          modbusParseInfoInverse(ptr,0, 0,&u32tempPM2,2,32);
          sPMInfo[i].u32ExportActiveE = (int32_t)(u32tempPM2*10);
          modbusParseInfoInverse(ptr,0, 0,&u32tempPM2,4,32);
          sPMInfo[i].u32TotalActiveE = (int32_t)(u32tempPM2*10);
          
          modbusParseInfoInverse(ptr,0, 0,&u32tempPM2,6,32);
          sPMInfo[i].u32ImportReActiveE = (int32_t)(u32tempPM2*10);
          modbusParseInfoInverse(ptr,0, 0,&u32tempPM2,8,32);
          sPMInfo[i].u32ExportReActiveE = (int32_t)(u32tempPM2*10);
          modbusParseInfoInverse(ptr,0, 0,&u32tempPM2,10,32);
          sPMInfo[i].u32TotalReActiveE = (int32_t)(u32tempPM2*10);
          
          sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_PILOT_3PHASE_INFO_3;
        }
        break;
        case PM_PILOT_3PHASE_INFO_3:
        {
          uint32_t u32tempPM2 = 0;
          
          modbusParseInfoInverse(ptr,0, 0,&u32tempPM2,0,32);
          sPMInfo[i].u32SerialNumber = u32tempPM2;
          
          sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_PILOT_3PHASE_INFO_1;
        }
        break;
        default:
            break;
      };
    }
    break;
    case 16://YADA_3PHASE_DPC
    {
      switch(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep){
        case PM_YADA_3PHASE_DPC_INFO_1:
        {
          uint32_t u32yadatempPM = 0;
          modbusParseInfo(ptr,0, 0,&u32yadatempPM,0,32);
          sPMInfo[i].sPhaseInfo[0].fVoltage = ((float)u32yadatempPM)/100.0;
          modbusParseInfo(ptr,0, 0,&u32yadatempPM,2,32);
          sPMInfo[i].sPhaseInfo[1].fVoltage = ((float)u32yadatempPM)/100.0;
          modbusParseInfo(ptr,0, 0,&u32yadatempPM,4,32);
          sPMInfo[i].sPhaseInfo[2].fVoltage = ((float)u32yadatempPM)/100.0;
          sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_YADA_3PHASE_DPC_INFO_2;
        }
        break;
        case PM_YADA_3PHASE_DPC_INFO_2:
        {
          uint32_t u32yadatempPM2 = 0;
          modbusParseInfo(ptr,0, 0,&u32yadatempPM2,0,32);
          sPMInfo[i].sPhaseInfo[0].fCurrent = ((float)u32yadatempPM2)/100.0;
          modbusParseInfo(ptr,0, 0,&u32yadatempPM2,2,32);
          sPMInfo[i].sPhaseInfo[1].fCurrent = ((float)u32yadatempPM2)/100.0;
          modbusParseInfo(ptr,0, 0,&u32yadatempPM2,4,32);
          sPMInfo[i].sPhaseInfo[2].fCurrent = ((float)u32yadatempPM2)/100.0;
          sPMInfo[i].fTotalCurrent = sPMInfo[i].sPhaseInfo[0].fCurrent 
                                    +sPMInfo[i].sPhaseInfo[1].fCurrent
                                    +sPMInfo[i].sPhaseInfo[2].fCurrent;
          
          sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_YADA_3PHASE_DPC_INFO_3;
        }
        break;
        case PM_YADA_3PHASE_DPC_INFO_3:
        {
          int16_t i16yadatempPM3 = 0;
          modbusParseInfoSign(ptr,0, &i16yadatempPM3,0,0,16);
          sPMInfo[i].fPowerFactor = ((float)i16yadatempPM3)/10.0;
          modbusParseInfoSign(ptr,0, &i16yadatempPM3,0,1,16);
          sPMInfo[i].sPhaseInfo[0].fPowerFactor = ((float)i16yadatempPM3)/10.0;
          modbusParseInfoSign(ptr,0, &i16yadatempPM3,0,2,16);
          sPMInfo[i].sPhaseInfo[1].fPowerFactor = ((float)i16yadatempPM3)/10.0;
          modbusParseInfoSign(ptr,0, &i16yadatempPM3,0,3,16);
          sPMInfo[i].sPhaseInfo[2].fPowerFactor = ((float)i16yadatempPM3)/10.0;
          sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_YADA_3PHASE_DPC_INFO_4;
        }
        break;
        case PM_YADA_3PHASE_DPC_INFO_4:
        {
          uint16_t u16yadatempPM4 = 0;
          modbusParseInfo(ptr,0, &u16yadatempPM4,0,0,16);
          sPMInfo[i].fFrequency = ((float)u16yadatempPM4)/10.0;
          
          sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_YADA_3PHASE_DPC_INFO_5;
        }
        break;
        case PM_YADA_3PHASE_DPC_INFO_5:
        {
          uint32_t u32yadatempPM5 = 0;
          modbusParseInfo(ptr,0, 0,&u32yadatempPM5,0,32);
          sPMInfo[i].u32TotalReActiveE = u32yadatempPM5;
          
          sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_YADA_3PHASE_DPC_INFO_6;
        }
        break;
        case PM_YADA_3PHASE_DPC_INFO_6:
        {
          uint32_t u32yadatempPM6 = 0;
          modbusParseInfo(ptr,0, 0,&u32yadatempPM6,0,32);
          sPMInfo[i].u32TotalActiveE = u32yadatempPM6;
          sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_YADA_3PHASE_DPC_INFO_7;
        }
        break;
        case PM_YADA_3PHASE_DPC_INFO_7:
        {
          int32_t i32yadatempPM7 = 0;
          modbusParseInfoSign(ptr,0, 0,&i32yadatempPM7,0,32);
          sPMInfo[i].fRealPower = ((float)i32yadatempPM7)/100.0;
          modbusParseInfoSign(ptr,0, 0,&i32yadatempPM7,2,32);
          sPMInfo[i].sPhaseInfo[0].fActivePower = ((float)i32yadatempPM7)/100.0;
          modbusParseInfoSign(ptr,0, 0,&i32yadatempPM7,4,32);
          sPMInfo[i].sPhaseInfo[1].fActivePower = ((float)i32yadatempPM7)/100.0;
          modbusParseInfoSign(ptr,0, 0,&i32yadatempPM7,6,32);
          sPMInfo[i].sPhaseInfo[2].fActivePower = ((float)i32yadatempPM7)/100.0;
          sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_YADA_3PHASE_DPC_INFO_8;
        }
        break;
        case PM_YADA_3PHASE_DPC_INFO_8:
        {
          int32_t i32yadatempPM8 = 0;
          modbusParseInfoSign(ptr,0, 0,&i32yadatempPM8,0,32);
          sPMInfo[i].fReactivePower = ((float)i32yadatempPM8)/100.0;
          modbusParseInfoSign(ptr,0, 0,&i32yadatempPM8,2,32);
          sPMInfo[i].sPhaseInfo[0].fReactivePower = ((float)i32yadatempPM8)/100.0;
          modbusParseInfoSign(ptr,0, 0,&i32yadatempPM8,4,32);
          sPMInfo[i].sPhaseInfo[1].fReactivePower = ((float)i32yadatempPM8)/100.0;
          modbusParseInfoSign(ptr,0, 0,&i32yadatempPM8,6,32);
          sPMInfo[i].sPhaseInfo[2].fReactivePower = ((float)i32yadatempPM8)/100.0;
          sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_YADA_3PHASE_DPC_INFO_1;
        }
        break;
      default:
        break;
      }
    }
    break;
   case 17://Schneider 2022
     {
       switch(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep){
       case  PM_SCHNEDER_3PHASE_INFO_1:
         {
          modbusParseInfoString(ptr,&sPMInfo[i].u8Model[0],20,0);           
         sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_SCHNEDER_3PHASE_INFO_2;
         }
         break;
       case  PM_SCHNEDER_3PHASE_INFO_2: //Serial
         {
          int32_t  i32schneiderPM2=0;
          
          float val2=0;  
          modbusParseInfoSign(ptr,0,&sPMInfo[i].id_SerialNumber,0,0,16);
          modbusParseInfoSign(ptr,0,0, &i32schneiderPM2,1,32);          
          sPMInfo[i].u32SerialNumber = (uint32_t)i32schneiderPM2;         
          sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_SCHNEDER_3PHASE_INFO_3;
         }
         break;
       case  PM_SCHNEDER_3PHASE_INFO_3:   //E
         {
          uint32_t u32schneiderPM3=0;
          float val3=0;          
          modbusParseInfo(ptr,0, 0,&u32schneiderPM3,4,32);
          memcpy(&val3,&u32schneiderPM3,4);
          sPMInfo[i].u32TotalActiveE = (int32_t)(val3*100);

          modbusParseInfo(ptr,0, 0,&u32schneiderPM3,12,32);
          memcpy(&val3,&u32schneiderPM3,4);
          sPMInfo[i].u32TotalReActiveE = (int32_t)(val3*100);          
          sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_SCHNEDER_3PHASE_INFO_4;
         }
         break;
       case  PM_SCHNEDER_3PHASE_INFO_4: //I
         {
          uint32_t u32schneiderPM4=0;
          float val4=0;
          modbusParseInfo(ptr,0, 0,&u32schneiderPM4,0,32);
          memcpy(&val4,&u32schneiderPM4,4);
          sPMInfo[i].sPhaseInfo[0].fCurrent = ((float)val4)*100;             

          modbusParseInfo(ptr,0, 0,&u32schneiderPM4,2,32);
          memcpy(&val4,&u32schneiderPM4,4);
          sPMInfo[i].sPhaseInfo[1].fCurrent = ((float)val4)*100;
          
          modbusParseInfo(ptr,0, 0,&u32schneiderPM4,4,32);
          memcpy(&val4,&u32schneiderPM4,4);
          sPMInfo[i].sPhaseInfo[2].fCurrent = (float)val4*100; 
          
          sPMInfo[i].fTotalCurrent = sPMInfo[i].sPhaseInfo[0].fCurrent 
                                    +sPMInfo[i].sPhaseInfo[1].fCurrent
                                    +sPMInfo[i].sPhaseInfo[2].fCurrent;
           
          sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_SCHNEDER_3PHASE_INFO_5;
         }
         break;
       case  PM_SCHNEDER_3PHASE_INFO_5: //U
         {
          uint32_t u32schneiderPM5=0;
          float val5=0;
          modbusParseInfo(ptr,0, 0,&u32schneiderPM5,0,32);
          memcpy(&val5,&u32schneiderPM5,4);
          sPMInfo[i].sPhaseInfo[0].fVoltage = ((float)val5)*100;             

          modbusParseInfo(ptr,0, 0,&u32schneiderPM5,2,32);
          memcpy(&val5,&u32schneiderPM5,4);
          sPMInfo[i].sPhaseInfo[1].fVoltage = ((float)val5)*100;
          
          modbusParseInfo(ptr,0, 0,&u32schneiderPM5,4,32);
          memcpy(&val5,&u32schneiderPM5,4);
          sPMInfo[i].sPhaseInfo[2].fVoltage = ((float)val5)*100;          
          sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_SCHNEDER_3PHASE_INFO_6;
         }
         break; 
       case  PM_SCHNEDER_3PHASE_INFO_6: //KW, kVAR
         {
          uint32_t u32schneiderPM6=0;
          float val6=0;
          modbusParseInfo(ptr,0, 0,&u32schneiderPM6,0,32);
          memcpy(&val6,&u32schneiderPM6,4);
          sPMInfo[i].sPhaseInfo[0].fActivePower = ((float)val6)*100;             

          modbusParseInfo(ptr,0, 0,&u32schneiderPM6,2,32);
          memcpy(&val6,&u32schneiderPM6,4);
          sPMInfo[i].sPhaseInfo[1].fActivePower = ((float)val6)*100;
          
          modbusParseInfo(ptr,0, 0,&u32schneiderPM6,4,32);
          memcpy(&val6,&u32schneiderPM6,4);
          sPMInfo[i].sPhaseInfo[2].fActivePower = ((float)val6)*100; 
          
          modbusParseInfo(ptr,0, 0,&u32schneiderPM6,6,32);
          memcpy(&val6,&u32schneiderPM6,4);
          sPMInfo[i].fRealPower = ((float)val6)*100;
          
           modbusParseInfo(ptr,0, 0,&u32schneiderPM6,8,32);
          memcpy(&val6,&u32schneiderPM6,4);
          sPMInfo[i].sPhaseInfo[0].fReactivePower = ((float)val6)*100;             

          modbusParseInfo(ptr,0, 0,&u32schneiderPM6,10,32);
          memcpy(&val6,&u32schneiderPM6,4);
          sPMInfo[i].sPhaseInfo[1].fReactivePower = ((float)val6)*100;
          
          modbusParseInfo(ptr,0, 0,&u32schneiderPM6,12,32);
          memcpy(&val6,&u32schneiderPM6,4);
          sPMInfo[i].sPhaseInfo[2].fReactivePower = ((float)val6)*100; 
          
          modbusParseInfo(ptr,0, 0,&u32schneiderPM6,14,32);
          memcpy(&val6,&u32schneiderPM6,4);
          sPMInfo[i].fReactivePower = ((float)val6)*100;         

          modbusParseInfo(ptr,0, 0,&u32schneiderPM6,16,32);
          memcpy(&val6,&u32schneiderPM6,4);
          sPMInfo[i].sPhaseInfo[0].fApparentPower = ((float)val6)*100;             

          modbusParseInfo(ptr,0, 0,&u32schneiderPM6,18,32);
          memcpy(&val6,&u32schneiderPM6,4);
          sPMInfo[i].sPhaseInfo[1].fApparentPower = ((float)val6)*100;
          
          modbusParseInfo(ptr,0, 0,&u32schneiderPM6,20,32);
          memcpy(&val6,&u32schneiderPM6,4);
          sPMInfo[i].sPhaseInfo[2].fApparentPower = ((float)val6)*100; 
          
          modbusParseInfo(ptr,0, 0,&u32schneiderPM6,22,32);
          memcpy(&val6,&u32schneiderPM6,4);
          sPMInfo[i].fApparentPower = ((float)val6)*100;           
          
          sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_SCHNEDER_3PHASE_INFO_7;
         }
         break;
       case  PM_SCHNEDER_3PHASE_INFO_7: //Factor
         {
          uint32_t u32schneiderPM7=0;
          float val7=0;
          modbusParseInfo(ptr,0, 0,&u32schneiderPM7,0,32);
          memcpy(&val7,&u32schneiderPM7,4);
          if(val7 >1)val7=2-val7;
          else if(val7<-1)val7=-2-val7;
          if(val7<0)val7=-val7;
          sPMInfo[i].sPhaseInfo[0].fPowerFactor = ((float)val7)*100;             

          modbusParseInfo(ptr,0, 0,&u32schneiderPM7,2,32);
          memcpy(&val7,&u32schneiderPM7,4);
          if(val7 >1)val7=2-val7;
          else if(val7<-1)val7=-2-val7;
          if(val7<0)val7=-val7;
          sPMInfo[i].sPhaseInfo[1].fPowerFactor = ((float)val7)*100;
          
          modbusParseInfo(ptr,0, 0,&u32schneiderPM7,4,32);
          memcpy(&val7,&u32schneiderPM7,4);
          if(val7 >1)val7=2-val7;
          else if(val7<-1)val7=-2-val7;
          if(val7<0)val7=-val7;
          sPMInfo[i].sPhaseInfo[2].fPowerFactor = ((float)val7)*100; 
          
          modbusParseInfo(ptr,0, 0,&u32schneiderPM7,6,32);
          memcpy(&val7,&u32schneiderPM7,4);
          if(val7 >1)val7=2-val7;
          else if(val7<-1)val7=-2-val7; 
          if(val7<0)val7=-val7;
          sPMInfo[i].fPowerFactor = ((float)val7)*100;
          
          
          sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_SCHNEDER_3PHASE_INFO_8;
         }
         break; 
       case  PM_SCHNEDER_3PHASE_INFO_8: //F
         {
          uint32_t u32schneiderPM8=0;
          float val8=0;         
          modbusParseInfo(ptr,0, 0,&u32schneiderPM8,0,32);
          memcpy(&val8,&u32schneiderPM8,4);
          sPMInfo[i].fFrequency = ((float)val8*10);  
          sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_SCHNEDER_3PHASE_INFO_1;
         }
         break;                 
       default:
        break;
       }
     }
    break;
  case 18: //EASTRON SMD72D 2022
    {
       u32eastronSDM72DPM1 = 0;
       val1=0; 
      switch(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep){      
      case PM_EASTRON_SDM72D_INFO_1: 
        {
         for(uint8_t m =0;m<3; m++) {
          modbusParseInfo(ptr,0, 0,&u32eastronSDM72DPM1,m*2,32);                // Phase Voltage
          memcpy(&val1,&u32eastronSDM72DPM1,4);
          sPMInfo[i].sPhaseInfo[m].fVoltage =  val1*100;
          
          modbusParseInfo(ptr,0, 0,&u32eastronSDM72DPM1,m*2+6,32);              // Phase current
          memcpy(&val1,&u32eastronSDM72DPM1,4);
          sPMInfo[i].sPhaseInfo[m].fCurrent =  val1*100; 
          
          modbusParseInfo(ptr,0, 0,&u32eastronSDM72DPM1,m*2+12,32);             // Phase Active power
          memcpy(&val1,&u32eastronSDM72DPM1,4);
          sPMInfo[i].sPhaseInfo[m].fActivePower =  val1/10;    
          
          modbusParseInfo(ptr,0, 0,&u32eastronSDM72DPM1,m*2+18,32); 
          memcpy(&val1,&u32eastronSDM72DPM1,4);
          sPMInfo[i].sPhaseInfo[m].fApparentPower =  val1/10;    
          
          modbusParseInfo(ptr,0, 0,&u32eastronSDM72DPM1,m*2+24,32);             // Phase Reactive power
          memcpy(&val1,&u32eastronSDM72DPM1,4);
          sPMInfo[i].sPhaseInfo[m].fReactivePower =  - (val1/10);             
          }                        
         sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_EASTRON_SDM72D_INFO_2;
        }
        break;
      case PM_EASTRON_SDM72D_INFO_2: //30-60
        {
         for(uint8_t m =0;m<3; m++){
          modbusParseInfo(ptr,0, 0,&u32eastronSDM72DPM1,m*2,32);                // Phase power factor
          memcpy(&val1,&u32eastronSDM72DPM1,4);
          sPMInfo[i].sPhaseInfo[m].fPowerFactor =  val1*100;                      
          }
         
         modbusParseInfo(ptr,0, 0,&u32eastronSDM72DPM1,18,32);                  // Total current
         memcpy(&val1,&u32eastronSDM72DPM1,4);
         sPMInfo[i].fTotalCurrent =  val1*100;          
         
         modbusParseInfo(ptr,0, 0,&u32eastronSDM72DPM1,22,32);                  // Total system W
         memcpy(&val1,&u32eastronSDM72DPM1,4);
         sPMInfo[i].fRealPower =  val1/10;          
         sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_EASTRON_SDM72D_INFO_3;
        }
        break;
      case PM_EASTRON_SDM72D_INFO_3: //60-90
        {
         modbusParseInfo(ptr,0, 0,&u32eastronSDM72DPM1,0,32);                   // Total system Var
         memcpy(&val1,&u32eastronSDM72DPM1,4);
         sPMInfo[i].fReactivePower =  -(val1/10);          
          
         modbusParseInfo(ptr,0, 0,&u32eastronSDM72DPM1,2,32);                   // Total power factor
         memcpy(&val1,&u32eastronSDM72DPM1,4);
         sPMInfo[i].fPowerFactor =  val1*100;
          
         modbusParseInfo(ptr,0, 0,&u32eastronSDM72DPM1,10,32);                  // Frequence
         memcpy(&val1,&u32eastronSDM72DPM1,4);
         sPMInfo[i].fFrequency =  val1*10;

         modbusParseInfo(ptr,0, 0,&u32eastronSDM72DPM1,12,32);                  // Import active energy
         memcpy(&val1,&u32eastronSDM72DPM1,4);
         sPMInfo[i].u32ImportActiveE = (int32_t)(val1*100);         

         modbusParseInfo(ptr,0, 0,&u32eastronSDM72DPM1,14,32);                  // Export active energy
         memcpy(&val1,&u32eastronSDM72DPM1,4);
         sPMInfo[i].u32ExportActiveE =  (int32_t)(val1*100);         
         sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_EASTRON_SDM72D_INFO_4;                                
        }
        break;
      case PM_EASTRON_SDM72D_INFO_4:
        {
         modbusParseInfo(ptr,0, 0,&u32eastronSDM72DPM1,0,32);                   // Total Active Energy
         memcpy(&val1,&u32eastronSDM72DPM1,4);
         sPMInfo[i].u32TotalActiveE =  (int32_t)(val1*100);   
         
         modbusParseInfo(ptr,0, 0,&u32eastronSDM72DPM1,2,32);                   // Total Reactive Energy
         memcpy(&val1,&u32eastronSDM72DPM1,4);
         sPMInfo[i].u32TotalReActiveE= (int32_t) (val1*100);  
                  
         sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_EASTRON_SDM72D_INFO_5;             
        }
        break;   
      case PM_EASTRON_SDM72D_INFO_5:
        {
                  
         sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_EASTRON_SDM72D_INFO_6;             
        }
        break; 
      case PM_EASTRON_SDM72D_INFO_6:
        {
         modbusParseInfo(ptr,0, 0,&u32eastronSDM72DPM1,0,32);                   // Serial Number
         sPMInfo[i].u32SerialNumber =  u32eastronSDM72DPM1;    
                  
         sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_EASTRON_SDM72D_INFO_1;             
        }
        break;        
      default:
        break;       
      }
    }
    break;    
    default:
       break;
    };
}
#endif
#if (USERDEF_MONITOR_VAC == ENABLED)
uint32_t VacResetOK = 1; // dang reset
void Extract_Holding_Regs_Data_VAC ()
{
    uint8_t i,j;
    uint8_t *ptr = &Modbus.u8BuffRead[0];
    switch(Modbus.u8SlaveID - sModbusManager.sVACManager[sModbusManager.u8VACCurrentIndex].u32VACSlaveOffset)
    {
    case 1:
        i = 0;
        break;
    case 2:
        i = 1;
        break;
    };
    switch(sModbusManager.sVACManager[sModbusManager.u8VACCurrentIndex].u8VACType)
    {
    case 1:// VIETTEL
    {
        switch(sModbusManager.sVACManager[sModbusManager.u8VACCurrentIndex].u8VACRunningStep)
        {
        case VAC_VIETTEL_INFO_1:
        {
          modbusParseInfo(ptr,0,&sVACInfo[i].u16FirmVersion,0,0,16);
          modbusParseInfo(ptr,0,&sVACInfo[i].u16SN_Hi,0,1,16);
          modbusParseInfo(ptr,0,&sVACInfo[i].u16SN_Lo,0,2,16);
          
          modbusParseInfoSign(ptr,0,&sVACInfo[i].i16InTemp,0,3,16);
          modbusParseInfoSign(ptr,0,&sVACInfo[i].i16ExTemp,0,4,16);
          modbusParseInfoSign(ptr,0,&sVACInfo[i].i16Frost1Temp,0,5,16);
          modbusParseInfoSign(ptr,0,&sVACInfo[i].i16Frost2Temp,0,6,16);
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32Humid,0,7,16);
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32Fan1Duty,0,8,16);
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32Fan1RPM,0,9,16);
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32Fan2Duty,0,10,16);
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32Fan2RPM,0,11,16);
          modbusParseInfo(ptr,0,0,&sVACInfo[i].u32AlarmStatus,12,32);
          if((sVACInfo[i].u32AlarmStatus & 0x00008000) == 0x00008000){
//            sVACInfo[i].u32AlarmStatus = sVACInfo[i].u32AlarmStatus & 0xFFFFEFFF; 
            VacResetOK = 1;
          }
          else VacResetOK = 0;
           
           sModbusManager.sVACManager[sModbusManager.u8VACCurrentIndex].u8VACRunningStep = VAC_VIETTEL_INFO_2;
        }
        break;
        case VAC_VIETTEL_INFO_2:
        {          
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32SysMode,0,0,16);
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32FanSpeedMode,0,1,16);
          modbusParseInfoSign(ptr,0,&sVACInfo[i].i16FanStartTemp,0,2,16);
          modbusParseInfoSign(ptr,0,&sVACInfo[i].i16SetTemp,0,3,16);
          modbusParseInfoSign(ptr,0,&sVACInfo[i].i16PidOffsetTemp,0,4,16);
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32FanMinSpeed,0,5,16);
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32FanMaxSpeed,0,6,16);
          modbusParseInfoSign(ptr,0,&sVACInfo[i].i16FilterStuckTemp,0,7,16);
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32NightModeEn,0,8,16);
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32NightModeStart,0,9,16);
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32NightModeEnd,0,10,16);
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32NightMaxSpeed,0,11,16);
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32ManualMode,0,12,16);
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32ManualMaxSpeed,0,13,16);
          modbusParseInfoSign(ptr,0,&sVACInfo[i].i16InMaxTemp,0,14,16);
          modbusParseInfoSign(ptr,0,&sVACInfo[i].i16ExMaxTemp,0,15,16);
          modbusParseInfoSign(ptr,0,&sVACInfo[i].i16FrostMaxTemp,0,16,16);
          modbusParseInfoSign(ptr,0,&sVACInfo[i].i16InMinTemp,0,17,16);
          modbusParseInfoSign(ptr,0,&sVACInfo[i].i16ExMinTemp,0,18,16);
          modbusParseInfoSign(ptr,0,&sVACInfo[i].i16FrostMinTemp,0,19,16);
          modbusParseInfoSign(ptr,0,&sVACInfo[i].i16MinOutTemp,0,20,16);
          modbusParseInfoSign(ptr,0,&sVACInfo[i].i16DeltaTemp,0,21,16);
          modbusParseInfoSign(ptr,0,&sVACInfo[i].i16PanicTemp,0,22,16);
          modbusParseInfo(ptr,0,&sVACInfo[i].u16ActiveFan,0,23,16);
          modbusParseInfo(ptr,0,&sVACInfo[i].u16InstalledAirCon,0,24,16);
          modbusParseInfo(ptr,0,&sVACInfo[i].u16InstalledFan,0,25,16);
          
          modbusParseTime(ptr, &SyncTime, 26);
          
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32AirCon1Model,0,30,16);
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32AirCon1Type,0,31,16);
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32AirCon2Model,0,32,16);
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32AirCon2Type,0,33,16);
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32AirConOnOff,0,34,16);
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32AirConMode,0,35,16);
          modbusParseInfo(ptr,0,&sVACInfo[i].i16AirConTemp,0,36,16);
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32AirConSpeed,0,37,16);
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32AircondDir,0,38,16);
                           
          sModbusManager.sVACManager[sModbusManager.u8VACCurrentIndex].u8VACRunningStep = VAC_VIETTEL_INFO_3;
        }
        break;
        case VAC_VIETTEL_INFO_3:
        {          
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32WEnable,0,0,16);
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32WSerial1,0,1,16);
          modbusParseInfo(ptr,0,(uint16_t *)&sVACInfo[i].u32WSerial2,0,2,16);     
                           
          sModbusManager.sVACManager[sModbusManager.u8VACCurrentIndex].u8VACRunningStep = VAC_VIETTEL_INFO_1;
        }
        break;
        default:
            break;
        };
    }
    break;
    default:
        break;
    };
}
//Check app ===================================================================
uint8_t Check_app(uint8_t slaveAddr, uint16_t regAddr, int16_t val){
  uint8_t u8DataBuff[4];
  u8DataBuff[0] = (uint8_t) (val >> 8);
  u8DataBuff[1] = (uint8_t) (val);
  for(uint8_t i=0;i<3;i++){
    Write_Multi_Reg (slaveAddr, regAddr, &u8DataBuff[0], 1);
    vTaskDelay(10000);
    if(RS4851_Check_Respond_Data_VAC()== 16){
      return CHECK_TRUE;
    }
  }
  return CHECK_FALSE;
}
void vac_v2_send_block(uint8_t *data, size_t length){
  GPIO_SetPinsOutput(GPIOE,1u << 3u);
  GPIO_SetPinsOutput(GPIOE,1u << 2u);
  vTaskDelay(5);
  UART_WriteBlocking(RS4851T_UART,data,length);
  vTaskDelay(5);
  GPIO_ClearPinsOutput(GPIOE,1u << 3u);
  GPIO_ClearPinsOutput(GPIOE,1u << 2u);
}
//check boot ==================================================================
uint8_t VAC_V2_check_boot(){
  uint8_t *pData=&vac_v2_update_receive.buff[0];
  for(uint8_t i=0;i<3;i++){
    memset(&vac_v2_update_receive,0,sizeof(vac_v2_update_receive));
    vac_v2_send_block(ATCMD_VAC_STATE_CHECK, sizeof(ATCMD_VAC_STATE_CHECK) - 1);
    vTaskDelay(1000);
    if (!strcasecmp((const char *)pData, ATCMD_VAC_STATE_RESPON)) return CHECK_TRUE;
  }
  return CHECK_FALSE;
}
// check state vac v2========================================================
uint8_t VAC_V2_check_state(){
  //check state boot vac v2--------------------------------------------------
  vTaskDelay(500);
  ReInit2_RS485_MODBUS_UART (172800);
  if(VAC_V2_check_boot()==CHECK_TRUE) return STATE_VAC_V2_BOOT;
  
  // check state app vac v2--------------------------------------------------
  vTaskDelay(500);
  ReInit2_RS485_MODBUS_UART (14400);
  if(Check_app(35,100,0)==CHECK_TRUE) return STATE_VAC_V2_APP;
  
  //drive disconnect-------------------------------------------------------
  ReInit2_RS485_MODBUS_UART (14400);
  return STATE_VAC_V2_DISCONNECT;
}
//ma lenh 16: VAC v2 update================================================
void SetVar_i16VACUpdate_v2(uint8_t slaveAddr, uint16_t regAddr, int16_t val)
{
  uint8_t u8DataBuff[4];
  u8DataBuff[0] = (uint8_t) (val >> 8);
  u8DataBuff[1] = (uint8_t) (val);
  Write_Multi_Reg (slaveAddr, regAddr, &u8DataBuff[0], 1);
  vTaskDelay(10000);
  if(RS4851_Check_Respond_Data_VAC() != 16){
      Write_Multi_Reg (slaveAddr, regAddr, &u8DataBuff[0], 1);
      vTaskDelay(10000);
      if(RS4851_Check_Respond_Data_VAC() != 16){
      Write_Multi_Reg (slaveAddr, regAddr, &u8DataBuff[0], 1);
      vTaskDelay(10000);
    }
  }
}
// ma lenh 16: VAC_Update
void SetVar_i16VACUpdate(uint8_t slaveAddr, uint16_t regAddr, int16_t val)
{
  uint8_t u8DataBuff[4];
  u8DataBuff[0] = (uint8_t) (val >> 8);
  u8DataBuff[1] = (uint8_t) (val);
  Write_Multi_Reg (slaveAddr, regAddr, &u8DataBuff[0], 1);
  vTaskDelay(10000);
  if(RS4851_Check_Respond_Data_VAC() != 16){
      Write_Multi_Reg (slaveAddr, regAddr, &u8DataBuff[0], 1);
      vTaskDelay(10000);
      if(RS4851_Check_Respond_Data_VAC() != 16){
      Write_Multi_Reg (slaveAddr, regAddr, &u8DataBuff[0], 1);
      vTaskDelay(10000);
    }
  }
}
// ma lenh 16: VAC
void SetVar_i16VAC(uint8_t slaveAddr, uint16_t regAddr, int16_t val)
{
  uint8_t u8DataBuff[4];
  u8DataBuff[0] = (uint8_t) (val >> 8);
  u8DataBuff[1] = (uint8_t) (val);
  Write_Multi_Reg (slaveAddr, regAddr, &u8DataBuff[0], 1);
  vTaskDelay(300);
  if(RS4851_Check_Respond_Data_VAC() != 16){
      Write_Multi_Reg (slaveAddr, regAddr, &u8DataBuff[0], 1);
      vTaskDelay(300);
      if(RS4851_Check_Respond_Data_VAC() != 16){
      Write_Multi_Reg (slaveAddr, regAddr, &u8DataBuff[0], 1);
    }
  }
}
#endif
#if (USERDEF_MONITOR_SMCB == ENABLED) // smcb
// ma lenh 06
void SetVar_FC06SMCB(uint8_t slaveAddr, uint16_t regAddr, uint16_t Val)
{
  Write_Single_Reg(slaveAddr, regAddr, Val);
  vTaskDelay(1000);
  if(RS4851_Check_Respond_Data_SMCB() != 6){
      Write_Single_Reg(slaveAddr, regAddr, Val);
      vTaskDelay(1000);
      if(RS4851_Check_Respond_Data_SMCB() != 6){
      Write_Single_Reg(slaveAddr, regAddr, Val);
    }
  }
}

void Extract_Holding_Regs_Data_SMCB ()
{
    uint8_t i,j;
    uint8_t *ptr = &Modbus.u8BuffRead[0];
    switch(Modbus.u8SlaveID - sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u32SMCBSlaveOffset)
    {
    case 1:
        i = 0;
        break;
    case 2:
        i = 1;
        break;
    case 3:
        i = 2;
        break;
    case 4:
        i = 3;
        break;
    case 5:
        i = 4;
        break;
    };
    switch(sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u8SMCBType)
    {
    case 1:// OPEN
    {
        switch(sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u8SMCBRunningStep)
        {
        case SMCB_OPEN_INFO_1:
        {
          modbusParseInfo(ptr,0,(uint16_t*) &sSMCBInfo[i].u32State,0,0,16);
           
          sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u8SMCBRunningStep = SMCB_OPEN_INFO_1;
        }
        break;
        default:
            break;
        };
    }
    break;
    case 2:// MATIS
    {
        switch(sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u8SMCBRunningStep)
        {
        case SMCB_MATIS_INFO_1:
        {
          modbusParseInfo(ptr,0,(uint16_t*) &sSMCBInfo[i].u32State,0,0,16);
           
          sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u8SMCBRunningStep = SMCB_MATIS_INFO_1;
        }
        break;
        default:
            break;
        };
    }
    break;
     case 3:// GOL
    {
        switch(sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u8SMCBRunningStep)
        {
        case SMCB_GOL_INFO_1:
        {
          modbusParseInfo(ptr,0,(uint16_t*) &sSMCBInfo[i].u32State,0,0,16);
           
          sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u8SMCBRunningStep = SMCB_GOL_INFO_1;
        }
        break;
        default:
            break;
        };
    }
    break;
    default:
        break;
    };
}
#endif
#if (USERDEF_MONITOR_FUEL == ENABLED) // fuel
void Extract_Holding_Regs_Data_FUEL ()
{
    uint8_t i,j;
    uint8_t *ptr = &Modbus.u8BuffRead[0];
    switch(Modbus.u8SlaveID - sModbusManager.sFUELManager[sModbusManager.u8FUELCurrentIndex].u32FUELSlaveOffset)
    {
    case 1:
        i = 0;
        break;
    case 2:
        i = 1;
        break;
    };
    switch(sModbusManager.sFUELManager[sModbusManager.u8FUELCurrentIndex].u8FUELType)
    {
    case 1:// HPT621
    {
        switch(sModbusManager.sFUELManager[sModbusManager.u8FUELCurrentIndex].u8FUELRunningStep)
        {
        case FUEL_HPT621_INFO_1:
        {
          modbusParseInfo(ptr,0,(uint16_t*) &sFUELInfo[i].u32FuelLevel,0,0,16);
           
          sModbusManager.sFUELManager[sModbusManager.u8FUELCurrentIndex].u8FUELRunningStep = FUEL_HPT621_INFO_1;
        }
        break;
        default:
            break;
        };
    }
    break;
    default:
        break;
    };
}
#endif
#if (USERDEF_MONITOR_BM == ENABLED)  
void Extract_Holding_Regs_Data_BM ()
{
    uint8_t i,j;
    uint8_t *ptr = &Modbus.u8BuffRead[0];
    switch(Modbus.u8SlaveID - sModbusManager.sBMManager[sModbusManager.u8BMCurrentIndex].u32BMSlaveOffset)
    {
    case 1:
        i = 0;
        break;
    case 2:
        i = 1;
        break;
    case 3:
        i = 2;
        break;
    case 4:
        i = 3;
        break;
    case 5:
        i = 4;
        break;
    case 6:
        i = 5;
        break;
    case 7:
        i = 6;
        break;
    case 8:
        i = 7;
        break;
    case 9:
        i = 8;
        break;
    case 10:
        i = 9;
        break;
    case 11:
        i = 10;
        break;
    case 12:
        i = 11;
        break;
    case 13:
        i = 12;
        break;
    case 14:
        i = 13;
        break;
    case 15:
        i = 14;
        break;
    case 16:
        i = 15;
        break;
    };
    switch(sModbusManager.sBMManager[sModbusManager.u8BMCurrentIndex].u8BMType)
    {
    case 1:// VIETTEL
    {
        switch(sModbusManager.sBMManager[sModbusManager.u8BMCurrentIndex].u8BMRunningStep)
        {
        case BM_VIETTEL_INFO_1:
        {          
            modbusParseInfo(ptr,0,0,&sBMInfo[i].u32BattVolt,0,32);       
            modbusParseInfo(ptr,0,0,&sBMInfo[i].u32PackVolt,2,32);
            
            for (j = 0; j < 4; j++)
            {
                modbusParseInfo(ptr,0,&sBMInfo[i].u16CellVolt[j],0,4+j,16);
            }
            modbusParseInfoSign(ptr,0,0,&sBMInfo[i].i32PackCurr,8,32);
            for (j = 0; j < 6; j++)
            {
                modbusParseInfo(ptr,0,&sBMInfo[i].u16CellTemp[j],0,10+j,16);
            }

            modbusParseInfo(ptr,0,0,&sBMInfo[i].u32AlarmStatus,16,32);
            modbusParseInfo(ptr,0,0,&sBMInfo[i].u32BalanceStatus,18,32);
            modbusParseInfo(ptr,0,&sBMInfo[i].u16BattStatus,0,20,16);
            
            modbusParseInfo(ptr,0,(uint16_t *)&sBMInfo[i].u16SOC,0,21,16);
            modbusParseInfo(ptr,0,0,&sBMInfo[i].u32SOH,22,32);
            modbusParseInfo(ptr,0,0,&sBMInfo[i].u32DischargeTime,24,32);
            modbusParseInfo(ptr,0,0,&sBMInfo[i].u32TotalRunTime,26,32);
            
            modbusParseInfo(ptr,0,0,&sBMInfo[i].u32VoltDiff,32,32);
            modbusParseInfo(ptr,0,0,&sBMInfo[i].u32MaxTemp,34,32);
            modbusParseInfo(ptr,0,0,&sBMInfo[i].u32VoltThres,36,32);
            modbusParseInfo(ptr,0,0,&sBMInfo[i].u32CurrThres,38,32);
            modbusParseInfo(ptr,0,0,&sBMInfo[i].u32TimeThres,40,32);
            modbusParseInfo(ptr,0,0,&sBMInfo[i].u32SOCThres,42,32);
            modbusParseInfo(ptr,0,0,&sBMInfo[i].u32MinTemp,44,32);
            modbusParseInfo(ptr,0,0,&sBMInfo[i].u32LowCapTime,46,32);
            modbusParseInfo(ptr,0,0,&sBMInfo[i].u32TotalDisAH,48,32);
            modbusParseInfo(ptr,0,0,&sBMInfo[i].u32HalfVoltAlarm,50,32);
            modbusParseInfo(ptr,0,0,&sBMInfo[i].u32RechargeVolt,52,32);
            modbusParseInfo(ptr,0,0,&sBMInfo[i].u32CurrBalanceThres,54,32);
           
            sModbusManager.sBMManager[sModbusManager.u8BMCurrentIndex].u8BMRunningStep = BM_VIETTEL_INFO_1;
        }
        break;
        case BM_VIETTEL_INFO_2:
        {
            sModbusManager.sBMManager[sModbusManager.u8BMCurrentIndex].u8BMRunningStep = BM_VIETTEL_INFO_1;
        }
        break;
        default:
            break;
        };
    }
    break;
    default:
        break;
    };
}
#endif


#if (USERDEF_MONITOR_ISENSE == ENABLED)
void Extract_Holding_Regs_Data_ISENSE(void)
{
    uint8_t i,j;
    uint8_t *ptr = &Modbus.u8BuffRead[0];
    switch(Modbus.u8SlaveID - sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u32ISENSESlaveOffset)
    {
    case 1:
      i = 0;
      break;
    };
    switch(sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSEType)
    {
    case 1:// FORLONG
      {
        switch(sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSERunningStep)
        {
        case ISENSE_FORLONG_INFO_1:
          {
            uint16_t u16Forlong;
            uint32_t u32Forlong;
            modbusParseInfobcd(ptr,0,0,&sISENSEInfo[i].u32TotalActiveE,0,32);
            modbusParseInfobcd(ptr,0,0,&sISENSEInfo[i].u32TotalReActiveE,4,32);
            modbusParseInfobcd(ptr,0,0,&u32Forlong,6,32);
            sISENSEInfo[i].fVoltage = (float)u32Forlong; 
            modbusParseInfobcd(ptr,0,0,&u32Forlong,8,32);
            sISENSEInfo[i].fCurrent = (float)u32Forlong;
            modbusParseInfobcd(ptr,0,0,&u32Forlong,10,32);
            sISENSEInfo[i].fActivePower = (float)u32Forlong;
            modbusParseInfobcd(ptr,0,0,&u32Forlong,16,32);
            sISENSEInfo[i].fPowerFactor = (float)u32Forlong;
            modbusParseInfobcd(ptr,0,0,&u32Forlong,18,32);
            sISENSEInfo[i].fFrequency = (float)u32Forlong;
            modbusParseInfobcd(ptr,0,0,&u32Forlong,20,32);
            sISENSEInfo[i].u32SerialNumber =u32Forlong;
            
            sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSERunningStep = ISENSE_FORLONG_INFO_1;
          }
          break;
        default:
          break;
        };
      }
      break;
    case 2: // IVY_DDS353H_2
      {
        int32_t i32IVY = 0; 
        int16_t i16IVY =0;
        uint32_t u32IVY =0;
        switch(sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSERunningStep){
        case ISENSE_IVY_INFO_1: 
          {
            modbusParseInfoSign(ptr,0,0,&i32IVY,0,32);
            sISENSEInfo[i].fVoltage =  (float)i32IVY/10;
            
            modbusParseInfoSign(ptr,0,0,&i32IVY,2,32);
            sISENSEInfo[i].fCurrent =  (float)i32IVY/10;            
            
            modbusParseInfoSign(ptr,0,0,&i32IVY,4,32);
            sISENSEInfo[i].fActivePower =  (float)i32IVY; 
            
            modbusParseInfoSign(ptr,0,0,&i32IVY,6,32);
            sISENSEInfo[i].fApparentPower =  (float)i32IVY;            
            
            modbusParseInfoSign(ptr,0,0,&i32IVY,8,32);
            sISENSEInfo[i].fReactivePower =  (float)i32IVY;            
            
            modbusParseInfoSign(ptr,0,&i16IVY,0,10,16);
            sISENSEInfo[i].fFrequency =  (float)i16IVY;           
            
            modbusParseInfoSign(ptr,0,&i16IVY,0,11,16);
            sISENSEInfo[i].fPowerFactor =  (float)i16IVY/100;   
                                   
            sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSERunningStep = ISENSE_IVY_INFO_2;
          }
          break;
        case ISENSE_IVY_INFO_2: 
          {
            modbusParseInfoSign(ptr,0,0,&i32IVY,2,32);
            sISENSEInfo[i].u32TotalActiveE =  i32IVY;
            
            modbusParseInfoSign(ptr,0,0,&i32IVY,32,32);
            sISENSEInfo[i].u32TotalReActiveE =  i32IVY;
            
            sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSERunningStep = ISENSE_IVY_INFO_3;
          }
          break;  
        case ISENSE_IVY_INFO_3: 
          {
            modbusParseInfo(ptr,0, &sISENSEInfo[i].serial_IVY[0],0,0,16);
            modbusParseInfo(ptr,0, &sISENSEInfo[i].serial_IVY[1],0,1,16);
            modbusParseInfo(ptr,0, &sISENSEInfo[i].serial_IVY[2],0,2,16); 
            modbusParseInfo(ptr,0, &sISENSEInfo[i].serial_IVY[3],0,0,16);
            modbusParseInfo(ptr,0, &sISENSEInfo[i].serial_IVY[4],0,1,16);
            modbusParseInfo(ptr,0, &sISENSEInfo[i].serial_IVY[5],0,2,16); 
            sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSERunningStep = ISENSE_IVY_INFO_1;
          }
          break;          
        default:
          break;       
        }
      }
      break;
    default:
      break;
    };
}
#endif
// extract holding regs data pm dc-------------------------------------------------------------------------------------------
#if (USERDEF_MONITOR_PM_DC == ENABLED)
void Extract_Holding_Regs_Data_PM_DC(void){
  uint8_t i,j;
  uint8_t *ptr = &Modbus.u8BuffRead[0];
  switch(Modbus.u8SlaveID - sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u32_pm_dc_slave_offset)
  {
  case 1:
      i = 0;
      break;
  };
  switch(sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u8_pm_dc_type){
  case 1:// YADA_DC
  {
    switch(sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u8_pm_dc_running_step){
    case PM_DC_YADA_INFO_1:
      {
      uint32_t temp_yada_pm_dc_1;
      modbusParseInfo(ptr,0, 0,&temp_yada_pm_dc_1,0,32);
      s_pm_dc_info[i].f_voltage = ((float)temp_yada_pm_dc_1)/10.0;
      sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u8_pm_dc_running_step = PM_DC_YADA_INFO_2;
      }
      break;
    case PM_DC_YADA_INFO_2:
      {
      uint32_t temp_yada_pm_dc_2;
      modbusParseInfo(ptr,0, 0,&temp_yada_pm_dc_2,0,32);
      s_pm_dc_info[i].f_current = ((float)temp_yada_pm_dc_2)/100.0;
      sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u8_pm_dc_running_step = PM_DC_YADA_INFO_3;
      }
      break;
    case PM_DC_YADA_INFO_3:
      {
      uint32_t temp_yada_pm_dc_3;
      modbusParseInfo(ptr,0, 0,&temp_yada_pm_dc_3,0,32);
      s_pm_dc_info[i].f_active_power = ((float)temp_yada_pm_dc_3)/100.0;
      sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u8_pm_dc_running_step = PM_DC_YADA_INFO_4;
      }
      break;
    case PM_DC_YADA_INFO_4:
      {
      uint32_t temp_yada_pm_dc_4;
      modbusParseInfo(ptr,0, 0,&temp_yada_pm_dc_4,0,32);
      s_pm_dc_info[i].f_active_energy = ((float)temp_yada_pm_dc_4)*10;
      sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u8_pm_dc_running_step = PM_DC_YADA_INFO_1;
      }
      break;
    default:
      break;
    }
  }
  break;
  default:
  break; 
  }
}
#endif 