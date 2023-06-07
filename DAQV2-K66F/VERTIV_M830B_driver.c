/** Anvd33
** 12/4/2022
** Driver Vertiv M830B
**/
#include "Header.h"
extern sMODBUSRTU_struct Modbus_vertiv;
uart_config_t  VERTIV_config;
uint32_t Basic_value;
static const unsigned char CRCHighTable1[] =
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

static const unsigned char CRCLowTable1[] =
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
void VERTIV_init_rs485_uart(){
    UART_GetDefaultConfig(&VERTIV_config);
    VERTIV_config.baudRate_Bps = 14400;
    VERTIV_config.enableTx = true;
    VERTIV_config.enableRx = true;
    VERTIV_config.parityMode= kUART_ParityDisabled;
    VERTIV_config.stopBitCount= kUART_OneStopBit;

    UART_Init(RS485T_UART, &VERTIV_config, CLOCK_GetFreq(RS485T_UART_CLKSRC));
    UART_Init(RS485R_UART, &VERTIV_config, CLOCK_GetFreq(RS485R_UART_CLKSRC));
    /* Enable RX interrupt. */
    UART_EnableInterrupts(RS485R_UART, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
    EnableIRQ(RS485R_UART_IRQn);
}
uint16_t tesst;
/* Write Multi message over UART0 
  *size                   Number of reg
  *funcitonCode           0x10  
*/
void Write_Multi_Reg_Vertiv(uint8_t slaveAddr, uint16_t regAddr, uint8_t *writeBuff, uint8_t size){
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
    tesst=regAddr;
    Modbus_vertiv.u8SlaveID = slaveAddr;
    Modbus_vertiv.u8FunctionCode = 0x10;
    Modbus_vertiv.u8StartHigh = (uint8_t)(regAddr>>8); // Address high
    Modbus_vertiv.u8StartLow = (uint8_t)(regAddr);     // Address Low
    Modbus_vertiv.u8NumberRegHigh = (uint8_t)(size>>8);        
    Modbus_vertiv.u8NumberRegLow = (uint8_t)(size);

    Modbus_vertiv.u8BuffWrite[0] = Modbus_vertiv.u8SlaveID;
    Modbus_vertiv.u8BuffWrite[1] = Modbus_vertiv.u8FunctionCode;
    Modbus_vertiv.u8BuffWrite[2] = Modbus_vertiv.u8StartHigh;
    Modbus_vertiv.u8BuffWrite[3] = Modbus_vertiv.u8StartLow;
    Modbus_vertiv.u8BuffWrite[4] = Modbus_vertiv.u8NumberRegHigh;
    Modbus_vertiv.u8BuffWrite[5] = Modbus_vertiv.u8NumberRegLow;
    Modbus_vertiv.u8BuffWrite[6] = numOfByte;
    
    for(i = 0; i < numOfByte; i++)
    {
      temp[i] = *(writeBuff+i);
      Modbus_vertiv.u8BuffWrite[7 + i] = temp[i];
    }  

    ModbusCRC1(&Modbus_vertiv.u8BuffWrite[0],7 + numOfByte);

    Modbus_vertiv.u8BuffWrite[7 + numOfByte] = Modbus_vertiv.u8CRCHigh;
    Modbus_vertiv.u8BuffWrite[8 + numOfByte] = Modbus_vertiv.u8CRCLow;    

    switch(sModbusManager.SettingCommand)
    {
    default:
      {                   
    GPIO_SetPinsOutput(GPIOD,1u << 1u); 
    GPIO_SetPinsOutput(GPIOD,1u << 0u);
    vTaskDelay(5);
    UART_WriteBlocking(RS485T_UART,&Modbus_vertiv.u8BuffWrite[0],9+numOfByte);
    vTaskDelay(5);
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    GPIO_ClearPinsOutput(GPIOD,1u << 0u);   
      }break;
    };
}
/* Message request data 
* fc                    functionCode: 03 read setting infor
                                      04 read data infor
*numPoint               Number of reg  
*/
void Read_Regs_Query_Vertiv(uint8_t slaveAddr, uint16_t startingAddr, uint16_t numPoint, uint8_t fc){
    Modbus_vertiv.u8SlaveID = slaveAddr;
    Modbus_vertiv.u8FunctionCode = fc;
    Modbus_vertiv.u8StartHigh = (uint8_t)(startingAddr>>8);
    Modbus_vertiv.u8StartLow = (uint8_t)(startingAddr);
    Modbus_vertiv.u8NumberRegHigh = (uint8_t)(numPoint>>8);
    Modbus_vertiv.u8NumberRegLow = (uint8_t)(numPoint);

    Modbus_vertiv.u8BuffWrite[0] = Modbus_vertiv.u8SlaveID;
    Modbus_vertiv.u8BuffWrite[1] = Modbus_vertiv.u8FunctionCode;
    Modbus_vertiv.u8BuffWrite[2] = Modbus_vertiv.u8StartHigh;
    Modbus_vertiv.u8BuffWrite[3] = Modbus_vertiv.u8StartLow;
    Modbus_vertiv.u8BuffWrite[4] = Modbus_vertiv.u8NumberRegHigh;
    Modbus_vertiv.u8BuffWrite[5] = Modbus_vertiv.u8NumberRegLow;

    ModbusCRC1(&Modbus_vertiv.u8BuffWrite[0],6);

    Modbus_vertiv.u8BuffWrite[6] = Modbus_vertiv.u8CRCHigh;
    Modbus_vertiv.u8BuffWrite[7] = Modbus_vertiv.u8CRCLow;
    GPIO_SetPinsOutput(GPIOD,1u << 1u); 
    GPIO_SetPinsOutput(GPIOD,1u << 0u);
    vTaskDelay(5);
    UART_WriteBlocking(RS485T_UART,&Modbus_vertiv.u8BuffWrite[0],8);
    DKD51_delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    GPIO_ClearPinsOutput(GPIOD,1u << 0u);
}
/* Check respond data message
  *return        1: Normal
                 other: Error
*/
int8_t Vertiv_Check_Respond_Data(){
uint16_t	mTemp = 0;

    if(Modbus_vertiv.u8MosbusEn==2)    // finish reading
    {
        Modbus_vertiv.u8CRCHighRead = Modbus_vertiv.u8BuffRead[Modbus_vertiv.u8ByteCount-2];
        Modbus_vertiv.u8CRCLowRead  = Modbus_vertiv.u8BuffRead[Modbus_vertiv.u8ByteCount-1];

        ModbusCRC1(&Modbus_vertiv.u8BuffRead[0],Modbus_vertiv.u8ByteCount-2);

        if(Modbus_vertiv.u8CRCHigh == Modbus_vertiv.u8CRCHighRead && Modbus_vertiv.u8CRCLow==Modbus_vertiv.u8CRCLowRead)
        {
            Modbus_vertiv.u8FunctionCode 	= Modbus_vertiv.u8BuffRead[1];

            switch(Modbus_vertiv.u8FunctionCode)
            {
            case _READ_COIL_STATUS:
            case _READ_INPUT_STATUS:
            {
                mTemp = (Modbus_vertiv.u8NumberRegHigh<<8)|(Modbus_vertiv.u8NumberRegLow);
                if (Modbus_vertiv.u8BuffRead[2] == mTemp)  
                { 
                        Extract_Holding_Regs_Data_Vertiv();
                        
                }
                else
                {
                    Modbus_vertiv.u8MosbusEn = 0;
                    return -3;
                }
            }
            break;
            case _READ_HOLDING_REGS:
            case _READ_INPUT_REG:       
            {
                mTemp = (Modbus_vertiv.u8NumberRegHigh<<8)|(Modbus_vertiv.u8NumberRegLow);
                if (Modbus_vertiv.u8BuffRead[2] == (mTemp*2))  // ByteCount == 2*NumberReg
                {                    
                        Extract_Holding_Regs_Data_Vertiv();                                       
                }
                else
                {
                    Modbus_vertiv.u8MosbusEn = 0;
                    return -3;
                }
            }
            break;
            
            case _EXCEPTION_READ_HOLDING:
            {
              Modbus_vertiv.u8MosbusEn = 0;
              return -4;
            }
            break;
            case _PRESET_SINGLE_REG:

                break;
            case _USER_REGISTER:

                break;
                           
            case _PRESET_MULTIPLE_REGS:
            {
              Modbus_vertiv.u8MosbusEn = 0;
              return 16;
            }
            case _REPORT_SLAVE_ID:
            {
                  Extract_Holding_Regs_Data_Vertiv();              
            }
            break;
            default:
                break;
            }

            Modbus_vertiv.u8MosbusEn = 0;
        }
        else
        {
            
            
            Modbus_vertiv.u8MosbusEn = 0;
            return -2;
        }
        Modbus_vertiv.u8MosbusEn = 0; //start
    }
    else
    {
        Modbus_vertiv.u8MosbusEn = 0;
        
        
        return -1;
    }
    
    return 1;

}
/* Extract message to get data 
  *MESGState        Type of group data
*/
   uint32_t u32temp;
   uint16_t u16temp;
   uint8_t  u8temp;
void Extract_Holding_Regs_Data_Vertiv(){
  uint8_t i=0; 
  switch(MESGState){ 
  case SYSINFO_RES:                                                             // address 0
      modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,0,16);          // batt group Volt
      sBattInfo.sRAWBattInfo.fDcVolt = (float)(u16temp)/10;
      modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,0,&u32temp,1,32);          // Load Current
      sDcInfo.sINFODcInfo.fCurrent = (float)(( u32temp - 1000000))/10;
      for(uint8_t i=0;i<7;i++){
      modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,9+i,16);        // Alarm system
      sDcInfo.sALARMDcInfo.u16SysAlarm[i] = u16temp;      
      }
      break;
  case SYSPARAMETER_RES:                                                        // address 16
      modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,0,16);          //  float voltage
      sBattInfo.sRAWBattInfo.fFltVoltCfg = (float)(u16temp)/10;    
      modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,1,16);          // under Voltage 1, DCLow
      sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg = (float)(u16temp)/10;
      modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,2,16);          // under Voltage 2, DC Under
      sBattInfo.sRAWBattInfo.fDCUnderCfg = (float)(u16temp)/10;
      modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,3,16);          // Over Dc Volt
      sBattInfo.sRAWBattInfo.fDCOverCfg = (float)(u16temp)/10;      
      break;
  case BATTGROUPINFO_RES:                                                       // address 22
//      modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,0,16);         // batt group Volt
//      sBattInfo.sRAWBattInfo.fDcVolt = (float)(u16temp)/10;
      
      modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,0,&u32temp,1,32);          // batt Current
      sBattInfo.sRAWBattInfo.fTotalCurr = (float)(( u32temp - 1000000))/10;
      modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,3,16);          // Bat tempt 
      if(u16temp == 0xFFFF) u16temp=32000;
      sDcInfo.sINFODcInfo.fSen1BattTemp = (float)(u16temp - 32000)/10;
      
      for(uint8_t i=0;i<5;i++){
      modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,4+i,16);  
      sBattInfo.sRAWBattInfo.u16BattAlarm[i] = u16temp;      
      }
      break;
  case BATTPARAMETTER_RES:                                                      // address 31
    modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,1,16);            // Bat High Tempt
    sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel =  (float)(u16temp - 32000)/10;

    
    modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,4,16);            // charge Current Limit
    sBattInfo.sRAWBattInfo.fCCLVal = (float)u16temp;

    modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,5,16);            // BootVoltage
    sBattInfo.sRAWBattInfo.fBotVoltCfg = (float)u16temp/10;    
    
    modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,14,16);           // Temp Comp 
    sBattInfo.sRAWBattInfo.fTempCompVal = (float)u16temp/10;        
    break;
  case BATTINFO_RES:                                                            // address 50

    modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,0,16);            // Bat Volt
    sDcInfo.sINFODcInfo.fBatt1Volt      = (float)(u16temp)/10;
    
    modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,0,&u32temp,1,32);            // Bat Curr
    sDcInfo.sINFODcInfo.fBatt1Curr      = (float)(( u32temp - 1000000))/10;
    
    modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,3,16);            // Bat Cap Remain
    sDcInfo.sINFODcInfo.fBatt1RmnCap     = (float)u16temp/10 ;
    
    modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,4,16);            // Batt Cap Total
    sBattInfo.sRAWBattInfo.fCapTotal    = (float) u16temp;
    
    modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,5,16);            // No use
    sDcInfo.sINFODcInfo.fBatt2Volt      = (float)(u16temp)/10;
    
    modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,0,&u32temp,6,32);            // No use
    sDcInfo.sINFODcInfo.fBatt2Curr      = (float)(( u32temp - 1000000))/10;
    
    modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,8,16);            // No use
    sDcInfo.sINFODcInfo.fBatt2RmnCap    = (float)u16temp/10 ;
    
    modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,9,16);            // No use
    sBattInfo.sRAWBattInfo.fCapTotal2    = (float) u16temp;    
    break;    
//  case DCINFO_RES:
//    
//    break;       
  case RECTGROUPINFO_RES:                                                       // address 650

    modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,0,&u32temp,1,32);            // rect total Curr
    sRectInfo.fTotal_Curr =  (float)((u32temp - 1000000))/10; 
               
    modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,6,16);            //number of rect
    sRectInfo.u8Rect_Num = u16temp;          

    modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,7,16);            // Ac phase
    if(u16temp == 0) sAcInfo.acphase =  1;
    else sAcInfo.acphase =  3;
      
    modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,9,16);            // Walk in time 
    sRectInfo.u8WITI = (uint8_t)u16temp;    
       
    modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,12,16);           //Walk in time Enable
    sRectInfo.u8WITE = (uint8_t)u16temp;             
           
    break;
  case RECTINFO_RES:                                                            // address 663
    for(int i=0; i < sRectInfo.u8Rect_Num; i++){
      
       modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,9*i,16);       // rect Volt 
       sRectInfo.sRAWRectParam[i].fRect_DcVolt = (float)(u16temp)/10;      
      
       modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,9*i+1,16);     // rect curr
       sRectInfo.sRAWRectParam[i].fRect_DcCurr = (float)( u16temp)/10;
       for(int j=0;j<3;j++){
         modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,9*i+6+j,16); // Alarm rect
         sRectInfo.sRAWRectParam[i].Alarm[j] = u16temp;       
       }      
    }

    break;
        
  case ACINPUTINFO_RES:                                                         // address 1749
        modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,0,16);        // phase 1
        sAcInfo.facVolt[0] = (float)(u16temp)/10;        
        modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,1,16);        // phase 2
        sAcInfo.facVolt[1] = (float)(u16temp)/10;        
        modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,2,16);        // phase 3
        sAcInfo.facVolt[2] = (float)(u16temp)/10; 
        for(int i=0;i<9;i++){
         modbusParseInfo(&Modbus_vertiv.u8BuffRead[0],0,&u16temp,0,3+i,16);     //alarm AC
         sAcInfo.acAlarm[i] =  u16temp;              
        }  
      break;
    default:
      break;
  }  
}
/* Send setting message over uart0
*/
void SetVar_i16_vertiv(uint8_t slaveAddr, uint16_t regAddr, uint16_t val){
  uint8_t u8DataBuff[2];
  u8DataBuff[0] = (uint8_t) (val >> 8);
  u8DataBuff[1] = (uint8_t) (val);
  Write_Multi_Reg_Vertiv(slaveAddr, regAddr, &u8DataBuff[0], 1);
}
/*Check respond setting message
  *return        1: Normal
                 other: Error
*/
int8_t Vertiv_Check_Respond_Setting_Data (void){
    uint16_t	mTemp = 0;

    if(Modbus_vertiv.u8MosbusEn==2)
    {
        Modbus_vertiv.u8CRCHighRead = Modbus_vertiv.u8BuffRead[Modbus_vertiv.u8ByteCount-2];
        Modbus_vertiv.u8CRCLowRead  = Modbus_vertiv.u8BuffRead[Modbus_vertiv.u8ByteCount-1];

        ModbusCRC(&Modbus_vertiv.u8BuffRead[0],Modbus_vertiv.u8ByteCount-2);

        if(Modbus_vertiv.u8CRCHigh == Modbus_vertiv.u8CRCHighRead && Modbus_vertiv.u8CRCLow==Modbus_vertiv.u8CRCLowRead)
        {
            Modbus_vertiv.u8FunctionCode 	= Modbus_vertiv.u8BuffRead[1];

            switch(Modbus_vertiv.u8FunctionCode)
            {
            case _PRESET_MULTIPLE_REGS:
            {
                mTemp = (Modbus_vertiv.u8NumberRegHigh<<8)|(Modbus_vertiv.u8NumberRegLow);
                if (Modbus_vertiv.u8BuffRead[2] == (mTemp*2))
                {
                    
                }
                else
                {
                    Modbus_vertiv.u8MosbusEn = 0;
                    return -3;
                }
            }
            break;
            case _EXCEPTION_WRITE_MULTI_REG:
            {
              Modbus_vertiv.u8MosbusEn = 0;
              return -4;
            }
            break;
            default:
                break;
            }

            Modbus_vertiv.u8MosbusEn = 0;
        }
        else
        {
            Modbus_vertiv.u8MosbusEn = 0;
            return -2;
        }
        Modbus_vertiv.u8MosbusEn = 0;
    }
    else
    {
        Modbus_vertiv.u8MosbusEn = 0;
        return -1;
    }
    return 1;
}
void ModbusCRC1(unsigned char *str, unsigned char len){
    unsigned char temp;
    Modbus_vertiv.u8CRCHigh = 0xFF;
    Modbus_vertiv.u8CRCLow = 0xFF;

    while( len--)
    {
        temp = *str++ ^ Modbus_vertiv.u8CRCHigh;
        Modbus_vertiv.u8CRCHigh = CRCHighTable1[temp] ^ Modbus_vertiv.u8CRCLow;
        Modbus_vertiv.u8CRCLow  = CRCLowTable1[temp];
    }
}