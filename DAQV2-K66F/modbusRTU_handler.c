#include "modbusRTU_handler.h"
#include "ftp_handler.h"
//#include "rs485RTU.h"
#define NUM_BYTE_OF_S19_LINE 53

//extern tGenState eGenState;
#pragma default_variable_attributes = @ "MySDRAM"
#if (USERDEF_MONITOR_BM == ENABLED)
tBMInfo sBMInfo[16];
#endif
#if (USERDEF_MONITOR_PM == ENABLED)
tPMInfo sPMInfo[2];
#endif
#if (USERDEF_MONITOR_VAC == ENABLED)
tVACInfo sVACInfo[1];
#endif
tLiionBattInfo sLiionBattInfo[16];
tGenInfo sGenInfo[2];
#if (USERDEF_MONITOR_SMCB == ENABLED) //smcb
tSMCBInfo sSMCBInfo[5]; 
#endif
#if (USERDEF_MONITOR_FUEL == ENABLED) //fuel
tFUELInfo sFUELInfo[2]; 
#endif
#if (USERDEF_MONITOR_ISENSE == ENABLED) //isense
tISENSEInfo sISENSEInfo[1]; 
#endif
#if (USERDEF_MONITOR_PM_DC == ENABLED)
T_PM_DC_INFO s_pm_dc_info[1];
#endif
#pragma default_variable_attributes = 

uint8_t return1=0;
typedef struct
{
  uint16_t tokenCnt;
  uint8_t line[NUM_BYTE_OF_S19_LINE];
  uint8_t bytePerLine;
  uint8_t eol;
  uint16_t linePerFile;
  uint32_t totalByteToSend;
  int32_t remainByteToSend;
  uint32_t sentByte;
}tS19FileParser;

tS19FileParser s19FileParser;
extern tConfigHandler configHandle;
extern TimeFormat SyncTime;
extern uint8_t u8FwUpdateCompleteFlag;

//thanhcm3 fix------------------------------------------
typedef struct {
  uint32_t _0;
  uint32_t _1;
}DELTA_STRUCT;
//thanhcm3 fix------------------------------------------
uint16_t s19FileParse(uint32_t startAddr)
{
  uint8_t data[60];
  sFLASH_ReadBuffer(&data[0], startAddr, NUM_BYTE_OF_S19_LINE); // doc 53 byte tu startAddr --> data
  s19FileParser.eol = 0;
  s19FileParser.tokenCnt = 0;
  s19FileParser.bytePerLine = 0;
  for (uint16_t i = 0; i < NUM_BYTE_OF_S19_LINE; i++)
  {
    switch(data[i])
    {
    case 'S':
      {        
        s19FileParser.tokenCnt++;
        if (s19FileParser.tokenCnt > 1)
        {
          s19FileParser.eol = 1;          
        }
        else
        {
          s19FileParser.line[i] = data[i];
        }
      }
      break;
    default:
      {
        s19FileParser.line[i] = data[i];
      }
      break;
    };
    if (s19FileParser.eol == 1)
    {
      s19FileParser.bytePerLine = i;
      break;
    }
  }  
  return s19FileParser.bytePerLine;
}

uint32_t cntDisConnectLIB[17];
uint32_t cntDisConnectVAC[2];
uint32_t cntDisConnectPM[2];
uint32_t cntDisConnectSMCB[5];
uint32_t cntDisConnectGEN[2];
uint32_t cntDisConnectFUEL[2];
uint32_t cntDisConnectISENSE[1];
uint32_t cnt_disConnect_pm_dc[1];

uint8_t LibM1Addr = 0;

uint32_t ModbusRTU_cnt=0;

FAN_DPC_SWITCH_UART_T fan_dpc_switch_uart_t;
void modbusRTU_task(void *pvParameters)
{
//    int8_t      Type_PM = 5;
    int8_t	reVal = 0;
#if (USERDEF_MONITOR_BM == ENABLED)
    sModbusManager.u8BMCurrentIndex = 0;
#endif
#if (USERDEF_MONITOR_LIB == ENABLED)
    sModbusManager.u8LIBCurrentIndex = 0;
#endif
#if (USERDEF_MONITOR_GEN == ENABLED)
    sModbusManager.u8GenCurrentIndex = 0;
#endif
#if (USERDEF_MONITOR_PM == ENABLED)
    sModbusManager.u8PMCurrentIndex = 0;
#endif
#if (USERDEF_MONITOR_VAC == ENABLED)
    sModbusManager.u8VACCurrentIndex = 0;
#endif
#if (USERDEF_MONITOR_SMCB == ENABLED) //smcb
    sModbusManager.u8SMCBCurrentIndex = 0;
#endif
#if (USERDEF_MONITOR_FUEL == ENABLED) //fuel
    sModbusManager.u8FUELCurrentIndex = 0;
#endif
    int8_t initMB =0;
    int8_t i =0;
    sTestRS485.InitUart=0;
    uint8_t cntTimeOutUpdateVAC = 0;
    for(i=0;i<15;i++)
      {
         sTestRS485.CabBuff[i]=0;
         sTestRS485.GenBuff[i]=0;
      }
    configHandle.devType                       = _NO_FW_UPDATE;
    fan_dpc_switch_uart_t.rx_enable            = STATE_OFF;
    fan_dpc_switch_uart_t.fan_state_e          = _DKD51_FAN_SYS_REQ;
    fan_dpc_switch_uart_t.check_sum_calculator = 0;
    fan_dpc_switch_uart_t.check_sum_read       = 0xFFFF;
  
    vTaskDelay(1000);
    for (;;)
    {
      
      ModbusRTU_cnt++;
      privateMibBase.cntGroup.ModbusRTUcnt=ModbusRTU_cnt++;
      
      switch(configHandle.devType)
      {
      case _DAQ_FW_:
        {
          vTaskDelay(1000);
        }
        break;
      case _VAC_FW_:
        {
          
          if ((configHandle.isVACUpdate == 1)
              &&(configHandle.Check_vac_firm_version<VAC_CHECK_FIRM_VERSION)
              &&(configHandle.Check_vac_firm_version!=0))
          {
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0; // gui 3 lan
            SetVar_i16VACUpdate(35, 100, 1);  
            privateMibBase.vacGroup.vacTable[0].vacUpdateStep = 3;
            
            ReInit2_RS485_MODBUS_UART (172800);            
            s19FileParser.totalByteToSend = configHandle.totalByteToWrite;
            s19FileParser.remainByteToSend = s19FileParser.totalByteToSend;
            s19FileParser.bytePerLine = 0;
            s19FileParser.sentByte = 0;
            s19FileParser.linePerFile = 0;
            while(s19FileParser.remainByteToSend > 0)
            {
              
              GPIO_SetPinsOutput(GPIOE,1u << 3u);
              GPIO_SetPinsOutput(GPIOE,1u << 2u);
              
              s19FileParse(FLASH_FW_START + s19FileParser.sentByte);
              s19FileParser.linePerFile += 1;
              if (s19FileParser.bytePerLine == 0)
              {
                s19FileParser.bytePerLine = s19FileParser.remainByteToSend;
//                s19FileParser.sentByte += s19FileParser.bytePerLine;
//                s19FileParser.remainByteToSend = 0;
//                break;
              }
              
              UART_WriteBlocking(RS4851T_UART, &s19FileParser.line[0], s19FileParser.bytePerLine);
              
              s19FileParser.sentByte += s19FileParser.bytePerLine;
              s19FileParser.remainByteToSend = s19FileParser.totalByteToSend - s19FileParser.sentByte;
              for (uint16_t i = 0; i < 5000; i++)
              {
                __NOP();
              }
              

//              vTaskDelay(1);
            }
            delay2();
            delay2();
            GPIO_ClearPinsOutput(GPIOE,1u << 3u);
            GPIO_ClearPinsOutput(GPIOE,1u << 2u);
            configHandle.isVACUpdate = 0;
            configHandle.devType = _NO_FW_UPDATE;
            configHandle.Check_vac_firm_version=0;
            u8FwUpdateCompleteFlag = 0;
//            configHandle.vacSendUpdateCtrl = 0;
            privateMibBase.siteGroup.siteFirmwareUpdate = 0;
            privateMibBase.vacGroup.vacTable[0].vacUpdateStep = 4;
            vTaskDelay(5000);
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            ReInit2_RS485_MODBUS_UART (14400); 
          }
          else
          {
            //configHandle.Check_vac_firm_version=0;
            if(u8FwUpdateFlag == 2)
              if(cntTimeOutUpdateVAC++ > 20){
                cntTimeOutUpdateVAC = 0;
                configHandle.devType = _NO_FW_UPDATE;
                configHandle.Check_vac_firm_version=0;
              }
            vTaskDelay(1000);
          }
        }
        break;
       case _VACV2_FW_:
         {
           if((configHandle.is_vac_v2_update==1)
              &&(configHandle.Check_vac_firm_version>=VAC_CHECK_FIRM_VERSION)
              &&(configHandle.Check_vac_firm_version!=0))
           {
             Modbus.u8MosbusEn = 0;
             Modbus.u8DataPointer = 0; 
             SetVar_i16VACUpdate_v2(35, 100, 1); // gui 3 lan
             configHandle.is_check_state_vac_v2 = VAC_V2_check_state();
             if(configHandle.is_check_state_vac_v2 == STATE_VAC_V2_APP){
               SetVar_i16VACUpdate_v2(35, 100, 1);
             }
             else if(configHandle.is_check_state_vac_v2 == STATE_VAC_V2_BOOT){
               privateMibBase.vacGroup.vacTable[0].vacUpdateStep = 3;
               
               s19FileParser.totalByteToSend = configHandle.totalByteToWrite;
               s19FileParser.remainByteToSend = s19FileParser.totalByteToSend;
               s19FileParser.bytePerLine = 0;
               s19FileParser.sentByte = 0;
               s19FileParser.linePerFile = 0;
               while(s19FileParser.remainByteToSend > 0)
               {
              
                 GPIO_SetPinsOutput(GPIOE,1u << 3u);
                 GPIO_SetPinsOutput(GPIOE,1u << 2u);
              
                 s19FileParse(FLASH_FW_START + s19FileParser.sentByte);
                 s19FileParser.linePerFile += 1;
                 if (s19FileParser.bytePerLine == 0)
                 {
                    s19FileParser.bytePerLine = s19FileParser.remainByteToSend;
//                  s19FileParser.sentByte += s19FileParser.bytePerLine;
//                  s19FileParser.remainByteToSend = 0;
//                  break;
                 }
              
                 UART_WriteBlocking(RS4851T_UART, &s19FileParser.line[0], s19FileParser.bytePerLine);
              
                 s19FileParser.sentByte += s19FileParser.bytePerLine;
                 s19FileParser.remainByteToSend = s19FileParser.totalByteToSend - s19FileParser.sentByte;
                 for (uint16_t i = 0; i < 5000; i++)
                 {
                  __NOP();
                 }
                 vTaskDelay(2);

//              vTaskDelay(1);
              }
              //send line end=====================================================================
              UART_WriteBlocking(RS4851T_UART, &configHandle.buff_end_CRC32[0], 19);
              for (uint16_t i = 0; i < 5000; i++)
                 {
                  __NOP();
                 }
              //==================================================================================
              delay2();
              delay2();
              GPIO_ClearPinsOutput(GPIOE,1u << 3u);
              GPIO_ClearPinsOutput(GPIOE,1u << 2u);
              configHandle.is_vac_v2_update=0;
              configHandle.devType = _NO_FW_UPDATE;
              configHandle.Check_vac_firm_version=0;
              u8FwUpdateCompleteFlag = 0;
//            configHandle.vacSendUpdateCtrl = 0;
              privateMibBase.siteGroup.siteFirmwareUpdate = 0;
              privateMibBase.vacGroup.vacTable[0].vacUpdateStep = 4;
              vTaskDelay(30000);
              Modbus.u8MosbusEn = 0;
              Modbus.u8DataPointer = 0;
              
              ReInit2_RS485_MODBUS_UART (14400);     
              //memset(&Modbus,0,sizeof(Modbus));
              //UART_Deinit(RS4851R_UART);
              //vTaskDelay(2);
              //Init_RS485_MODBUS_UART();
             
             }   
           }else
           {
             //configHandle.Check_vac_firm_version=0;
             if(u8FwUpdateFlag == 2)
              if(cntTimeOutUpdateVAC++ > 20){
                cntTimeOutUpdateVAC = 0;
                configHandle.devType = _NO_FW_UPDATE;
                configHandle.Check_vac_firm_version=0;
              }
             
             vTaskDelay(1000);
           }
           
         }
         break;
       case _NO_FW_UPDATE:
        {          
          u8FwUpdateCompleteFlag = 0;
          u8FwUpdateFlag = 2;
//          configHandle.vacSendUpdateCtrl = 0;
          vTaskDelay(500);//500
          if(sTestRS485.requesttestRS485 ==1)
          {
                    Modbus.runningStep = _TEST_MODE;
          }
           //UART_Deinit(RS4851R_UART);
           //vTaskDelay(1);
           //Init_RS485_MODBUS_UART();
          switch(sModbusManager.SettingCommand)
          {
            //UART_Deinit(RS4851R_UART);
            //vTaskDelay(1);
            Init_RS485_MODBUS_UART();
#if (USERDEF_RS485_DKD51_BDP == ENABLED)
            case SET_FAN_DPC_STARTING_POINT:
            {
              if(TYPE == DKD51_BDP){
                fan_dpc_switch_uart_t.rx_enable   = STATE_ON;
                Modbus.u8MosbusEn = 0;
                Modbus.u8DataPointer = 0;
                UART_Deinit(RS4851R_UART);
                vTaskDelay(1);
                Init_RS485_MODBUS_UART();
                DKD51_set_fan_starting_point((uint16_t)privateMibBase.fan_dpc_info.mib.temp_set);
                privateMibBase.fan_dpc_info.mib.temp_set = 0;
                //code here
                Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
              }
              else{
                sModbusManager.SettingCommand = SET_DAQ_NOOP;
              }
            }
            break;
            case SET_FAN_DPC_SENSITIVE_POINT:
            {
              if(TYPE == DKD51_BDP){
                fan_dpc_switch_uart_t.rx_enable   = STATE_ON;
                Modbus.u8MosbusEn = 0;
                Modbus.u8DataPointer = 0;
                UART_Deinit(RS4851R_UART);
                vTaskDelay(1);
                Init_RS485_MODBUS_UART();
                DKD51_set_fan_sensitive_point((uint16_t)privateMibBase.fan_dpc_info.mib.temp_set);
                privateMibBase.fan_dpc_info.mib.temp_set = 0;
                //code here
                Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
              }
              else{
                sModbusManager.SettingCommand = SET_DAQ_NOOP;
              }
            }
            break;
            case SET_FAN_DPC_H_TEMP_W_POINT:
            {
              if(TYPE == DKD51_BDP){
                fan_dpc_switch_uart_t.rx_enable   = STATE_ON;
                Modbus.u8MosbusEn = 0;
                Modbus.u8DataPointer = 0;
                UART_Deinit(RS4851R_UART);
                vTaskDelay(1);
                Init_RS485_MODBUS_UART();
                DKD51_set_fan_H_temp_W_point((uint16_t)privateMibBase.fan_dpc_info.mib.temp_set);
                privateMibBase.fan_dpc_info.mib.temp_set = 0;
                //code here
                Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
              }
              else{
                sModbusManager.SettingCommand = SET_DAQ_NOOP;
              }
            }
            break;
            case SET_FAN_DPC_L_TEMP_W_POINT:
            {
              if(TYPE == DKD51_BDP){
                fan_dpc_switch_uart_t.rx_enable   = STATE_ON;
                Modbus.u8MosbusEn = 0;
                Modbus.u8DataPointer = 0;
                UART_Deinit(RS4851R_UART);
                vTaskDelay(1);
                Init_RS485_MODBUS_UART();
                DKD51_set_fan_L_temp_W_point((int16_t)privateMibBase.fan_dpc_info.mib.i_tem_set);
                privateMibBase.fan_dpc_info.mib.i_tem_set = 0;
                //code here
                Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
              }
              else{
                sModbusManager.SettingCommand = SET_DAQ_NOOP;
              }
            }
            break;
            case SET_FAN_DPC_HEATING_START_POINT:
            {
              if(TYPE == DKD51_BDP){
                fan_dpc_switch_uart_t.rx_enable   = STATE_ON;
                Modbus.u8MosbusEn = 0;
                Modbus.u8DataPointer = 0;
                UART_Deinit(RS4851R_UART);
                vTaskDelay(1);
                Init_RS485_MODBUS_UART();
                DKD51_set_fan_heating_start_point((int16_t)privateMibBase.fan_dpc_info.mib.i_tem_set);
                privateMibBase.fan_dpc_info.mib.i_tem_set = 0;
                //code here
                Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
              }
              else{
                sModbusManager.SettingCommand = SET_DAQ_NOOP;
              }             
            }
            break;
            case SET_FAN_DPC_HEATING_SENSITIVE_POINT:
            {
               if(TYPE == DKD51_BDP){
                fan_dpc_switch_uart_t.rx_enable   = STATE_ON;
                Modbus.u8MosbusEn = 0;
                Modbus.u8DataPointer = 0;
                UART_Deinit(RS4851R_UART);
                vTaskDelay(1);
                Init_RS485_MODBUS_UART();
                DKD51_set_fan_heating_sensitive_point((int16_t)privateMibBase.fan_dpc_info.mib.i_tem_set);
                privateMibBase.fan_dpc_info.mib.i_tem_set = 0;
                //code here
                Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
              }
              else{
                sModbusManager.SettingCommand = SET_DAQ_NOOP;
              }             
            }
            break;
            
            
#endif            
#if (USERDEF_MONITOR_BM == ENABLED)
          //================================ BM Setting ===================================//
            case SET_BM_VOLTDIFF:
            {
                if (sModbusManager.u8BMCurrentIndex > sModbusManager.u8NumberOfBM - 1)  // thiet bi hien tai > tong so thiet bi
                {
                    sModbusManager.u8BMCurrentIndex = 0;
                }
                fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
                Modbus.u8DataPointer = 0;
                SetVar_i32(70 + sModbusManager.u8BMCurrentIndex, 32, privateMibBase.bmGroup.bmTable[0].bmVoltDiff);   
                sModbusManager.u8BMCurrentIndex++;                                      // chuyen sang thiet bi tiep
                Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
                
            }
            break;
            case SET_BM_MAXTEMP:
            {
                if (sModbusManager.u8BMCurrentIndex > sModbusManager.u8NumberOfBM - 1)
                {
                    sModbusManager.u8BMCurrentIndex = 0;
                }
                fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
                Modbus.u8DataPointer = 0;
                SetVar_i32(70 + sModbusManager.u8BMCurrentIndex, 34, privateMibBase.bmGroup.bmTable[0].bmMaxTemp);
                sModbusManager.u8BMCurrentIndex++;
                Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
                
            }
            break;
            case SET_BM_VOLTTHRES:
            {
                if (sModbusManager.u8BMCurrentIndex > sModbusManager.u8NumberOfBM - 1)
                {
                    sModbusManager.u8BMCurrentIndex = 0;
                }
                fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
                Modbus.u8DataPointer = 0;
                SetVar_i32(70 + sModbusManager.u8BMCurrentIndex, 36, privateMibBase.bmGroup.bmTable[0].bmVoltThres);
                sModbusManager.u8BMCurrentIndex++;
                Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
                
            }
            break;
            case SET_BM_CURRTHRES:
            {
                if (sModbusManager.u8BMCurrentIndex > sModbusManager.u8NumberOfBM - 1)
                {
                    sModbusManager.u8BMCurrentIndex = 0;
                }
                fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
                Modbus.u8DataPointer = 0;
                SetVar_i32(70 + sModbusManager.u8BMCurrentIndex, 38, privateMibBase.bmGroup.bmTable[0].bmCurrThres);
                sModbusManager.u8BMCurrentIndex++;
                Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
                
            }
            break;
            case SET_BM_TIMETHRES:
            {
                if (sModbusManager.u8BMCurrentIndex > sModbusManager.u8NumberOfBM - 1)
                {
                    sModbusManager.u8BMCurrentIndex = 0;
                }
                fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
                Modbus.u8DataPointer = 0;
                SetVar_i32(70 + sModbusManager.u8BMCurrentIndex, 40, privateMibBase.bmGroup.bmTable[0].bmTimeThres);
                sModbusManager.u8BMCurrentIndex++;
                Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
                
            }
            break;
            case SET_BM_SOCTHRES:
            {
                if (sModbusManager.u8BMCurrentIndex > sModbusManager.u8NumberOfBM - 1)
                {
                    sModbusManager.u8BMCurrentIndex = 0;
                }
                fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
                Modbus.u8DataPointer = 0;
                SetVar_i32(70 + sModbusManager.u8BMCurrentIndex, 42, privateMibBase.bmGroup.bmTable[0].bmSOCThres);
                sModbusManager.u8BMCurrentIndex++;
                Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
                
            }
            break;
            case SET_BM_MINTEMP:
            {
                if (sModbusManager.u8BMCurrentIndex > sModbusManager.u8NumberOfBM - 1)
                {
                    sModbusManager.u8BMCurrentIndex = 0;
                }
                fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
                Modbus.u8DataPointer = 0;
                SetVar_i32(70 + sModbusManager.u8BMCurrentIndex, 44, privateMibBase.bmGroup.bmTable[0].bmMinTemp);
                sModbusManager.u8BMCurrentIndex++;
                Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
                
            }
            break;
            case SET_BM_LOWCAPTIME:
            {
                if (sModbusManager.u8BMCurrentIndex > sModbusManager.u8NumberOfBM - 1)
                {
                    sModbusManager.u8BMCurrentIndex = 0;
                }
                fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
                Modbus.u8DataPointer = 0;
                SetVar_i32(70 + sModbusManager.u8BMCurrentIndex, 46, privateMibBase.bmGroup.bmTable[0].bmLowCapTime);
                sModbusManager.u8BMCurrentIndex++;
                Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
                
            }
            break;
            case SET_BM_HALFVOLTALARM:
            {
                if (sModbusManager.u8BMCurrentIndex > sModbusManager.u8NumberOfBM - 1)
                {
                    sModbusManager.u8BMCurrentIndex = 0;
                }
                fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
                Modbus.u8DataPointer = 0;
                SetVar_i32(70 + sModbusManager.u8BMCurrentIndex, 50, privateMibBase.bmGroup.bmTable[0].bmHalfVoltAlarm);
                sModbusManager.u8BMCurrentIndex++;
                Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
                
            }
            break;
            case SET_BM_RECHARGEVOLT:
            {
                if (sModbusManager.u8BMCurrentIndex > sModbusManager.u8NumberOfBM - 1)
                {
                    sModbusManager.u8BMCurrentIndex = 0;
                }
                fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
                Modbus.u8DataPointer = 0;
                SetVar_i32(70 + sModbusManager.u8BMCurrentIndex, 52, privateMibBase.bmGroup.bmTable[0].bmRechargeVolt);
                sModbusManager.u8BMCurrentIndex++;
                Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
                
            }
            break;
            case SET_BM_CURRBALANCETHRES:
            {
                if (sModbusManager.u8BMCurrentIndex > sModbusManager.u8NumberOfBM - 1)
                {
                    sModbusManager.u8BMCurrentIndex = 0;
                }
                fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
                Modbus.u8DataPointer = 0;
                SetVar_i32(70 + sModbusManager.u8BMCurrentIndex, 54, privateMibBase.bmGroup.bmTable[0].bmCurrBalanceThres);
                sModbusManager.u8BMCurrentIndex++;
                Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
                
            }
            break;
            //================================ BM Setting ===================================//
#endif          
#if (USERDEF_MONITOR_VAC == ENABLED)        
        case SET_VAC_SYSMODE:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 0, privateMibBase.vacGroup.vacTable[0].vacSysModeWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_FANSPEEDMODE:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 1, privateMibBase.vacGroup.vacTable[0].vacFanSpeedModeWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_FANSTARTTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 2, privateMibBase.vacGroup.vacTable[0].vacFanStartTempWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_SETTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 3, privateMibBase.vacGroup.vacTable[0].vacSetTempWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_PIDOFFSETTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 4, privateMibBase.vacGroup.vacTable[0].vacPidOffsetTempWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_FANMINSPEED:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 5, privateMibBase.vacGroup.vacTable[0].vacFanMinSpeedWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_FANMAXSPEED:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 6, privateMibBase.vacGroup.vacTable[0].vacFanMaxSpeedWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_FILTERSTUCKTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 7, privateMibBase.vacGroup.vacTable[0].vacFilterStuckTempWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_NIGHTMODEEN:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 8, privateMibBase.vacGroup.vacTable[0].vacNightModeEnWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_NIGHTMODESTART:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 9, privateMibBase.vacGroup.vacTable[0].vacNightModeStartWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_NIGHTMODEEND:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 10, privateMibBase.vacGroup.vacTable[0].vacNightModeEndWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_NIGHTMAXSPEED:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 11, privateMibBase.vacGroup.vacTable[0].vacNightMaxSpeedWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_MANUALMODE:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 12, privateMibBase.vacGroup.vacTable[0].vacManualModeWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_MANUALMAXSPEED:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 13, privateMibBase.vacGroup.vacTable[0].vacManualMaxSpeedWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_INMAXTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 14, privateMibBase.vacGroup.vacTable[0].vacInMaxTempWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_EXMAXTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 15, privateMibBase.vacGroup.vacTable[0].vacExMaxTempWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_FROSTMAXTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 16, privateMibBase.vacGroup.vacTable[0].vacFrostMaxTempWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_INMINTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 17, privateMibBase.vacGroup.vacTable[0].vacInMinTempWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_EXMINTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 18, privateMibBase.vacGroup.vacTable[0].vacExMinTempWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_FROSTMINTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 19, privateMibBase.vacGroup.vacTable[0].vacFrostMinTempWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_MINOUTTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 20, privateMibBase.vacGroup.vacTable[0].vacMinOutTempWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_DELTATEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 21, privateMibBase.vacGroup.vacTable[0].vacDeltaTempWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_PANICTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 22, privateMibBase.vacGroup.vacTable[0].vacPanicTempWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_ACU1ONTEMP:
        {   
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 23, privateMibBase.vacGroup.vacTable[0].vacACU1OnTemp);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_ACU2ONTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 24, privateMibBase.vacGroup.vacTable[0].vacACU2OnTemp);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_ACU2EN:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 25, privateMibBase.vacGroup.vacTable[0].vacACU2En);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_SYNCTIME:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_Time(35, 26, SyncTime);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_AIRCON1MODEL:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 30, privateMibBase.vacGroup.vacTable[0].vacAirCon1ModelWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_AIRCON1TYPE:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 31, privateMibBase.vacGroup.vacTable[0].vacAirCon1TypeWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_AIRCON2MODEL:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 32, privateMibBase.vacGroup.vacTable[0].vacAirCon2ModelWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_AIRCON2TYPE:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 33, privateMibBase.vacGroup.vacTable[0].vacAirCon2TypeWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_AIRCONONOFF:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 34, privateMibBase.vacGroup.vacTable[0].vacAirConOnOffWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_AIRCONMODE:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 35, privateMibBase.vacGroup.vacTable[0].vacAirConModeWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_AIRCONTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 36, privateMibBase.vacGroup.vacTable[0].vacAirConTempWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_AIRCONSPEED:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 37, privateMibBase.vacGroup.vacTable[0].vacAirConSpeedWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_VAC_AIRCONDDIR:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 38, privateMibBase.vacGroup.vacTable[0].vacAircondDirWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;  
        case SET_VAC_W_ENABLE:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 80, privateMibBase.vacGroup.vacTable[0].vacWEnableWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break; 
        case SET_VAC_W_SERIAL1:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 81, privateMibBase.vacGroup.vacTable[0].vacWSerial1Write);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break; 
        case SET_VAC_W_SERIAL2:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 82, privateMibBase.vacGroup.vacTable[0].vacWSerial2Write);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break; 
        case SET_VAC_RESET:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i16VAC(35, 47, 1);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break; 
  #endif
#if (USERDEF_MONITOR_SMCB == ENABLED) 
        case SET_SMCB_STATE:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            vTaskDelay(1000); // do toc do xu ly ban tin ko dung id cham, nen phai them tre
            Modbus.u8DataPointer = 0;
            if (sModbusManager.sSMCBManager[privateMibBase.smcbGroup.flag].u8SMCBType == 1) // OPEN
              SetVar_FC06SMCB((uint8_t)privateMibBase.smcbGroup.SmcbTable[privateMibBase.smcbGroup.flag].SmcbModbusID, 0,(uint16_t) privateMibBase.smcbGroup.SmcbTable[privateMibBase.smcbGroup.flag].SmcbStateWrite);
            else if (sModbusManager.sSMCBManager[privateMibBase.smcbGroup.flag].u8SMCBType == 2) // MATIS
              SetVar_FC06SMCB((uint8_t)privateMibBase.smcbGroup.SmcbTable[privateMibBase.smcbGroup.flag].SmcbModbusID, 17,(uint16_t) privateMibBase.smcbGroup.SmcbTable[privateMibBase.smcbGroup.flag].SmcbStateWrite+1);
            else if (sModbusManager.sSMCBManager[privateMibBase.smcbGroup.flag].u8SMCBType == 3) // GOL
              SetVar_FC06SMCB((uint8_t)privateMibBase.smcbGroup.SmcbTable[privateMibBase.smcbGroup.flag].SmcbModbusID, 32768,(uint16_t) privateMibBase.smcbGroup.SmcbTable[privateMibBase.smcbGroup.flag].SmcbStateWrite+1);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
#endif        
        case SET_GEN_AUTOMODE:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
          if (sModbusManager.sGenManager[privateMibBase.genGroup.genflag].u8GenType == 1) // KOBUTA
          {
            Modbus.u8DataPointer = 0;
            SetVar_FC06GEN((uint8_t)privateMibBase.genGroup.genTable[privateMibBase.genGroup.genflag].genStatusModbusID, 760,0x10);
//            sModbusManager.SettingCommand = _WAIT_WRITE_MULTI_REG;
          }
          else if (sModbusManager.sGenManager[privateMibBase.genGroup.genflag].u8GenType == 3) // DEEPSEA
          {
            Modbus.u8DataPointer = 0;
            SetVar_i16GEN((uint8_t)privateMibBase.genGroup.genTable[privateMibBase.genGroup.genflag].genStatusModbusID,4104,35701);
//            sModbusManager.SettingCommand = _WAIT_WRITE_MULTI_REG;
          }
//          else
//          {
//            sModbusManager.SettingCommand = _WAIT_WRITE_MULTI_REG;
//          }  
          Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
          
        }
        break;
        case SET_GEN_MANUALMODE:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
          if (sModbusManager.sGenManager[privateMibBase.genGroup.genflag].u8GenType == 1) // KOBUTA
          {
            Modbus.u8DataPointer = 0;
            SetVar_FC06GEN((uint8_t)privateMibBase.genGroup.genTable[privateMibBase.genGroup.genflag].genStatusModbusID, 760,0x40);
//            sModbusManager.SettingCommand = _WAIT_WRITE_MULTI_REG;
          }
          else if (sModbusManager.sGenManager[privateMibBase.genGroup.genflag].u8GenType == 3) // DEEPSEA
          {
            Modbus.u8DataPointer = 0;
            SetVar_i16GEN((uint8_t)privateMibBase.genGroup.genTable[privateMibBase.genGroup.genflag].genStatusModbusID,4104,35702);
//            sModbusManager.SettingCommand = _WAIT_WRITE_MULTI_REG;
          }
//          else
//          {
//            sModbusManager.SettingCommand = _WAIT_WRITE_MULTI_REG;
//          }  
          Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
          
        }
        break;
        case SET_GEN_STARTMODE:
        {
          fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
          Modbus.u8MosbusEn = 0;
          if (sModbusManager.sGenManager[privateMibBase.genGroup.genflag].u8GenType == 1) // KOBUTA
          {
            Modbus.u8DataPointer = 0;
            SetVar_FC06GEN((uint8_t)privateMibBase.genGroup.genTable[privateMibBase.genGroup.genflag].genStatusModbusID, 760,0x01);
//            sModbusManager.SettingCommand = _WAIT_WRITE_MULTI_REG;
          }
          else if (sModbusManager.sGenManager[privateMibBase.genGroup.genflag].u8GenType == 3) // DEEPSEA
          {
            Modbus.u8DataPointer = 0;
            SetVar_i16GEN((uint8_t)privateMibBase.genGroup.genTable[privateMibBase.genGroup.genflag].genStatusModbusID,4104,35705);
//            sModbusManager.SettingCommand = _WAIT_WRITE_MULTI_REG;
          }
//          else
//          {
//            sModbusManager.SettingCommand = _WAIT_WRITE_MULTI_REG;
//          }
          Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
          
        }
        break;
        case SET_GEN_STOPMODE:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
          if (sModbusManager.sGenManager[privateMibBase.genGroup.genflag].u8GenType == 1) // KOBUTA
          {
            Modbus.u8DataPointer = 0;
            SetVar_FC06GEN((uint8_t)privateMibBase.genGroup.genTable[privateMibBase.genGroup.genflag].genStatusModbusID, 760,0x02);
//            sModbusManager.SettingCommand = _WAIT_WRITE_MULTI_REG;
          }
          else if (sModbusManager.sGenManager[privateMibBase.genGroup.genflag].u8GenType == 3) // DEEPSEA
          {
            Modbus.u8DataPointer = 0;
            SetVar_i16GEN((uint8_t)privateMibBase.genGroup.genTable[privateMibBase.genGroup.genflag].genStatusModbusID,4104,35700);
//            sModbusManager.SettingCommand = _WAIT_WRITE_MULTI_REG;
          }
//          else
//          {
//            sModbusManager.SettingCommand = _WAIT_WRITE_MULTI_REG;
//          }
          Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
          
        }
        break;
        case SET_LIB_PACKHIGHVOLT:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 0, privateMibBase.liBattGroup.liBattTable[0].i32PackHighVolt_AWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_BATTHIGHVOLT:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 2, privateMibBase.liBattGroup.liBattTable[0].i32BattHighVolt_AWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_CELLHIGHVOLT:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 4, privateMibBase.liBattGroup.liBattTable[0].i32CellHighVolt_AWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_BATTLOWVOLT:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 6, privateMibBase.liBattGroup.liBattTable[0].i32BattLowVolt_AWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;   
        }
        break;
        case SET_LIB_CELLLOWVOLT:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 8, privateMibBase.liBattGroup.liBattTable[0].i32CellLowVolt_AWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;  
        }
        break;
        case SET_LIB_CHARHIGHCURR:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 10, privateMibBase.liBattGroup.liBattTable[0].i32CharHighCurr_AWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
        }
        break;
        case SET_LIB_DISCHARHIGHCURR:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 12, privateMibBase.liBattGroup.liBattTable[0].i32DisCharHighCurr_AWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_CHARHIGHTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 14, privateMibBase.liBattGroup.liBattTable[0].i32CharHighTemp_AWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_DISCHARHIGHTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 16, privateMibBase.liBattGroup.liBattTable[0].i32DisCharHighTemp_AWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_CHARLOWTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 18, privateMibBase.liBattGroup.liBattTable[0].i32CharLowTemp_AWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_DISCHARLOWTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 20, privateMibBase.liBattGroup.liBattTable[0].i32DisCharLowtemp_AWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_LOWCAP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 22, privateMibBase.liBattGroup.liBattTable[0].i32LowCap_AWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_BMSHIGHTEMP:
        {
           fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 24, privateMibBase.liBattGroup.liBattTable[0].i32BMSHighTemp_AWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_BMSLOWTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 26, privateMibBase.liBattGroup.liBattTable[0].i32BMSLowTemp_AWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_PACKOVERVOLT:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 28, privateMibBase.liBattGroup.liBattTable[0].i32PackOverVolt_PWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_BATTOVERVOLT:
        {  
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 30, privateMibBase.liBattGroup.liBattTable[0].i32BattOverVolt_PWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_CELLOVERVOLT:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 32, privateMibBase.liBattGroup.liBattTable[0].i32CellOverVolt_PWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_BATTUNDERVOLT:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 34, privateMibBase.liBattGroup.liBattTable[0].i32BattUnderVolt_PWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_CELLUNDERVOLT:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 36, privateMibBase.liBattGroup.liBattTable[0].i32CellUnderVolt_PWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_CHAROVERCURR:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 38, privateMibBase.liBattGroup.liBattTable[0].i32CharOverCurr_PWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_DISCHAROVERCURR:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 40, privateMibBase.liBattGroup.liBattTable[0].i32DisCharOverCurr_PWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_CHAROVERTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 42, privateMibBase.liBattGroup.liBattTable[0].i32CharOverTemp_PWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_DISCHAROVERTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 44, privateMibBase.liBattGroup.liBattTable[0].i32DisCharOverTemp_PWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_CHARUNDERTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 46, privateMibBase.liBattGroup.liBattTable[0].i32CharUnderTemp_PWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_DISCHARUNDERTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 48, privateMibBase.liBattGroup.liBattTable[0].i32DisCharUnderTemp_PWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_UNDERCAP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 50, privateMibBase.liBattGroup.liBattTable[0].i32UnderCap_PWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_BMSOVERTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 52, privateMibBase.liBattGroup.liBattTable[0].i32BMSOverTemp_PWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_BMSUNDERTEMP:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 54, privateMibBase.liBattGroup.liBattTable[0].i32BMSUnderTemp_PWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_DIFFERENTVOLT:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 56, privateMibBase.liBattGroup.liBattTable[0].i32DifferentVolt_PWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_VOLTBALANCE:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 60, privateMibBase.liBattGroup.liBattTable[0].u32VoltBalanceWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_DELTAVOLTBALANCE:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 62, privateMibBase.liBattGroup.liBattTable[0].u32DeltaVoltBalanceWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_DISCHARCURRLIMIT:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 64, privateMibBase.liBattGroup.liBattTable[0].u32DisCharCurrLimitWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_CHARCURRLIMIT:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 66, privateMibBase.liBattGroup.liBattTable[0].u32CharCurrLimitWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_VOLTDISCHARREF:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 72, privateMibBase.liBattGroup.liBattTable[0].u32VoltDisCharRefWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_VOLTDISCHARCMD:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_i32LIB(LibM1Addr, 74, privateMibBase.liBattGroup.liBattTable[0].u32VoltDisCharCMDWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_SYSTEMBYTE:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_u16LIB(LibM1Addr, 76, (uint16_t)privateMibBase.liBattGroup.liBattTable[0].u32SystemByteWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_KEYTIME:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_u16LIB(LibM1Addr, 77, (uint16_t)privateMibBase.liBattGroup.liBattTable[0].u32KeyTimeWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_INIMAXIM:
        {
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_u16LIB(LibM1Addr, 162, (uint16_t)privateMibBase.liBattGroup.liBattTable[0].u32IniMaximWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_ENABLELOCK:
        {   
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_u16LIB(LibM1Addr, 115, (uint16_t)privateMibBase.liBattGroup.liBattTable[0].u32EnableLockWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        case SET_LIB_CAPINIT:
        {   
            fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
            Modbus.u8MosbusEn = 0;
            Modbus.u8DataPointer = 0;
            SetVar_u16LIB(LibM1Addr, 164, (uint16_t)privateMibBase.liBattGroup.liBattTable[0].u16_liionCapInitWrite);
            Modbus.runningStep = _WAIT_WRITE_MULTI_REG;
            
        }
        break;
        default:
          break;            
      };
          
          switch(Modbus.runningStep)
          {
          case _TEST_MODE:    
            {
              sTestRS485.Cnt++;
              if(sTestRS485.RS485CabRecv!=0)
              {

              }
              else
              {
                 for(i=0;i<15;i++)
                  {
                    if(sTestRS485.CabBuff[i] ==i)
                      {
                         sTestRS485.Numberofbyterecvok++;
                      } 
                  }
                    if(sTestRS485.Numberofbyterecvok==15)
                      {
                        sTestRS485.RS485CabRecv =1;
                        sTestRS485.Numberofbyterecvok = 0;
                        
                        GPIO_SetPinsOutput(GPIOD,1u << 0u);
                        GPIO_SetPinsOutput(GPIOD,1u << 1u);
                        UART_WriteBlocking(RS485R_UART, &sTestRS485.CabBuff[0], 15);
                        delay2();
                        GPIO_ClearPinsOutput(GPIOD,1u << 0u);
                        GPIO_ClearPinsOutput(GPIOD,1u << 1u); 
                   
                      }
                    else 
                      {
                        sTestRS485.RS485CabRecv =0;
                        sTestRS485.Numberofbyterecvok = 0;
                      }
                   
              }
              if(sTestRS485.RS485GenRecv!=0)
              {

              }
              else
              {
                 for(i=0;i<15;i++)
                  {
                    if(sTestRS485.GenBuff[i] ==i)
                      {
                         sTestRS485.Numberofbyterecvok++;
                      } 
                  }
                    if(sTestRS485.Numberofbyterecvok==15)
                      {
                        sTestRS485.RS485GenRecv =1;
                        sTestRS485.Numberofbyterecvok = 0;
                        
                        GPIO_SetPinsOutput(GPIOE,1u << 3u);
                        GPIO_SetPinsOutput(GPIOE,1u << 2u);
                        UART_WriteBlocking(RS4851T_UART, &sTestRS485.GenBuff[0], 15);
                        delay2();
                        GPIO_ClearPinsOutput(GPIOE,1u << 3u);
                        GPIO_ClearPinsOutput(GPIOE,1u << 2u); 
                     
                      }
                    else 
                      {
                        sTestRS485.RS485GenRecv =0;
                        sTestRS485.Numberofbyterecvok = 0;
                      }
                   
              }
             if(sTestRS485.Cnt>10) 
              {
                  UART_Deinit(RS485T_UART); 
                  Modbus.runningStep = _WAIT_MODE;
                  sTestRS485.requesttestRS485=0;
                  sTestRS485.InitUart=0;
              }
               
            }
            break;
          case _WAIT_MODE:
            {
              fan_dpc_switch_uart_t.rx_enable    = STATE_OFF;
              fan_dpc_switch_uart_t.fan_state_e  = _DKD51_FAN_SYS_REQ;
              vTaskDelay(1000);
              Modbus.runningStep = _READ_LIB_STATUS;
            }
            break;
              
  #if (USERDEF_MONITOR_LIB == ENABLED)
          case _READ_LIB_STATUS:
          {
              Modbus.u8DataPointer = 0;
              if(sModbusManager.u8NumberOfLIB != 0)
              {
                  if (sModbusManager.u8LIBCurrentIndex > sModbusManager.u8NumberOfLIB - 1)
                  {
                      sModbusManager.u8LIBCurrentIndex = 0;
                  }
                  switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBType)
                  {
                  case 13:// COSLIGHT_CF4850T  
                  case 1:// COSLIGHT  
                  {
                      ReInit2_RS485_MODBUS_UART(28800);
                      sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID = sModbusManager.u8LIBCurrentIndex + 1;
                      switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep)
                      {
                      case LIB_COSLIGHT_INFO_1:
                      {
                          Read_Input_Regs_Query_coslight(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,//Read_Input_Regs_Query
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[0],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[0]);
                      }
                      break;
                      case LIB_COSLIGHT_INFO_2:
                      {
                          Read_Input_Status_Query_coslight(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[1],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[1]);
                      }
                      break;
                      case LIB_COSLIGHT_INFO_3:
                      {
                          Read_Holding_Regs_Query_coslight(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[2],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[2]);
                      }
                      break;
                      default:
                      break;
                      };
                  }
                  break;
                  case 2:// COSLIGHT_OLD_V1.1  
                  {
                      ReInit2_RS485_MODBUS_UART(28800);
                      sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID = sModbusManager.u8LIBCurrentIndex + 1;
                      switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep)
                      {
                      case LIB_COSLIGHT_OLD_INFO_1:
                      {
                          Read_Input_Regs_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[0],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[0]);
                      }
                      break;
                      case LIB_COSLIGHT_OLD_INFO_2:
                      {
                          Read_Input_Status_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[1],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[1]);
                      }
                      break;
                      case LIB_COSLIGHT_OLD_INFO_3:
                      {
                          Read_Holding_Regs_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[2],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[2]);
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
                    Init_RS485_MODBUS_UART();
                    sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID = sModbusManager.u8LIBCurrentIndex + 1;
                      switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep)
                      {
                      case LIB_SHOTO_INFO_1:
                      {
                          Read_Input_Regs_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[0],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[0]);
                      }
                      break;
                      case LIB_SHOTO_INFO_2:
                      {
                          Read_Input_Regs_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[1],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[1]);
                      }
                      break;
                      case LIB_SHOTO_INFO_3:
                      {
                          Read_Coil_Status_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[2],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[2]);
                      }
                      break;
                      case LIB_SHOTO_INFO_4:
                      {
                          Read_Input_Regs_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[3],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[3]);
                      }
                      break;
                      default:
                      break;
                      };
                  }
                  break;
                  case 4:// HUAWEI
                  {
                    Init_RS485_MODBUS_UART();
                    if(sModbusManager.u8LIBCurrentIndex < 8)
                      sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID = sModbusManager.u8LIBCurrentIndex + 214;
                    else
                      sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID = sModbusManager.u8LIBCurrentIndex + 216;
                    switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep)
                      {
                      case LIB_HUAWEI_INFO_1:
                      {
                          Read_Holding_Regs_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[0],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[0]);//39,4096,9
                      }
                      break;
                      case LIB_HUAWEI_INFO_2:
                      {
                          Read_Holding_Regs_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[1],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[1]);//39,4096,9
                      }
                      break;
                      case LIB_HUAWEI_INFO_3:
                      {
                          Read_Holding_Regs_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[2],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[2]);//39,4096,9
                      }
                      break;
                      case LIB_HUAWEI_INFO_4:
                      {
                          Read_Holding_Regs_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[3],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[3]);//39,4096,9
                      }
                      break;
                      default:
                      break;
                      };
                  }
                  break;
                  case 5:// M1Viettel50
                  {
                    Init_RS485_MODBUS_UART();
                      switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep)
                      {
                      case LIB_VIETTEL_INFO_1:
                      {
                          Read_Input_Regs_Query_fast(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                              sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[0],
                                              sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[0]);//1,0,61
                      }
                      break;
                      case LIB_VIETTEL_INFO_2:
                      {
                          Read_Input_Regs_Query_fast(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                              sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[1],
                                              sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[1]);//1,0,61
                      }
                      break;
                      case LIB_VIETTEL_INFO_3:
                      {
                          Read_Holding_Regs_Query_Deepsea(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                              sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[2],
                                              sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[2]);//1,0,61
                      }
                      break;
                      case LIB_VIETTEL_INFO_4:
                      {
                          Read_Holding_Regs_Query_Deepsea(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                              sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[3],
                                              sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[3]);//1,0,61
                      }
                      break;
                      case LIB_VIETTEL_INFO_5:
                      {
                          LoadCurr = privateMibBase.loadGroup.loadCurrent;
                          ACFaultM1 = privateMibBase.mainAlarmGroup.alarmACmains;
                          PMUConnectM1 = privateMibBase.connAlarmGroup.alarmPMUConnect;
                          Read_Holding_Regs_Query_Deepsea(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                              110,
                                              6);//1,0,61
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
                    Init_RS485_MODBUS_UART();
                    sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID = sModbusManager.u8LIBCurrentIndex + 1;
                      switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep)
                      {
                      case LIB_ZTT_2020_INFO_1:
                      {
                          Read_Holding_Regs_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[0],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[0]);
                      }
                      break;
                      case LIB_ZTT_2020_INFO_2:
                      {
                          Read_Holding_Regs_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[1],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[1]);
                      }
                      break;
                      case LIB_ZTT_2020_INFO_3:
                      {
                          Read_Holding_Regs_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[2],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[2]);
                      }
                      break;
                      default:
                      break;
                      };
                  }
                break;
                  case 8:// HUAWEI_A1
                  {
                    Init_RS485_MODBUS_UART();
                    if(sModbusManager.u8LIBCurrentIndex < 8)
                      sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID = sModbusManager.u8LIBCurrentIndex + 214;
                    else
                      sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID = sModbusManager.u8LIBCurrentIndex + 216;
                      switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep)
                      {
                      case LIB_HUAWEI_A1_INFO_1:
                      {
                          Read_Holding_Regs_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[0],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[0]);//39,4096,9
                      }
                      break;
                      case LIB_HUAWEI_A1_INFO_2:
                      {
                          Read_Holding_Regs_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[1],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[1]);//39,4096,9
                      }
                      break;
                      case LIB_HUAWEI_A1_INFO_3:
                      {
                          Read_Holding_Regs_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[2],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[2]);//39,4096,9
                      }
                      break;
                      default:
                      break;
                      };
                  }
                  break;
//                
                  case 9:// SAFT
                  {
                    Init_RS485_MODBUS_UART();
                    sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID = sModbusManager.u8LIBCurrentIndex + 1;
                    switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep)
                    {
                      case LIB_SAFT_INFO_1:
                      {
                        uint8_t key_saft[4] = 0;
                        Write_Multi_Reg (sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                         0x04, 
                                         &key_saft[0], 
                                         2);
                        vTaskDelay(1000);
                        Modbus.u8MosbusEn = 0;
                        Read_Holding_Regs_Query_Deepsea(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                        sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[0],
                                                        sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[0]);
                      }
                      break;
                      default:
                      break;
                    }
                  }
                  break;
                  case 10:// Narada75
                  {
                    Init_RS485_MODBUS_UART();
                    sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID = sModbusManager.u8LIBCurrentIndex + 39;
                    switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep)
                      {
                      case LIB_NARADA75_INFO_1:
                      {
                          Read_Input_Regs_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[0],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[0]);
                      }
                      break;
                      case LIB_NARADA75_INFO_2:
                      {
                          Read_Product_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID);
                      }
                      break;
                      default:
                      break;
                      };
                  }
                  break;
                  case 7:// ZTT50
                  {
                    Init_RS485_MODBUS_UART();
                    sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID = sModbusManager.u8LIBCurrentIndex + 39;
                      switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep)
                      {
                      case LIB_ZTT_INFO_1:
                      {
                          Read_Input_Regs_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[0],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[0]);//39,4096,9
                      }
                      break;
                      case LIB_ZTT_INFO_2:
                      {
                          Read_Product_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID);
                      }
                      break;        
                      default:
                      break;
                      };
                  }
                  break;
                  case 11:// EVE
                  {
                    Init_RS485_MODBUS_UART();
                    sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID = sModbusManager.u8LIBCurrentIndex + 1;
                      switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep)
                      {
                      case LIB_EVE_INFO_1:
                      {
                          Read_Holding_Regs_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[0],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[0]);//39,4096,9
                      }
                      break;
                      case LIB_EVE_INFO_2:
                      {
                          Read_Holding_Regs_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[1],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[1]);//39,4096,9
                      }
                      break;
                      case LIB_EVE_INFO_3:
                      {
                          Read_Holding_Regs_Query(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBAbsSlaveID,
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u32LIBBaseAddr[2],
                                                sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBNumOfReg[2]);//39,4096,9
                      }
                      break;
                      default:
                      break;
                      };
                  }
                break;
                }
                Modbus.runningStep = _WAIT_LIB_RESPOND;
              }
              else
              {
                  Modbus.runningStep = _READ_GEN_STATUS;
//                  Init_RS485_MODBUS_UART();
              }
          }
          break;
          case _WAIT_LIB_RESPOND:
          {
              reVal = RS4851_Check_Respond_Data_LIB();
              if(sModbusManager.u8LIBCurrentIndex <= (sModbusManager.u8NumberOfLIB - 1))
              {
                  if(reVal != 1)
                  {
                      sLiionBattInfo[sModbusManager.u8LIBCurrentIndex].u8NoResponseCnt++;
                      if (sLiionBattInfo[sModbusManager.u8LIBCurrentIndex].u8NoResponseCnt >= 5)
                      {
                          sLiionBattInfo[sModbusManager.u8LIBCurrentIndex].u8ErrorFlag = 1;
                          cntDisConnectLIB[sModbusManager.u8LIBCurrentIndex]++;
                          switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBType)
                          {
                          case 13:// COSLIGHT_CF4850T  
                          case 1:// COSLIGHT
                          {
                            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_COSLIGHT_INFO_1;
                          }
                          break;
                          case 2:
                          {
                            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_COSLIGHT_OLD_INFO_1;
                          }
                          break;
                          case 12:// SHOTO_SDA10_48100 
                          case 3:// SHOTO_2019
                          {
                            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_SHOTO_INFO_1;
                          }
                          break;
                          case 4:
                          {
                            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_HUAWEI_INFO_1;
                          }
                          break;
                          case 5:
                          {
                            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_VIETTEL_INFO_1;
                          }
                          break;
                          case 14:// HUAFU_HF48100C
                          case 6:// ZTT_2020
                          {
                            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_ZTT_2020_INFO_1;
                          }
                          break;
                          case 7:
                          {
                            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_ZTT_INFO_1;
                          }
                          break;
                          case 8:
                          {
                            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_HUAWEI_A1_INFO_1;
                          }
                          break;
                          case 9:
                          {
                            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_SAFT_INFO_1;
                          }
                          break;
                          case 10:
                          {
                            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_NARADA75_INFO_1;
                          }
                          break;
                          case 11:
                          {
                            sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep = LIB_EVE_INFO_1;
                          }
                          break;
                          };      
                          sModbusManager.u8LIBCurrentIndex++;
                      }
                  }
                  else
                  {
                      sLiionBattInfo[sModbusManager.u8LIBCurrentIndex].u8NoResponseCnt = 0;
//                      sLiionBattInfo[sModbusManager.u8LIBCurrentIndex].u8ErrorFlag = 0;
                      switch(sModbusManager.sLIBManager[sModbusManager.u8LIBCurrentIndex].u8LIBRunningStep)
                      {
                      case LIB_COSLIGHT_INFO_1:
                      case LIB_COSLIGHT_OLD_INFO_1:
                      case LIB_SHOTO_INFO_1:
                      case LIB_HUAWEI_INFO_1:
                      case LIB_VIETTEL_INFO_1:
                      case LIB_ZTT_2020_INFO_1:
                      case LIB_ZTT_INFO_1:
                      case LIB_HUAWEI_A1_INFO_1:
                      case LIB_SAFT_INFO_1:
                      case LIB_NARADA75_INFO_1:
                      case LIB_EVE_INFO_1:
                      {
                        sModbusManager.u8LIBCurrentIndex++;
                      }break;                      
                      };
                  }

                  if (sModbusManager.u8LIBCurrentIndex > sModbusManager.u8NumberOfLIB - 1)
                  {
                      Modbus.runningStep = _READ_GEN_STATUS;
                      //Init_RS485_MODBUS_UART();
                  }
                  else
                  {
                      Modbus.runningStep = _READ_LIB_STATUS;
                  }
              }
              else Modbus.runningStep = _READ_LIB_STATUS;
          }
          break;
  #endif
  #if (USERDEF_MONITOR_GEN == ENABLED)
          case _READ_GEN_STATUS:
          {
              Modbus.u8DataPointer = 0;
              if(sModbusManager.u8NumberOfGen != 0)
              {
                  if (sModbusManager.u8GenCurrentIndex > sModbusManager.u8NumberOfGen - 1)
                  {
                      sModbusManager.u8GenCurrentIndex = 0;
                  }
                  switch(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenType)
                  {
                  case 1:// KUBOTA
                  {
                    Init_RS485_MODBUS_UART();
                      switch(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep)
                      {
                      case GEN_KUBOTA_INFO_1:
                      {
                          Read_Input_Regs_Query(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenAbsSlaveID,
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenBaseAddr[0],
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenNumOfReg[0]);//1,0x00,85
                      }
                      break;
                      case GEN_KUBOTA_INFO_2:
                      {
                          Read_Input_Regs_Query(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenAbsSlaveID,
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenBaseAddr[1],
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenNumOfReg[1]);//1,0x20,5
                      }
                      break;
                      default:
                      break;
                      };
                  }
                  break;
                  case 2:// BE142
                  {
                    Init_RS485_MODBUS_UART();
                      switch(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep)
                      {
                      case GEN_BE142_INFO_1:
                      {
                          Read_Input_Regs_Query(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenAbsSlaveID,
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenBaseAddr[0],
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenNumOfReg[0]);
                      }
                      break;
                      case GEN_BE142_INFO_2:
                      {
                          Read_Input_Regs_Query(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenAbsSlaveID,
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenBaseAddr[1],
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenNumOfReg[1]);
                      }
                      break;
                      case GEN_BE142_INFO_3:
                      {
                          Read_Input_Regs_Query(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenAbsSlaveID,
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenBaseAddr[2],
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenNumOfReg[2]);
                      }
                      break;
                      default:
                      break;
                      };
                  }
                  break;
                  case 3:// DEEPSEA
                  {
                    Init_RS485_MODBUS_UART();
                      switch(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep)
                      {
                      case GEN_DEEPSEA_INFO_1:
                      {
                          Read_Holding_Regs_Query_Deepsea(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenAbsSlaveID,
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenBaseAddr[0],
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenNumOfReg[0]);
                      }
                      break;
                      case GEN_DEEPSEA_INFO_2:
                      {
                          Read_Holding_Regs_Query_Deepsea(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenAbsSlaveID,
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenBaseAddr[1],
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenNumOfReg[1]);
                      }
                      break;
                      case GEN_DEEPSEA_INFO_3:
                      {
                          Read_Holding_Regs_Query_Deepsea(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenAbsSlaveID,
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenBaseAddr[2],
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenNumOfReg[2]);
                      }
                      break;
                      case GEN_DEEPSEA_INFO_4:
                      {
                          Read_Holding_Regs_Query_Deepsea(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenAbsSlaveID,
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenBaseAddr[3],
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenNumOfReg[3]);
                      }
                      break;
                      default:
                      break;
                      };
                  }
                  break;
                  case 4: // LR2057
                  {
                    Init_RS485_MODBUS_UART();
                      switch(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep)
                      {
                      case GEN_LR2057_INFO_1:
                      {
                          Read_Holding_Regs_Query(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenAbsSlaveID,
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenBaseAddr[0],
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenNumOfReg[0]);
                      }
                      break;
                      case GEN_LR2057_INFO_2:
                      {
                          Read_Holding_Regs_Query(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenAbsSlaveID,
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenBaseAddr[1],
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenNumOfReg[1]);
                      }
                      break;
                      case GEN_LR2057_INFO_3:
                      {
                          Read_Coil_Status_Query(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenAbsSlaveID,
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenBaseAddr[2],
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenNumOfReg[2]);
                      }
                      break;
                      default:
                      break;
                      }
                  }
                  break;
                  case 5:// HIMOINSA
                  {
                    Init_RS485_MODBUS_UART();
                      switch(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep)
                      {
                      case GEN_HIMOINSA_INFO_1:
                      {
                          Read_Input_Status_Query_fast(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenAbsSlaveID,
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenBaseAddr[0],
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenNumOfReg[0]);
                      }
                      break;
                      case GEN_HIMOINSA_INFO_2:
                      {
                          Read_Input_Regs_Query_fast(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenAbsSlaveID,
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenBaseAddr[1],
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenNumOfReg[1]);
                      }
                      break;
                      default:
                      break;
                      };
                  }
                  break;
                  case 6:// QC315
                  {
                    Init_RS485_MODBUS_UART();
                      switch(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep)
                      {
                      case GEN_QC315_INFO_1:
                      {
                          Read_Input_Regs_Query_fast(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenAbsSlaveID,
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenBaseAddr[0],
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenNumOfReg[0]);
                      }
                      break;
                      case GEN_QC315_INFO_2:
                      {
                          Read_Input_Regs_Query_fast(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenAbsSlaveID,
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenBaseAddr[1],
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenNumOfReg[1]);
                      }
                      break;
                      case GEN_QC315_INFO_3:
                      {
                          Read_Input_Regs_Query_fast(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenAbsSlaveID,
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenBaseAddr[2],
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenNumOfReg[2]);
                      }
                      break;
                      default:
                      break;
                      };
                  }
                  break;
                  case 7:// CUMMIN
                  {
                    Init_RS485_MODBUS_UART();
                      switch(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep)
                      {
                      case GEN_CUMMIN_INFO_1:
                      {
                          Read_Holding_Regs_Query_Deepsea(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenAbsSlaveID,
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenBaseAddr[0],
                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenNumOfReg[0]);
                      }
                      break;
//                      case GEN_CUMMIN_INFO_2:
//                      {
//                          Read_Input_Regs_Query_fast(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenAbsSlaveID,
//                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenBaseAddr[1],
//                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenNumOfReg[1]);
//                      }
//                      break;
//                      case GEN_CUMMIN_INFO_3:
//                      {
//                          Read_Input_Regs_Query_fast(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenAbsSlaveID,
//                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u32GenBaseAddr[2],
//                                                sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenNumOfReg[2]);
//                      }
//                      break;
                      default:
                      break;
                      };
                  }
                  break;
                  default:
                      break;
                  }
                  Modbus.runningStep = _WAIT_GEN_RESPOND;
              }
              else
              {
  #if (USERDEF_MONITOR_PM == ENABLED)
                    Modbus.runningStep = _READ_PM_STATUS;
  #endif
  #if (USERDEF_MONITOR_PM == DISABLED)
                    Modbus.runningStep = _READ_LIB_STATUS;
  #endif  
              }
          }
          break;
          case _WAIT_GEN_RESPOND:
          {
              reVal = RS4851_Check_Respond_Data_GEN();
              if(sModbusManager.u8GenCurrentIndex <= (sModbusManager.u8NumberOfGen - 1))
              {
                  if(reVal != 1)
                  {
                      sGenInfo[sModbusManager.u8GenCurrentIndex].u8NoResponseCnt++;
                      if (sGenInfo[sModbusManager.u8GenCurrentIndex].u8NoResponseCnt >= 5)
                      {
                          sGenInfo[sModbusManager.u8GenCurrentIndex].u8NoResponseCnt = 2;
                          sGenInfo[sModbusManager.u8GenCurrentIndex].u8ErrorFlag = 1;
                          cntDisConnectGEN[sModbusManager.u8GenCurrentIndex]++;
                          switch(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenType)
                          {
                          case 1://KUBOTA
                          {
                            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_KUBOTA_INFO_1;
                          }
                          break;
                          case 2://BE142
                          {
                            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_BE142_INFO_1;
                          }
                          break;
                          case 3://DEEPSEA
                          {
                            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_DEEPSEA_INFO_1;
                          }
                          break;
                          case 4: //LR2057
                          {
                            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_LR2057_INFO_1;
                          }
                          break;
                          case 5: //HIMOINSA
                          {
                            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_HIMOINSA_INFO_1;
                          }
                          break;
                          case 6: //QC315
                          {
                            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_QC315_INFO_1;
                          }
                          break;
                          case 7: //CUMMIN
                          {
                            sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep = GEN_CUMMIN_INFO_1;
                          }
                          break;
                          };  
                          sModbusManager.u8GenCurrentIndex++;
                      }
                  }
                  else
                  {
                      sGenInfo[sModbusManager.u8GenCurrentIndex].u8NoResponseCnt = 0;
                      sGenInfo[sModbusManager.u8GenCurrentIndex].u8ErrorFlag = 0;
                      switch(sModbusManager.sGenManager[sModbusManager.u8GenCurrentIndex].u8GenRunningStep)
                      {
                      case GEN_KUBOTA_INFO_1:
                      case GEN_DEEPSEA_INFO_1:
                      case GEN_BE142_INFO_1:
                      case GEN_LR2057_INFO_1:
                      case GEN_HIMOINSA_INFO_1:
                      case GEN_QC315_INFO_1:
                      case GEN_CUMMIN_INFO_1:
                      {
                        sModbusManager.u8GenCurrentIndex++;
                      }break;                        
                      };
                  }

                  if (sModbusManager.u8GenCurrentIndex > sModbusManager.u8NumberOfGen - 1)
                  {
  #if (USERDEF_MONITOR_PM == ENABLED)
                    Modbus.runningStep = _READ_PM_STATUS;
  #endif
  #if (USERDEF_MONITOR_PM == DISABLED)
                    Modbus.runningStep = _WAIT_MODE;
  #endif               
                      
                  }
                  else
                  {
                      Modbus.runningStep = _READ_GEN_STATUS;
                  }
              }
              else Modbus.runningStep = _READ_GEN_STATUS;
          }
          break;
  #endif
  #if (USERDEF_MONITOR_PM == ENABLED)
            case _READ_PM_STATUS:
            {              
                Modbus.u8DataPointer = 0;
                if(sModbusManager.u8NumberOfPM != 0)           
                {
//                    ReInit_RS485_MODBUS_UART();
                    if (sModbusManager.u8PMCurrentIndex > sModbusManager.u8NumberOfPM - 1)     
                    {
                        sModbusManager.u8PMCurrentIndex = 0;
                    }
                    vTaskDelay(500);
                    switch(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMType) 
                    {
                    //vTaskDelay(500); // sua loi mat ket noi
                    case 1:// FINECO
                    {   
                        ReInit_RS485_MODBUS_UART();
                        switch(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep)      
                        {
                        case PM_FINECO_INFO_1:
                        {
                            Read_Input_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID,
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[0],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[0]);//1,0x10,50
                        }
                        break;
                        case PM_FINECO_INFO_2:
                        {
                            Read_Input_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID,
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[1],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[1]);//1,0x20,5
                        }
                        break;
                        default:
                        break;
                        };
                    }
                    break;
                    case 2:// ASCENT
                    {   
                        ReInit_RS485_MODBUS_UART();
                        switch(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep)
                        {
                        case PM_ASCENT_INFO_1:
                        {
                            Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID,
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[0],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[0]);//1,0x10,50
                        }
                        break;
                        case PM_ASCENT_INFO_2:
                        {
                            Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID,
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[1],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[1]);//1,0x20,5
                        }
                        break;
                        case PM_ASCENT_INFO_3:
                        {
                            Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID,
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[2],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[2]);//1,0x20,5
                        }
                        break;
                        case PM_ASCENT_INFO_4:
                        {
                            Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID,
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[3],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[3]);//1,0x20,5
                        }
                        break;
                        case PM_ASCENT_INFO_5:
                        {
                            Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID,
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[4],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[4]);//1,0x20,5
                        }
                        break;
                        case PM_ASCENT_INFO_6:
                        {
                            Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID,
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[5],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[5]);//1,0x20,5
                        }
                        break;
                        default:
                        break;
                        };
                    }
                    break;
                    case 3:// EASTRON
                    {
                      UART_Deinit(RS4851R_UART);
                      Init_RS485_MODBUS_UART();
                        switch(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep)
                        {
                        case PM_EASTRON_INFO_1:
                        {
                            Read_Input_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID,
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[0],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[0]);
                        }
                        break;
                        case PM_EASTRON_INFO_2:
                        {
                            Read_Input_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID,
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[1],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[1]);
                        }
                        break;
                        case PM_EASTRON_INFO_3:
                        {
                            Read_Input_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID,
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[2],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[2]);
                        }
                        break;
                        case PM_EASTRON_INFO_4:
                        {
                            Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID,
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[3],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[3]);
                        }
                        break;
                        default:
                        break;
                        };
                    }
                    break;
                    case 4:// CET1
                    {
                        ReInit_RS485_MODBUS_UART();
                        switch(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep)
                        {
                        case PM_CET1_INFO_1:
                        {
                            Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID,
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[0],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[0]);//1,0x10,50
                        }
                        break;
                        case PM_CET1_INFO_2:
                        {
                            Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID,
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[1],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[1]);//1,0x20,5
                        }
                        break;
                        case PM_CET1_INFO_3:
                        {
                            Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID,
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[2],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[2]);//1,0x20,5
                        }
                        break;
                        default:
                        break;
                        };
                    }
                    break;
                    case 5://PILOT
                    {   
                      UART_Deinit(RS4851R_UART);
                      Init_RS485_MODBUS_UART();
                        switch(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep)
                        {
                        case PM_PILOT_INFO_1:
                        {
                            Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[0],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[0]);
                        }                   
                        break;
                        case PM_PILOT_INFO_2:
                        {
                            Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[1],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[1]);
                        }
                        break;
                        default:
                        break;
                        }
                    }
                    break;
                    case 15://PILOT_3PHASE
                    {   
                      UART_Deinit(RS4851R_UART);
                      Init_RS485_MODBUS_UART();
                        switch(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep)
                        {
                        case PM_PILOT_3PHASE_INFO_1:
                        {
                            Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[0],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[0]);
                        }                   
                        break;
                        case PM_PILOT_3PHASE_INFO_2:
                        {
                            Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[1],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[1]);
                        }
                        break;
                        case PM_PILOT_3PHASE_INFO_3:
                        {
                            Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[2],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[2]);
                        }
                        break;
                        default:
                        break;
                        }
                    }
                    break;
                    case 16://YADA_3PHASE_DPC
                    {
                      ReInit_RS485_MODBUS_UART_yada_dpc();
                      switch(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep){
                        case PM_YADA_3PHASE_DPC_INFO_1:
                          Read_Holding_Regs_Query_yada(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[0],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[0]);
                          break;
                        case PM_YADA_3PHASE_DPC_INFO_2:
                          Read_Holding_Regs_Query_yada(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[1],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[1]);
                          break;
                        case PM_YADA_3PHASE_DPC_INFO_3:
                          Read_Holding_Regs_Query_yada(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[2],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[2]);
                          break;
                        case PM_YADA_3PHASE_DPC_INFO_4:
                          Read_Holding_Regs_Query_yada(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[3],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[3]);
                          break;
                        case PM_YADA_3PHASE_DPC_INFO_5:
                          Read_Holding_Regs_Query_yada(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[4],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[4]);
                          break;
                        case PM_YADA_3PHASE_DPC_INFO_6:
                          Read_Holding_Regs_Query_yada(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[5],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[5]);
                          break;
                        case PM_YADA_3PHASE_DPC_INFO_7:
                          Read_Holding_Regs_Query_yada(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[6],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[6]);
                          break;
                        case PM_YADA_3PHASE_DPC_INFO_8:
                          Read_Holding_Regs_Query_yada(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[7],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[7]);
                          break;
                      default:
                        break;
                      }  
                    }
                    break;
                    case 17: //Schneider 2022
                     { 
                       ReInit_RS485_MODBUS_UART_2022 (14400,kUART_ParityEven);
                      switch(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep){
                      case PM_SCHNEDER_3PHASE_INFO_1:
                        Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[0],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[0]);
                        break;
                      case PM_SCHNEDER_3PHASE_INFO_2:
                        Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[1],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[1]);                
                        break; 
                      case PM_SCHNEDER_3PHASE_INFO_3:
                        Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[2],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[2]);                        
                        break;
                      case PM_SCHNEDER_3PHASE_INFO_4:
                         Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[3],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[3]);                
                        break;
                      case PM_SCHNEDER_3PHASE_INFO_5:
                         Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[4],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[4]);                
                        break;
                      case PM_SCHNEDER_3PHASE_INFO_6:
                        Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[5],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[5]);                
                        break; 
                      case PM_SCHNEDER_3PHASE_INFO_7:
                         Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[6],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[6]);                
                        break;
                      case PM_SCHNEDER_3PHASE_INFO_8:
                        Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[7],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[7]);                
                        break;                        
                      case PM_SCHNEDER_3PHASE_INFO_9:
                        Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[8],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[8]);                
                        break;                         
                      default:
                        break;  
                      }
                     }
                     break;
                    case 18: //EASTRON SMD72D 2022
                     { 
                      UART_Deinit(RS4851R_UART);
                      Init_RS485_MODBUS_UART();
                      switch(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep){
                      case PM_EASTRON_SDM72D_INFO_1:
                        Read_Input_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[0],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[0]);
                        break;
                      case PM_EASTRON_SDM72D_INFO_2:
                        Read_Input_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[1],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[1]);                
                        break;  
                      case PM_EASTRON_SDM72D_INFO_3:
                        Read_Input_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[2],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[2]);                
                        break;
                      case PM_EASTRON_SDM72D_INFO_4:
                        Read_Input_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[3],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[3]);                
                        break; 
                      case PM_EASTRON_SDM72D_INFO_5:
                        Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[4],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[4]);                
                        break;  
                      case PM_EASTRON_SDM72D_INFO_6:
                        Read_Holding_Regs_Query(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMAbsSlaveID, 
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u32PMBaseAddr[5],
                                                  sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMNumOfReg[5]);                
                        break;                          
                      default:
                        break;  
                      }
                     }                     
                     break;                     
                    default:
                     break;
                    }
                    Modbus.runningStep = _WAIT_PM_RESPOND;
                }
                else
                {
                    Modbus.runningStep = _READ_VAC_STATUS;
                    //Init_RS485_MODBUS_UART();
                }
            }
            break;
            case _WAIT_PM_RESPOND:
            {
                reVal = RS4851_Check_Respond_Data_PM();
                return1= reVal;
                if(sModbusManager.u8PMCurrentIndex <= (sModbusManager.u8NumberOfPM - 1))        //
                {
                    if(reVal != 1)
                    {
                      
                        sPMInfo[sModbusManager.u8PMCurrentIndex].u8NoResponseCnt++;
                        if (sPMInfo[sModbusManager.u8PMCurrentIndex].u8NoResponseCnt >= 5)
                        {
                            sPMInfo[sModbusManager.u8PMCurrentIndex].u8ErrorFlag = 1;
                            cntDisConnectPM[sModbusManager.u8PMCurrentIndex]++;
                            switch(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMType)
                            {
                            case 1://FINECO
                            {
                              sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_FINECO_INFO_1;
                            }
                            break;
                            case 2://ASCENT
                            {
                              sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_ASCENT_INFO_1;
                            }
                            break;
                            case 3://EASTRON
                            {
                              sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_EASTRON_INFO_1;
                            }
                            break;
                            case 4://CET1
                            {
                              sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_CET1_INFO_1;
                            }
                            break;
                            case 5://PILOT
                            {
                              sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_PILOT_INFO_1;
                            }
                            break;
                            case 15://PILOT_3PHASE
                            {
                              sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_PILOT_3PHASE_INFO_1;
                            }
                            break;
                            case 16: //YADA_3PHASE_DPC
                            {
                              sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_YADA_3PHASE_DPC_INFO_1;
                            }
                            break;
                            case 17: //Schneider 2022
                            {
                              sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_SCHNEDER_3PHASE_INFO_1;
                            }
                            break; 
                            case 18: //EASTRON SMD72D 2022
                            {
                              sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep = PM_EASTRON_SDM72D_INFO_1;
                            }
                            break;
                            };
                            sModbusManager.u8PMCurrentIndex++;
                        }
                    }
                    else
                    {
                        sPMInfo[sModbusManager.u8PMCurrentIndex].u8NoResponseCnt = 0;
                        sPMInfo[sModbusManager.u8PMCurrentIndex].u8ErrorFlag = 0; 
                        switch(sModbusManager.sPMManager[sModbusManager.u8PMCurrentIndex].u8PMRunningStep)
                        {
                        case PM_FINECO_INFO_1:
                        case PM_ASCENT_INFO_1:
                        case PM_EASTRON_INFO_1:
                        case PM_CET1_INFO_1:
                        case PM_PILOT_INFO_1:
//                        case PM_FORLONG_INFO_1:
                        case PM_YADA_3PHASE_DPC_INFO_1:
                        case PM_EASTRON_SDM72D_INFO_1:  
                        case PM_PILOT_3PHASE_INFO_1:
                        {
                          sModbusManager.u8PMCurrentIndex++;
                        }break;                      
                        };                        
                    }

                    if (sModbusManager.u8PMCurrentIndex > sModbusManager.u8NumberOfPM - 1)
                    {
                        Modbus.runningStep = _READ_VAC_STATUS;
//                        Init_RS485_MODBUS_UART();
                        sModbusManager.u8PMBootPassFlag = 1;
                    }
                    else
                    {
                        Modbus.runningStep = _READ_PM_STATUS;   
                    }
                }
                else Modbus.runningStep = _READ_PM_STATUS;
            }
            break;
  #endif
  #if (USERDEF_MONITOR_VAC == ENABLED)
            case _READ_VAC_STATUS:
            {
                Modbus.u8MosbusEn =0;
                Modbus.u8DataPointer = 0;
                //ReInit2_RS485_MODBUS_UART (14400);
                if(sModbusManager.u8NumberOfVAC != 0)
                {
                    if (sModbusManager.u8VACCurrentIndex > sModbusManager.u8NumberOfVAC - 1)
                    {
                        sModbusManager.u8VACCurrentIndex = 0;
                    }
                    switch(sModbusManager.sVACManager[sModbusManager.u8VACCurrentIndex].u8VACType)
                    {
                    case 1:// VIETTEL
                    {
                      UART_Deinit(RS4851R_UART);
                      vTaskDelay(1);
                      Init_RS485_MODBUS_UART();
                      vTaskDelay(1);
                      //ReInit2_RS485_MODBUS_UART (14400);
                        switch(sModbusManager.sVACManager[sModbusManager.u8VACCurrentIndex].u8VACRunningStep)
                        {
                        case VAC_VIETTEL_INFO_1:
                        {
                            Read_Input_Regs_Query(sModbusManager.sVACManager[sModbusManager.u8VACCurrentIndex].u32VACAbsSlaveID,
                                                  sModbusManager.sVACManager[sModbusManager.u8VACCurrentIndex].u32VACBaseAddr[0],
                                                  sModbusManager.sVACManager[sModbusManager.u8VACCurrentIndex].u8VACNumOfReg[0]);//35,0x00,20                           
                        }
                        break;
                        case VAC_VIETTEL_INFO_2:
                        {
                            Read_Holding_Regs_Query(sModbusManager.sVACManager[sModbusManager.u8VACCurrentIndex].u32VACAbsSlaveID,
                                                  sModbusManager.sVACManager[sModbusManager.u8VACCurrentIndex].u32VACBaseAddr[1],
                                                  sModbusManager.sVACManager[sModbusManager.u8VACCurrentIndex].u8VACNumOfReg[1]);//35,0x20,80
                        }
                        break;
                        case VAC_VIETTEL_INFO_3:
                        {
                            Read_Holding_Regs_Query(sModbusManager.sVACManager[sModbusManager.u8VACCurrentIndex].u32VACAbsSlaveID,
                                                  sModbusManager.sVACManager[sModbusManager.u8VACCurrentIndex].u32VACBaseAddr[2],
                                                  sModbusManager.sVACManager[sModbusManager.u8VACCurrentIndex].u8VACNumOfReg[2]);//35,0x20,80
                        }
                        break;
                        default:
                            break;
                        };
                    }
                    break;
                    default:
                        break;
                    }
                    Modbus.runningStep = _WAIT_VAC_RESPOND;
                }
                else
                {
                    Modbus.runningStep = _READ_SMCB_STATUS;
                }
            }
            break;
            case _WAIT_VAC_RESPOND:
            {
                reVal = RS4851_Check_Respond_Data_VAC();
                if(sModbusManager.u8VACCurrentIndex <= (sModbusManager.u8NumberOfVAC - 1))
                {
                    if(reVal != 1)
                    {
                        sVACInfo[sModbusManager.u8VACCurrentIndex].u8NoResponseCnt++;
                        if (sVACInfo[sModbusManager.u8VACCurrentIndex].u8NoResponseCnt >= 5)
                        {
                            sVACInfo[sModbusManager.u8VACCurrentIndex].u8ErrorFlag = 1;
                            cntDisConnectVAC[sModbusManager.u8VACCurrentIndex]++;
                            switch(sModbusManager.sVACManager[sModbusManager.u8VACCurrentIndex].u8VACType)
                            {
                            case 1:
                            {
                              sModbusManager.sVACManager[sModbusManager.u8VACCurrentIndex].u8VACRunningStep = VAC_VIETTEL_INFO_1;
                            }
                            break;
                            };                        
                            sModbusManager.u8VACCurrentIndex++;
                        }
                    }
                    else
                    {
                        sVACInfo[sModbusManager.u8VACCurrentIndex].u8NoResponseCnt = 0;
                        sVACInfo[sModbusManager.u8VACCurrentIndex].u8ErrorFlag = 0;
                        switch(sModbusManager.sVACManager[sModbusManager.u8VACCurrentIndex].u8VACRunningStep)
                        {
                        case VAC_VIETTEL_INFO_1:
                        {
                          sModbusManager.u8VACCurrentIndex++;
                        }break;                      
                        };
                    }

                    if (sModbusManager.u8VACCurrentIndex > sModbusManager.u8NumberOfVAC - 1)
                    {
                        Modbus.runningStep = _READ_SMCB_STATUS;
                    }
                    else
                    {
                        Modbus.runningStep = _READ_VAC_STATUS;
                    }
                }
                else Modbus.runningStep = _READ_VAC_STATUS;
            }
            break;
    #endif
  #if (USERDEF_MONITOR_SMCB == ENABLED) //smcb
            case _READ_SMCB_STATUS:
            {
                Modbus.u8DataPointer = 0;
                if(sModbusManager.u8NumberOfSMCB != 0)
                {
                    if (sModbusManager.u8SMCBCurrentIndex > sModbusManager.u8NumberOfSMCB - 1)
                    {
                        sModbusManager.u8SMCBCurrentIndex = 0;
                    }
                    switch(sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u8SMCBType)
                    {
                    case 1:// OPEN
                    {
                      vTaskDelay(1000);
                      Init_RS485_MODBUS_UART();
                        switch(sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u8SMCBRunningStep)
                        {
                        case SMCB_OPEN_INFO_1:
                        {
                            Read_Holding_Regs_Query(sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u32SMCBAbsSlaveID,
                                                  sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u32SMCBBaseAddr[0],
                                                  sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u8SMCBNumOfReg[0]);//35,0x00,20
                        }
                        break;
                        default:
                            break;
                        };
                    }
                    break;
                    case 2:// MATIS
                    {
                      vTaskDelay(1000);
                      Init_RS485_MODBUS_UART();
                        switch(sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u8SMCBRunningStep)
                        {
                        case SMCB_MATIS_INFO_1:
                        {
                            Read_Holding_Regs_Query(sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u32SMCBAbsSlaveID,
                                                  sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u32SMCBBaseAddr[0],
                                                  sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u8SMCBNumOfReg[0]);//35,0x00,20
                        }
                        break;
                        default:
                            break;
                        };
                    }
                    break;
                    case 3:// GOL
                    {
                      vTaskDelay(1000);
                      Init_RS485_MODBUS_UART();
                        switch(sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u8SMCBRunningStep)
                        {
                        case SMCB_GOL_INFO_1:
                        {
                            Read_Holding_Regs_Query(sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u32SMCBAbsSlaveID,
                                                  sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u32SMCBBaseAddr[0],
                                                  sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u8SMCBNumOfReg[0]);
                        }
                        break;
                        default:
                            break;
                        };
                    }
                    break;
                    default:
                        break;
                    }
                    Modbus.runningStep = _WAIT_SMCB_RESPOND;
                }
                else
                {
                    Modbus.runningStep = _READ_FUEL_STATUS;
                }
            }
            break;
            case _WAIT_SMCB_RESPOND:
            {
                reVal = RS4851_Check_Respond_Data_SMCB();
                if(sModbusManager.u8SMCBCurrentIndex <= (sModbusManager.u8NumberOfSMCB - 1))
                {
                    if(reVal != 1)
                    {
                        sSMCBInfo[sModbusManager.u8SMCBCurrentIndex].u8NoResponseCnt++;
                        if (sSMCBInfo[sModbusManager.u8SMCBCurrentIndex].u8NoResponseCnt >= 5)
                        {
                            sSMCBInfo[sModbusManager.u8SMCBCurrentIndex].u8ErrorFlag = 1;
                            cntDisConnectSMCB[sModbusManager.u8SMCBCurrentIndex]++;
                            switch(sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u8SMCBType)
                            {
                            case 1:
                            {
                              sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u8SMCBRunningStep = SMCB_OPEN_INFO_1;
                            }
                            break;
                            case 2:
                            {
                              sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u8SMCBRunningStep = SMCB_MATIS_INFO_1;
                            }
                            break;
                            case 3:
                            {
                              sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u8SMCBRunningStep = SMCB_GOL_INFO_1;
                            }
                            break;
                            };                        
                            sModbusManager.u8SMCBCurrentIndex++;
                        }
                    }
                    else
                    {
                        sSMCBInfo[sModbusManager.u8SMCBCurrentIndex].u8NoResponseCnt = 0;
                        sSMCBInfo[sModbusManager.u8SMCBCurrentIndex].u8ErrorFlag = 0;
                        switch(sModbusManager.sSMCBManager[sModbusManager.u8SMCBCurrentIndex].u8SMCBRunningStep)
                        {
                        case SMCB_OPEN_INFO_1:
                        case SMCB_MATIS_INFO_1:
                        case SMCB_GOL_INFO_1:
                        {
                          sModbusManager.u8SMCBCurrentIndex++;
                        }break;                      
                        };
                    }

                    if (sModbusManager.u8SMCBCurrentIndex > sModbusManager.u8NumberOfSMCB - 1)
                    {
                        Modbus.runningStep = _READ_FUEL_STATUS;
                    }
                    else
                    {
                        Modbus.runningStep = _READ_SMCB_STATUS;
                    }
                }
                else Modbus.runningStep = _READ_SMCB_STATUS;
            }
            break;      
  #endif
  #if (USERDEF_MONITOR_FUEL == ENABLED) //fuel
            case _READ_FUEL_STATUS:
            {
                Modbus.u8DataPointer = 0;
                if(sModbusManager.u8NumberOfFUEL != 0)
                {
                    if (sModbusManager.u8FUELCurrentIndex > sModbusManager.u8NumberOfFUEL - 1)
                    {
                        sModbusManager.u8FUELCurrentIndex = 0;
                    }
                    switch(sModbusManager.sFUELManager[sModbusManager.u8FUELCurrentIndex].u8FUELType)
                    {
                    case 1:// HPT621
                    {
                      Init_RS485_MODBUS_UART();
                        switch(sModbusManager.sFUELManager[sModbusManager.u8FUELCurrentIndex].u8FUELRunningStep)
                        {
                        case FUEL_HPT621_INFO_1:
                        {
                            Read_Holding_Regs_Query(sModbusManager.sFUELManager[sModbusManager.u8FUELCurrentIndex].u32FUELAbsSlaveID,
                                                  sModbusManager.sFUELManager[sModbusManager.u8FUELCurrentIndex].u32FUELBaseAddr[0],
                                                  sModbusManager.sFUELManager[sModbusManager.u8FUELCurrentIndex].u8FUELNumOfReg[0]);
                        }
                        break;
                        default:
                            break;
                        };
                    }
                    break;
                    default:
                        break;
                    }
                    Modbus.runningStep = _WAIT_FUEL_RESPOND;
                }
                else
                {
                    Modbus.runningStep = _READ_ISENSE_STATUS;
                }
            }
            break;
            case _WAIT_FUEL_RESPOND:
            {
                reVal = RS4851_Check_Respond_Data_FUEL();
                if(sModbusManager.u8FUELCurrentIndex <= (sModbusManager.u8NumberOfFUEL - 1))
                {
                    if(reVal != 1)
                    {
                        sFUELInfo[sModbusManager.u8FUELCurrentIndex].u8NoResponseCnt++;
                        if (sFUELInfo[sModbusManager.u8FUELCurrentIndex].u8NoResponseCnt >= 5)
                        {
                            sFUELInfo[sModbusManager.u8FUELCurrentIndex].u8ErrorFlag = 1;
                            cntDisConnectFUEL[sModbusManager.u8FUELCurrentIndex]++;
                            switch(sModbusManager.sFUELManager[sModbusManager.u8FUELCurrentIndex].u8FUELType)
                            {
                            case 1:
                            {
                              sModbusManager.sFUELManager[sModbusManager.u8FUELCurrentIndex].u8FUELRunningStep = FUEL_HPT621_INFO_1;
                            }
                            break;
                            };                        
                            sModbusManager.u8FUELCurrentIndex++;
                        }
                    }
                    else
                    {
                        sFUELInfo[sModbusManager.u8FUELCurrentIndex].u8NoResponseCnt = 0;
                        sFUELInfo[sModbusManager.u8FUELCurrentIndex].u8ErrorFlag = 0;
                        switch(sModbusManager.sFUELManager[sModbusManager.u8FUELCurrentIndex].u8FUELRunningStep)
                        {
                        case FUEL_HPT621_INFO_1:
                        {
                          sModbusManager.u8FUELCurrentIndex++;
                        }break;                      
                        };
                    }

                    if (sModbusManager.u8FUELCurrentIndex > sModbusManager.u8NumberOfFUEL - 1)
                    {
                        Modbus.runningStep = _READ_ISENSE_STATUS;
                    }
                    else
                    {
                        Modbus.runningStep = _READ_FUEL_STATUS;
                    }
                }
                else Modbus.runningStep = _READ_FUEL_STATUS;
            }
            break;      
  #endif
  #if (USERDEF_MONITOR_ISENSE == ENABLED)
            case _READ_ISENSE_STATUS:
            {              
                Modbus.u8DataPointer = 0;
                if(sModbusManager.u8NumberOfISENSE != 0)           
                {
                    if (sModbusManager.u8ISENSECurrentIndex > sModbusManager.u8NumberOfISENSE - 1)     
                    {
                        sModbusManager.u8ISENSECurrentIndex = 0;
                    }
                    vTaskDelay(500);
                    switch(sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSEType) 
                    {
                    //vTaskDelay(500); 
                    case 1://FORLONG
                    {   
                      UART_Deinit(RS4851R_UART);
                      Init_RS485_MODBUS_UART();
                        switch(sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSERunningStep)
                        {
                        case ISENSE_FORLONG_INFO_1:
                        {
                            Read_Input_Regs_Query(sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u32ISENSEAbsSlaveID, 
                                                  sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u32ISENSEBaseAddr[0],
                                                  sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSENumOfReg[0]);
                        }                   
                        break;
                        default:
                        break;
                        }
                    }
                    break;
                    //An 2022
                    case 2: // IVY_DDS353H_2  
                      {
                        ReInit_RS485_MODBUS_UART_2022 (14400, kUART_ParityDisabled);
                        switch(sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSERunningStep)
                        { 
                        case ISENSE_IVY_INFO_1:
                          Read_Input_Regs_Query(sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u32ISENSEAbsSlaveID, 
                                                  sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u32ISENSEBaseAddr[0],
                                                  sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSENumOfReg[0]);
                          break; 
                        case ISENSE_IVY_INFO_2:
                          Read_Input_Regs_Query(sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u32ISENSEAbsSlaveID, 
                                                  sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u32ISENSEBaseAddr[1],
                                                  sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSENumOfReg[1]);
                          break;
                        case ISENSE_IVY_INFO_3:
                          Read_Input_Regs_Query(sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u32ISENSEAbsSlaveID, 
                                                  sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u32ISENSEBaseAddr[2],
                                                  sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSENumOfReg[2]);
                          break;
//                        case ISENSE_IVY_INFO_4:
//                          Read_Input_Regs_Query(sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u32ISENSEAbsSlaveID, 
//                                                  sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u32ISENSEBaseAddr[3],
//                                                  sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSENumOfReg[3]);
//                          break;
//                        case ISENSE_IVY_INFO_5:
//                          Read_Input_Regs_Query(sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u32ISENSEAbsSlaveID, 
//                                                  sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u32ISENSEBaseAddr[4],
//                                                  sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSENumOfReg[4]);
//                          break;
//                        case ISENSE_IVY_INFO_6:
//                          Read_Input_Regs_Query(sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u32ISENSEAbsSlaveID, 
//                                                  sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u32ISENSEBaseAddr[5],
//                                                  sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSENumOfReg[5]);
//                          break;
//                        case ISENSE_IVY_INFO_7:
//                          Read_Input_Regs_Query(sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u32ISENSEAbsSlaveID, 
//                                                  sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u32ISENSEBaseAddr[6],
//                                                  sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSENumOfReg[6]);
//                          break;
//                        case ISENSE_IVY_INFO_8:
//                          Read_Holding_Regs_Query(sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u32ISENSEAbsSlaveID, 
//                                                  sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u32ISENSEBaseAddr[7],
//                                                  sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSENumOfReg[7]);
//                          break;  
//                        case ISENSE_IVY_INFO_9:
//                          Read_Input_Regs_Query(sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u32ISENSEAbsSlaveID, 
//                                                  sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u32ISENSEBaseAddr[8],
//                                                  sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSENumOfReg[8]);
//                          break;  
                        default:
                          break;                         
                        }  
                      }
                      break;
                      
                    default:
                      break;
                    }
                
                
                    Modbus.runningStep = _WAIT_ISENSE_RESPOND;
                }
                else
                {
                    Modbus.runningStep = _READ_PM_DC_STATUS;
                }
            }
            break;
            case _WAIT_ISENSE_RESPOND:
            {
                reVal = RS4851_Check_Respond_Data_ISENSE();
                if(sModbusManager.u8ISENSECurrentIndex <= (sModbusManager.u8NumberOfISENSE - 1))        //
                {
                    if(reVal != 1)
                    {
                      
                        sISENSEInfo[sModbusManager.u8ISENSECurrentIndex].u8NoResponseCnt++;
                        if (sISENSEInfo[sModbusManager.u8ISENSECurrentIndex].u8NoResponseCnt >= 5)
                        {
                            sISENSEInfo[sModbusManager.u8ISENSECurrentIndex].u8ErrorFlag = 1;
                            cntDisConnectISENSE[sModbusManager.u8ISENSECurrentIndex]++;
                            switch(sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSEType)
                            {
                            case 1://FORLONG
                              {
                                sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSERunningStep = ISENSE_FORLONG_INFO_1;
                              }
                              break;
                            case 2: // IVY_DDS353H_2
                              {
                                sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSERunningStep = ISENSE_IVY_INFO_1;
                              }
                              break;
                            }
                            sModbusManager.u8ISENSECurrentIndex++;
                        }
                    }
                    else
                    {
                        sISENSEInfo[sModbusManager.u8ISENSECurrentIndex].u8NoResponseCnt = 0;
                        sISENSEInfo[sModbusManager.u8ISENSECurrentIndex].u8ErrorFlag = 0; 
                        switch(sModbusManager.sISENSEManager[sModbusManager.u8ISENSECurrentIndex].u8ISENSERunningStep)
                        {
                        case ISENSE_FORLONG_INFO_1:
                        case ISENSE_IVY_INFO_1:
                        {
                          sModbusManager.u8ISENSECurrentIndex++;
                        }break;                      
                        };                        
                    }

                    if (sModbusManager.u8ISENSECurrentIndex > sModbusManager.u8NumberOfISENSE - 1)
                    {
                        Modbus.runningStep = _READ_PM_DC_STATUS;
                    }
                    else
                    {
                        Modbus.runningStep = _READ_ISENSE_STATUS;   
                    }
                }
                else Modbus.runningStep = _READ_ISENSE_STATUS;
            }
            break;
  #endif
  #if (USERDEF_MONITOR_PM_DC == ENABLED)
          case _READ_PM_DC_STATUS:
          {
            Modbus.u8DataPointer = 0;
            if(sModbusManager.u8_number_of_pm_dc != 0){
              if(sModbusManager.u8_pm_dc_current_index > sModbusManager.u8_number_of_pm_dc - 1){
                sModbusManager.u8_pm_dc_current_index = 0;
              }
              //vTaskDelay(500);
              switch(sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u8_pm_dc_type){
              case 1://YADA_DC
                {
                  ReInit_RS485_MODBUS_UART_yada_dpc();
                  switch(sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u8_pm_dc_running_step){
                  case PM_DC_YADA_INFO_1:
                    Read_Holding_Regs_Query_yada(sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u32_pm_dc_abs_slaveID, 
                                            sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u32_pm_dc_base_addr[0],
                                            sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u8_pm_dc_num_of_reg[0]);
                    break;
                  case PM_DC_YADA_INFO_2:
                    Read_Holding_Regs_Query_yada(sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u32_pm_dc_abs_slaveID, 
                                            sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u32_pm_dc_base_addr[1],
                                            sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u8_pm_dc_num_of_reg[1]);
                    break;
                  case PM_DC_YADA_INFO_3:
                    Read_Holding_Regs_Query_yada(sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u32_pm_dc_abs_slaveID, 
                                            sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u32_pm_dc_base_addr[2],
                                            sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u8_pm_dc_num_of_reg[2]);
                    break;
                  case PM_DC_YADA_INFO_4:
                    Read_Holding_Regs_Query_yada(sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u32_pm_dc_abs_slaveID, 
                                            sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u32_pm_dc_base_addr[3],
                                            sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u8_pm_dc_num_of_reg[3]);
                    break;
                  default:
                    break;
                  }
                }
                break;
              default:
                break;
              }
              Modbus.runningStep = _WAIT_PM_DC_RESPOND;
            }else{
              Modbus.runningStep = _READ_BM_STATUS;
            }
          }
          break;
          
          case _WAIT_PM_DC_RESPOND:
          {
            reVal = RS4851_Check_Respond_Data_PM_DC();
            if(sModbusManager.u8_pm_dc_current_index <= (sModbusManager.u8_number_of_pm_dc - 1)){
              if(reVal != 1){
                s_pm_dc_info[sModbusManager.u8_pm_dc_current_index].u8_no_response_cnt++;
                if (s_pm_dc_info[sModbusManager.u8_pm_dc_current_index].u8_no_response_cnt >= 5)
                {
                    s_pm_dc_info[sModbusManager.u8_pm_dc_current_index].u8_error_flag = 1;
                    cnt_disConnect_pm_dc[sModbusManager.u8_pm_dc_current_index]++;
                    switch(sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u8_pm_dc_type)
                    {
                    case 1://YADA_DC
                    {
                       sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u8_pm_dc_running_step = PM_DC_YADA_INFO_1;
                    }
                    break;
                    }
                    sModbusManager.u8_pm_dc_current_index++;
                }
                
              }
              else{
                s_pm_dc_info[sModbusManager.u8_pm_dc_current_index].u8_no_response_cnt = 0;
                s_pm_dc_info[sModbusManager.u8_pm_dc_current_index].u8_error_flag = 0; 
                switch(sModbusManager.s_pm_dc_manager[sModbusManager.u8_pm_dc_current_index].u8_pm_dc_running_step)
                {
                case PM_DC_YADA_INFO_1:
                {
                sModbusManager.u8_pm_dc_current_index++;
                }break;                      
                };
              }
              
              if (sModbusManager.u8_pm_dc_current_index > sModbusManager.u8_number_of_pm_dc - 1)
              {
                   Modbus.runningStep = _READ_BM_STATUS;
              }
              else
              {
                   Modbus.runningStep = _READ_PM_DC_STATUS;   
              } 
            }
            else{
              Modbus.runningStep = _READ_PM_DC_STATUS;
            }
          }
          break;     
  #endif
  #if (USERDEF_MONITOR_BM == ENABLED)
          case _READ_BM_STATUS:
          {
              Modbus.u8DataPointer = 0;
              if(sModbusManager.u8NumberOfBM != 0)
              {
                  if (sModbusManager.u8BMCurrentIndex > sModbusManager.u8NumberOfBM - 1)
                  {
                      sModbusManager.u8BMCurrentIndex = 0;
                  }
                  switch(sModbusManager.sBMManager[sModbusManager.u8BMCurrentIndex].u8BMType)
                  {
                  case 1:// VIETTEL
                  {
                      UART_Deinit(RS4851R_UART);
                      Init_RS485_MODBUS_UART();
                      switch(sModbusManager.sBMManager[sModbusManager.u8BMCurrentIndex].u8BMRunningStep)
                      {
                      case BM_VIETTEL_INFO_1:
                      {
                          Read_Holding_Regs_Query(sModbusManager.sBMManager[sModbusManager.u8BMCurrentIndex].u32BMAbsSlaveID,
                                                sModbusManager.sBMManager[sModbusManager.u8BMCurrentIndex].u32BMBaseAddr[0],
                                                sModbusManager.sBMManager[sModbusManager.u8BMCurrentIndex].u8BMNumOfReg[0]);//1,0x00,28
                      }
                      break;
                      case BM_VIETTEL_INFO_2:
                      {
                          Read_Holding_Regs_Query(sModbusManager.sBMManager[sModbusManager.u8BMCurrentIndex].u32BMAbsSlaveID,
                                                sModbusManager.sBMManager[sModbusManager.u8BMCurrentIndex].u32BMBaseAddr[1],
                                                sModbusManager.sBMManager[sModbusManager.u8BMCurrentIndex].u8BMNumOfReg[1]);//1,0x20,5
                      }
                      break;
                      default:
                          break;
                      };
                  }
                  break;
                  default:
                      break;
                  }
                  Modbus.runningStep = _WAIT_BM_RESPOND;
              }
              else
              {
                  Modbus.runningStep = _READ_FAN_DPC_STATUS; //_WAIT_MODE;//edit
              }
          }
          break;
          case _WAIT_BM_RESPOND:
          {
              reVal = RS4851_Check_Respond_Data_BM();
              if(sModbusManager.u8BMCurrentIndex <= (sModbusManager.u8NumberOfBM - 1))
              {
                  if(reVal != 1)
                  {
                      sBMInfo[sModbusManager.u8BMCurrentIndex].u8NoResponseCnt++;
                      if (sBMInfo[sModbusManager.u8BMCurrentIndex].u8NoResponseCnt >= 5)
                      {
                          sBMInfo[sModbusManager.u8BMCurrentIndex].u8ErrorFlag = 1;
                          switch(sModbusManager.sBMManager[sModbusManager.u8BMCurrentIndex].u8BMType)
                          {
                          case 1:
                          {
                            sModbusManager.sBMManager[sModbusManager.u8BMCurrentIndex].u8BMRunningStep = BM_VIETTEL_INFO_1;
                          }
                          break;
                          }; 
                          sModbusManager.u8BMCurrentIndex++;
                      }
                  }
                  else
                  {
                      sBMInfo[sModbusManager.u8BMCurrentIndex].u8NoResponseCnt = 0;
                      sBMInfo[sModbusManager.u8BMCurrentIndex].u8ErrorFlag = 0;
                      switch(sModbusManager.sBMManager[sModbusManager.u8BMCurrentIndex].u8BMRunningStep)
                      {
                      case BM_VIETTEL_INFO_1:
                      {
                        sModbusManager.u8BMCurrentIndex++;
                      }break;                      
                      };
                  }

                  if (sModbusManager.u8BMCurrentIndex > sModbusManager.u8NumberOfBM - 1)
                  {
                      Modbus.runningStep = _READ_FAN_DPC_STATUS;//_WAIT_MODE;//edit
                  }
                  else
                  {
                      Modbus.runningStep = _READ_BM_STATUS;
                  }
              }else {
                Modbus.runningStep = _READ_BM_STATUS;
              }
          }
          break;
  #endif
  #if (USERDEF_RS485_DKD51_BDP == ENABLED)
          case _READ_FAN_DPC_STATUS:
          {
            Modbus.u8DataPointer = 0;
            if(TYPE == DKD51_BDP){
               UART_Deinit(RS4851R_UART);
               vTaskDelay(1);
               Init_RS485_MODBUS_UART();
              //clearn-------------------------------------------------- 
                DKD51_clean_buff_fan(&fan_dpc_switch_uart_t);
              //clearn--------------------------------------------------
              switch(fan_dpc_switch_uart_t.fan_state_e){
              case _DKD51_FAN_SYS_REQ:
                fan_dpc_switch_uart_t.rx_enable = STATE_ON;
                //code here
                DKD51_send_message_resquest_fan((uint8_t*)&DKD51_FAN_REQ_sys_analog_data[0],(uint8_t)DKD51_FAN_REQ_sys_analog_data_len);
                Modbus.runningStep = _WAIT_FAN_DPC_RESPOND;
                break;
              case _DKD51_FAN_STATE_REQ:
                fan_dpc_switch_uart_t.rx_enable = STATE_ON;
                //code here
                DKD51_send_message_resquest_fan((uint8_t*)&DKD51_FAN_REQ_switch_input_state[0],(uint8_t)DKD51_FAN_REQ_switch_input_state_len);
                Modbus.runningStep = _WAIT_FAN_DPC_RESPOND;
                break;
              case _DKD51_FAN_WARNING_REQ:
                fan_dpc_switch_uart_t.rx_enable = STATE_ON;
                //code here
                DKD51_send_message_resquest_fan((uint8_t*)&DKD51_FAN_REQ_warning_status[0],(uint8_t)DKD51_FAN_REQ_warning_status_len);
                Modbus.runningStep = _WAIT_FAN_DPC_RESPOND;
                break;
              case _DKD51_FAN_PARAMETER_REQ:
                fan_dpc_switch_uart_t.rx_enable = STATE_ON;
                //code here
                DKD51_send_message_resquest_fan((uint8_t*)&DKD51_FAN_REQ_read_parameter[0],(uint8_t)DKD51_FAN_REQ_read_parameter_len);
                Modbus.runningStep = _WAIT_FAN_DPC_RESPOND;
                break;
              case _DKD51_FAN_EQUIPMENT_REQ:
                fan_dpc_switch_uart_t.rx_enable = STATE_ON;
                //code here
                Modbus.runningStep = _WAIT_FAN_DPC_RESPOND;
                break;
              default:
                fan_dpc_switch_uart_t.rx_enable  = STATE_OFF;
                Modbus.runningStep               = _WAIT_MODE;
                break;
              }
              
            }else{
              Modbus.runningStep = _WAIT_MODE;
              privateMibBase.connAlarmGroup.alarm_dpc_fan_connect = 0;
            } 
          }
          break;
          case _WAIT_FAN_DPC_RESPOND:
          {
            Modbus.u8DataPointer = 0;
            if(TYPE == DKD51_BDP){
              switch(fan_dpc_switch_uart_t.fan_state_e){
              case _DKD51_FAN_SYS_REQ:
                //code here------------------------------------------------------------------------
                if(DKD51_check_sum_fan(&fan_dpc_switch_uart_t) == NO_ERR){
                  fan_info_dpc_t.no_response =0;
                  privateMibBase.connAlarmGroup.alarm_dpc_fan_connect = 0;
                  if(DKD51_check_RTN_fan(&fan_dpc_switch_uart_t) == NO_ERR){
                    fan_info_dpc_t.env_temp          = hex2byte2(&fan_dpc_switch_uart_t.rx.buff_485[0],15);  // realx100
                    fan_info_dpc_t.airflow_temp      = hex2byte2(&fan_dpc_switch_uart_t.rx.buff_485[0],19);  // realx100
                    fan_info_dpc_t.voltage           = hex2byte2(&fan_dpc_switch_uart_t.rx.buff_485[0],23);  // realx100
                    fan_info_dpc_t.working_current   = hex2byte2(&fan_dpc_switch_uart_t.rx.buff_485[0],27);  // realx100
                  } 
                }
                else{
                  fan_dpc_switch_uart_t.check_sum_calculator = 0;
                  fan_dpc_switch_uart_t.check_sum_read       = 0xFFFF;
                  fan_info_dpc_t.no_response++;
                  if(fan_info_dpc_t.no_response>3){
                    fan_info_dpc_t.no_response = 10;
                    privateMibBase.connAlarmGroup.alarm_dpc_fan_connect =1;
                    //clean-----------------------------------------------
                    fan_info_dpc_t.env_temp                    = 0;
                    fan_info_dpc_t.airflow_temp                = 0;
                    fan_info_dpc_t.voltage                     = 0;
                    fan_info_dpc_t.working_current             = 0;
                    
                    fan_info_dpc_t.state_device                = 0;
                    fan_info_dpc_t.state_fan                   = 0;
                    
                    fan_info_dpc_t.alarm_fail_refrigeration    = 0;
                    fan_info_dpc_t.alarm_high_temp             = 0;
                    fan_info_dpc_t.alarm_low_temp              = 0;
                    fan_info_dpc_t.alarm_fail_sensor_temp      = 0;
                    fan_info_dpc_t.alarm_high_voltage          = 0;
                    fan_info_dpc_t.alarm_low_voltage           = 0;
                    
                    fan_info_dpc_t.para_dc_starting_point      = 0;
                    fan_info_dpc_t.para_dc_sensivive_point     = 0;
                    fan_info_dpc_t.para_dc_heating_start_point = 0;
                    fan_info_dpc_t.para_dc_heating_sensivive   = 0;
                    fan_info_dpc_t.para_alarm_high_temp        = 0;
                    fan_info_dpc_t.para_alarm_low_temp         = 0;
                    //------------------------------------------------------
                  }
                }
                fan_dpc_switch_uart_t.uart_rx.cnt =0;
                //code here-------------------------------------------------------------------------
                fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
                Modbus.runningStep                = _READ_FAN_DPC_STATUS;
                fan_dpc_switch_uart_t.fan_state_e = _DKD51_FAN_STATE_REQ;
                break;
              case _DKD51_FAN_STATE_REQ:
                //code here-------------------------------------------------------------------------
                if(DKD51_check_sum_fan(&fan_dpc_switch_uart_t) == NO_ERR){
                  if(DKD51_check_RTN_fan(&fan_dpc_switch_uart_t) == NO_ERR){
                    fan_info_dpc_t.state_device = hex2byte(&fan_dpc_switch_uart_t.rx.buff_485[0],15);  // 0x00H:Shut down; 0x01H: boot
                    fan_info_dpc_t.state_fan    = hex2byte(&fan_dpc_switch_uart_t.rx.buff_485[0],17);  // 0x00H:Shut down; 0x01H: boot
                  } 
                }
                else{
                  fan_dpc_switch_uart_t.check_sum_calculator = 0;
                  fan_dpc_switch_uart_t.check_sum_read       = 0xFFFF;
                }
                fan_dpc_switch_uart_t.uart_rx.cnt =0;                
                //code here-------------------------------------------------------------------------
                fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
                Modbus.runningStep                = _READ_FAN_DPC_STATUS;  
                fan_dpc_switch_uart_t.fan_state_e = _DKD51_FAN_WARNING_REQ;
                break;
              case _DKD51_FAN_WARNING_REQ:
                //code here------------------------------------------------------------------------
                if(DKD51_check_sum_fan(&fan_dpc_switch_uart_t) == NO_ERR){
                  if(DKD51_check_RTN_fan(&fan_dpc_switch_uart_t) == NO_ERR){
                    fan_info_dpc_t.alarm_fail_refrigeration   = hex2byte(&fan_dpc_switch_uart_t.rx.buff_485[0],15);  //0x00H: normal; 0xF0H:Falty
                    fan_info_dpc_t.alarm_high_temp            = hex2byte(&fan_dpc_switch_uart_t.rx.buff_485[0],17);  //0x00H: normal; 0xF0H:Falty
                    fan_info_dpc_t.alarm_low_temp             = hex2byte(&fan_dpc_switch_uart_t.rx.buff_485[0],19);  //0x00H: normal; 0xF0H:Falty
                    fan_info_dpc_t.alarm_fail_sensor_temp     = hex2byte(&fan_dpc_switch_uart_t.rx.buff_485[0],23);  //0x00H: normal; 0xF0H:Falty
                    fan_info_dpc_t.alarm_high_voltage         = hex2byte(&fan_dpc_switch_uart_t.rx.buff_485[0],25);  //0x00H: normal; 0xF0H:Falty
                    fan_info_dpc_t.alarm_low_voltage          = hex2byte(&fan_dpc_switch_uart_t.rx.buff_485[0],27);  //0x00H: normal; 0xF0H:Falty
                  } 
                }
                else{
                  fan_dpc_switch_uart_t.check_sum_calculator = 0;
                  fan_dpc_switch_uart_t.check_sum_read       = 0xFFFF;
                }
                fan_dpc_switch_uart_t.uart_rx.cnt =0;
                //code here-------------------------------------------------------------------------
                fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
                Modbus.runningStep                = _READ_FAN_DPC_STATUS; 
                fan_dpc_switch_uart_t.fan_state_e = _DKD51_FAN_PARAMETER_REQ;
                break;
              case _DKD51_FAN_PARAMETER_REQ:
                //code here-------------------------------------------------------------------------
                if(DKD51_check_sum_fan(&fan_dpc_switch_uart_t) == NO_ERR){
                  if(DKD51_check_RTN_fan(&fan_dpc_switch_uart_t) == NO_ERR){
                    fan_info_dpc_t.para_dc_starting_point    = hex2byte2(&fan_dpc_switch_uart_t.rx.buff_485[0],13);  // realx100
                    fan_info_dpc_t.para_dc_sensivive_point   = hex2byte2(&fan_dpc_switch_uart_t.rx.buff_485[0],17);  // realx100
                    fan_info_dpc_t.para_dc_heating_start_point = ihex2byte2(&fan_dpc_switch_uart_t.rx.buff_485[0],21);// realx100
                    fan_info_dpc_t.para_dc_heating_sensivive   = iihex2byte2(&fan_dpc_switch_uart_t.rx.buff_485[0],25);// realx100
                    fan_info_dpc_t.para_alarm_high_temp      = hex2byte2(&fan_dpc_switch_uart_t.rx.buff_485[0],29);  // realx100
                    fan_info_dpc_t.para_alarm_low_temp       = ihex2byte2(&fan_dpc_switch_uart_t.rx.buff_485[0],33);  // realx100
                 } 
                }
                else{
                  fan_dpc_switch_uart_t.check_sum_calculator = 0;
                  fan_dpc_switch_uart_t.check_sum_read       = 0xFFFF;
                }
                fan_dpc_switch_uart_t.uart_rx.cnt =0;
                //code here-------------------------------------------------------------------------
                fan_dpc_switch_uart_t.rx_enable   = STATE_OFF;
                Modbus.runningStep                = _READ_FAN_DPC_STATUS;    
                fan_dpc_switch_uart_t.fan_state_e = _DKD51_FAN_EQUIPMENT_REQ;
                break;
              case _DKD51_FAN_EQUIPMENT_REQ:
                //code here
                fan_dpc_switch_uart_t.rx_enable  = STATE_OFF;
                Modbus.runningStep               = _WAIT_MODE;
                break;
              default:
                fan_dpc_switch_uart_t.rx_enable  = STATE_OFF;
                Modbus.runningStep               = _WAIT_MODE;
                break;
              }
              
            }else{
              Modbus.runningStep = _WAIT_MODE;
              privateMibBase.connAlarmGroup.alarm_dpc_fan_connect = 0;
            }
          }
          break;
  #endif
  
          case _WAIT_WRITE_MULTI_REG:
          {          
             vTaskDelay(300);
             reVal = RS4851_Check_Respond_Setting_Data();
             switch(sModbusManager.SettingCommand)
             {                
                case SET_BM_VOLTDIFF:
                case SET_BM_MAXTEMP:
                case SET_BM_VOLTTHRES:
                case SET_BM_CURRTHRES:
                case SET_BM_TIMETHRES:
                case SET_BM_SOCTHRES:
                case SET_BM_MINTEMP:
                case SET_BM_LOWCAPTIME:
                case SET_BM_HALFVOLTALARM:
                case SET_BM_RECHARGEVOLT:
                case SET_BM_CURRBALANCETHRES: 
                  {
                    if (sModbusManager.u8BMCurrentIndex > sModbusManager.u8NumberOfBM - 1)
                    {
                      sModbusManager.u8BMCurrentIndex = 0;
                      Modbus.runningStep = _READ_BM_STATUS;                    
                      sModbusManager.SettingCommand = SET_DAQ_NOOP;  
                    }
                  }break;
                case SET_VAC_SYSMODE:
                case SET_VAC_FANSPEEDMODE:
                case SET_VAC_FANSTARTTEMP:
                case SET_VAC_SETTEMP:
                case SET_VAC_PIDOFFSETTEMP:
                case SET_VAC_FANMINSPEED:
                case SET_VAC_FANMAXSPEED:
                case SET_VAC_FILTERSTUCKTEMP:
                case SET_VAC_NIGHTMODEEN:
                case SET_VAC_NIGHTMODESTART:
                case SET_VAC_NIGHTMODEEND:
                case SET_VAC_NIGHTMAXSPEED:
                case SET_VAC_MANUALMODE:
                case SET_VAC_MANUALMAXSPEED:
                case SET_VAC_INMAXTEMP:
                case SET_VAC_EXMAXTEMP:
                case SET_VAC_FROSTMAXTEMP:
                case SET_VAC_INMINTEMP:
                case SET_VAC_EXMINTEMP:
                case SET_VAC_FROSTMINTEMP:
                case SET_VAC_MINOUTTEMP:
                case SET_VAC_DELTATEMP:
                case SET_VAC_PANICTEMP:
                case SET_VAC_ACU1ONTEMP:
                case SET_VAC_ACU2ONTEMP:
                case SET_VAC_ACU2EN:
                case SET_VAC_SYNCTIME:
                case SET_VAC_AIRCON1MODEL:
                case SET_VAC_AIRCON1TYPE:
                case SET_VAC_AIRCON2MODEL:
                case SET_VAC_AIRCON2TYPE:
                case SET_VAC_AIRCONONOFF:
                case SET_VAC_AIRCONMODE:
                case SET_VAC_AIRCONTEMP:
                case SET_VAC_AIRCONSPEED:
                case SET_VAC_AIRCONDDIR:
                case SET_VAC_W_ENABLE:
                case SET_VAC_W_SERIAL1:
                case SET_VAC_W_SERIAL2:
                case SET_VAC_RESET:
                  {
                    Modbus.runningStep = _READ_VAC_STATUS;                     
                    sModbusManager.SettingCommand = SET_DAQ_NOOP; 
                  }break; 
                case SET_SMCB_STATE:
                  {
                    vTaskDelay(2000); // do SMCB update thong so cham, nen phai doi 1 thoi gian sau khi cai dat moi hoi lai
                    Modbus.runningStep = _READ_SMCB_STATUS;                     
                    sModbusManager.SettingCommand = SET_DAQ_NOOP; 
                  }break;
             case SET_GEN_AUTOMODE:
             case SET_GEN_STOPMODE:
             case SET_GEN_STARTMODE:
             case SET_GEN_MANUALMODE:
                  {
                    Modbus.runningStep = _READ_GEN_STATUS;                     
                    sModbusManager.SettingCommand = SET_DAQ_NOOP; 
                  }break;
             case SET_LIB_PACKHIGHVOLT:
              case SET_LIB_BATTHIGHVOLT:
              case SET_LIB_CELLHIGHVOLT:
              case SET_LIB_BATTLOWVOLT:
              case SET_LIB_CELLLOWVOLT:
              case SET_LIB_CHARHIGHCURR:
              case SET_LIB_DISCHARHIGHCURR:
              case SET_LIB_CHARHIGHTEMP:
              case SET_LIB_DISCHARHIGHTEMP:
              case SET_LIB_CHARLOWTEMP:
              case SET_LIB_DISCHARLOWTEMP:
              case SET_LIB_LOWCAP:
              case SET_LIB_BMSHIGHTEMP:
              case SET_LIB_BMSLOWTEMP:
              case SET_LIB_PACKOVERVOLT:
              case SET_LIB_BATTOVERVOLT:
              case SET_LIB_CELLOVERVOLT:
              case SET_LIB_BATTUNDERVOLT:
              case SET_LIB_CELLUNDERVOLT:
              case SET_LIB_CHAROVERCURR:
              case SET_LIB_DISCHAROVERCURR:
              case SET_LIB_CHAROVERTEMP:
              case SET_LIB_DISCHAROVERTEMP:
              case SET_LIB_CHARUNDERTEMP:
              case SET_LIB_DISCHARUNDERTEMP:
              case SET_LIB_UNDERCAP:
              case SET_LIB_BMSOVERTEMP:
              case SET_LIB_BMSUNDERTEMP:
              case SET_LIB_DIFFERENTVOLT:
              case SET_LIB_VOLTBALANCE:
              case SET_LIB_DELTAVOLTBALANCE:
              case SET_LIB_DISCHARCURRLIMIT:
              case SET_LIB_CHARCURRLIMIT:
              case SET_LIB_VOLTDISCHARREF:
              case SET_LIB_VOLTDISCHARCMD:
              case SET_LIB_SYSTEMBYTE:
              case SET_LIB_KEYTIME:
              case SET_LIB_INIMAXIM:
              case SET_LIB_ENABLEKCS:
              case SET_LIB_ENABLELOCK:
              case SET_LIB_CAPINIT:
                {
                Modbus.runningStep = _READ_LIB_STATUS;                     
                sModbusManager.SettingCommand = SET_DAQ_NOOP; 
                }break;
              case SET_FAN_DPC_STARTING_POINT:
              case SET_FAN_DPC_SENSITIVE_POINT:
              case SET_FAN_DPC_H_TEMP_W_POINT:
              case SET_FAN_DPC_L_TEMP_W_POINT:
              case SET_FAN_DPC_HEATING_START_POINT:
              case SET_FAN_DPC_HEATING_SENSITIVE_POINT:
                {
                  Modbus.runningStep = _READ_FAN_DPC_STATUS;                     
                  sModbusManager.SettingCommand = SET_DAQ_NOOP;
                }
                break;
             
             }; 
          }
            break;
          default:
            break;
          }
          UpdateInfo ();
        }
        break;
      };
    }
}

#if (USER_DEF_CHECK_GEN ==  ENABLED)
uint8_t  remainning_gen       = 0;
uint16_t cnt_remainning_gen   = 0;
#endif


void UpdateInfo (void)
{    
   UpdateLIBInfo();
   UpdateGENInfo();
#if (USERDEF_MONITOR_BM == ENABLED)
  UpdateBMInfo();
#endif

#if (USERDEF_MONITOR_VAC == ENABLED)
  UpdateVACInfo();
#endif
#if (USERDEF_MONITOR_SMCB == ENABLED) //smcb
  UpdateSMCBInfo();
#endif
#if (USERDEF_MONITOR_PM == ENABLED)
  UpdatePMInfo();
#endif
#if (USERDEF_MONITOR_FUEL == ENABLED)
  UpdateFUELInfo();
#endif
#if (USERDEF_MONITOR_ISENSE == ENABLED)
  UpdateISENSEInfo();
#endif
#if ( USERDEF_MONITOR_PM_DC == ENABLED )
  Update_PM_DC_Info();
#endif
#if (USERDEF_RS485_DKD51_BDP == ENABLED)
  update_fan_dpc_info();
#endif
#if (USER_DEF_CHECK_GEN ==  ENABLED)
  if((sModbusManager.u8NumberOfISENSE!=0)&&(sModbusManager.u8NumberOfPM!=0))
  {
    check_gen(); 
  }else{
    remainning_gen     = 0;
    cnt_remainning_gen = 0;
  }
    
  
#endif

}

void UpdateLIBInfo(void)
{
    uint32_t j,i=0;
    privateMibBase.liBattGroup.liBattInstalledPack = sModbusManager.u8NumberOfLIB;
    privateMibBase.connAlarmGroup.alarmLIBConnect = 0;
    privateMibBase.mainAlarmGroup.alarmLIB = 0;
    privateMibBase.liBattGroup.liBattActivePack = privateMibBase.liBattGroup.liBattInstalledPack;
    for (i = 0; i < privateMibBase.liBattGroup.liBattInstalledPack; i++)
    {
//        privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag = 0;
        privateMibBase.mainAlarmGroup.alarmLIBPack[i] = 0;
        if (sLiionBattInfo[i].u8ErrorFlag == 1)
        {
          if(cntDisConnectLIB[i] > 2){ // mat ket noi 3 lan moi bao len
            cntDisConnectLIB[i] = 3;
            privateMibBase.liBattGroup.liBattActivePack--;
            
            if (privateMibBase.liBattGroup.liBattActivePack <= 0)
            {
              privateMibBase.liBattGroup.liBattActivePack = 0;
            }
            privateMibBase.connAlarmGroup.alarmLIBConnect |= (sLiionBattInfo[i].u8ErrorFlag << i);
            privateMibBase.liBattGroup.liBattTable[i].liBattStatus = 170;
          }     
        }
        else
        {
            cntDisConnectLIB[i] = 0;

            if(privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.all != 0)
            {
                privateMibBase.liBattGroup.liBattTable[i].liBattStatus = 165;
            }
            else
            {
                privateMibBase.liBattGroup.liBattTable[i].liBattStatus = 255;
            }
        }

        if (privateMibBase.liBattGroup.liBattTable[i].liBattStatus != 170)
        {
            if (privateMibBase.liBattGroup.liBattTable[i].liBattStatus == 165)
            {
                privateMibBase.mainAlarmGroup.alarmLIBPack[i] = privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.all;
            }

            switch(sModbusManager.sLIBManager[i].u8LIBType)
            {
            case 13:// COSLIGHT_CF4850T
            case 1:// COSLIGHT 
              {
                privateMibBase.liBattGroup.liBattTable[i].liBattIndex = i+1; 
                for (j = 0; j < 16; j++)
                {
                    privateMibBase.liBattGroup.liBattTable[i].liBattCellVolt[j] = sLiionBattInfo[i].u16CellVolt[j];
                }
                for (j = 0; j < 16; j++)
                {
                    privateMibBase.liBattGroup.liBattTable[i].liBattCellTemp[j] = sLiionBattInfo[i].u16CellTemp[j] * 10;
                }
                privateMibBase.liBattGroup.liBattTable[i].liBattAvrCellTemp = sLiionBattInfo[i].u16AverTempCell * 10;
                privateMibBase.liBattGroup.liBattTable[i].liBattAmbTemp = sLiionBattInfo[i].u16EnvTemp * 10;
                privateMibBase.liBattGroup.liBattTable[i].liBattPackVolt = sLiionBattInfo[i].u32PackVolt;
                privateMibBase.liBattGroup.liBattTable[i].liBattSOH = sLiionBattInfo[i].u32SOH * 100;
                privateMibBase.liBattGroup.liBattTable[i].liBattSOC = sLiionBattInfo[i].u32SOC * 100;
                privateMibBase.liBattGroup.liBattTable[i].liBattRemainCap = sLiionBattInfo[i].u32CapRemain;
                privateMibBase.liBattGroup.liBattTable[i].liBattPeriod = sLiionBattInfo[i].u32DischargeTime;
                privateMibBase.liBattGroup.liBattTable[i].liBattPackCurr = sLiionBattInfo[i].u32PackCurr;
                privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.all = sLiionBattInfo[i].u32AlarmStatus;
                privateMibBase.liBattGroup.liBattTable[i].liBattMode = sLiionBattInfo[i].u16ModuleMode;
                //thanhcm3 fix---------------------------------------------------------------------------------------
                if(sModbusManager.sLIBManager[i].u8LIBType == 13){
                  strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattModel, "COSLIGHT_CF4850T");
                  privateMibBase.liBattGroup.liBattTable[i].liBattModelLen = 16; 
                }else if(sModbusManager.sLIBManager[i].u8LIBType == 1)
                {
                  strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattModel, "GYFP4850T");
                  privateMibBase.liBattGroup.liBattTable[i].liBattModelLen = 9;
                }
                //thanhcm3 fix---------------------------------------------------------------------------------------
 
                privateMibBase.liBattGroup.liBattTable[i].u32DSGCapacity = sLiionBattInfo[i].u32DSGCapacity;  
                float SV;
                uint32_t str_len;
                SV = (float)sLiionBattInfo[i].u16SoftwareVersion/100;
                str_len = sprintf(&privateMibBase.liBattGroup.liBattTable[i].liBattSWVer[0],"%.2f",SV);
                privateMibBase.liBattGroup.liBattTable[i].liBattSWVerLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattSWVer);
                sprintf(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo,"%d",sLiionBattInfo[i].u32Serial);
                privateMibBase.liBattGroup.liBattTable[i].liBattSerialNoLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo);
                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattHWVer, "unavailable");
                privateMibBase.liBattGroup.liBattTable[i].liBattHWVerLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattHWVer);
                privateMibBase.liBattGroup.liBattTable[i].liBattStatusCCL = 0;

                privateMibBase.liBattGroup.liBattTable[i].liBattType = sModbusManager.sLIBManager[i].u8LIBType;
                privateMibBase.liBattGroup.liBattTable[i].liBattModbusID = sModbusManager.sLIBManager[i].u32LIBAbsSlaveID;
              }
              break;
              case 2:// COSLIGHT_OLD_V1.1 
              {
                privateMibBase.liBattGroup.liBattTable[i].liBattIndex = i+1; 
                privateMibBase.liBattGroup.liBattTable[i].liBattAmbTemp = sLiionBattInfo[i].u16AverTempCell * 10;
                privateMibBase.liBattGroup.liBattTable[i].liBattPackVolt = sLiionBattInfo[i].u32PackVolt;
                privateMibBase.liBattGroup.liBattTable[i].liBattSOH = sLiionBattInfo[i].u32SOH * 100;
                privateMibBase.liBattGroup.liBattTable[i].liBattSOC = sLiionBattInfo[i].u32SOC * 100;
                privateMibBase.liBattGroup.liBattTable[i].liBattRemainCap = sLiionBattInfo[i].u32CapRemain;
                privateMibBase.liBattGroup.liBattTable[i].liBattPeriod = sLiionBattInfo[i].u32DischargeTime;
                privateMibBase.liBattGroup.liBattTable[i].liBattPackCurr = sLiionBattInfo[i].u32PackCurr;
                privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.all = sLiionBattInfo[i].u32AlarmStatus;

                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattModel, "GYFP4850T");
                privateMibBase.liBattGroup.liBattTable[i].liBattModelLen = 9;
                

                uint32_t str_len;
                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattSWVer, "V1.1");
                privateMibBase.liBattGroup.liBattTable[i].liBattSWVerLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattSWVer);
                sprintf(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo,"%d",sLiionBattInfo[i].u32Serial);
                privateMibBase.liBattGroup.liBattTable[i].liBattSerialNoLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo);
                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattHWVer, "unavailable");
                privateMibBase.liBattGroup.liBattTable[i].liBattHWVerLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattHWVer);
                privateMibBase.liBattGroup.liBattTable[i].liBattStatusCCL = 0;

                privateMibBase.liBattGroup.liBattTable[i].liBattType = sModbusManager.sLIBManager[i].u8LIBType;
                privateMibBase.liBattGroup.liBattTable[i].liBattModbusID = sModbusManager.sLIBManager[i].u32LIBAbsSlaveID;
              }
              break;
            case 12:// SHOTO_SDA10_48100  
            case 3:// SHOTO_2019
              {
                privateMibBase.liBattGroup.liBattTable[i].liBattIndex = i+1;
                privateMibBase.liBattGroup.liBattTable[i].liBattPackVolt = sLiionBattInfo[i].u32PackVolt;
                privateMibBase.liBattGroup.liBattTable[i].liBattPackCurr = (int16_t)sLiionBattInfo[i].u32PackCurr;
                privateMibBase.liBattGroup.liBattTable[i].liBattSOH = sLiionBattInfo[i].u32SOH * 10;
                privateMibBase.liBattGroup.liBattTable[i].liBattSOC = sLiionBattInfo[i].u32SOC * 10;
                privateMibBase.liBattGroup.liBattTable[i].liBattPeriod = sLiionBattInfo[i].u32DischargeTime;
                privateMibBase.liBattGroup.liBattTable[i].liBattAvrCellTemp = sLiionBattInfo[i].u16AverTempCell;
                privateMibBase.liBattGroup.liBattTable[i].liBattRemainCap = sLiionBattInfo[i].u32CapRemain;
                privateMibBase.liBattGroup.liBattTable[i].liBattAmbTemp = sLiionBattInfo[i].u16EnvTemp ;
                privateMibBase.liBattGroup.liBattTable[i].u32DSGCapacity = sLiionBattInfo[i].u32DSGCapacity * 1000;  
                for (j = 0; j < 16; j++)
                {
                    privateMibBase.liBattGroup.liBattTable[i].liBattCellVolt[j] = sLiionBattInfo[i].u16CellVolt[j];
                }
                for (j = 0; j < 16; j++)
                {
                    privateMibBase.liBattGroup.liBattTable[i].liBattCellTemp[j] = sLiionBattInfo[i].u16CellTemp[j];
                }
                
                privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.all = sLiionBattInfo[i].u32AlarmStatus;
                privateMibBase.liBattGroup.liBattTable[i].liBattMode = sLiionBattInfo[i].u16ModuleMode;
                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattHWVer, "unavailable");
                privateMibBase.liBattGroup.liBattTable[i].liBattHWVerLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattHWVer);
                //thanhcm3 fix---------------------------------------------------------------------------------------
                if(sModbusManager.sLIBManager[i].u8LIBType == 12){
                  strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattModel, "SHOTO_SDA10_48100");
                  privateMibBase.liBattGroup.liBattTable[i].liBattModelLen = 17;
                }else if(sModbusManager.sLIBManager[i].u8LIBType == 3){
                  strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattModel, "SDA10-4850");
                  privateMibBase.liBattGroup.liBattTable[i].liBattModelLen = 10;
                }
                //thanhcm3 fix---------------------------------------------------------------------------------------
                
                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattSWVer,(const char*) &sLiionBattInfo[i].u8swversion[0]);
//                sprintf(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo,"%d",sLiionBattInfo[i].u32Serial);
                for(int k=0;k<30;k++)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo[k] = sLiionBattInfo[i].u8mode[k];
                }
                privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo[30] = '\0';
                privateMibBase.liBattGroup.liBattTable[i].liBattSerialNoLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo);
                privateMibBase.liBattGroup.liBattTable[i].liBattSWVerLen = 2;
                privateMibBase.liBattGroup.liBattTable[i].liBattStatusCCL = 0;
//                privateMibBase.liBattGroup.liBattTable[i].liBattSerialNoLen = 30;

                privateMibBase.liBattGroup.liBattTable[i].liBattType = sModbusManager.sLIBManager[i].u8LIBType;
                privateMibBase.liBattGroup.liBattTable[i].liBattModbusID = sModbusManager.sLIBManager[i].u32LIBAbsSlaveID;
              }
              break;
            case 4:// HUAWEI
            {
                privateMibBase.liBattGroup.liBattTable[i].liBattIndex = i+1;
                privateMibBase.liBattGroup.liBattTable[i].liBattPackVolt = sLiionBattInfo[i].u32PackVolt;
                privateMibBase.liBattGroup.liBattTable[i].liBattPackCurr = sLiionBattInfo[i].u16PackCurr;
                privateMibBase.liBattGroup.liBattTable[i].liBattRemainCap = sLiionBattInfo[i].u32CapRemain;
                privateMibBase.liBattGroup.liBattTable[i].liBattAvrCellTemp = sLiionBattInfo[i].u16AverTempCell * 10;
                privateMibBase.liBattGroup.liBattTable[i].liBattAmbTemp = sLiionBattInfo[i].u16EnvTemp * 10;
                privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag = sLiionBattInfo[i].u32AlarmStatus;
                privateMibBase.liBattGroup.liBattTable[i].liBattProtectFlag = sLiionBattInfo[i].u32ProtectStatus;
                privateMibBase.liBattGroup.liBattTable[i].liBattFaultStat = sLiionBattInfo[i].u32FaultStatus;
                privateMibBase.liBattGroup.liBattTable[i].liBattSOC = sLiionBattInfo[i].u32SOC * 100;
                privateMibBase.liBattGroup.liBattTable[i].liBattSOH = sLiionBattInfo[i].u32SOH * 100;
                //thanhcm3 fix---------------------------------------------------------------
                if(sLiionBattInfo[i].u16ModuleMode == 2)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattMode = 1;
                }else if(sLiionBattInfo[i].u16ModuleMode == 3)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattMode = 2;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattMode = 4;
                }
                //privateMibBase.liBattGroup.liBattTable[i].liBattMode = sLiionBattInfo[i].u16ModuleMode;
                //thanhcm3 fix---------------------------------------------------------------
                privateMibBase.liBattGroup.liBattTable[i].liBattPeriod = sLiionBattInfo[i].u32DischargeTime;
                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo,(const char*) &sLiionBattInfo[i].u8mode[0]);
                privateMibBase.liBattGroup.liBattTable[i].u32DSGCapacity = sLiionBattInfo[i].u32CapRemain * 100;  
//                privateMibBase.liBattGroup.liBattTable[i].liBattSerialNoLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo);
                privateMibBase.liBattGroup.liBattTable[i].liBattSerialNoLen = 12;
                 for (j = 0; j < 16; j++)
                {
                    privateMibBase.liBattGroup.liBattTable[i].liBattCellVolt[j] = sLiionBattInfo[i].u16CellVolt[j];
                }

                for (j = 0; j < 16; j++)
                {
                    privateMibBase.liBattGroup.liBattTable[i].liBattCellTemp[j] = sLiionBattInfo[i].u16CellTemp[j] * 10;
                }

                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattModel, "ESM-48100A2");
                privateMibBase.liBattGroup.liBattTable[i].liBattModelLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattModel);
                sprintf(privateMibBase.liBattGroup.liBattTable[i].liBattSWVer,"%d",sLiionBattInfo[i].u16SoftwareVersion);
                privateMibBase.liBattGroup.liBattTable[i].liBattSWVerLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattSWVer);
                sprintf(privateMibBase.liBattGroup.liBattTable[i].liBattHWVer,"%d",sLiionBattInfo[i].u16HardwareVersion);
                privateMibBase.liBattGroup.liBattTable[i].liBattHWVerLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattHWVer);
//                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo, "unavailable");
//                privateMibBase.liBattGroup.liBattTable[i].liBattSerialNoLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo);
                //thanhcm3 fix------------------------------------------------------
                if(((sLiionBattInfo[i].u32ProtectStatus &0x00020000)==0x00020000)||((sLiionBattInfo[i].u32ProtectStatus&0x0000ffff)!=0)){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_cell =1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_cell =0;
                }
                if((sLiionBattInfo[i].u32ProtectStatus &0x00040000)==0x00040000){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_sensor =1;
                 
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_sensor =0;
                }
                //thanhcm3 fix------------------------------------------------------
                
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000020) == 0x00000020)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellOverVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellOverVolt = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000010) == 0x00000010)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackOverVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackOverVolt = 0;
                }
                
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000080) == 0x00000080)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellLowVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellLowVolt = 0;
                }
                
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000040) == 0x00000040)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackLowVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackLowVolt = 0;
                }
//                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000001) == 0x00000001)
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_VoltDiff = 1;
//                }
//                if ((sLiionBattInfo[i].u32FaultStatus & 0x00000001) == 0x00000001)
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CharOverCurrent = 1;
//                }
                if ((sLiionBattInfo[i].u32FaultStatus & 0x00000010) == 0x00000010)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_DisCharOverCurrent = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_DisCharOverCurrent = 0;
                }
                
                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00000001) == 0x00000001) || ((sLiionBattInfo[i].u32AlarmStatus & 0x00000004) == 0x00000004))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattLowTemp = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattLowTemp = 0;
                }
                
                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00000002) == 0x00000002) || ((sLiionBattInfo[i].u32AlarmStatus & 0x00000008) == 0x00000008))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattHighTemp = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattHighTemp = 0;
                }
                
//                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000001) == 0x00000001)
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_LowCapacity = 1;
//                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00200000) == 0x00200000)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellOverVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellOverVolt = 0;
                }                
                
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00100000) == 0x00100000)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackOverVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackOverVolt = 0;
                }
                
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00800000) == 0x00800000)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellLowVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellLowVolt = 0;
                }                
                
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00400000) == 0x00400000)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackLowVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackLowVolt = 0;
                }
                
                if ((sLiionBattInfo[i].u32FaultStatus & 0x00000020) == 0x00000020)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_OverCurr = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_OverCurr = 0;
                }
                
                if ((sLiionBattInfo[i].u32ProtectStatus & 0x00080000) == 0x00080000)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_ShortCircuit = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_ShortCircuit = 0;
                }
                
                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00020000) == 0x00020000)||((sLiionBattInfo[i].u32AlarmStatus & 0x00080000) == 0x00080000))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_high_temp = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_high_temp = 0;
                }
                
                
//                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00080000) == 0x00080000))
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharHighTemp = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharHighTemp = 0;
//                }
                
                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00010000) == 0x00010000)||((sLiionBattInfo[i].u32AlarmStatus & 0x00040000) == 0x00040000))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_low_temp = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_low_temp = 0;
                }
                
//                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00040000) == 0x00040000))
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharLowTemp = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharLowTemp = 0;
//                }
                
//                privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.all = sLiionBattInfo[i].u32AlarmStatus;
                privateMibBase.liBattGroup.liBattTable[i].liBattStatusCCL = 0;

                privateMibBase.liBattGroup.liBattTable[i].liBattType = sModbusManager.sLIBManager[i].u8LIBType;
                privateMibBase.liBattGroup.liBattTable[i].liBattModbusID = sModbusManager.sLIBManager[i].u32LIBAbsSlaveID;
            }
            break;
            case 5:// M1Viettel50
            {
                privateMibBase.liBattGroup.liBattTable[i].liBattIndex = i+1;
                
                privateMibBase.liBattGroup.liBattTable[i].liBattPackVolt = sLiionBattInfo[i].u32PackVolt;
                privateMibBase.liBattGroup.liBattTable[i].u32ADCPackVolt = sLiionBattInfo[i].u32ADCPackVolt;
                privateMibBase.liBattGroup.liBattTable[i].u32ADCBattVolt = sLiionBattInfo[i].u32ADCBattVolt;
                privateMibBase.liBattGroup.liBattTable[i].liBattPackCurr = sLiionBattInfo[i].u32PackCurr;
                privateMibBase.liBattGroup.liBattTable[i].i32ADCPackCurr = sLiionBattInfo[i].i32ADCPackCurr;
                privateMibBase.liBattGroup.liBattTable[i].i32ADCBattCurr = sLiionBattInfo[i].i32ADCBattCurr;
                
                for (j = 0; j < 15; j++)
                {
                    privateMibBase.liBattGroup.liBattTable[i].liBattCellVolt[j] = sLiionBattInfo[i].u16CellVolt[j];
                }

                for (j = 0; j < 4; j++)
                {
                    privateMibBase.liBattGroup.liBattTable[i].liBattCellTemp[j] = sLiionBattInfo[i].u16CellTemp[j]/10;
                }
                privateMibBase.liBattGroup.liBattTable[i].liBattAvrCellTemp = sLiionBattInfo[i].u16AverTempCell/10;
                
                int32_t cellTemp1 = 0,cellTemp2 = 0;
                for (j = 0; j < 4; j++)
                {
                  if((sLiionBattInfo[i].u16CellTemp[j]>0)&&(sLiionBattInfo[i].u16CellTemp[j]<8000)){
                    cellTemp1++;
                    cellTemp2 = cellTemp2 + sLiionBattInfo[i].u16CellTemp[j];
                  }
                }
                privateMibBase.liBattGroup.liBattTable[i].liBattAmbTemp = cellTemp2 / cellTemp1;
                privateMibBase.liBattGroup.liBattTable[i].liBattAmbTemp = privateMibBase.liBattGroup.liBattTable[i].liBattAmbTemp / 10;
                
                privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag = sLiionBattInfo[i].u32AlarmStatus;
                privateMibBase.liBattGroup.liBattTable[i].liBattProtectFlag = sLiionBattInfo[i].u32ProtectStatus;
                privateMibBase.liBattGroup.liBattTable[i].liBattFaultStat = sLiionBattInfo[i].u32FaultStatus;
                privateMibBase.liBattGroup.liBattTable[i].liBattMode = sLiionBattInfo[i].u16ModuleMode;
                privateMibBase.liBattGroup.liBattTable[i].liBattSOC = sLiionBattInfo[i].u32SOC*10;
                privateMibBase.liBattGroup.liBattTable[i].liBattSOH = sLiionBattInfo[i].u32SOH*100;
                privateMibBase.liBattGroup.liBattTable[i].liBattPeriod = sLiionBattInfo[i].u32DischargeTime;  
                privateMibBase.liBattGroup.liBattTable[i].u32ConverterState = sLiionBattInfo[i].u16ConverterState;     
                privateMibBase.liBattGroup.liBattTable[i].u32ErrCode = sLiionBattInfo[i].u16ErrCode;     
                privateMibBase.liBattGroup.liBattTable[i].u32BalanceStatus = sLiionBattInfo[i].u16BalanceStatus; 
                privateMibBase.liBattGroup.liBattTable[i].u32MosfetMode = sLiionBattInfo[i].u16MosfetMode;     
                privateMibBase.liBattGroup.liBattTable[i].u32Mcu2McuErr = sLiionBattInfo[i].u32Mcu2McuErr;     
                privateMibBase.liBattGroup.liBattTable[i].u32CHGCapacity = sLiionBattInfo[i].u32CHGCapacity;     
                privateMibBase.liBattGroup.liBattTable[i].u32DSGCapacity = sLiionBattInfo[i].u32DSGCapacity;     
                privateMibBase.liBattGroup.liBattTable[i].u32Efficiency = sLiionBattInfo[i].u32Efficiency;     
                privateMibBase.liBattGroup.liBattTable[i].u32NumberOfCan = sLiionBattInfo[i].u16NumberOfCan;  
                //strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattSWVer, &sLiionBattInfo[i].u8swversion[0]);
                sprintf(&privateMibBase.liBattGroup.liBattTable[i].liBattSWVer[0],"%d",sLiionBattInfo[i].sw_ver);
                privateMibBase.liBattGroup.liBattTable[i].liBattSWVerLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattSWVer);
                  
                privateMibBase.liBattGroup.liBattTable[i].u32SOCMaxim = (uint32_t)(sLiionBattInfo[i].u16SOCMaxim*100/256);    
                privateMibBase.liBattGroup.liBattTable[i].u32SOHMaxim = (uint32_t)(sLiionBattInfo[i].u16SOHMaxim*100/256); 
                if(privateMibBase.liBattGroup.liBattTable[i].u32SOHMaxim > 10000)
                  privateMibBase.liBattGroup.liBattTable[i].u32SOHMaxim = 10000;
                privateMibBase.liBattGroup.liBattTable[i].u32FullCapRepMaxim = sLiionBattInfo[i].u16FullCapRepMaxim;    
                privateMibBase.liBattGroup.liBattTable[i].u32VoltMaxim = sLiionBattInfo[i].u16VoltMaxim;    
                privateMibBase.liBattGroup.liBattTable[i].u32CurrMaxim = sLiionBattInfo[i].u16CurrMaxim;  
                  
                privateMibBase.liBattGroup.liBattTable[i].u32IKalamn = sLiionBattInfo[i].u32IKalamn;    
                privateMibBase.liBattGroup.liBattTable[i].u32SOCKalamn = sLiionBattInfo[i].u32SOCKalamn;    
                privateMibBase.liBattGroup.liBattTable[i].u32VpriKalamn = sLiionBattInfo[i].u32VpriKalamn;    
                privateMibBase.liBattGroup.liBattTable[i].u32VmesKalamn = sLiionBattInfo[i].u32VmesKalamn;    
                privateMibBase.liBattGroup.liBattTable[i].u32CapKalamn = sLiionBattInfo[i].u32CapKalamn;    
                privateMibBase.liBattGroup.liBattTable[i].u32InternalR0Kalamn = sLiionBattInfo[i].u32InternalR0Kalamn;  
                
                privateMibBase.liBattGroup.liBattTable[i].i32PackHighVolt_A = sLiionBattInfo[i].i32PackHighVolt_A;  
                privateMibBase.liBattGroup.liBattTable[i].i32BattHighVolt_A = sLiionBattInfo[i].i32BattHighVolt_A;  
                privateMibBase.liBattGroup.liBattTable[i].i32CellHighVolt_A = sLiionBattInfo[i].i32CellHighVolt_A;  
                privateMibBase.liBattGroup.liBattTable[i].i32BattLowVolt_A = sLiionBattInfo[i].i32BattLowVolt_A;  
                privateMibBase.liBattGroup.liBattTable[i].i32CellLowVolt_A = sLiionBattInfo[i].i32CellLowVolt_A;  
                privateMibBase.liBattGroup.liBattTable[i].i32CharHighCurr_A = sLiionBattInfo[i].i32CharHighCurr_A;  
                privateMibBase.liBattGroup.liBattTable[i].i32DisCharHighCurr_A = sLiionBattInfo[i].i32DisCharHighCurr_A;  
                privateMibBase.liBattGroup.liBattTable[i].i32CharHighTemp_A = sLiionBattInfo[i].i32CharHighTemp_A;  
                privateMibBase.liBattGroup.liBattTable[i].i32DisCharHighTemp_A = sLiionBattInfo[i].i32DisCharHighTemp_A;  
                privateMibBase.liBattGroup.liBattTable[i].i32CharLowTemp_A = sLiionBattInfo[i].i32CharLowTemp_A;  
                privateMibBase.liBattGroup.liBattTable[i].i32DisCharLowtemp_A = sLiionBattInfo[i].i32DisCharLowtemp_A;  
                privateMibBase.liBattGroup.liBattTable[i].i32LowCap_A = sLiionBattInfo[i].i32LowCap_A;  
                privateMibBase.liBattGroup.liBattTable[i].i32BMSHighTemp_A = sLiionBattInfo[i].i32BMSHighTemp_A;  
                privateMibBase.liBattGroup.liBattTable[i].i32BMSLowTemp_A = sLiionBattInfo[i].i32BMSLowTemp_A;  
                privateMibBase.liBattGroup.liBattTable[i].i32PackOverVolt_P = sLiionBattInfo[i].i32PackOverVolt_P;  
                privateMibBase.liBattGroup.liBattTable[i].i32BattOverVolt_P = sLiionBattInfo[i].i32BattOverVolt_P;  
                privateMibBase.liBattGroup.liBattTable[i].i32CellOverVolt_P = sLiionBattInfo[i].i32CellOverVolt_P;  
                privateMibBase.liBattGroup.liBattTable[i].i32BattUnderVolt_P = sLiionBattInfo[i].i32BattUnderVolt_P;  
                privateMibBase.liBattGroup.liBattTable[i].i32CellUnderVolt_P = sLiionBattInfo[i].i32CellUnderVolt_P;  
                privateMibBase.liBattGroup.liBattTable[i].i32CharOverCurr_P = sLiionBattInfo[i].i32CharOverCurr_P; 
                privateMibBase.liBattGroup.liBattTable[i].i32DisCharOverCurr_P = sLiionBattInfo[i].i32DisCharOverCurr_P;  
                privateMibBase.liBattGroup.liBattTable[i].i32CharOverTemp_P = sLiionBattInfo[i].i32CharOverTemp_P;  
                privateMibBase.liBattGroup.liBattTable[i].i32DisCharOverTemp_P = sLiionBattInfo[i].i32DisCharOverTemp_P;  
                privateMibBase.liBattGroup.liBattTable[i].i32CharUnderTemp_P = sLiionBattInfo[i].i32CharUnderTemp_P;  
                privateMibBase.liBattGroup.liBattTable[i].i32DisCharUnderTemp_P = sLiionBattInfo[i].i32DisCharUnderTemp_P;  
                privateMibBase.liBattGroup.liBattTable[i].i32UnderCap_P = sLiionBattInfo[i].i32UnderCap_P;  
                privateMibBase.liBattGroup.liBattTable[i].i32BMSOverTemp_P = sLiionBattInfo[i].i32BMSOverTemp_P;  
                privateMibBase.liBattGroup.liBattTable[i].i32BMSUnderTemp_P = sLiionBattInfo[i].i32BMSUnderTemp_P;  
                privateMibBase.liBattGroup.liBattTable[i].i32DifferentVolt_P = sLiionBattInfo[i].i32DifferentVolt_P;
                privateMibBase.liBattGroup.liBattTable[i].u32VoltBalance = sLiionBattInfo[i].u32VoltBalance; 
                privateMibBase.liBattGroup.liBattTable[i].u32DeltaVoltBalance = sLiionBattInfo[i].u32DeltaVoltBalance; 
                privateMibBase.liBattGroup.liBattTable[i].u32DisCharCurrLimit = sLiionBattInfo[i].u32DisCharCurrLimit; 
                privateMibBase.liBattGroup.liBattTable[i].u32CharCurrLimit = sLiionBattInfo[i].u32CharCurrLimit; 
                privateMibBase.liBattGroup.liBattTable[i].u32VoltDisCharRef = sLiionBattInfo[i].u32VoltDisCharRef; 
                privateMibBase.liBattGroup.liBattTable[i].u32VoltDisCharCMD = sLiionBattInfo[i].u32VoltDisCharCMD; 
                privateMibBase.liBattGroup.liBattTable[i].u32SystemByte = sLiionBattInfo[i].u16SystemByte; 
                privateMibBase.liBattGroup.liBattTable[i].u32KeyTime = sLiionBattInfo[i].u16KeyTime; 
                privateMibBase.liBattGroup.liBattTable[i].u16_liionCapInit = sLiionBattInfo[i].u16_liionCapInit;

                privateMibBase.liBattGroup.liBattTable[i].u32EnableLock = sLiionBattInfo[i].u16EnableLock;  
                
                if(sLiionBattInfo[i].u16BattType == 50){
                  strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattModel, "VLB48C50F");
                  privateMibBase.liBattGroup.liBattTable[i].liBattModelLen = 9;
                } else if (sLiionBattInfo[i].u16BattType == 100){
                  strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattModel, "VLB48C100F");
                  privateMibBase.liBattGroup.liBattTable[i].liBattModelLen = 10;
                }
                
                sprintf(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo,"%d",(uint16_t)(sLiionBattInfo[i].u32Serial>>16));
                sprintf(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo+4,"%d",(uint16_t)(sLiionBattInfo[i].u32Serial));
                privateMibBase.liBattGroup.liBattTable[i].liBattSerialNoLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo);
                
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000001) == 0x00000001){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackOverVolt = 1;
                } else if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000002) == 0x00000002){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackOverVolt = 1;
                } else {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackOverVolt = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000004) == 0x00000004){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellOverVolt = 1;
                } else {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellOverVolt = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000010) == 0x00000010){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackLowVolt = 1;
                } else {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackLowVolt = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000020) == 0x00000020){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellLowVolt = 1;
                } else {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellLowVolt = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000040) == 0x00000040){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CharOverCurrent = 1;
                } else {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CharOverCurrent = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000080) == 0x00000080){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_DisCharOverCurrent = 1;
                } else {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_DisCharOverCurrent = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000100) == 0x00000100){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattHighTemp = 1;
                } else if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000200) == 0x00000200){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattHighTemp = 1;
                } else {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattHighTemp = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000400) == 0x00000400){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattLowTemp = 1;
                } else if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000800) == 0x00000800){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattLowTemp = 1;
                } else {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattLowTemp = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00001000) == 0x00001000){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_LowCapacity = 1;
                } else {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_LowCapacity = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00002000) == 0x00002000){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PCBHighTemp = 1;
                } else {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PCBHighTemp = 0;
                }
                if ((sLiionBattInfo[i].u32ProtectStatus & 0x00000001) == 0x00000001){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackOverVolt = 1;
                } else if ((sLiionBattInfo[i].u32ProtectStatus & 0x00000002) == 0x00000002){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackOverVolt = 1;
                } else {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackOverVolt = 0;
                }
                if ((sLiionBattInfo[i].u32ProtectStatus & 0x00000004) == 0x00000004){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellOverVolt = 1;
                } else {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellOverVolt = 0;
                }
                if ((sLiionBattInfo[i].u32ProtectStatus & 0x00000010) == 0x00000010){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackLowVolt = 1;
                } else {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackLowVolt = 0;
                }
                if ((sLiionBattInfo[i].u32ProtectStatus & 0x00000020) == 0x00000020){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellLowVolt = 1;
                } else {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellLowVolt = 0;
                }
                if ((sLiionBattInfo[i].u32ProtectStatus & 0x00000040) == 0x00000040){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_OverCurr = 1;
                } else if ((sLiionBattInfo[i].u32ProtectStatus & 0x00000080) == 0x00000080){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_OverCurr = 1;
                } else {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_OverCurr = 0;
                }
                if (((sLiionBattInfo[i].u32ProtectStatus & 0x00000100) == 0x00000100)||((sLiionBattInfo[i].u32ProtectStatus & 0x00000200) == 0x00000200)){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_high_temp = 1;
                } else {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_high_temp = 0;
                }
//                if ((sLiionBattInfo[i].u32ProtectStatus & 0x00000200) == 0x00000200){
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharHighTemp = 1;
//                } else {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharHighTemp = 0;
//                }
                if (((sLiionBattInfo[i].u32ProtectStatus & 0x00000400) == 0x00000400)||((sLiionBattInfo[i].u32ProtectStatus & 0x00000800) == 0x00000800)){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_low_temp = 1;
                } else {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_low_temp = 0;
                }
//                if ((sLiionBattInfo[i].u32ProtectStatus & 0x00000800) == 0x00000800){
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharLowTemp = 1;
//                } else {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharLowTemp = 0;
//                }
                if ((sLiionBattInfo[i].u32ProtectStatus & 0x00002000) == 0x00002000){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_ShortCircuit = 1;
                } else {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_ShortCircuit = 0;
                }
                if ((sLiionBattInfo[i].u32ProtectStatus & 0x00004000) == 0x00004000){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_VoltDiff = 1;
                } else {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_VoltDiff = 0;
                }
                if ((sLiionBattInfo[i].u32ProtectStatus & 0x00020000) == 0x00020000){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_reverse = 1;
                } else {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_reverse = 0;
                }
                
                //thanhcm3 fix --------------------------------------------------------------------------
                if(((sLiionBattInfo[i].u32FaultStatus & 0x00000010) == 0x00000010)          //TempSensor1
                   ||((sLiionBattInfo[i].u32FaultStatus & 0x00000020) == 0x00000020)        //TempSensor2
                   ||((sLiionBattInfo[i].u32FaultStatus & 0x00000040) == 0x00000040)        //TempSensor3
                   ||((sLiionBattInfo[i].u32FaultStatus & 0x00000080) == 0x00000080)        //TempSensor4
                   ||((sLiionBattInfo[i].u32FaultStatus & 0x00000100) == 0x00000100)        //BMSTemp
                   ||((sLiionBattInfo[i].u32FaultStatus & 0x00020000) == 0x00020000)){      //EnvTemp
                   privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_sensor = 1;
                }else{
                   privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_sensor = 0;
                  
                }
                //thanhcm3 fix --------------------------------------------------------------------------
                
                privateMibBase.liBattGroup.liBattTable[i].liBattType = sModbusManager.sLIBManager[i].u8LIBType;
                privateMibBase.liBattGroup.liBattTable[i].liBattModbusID = sModbusManager.sLIBManager[i].u32LIBAbsSlaveID;
            }
            break;
            case 8:// HUAWEI_A1
            {
                privateMibBase.liBattGroup.liBattTable[i].liBattIndex = i+1;
                privateMibBase.liBattGroup.liBattTable[i].liBattPackVolt = sLiionBattInfo[i].u32PackVolt;
                privateMibBase.liBattGroup.liBattTable[i].liBattPackCurr = sLiionBattInfo[i].u16PackCurr;
                privateMibBase.liBattGroup.liBattTable[i].liBattRemainCap = sLiionBattInfo[i].u32CapRemain;
                privateMibBase.liBattGroup.liBattTable[i].liBattAvrCellTemp = sLiionBattInfo[i].u16AverTempCell * 10;
                privateMibBase.liBattGroup.liBattTable[i].liBattAmbTemp = sLiionBattInfo[i].u16EnvTemp * 10;
                privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag = sLiionBattInfo[i].u32AlarmStatus;
                privateMibBase.liBattGroup.liBattTable[i].liBattProtectFlag = sLiionBattInfo[i].u32ProtectStatus;
                privateMibBase.liBattGroup.liBattTable[i].liBattFaultStat = sLiionBattInfo[i].u32FaultStatus;
                privateMibBase.liBattGroup.liBattTable[i].liBattSOC = sLiionBattInfo[i].u32SOC * 100;
                privateMibBase.liBattGroup.liBattTable[i].liBattSOH = sLiionBattInfo[i].u32SOH * 100;
                //thanhcm3 fix---------------------------------------------------------------------------------
                if(sLiionBattInfo[i].u16ModuleMode == 2)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattMode = 1;
                }else if(sLiionBattInfo[i].u16ModuleMode == 3)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattMode = 2;
                }else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattMode = 4;
                }
                //privateMibBase.liBattGroup.liBattTable[i].liBattMode = sLiionBattInfo[i].u16ModuleMode;
                //thanhcm3 fix---------------------------------------------------------------------------------
                privateMibBase.liBattGroup.liBattTable[i].liBattPeriod = sLiionBattInfo[i].u32DischargeTime;
//                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo, &sLiionBattInfo[i].u8mode[0]);
//                privateMibBase.liBattGroup.liBattTable[i].liBattSerialNoLen = 12;
                privateMibBase.liBattGroup.liBattTable[i].u32DSGCapacity = sLiionBattInfo[i].u32DSGCapacity * 100;  
                 for (j = 0; j < 16; j++)
                {
                    privateMibBase.liBattGroup.liBattTable[i].liBattCellVolt[j] = sLiionBattInfo[i].u16CellVolt[j];
                }

                for (j = 0; j < 16; j++)
                {
                    privateMibBase.liBattGroup.liBattTable[i].liBattCellTemp[j] = sLiionBattInfo[i].u16CellTemp[j] * 10;
                }

                privateMibBase.liBattGroup.liBattTable[i].u32DSGCapacity = sLiionBattInfo[i].u32CapRemain * 100;  
                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattModel, "ESM-48100A1");
                privateMibBase.liBattGroup.liBattTable[i].liBattModelLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattModel);
                sprintf(privateMibBase.liBattGroup.liBattTable[i].liBattSWVer,"%d",sLiionBattInfo[i].u16SoftwareVersion);
                privateMibBase.liBattGroup.liBattTable[i].liBattSWVerLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattSWVer);
                sprintf(privateMibBase.liBattGroup.liBattTable[i].liBattHWVer,"%d",sLiionBattInfo[i].u16HardwareVersion);
                privateMibBase.liBattGroup.liBattTable[i].liBattHWVerLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattHWVer);
                
//                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000020) == 0x00000020)
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellOverVolt = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellOverVolt = 0;
//                }
//                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000010) == 0x00000010)
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackOverVolt = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackOverVolt = 0;
//                }
//                
//                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000080) == 0x00000080)
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellLowVolt = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellLowVolt = 0;
//                }
//                
//                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000040) == 0x00000040)
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackLowVolt = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackLowVolt = 0;
//                }

//                if ((sLiionBattInfo[i].u32FaultStatus & 0x00000010) == 0x00000010)
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_DisCharOverCurrent = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_DisCharOverCurrent = 0;
//                }
//                
//                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00000001) == 0x00000001) || ((sLiionBattInfo[i].u32AlarmStatus & 0x00000004) == 0x00000004))
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattLowTemp = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattLowTemp = 0;
//                }
//                
//                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00000002) == 0x00000002) || ((sLiionBattInfo[i].u32AlarmStatus & 0x00000008) == 0x00000008))
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattHighTemp = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattHighTemp = 0;
//                }
                
//                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00200000) == 0x00200000)
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellOverVolt = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellOverVolt = 0;
//                }                
//                
//                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00100000) == 0x00100000)
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackOverVolt = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackOverVolt = 0;
//                }
//                
//                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00800000) == 0x00800000)
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellLowVolt = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellLowVolt = 0;
//                }                
//                
//                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00400000) == 0x00400000)
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackLowVolt = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackLowVolt = 0;
//                }
//                
//                if ((sLiionBattInfo[i].u32FaultStatus & 0x00000020) == 0x00000020)
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_OverCurr = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_OverCurr = 0;
//                }
//                
//                if ((sLiionBattInfo[i].u32ProtectStatus & 0x00080000) == 0x00080000)
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_ShortCircuit = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_ShortCircuit = 0;
//                }
//                
//                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00020000) == 0x00020000))
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CharHighTemp = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CharHighTemp = 0;
//                }
//                
//                
//                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00080000) == 0x00080000))
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharHighTemp = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharHighTemp = 0;
//                }
//                
//                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00010000) == 0x00010000))
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CharLowTemp = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CharLowTemp = 0;
//                }
//                
//                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00040000) == 0x00040000))
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharLowTemp = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharLowTemp = 0;
//                }
                
                privateMibBase.liBattGroup.liBattTable[i].liBattStatusCCL = 0;

                privateMibBase.liBattGroup.liBattTable[i].liBattType = sModbusManager.sLIBManager[i].u8LIBType;
                privateMibBase.liBattGroup.liBattTable[i].liBattModbusID = sModbusManager.sLIBManager[i].u32LIBAbsSlaveID;
            }
            break;
            case 14:// HUAFU_HF48100C
            case 6:// ZTT_2020
            {
                privateMibBase.liBattGroup.liBattTable[i].liBattIndex = i+1;
                privateMibBase.liBattGroup.liBattTable[i].liBattPackCurr = sLiionBattInfo[i].u16PackCurr;
                privateMibBase.liBattGroup.liBattTable[i].liBattPackVolt = sLiionBattInfo[i].u32PackVolt;
                privateMibBase.liBattGroup.liBattTable[i].liBattSOC = sLiionBattInfo[i].u32SOC * 100;
                privateMibBase.liBattGroup.liBattTable[i].liBattSOH = sLiionBattInfo[i].u32SOH * 100;
                privateMibBase.liBattGroup.liBattTable[i].liBattRemainCap = sLiionBattInfo[i].u32CapRemain;
                privateMibBase.liBattGroup.liBattTable[i].liBattPeriod = sLiionBattInfo[i].u32DischargeTime;
                privateMibBase.liBattGroup.liBattTable[i].liBattAvrCellTemp = sLiionBattInfo[i].u16AverTempCell;
                privateMibBase.liBattGroup.liBattTable[i].liBattAmbTemp = sLiionBattInfo[i].u16EnvTemp;
                privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag = sLiionBattInfo[i].u32AlarmStatus;
                privateMibBase.liBattGroup.liBattTable[i].liBattProtectFlag = sLiionBattInfo[i].u32ProtectStatus;
                privateMibBase.liBattGroup.liBattTable[i].liBattFaultStat = sLiionBattInfo[i].u32FaultStatus;    
                privateMibBase.liBattGroup.liBattTable[i].u32DSGCapacity = sLiionBattInfo[i].u32DSGCapacity;
                
                //thanhcm3 fix---------------------------------------------------------------------------------------
                if(sModbusManager.sLIBManager[i].u8LIBType == 14){
                  strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattModel,"HUAFU_HF48100C");
                  privateMibBase.liBattGroup.liBattTable[i].liBattModelLen = 14;
                }else if(sModbusManager.sLIBManager[i].u8LIBType == 6){
                  strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattModel,"ZTT50");
                  privateMibBase.liBattGroup.liBattTable[i].liBattModelLen = 5;
                }
                //thanhcm3 fix---------------------------------------------------------------------------------------
                
                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattHWVer, "unavailable");
                privateMibBase.liBattGroup.liBattTable[i].liBattHWVerLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattHWVer);
                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo, (const char*)&sLiionBattInfo[i].u8mode[0]);
                privateMibBase.liBattGroup.liBattTable[i].liBattSerialNoLen = 20;
                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattSWVer, (const char*)&sLiionBattInfo[i].u8swversion[0]);
                privateMibBase.liBattGroup.liBattTable[i].liBattSWVerLen = 20;
                
                for (j = 0; j < 16; j++)
                {
                    privateMibBase.liBattGroup.liBattTable[i].liBattCellVolt[j] = sLiionBattInfo[i].u16CellVolt[j];
                }

                for (j = 0; j < 8; j++)
                {
                    privateMibBase.liBattGroup.liBattTable[i].liBattCellTemp[j] = sLiionBattInfo[i].u16CellTemp[j];
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000001) == 0x00000001)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellOverVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellOverVolt = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000002) == 0x00000002)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellLowVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellLowVolt = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000004) == 0x00000004)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackOverVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackOverVolt = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000008) == 0x00000008)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackLowVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackLowVolt = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000010) == 0x00000010)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CharOverCurrent = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CharOverCurrent = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000020) == 0x00000020)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_DisCharOverCurrent = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_DisCharOverCurrent = 0;
                }
                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00000100) == 0x00000100) || ((sLiionBattInfo[i].u32AlarmStatus & 0x00000200) == 0x00000200))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattHighTemp = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattHighTemp = 0;
                }
                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00000400) == 0x00000400) || ((sLiionBattInfo[i].u32AlarmStatus & 0x00000800) == 0x00000800))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattLowTemp = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattLowTemp = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00001000) == 0x00001000) 
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_EnvHighTemp = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_EnvHighTemp = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00002000) == 0x00002000) 
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_EnvLowTemp = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_EnvLowTemp = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00004000) == 0x00004000){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PCBHighTemp = 1;  
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PCBHighTemp = 0;
                }
                
                 if ((sLiionBattInfo[i].u32AlarmStatus & 0x00008000) == 0x00008000) 
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_LowCapacity = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_LowCapacity = 0;
                }
                if ((sLiionBattInfo[i].u32ProtectStatus & 0x00000001) == 0x00000001)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellOverVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellOverVolt = 0;
                }  
                if ((sLiionBattInfo[i].u32ProtectStatus & 0x00000002) == 0x00000002)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellLowVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellLowVolt = 0;
                }
                if ((sLiionBattInfo[i].u32ProtectStatus & 0x00000004) == 0x00000004)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackOverVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackOverVolt = 0;
                }
                if ((sLiionBattInfo[i].u32ProtectStatus & 0x00000008) == 0x00000008)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackLowVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackLowVolt = 0;
                }
                if (((sLiionBattInfo[i].u32ProtectStatus & 0x00000010) == 0x00000010)||((sLiionBattInfo[i].u32ProtectStatus & 0x00000020) == 0x00000020))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_OverCurr = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_OverCurr = 0;
                }
                if ((sLiionBattInfo[i].u32ProtectStatus & 0x00000040) == 0x00000040)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_ShortCircuit = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_ShortCircuit = 0;
                }
                if (((sLiionBattInfo[i].u32ProtectStatus & 0x00000100) == 0x00000100)||((sLiionBattInfo[i].u32ProtectStatus & 0x00000200) == 0x00000200))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_high_temp = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_high_temp = 0;
                }
//                if (((sLiionBattInfo[i].u32ProtectStatus & 0x00000200) == 0x00000200))
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharHighTemp = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharHighTemp = 0;
//                }
                if (((sLiionBattInfo[i].u32ProtectStatus & 0x00000400) == 0x00000400)||((sLiionBattInfo[i].u32ProtectStatus & 0x00000800) == 0x00000800))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_low_temp = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_low_temp = 0;
                }
                //thanhcm3 fix=================================================================================
                if((sLiionBattInfo[i].u32ProtectStatus & 0x00001000) == 0x00001000){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_pcb_high_temp = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_pcb_high_temp = 0;
                }
                
                if((sLiionBattInfo[i].u32ProtectStatus & 0x00002000) == 0x00002000){    //protect_EnvHighTemp
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_env_high_temp = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_env_high_temp = 0;
                }
                if((sLiionBattInfo[i].u32ProtectStatus & 0x00004000) == 0x00004000){    //protect_EnvLowTemp
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_env_low_temp = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_env_low_temp = 0;
                }
                
                //thanhcm3 fix=================================================================================
//                if (((sLiionBattInfo[i].u32ProtectStatus & 0x00000800) == 0x00000800))
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharLowTemp = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharLowTemp = 0;
//                }
                if (((sLiionBattInfo[i].u32FaultStatus & 0x00000004) == 0x00000004))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_sensor = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_sensor = 0;
                }
                
                if((sLiionBattInfo[i].u32FaultStatus & 0x00000001) == 0x00000001){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_charging_mosfet = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_charging_mosfet = 0;
                }
                
                if((sLiionBattInfo[i].u32FaultStatus & 0x00000002) == 0x00000002){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_discharging_mosfet = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_discharging_mosfet = 0;
                }
                
                if((sLiionBattInfo[i].u32FaultStatus & 0x00000010) == 0x00000010){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_cell = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_cell = 0;
                }
                
                  
                if (((sLiionBattInfo[i].u32FaultStatus & 0x00000100) == 0x00000100))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattMode = 1;
                }
                else if (((sLiionBattInfo[i].u32FaultStatus & 0x00000200) == 0x00000200))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattMode = 2;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattMode = 3;
                }
                privateMibBase.liBattGroup.liBattTable[i].liBattType = sModbusManager.sLIBManager[i].u8LIBType;
                privateMibBase.liBattGroup.liBattTable[i].liBattModbusID = sModbusManager.sLIBManager[i].u32LIBAbsSlaveID;
            }
            break;
            case 9:// SAFT
            {
                privateMibBase.liBattGroup.liBattTable[i].liBattPackVolt = sLiionBattInfo[i].u32PackVolt * 10;
                privateMibBase.liBattGroup.liBattTable[i].liBattPackCurr = sLiionBattInfo[i].u32PackCurr * 10;
                privateMibBase.liBattGroup.liBattTable[i].liBattSOH = sLiionBattInfo[i].u32SOH * 100;
                privateMibBase.liBattGroup.liBattTable[i].liBattSOC = sLiionBattInfo[i].u32SOC * 100;
                privateMibBase.liBattGroup.liBattTable[i].liBattAmbTemp = sLiionBattInfo[i].u16AverTempCell * 10;
                privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.all = sLiionBattInfo[i].u32AlarmStatus;
              
                privateMibBase.liBattGroup.liBattTable[i].liBattType = sModbusManager.sLIBManager[i].u8LIBType;
                privateMibBase.liBattGroup.liBattTable[i].liBattModbusID = sModbusManager.sLIBManager[i].u32LIBAbsSlaveID;
            }
            break;
            case 10:// Narada75
            {
                privateMibBase.liBattGroup.liBattTable[i].liBattIndex = i+1;
                privateMibBase.liBattGroup.liBattTable[i].liBattPackVolt = sLiionBattInfo[i].u32PackVolt;
                privateMibBase.liBattGroup.liBattTable[i].liBattPackCurr = (sLiionBattInfo[i].u32PackCurr - 10000) * 10;
                privateMibBase.liBattGroup.liBattTable[i].liBattRemainCap = sLiionBattInfo[i].u32CapRemain * 10;
                privateMibBase.liBattGroup.liBattTable[i].liBattAvrCellTemp = sLiionBattInfo[i].u16AverTempCell - 400;
                privateMibBase.liBattGroup.liBattTable[i].liBattAmbTemp = sLiionBattInfo[i].u16EnvTemp - 400;
                privateMibBase.liBattGroup.liBattTable[i].liBattSOC = sLiionBattInfo[i].u32SOC;
                
                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattHWVer, "unavailable");
                privateMibBase.liBattGroup.liBattTable[i].liBattHWVerLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattHWVer);
                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo, "unavailable");
                privateMibBase.liBattGroup.liBattTable[i].liBattSerialNoLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo);
                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattSWVer, "unavailable");
                privateMibBase.liBattGroup.liBattTable[i].liBattSWVerLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattSWVer);
                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattModel, "unavailable");
                privateMibBase.liBattGroup.liBattTable[i].liBattModelLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattModel);
                
                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00000001) == 0x00000001))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellOverVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellOverVolt = 0;
                }
                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00000002) == 0x00000002))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellLowVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellLowVolt = 0;
                }
                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00000004) == 0x00000004))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackOverVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackOverVolt = 0;
                }
                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00000008) == 0x00000008))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackLowVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackLowVolt = 0;
                }
                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00000010) == 0x00000010))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CharOverCurrent = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CharOverCurrent = 0;
                }
                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00000020) == 0x00000020))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_DisCharOverCurrent = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_DisCharOverCurrent = 0;
                }
                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00000040) == 0x00000040))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattHighTemp = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattHighTemp = 0;
                }
                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00000080) == 0x00000080))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattLowTemp = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattLowTemp = 0;
                }
                
                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00000100) == 0x00000100))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_EnvHighTemp = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_EnvHighTemp = 0;
                }
                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00000200) == 0x00000200))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_EnvLowTemp = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_EnvLowTemp = 0;
                }
                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00000400) == 0x00000400))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PCBHighTemp = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PCBHighTemp = 0;
                }
                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00000800) == 0x00000800))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_LowCapacity = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_LowCapacity = 0;
                }
                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00001000) == 0x00001000))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_VoltDiff = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_VoltDiff = 0;
                }
                if (((sLiionBattInfo[i].u32ProtectStatus & 0x00000001) == 0x00000001))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellOverVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellOverVolt = 0;
                }
                if (((sLiionBattInfo[i].u32ProtectStatus & 0x00000002) == 0x00000002))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellLowVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellLowVolt = 0;
                }
                if (((sLiionBattInfo[i].u32ProtectStatus & 0x00000004) == 0x00000004))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackOverVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackOverVolt = 0;
                }
                if (((sLiionBattInfo[i].u32ProtectStatus & 0x00000008) == 0x00000008))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackLowVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackLowVolt = 0;
                }
                if (((sLiionBattInfo[i].u32ProtectStatus & 0x00000010) == 0x00000010))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_ShortCircuit = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_ShortCircuit = 0;
                }
                if (((sLiionBattInfo[i].u32ProtectStatus & 0x00000020) == 0x00000020))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_OverCurr = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_OverCurr = 0;
                }
                if (((sLiionBattInfo[i].u32ProtectStatus & 0x00000040) == 0x00000040)||((sLiionBattInfo[i].u32ProtectStatus & 0x00000100) == 0x00000100))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_high_temp = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_high_temp = 0;
                }
                if (((sLiionBattInfo[i].u32ProtectStatus & 0x00000080) == 0x00000080)||((sLiionBattInfo[i].u32ProtectStatus & 0x00000200) == 0x00000200))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_low_temp = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_low_temp = 0;
                }
//                if (((sLiionBattInfo[i].u32ProtectStatus & 0x00000100) == 0x00000100))
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharHighTemp = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharHighTemp = 0;
//                }
//                if (((sLiionBattInfo[i].u32ProtectStatus & 0x00000200) == 0x00000200))
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharLowTemp = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharLowTemp = 0;
//                }
                //thanhcm3 fix ---------------------------------------------------------------------------------
                if ((sLiionBattInfo[i].u32FaultStatus & 0x00000002) == 0x00000002){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_sensor = 1; 
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_sensor = 0;
                }
                //thanhcm3 fix ---------------------------------------------------------------------------------
                  
                if (((sLiionBattInfo[i].u32FaultStatus & 0x00000100) == 0x00000100))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattMode = 1;
                }
                else if (((sLiionBattInfo[i].u32FaultStatus & 0x00000200) == 0x00000200))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattMode = 2;
                }
                else 
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattMode = 0;
                }
                        
                privateMibBase.liBattGroup.liBattTable[i].liBattStatusCCL = 0;
                privateMibBase.liBattGroup.liBattTable[i].liBattType = sModbusManager.sLIBManager[i].u8LIBType;
                privateMibBase.liBattGroup.liBattTable[i].liBattModbusID = sModbusManager.sLIBManager[i].u32LIBAbsSlaveID;   
            }
            break;
            case 7:// ZTT50
            {
                privateMibBase.liBattGroup.liBattTable[i].liBattIndex = i+1;
                privateMibBase.liBattGroup.liBattTable[i].liBattPackVolt = (int32_t)(sLiionBattInfo[i].fPackVolt * 100);
                privateMibBase.liBattGroup.liBattTable[i].liBattPackCurr = (int32_t)(sLiionBattInfo[i].fPackCurr * 100);
                privateMibBase.liBattGroup.liBattTable[i].liBattRemainCap = (uint32_t)(sLiionBattInfo[i].fCapRemain * 10);
                privateMibBase.liBattGroup.liBattTable[i].liBattAvrCellTemp = (int32_t)(sLiionBattInfo[i].fAverTempCell * 10);
                privateMibBase.liBattGroup.liBattTable[i].liBattAmbTemp = (int32_t)(sLiionBattInfo[i].fEnvTemp * 10);
                privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag = (uint16_t)sLiionBattInfo[i].u32AlarmStatus;
                privateMibBase.liBattGroup.liBattTable[i].liBattProtectFlag = (uint16_t)sLiionBattInfo[i].u32ProtectStatus;
                privateMibBase.liBattGroup.liBattTable[i].liBattFaultStat = (uint16_t) sLiionBattInfo[i].u32FaultStatus;
                privateMibBase.liBattGroup.liBattTable[i].liBattSOC = (uint32_t)(sLiionBattInfo[i].fSOC * 100);
                privateMibBase.liBattGroup.liBattTable[i].liBattSOH = sLiionBattInfo[i].u32SOH;
                privateMibBase.liBattGroup.liBattTable[i].liBattPeriod = sLiionBattInfo[i].u32DischargeTime;

                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattModel, "LD4850-X");
                privateMibBase.liBattGroup.liBattTable[i].liBattModelLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattModel);

//                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattSWVer, "V10.10");
                for(int j=0;j<7;j++)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattSWVer[j] = sLiionBattInfo[i].u8swversion[j];
                }
                privateMibBase.liBattGroup.liBattTable[i].liBattSWVerLen = 6;

//                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattHWVer, "V01.10.11.02.00");
                for(int j=0;j<16;j++)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattHWVer[j] = sLiionBattInfo[i].u8hwversion[j];
                }
                privateMibBase.liBattGroup.liBattTable[i].liBattHWVerLen = 15;

//                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo, "2015111101180");
                for(int j=0;j<21;j++)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo[j] = sLiionBattInfo[i].u8mode[j];
                }
                privateMibBase.liBattGroup.liBattTable[i].liBattSerialNoLen = 20;
                
//                privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.all = (privateMibBase.liBattGroup.liBattTable[i].liBattProtectFlag << 16) |
//                                                                               privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag;
                
                //thanhcm3 fix -----------------------------------------------------------------------------------------------------------
                //0.
                if ((privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag&0x00000001)==0x00000001){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellOverVolt = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellOverVolt = 0;
                }
                //1.
                if ((privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag&0x00000002)==0x00000002){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellLowVolt  = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellLowVolt  = 0;
                }
                //2.
                if ((privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag&0x00000004)==0x00000004){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackOverVolt = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackOverVolt = 0;
                }
                //3.
                if ((privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag&0x00000008)==0x00000008){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackLowVolt = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackLowVolt = 0;
                }
                //4.
                if ((privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag&0x00000010)==0x00000010){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CharOverCurrent = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CharOverCurrent = 0;
                }
                //5.
                if ((privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag&0x00000020)==0x00000020){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_DisCharOverCurrent = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_DisCharOverCurrent = 0;
                }
                //6.
                if ((privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag&0x00000040)==0x00000040){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattHighTemp = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattHighTemp = 0;
                }
                //7.
                if ((privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag&0x00000080)==0x00000080){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattLowTemp = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattLowTemp = 0;
                }
                //8.
                if ((privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag&0x00000100)==0x00000100){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_EnvHighTemp = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_EnvHighTemp = 0;
                }
                //9.
                if ((privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag&0x00000200)==0x00000200){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_EnvLowTemp = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_EnvLowTemp = 0;
                }
                //10.
                if ((privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag&0x00000400)==0x00000400){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PCBHighTemp = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PCBHighTemp = 0;
                }
                //11.
                if ((privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag&0x00000800)==0x00000800){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_LowCapacity = 1; 
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_LowCapacity = 0;
                }
                //12.
                if ((privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag&0x00001000)==0x00001000){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_VoltDiff = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_VoltDiff = 0;
                }
                //16.
                if((privateMibBase.liBattGroup.liBattTable[i].liBattProtectFlag&0x00000001)==0x00000001){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellOverVolt = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellOverVolt = 0;
                }
                //17.
                if((privateMibBase.liBattGroup.liBattTable[i].liBattProtectFlag&0x00000002)==0x00000002){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellLowVolt = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellLowVolt = 0;
                }
                //18.
                if((privateMibBase.liBattGroup.liBattTable[i].liBattProtectFlag&0x00000004)==0x00000004){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackOverVolt = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackOverVolt = 0;
                }
                //19.
                if((privateMibBase.liBattGroup.liBattTable[i].liBattProtectFlag&0x00000008)==0x00000008){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackLowVolt = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackLowVolt = 0;
                }
                //20.
                if((privateMibBase.liBattGroup.liBattTable[i].liBattProtectFlag&0x00000010)==0x00000010){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_ShortCircuit = 1; 
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_ShortCircuit = 0;
                }
                //21.
                if((privateMibBase.liBattGroup.liBattTable[i].liBattProtectFlag&0x00000020)==0x00000020){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_OverCurr = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_OverCurr = 0;
                }
                //22.
                if(((privateMibBase.liBattGroup.liBattTable[i].liBattProtectFlag&0x00000040)==0x00000040)
                   ||((privateMibBase.liBattGroup.liBattTable[i].liBattProtectFlag&0x00000100)==0x00000100)){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_high_temp = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_high_temp = 0;
                }
                //23.
                if(((privateMibBase.liBattGroup.liBattTable[i].liBattProtectFlag&0x00000080)==0x00000080)
                  ||((privateMibBase.liBattGroup.liBattTable[i].liBattProtectFlag&0x00000200)==0x00000200)){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_low_temp = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_low_temp = 0;
                }
                //28.
                if((privateMibBase.liBattGroup.liBattTable[i].liBattFaultStat&0x00000002)==0x00000002){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_sensor = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_sensor = 0;
                }
                
                //thanhcm3 fix -----------------------------------------------------------------------------------------------------------
                privateMibBase.liBattGroup.liBattTable[i].liBattStatusCCL = 0;

                privateMibBase.liBattGroup.liBattTable[i].liBattType = sModbusManager.sLIBManager[i].u8LIBType;
                privateMibBase.liBattGroup.liBattTable[i].liBattModbusID = sModbusManager.sLIBManager[i].u32LIBAbsSlaveID;
            }
            break;
            case 11:// EVE
            {
                privateMibBase.liBattGroup.liBattTable[i].liBattIndex = i+1;
                privateMibBase.liBattGroup.liBattTable[i].liBattPackCurr = sLiionBattInfo[i].u16PackCurr;
                privateMibBase.liBattGroup.liBattTable[i].liBattPackVolt = sLiionBattInfo[i].u32PackVolt;
                privateMibBase.liBattGroup.liBattTable[i].liBattSOC = sLiionBattInfo[i].u32SOC * 100;
                privateMibBase.liBattGroup.liBattTable[i].liBattSOH = sLiionBattInfo[i].u32SOH * 100;
                privateMibBase.liBattGroup.liBattTable[i].liBattRemainCap = sLiionBattInfo[i].u32CapRemain;
                privateMibBase.liBattGroup.liBattTable[i].liBattPeriod = sLiionBattInfo[i].u32DischargeTime;
                privateMibBase.liBattGroup.liBattTable[i].liBattAvrCellTemp = sLiionBattInfo[i].u16AverTempCell;
                privateMibBase.liBattGroup.liBattTable[i].liBattAmbTemp = sLiionBattInfo[i].u16EnvTemp;
                privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag = sLiionBattInfo[i].u32AlarmStatus;
                privateMibBase.liBattGroup.liBattTable[i].liBattProtectFlag = sLiionBattInfo[i].u32ProtectStatus;
                privateMibBase.liBattGroup.liBattTable[i].liBattFaultStat = sLiionBattInfo[i].u32FaultStatus;    
                privateMibBase.liBattGroup.liBattTable[i].u32DSGCapacity = sLiionBattInfo[i].u32DSGCapacity;  
                
                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattModel,"EVE4850A");
//                privateMibBase.liBattGroup.liBattTable[i].liBattModelLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattModel);
                privateMibBase.liBattGroup.liBattTable[i].liBattModelLen = 8;
                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattHWVer, "unavailable");
                privateMibBase.liBattGroup.liBattTable[i].liBattHWVerLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattHWVer);
                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo,(const char*) &sLiionBattInfo[i].u8mode[0]);
//                privateMibBase.liBattGroup.liBattTable[i].liBattSerialNoLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo);
                privateMibBase.liBattGroup.liBattTable[i].liBattSerialNoLen = 20;
                strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattSWVer,(const char*) &sLiionBattInfo[i].u8swversion[0]);
                privateMibBase.liBattGroup.liBattTable[i].liBattSWVerLen = 20;
                
                for (j = 0; j < 16; j++)
                {
                    privateMibBase.liBattGroup.liBattTable[i].liBattCellVolt[j] = sLiionBattInfo[i].u16CellVolt[j];
                }

                for (j = 0; j < 8; j++)
                {
                    privateMibBase.liBattGroup.liBattTable[i].liBattCellTemp[j] = sLiionBattInfo[i].u16CellTemp[j];
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000001) == 0x00000001)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellOverVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellOverVolt = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000002) == 0x00000002)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellLowVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CellLowVolt = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000004) == 0x00000004)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackOverVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackOverVolt = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000008) == 0x00000008)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackLowVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PackLowVolt = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000010) == 0x00000010)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CharOverCurrent = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_CharOverCurrent = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00000020) == 0x00000020)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_DisCharOverCurrent = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_DisCharOverCurrent = 0;
                }
                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00000100) == 0x00000100) || ((sLiionBattInfo[i].u32AlarmStatus & 0x00000200) == 0x00000200))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattHighTemp = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattHighTemp = 0;
                }
                if (((sLiionBattInfo[i].u32AlarmStatus & 0x00000400) == 0x00000400) || ((sLiionBattInfo[i].u32AlarmStatus & 0x00000800) == 0x00000800))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattLowTemp = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_BattLowTemp = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00001000) == 0x00001000) 
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_EnvHighTemp = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_EnvHighTemp = 0;
                }
                if ((sLiionBattInfo[i].u32AlarmStatus & 0x00002000) == 0x00002000) 
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_EnvLowTemp = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_EnvLowTemp = 0;
                }
                
                if((sLiionBattInfo[i].u32AlarmStatus & 0x00004000) == 0x00004000){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PCBHighTemp = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_PCBHighTemp = 0;
                }
                
                
                 if ((sLiionBattInfo[i].u32AlarmStatus & 0x00008000) == 0x00008000) 
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_LowCapacity = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.alarm_LowCapacity = 0;
                }
                if ((sLiionBattInfo[i].u32ProtectStatus & 0x00000001) == 0x00000001)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellOverVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellOverVolt = 0;
                }  
                if ((sLiionBattInfo[i].u32ProtectStatus & 0x00000002) == 0x00000002)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellLowVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_CellLowVolt = 0;
                }
                if ((sLiionBattInfo[i].u32ProtectStatus & 0x00000004) == 0x00000004)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackOverVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackOverVolt = 0;
                }
                if ((sLiionBattInfo[i].u32ProtectStatus & 0x00000008) == 0x00000008)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackLowVolt = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_PackLowVolt = 0;
                }
                if (((sLiionBattInfo[i].u32ProtectStatus & 0x00000010) == 0x00000010)||((sLiionBattInfo[i].u32ProtectStatus & 0x00000020) == 0x00000020))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_OverCurr = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_OverCurr = 0;
                }
                if ((sLiionBattInfo[i].u32ProtectStatus & 0x00000040) == 0x00000040)
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_ShortCircuit = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_ShortCircuit = 0;
                }
                if (((sLiionBattInfo[i].u32ProtectStatus & 0x00000100) == 0x00000100)||((sLiionBattInfo[i].u32ProtectStatus & 0x00000200) == 0x00000200))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_high_temp = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_high_temp = 0;
                }
//                if (((sLiionBattInfo[i].u32ProtectStatus & 0x00000200) == 0x00000200))
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharHighTemp = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharHighTemp = 0;
//                }
                if (((sLiionBattInfo[i].u32ProtectStatus & 0x00000400) == 0x00000400)||(((sLiionBattInfo[i].u32ProtectStatus & 0x00000800) == 0x00000800)))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_low_temp = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_char_or_dischar_low_temp = 0;
                }
                
//                if (((sLiionBattInfo[i].u32ProtectStatus & 0x00000800) == 0x00000800))
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharLowTemp = 1;
//                }
//                else
//                {
//                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_DisCharLowTemp = 0;
//                }
                if((sLiionBattInfo[i].u32ProtectStatus & 0x00001000) == 0x00001000){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_pcb_high_temp = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_pcb_high_temp = 0;
                }
                
                if((sLiionBattInfo[i].u32ProtectStatus & 0x00002000) == 0x00002000){//protect_EnvHighTemp
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_env_high_temp = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_env_high_temp = 0;
                }
                
                if((sLiionBattInfo[i].u32ProtectStatus & 0x00004000) == 0x00004000){//protect_EnvLowTemp
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_env_low_temp = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.protect_env_low_temp = 0;
                }
                
                if(((sLiionBattInfo[i].u32FaultStatus & 0x00000001) == 0x00000001)){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_charging_mosfet = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_charging_mosfet = 0;
                }
                
                if(((sLiionBattInfo[i].u32FaultStatus & 0x00000002) == 0x00000002)){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_discharging_mosfet =1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_discharging_mosfet =0;
                }
                
                if(((sLiionBattInfo[i].u32FaultStatus & 0x00000010) == 0x00000010)){
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_cell = 1;
                }else{
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_cell = 0;
                }
                
                if (((sLiionBattInfo[i].u32FaultStatus & 0x00000004) == 0x00000004))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_sensor = 1;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.bit.break_sensor = 0;
                }
                
                if (((sLiionBattInfo[i].u32FaultStatus & 0x00000100) == 0x00000100))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattMode = 1;
                }
                else if (((sLiionBattInfo[i].u32FaultStatus & 0x00000200) == 0x00000200))
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattMode = 2;
                }
                else
                {
                  privateMibBase.liBattGroup.liBattTable[i].liBattMode = 3;
                }
                
//                privateMibBase.liBattGroup.liBattTable[i].liBattStatusCCL = 0;
                privateMibBase.liBattGroup.liBattTable[i].liBattType = sModbusManager.sLIBManager[i].u8LIBType;
                privateMibBase.liBattGroup.liBattTable[i].liBattModbusID = sModbusManager.sLIBManager[i].u32LIBAbsSlaveID;
            }
            break;
            };


        }
        else
        {
            privateMibBase.liBattGroup.liBattTable[i].liBattIndex = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattPackVolt = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattPackCurr = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattRemainCap = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattAvrCellTemp = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattAmbTemp = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattProtectFlag = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattFaultStat = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattMode = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattSOC = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattSOH = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattPeriod = 0;

            for (j = 0; j < 16; j++)
            {
                privateMibBase.liBattGroup.liBattTable[i].liBattCellVolt[j] = 0;
            }

            for (j = 0; j < 16; j++)
            {
                privateMibBase.liBattGroup.liBattTable[i].liBattCellTemp[j] = 0;
            }

            strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattModel, "unavailable");
            privateMibBase.liBattGroup.liBattTable[i].liBattModelLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattModel);

            strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattSWVer, "unavailable");
            privateMibBase.liBattGroup.liBattTable[i].liBattSWVerLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattSWVer);

            strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattHWVer, "unavailable");
            privateMibBase.liBattGroup.liBattTable[i].liBattHWVerLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattHWVer);

            strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo, "unavailable");
            privateMibBase.liBattGroup.liBattTable[i].liBattSerialNoLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo);
            
            privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.all = (privateMibBase.liBattGroup.liBattTable[i].liBattProtectFlag << 16) |
                                                                               privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag;
            privateMibBase.liBattGroup.liBattTable[i].liBattStatusCCL = 0;

            privateMibBase.liBattGroup.liBattTable[i].liBattType = sModbusManager.sLIBManager[i].u8LIBType;
            privateMibBase.liBattGroup.liBattTable[i].liBattModbusID = sModbusManager.sLIBManager[i].u32LIBAbsSlaveID;
            
            //=== M1
            privateMibBase.liBattGroup.liBattTable[i].u32ConverterState = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32ErrCode = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32BalanceStatus = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32MosfetMode = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32Mcu2McuErr = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32CHGCapacity = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32DSGCapacity = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32Efficiency = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32NumberOfCan = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32PackHighVolt_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32BattHighVolt_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32CellHighVolt_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32BattLowVolt_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32CellLowVolt_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32CharHighCurr_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32DisCharHighCurr_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32CharHighTemp_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32DisCharHighTemp_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32CharLowTemp_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32DisCharLowtemp_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32LowCap_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32BMSHighTemp_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32BMSLowTemp_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32PackOverVolt_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32BattOverVolt_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32CellOverVolt_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32BattUnderVolt_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32CellUnderVolt_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32CharOverCurr_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32DisCharOverCurr_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32CharOverTemp_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32DisCharOverTemp_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32CharUnderTemp_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32DisCharUnderTemp_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32UnderCap_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32BMSOverTemp_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32BMSUnderTemp_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32DifferentVolt_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32VoltBalance = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32DeltaVoltBalance = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32DisCharCurrLimit = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32CharCurrLimit = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32VoltDisCharRef = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32VoltDisCharCMD = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32SystemByte = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32KeyTime = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32IniMaxim = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32EnableKCS = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32EnableLock = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32EnableLock = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32ADCPackVolt = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32ADCBattVolt = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32ADCPackCurr = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32ADCBattCurr = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32SOCMaxim = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32SOHMaxim = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32FullCapRepMaxim = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32VoltMaxim = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32CurrMaxim = 0; 
            privateMibBase.liBattGroup.liBattTable[i].u32IKalamn = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32SOCKalamn = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32VpriKalamn = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32VmesKalamn = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32CapKalamn = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32InternalR0Kalamn = 0;
            privateMibBase.liBattGroup.liBattTable[i].u16_liionCapInit    = 0;

        }
    }
    //clean---------------------------------------------------------------
    for(i = privateMibBase.liBattGroup.liBattInstalledPack;i<16;i++){
            privateMibBase.liBattGroup.liBattTable[i].liBattIndex = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattPackVolt = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattPackCurr = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattRemainCap = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattAvrCellTemp = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattAmbTemp = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattProtectFlag = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattFaultStat = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattMode = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattSOC = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattSOH = 0;
            privateMibBase.liBattGroup.liBattTable[i].liBattPeriod = 0;

            for (j = 0; j < 16; j++)
            {
                privateMibBase.liBattGroup.liBattTable[i].liBattCellVolt[j] = 0;
            }

            for (j = 0; j < 16; j++)
            {
                privateMibBase.liBattGroup.liBattTable[i].liBattCellTemp[j] = 0;
            }

            strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattModel, "unavailable");
            privateMibBase.liBattGroup.liBattTable[i].liBattModelLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattModel);

            strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattSWVer, "unavailable");
            privateMibBase.liBattGroup.liBattTable[i].liBattSWVerLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattSWVer);

            strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattHWVer, "unavailable");
            privateMibBase.liBattGroup.liBattTable[i].liBattHWVerLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattHWVer);

            strcpy(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo, "unavailable");
            privateMibBase.liBattGroup.liBattTable[i].liBattSerialNoLen = strlen(privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo);
            
            privateMibBase.liBattGroup.liBattTable[i].liBattAlarmStatus.all = (privateMibBase.liBattGroup.liBattTable[i].liBattProtectFlag << 16) |
                                                                               privateMibBase.liBattGroup.liBattTable[i].liBattWarningFlag;
            privateMibBase.liBattGroup.liBattTable[i].liBattStatusCCL = 0;

            privateMibBase.liBattGroup.liBattTable[i].liBattType = sModbusManager.sLIBManager[i].u8LIBType;
            privateMibBase.liBattGroup.liBattTable[i].liBattModbusID = sModbusManager.sLIBManager[i].u32LIBAbsSlaveID;
            
            //=== M1
            privateMibBase.liBattGroup.liBattTable[i].u32ConverterState = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32ErrCode = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32BalanceStatus = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32MosfetMode = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32Mcu2McuErr = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32CHGCapacity = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32DSGCapacity = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32Efficiency = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32NumberOfCan = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32PackHighVolt_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32BattHighVolt_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32CellHighVolt_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32BattLowVolt_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32CellLowVolt_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32CharHighCurr_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32DisCharHighCurr_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32CharHighTemp_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32DisCharHighTemp_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32CharLowTemp_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32DisCharLowtemp_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32LowCap_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32BMSHighTemp_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32BMSLowTemp_A = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32PackOverVolt_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32BattOverVolt_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32CellOverVolt_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32BattUnderVolt_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32CellUnderVolt_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32CharOverCurr_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32DisCharOverCurr_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32CharOverTemp_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32DisCharOverTemp_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32CharUnderTemp_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32DisCharUnderTemp_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32UnderCap_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32BMSOverTemp_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32BMSUnderTemp_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32DifferentVolt_P = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32VoltBalance = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32DeltaVoltBalance = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32DisCharCurrLimit = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32CharCurrLimit = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32VoltDisCharRef = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32VoltDisCharCMD = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32SystemByte = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32KeyTime = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32IniMaxim = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32EnableKCS = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32EnableLock = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32EnableLock = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32ADCPackVolt = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32ADCBattVolt = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32ADCPackCurr = 0;
            privateMibBase.liBattGroup.liBattTable[i].i32ADCBattCurr = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32SOCMaxim = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32SOHMaxim = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32FullCapRepMaxim = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32VoltMaxim = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32CurrMaxim = 0; 
            privateMibBase.liBattGroup.liBattTable[i].u32IKalamn = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32SOCKalamn = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32VpriKalamn = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32VmesKalamn = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32CapKalamn = 0;
            privateMibBase.liBattGroup.liBattTable[i].u32InternalR0Kalamn = 0;
            privateMibBase.liBattGroup.liBattTable[i].u16_liionCapInit    = 0;
    }
}
void UpdateGENInfo (void)
{
  uint32_t j,i=0;
  
  privateMibBase.genGroup.genInstalledGen = sModbusManager.u8NumberOfGen;
  privateMibBase.connAlarmGroup.alarmGenConnect = 0;
  privateMibBase.mainAlarmGroup.alarmGen = 0;
  privateMibBase.genGroup.genActiveGen = privateMibBase.genGroup.genInstalledGen;
  for (i = 0; i < privateMibBase.genGroup.genInstalledGen; i++)
  {
      privateMibBase.mainAlarmGroup.alarmGENPack[i] = 0;
      if (sGenInfo[i].u8ErrorFlag == 1)
      {
        if(cntDisConnectGEN[i] > 2){ // mat ket noi 3 lan moi bao len
          cntDisConnectGEN[i] = 3;
          privateMibBase.genGroup.genActiveGen--;
          if (privateMibBase.genGroup.genActiveGen <= 0)
          {
            privateMibBase.genGroup.genActiveGen = 0;
          }
          privateMibBase.connAlarmGroup.alarmGenConnect |= (sGenInfo[i].u8ErrorFlag << i);
          privateMibBase.genGroup.genTable[i].genStatusStatus = 170;
        }
      }
      else
      {
        cntDisConnectGEN[i] = 0;
//          privateMibBase.genGroup.genTable[i].genStatusStatus = 255;
            if(privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.all != 0)
            {
                privateMibBase.genGroup.genTable[i].genStatusStatus = 165;
            }
            else
            {
                privateMibBase.genGroup.genTable[i].genStatusStatus = 255;
            }
      }

      if (privateMibBase.genGroup.genTable[i].genStatusStatus != 170)
      {
          if (privateMibBase.genGroup.genTable[i].genStatusStatus == 165)
          {
              privateMibBase.mainAlarmGroup.alarmGENPack[i] = privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.all;
          }

          switch(sModbusManager.sGenManager[i].u8GenType)
          {
          case 1:// KUBOTA
          {
              privateMibBase.genGroup.genTable[i].genStatusID = i+1;
              privateMibBase.genGroup.genTable[i].genStatusFrequency = sGenInfo[i].u32GenFrequency;
              privateMibBase.genGroup.genTable[i].genStatusEngineSpeed = sGenInfo[i].u32EngineSpeed;
              privateMibBase.genGroup.genTable[i].genStatusOilPressure = sGenInfo[i].u32OilPressure;
              privateMibBase.genGroup.genTable[i].genStatusCoolantTemp = sGenInfo[i].u32CoolantTemp * 10;
              privateMibBase.genGroup.genTable[i].genStatusFuelLevel = sGenInfo[i].u32FuelLevel;
              privateMibBase.genGroup.genTable[i].genStatusBattVolt = sGenInfo[i].u32BattVolt;
              
              privateMibBase.genGroup.genTable[i].genStatusLNVolt1 = sGenInfo[i].u32LNVolt1 * 100;
              privateMibBase.genGroup.genTable[i].genStatusLNVolt2 = sGenInfo[i].u32LNVolt2 * 100;
              privateMibBase.genGroup.genTable[i].genStatusLNVolt3 = sGenInfo[i].u32LNVolt3 * 100;
              
              privateMibBase.genGroup.genTable[i].genStatusLNCurr1 = sGenInfo[i].u32LNCurr1 * 10;
              privateMibBase.genGroup.genTable[i].genStatusLNCurr2 = sGenInfo[i].u32LNCurr2 * 10;
              privateMibBase.genGroup.genTable[i].genStatusLNCurr3 = sGenInfo[i].u32LNCurr3 * 10;
              
              privateMibBase.genGroup.genTable[i].genStatusLNSPower1 = sGenInfo[i].u32LNSPower1;
              privateMibBase.genGroup.genTable[i].genStatusLNSPower2 = sGenInfo[i].u32LNSPower2;
              privateMibBase.genGroup.genTable[i].genStatusLNSPower3 = sGenInfo[i].u32LNSPower3;
              
              privateMibBase.genGroup.genTable[i].genStatusLNPower1 = sGenInfo[i].u32LNPower1;
              privateMibBase.genGroup.genTable[i].genStatusLNPower2 = sGenInfo[i].u32LNPower2;
              privateMibBase.genGroup.genTable[i].genStatusLNPower3 = sGenInfo[i].u32LNPower3;
              
              privateMibBase.genGroup.genTable[i].genStatusWorkingHour = sGenInfo[i].u32EngineWorkingHour;
              privateMibBase.genGroup.genTable[i].genStatusWorkingMin = sGenInfo[i].u32EngineWorkingMin;
              privateMibBase.genGroup.genTable[i].genStatusActiveEnergyLow = sGenInfo[i].u32ActiveEnergyLow;
              privateMibBase.genGroup.genTable[i].genStatusActiveEnergyHigh = sGenInfo[i].u32ActiveEnergyHigh;
              
              sGenInfo[i].u32AlarmStatus = (sGenInfo[i].u16Error2Status << 16) | (sGenInfo[i].u16Error1Status);
              privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.all = sGenInfo[i].u32AlarmStatus;
              privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.bit.err_LowFuel = 0;
              if(sGenInfo[i].u16ErrorBattVolt == 1) 
              {
                privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.bit.err_HiBattVol = 1;
                privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.bit.err_LowBattVol = 0;
                
              }
              else if(sGenInfo[i].u16ErrorBattVolt == 2) 
              {
                privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.bit.err_HiBattVol = 0;
                privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.bit.err_LowBattVol = 1;          
              }
              else 
              {
                privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.bit.err_HiBattVol = 0;
                privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.bit.err_LowBattVol = 0; 
              }       
              
              if((sGenInfo[i].u16StatusBits1 & 0x02) == 0x02)
              {
                privateMibBase.genGroup.genTable[i].genStopMode = 1;
                privateMibBase.genGroup.genTable[i].genAutoMode = 0;
                privateMibBase.genGroup.genTable[i].genManualMode = 0;
              }
              else if((sGenInfo[i].u16StatusBits1 & 0x04) == 0x04)
              {
                privateMibBase.genGroup.genTable[i].genStopMode = 0;
                privateMibBase.genGroup.genTable[i].genAutoMode = 0;
                privateMibBase.genGroup.genTable[i].genManualMode = 1;
              }
              else if((sGenInfo[i].u16StatusBits2 & 0x08) == 0x08)
              {
                privateMibBase.genGroup.genTable[i].genStopMode = 0;
                privateMibBase.genGroup.genTable[i].genAutoMode = 1;
                privateMibBase.genGroup.genTable[i].genManualMode = 0;
              }
              else 
              {
                privateMibBase.genGroup.genTable[i].genStopMode = 0;
                privateMibBase.genGroup.genTable[i].genAutoMode = 0;
                privateMibBase.genGroup.genTable[i].genManualMode = 0;             
              }
              
              if (sGenInfo[i].u32GenFrequency == 0)
                privateMibBase.genGroup.genTable[i].genStartMode = 0;
              else 
                privateMibBase.genGroup.genTable[i].genStartMode = 1;
              
              privateMibBase.genGroup.genTable[i].genStatusType = sModbusManager.sGenManager[i].u8GenType;
              privateMibBase.genGroup.genTable[i].genStatusModbusID = sModbusManager.sGenManager[i].u32GenAbsSlaveID;
          }
          break;
          case 2:// BE142
          {
            
              privateMibBase.genGroup.genTable[i].genStatusID = i+1;
              
              privateMibBase.genGroup.genTable[i].genStatusFrequency = sGenInfo[i].u32GenFrequency;
              privateMibBase.genGroup.genTable[i].genStatusEngineSpeed = sGenInfo[i].u32EngineSpeed;
              privateMibBase.genGroup.genTable[i].genStatusOilPressure = sGenInfo[i].u32OilPressure;
              privateMibBase.genGroup.genTable[i].genStatusCoolantTemp = sGenInfo[i].u32CoolantTemp * 10;
              privateMibBase.genGroup.genTable[i].genStatusFuelLevel = sGenInfo[i].u32FuelLevel;
              privateMibBase.genGroup.genTable[i].genStatusBattVolt = sGenInfo[i].u32BattVolt;
              
              privateMibBase.genGroup.genTable[i].genStatusLNVolt1 = sGenInfo[i].u32LNVolt1 * 100;
              privateMibBase.genGroup.genTable[i].genStatusLNVolt2 = sGenInfo[i].u32LNVolt2 * 100;
              privateMibBase.genGroup.genTable[i].genStatusLNVolt3 = sGenInfo[i].u32LNVolt3 * 100;
              
              privateMibBase.genGroup.genTable[i].genStatusLNCurr1 = sGenInfo[i].u32LNCurr1 * 100;
              privateMibBase.genGroup.genTable[i].genStatusLNCurr2 = sGenInfo[i].u32LNCurr2 * 100;
              privateMibBase.genGroup.genTable[i].genStatusLNCurr3 = sGenInfo[i].u32LNCurr3 * 100;
//              
//              privateMibBase.genGroup.genTable[i].genStatusLNPower1 = sGenInfo[i].u32LNPower1;
//              privateMibBase.genGroup.genTable[i].genStatusLNPower2 = sGenInfo[i].u32LNPower2;
//              privateMibBase.genGroup.genTable[i].genStatusLNPower3 = sGenInfo[i].u32LNPower3;
//              
              privateMibBase.genGroup.genTable[i].genStatusWorkingHour = sGenInfo[i].u32EngineWorkingHour;
//              privateMibBase.genGroup.genTable[i].genStatusWorkingMin = sGenInfo[i].u32EngineWorkingMin;
//              privateMibBase.genGroup.genTable[i].genStatusActiveEnergyLow = sGenInfo[i].u32ActiveEnergyLow;
//              privateMibBase.genGroup.genTable[i].genStatusActiveEnergyHigh = sGenInfo[i].u32ActiveEnergyHigh;
//              
//              sGenInfo[i].u32AlarmStatus = (sGenInfo[i].u16Error2Status << 16) | (sGenInfo[i].u16Error1Status);
//             
              privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.all = sGenInfo[i].u32AlarmStatus;
              
              privateMibBase.genGroup.genTable[i].genStatusType = sModbusManager.sGenManager[i].u8GenType;
              privateMibBase.genGroup.genTable[i].genStatusModbusID = sModbusManager.sGenManager[i].u32GenAbsSlaveID;
          }
          break;
          case 3:// DEEPSEA
          {
              privateMibBase.genGroup.genTable[i].genStatusID = i+1;
              privateMibBase.genGroup.genTable[i].genStatusFrequency = sGenInfo[i].u32GenFrequency;
              privateMibBase.genGroup.genTable[i].genStatusEngineSpeed = sGenInfo[i].u32EngineSpeed;
              privateMibBase.genGroup.genTable[i].genStatusOilPressure = sGenInfo[i].u32OilPressure / 10;
              privateMibBase.genGroup.genTable[i].genStatusCoolantTemp = sGenInfo[i].u32CoolantTemp * 10;
              privateMibBase.genGroup.genTable[i].genStatusFuelLevel = sGenInfo[i].u32FuelLevel;
              privateMibBase.genGroup.genTable[i].genStatusBattVolt = sGenInfo[i].u32BattVolt;
              
              privateMibBase.genGroup.genTable[i].genStatusLNVolt1 = sGenInfo[i].u32LNVolt1 * 10;
              privateMibBase.genGroup.genTable[i].genStatusLNVolt2 = sGenInfo[i].u32LNVolt2 * 10;
              privateMibBase.genGroup.genTable[i].genStatusLNVolt3 = sGenInfo[i].u32LNVolt3 * 10;
              
              privateMibBase.genGroup.genTable[i].genStatusLNCurr1 = sGenInfo[i].u32LNCurr1 * 10;
              privateMibBase.genGroup.genTable[i].genStatusLNCurr2 = sGenInfo[i].u32LNCurr2 * 10;
              privateMibBase.genGroup.genTable[i].genStatusLNCurr3 = sGenInfo[i].u32LNCurr3 * 10;
              
              privateMibBase.genGroup.genTable[i].genStatusLNPower1 = sGenInfo[i].u32LNPower1;
              privateMibBase.genGroup.genTable[i].genStatusLNPower2 = sGenInfo[i].u32LNPower2;
              privateMibBase.genGroup.genTable[i].genStatusLNPower3 = sGenInfo[i].u32LNPower3;
              
              privateMibBase.genGroup.genTable[i].genStatusWorkingHour = sGenInfo[i].u32EngineWorkingHour;
              privateMibBase.genGroup.genTable[i].genStatusWorkingMin = sGenInfo[i].u32EngineWorkingMin;
//              privateMibBase.genGroup.genTable[i].genStatusActiveEnergyLow = sGenInfo[i].u32ActiveEnergyLow;
//              privateMibBase.genGroup.genTable[i].genStatusActiveEnergyHigh = sGenInfo[i].u32ActiveEnergyHigh;
              
//              sGenInfo[i].u32AlarmStatus = (sGenInfo[i].u16Error2Status << 16) | (sGenInfo[i].u16Error1Status);
              privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.all = sGenInfo[i].u32AlarmStatus;
              
              if(sGenInfo[i].u16StatusBits1 == 0)
              {
                privateMibBase.genGroup.genTable[i].genStopMode = 1;
                privateMibBase.genGroup.genTable[i].genAutoMode = 0;
                privateMibBase.genGroup.genTable[i].genManualMode = 0;
              }
              else if(sGenInfo[i].u16StatusBits1 == 2)
              {
                privateMibBase.genGroup.genTable[i].genStopMode = 0;
                privateMibBase.genGroup.genTable[i].genAutoMode = 0;
                privateMibBase.genGroup.genTable[i].genManualMode = 1;
              }
              else if(sGenInfo[i].u16StatusBits1 == 1)
              {
                privateMibBase.genGroup.genTable[i].genStopMode = 0;
                privateMibBase.genGroup.genTable[i].genAutoMode = 1;
                privateMibBase.genGroup.genTable[i].genManualMode = 0;
              }
              else 
              {
                privateMibBase.genGroup.genTable[i].genStopMode = 0;
                privateMibBase.genGroup.genTable[i].genAutoMode = 0;
                privateMibBase.genGroup.genTable[i].genManualMode = 0;             
              }
              
              if (sGenInfo[i].u32GenFrequency == 0)
                privateMibBase.genGroup.genTable[i].genStartMode = 0;
              else 
                privateMibBase.genGroup.genTable[i].genStartMode = 1;              
              
              privateMibBase.genGroup.genTable[i].genStatusType = sModbusManager.sGenManager[i].u8GenType;
              privateMibBase.genGroup.genTable[i].genStatusModbusID = sModbusManager.sGenManager[i].u32GenAbsSlaveID;
          }
          break;
          case 4:// LR2057
          {
              privateMibBase.genGroup.genTable[i].genStatusID = i+1;
              privateMibBase.genGroup.genTable[i].genStatusLNVolt1 = sGenInfo[i].u32LNVolt1 * 100;
              privateMibBase.genGroup.genTable[i].genStatusLNVolt2 = sGenInfo[i].u32LNVolt2 * 100;
              privateMibBase.genGroup.genTable[i].genStatusLNVolt3 = sGenInfo[i].u32LNVolt3 * 100;
              
              privateMibBase.genGroup.genTable[i].genStatusLNCurr1 = sGenInfo[i].u32LNCurr1 * 100;
              privateMibBase.genGroup.genTable[i].genStatusLNCurr2 = sGenInfo[i].u32LNCurr2 * 100;
              privateMibBase.genGroup.genTable[i].genStatusLNCurr3 = sGenInfo[i].u32LNCurr3 * 100;
              
              privateMibBase.genGroup.genTable[i].genStatusLNPower1 = sGenInfo[i].u32LNPower1 * 100;
              privateMibBase.genGroup.genTable[i].genStatusLNPower2 = sGenInfo[i].u32LNPower2 * 100;
              privateMibBase.genGroup.genTable[i].genStatusLNPower3 = sGenInfo[i].u32LNPower3 * 100;
              
              privateMibBase.genGroup.genTable[i].genStatusFrequency = sGenInfo[i].u32GenFrequency;
              privateMibBase.genGroup.genTable[i].genStatusEngineSpeed = sGenInfo[i].u32EngineSpeed;
              privateMibBase.genGroup.genTable[i].genStatusBattVolt = sGenInfo[i].u32BattVolt;
              privateMibBase.genGroup.genTable[i].genStatusCoolantTemp = sGenInfo[i].u32CoolantTemp * 10;
              privateMibBase.genGroup.genTable[i].genStatusOilPressure = sGenInfo[i].u32OilPressure / 10;
              privateMibBase.genGroup.genTable[i].genStatusFuelLevel = sGenInfo[i].u32FuelLevel;
              privateMibBase.genGroup.genTable[i].genStatusWorkingHour = sGenInfo[i].u32EngineWorkingHour;
              privateMibBase.genGroup.genTable[i].genStatusWorkingMin = sGenInfo[i].u32EngineWorkingMin;
              
              
//              privateMibBase.genGroup.genTable[i].genStatusCoolantTemp = sGenInfo[i].u32CoolantTemp;
//              privateMibBase.genGroup.genTable[i].genStatusFuelLevel = sGenInfo[i].u32FuelLevel;
//              privateMibBase.genGroup.genTable[i].genStatusActiveEnergyLow = sGenInfo[i].u32ActiveEnergyLow;
//              privateMibBase.genGroup.genTable[i].genStatusActiveEnergyHigh = sGenInfo[i].u32ActiveEnergyHigh;
              
//              sGenInfo[i].u32AlarmStatus = (sGenInfo[i].u16Error2Status << 16) | (sGenInfo[i].u16Error1Status);
              privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.all = sGenInfo[i].u32AlarmStatus;
              
              privateMibBase.genGroup.genTable[i].genStatusType = sModbusManager.sGenManager[i].u8GenType;
              privateMibBase.genGroup.genTable[i].genStatusModbusID = sModbusManager.sGenManager[i].u32GenAbsSlaveID;
          }
          break;
          case 5:// HIMOINSA
          {
              privateMibBase.genGroup.genTable[i].genStatusID = i+1;
              privateMibBase.genGroup.genTable[i].genStatusFrequency = sGenInfo[i].u32GenFrequency;
              privateMibBase.genGroup.genTable[i].genStatusLNVolt1 = sGenInfo[i].u32LNVolt1 * 100;
              privateMibBase.genGroup.genTable[i].genStatusLNVolt2 = sGenInfo[i].u32LNVolt2 * 100;
              privateMibBase.genGroup.genTable[i].genStatusLNVolt3 = sGenInfo[i].u32LNVolt3 * 100;
              privateMibBase.genGroup.genTable[i].genStatusLNCurr1 = sGenInfo[i].u32LNCurr1 * 100;
              privateMibBase.genGroup.genTable[i].genStatusLNCurr2 = sGenInfo[i].u32LNCurr2 * 100;
              privateMibBase.genGroup.genTable[i].genStatusLNCurr3 = sGenInfo[i].u32LNCurr3 * 100;
              privateMibBase.genGroup.genTable[i].genStatusLNPower1 = sGenInfo[i].u32LNPower1 * 100;
              privateMibBase.genGroup.genTable[i].genStatusEngineSpeed = sGenInfo[i].u32EngineSpeed;
              privateMibBase.genGroup.genTable[i].genStatusFuelLevel = sGenInfo[i].u32FuelLevel / 10;
              privateMibBase.genGroup.genTable[i].genStatusBattVolt = sGenInfo[i].u32BattVolt;
              privateMibBase.genGroup.genTable[i].genStatusOilPressure = sGenInfo[i].u32OilPressure;
              privateMibBase.genGroup.genTable[i].genStatusCoolantTemp = sGenInfo[i].u32CoolantTemp;
              privateMibBase.genGroup.genTable[i].genStatusWorkingHour = sGenInfo[i].u32EngineWorkingHour;
//              privateMibBase.genGroup.genTable[i].genStatusWorkingMin = sGenInfo[i].u32EngineWorkingMin;
             
             
              privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.all = sGenInfo[i].u32AlarmStatus;
              
              privateMibBase.genGroup.genTable[i].genStatusType = sModbusManager.sGenManager[i].u8GenType;
              privateMibBase.genGroup.genTable[i].genStatusModbusID = sModbusManager.sGenManager[i].u32GenAbsSlaveID;
          }
          break;
          case 6:// QC315
          {
              privateMibBase.genGroup.genTable[i].genStatusID = i+1;
              
              privateMibBase.genGroup.genTable[i].genStatusLNVolt1 = sGenInfo[i].u32LNVolt1 * 100 / 256;
              privateMibBase.genGroup.genTable[i].genStatusLNVolt2 = sGenInfo[i].u32LNVolt2 * 100 / 256;
              privateMibBase.genGroup.genTable[i].genStatusLNVolt3 = sGenInfo[i].u32LNVolt3 * 100 / 256;
              
              privateMibBase.genGroup.genTable[i].genStatusLNCurr1 = sGenInfo[i].u32LNCurr1 * 100 / 256;
              privateMibBase.genGroup.genTable[i].genStatusLNCurr2 = sGenInfo[i].u32LNCurr2 * 100 / 256;
              privateMibBase.genGroup.genTable[i].genStatusLNCurr3 = sGenInfo[i].u32LNCurr3 * 100 / 256;
              
              privateMibBase.genGroup.genTable[i].genStatusLNPower1 = sGenInfo[i].u32LNPower1 * 100 / 256;
              privateMibBase.genGroup.genTable[i].genStatusLNPower2 = sGenInfo[i].u32LNPower2 * 100 / 256;
              privateMibBase.genGroup.genTable[i].genStatusLNPower3 = sGenInfo[i].u32LNPower3 * 100 / 256;
              
              privateMibBase.genGroup.genTable[i].genStatusLNSPower1 = sGenInfo[i].u32LNSPower1 * 100 / 256;
              privateMibBase.genGroup.genTable[i].genStatusLNSPower2 = sGenInfo[i].u32LNSPower2 * 100 / 256;
              privateMibBase.genGroup.genTable[i].genStatusLNSPower3 = sGenInfo[i].u32LNSPower3 * 100 / 256;
              
              privateMibBase.genGroup.genTable[i].genStatusFrequency = sGenInfo[i].u32GenFrequency *10 / 256;
              privateMibBase.genGroup.genTable[i].genStatusEngineSpeed = sGenInfo[i].u32EngineSpeed;
              privateMibBase.genGroup.genTable[i].genStatusOilPressure = sGenInfo[i].u32OilPressure * 10 / 256;
              privateMibBase.genGroup.genTable[i].genStatusCoolantTemp = sGenInfo[i].u32CoolantTemp * 10 / 256;
              privateMibBase.genGroup.genTable[i].genStatusFuelLevel = sGenInfo[i].u32FuelLevel / 256;
              privateMibBase.genGroup.genTable[i].genStatusBattVolt = sGenInfo[i].u32BattVolt * 10 / 256;
              
              privateMibBase.genGroup.genTable[i].genStatusWorkingHour = sGenInfo[i].u32EngineWorkingHour;
              privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.all = sGenInfo[i].u32AlarmStatus;
              
              privateMibBase.genGroup.genTable[i].genStatusType = sModbusManager.sGenManager[i].u8GenType;
              privateMibBase.genGroup.genTable[i].genStatusModbusID = sModbusManager.sGenManager[i].u32GenAbsSlaveID;
          }
          break;
          case 7:// CUMMIN
          {
              privateMibBase.genGroup.genTable[i].genStatusID = i+1;
              
              privateMibBase.genGroup.genTable[i].genStatusLNVolt1 = sGenInfo[i].u32LNVolt1 * 100;
              privateMibBase.genGroup.genTable[i].genStatusLNVolt2 = sGenInfo[i].u32LNVolt2 * 100;
              privateMibBase.genGroup.genTable[i].genStatusLNVolt3 = sGenInfo[i].u32LNVolt3 * 100;
//              
              privateMibBase.genGroup.genTable[i].genStatusLNCurr1 = sGenInfo[i].u32LNCurr1 * 10;
              privateMibBase.genGroup.genTable[i].genStatusLNCurr2 = sGenInfo[i].u32LNCurr2 * 10;
              privateMibBase.genGroup.genTable[i].genStatusLNCurr3 = sGenInfo[i].u32LNCurr3 * 10;
//              
//              privateMibBase.genGroup.genTable[i].genStatusLNPower1 = sGenInfo[i].u32LNPower1 * 100 / 256;
//              privateMibBase.genGroup.genTable[i].genStatusLNPower2 = sGenInfo[i].u32LNPower2 * 100 / 256;
//              privateMibBase.genGroup.genTable[i].genStatusLNPower3 = sGenInfo[i].u32LNPower3 * 100 / 256;
//  
              privateMibBase.genGroup.genTable[i].genStatusLNSPower1 = sGenInfo[i].u32LNSPower1 * 100;
              privateMibBase.genGroup.genTable[i].genStatusLNSPower2 = sGenInfo[i].u32LNSPower2 * 100;
              privateMibBase.genGroup.genTable[i].genStatusLNSPower3 = sGenInfo[i].u32LNSPower3 * 100;
              
//              privateMibBase.genGroup.genTable[i].genStatusFrequency = sGenInfo[i].u32GenFrequency *10 / 256;
              privateMibBase.genGroup.genTable[i].genStatusEngineSpeed = sGenInfo[i].u32EngineSpeed;
              privateMibBase.genGroup.genTable[i].genStatusOilPressure = (sGenInfo[i].u32OilPressure * 10)/100;
              privateMibBase.genGroup.genTable[i].genStatusCoolantTemp = sGenInfo[i].u32CoolantTemp;
//              privateMibBase.genGroup.genTable[i].genStatusFuelLevel = sGenInfo[i].u32FuelLevel / 256;
              privateMibBase.genGroup.genTable[i].genStatusBattVolt = sGenInfo[i].u32BattVolt;
//          
              privateMibBase.genGroup.genTable[i].genStatusWorkingHour = sGenInfo[i].u32EngineWorkingHour/3600;
              
              // thanhcm3 fix ----------------------------------------------------------------------------
              privateMibBase.genGroup.genTable[i].genStatusWorkingMin  = (sGenInfo[i].u32EngineWorkingHour%3600)/60;
              privateMibBase.genGroup.genTable[i].genStatusFrequency   = sGenInfo[i].u32GenFrequency;
              // thanhcm3 fix ----------------------------------------------------------------------------
              
              if(sGenInfo[i].u32AlarmStatus == 1448)
                privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.bit.err_UnderFreq = 1;
              else
                privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.bit.err_UnderFreq = 0;
              
              if((sGenInfo[i].u32AlarmStatus == 146)||(sGenInfo[i].u32AlarmStatus == 151))
                privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.bit.err_HiCoolantTemp = 1;
              else
                privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.bit.err_HiCoolantTemp = 0;
              
              if((sGenInfo[i].u32AlarmStatus == 143)||(sGenInfo[i].u32AlarmStatus == 415))
                privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.bit.err_LowOilPressure = 1;
              else
                privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.bit.err_LowOilPressure = 0;
              
              if(sGenInfo[i].u32AlarmStatus == 442)
                privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.bit.err_HiBattVol = 1;
              else
                privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.bit.err_HiBattVol = 0;
              
              if(sGenInfo[i].u32AlarmStatus == 441)
                privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.bit.err_LowBattVol = 1;
              else
                privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.bit.err_LowBattVol = 0;
              
              privateMibBase.genGroup.genTable[i].genStatusType = sModbusManager.sGenManager[i].u8GenType;
              privateMibBase.genGroup.genTable[i].genStatusModbusID = sModbusManager.sGenManager[i].u32GenAbsSlaveID;
          }
          break;
          };
      }
      else
      {
              privateMibBase.genGroup.genTable[i].genStatusID = i+1;
              privateMibBase.genGroup.genTable[i].genStatusFrequency = 0;
              privateMibBase.genGroup.genTable[i].genStatusEngineSpeed = 0;
              privateMibBase.genGroup.genTable[i].genStatusOilPressure = 0;
              privateMibBase.genGroup.genTable[i].genStatusCoolantTemp = 0;
              privateMibBase.genGroup.genTable[i].genStatusFuelLevel = 0;
              privateMibBase.genGroup.genTable[i].genStatusBattVolt = 0;
              
              privateMibBase.genGroup.genTable[i].genStatusLNVolt1 = 0;
              privateMibBase.genGroup.genTable[i].genStatusLNVolt2 = 0;
              privateMibBase.genGroup.genTable[i].genStatusLNVolt3 = 0;
              
              privateMibBase.genGroup.genTable[i].genStatusLNCurr1 = 0;
              privateMibBase.genGroup.genTable[i].genStatusLNCurr2 = 0;
              privateMibBase.genGroup.genTable[i].genStatusLNCurr3 = 0;
              
              privateMibBase.genGroup.genTable[i].genStatusLNPower1 = 0;
              privateMibBase.genGroup.genTable[i].genStatusLNPower2 = 0;
              privateMibBase.genGroup.genTable[i].genStatusLNPower3 = 0;
              
              privateMibBase.genGroup.genTable[i].genStatusWorkingHour = 0;
              privateMibBase.genGroup.genTable[i].genStatusWorkingMin = 0;
              privateMibBase.genGroup.genTable[i].genStatusActiveEnergyLow = 0;
              privateMibBase.genGroup.genTable[i].genStatusActiveEnergyHigh = 0;
              
              privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.all = 0;
              
              privateMibBase.genGroup.genTable[i].genStopMode = 0;
              privateMibBase.genGroup.genTable[i].genAutoMode = 0;
              privateMibBase.genGroup.genTable[i].genManualMode = 0;
              privateMibBase.genGroup.genTable[i].genStartMode = 0;
              
              privateMibBase.genGroup.genTable[i].genStatusType = sModbusManager.sGenManager[i].u8GenType;
              privateMibBase.genGroup.genTable[i].genStatusModbusID = sModbusManager.sGenManager[i].u32GenAbsSlaveID;
      }
  }
  //clean gen---------------------------------------------------------------
  for(i= privateMibBase.genGroup.genInstalledGen;i<1;i++){
              privateMibBase.genGroup.genTable[i].genStatusStatus = 0;
                
              privateMibBase.genGroup.genTable[i].genStatusID = i+1;
              privateMibBase.genGroup.genTable[i].genStatusFrequency = 0;
              privateMibBase.genGroup.genTable[i].genStatusEngineSpeed = 0;
              privateMibBase.genGroup.genTable[i].genStatusOilPressure = 0;
              privateMibBase.genGroup.genTable[i].genStatusCoolantTemp = 0;
              privateMibBase.genGroup.genTable[i].genStatusFuelLevel = 0;
              privateMibBase.genGroup.genTable[i].genStatusBattVolt = 0;
              
              privateMibBase.genGroup.genTable[i].genStatusLNVolt1 = 0;
              privateMibBase.genGroup.genTable[i].genStatusLNVolt2 = 0;
              privateMibBase.genGroup.genTable[i].genStatusLNVolt3 = 0;
              
              privateMibBase.genGroup.genTable[i].genStatusLNCurr1 = 0;
              privateMibBase.genGroup.genTable[i].genStatusLNCurr2 = 0;
              privateMibBase.genGroup.genTable[i].genStatusLNCurr3 = 0;
              
              privateMibBase.genGroup.genTable[i].genStatusLNPower1 = 0;
              privateMibBase.genGroup.genTable[i].genStatusLNPower2 = 0;
              privateMibBase.genGroup.genTable[i].genStatusLNPower3 = 0;
              
              privateMibBase.genGroup.genTable[i].genStatusWorkingHour = 0;
              privateMibBase.genGroup.genTable[i].genStatusWorkingMin = 0;
              privateMibBase.genGroup.genTable[i].genStatusActiveEnergyLow = 0;
              privateMibBase.genGroup.genTable[i].genStatusActiveEnergyHigh = 0;
              
              privateMibBase.genGroup.genTable[i].genStatusAlarmStatus.all = 0;
              
              privateMibBase.genGroup.genTable[i].genStopMode = 0;
              privateMibBase.genGroup.genTable[i].genAutoMode = 0;
              privateMibBase.genGroup.genTable[i].genManualMode = 0;
              privateMibBase.genGroup.genTable[i].genStartMode = 0;
              
              privateMibBase.genGroup.genTable[i].genStatusType = sModbusManager.sGenManager[i].u8GenType;
              privateMibBase.genGroup.genTable[i].genStatusModbusID = sModbusManager.sGenManager[i].u32GenAbsSlaveID;
  }

}

#if (USERDEF_MONITOR_BM == ENABLED)
void UpdateBMInfo (void)
{
  uint32_t j,i=0;
  
  privateMibBase.bmGroup.bmInstalledBM = sModbusManager.u8NumberOfBM;
  privateMibBase.connAlarmGroup.alarmBMConnect = 0;
  privateMibBase.mainAlarmGroup.alarmBM = 0;
  privateMibBase.bmGroup.bmActiveBM = privateMibBase.bmGroup.bmInstalledBM;
  for (i = 0; i < privateMibBase.bmGroup.bmInstalledBM; i++)
  {
      privateMibBase.mainAlarmGroup.alarmBMPack[i] = 0;
      if (sBMInfo[i].u8ErrorFlag == 1)
      {
          privateMibBase.bmGroup.bmActiveBM--;
          if (privateMibBase.bmGroup.bmActiveBM <= 0)
          {
            privateMibBase.bmGroup.bmActiveBM = 0;
          }
          privateMibBase.connAlarmGroup.alarmBMConnect |= (sBMInfo[i].u8ErrorFlag << i);
          privateMibBase.bmGroup.bmTable[i].bmStatus = 170;
      }
      else
      {        
//          privateMibBase.bmGroup.bmTable[i].bmStatus = 255;
            if(privateMibBase.bmGroup.bmTable[i].bmAlarmStatus.all != 0)
            {
                privateMibBase.bmGroup.bmTable[i].bmStatus = 165;
            }
            else
            {
                privateMibBase.bmGroup.bmTable[i].bmStatus = 255;
            }
      }

      if (privateMibBase.bmGroup.bmTable[i].bmStatus != 170)
      {
          if (privateMibBase.bmGroup.bmTable[i].bmStatus == 165)
          {
              privateMibBase.mainAlarmGroup.alarmBMPack[i] = privateMibBase.bmGroup.bmTable[i].bmAlarmStatus.all;//1;
          }

          switch(sModbusManager.sBMManager[i].u8BMType)
          {
          case 1:// VIETTEL
          {
              privateMibBase.bmGroup.bmTable[i].bmID = i+1;
              privateMibBase.bmGroup.bmTable[i].bmBattVolt = sBMInfo[i].u32BattVolt;
              privateMibBase.bmGroup.bmTable[i].bmPackVolt = sBMInfo[i].u32PackVolt;
              privateMibBase.bmGroup.bmTable[i].bmPackCurr = sBMInfo[i].i32PackCurr;
              
              privateMibBase.bmGroup.bmTable[i].bmCellVolt1 = sBMInfo[i].u16CellVolt[0];              
              privateMibBase.bmGroup.bmTable[i].bmCellVolt2 = sBMInfo[i].u16CellVolt[1];             
              privateMibBase.bmGroup.bmTable[i].bmCellVolt3 = sBMInfo[i].u16CellVolt[2];             
              privateMibBase.bmGroup.bmTable[i].bmCellVolt4 = sBMInfo[i].u16CellVolt[3];          
              
              privateMibBase.bmGroup.bmTable[i].bmCellTemp1 = sBMInfo[i].u16CellTemp[0];
              privateMibBase.bmGroup.bmTable[i].bmCellTemp2 = sBMInfo[i].u16CellTemp[1];
              privateMibBase.bmGroup.bmTable[i].bmCellTemp3 = sBMInfo[i].u16CellTemp[2];
              privateMibBase.bmGroup.bmTable[i].bmCellTemp4 = sBMInfo[i].u16CellTemp[3];
              privateMibBase.bmGroup.bmTable[i].bmCellTemp5 = sBMInfo[i].u16CellTemp[4];
              privateMibBase.bmGroup.bmTable[i].bmCellTemp6 = sBMInfo[i].u16CellTemp[5];
              
              privateMibBase.bmGroup.bmTable[i].bmSOC = sBMInfo[i].u16SOC;              
              privateMibBase.bmGroup.bmTable[i].bmSOH = sBMInfo[i].u32SOH;
              privateMibBase.bmGroup.bmTable[i].bmAlarmStatus.all = sBMInfo[i].u32AlarmStatus;
              privateMibBase.bmGroup.bmTable[i].bmBalanceStatus = sBMInfo[i].u32BalanceStatus;
              privateMibBase.bmGroup.bmTable[i].bmBattStatus = sBMInfo[i].u16BattStatus;
              
              
              privateMibBase.bmGroup.bmTable[i].bmDischargeTime = sBMInfo[i].u32DischargeTime;
              privateMibBase.bmGroup.bmTable[i].bmTotalRunTime = sBMInfo[i].u32TotalRunTime;             
              
              privateMibBase.bmGroup.bmTable[i].bmVoltDiff = (uint32_t) (sBMInfo[i].u32VoltDiff);
              privateMibBase.bmGroup.bmTable[i].bmMaxTemp = (int32_t) (sBMInfo[i].u32MaxTemp);
              privateMibBase.bmGroup.bmTable[i].bmVoltThres = (uint32_t) (sBMInfo[i].u32VoltThres);
              privateMibBase.bmGroup.bmTable[i].bmCurrThres = (uint32_t) (sBMInfo[i].u32CurrThres);
              privateMibBase.bmGroup.bmTable[i].bmTimeThres = (uint32_t) (sBMInfo[i].u32TimeThres);
              privateMibBase.bmGroup.bmTable[i].bmSOCThres = (uint32_t) (sBMInfo[i].u32SOCThres);
              privateMibBase.bmGroup.bmTable[i].bmMinTemp = (int32_t) (sBMInfo[i].u32MinTemp);
              privateMibBase.bmGroup.bmTable[i].bmLowCapTime = (uint32_t) (sBMInfo[i].u32LowCapTime);
              privateMibBase.bmGroup.bmTable[i].bmTotalDisAH = (uint32_t) (sBMInfo[i].u32TotalDisAH);
              privateMibBase.bmGroup.bmTable[i].bmHalfVoltAlarm = (uint32_t) (sBMInfo[i].u32HalfVoltAlarm);
              privateMibBase.bmGroup.bmTable[i].bmRechargeVolt = (uint32_t) (sBMInfo[i].u32RechargeVolt);
              privateMibBase.bmGroup.bmTable[i].bmCurrBalanceThres = (uint32_t) (sBMInfo[i].u32CurrBalanceThres);

              privateMibBase.bmGroup.bmTable[i].bmType = sModbusManager.sBMManager[i].u8BMType;
              privateMibBase.bmGroup.bmTable[i].bmModbusID = sModbusManager.sBMManager[i].u32BMAbsSlaveID;
          }
          break;
          };
      }
      else
      {
              privateMibBase.bmGroup.bmTable[i].bmID = i+1;
              privateMibBase.bmGroup.bmTable[i].bmBattVolt = 0;
              privateMibBase.bmGroup.bmTable[i].bmPackVolt = 0;
              privateMibBase.bmGroup.bmTable[i].bmPackCurr = 0;
              
              privateMibBase.bmGroup.bmTable[i].bmCellVolt1 = 0;              
              privateMibBase.bmGroup.bmTable[i].bmCellVolt2 = 0;             
              privateMibBase.bmGroup.bmTable[i].bmCellVolt3 = 0;             
              privateMibBase.bmGroup.bmTable[i].bmCellVolt4 = 0;          
              
              privateMibBase.bmGroup.bmTable[i].bmCellTemp1 = 0;
              privateMibBase.bmGroup.bmTable[i].bmCellTemp2 = 0;
              privateMibBase.bmGroup.bmTable[i].bmCellTemp3 = 0;
              privateMibBase.bmGroup.bmTable[i].bmCellTemp4 = 0;
              privateMibBase.bmGroup.bmTable[i].bmCellTemp5 = 0;
              privateMibBase.bmGroup.bmTable[i].bmCellTemp6 = 0;
              
              privateMibBase.bmGroup.bmTable[i].bmSOC = 0;              
              privateMibBase.bmGroup.bmTable[i].bmSOH = 0;
              
              privateMibBase.bmGroup.bmTable[i].bmDischargeTime = 0;
              privateMibBase.bmGroup.bmTable[i].bmTotalRunTime = 0;
              
              privateMibBase.bmGroup.bmTable[i].bmVoltDiff = 0;
              privateMibBase.bmGroup.bmTable[i].bmMaxTemp = 0;
              privateMibBase.bmGroup.bmTable[i].bmVoltThres = 0;
              privateMibBase.bmGroup.bmTable[i].bmCurrThres = 0;
              privateMibBase.bmGroup.bmTable[i].bmTimeThres = 0;
              privateMibBase.bmGroup.bmTable[i].bmSOCThres = 0;
              privateMibBase.bmGroup.bmTable[i].bmMinTemp = 0;
              privateMibBase.bmGroup.bmTable[i].bmLowCapTime = 0;
              privateMibBase.bmGroup.bmTable[i].bmTotalDisAH = 0;
              privateMibBase.bmGroup.bmTable[i].bmHalfVoltAlarm = 0;
              privateMibBase.bmGroup.bmTable[i].bmRechargeVolt = 0;
              privateMibBase.bmGroup.bmTable[i].bmCurrBalanceThres = 0;
              privateMibBase.bmGroup.bmTable[i].bmAlarmStatus.all = 0;

              privateMibBase.bmGroup.bmTable[i].bmType = sModbusManager.sBMManager[i].u8BMType;
              privateMibBase.bmGroup.bmTable[i].bmModbusID = sModbusManager.sBMManager[i].u32BMAbsSlaveID;
      }
  }

//  if (privateMibBase.connAlarmGroup.alarmBMConnect != 0)
//  {
//      privateMibBase.connAlarmGroup.alarmBMConnect = 1;
//  }
  
}
#endif
#if (USERDEF_MONITOR_PM == ENABLED)

uint8_t cntOutFrq, cntInFrq;
DELTA_STRUCT delta_f_pm_struct;


void UpdatePMInfo (void)
{
  uint32_t j,i=0;
  
  privateMibBase.pmGroup.pmInstalledPM = sModbusManager.u8NumberOfPM;
  privateMibBase.connAlarmGroup.alarmPMConnect = 0;
  privateMibBase.mainAlarmGroup.alarmPM = 0;
  privateMibBase.pmGroup.pmActivePM = privateMibBase.pmGroup.pmInstalledPM;
  for (i = 0; i < privateMibBase.pmGroup.pmInstalledPM; i++)
  {
      if (sPMInfo[i].u8ErrorFlag == 1)
      {
        if(cntDisConnectPM[i] > 2){ // mat ket noi 3 lan moi bao len
          cntDisConnectPM[i] = 3;
          privateMibBase.pmGroup.pmActivePM--;
          if (privateMibBase.pmGroup.pmActivePM <= 0)
          {
            privateMibBase.pmGroup.pmActivePM = 0;
          }
          privateMibBase.connAlarmGroup.alarmPMConnect |= (sPMInfo[i].u8ErrorFlag << i);
          privateMibBase.pmGroup.pmTable[i].pmStatus = 170;
        }
      }
      else
      {
        cntDisConnectPM[i] = 0;
          privateMibBase.pmGroup.pmTable[i].pmStatus = 255;
//            if(privateMibBase.pmGroup.pmTable[i].liBattWarningFlag != 0)
//            {
//                privateMibBase.pmGroup.pmTable[i].pmStatusStatus = 165;
//            }
//            else
//            {
//                privateMibBase.pmGroup.pmTable[i].pmStatusStatus = 255;
//            }
      }

      if (privateMibBase.pmGroup.pmTable[i].pmStatus != 170)
      {
          if (privateMibBase.pmGroup.pmTable[i].pmStatus == 165)
          {
              privateMibBase.mainAlarmGroup.alarmPM = 1;
          }

          switch(sModbusManager.sPMManager[i].u8PMType)
          {
          case 1:// FINECO
          {
              privateMibBase.pmGroup.pmTable[i].pmID = i+1;
              privateMibBase.pmGroup.pmTable[i].pmImportActiveEnergy = (uint32_t) (sPMInfo[i].u32ImportActiveE * 100);
              privateMibBase.pmGroup.pmTable[i].pmExportActiveEnergy = (uint32_t) (sPMInfo[i].u32ExportActiveE * 100);
              privateMibBase.pmGroup.pmTable[i].pmImportReactiveEnergy = (uint32_t) (sPMInfo[i].u32ImportReActiveE * 100) ;
              privateMibBase.pmGroup.pmTable[i].pmExportReactiveEnergy = (uint32_t) (sPMInfo[i].u32ExportReActiveE * 1000) ;              
              privateMibBase.pmGroup.pmTable[i].pmTotalActiveEnergy = (uint32_t) (sPMInfo[i].u32TotalActiveE * 1000) ;             
              privateMibBase.pmGroup.pmTable[i].pmTotalReactiveEnergy = (uint32_t) (sPMInfo[i].u32TotalReActiveE * 1000) ;             
              privateMibBase.pmGroup.pmTable[i].pmPowerFactor = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fPowerFactor * 100) ; 
              privateMibBase.pmGroup.pmTable[i].pmFrequency = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fFrequency * 10) ;
              privateMibBase.pmGroup.pmTable[i].pmTotalCurrent = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fCurrent*100);
              privateMibBase.pmGroup.pmTable[i].pmActivePower = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fActivePower*1000);
              privateMibBase.pmGroup.pmTable[i].pmReactivePower = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fReactivePower*1000);
              
              privateMibBase.pmGroup.pmTable[i].pmPhase1Current = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fCurrent * 100);
              privateMibBase.pmGroup.pmTable[i].pmPhase1Voltage = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fVoltage * 100);
              privateMibBase.pmGroup.pmTable[i].pmPhase1RealPower = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fActivePower * 1000);
              privateMibBase.pmGroup.pmTable[i].pmPhase1ApparentPower = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fApparentPower * 1000);              
              privateMibBase.pmGroup.pmTable[i].pmPhase1ReactivePower = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fReactivePower * 1000);
              privateMibBase.pmGroup.pmTable[i].pmPhase1PowerFactor = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fPowerFactor * 100);
              
              privateMibBase.pmGroup.pmTable[i].pmPhase2Current = (uint32_t) (sPMInfo[i].sPhaseInfo[1].fCurrent * 100);
              privateMibBase.pmGroup.pmTable[i].pmPhase2Voltage = (uint32_t) (sPMInfo[i].sPhaseInfo[1].fVoltage * 100);
              privateMibBase.pmGroup.pmTable[i].pmPhase2RealPower = (uint32_t) (sPMInfo[i].sPhaseInfo[1].fActivePower * 1000);
              privateMibBase.pmGroup.pmTable[i].pmPhase2ApparentPower = (uint32_t) (sPMInfo[i].sPhaseInfo[1].fApparentPower * 1000);              
              privateMibBase.pmGroup.pmTable[i].pmPhase2ReactivePower = (uint32_t) (sPMInfo[i].sPhaseInfo[1].fReactivePower * 1000);
              privateMibBase.pmGroup.pmTable[i].pmPhase2PowerFactor = (uint32_t) (sPMInfo[i].sPhaseInfo[1].fPowerFactor * 100);
              
              privateMibBase.pmGroup.pmTable[i].pmPhase3Current = (uint32_t) (sPMInfo[i].sPhaseInfo[2].fCurrent * 100);
              privateMibBase.pmGroup.pmTable[i].pmPhase3Voltage = (uint32_t) (sPMInfo[i].sPhaseInfo[2].fVoltage * 100);
              privateMibBase.pmGroup.pmTable[i].pmPhase3RealPower = (uint32_t) (sPMInfo[i].sPhaseInfo[2].fActivePower * 1000);
              privateMibBase.pmGroup.pmTable[i].pmPhase3ApparentPower = (uint32_t) (sPMInfo[i].sPhaseInfo[2].fApparentPower * 1000);              
              privateMibBase.pmGroup.pmTable[i].pmPhase3ReactivePower = (uint32_t) (sPMInfo[i].sPhaseInfo[2].fReactivePower * 1000);
              privateMibBase.pmGroup.pmTable[i].pmPhase3PowerFactor = (uint32_t) (sPMInfo[i].sPhaseInfo[2].fPowerFactor * 100);
              
              privateMibBase.pmGroup.pmTable[i].pmType = sModbusManager.sPMManager[i].u8PMType;
              privateMibBase.pmGroup.pmTable[i].pmModbusID = sModbusManager.sPMManager[i].u32PMAbsSlaveID;
              
              
              
              if((privateMibBase.pmGroup.pmTable[i].pmFrequency != 0) && (privateMibBase.pmGroup.pmTable[i].pmEnableFreqTrap == 1))
              {
                //thanhcm3 fix ------------------------------------------------------------------------------------
                if(privateMibBase.pmGroup.pmTable[i].pmFrequency>=500){
                  delta_f_pm_struct._0 = privateMibBase.pmGroup.pmTable[i].pmFrequency -500;
                }else if(privateMibBase.pmGroup.pmTable[i].pmFrequency<500){
                  delta_f_pm_struct._0 = 500 - privateMibBase.pmGroup.pmTable[i].pmFrequency;
                }
                //thanhcm3 fix ------------------------------------------------------------------------------------
                if(delta_f_pm_struct._0<=privateMibBase.pmGroup.pmTable[i].pm_delta_freq_v){
                  if ((privateMibBase.pmGroup.pmTable[i].pmFrequency >= (500 + privateMibBase.pmGroup.pmTable[i].pmDeltaFreqDisConnect)) || 
                      (privateMibBase.pmGroup.pmTable[i].pmFrequency <= (500 - privateMibBase.pmGroup.pmTable[i].pmDeltaFreqDisConnect))){
                        cntInFrq = 0;
                        if(cntOutFrq++ > privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_out){
                          cntOutFrq = privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_out+1;
                          privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 1;
                        }
                      }
                  else if ((privateMibBase.pmGroup.pmTable[i].pmFrequency <= (500 + privateMibBase.pmGroup.pmTable[i].pmDeltaFreqReConnect)) && 
                           (privateMibBase.pmGroup.pmTable[i].pmFrequency >= (500 - privateMibBase.pmGroup.pmTable[i].pmDeltaFreqReConnect))){
                             cntOutFrq = 0;
                             if(cntInFrq++ > privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_in){
                               cntInFrq = privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_in+1;
                               privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
                             }
                           }
                }else{
                  cntOutFrq = 0;
                  cntInFrq  = 0;
                  privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
                }
              }
              else {
                cntOutFrq = 0;
                cntInFrq  = 0;
                privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
              }
              
              privateMibBase.mainAlarmGroup.alarmOutOfRangeFreqPack[i] = privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq;
          }
          break;
          case 2:// ASCENT
          {
              privateMibBase.pmGroup.pmTable[i].pmID = i+1;
              privateMibBase.pmGroup.pmTable[i].pmFrequency = (uint32_t) (sPMInfo[i].fFrequency * 10) ;
                     
              privateMibBase.pmGroup.pmTable[i].pmPowerFactor = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fPowerFactor*100) ; 
              privateMibBase.pmGroup.pmTable[i].pmPhase1Current = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fCurrent * 100);
              privateMibBase.pmGroup.pmTable[i].pmPhase1Voltage = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fVoltage * 100);
              privateMibBase.pmGroup.pmTable[i].pmPhase1RealPower = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fActivePower/10);
              privateMibBase.pmGroup.pmTable[i].pmPhase1PowerFactor = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fPowerFactor * 100);
              privateMibBase.pmGroup.pmTable[i].pmTotalCurrent = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fCurrent*100);
              privateMibBase.pmGroup.pmTable[i].pmActivePower = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fActivePower/10);
              
              privateMibBase.pmGroup.pmTable[i].pmPhase2Current = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2Voltage = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2RealPower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2ApparentPower = 0;              
              privateMibBase.pmGroup.pmTable[i].pmPhase2ReactivePower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2PowerFactor = 0;
              
              privateMibBase.pmGroup.pmTable[i].pmPhase3Current = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3Voltage = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3RealPower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3ApparentPower = 0;              
              privateMibBase.pmGroup.pmTable[i].pmPhase3ReactivePower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3PowerFactor = 0;
              
              privateMibBase.pmGroup.pmTable[i].pmTotalActiveEnergy = (uint32_t) (sPMInfo[i].fRealEnergy *100);
              
              privateMibBase.pmGroup.pmTable[i].pmType = sModbusManager.sPMManager[i].u8PMType;
              privateMibBase.pmGroup.pmTable[i].pmModbusID = sModbusManager.sPMManager[i].u32PMAbsSlaveID;
              
              
              if((privateMibBase.pmGroup.pmTable[i].pmFrequency != 0) && (privateMibBase.pmGroup.pmTable[i].pmEnableFreqTrap == 1))
              {
                //thanhcm3 fix ------------------------------------------------------------------------------------
                if(privateMibBase.pmGroup.pmTable[i].pmFrequency>=500){
                  delta_f_pm_struct._0 = privateMibBase.pmGroup.pmTable[i].pmFrequency -500;
                }else if(privateMibBase.pmGroup.pmTable[i].pmFrequency<500){
                  delta_f_pm_struct._0 = 500 - privateMibBase.pmGroup.pmTable[i].pmFrequency;
                }
                //thanhcm3 fix ------------------------------------------------------------------------------------
                if(delta_f_pm_struct._0<=privateMibBase.pmGroup.pmTable[i].pm_delta_freq_v){
                  if ((privateMibBase.pmGroup.pmTable[i].pmFrequency >= (500 + privateMibBase.pmGroup.pmTable[i].pmDeltaFreqDisConnect)) || 
                      (privateMibBase.pmGroup.pmTable[i].pmFrequency <= (500 - privateMibBase.pmGroup.pmTable[i].pmDeltaFreqDisConnect))){
                        cntInFrq = 0;
                        if(cntOutFrq++ > privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_out){
                          cntOutFrq = privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_out +1;
                          privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 1;
                        }
                      }
                  else if ((privateMibBase.pmGroup.pmTable[i].pmFrequency <= (500 + privateMibBase.pmGroup.pmTable[i].pmDeltaFreqReConnect)) && 
                           (privateMibBase.pmGroup.pmTable[i].pmFrequency >= (500 - privateMibBase.pmGroup.pmTable[i].pmDeltaFreqReConnect))){
                             cntOutFrq = 0;
                             if(cntInFrq++ > privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_in){
                               cntInFrq = privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_in +1;
                               privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
                             }
                           }
                }else{
                  cntOutFrq = 0;
                  cntInFrq  = 0;
                  privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
                  
                }
              }
              else {
                cntOutFrq = 0;
                cntInFrq  = 0;
                privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
              }
              
              privateMibBase.mainAlarmGroup.alarmOutOfRangeFreqPack[i] = privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq;
          }
          break;
          case 3:// EASTRON
          {
              privateMibBase.pmGroup.pmTable[i].pmID = i+1;
             
              privateMibBase.pmGroup.pmTable[i].pmImportActiveEnergy = (uint32_t) (sPMInfo[i].u32ImportActiveE);
              privateMibBase.pmGroup.pmTable[i].pmExportActiveEnergy = (uint32_t) (sPMInfo[i].u32ExportActiveE);
              privateMibBase.pmGroup.pmTable[i].pmImportReactiveEnergy = (uint32_t) (sPMInfo[i].u32ImportReActiveE) ;
              privateMibBase.pmGroup.pmTable[i].pmExportReactiveEnergy = (uint32_t) (sPMInfo[i].u32ExportReActiveE) ;              
              privateMibBase.pmGroup.pmTable[i].pmTotalActiveEnergy = (uint32_t) (sPMInfo[i].u32TotalActiveE) ;             
              privateMibBase.pmGroup.pmTable[i].pmTotalReactiveEnergy = (uint32_t) (sPMInfo[i].u32TotalReActiveE) ;             
              privateMibBase.pmGroup.pmTable[i].pmPowerFactor = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fPowerFactor*100) ; 
              privateMibBase.pmGroup.pmTable[i].pmFrequency = (uint32_t) (sPMInfo[i].fFrequency*10);
              privateMibBase.pmGroup.pmTable[i].pmTotalCurrent = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fCurrent*100);
              privateMibBase.pmGroup.pmTable[i].pmActivePower = (int32_t) (sPMInfo[i].sPhaseInfo[0].fActivePower/10);
              privateMibBase.pmGroup.pmTable[i].pmReactivePower = (int32_t) (sPMInfo[i].sPhaseInfo[0].fReactivePower/10);
              
              privateMibBase.pmGroup.pmTable[i].pmPhase1Current = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fCurrent*100);
              privateMibBase.pmGroup.pmTable[i].pmPhase1Voltage = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fVoltage*100);
              privateMibBase.pmGroup.pmTable[i].pmPhase1RealPower = (int32_t) (sPMInfo[i].sPhaseInfo[0].fActivePower/10);
              privateMibBase.pmGroup.pmTable[i].pmPhase1ApparentPower = (int32_t) (sPMInfo[i].sPhaseInfo[0].fApparentPower/10);              
              privateMibBase.pmGroup.pmTable[i].pmPhase1ReactivePower = (int32_t) (sPMInfo[i].sPhaseInfo[0].fReactivePower/10);
              privateMibBase.pmGroup.pmTable[i].pmPhase1PowerFactor = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fPowerFactor*100);

              privateMibBase.pmGroup.pmTable[i].pmPhase2Current = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2Voltage = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2RealPower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2ApparentPower = 0;              
              privateMibBase.pmGroup.pmTable[i].pmPhase2ReactivePower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2PowerFactor = 0;
              
              privateMibBase.pmGroup.pmTable[i].pmPhase3Current = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3Voltage = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3RealPower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3ApparentPower = 0;              
              privateMibBase.pmGroup.pmTable[i].pmPhase3ReactivePower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3PowerFactor = 0;
              
              sprintf(&privateMibBase.pmGroup.pmTable[i].pmSerial[0],"%d",sPMInfo[i].u32SerialNumber);
              privateMibBase.pmGroup.pmTable[i].pmSerialLen = 8;
              sprintf(&privateMibBase.pmGroup.pmTable[i].pmModel[0],"SDM220");
              privateMibBase.pmGroup.pmTable[i].pmModelLen = 6;
              privateMibBase.pmGroup.pmTable[i].pmType = sModbusManager.sPMManager[i].u8PMType;
              privateMibBase.pmGroup.pmTable[i].pmModbusID = sModbusManager.sPMManager[i].u32PMAbsSlaveID;
              
              if((privateMibBase.pmGroup.pmTable[i].pmFrequency != 0) && (privateMibBase.pmGroup.pmTable[i].pmEnableFreqTrap == 1))
              {
                //thanhcm3 fix ------------------------------------------------------------------------------------
                if(privateMibBase.pmGroup.pmTable[i].pmFrequency>=500){
                  delta_f_pm_struct._0 = privateMibBase.pmGroup.pmTable[i].pmFrequency -500;
                }else if(privateMibBase.pmGroup.pmTable[i].pmFrequency<500){
                  delta_f_pm_struct._0 = 500 - privateMibBase.pmGroup.pmTable[i].pmFrequency;
                }
                //thanhcm3 fix ------------------------------------------------------------------------------------
                if(delta_f_pm_struct._0<=privateMibBase.pmGroup.pmTable[i].pm_delta_freq_v){
                  if ((privateMibBase.pmGroup.pmTable[i].pmFrequency >= (500 + privateMibBase.pmGroup.pmTable[i].pmDeltaFreqDisConnect)) || 
                      (privateMibBase.pmGroup.pmTable[i].pmFrequency <= (500 - privateMibBase.pmGroup.pmTable[i].pmDeltaFreqDisConnect))){
                        cntInFrq = 0;
                        if(cntOutFrq++ > privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_out){
                          cntOutFrq = privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_out + 1;
                          privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 1;
                        }
                      }
                  else if ((privateMibBase.pmGroup.pmTable[i].pmFrequency <= (500 + privateMibBase.pmGroup.pmTable[i].pmDeltaFreqReConnect)) && 
                           (privateMibBase.pmGroup.pmTable[i].pmFrequency >= (500 - privateMibBase.pmGroup.pmTable[i].pmDeltaFreqReConnect))){
                             cntOutFrq = 0;
                             if(cntInFrq++ > privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_in){
                               cntInFrq = privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_in + 1;
                               privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
                             }
                           }
                }else{
                  cntOutFrq = 0;
                  cntInFrq  = 0;
                  privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
                }
              }
              else {
                cntOutFrq = 0;
                cntInFrq  = 0;
                privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
              }
              
              privateMibBase.mainAlarmGroup.alarmOutOfRangeFreqPack[i] = privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq;
             
          }
          break;
          case 4:// CET1
          {
              privateMibBase.pmGroup.pmTable[i].pmID = i+1;
              privateMibBase.pmGroup.pmTable[i].pmImportActiveEnergy = (uint32_t) (sPMInfo[i].u32ImportActiveE);
              privateMibBase.pmGroup.pmTable[i].pmExportActiveEnergy = (uint32_t) (sPMInfo[i].u32ExportActiveE);
              privateMibBase.pmGroup.pmTable[i].pmImportReactiveEnergy = (uint32_t) (sPMInfo[i].u32ImportReActiveE) ;
              privateMibBase.pmGroup.pmTable[i].pmExportReactiveEnergy = (uint32_t) (sPMInfo[i].u32ExportReActiveE) ;              
              privateMibBase.pmGroup.pmTable[i].pmTotalActiveEnergy = (uint32_t) (sPMInfo[i].u32TotalActiveE) ;             
              privateMibBase.pmGroup.pmTable[i].pmTotalReactiveEnergy = (uint32_t) (sPMInfo[i].u32TotalReActiveE) ;             
              privateMibBase.pmGroup.pmTable[i].pmPowerFactor = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fPowerFactor * 100) ; 
              privateMibBase.pmGroup.pmTable[i].pmFrequency = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fFrequency * 10) ;
              privateMibBase.pmGroup.pmTable[i].pmTotalCurrent = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fCurrent*100);
              privateMibBase.pmGroup.pmTable[i].pmActivePower = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fActivePower*100);
              privateMibBase.pmGroup.pmTable[i].pmReactivePower = (int32_t) (sPMInfo[i].sPhaseInfo[0].fReactivePower*100);
              
              privateMibBase.pmGroup.pmTable[i].pmPhase1Current = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fCurrent * 100);
              privateMibBase.pmGroup.pmTable[i].pmPhase1Voltage = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fVoltage * 100);
              privateMibBase.pmGroup.pmTable[i].pmPhase1RealPower = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fActivePower *100);
              privateMibBase.pmGroup.pmTable[i].pmPhase1ReactivePower = (int32_t) (sPMInfo[i].sPhaseInfo[0].fReactivePower *100);
              privateMibBase.pmGroup.pmTable[i].pmPhase1PowerFactor = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fPowerFactor * 100);
              
              privateMibBase.pmGroup.pmTable[i].pmPhase2Current = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2Voltage = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2RealPower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2ApparentPower = 0;              
              privateMibBase.pmGroup.pmTable[i].pmPhase2ReactivePower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2PowerFactor = 0;
              
              privateMibBase.pmGroup.pmTable[i].pmPhase3Current = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3Voltage = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3RealPower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3ApparentPower = 0;              
              privateMibBase.pmGroup.pmTable[i].pmPhase3ReactivePower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3PowerFactor = 0;
              
              sprintf(&privateMibBase.pmGroup.pmTable[i].pmModel[0],"%s",&sPMInfo[i].u8Model[0]);
              privateMibBase.pmGroup.pmTable[i].pmModelLen = 20;
              sprintf(&privateMibBase.pmGroup.pmTable[i].pmSerial[0],"%d",sPMInfo[i].u32SerialNumber);
              privateMibBase.pmGroup.pmTable[i].pmSerialLen = 20;
              
              privateMibBase.pmGroup.pmTable[i].pmType = sModbusManager.sPMManager[i].u8PMType;
              privateMibBase.pmGroup.pmTable[i].pmModbusID = sModbusManager.sPMManager[i].u32PMAbsSlaveID;
              
              if((privateMibBase.pmGroup.pmTable[i].pmFrequency != 0) && (privateMibBase.pmGroup.pmTable[i].pmEnableFreqTrap == 1))
              {
                //thanhcm3 fix ------------------------------------------------------------------------------------
                if(privateMibBase.pmGroup.pmTable[i].pmFrequency>=500){
                  delta_f_pm_struct._0 = privateMibBase.pmGroup.pmTable[i].pmFrequency -500;
                }else if(privateMibBase.pmGroup.pmTable[i].pmFrequency<500){
                  delta_f_pm_struct._0 = 500 - privateMibBase.pmGroup.pmTable[i].pmFrequency;
                }
                //thanhcm3 fix ------------------------------------------------------------------------------------
                if(delta_f_pm_struct._0<=privateMibBase.pmGroup.pmTable[i].pm_delta_freq_v){
                  if ((privateMibBase.pmGroup.pmTable[i].pmFrequency >= (500 + privateMibBase.pmGroup.pmTable[i].pmDeltaFreqDisConnect)) || 
                      (privateMibBase.pmGroup.pmTable[i].pmFrequency <= (500 - privateMibBase.pmGroup.pmTable[i].pmDeltaFreqDisConnect))){
                        cntInFrq = 0;
                        if(cntOutFrq++ > privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_out){
                          cntOutFrq = privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_out + 1;
                          privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 1;
                        }
                      }
                  else if ((privateMibBase.pmGroup.pmTable[i].pmFrequency <= (500 + privateMibBase.pmGroup.pmTable[i].pmDeltaFreqReConnect)) && 
                           (privateMibBase.pmGroup.pmTable[i].pmFrequency >= (500 - privateMibBase.pmGroup.pmTable[i].pmDeltaFreqReConnect))){
                             cntOutFrq = 0;
                             if(cntInFrq++ > privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_in){
                               cntInFrq = privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_in + 1;
                               privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
                             }
                           }
                }else {
                  cntOutFrq = 0;
                  cntInFrq  = 0;
                  privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
                  
                }
              }
              else {
                cntOutFrq = 0;
                cntInFrq  = 0;
                privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
              }
              
              privateMibBase.mainAlarmGroup.alarmOutOfRangeFreqPack[i] = privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq;
          }
          break;
          case 5://PILOT
          {
              privateMibBase.pmGroup.pmTable[i].pmID = i+1;
              privateMibBase.pmGroup.pmTable[i].pmImportActiveEnergy = (uint32_t) (sPMInfo[i].u32ImportActiveE*10);
              privateMibBase.pmGroup.pmTable[i].pmExportActiveEnergy = (uint32_t) (sPMInfo[i].u32ExportActiveE*10);
              privateMibBase.pmGroup.pmTable[i].pmImportReactiveEnergy = (uint32_t) (sPMInfo[i].u32ImportReActiveE*10) ;
              privateMibBase.pmGroup.pmTable[i].pmExportReactiveEnergy = (uint32_t) (sPMInfo[i].u32ExportReActiveE*10) ;              
              privateMibBase.pmGroup.pmTable[i].pmTotalActiveEnergy = (uint32_t) (sPMInfo[i].u32TotalActiveE*10) ;             
              privateMibBase.pmGroup.pmTable[i].pmTotalReactiveEnergy = (uint32_t) (sPMInfo[i].u32TotalReActiveE*10) ;             
              privateMibBase.pmGroup.pmTable[i].pmPowerFactor = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fPowerFactor*0.1) ; 
              privateMibBase.pmGroup.pmTable[i].pmFrequency = (uint32_t) (sPMInfo[i].fFrequency*0.1);
              privateMibBase.pmGroup.pmTable[i].pmTotalCurrent = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fCurrent*0.1);
              privateMibBase.pmGroup.pmTable[i].pmActivePower = (int32_t) (sPMInfo[i].sPhaseInfo[0].fActivePower/100);
              privateMibBase.pmGroup.pmTable[i].pmReactivePower = (int32_t) (sPMInfo[i].sPhaseInfo[0].fReactivePower/100);
              
              privateMibBase.pmGroup.pmTable[i].pmPhase1Current = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fCurrent*0.1);
              privateMibBase.pmGroup.pmTable[i].pmPhase1Voltage = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fVoltage);
              privateMibBase.pmGroup.pmTable[i].pmPhase1RealPower = (int32_t) (sPMInfo[i].sPhaseInfo[0].fActivePower/100);
              privateMibBase.pmGroup.pmTable[i].pmPhase1ApparentPower = (int32_t) (sPMInfo[i].sPhaseInfo[0].fApparentPower/100);              
              privateMibBase.pmGroup.pmTable[i].pmPhase1ReactivePower = (int32_t) (sPMInfo[i].sPhaseInfo[0].fReactivePower/100);
              privateMibBase.pmGroup.pmTable[i].pmPhase1PowerFactor = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fPowerFactor/10);
              
              privateMibBase.pmGroup.pmTable[i].pmPhase2Current = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2Voltage = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2RealPower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2ApparentPower = 0;              
              privateMibBase.pmGroup.pmTable[i].pmPhase2ReactivePower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2PowerFactor = 0;
              
              privateMibBase.pmGroup.pmTable[i].pmPhase3Current = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3Voltage = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3RealPower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3ApparentPower = 0;              
              privateMibBase.pmGroup.pmTable[i].pmPhase3ReactivePower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3PowerFactor = 0;
              
              privateMibBase.pmGroup.pmTable[i].pmType = sModbusManager.sPMManager[i].u8PMType;
              sprintf(&privateMibBase.pmGroup.pmTable[i].pmModel[0],"SPM91");
              privateMibBase.pmGroup.pmTable[i].pmModelLen = 5;
              sprintf(&privateMibBase.pmGroup.pmTable[i].pmSerial[0]," ");
              privateMibBase.pmGroup.pmTable[0].pmSerialLen = 1;
              privateMibBase.pmGroup.pmTable[i].pmModbusID = sModbusManager.sPMManager[i].u32PMAbsSlaveID;
              
              if((privateMibBase.pmGroup.pmTable[i].pmFrequency != 0) && (privateMibBase.pmGroup.pmTable[i].pmEnableFreqTrap == 1))
              {
                //thanhcm3 fix ------------------------------------------------------------------------------------
                if(privateMibBase.pmGroup.pmTable[i].pmFrequency>=500){
                  delta_f_pm_struct._0 = privateMibBase.pmGroup.pmTable[i].pmFrequency -500;
                }else if(privateMibBase.pmGroup.pmTable[i].pmFrequency<500){
                  delta_f_pm_struct._0 = 500 - privateMibBase.pmGroup.pmTable[i].pmFrequency;
                }
                //thanhcm3 fix ------------------------------------------------------------------------------------
                if(delta_f_pm_struct._0<=privateMibBase.pmGroup.pmTable[i].pm_delta_freq_v){
                  if ((privateMibBase.pmGroup.pmTable[i].pmFrequency >= (500 + privateMibBase.pmGroup.pmTable[i].pmDeltaFreqDisConnect)) || 
                      (privateMibBase.pmGroup.pmTable[i].pmFrequency <= (500 - privateMibBase.pmGroup.pmTable[i].pmDeltaFreqDisConnect))){
                        cntInFrq = 0;
                        if(cntOutFrq++ > privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_out){
                          cntOutFrq = privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_out + 1;
                          privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 1;
                        }
                      }
                  else if ((privateMibBase.pmGroup.pmTable[i].pmFrequency <= (500 + privateMibBase.pmGroup.pmTable[i].pmDeltaFreqReConnect)) && 
                           (privateMibBase.pmGroup.pmTable[i].pmFrequency >= (500 - privateMibBase.pmGroup.pmTable[i].pmDeltaFreqReConnect))){
                             cntOutFrq = 0;
                             if(cntInFrq++ > privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_in){
                               cntInFrq = privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_in + 1;
                               privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
                             }
                           }
                }else {
                  cntOutFrq = 0;
                  cntInFrq  = 0;
                  privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
                }
              }
              else {
                cntOutFrq = 0;
                cntInFrq  = 0;
                privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
              }
              
              privateMibBase.mainAlarmGroup.alarmOutOfRangeFreqPack[i] = privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq;
          }
          break;
          case 15://PILOT_3PHASE
          {
              privateMibBase.pmGroup.pmTable[i].pmID = i+1;
              privateMibBase.pmGroup.pmTable[i].pmImportActiveEnergy = (uint32_t) (sPMInfo[i].u32ImportActiveE);
              privateMibBase.pmGroup.pmTable[i].pmExportActiveEnergy = (uint32_t) (sPMInfo[i].u32ExportActiveE);
              privateMibBase.pmGroup.pmTable[i].pmImportReactiveEnergy = (uint32_t) (sPMInfo[i].u32ImportReActiveE) ;
              privateMibBase.pmGroup.pmTable[i].pmExportReactiveEnergy = (uint32_t) (sPMInfo[i].u32ExportReActiveE) ;              
              privateMibBase.pmGroup.pmTable[i].pmTotalActiveEnergy = (uint32_t) (sPMInfo[i].u32TotalActiveE) ;             
              privateMibBase.pmGroup.pmTable[i].pmTotalReactiveEnergy = (uint32_t) (sPMInfo[i].u32TotalReActiveE) ;             
              privateMibBase.pmGroup.pmTable[i].pmPowerFactor = (uint32_t) (sPMInfo[i].fPowerFactor) ; 
              privateMibBase.pmGroup.pmTable[i].pmFrequency = (uint32_t) (sPMInfo[i].fFrequency);
              privateMibBase.pmGroup.pmTable[i].pmTotalCurrent = (uint32_t) (sPMInfo[i].fTotalCurrent);
              privateMibBase.pmGroup.pmTable[i].pmActivePower = (uint32_t) sPMInfo[i].fRealPower;
              privateMibBase.pmGroup.pmTable[i].pmReactivePower = (uint32_t) sPMInfo[i].fReactivePower;
              
              privateMibBase.pmGroup.pmTable[i].pmPhase1Current = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fCurrent);
              privateMibBase.pmGroup.pmTable[i].pmPhase1Voltage = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fVoltage);
              privateMibBase.pmGroup.pmTable[i].pmPhase1RealPower = (int32_t) (sPMInfo[i].sPhaseInfo[0].fActivePower);
              privateMibBase.pmGroup.pmTable[i].pmPhase1ApparentPower = (int32_t) (sPMInfo[i].sPhaseInfo[0].fApparentPower);              
              privateMibBase.pmGroup.pmTable[i].pmPhase1ReactivePower = (int32_t) (sPMInfo[i].sPhaseInfo[0].fReactivePower);
              privateMibBase.pmGroup.pmTable[i].pmPhase1PowerFactor = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fPowerFactor);
              
              privateMibBase.pmGroup.pmTable[i].pmPhase2Current = (uint32_t) (sPMInfo[i].sPhaseInfo[1].fCurrent);
              privateMibBase.pmGroup.pmTable[i].pmPhase2Voltage = (uint32_t) (sPMInfo[i].sPhaseInfo[1].fVoltage);
              privateMibBase.pmGroup.pmTable[i].pmPhase2RealPower = (int32_t) (sPMInfo[i].sPhaseInfo[1].fActivePower);
              privateMibBase.pmGroup.pmTable[i].pmPhase2ApparentPower = (int32_t) (sPMInfo[i].sPhaseInfo[1].fApparentPower);              
              privateMibBase.pmGroup.pmTable[i].pmPhase2ReactivePower = (int32_t) (sPMInfo[i].sPhaseInfo[1].fReactivePower);
              privateMibBase.pmGroup.pmTable[i].pmPhase2PowerFactor = (uint32_t) (sPMInfo[i].sPhaseInfo[1].fPowerFactor);
              
              privateMibBase.pmGroup.pmTable[i].pmPhase3Current = (uint32_t) (sPMInfo[i].sPhaseInfo[2].fCurrent);
              privateMibBase.pmGroup.pmTable[i].pmPhase3Voltage = (uint32_t) (sPMInfo[i].sPhaseInfo[2].fVoltage);
              privateMibBase.pmGroup.pmTable[i].pmPhase3RealPower = (int32_t) (sPMInfo[i].sPhaseInfo[2].fActivePower);
              privateMibBase.pmGroup.pmTable[i].pmPhase3ApparentPower = (int32_t) (sPMInfo[i].sPhaseInfo[2].fApparentPower);              
              privateMibBase.pmGroup.pmTable[i].pmPhase3ReactivePower = (int32_t) (sPMInfo[i].sPhaseInfo[2].fReactivePower);
              privateMibBase.pmGroup.pmTable[i].pmPhase3PowerFactor = (uint32_t) (sPMInfo[i].sPhaseInfo[2].fPowerFactor);
              
              
              
              privateMibBase.pmGroup.pmTable[i].pmType = sModbusManager.sPMManager[i].u8PMType;
              sprintf(&privateMibBase.pmGroup.pmTable[i].pmModel[0],"SPM93");
              privateMibBase.pmGroup.pmTable[i].pmModelLen = 5;
              sprintf(&privateMibBase.pmGroup.pmTable[i].pmSerial[0],"%d",sPMInfo[i].u32SerialNumber);
              privateMibBase.pmGroup.pmTable[i].pmSerialLen = 20;
              privateMibBase.pmGroup.pmTable[i].pmModbusID = sModbusManager.sPMManager[i].u32PMAbsSlaveID;
              
              if((privateMibBase.pmGroup.pmTable[i].pmFrequency != 0) && (privateMibBase.pmGroup.pmTable[i].pmEnableFreqTrap == 1))
              {
                //thanhcm3 fix ------------------------------------------------------------------------------------
                if(privateMibBase.pmGroup.pmTable[i].pmFrequency>=500){
                  delta_f_pm_struct._0 = privateMibBase.pmGroup.pmTable[i].pmFrequency -500;
                }else if(privateMibBase.pmGroup.pmTable[i].pmFrequency<500){
                  delta_f_pm_struct._0 = 500 - privateMibBase.pmGroup.pmTable[i].pmFrequency;
                }
                //thanhcm3 fix ------------------------------------------------------------------------------------
                if(delta_f_pm_struct._0<=privateMibBase.pmGroup.pmTable[i].pm_delta_freq_v){
                  
                  if ((privateMibBase.pmGroup.pmTable[i].pmFrequency >= (500 + privateMibBase.pmGroup.pmTable[i].pmDeltaFreqDisConnect)) || 
                      (privateMibBase.pmGroup.pmTable[i].pmFrequency <= (500 - privateMibBase.pmGroup.pmTable[i].pmDeltaFreqDisConnect))){
                        cntInFrq = 0;
                        if(cntOutFrq++ > privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_out){
                          cntOutFrq = privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_out +1;
                          privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 1;
                        }
                      }
                  else if ((privateMibBase.pmGroup.pmTable[i].pmFrequency <= (500 + privateMibBase.pmGroup.pmTable[i].pmDeltaFreqReConnect)) && 
                           (privateMibBase.pmGroup.pmTable[i].pmFrequency >= (500 - privateMibBase.pmGroup.pmTable[i].pmDeltaFreqReConnect))){
                             cntOutFrq = 0;
                             if(cntInFrq++ > privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_in){
                               cntInFrq = privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_in + 1;
                               privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
                             }
                           }
                }else{
                  cntOutFrq = 0;
                  cntInFrq  = 0;
                  privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
                }
              }
              else {
                cntOutFrq = 0;
                cntInFrq  = 0;
                privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
              }
              
              privateMibBase.mainAlarmGroup.alarmOutOfRangeFreqPack[i] = privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq;
          }
          break;
          case 16: //YADA_3PHASE_DPC
          {
            privateMibBase.pmGroup.pmTable[i].pmID = i+1;
            privateMibBase.pmGroup.pmTable[i].pmImportActiveEnergy     = 0; //(uint32_t) (sPMInfo[i].u32ImportActiveE);
            privateMibBase.pmGroup.pmTable[i].pmExportActiveEnergy     = 0; //(uint32_t) (sPMInfo[i].u32ExportActiveE);
            privateMibBase.pmGroup.pmTable[i].pmImportReactiveEnergy   = 0; //(uint32_t) (sPMInfo[i].u32ImportReActiveE) ;
            privateMibBase.pmGroup.pmTable[i].pmExportReactiveEnergy   = 0; //(uint32_t) (sPMInfo[i].u32ExportReActiveE) ;              
            privateMibBase.pmGroup.pmTable[i].pmTotalActiveEnergy      = (uint32_t) (sPMInfo[i].u32TotalActiveE) ;             
            privateMibBase.pmGroup.pmTable[i].pmTotalReactiveEnergy    = (uint32_t) (sPMInfo[i].u32TotalReActiveE) ;             
            privateMibBase.pmGroup.pmTable[i].pmPowerFactor            = (uint32_t) (sPMInfo[i].fPowerFactor) ; 
            privateMibBase.pmGroup.pmTable[i].pmFrequency              = (uint32_t) (sPMInfo[i].fFrequency);
            privateMibBase.pmGroup.pmTable[i].pmTotalCurrent           = (uint32_t) (sPMInfo[i].fTotalCurrent);
            privateMibBase.pmGroup.pmTable[i].pmActivePower            = (uint32_t) sPMInfo[i].fRealPower;
            privateMibBase.pmGroup.pmTable[i].pmReactivePower          = (int32_t) sPMInfo[i].fReactivePower;
              
            privateMibBase.pmGroup.pmTable[i].pmPhase1Current          = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fCurrent);
            privateMibBase.pmGroup.pmTable[i].pmPhase1Voltage          = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fVoltage);
            privateMibBase.pmGroup.pmTable[i].pmPhase1RealPower        = (int32_t) (sPMInfo[i].sPhaseInfo[0].fActivePower);
            privateMibBase.pmGroup.pmTable[i].pmPhase1ApparentPower    = 0; // (int32_t) (sPMInfo[i].sPhaseInfo[0].fApparentPower);              
            privateMibBase.pmGroup.pmTable[i].pmPhase1ReactivePower    = (int32_t) (sPMInfo[i].sPhaseInfo[0].fReactivePower);
            privateMibBase.pmGroup.pmTable[i].pmPhase1PowerFactor      = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fPowerFactor);
              
            privateMibBase.pmGroup.pmTable[i].pmPhase2Current          = (uint32_t) (sPMInfo[i].sPhaseInfo[1].fCurrent);
            privateMibBase.pmGroup.pmTable[i].pmPhase2Voltage          = (uint32_t) (sPMInfo[i].sPhaseInfo[1].fVoltage);
            privateMibBase.pmGroup.pmTable[i].pmPhase2RealPower        = (int32_t) (sPMInfo[i].sPhaseInfo[1].fActivePower);
            privateMibBase.pmGroup.pmTable[i].pmPhase2ApparentPower    = 0; //(int32_t) (sPMInfo[i].sPhaseInfo[1].fApparentPower);              
            privateMibBase.pmGroup.pmTable[i].pmPhase2ReactivePower    = (int32_t) (sPMInfo[i].sPhaseInfo[1].fReactivePower);
            privateMibBase.pmGroup.pmTable[i].pmPhase2PowerFactor      = (uint32_t) (sPMInfo[i].sPhaseInfo[1].fPowerFactor);
              
            privateMibBase.pmGroup.pmTable[i].pmPhase3Current          = (uint32_t) (sPMInfo[i].sPhaseInfo[2].fCurrent);
            privateMibBase.pmGroup.pmTable[i].pmPhase3Voltage          = (uint32_t) (sPMInfo[i].sPhaseInfo[2].fVoltage);
            privateMibBase.pmGroup.pmTable[i].pmPhase3RealPower        = (int32_t) (sPMInfo[i].sPhaseInfo[2].fActivePower);
            privateMibBase.pmGroup.pmTable[i].pmPhase3ApparentPower    = 0; //(int32_t) (sPMInfo[i].sPhaseInfo[2].fApparentPower);              
            privateMibBase.pmGroup.pmTable[i].pmPhase3ReactivePower    = (int32_t) (sPMInfo[i].sPhaseInfo[2].fReactivePower);
            privateMibBase.pmGroup.pmTable[i].pmPhase3PowerFactor      = (uint32_t) (sPMInfo[i].sPhaseInfo[2].fPowerFactor);
            
            privateMibBase.pmGroup.pmTable[i].pmType = sModbusManager.sPMManager[i].u8PMType;
            sprintf(&privateMibBase.pmGroup.pmTable[i].pmModel[0],"YADA");
            privateMibBase.pmGroup.pmTable[i].pmModelLen = 4;
            privateMibBase.pmGroup.pmTable[i].pmModbusID = sModbusManager.sPMManager[i].u32PMAbsSlaveID;
            
            if((privateMibBase.pmGroup.pmTable[i].pmFrequency != 0) && (privateMibBase.pmGroup.pmTable[i].pmEnableFreqTrap == 1))
              {
                //thanhcm3 fix ------------------------------------------------------------------------------------
                if(privateMibBase.pmGroup.pmTable[i].pmFrequency>=500){
                  delta_f_pm_struct._0 = privateMibBase.pmGroup.pmTable[i].pmFrequency -500;
                }else if(privateMibBase.pmGroup.pmTable[i].pmFrequency<500){
                  delta_f_pm_struct._0 = 500 - privateMibBase.pmGroup.pmTable[i].pmFrequency;
                }
                //thanhcm3 fix ------------------------------------------------------------------------------------
                if(delta_f_pm_struct._0<=privateMibBase.pmGroup.pmTable[i].pm_delta_freq_v){
                  if ((privateMibBase.pmGroup.pmTable[i].pmFrequency >= (500 + privateMibBase.pmGroup.pmTable[i].pmDeltaFreqDisConnect)) || 
                      (privateMibBase.pmGroup.pmTable[i].pmFrequency <= (500 - privateMibBase.pmGroup.pmTable[i].pmDeltaFreqDisConnect))){
                        cntInFrq = 0;
                        if(cntOutFrq++ > privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_out){
                          cntOutFrq = privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_out +1;
                          privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 1;
                        }
                      }
                  else if ((privateMibBase.pmGroup.pmTable[i].pmFrequency <= (500 + privateMibBase.pmGroup.pmTable[i].pmDeltaFreqReConnect)) && 
                           (privateMibBase.pmGroup.pmTable[i].pmFrequency >= (500 - privateMibBase.pmGroup.pmTable[i].pmDeltaFreqReConnect))){
                             cntOutFrq = 0;
                             if(cntInFrq++ > privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_in){
                               cntInFrq = privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_in + 1;
                               privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
                             }
                           }
                }else{
                  cntOutFrq = 0;
                  cntInFrq  = 0;
                  privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
                }
              }
              else {
                cntOutFrq = 0;
                cntInFrq  = 0;
                privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
              }
              
              privateMibBase.mainAlarmGroup.alarmOutOfRangeFreqPack[i] = privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq;
            
          }
          break;
        case 17://Schneider 2022
          {
            privateMibBase.pmGroup.pmTable[i].pmID                     =  i+1;
            privateMibBase.pmGroup.pmTable[i].pmTotalActiveEnergy      = (uint32_t) (sPMInfo[i].u32TotalActiveE) ;             
            privateMibBase.pmGroup.pmTable[i].pmTotalReactiveEnergy    = (uint32_t) (sPMInfo[i].u32TotalReActiveE) ;             
            privateMibBase.pmGroup.pmTable[i].pmPowerFactor            = (uint32_t) (sPMInfo[i].fPowerFactor) ; 
            privateMibBase.pmGroup.pmTable[i].pmFrequency              = (uint32_t) (sPMInfo[i].fFrequency);
            privateMibBase.pmGroup.pmTable[i].pmActivePower            = (uint32_t) sPMInfo[i].fRealPower;
            privateMibBase.pmGroup.pmTable[i].pmReactivePower          = (int32_t) sPMInfo[i].fReactivePower;
            privateMibBase.pmGroup.pmTable[i].pmTotalCurrent           = (uint32_t) sPMInfo[i].fTotalCurrent;

            
            privateMibBase.pmGroup.pmTable[i].pmPhase1Current          = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fCurrent);
            privateMibBase.pmGroup.pmTable[i].pmPhase1Voltage          = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fVoltage);
            privateMibBase.pmGroup.pmTable[i].pmPhase1RealPower        = (int32_t) (sPMInfo[i].sPhaseInfo[0].fActivePower);
            privateMibBase.pmGroup.pmTable[i].pmPhase1ApparentPower    = (int32_t) (sPMInfo[i].sPhaseInfo[0].fApparentPower);              
            privateMibBase.pmGroup.pmTable[i].pmPhase1ReactivePower    = (int32_t) (sPMInfo[i].sPhaseInfo[0].fReactivePower);
            privateMibBase.pmGroup.pmTable[i].pmPhase1PowerFactor      = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fPowerFactor);
              
            privateMibBase.pmGroup.pmTable[i].pmPhase2Current          = (uint32_t) (sPMInfo[i].sPhaseInfo[1].fCurrent);
            privateMibBase.pmGroup.pmTable[i].pmPhase2Voltage          = (uint32_t) (sPMInfo[i].sPhaseInfo[1].fVoltage);
            privateMibBase.pmGroup.pmTable[i].pmPhase2RealPower        = (int32_t) (sPMInfo[i].sPhaseInfo[1].fActivePower);
            privateMibBase.pmGroup.pmTable[i].pmPhase2ApparentPower    = (int32_t) (sPMInfo[i].sPhaseInfo[1].fApparentPower);              
            privateMibBase.pmGroup.pmTable[i].pmPhase2ReactivePower    = (int32_t) (sPMInfo[i].sPhaseInfo[1].fReactivePower);
            privateMibBase.pmGroup.pmTable[i].pmPhase2PowerFactor      = (uint32_t) (sPMInfo[i].sPhaseInfo[1].fPowerFactor);
              
            privateMibBase.pmGroup.pmTable[i].pmPhase3Current          = (uint32_t) (sPMInfo[i].sPhaseInfo[2].fCurrent);
            privateMibBase.pmGroup.pmTable[i].pmPhase3Voltage          = (uint32_t) (sPMInfo[i].sPhaseInfo[2].fVoltage);
            privateMibBase.pmGroup.pmTable[i].pmPhase3RealPower        = (int32_t) (sPMInfo[i].sPhaseInfo[2].fActivePower);
            privateMibBase.pmGroup.pmTable[i].pmPhase3ApparentPower    = (int32_t) (sPMInfo[i].sPhaseInfo[2].fApparentPower);              
            privateMibBase.pmGroup.pmTable[i].pmPhase3ReactivePower    = (int32_t) (sPMInfo[i].sPhaseInfo[2].fReactivePower);
            privateMibBase.pmGroup.pmTable[i].pmPhase3PowerFactor      = (uint32_t) (sPMInfo[i].sPhaseInfo[2].fPowerFactor);
            
            privateMibBase.pmGroup.pmTable[i].pmType                   = sModbusManager.sPMManager[i].u8PMType;
            privateMibBase.pmGroup.pmTable[i].pmModbusID               = sModbusManager.sPMManager[i].u32PMAbsSlaveID;
              
            
            
            sprintf(&privateMibBase.pmGroup.pmTable[i].pmSerial[0],"%d%s%d",sPMInfo[i].id_SerialNumber,"0",sPMInfo[i].u32SerialNumber);
            privateMibBase.pmGroup.pmTable[i].pmSerialLen = 12;
            sprintf(&privateMibBase.pmGroup.pmTable[i].pmModel[0],"%s","PM2230");//&sPMInfo[i].u8Model[0]
            privateMibBase.pmGroup.pmTable[i].pmModelLen = 6;
            
            if((privateMibBase.pmGroup.pmTable[i].pmFrequency != 0) && (privateMibBase.pmGroup.pmTable[i].pmEnableFreqTrap == 1))
            {
              //thanhcm3 fix ------------------------------------------------------------------------------------
              if(privateMibBase.pmGroup.pmTable[i].pmFrequency>=500){
                delta_f_pm_struct._0 = privateMibBase.pmGroup.pmTable[i].pmFrequency -500;
              }else if(privateMibBase.pmGroup.pmTable[i].pmFrequency<500){
                delta_f_pm_struct._0 = 500 - privateMibBase.pmGroup.pmTable[i].pmFrequency;
              }
              //thanhcm3 fix ------------------------------------------------------------------------------------
              if(delta_f_pm_struct._0<=privateMibBase.pmGroup.pmTable[i].pm_delta_freq_v){
                if ((privateMibBase.pmGroup.pmTable[i].pmFrequency >= (500 + privateMibBase.pmGroup.pmTable[i].pmDeltaFreqDisConnect)) || 
                    (privateMibBase.pmGroup.pmTable[i].pmFrequency <= (500 - privateMibBase.pmGroup.pmTable[i].pmDeltaFreqDisConnect))){
                      cntInFrq = 0;
                      if(cntOutFrq++ > privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_out){
                        cntOutFrq = privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_out + 1;
                        privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 1;
                      }
                    }
                else if ((privateMibBase.pmGroup.pmTable[i].pmFrequency <= (500 + privateMibBase.pmGroup.pmTable[i].pmDeltaFreqReConnect)) && 
                         (privateMibBase.pmGroup.pmTable[i].pmFrequency >= (500 - privateMibBase.pmGroup.pmTable[i].pmDeltaFreqReConnect))){
                           cntOutFrq = 0;
                           if(cntInFrq++ > privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_in){
                             cntInFrq = privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_in + 1;
                             privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
                           }
                         }
              }else{
                cntOutFrq = 0;
                cntInFrq  = 0;
                privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
              }
            }
            else {
              cntOutFrq = 0;
              cntInFrq  = 0;
              privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
            }
            
            privateMibBase.mainAlarmGroup.alarmOutOfRangeFreqPack[i] = privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq;
            
          
          }
          break;
          case 18://EASTRON SMD72D 2022
            {
              privateMibBase.pmGroup.pmTable[i].pmImportActiveEnergy     = (uint32_t) (sPMInfo[i].u32ImportActiveE);
              privateMibBase.pmGroup.pmTable[i].pmExportActiveEnergy     = (uint32_t) (sPMInfo[i].u32ExportActiveE);              
              privateMibBase.pmGroup.pmTable[i].pmID                     = i+1;
              privateMibBase.pmGroup.pmTable[i].pmTotalActiveEnergy      = (uint32_t) (sPMInfo[i].u32TotalActiveE) ;             
              privateMibBase.pmGroup.pmTable[i].pmTotalReactiveEnergy    = (uint32_t) (sPMInfo[i].u32TotalReActiveE) ;             
              privateMibBase.pmGroup.pmTable[i].pmPowerFactor            = (uint32_t) (sPMInfo[i].fPowerFactor) ; 
              privateMibBase.pmGroup.pmTable[i].pmFrequency              = (uint32_t) (sPMInfo[i].fFrequency);
              privateMibBase.pmGroup.pmTable[i].pmTotalCurrent           = (uint32_t) (sPMInfo[i].fTotalCurrent);            
              privateMibBase.pmGroup.pmTable[i].pmActivePower            = (uint32_t) sPMInfo[i].fRealPower;
              privateMibBase.pmGroup.pmTable[i].pmReactivePower          = (int32_t) sPMInfo[i].fReactivePower;
              
              
              privateMibBase.pmGroup.pmTable[i].pmPhase1Current          = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fCurrent);
              privateMibBase.pmGroup.pmTable[i].pmPhase1Voltage          = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fVoltage);
              privateMibBase.pmGroup.pmTable[i].pmPhase1RealPower        = (int32_t) (sPMInfo[i].sPhaseInfo[0].fActivePower);
              privateMibBase.pmGroup.pmTable[i].pmPhase1ApparentPower    =  (int32_t) (sPMInfo[i].sPhaseInfo[0].fApparentPower);              
              privateMibBase.pmGroup.pmTable[i].pmPhase1ReactivePower    = (int32_t) (sPMInfo[i].sPhaseInfo[0].fReactivePower);
              privateMibBase.pmGroup.pmTable[i].pmPhase1PowerFactor      = (uint32_t) (sPMInfo[i].sPhaseInfo[0].fPowerFactor);
              
              privateMibBase.pmGroup.pmTable[i].pmPhase2Current          = (uint32_t) (sPMInfo[i].sPhaseInfo[1].fCurrent);
              privateMibBase.pmGroup.pmTable[i].pmPhase2Voltage          = (uint32_t) (sPMInfo[i].sPhaseInfo[1].fVoltage);
              privateMibBase.pmGroup.pmTable[i].pmPhase2RealPower        = (int32_t) (sPMInfo[i].sPhaseInfo[1].fActivePower);
              privateMibBase.pmGroup.pmTable[i].pmPhase2ApparentPower    = (int32_t) (sPMInfo[i].sPhaseInfo[1].fApparentPower);              
              privateMibBase.pmGroup.pmTable[i].pmPhase2ReactivePower    = (int32_t) (sPMInfo[i].sPhaseInfo[1].fReactivePower);
              privateMibBase.pmGroup.pmTable[i].pmPhase2PowerFactor      = (uint32_t) (sPMInfo[i].sPhaseInfo[1].fPowerFactor);
              
              privateMibBase.pmGroup.pmTable[i].pmPhase3Current          = (uint32_t) (sPMInfo[i].sPhaseInfo[2].fCurrent);
              privateMibBase.pmGroup.pmTable[i].pmPhase3Voltage          = (uint32_t) (sPMInfo[i].sPhaseInfo[2].fVoltage);
              privateMibBase.pmGroup.pmTable[i].pmPhase3RealPower        = (int32_t) (sPMInfo[i].sPhaseInfo[2].fActivePower);
              privateMibBase.pmGroup.pmTable[i].pmPhase3ApparentPower    = (int32_t) (sPMInfo[i].sPhaseInfo[2].fApparentPower);              
              privateMibBase.pmGroup.pmTable[i].pmPhase3ReactivePower    = (int32_t) (sPMInfo[i].sPhaseInfo[2].fReactivePower);
              privateMibBase.pmGroup.pmTable[i].pmPhase3PowerFactor      = (uint32_t) (sPMInfo[i].sPhaseInfo[2].fPowerFactor);
              
              privateMibBase.pmGroup.pmTable[i].pmType                   = sModbusManager.sPMManager[i].u8PMType;
              privateMibBase.pmGroup.pmTable[i].pmModbusID               = sModbusManager.sPMManager[i].u32PMAbsSlaveID;
              
              sprintf(&privateMibBase.pmGroup.pmTable[i].pmSerial[0],"%d",sPMInfo[i].u32SerialNumber);  
              privateMibBase.pmGroup.pmTable[i].pmSerialLen = 8;
              sprintf(&privateMibBase.pmGroup.pmTable[i].pmModel[0],"SDM72D");
              privateMibBase.pmGroup.pmTable[i].pmModelLen = 6;
              
              if((privateMibBase.pmGroup.pmTable[i].pmFrequency != 0) && (privateMibBase.pmGroup.pmTable[i].pmEnableFreqTrap == 1))
              {
                //thanhcm3 fix ------------------------------------------------------------------------------------
                if(privateMibBase.pmGroup.pmTable[i].pmFrequency>=500){
                  delta_f_pm_struct._0 = privateMibBase.pmGroup.pmTable[i].pmFrequency -500;
                }else if(privateMibBase.pmGroup.pmTable[i].pmFrequency<500){
                  delta_f_pm_struct._0 = 500 - privateMibBase.pmGroup.pmTable[i].pmFrequency;
                }
                //thanhcm3 fix ------------------------------------------------------------------------------------
                if(delta_f_pm_struct._0<=privateMibBase.pmGroup.pmTable[i].pm_delta_freq_v){
                  if ((privateMibBase.pmGroup.pmTable[i].pmFrequency >= (500 + privateMibBase.pmGroup.pmTable[i].pmDeltaFreqDisConnect)) || 
                      (privateMibBase.pmGroup.pmTable[i].pmFrequency <= (500 - privateMibBase.pmGroup.pmTable[i].pmDeltaFreqDisConnect))){
                        cntInFrq = 0;
                        if(cntOutFrq++ > privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_out){
                          cntOutFrq = privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_out + 1;
                          privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 1;
                        }
                      }
                  else if ((privateMibBase.pmGroup.pmTable[i].pmFrequency <= (500 + privateMibBase.pmGroup.pmTable[i].pmDeltaFreqReConnect)) && 
                           (privateMibBase.pmGroup.pmTable[i].pmFrequency >= (500 - privateMibBase.pmGroup.pmTable[i].pmDeltaFreqReConnect))){
                             cntOutFrq = 0;
                             if(cntInFrq++ > privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_in){
                               cntInFrq = privateMibBase.pmGroup.pmTable[i].pm_cnt_timeout_freq_in + 1;
                               privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
                             }
                           }
                }else{
                  cntOutFrq = 0;
                  cntInFrq  = 0;
                  privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
                }
              }
              else {
                cntOutFrq = 0;
                cntInFrq  = 0;
                privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq = 0;
              }
              
              privateMibBase.mainAlarmGroup.alarmOutOfRangeFreqPack[i] = privateMibBase.pmGroup.pmTable[i].pmOutOfRangeFreq;
            }
            break;            
          default:
            break;
          };
      }
      else
      {
              privateMibBase.pmGroup.pmTable[i].pmID = i+1;
              privateMibBase.pmGroup.pmTable[i].pmImportActiveEnergy = 0;
              privateMibBase.pmGroup.pmTable[i].pmExportActiveEnergy = 0;
              privateMibBase.pmGroup.pmTable[i].pmImportReactiveEnergy = 0;
              privateMibBase.pmGroup.pmTable[i].pmExportReactiveEnergy = 0;            
              privateMibBase.pmGroup.pmTable[i].pmTotalActiveEnergy = 0;          
              privateMibBase.pmGroup.pmTable[i].pmTotalReactiveEnergy = 0;          
              privateMibBase.pmGroup.pmTable[i].pmPowerFactor = 0;
              privateMibBase.pmGroup.pmTable[i].pmFrequency = 0;
              privateMibBase.pmGroup.pmTable[i].pmTotalCurrent = 0;
              privateMibBase.pmGroup.pmTable[i].pmActivePower = 0;
              privateMibBase.pmGroup.pmTable[i].pmReactivePower =0;
              
              privateMibBase.pmGroup.pmTable[i].pmPhase1Current = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase1Voltage = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase1RealPower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase1ApparentPower = 0;              
              privateMibBase.pmGroup.pmTable[i].pmPhase1ReactivePower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase1PowerFactor = 0;
              
              privateMibBase.pmGroup.pmTable[i].pmPhase2Current = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2Voltage = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2RealPower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2ApparentPower = 0;              
              privateMibBase.pmGroup.pmTable[i].pmPhase2ReactivePower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2PowerFactor = 0;
              
              privateMibBase.pmGroup.pmTable[i].pmPhase3Current = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3Voltage = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3RealPower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3ApparentPower = 0;              
              privateMibBase.pmGroup.pmTable[i].pmPhase3ReactivePower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3PowerFactor = 0;
         
              sprintf(&privateMibBase.pmGroup.pmTable[i].pmModel[0]," ");
              privateMibBase.pmGroup.pmTable[0].pmSerialLen = 1;
              privateMibBase.pmGroup.pmTable[0].pmModelLen = 1;
              sprintf(&privateMibBase.pmGroup.pmTable[i].pmSerial[0]," ");
              
              privateMibBase.mainAlarmGroup.alarmOutOfRangeFreqPack[i] = 0;
              
              privateMibBase.pmGroup.pmTable[i].pmType = sModbusManager.sPMManager[i].u8PMType;
              privateMibBase.pmGroup.pmTable[i].pmModbusID = sModbusManager.sPMManager[i].u32PMAbsSlaveID;
      }
      privateMibBase.pmGroup.pmTable[i].pm_remaining_gen = remainning_gen;
  }
  //clear------------------------------------------------------------------------------------------------------
  for(i = privateMibBase.pmGroup.pmInstalledPM;i<1;i++){
              privateMibBase.pmGroup.pmTable[i].pmStatus =0;
                
              privateMibBase.pmGroup.pmTable[i].pmID = i+1;
              privateMibBase.pmGroup.pmTable[i].pmImportActiveEnergy = 0;
              privateMibBase.pmGroup.pmTable[i].pmExportActiveEnergy = 0;
              privateMibBase.pmGroup.pmTable[i].pmImportReactiveEnergy = 0;
              privateMibBase.pmGroup.pmTable[i].pmExportReactiveEnergy = 0;            
              privateMibBase.pmGroup.pmTable[i].pmTotalActiveEnergy = 0;          
              privateMibBase.pmGroup.pmTable[i].pmTotalReactiveEnergy = 0;          
              privateMibBase.pmGroup.pmTable[i].pmPowerFactor = 0;
              privateMibBase.pmGroup.pmTable[i].pmFrequency = 0;
              privateMibBase.pmGroup.pmTable[i].pmTotalCurrent = 0;
              privateMibBase.pmGroup.pmTable[i].pmActivePower = 0;
              privateMibBase.pmGroup.pmTable[i].pmReactivePower =0;
              
              privateMibBase.pmGroup.pmTable[i].pmPhase1Current = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase1Voltage = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase1RealPower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase1ApparentPower = 0;              
              privateMibBase.pmGroup.pmTable[i].pmPhase1ReactivePower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase1PowerFactor = 0;
              
              privateMibBase.pmGroup.pmTable[i].pmPhase2Current = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2Voltage = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2RealPower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2ApparentPower = 0;              
              privateMibBase.pmGroup.pmTable[i].pmPhase2ReactivePower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase2PowerFactor = 0;
              
              privateMibBase.pmGroup.pmTable[i].pmPhase3Current = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3Voltage = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3RealPower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3ApparentPower = 0;              
              privateMibBase.pmGroup.pmTable[i].pmPhase3ReactivePower = 0;
              privateMibBase.pmGroup.pmTable[i].pmPhase3PowerFactor = 0;
         
              sprintf(&privateMibBase.pmGroup.pmTable[i].pmModel[0]," ");
              privateMibBase.pmGroup.pmTable[0].pmSerialLen = 1;
              privateMibBase.pmGroup.pmTable[0].pmModelLen = 1;
              sprintf(&privateMibBase.pmGroup.pmTable[i].pmSerial[0]," ");
              
              privateMibBase.mainAlarmGroup.alarmOutOfRangeFreqPack[i] = 0;
              privateMibBase.pmGroup.pmTable[i].pm_remaining_gen        = 0;
              
              privateMibBase.pmGroup.pmTable[i].pmType = sModbusManager.sPMManager[i].u8PMType;
              privateMibBase.pmGroup.pmTable[i].pmModbusID = sModbusManager.sPMManager[i].u32PMAbsSlaveID;
    
  }

}
#endif
#if (USERDEF_MONITOR_VAC == ENABLED)
void UpdateVACInfo (void)
{
  uint32_t j,i=0;
  
  privateMibBase.vacGroup.vacInstalledVAC = sModbusManager.u8NumberOfVAC;
  privateMibBase.connAlarmGroup.alarmVACConnect = 0;
  privateMibBase.mainAlarmGroup.alarmVAC = 0;
  privateMibBase.vacGroup.vacActiveVAC = privateMibBase.vacGroup.vacInstalledVAC;
  for (i = 0; i < privateMibBase.vacGroup.vacInstalledVAC; i++)
  {
      privateMibBase.mainAlarmGroup.alarmVACPack[i] = 0;
      if (sVACInfo[i].u8ErrorFlag == 1)
      {
        if(cntDisConnectVAC[i] > 2){ // mat ket noi 3 lan moi bao len
          cntDisConnectVAC[i] = 3;
          privateMibBase.vacGroup.vacActiveVAC--;
          if (privateMibBase.vacGroup.vacActiveVAC <= 0)
          {
            privateMibBase.vacGroup.vacActiveVAC = 0;
          }
          privateMibBase.connAlarmGroup.alarmVACConnect |= (sVACInfo[i].u8ErrorFlag << i);
          privateMibBase.vacGroup.vacTable[i].vacStatus = 170;
        }

      }
      else
      {        
        cntDisConnectVAC[i] = 0;
        
            if(privateMibBase.vacGroup.vacTable[i].vacAlarmStatus.all != 0)
            {
                privateMibBase.vacGroup.vacTable[i].vacStatus = 165;
            }
            else
            {
                privateMibBase.vacGroup.vacTable[i].vacStatus = 255;
            }
      }

      if (privateMibBase.vacGroup.vacTable[i].vacStatus != 170)
      {
          if (privateMibBase.vacGroup.vacTable[i].vacStatus == 165)
          {
              privateMibBase.mainAlarmGroup.alarmVACPack[i] = privateMibBase.vacGroup.vacTable[i].vacAlarmStatus.all;//1;
          }

          switch(sModbusManager.sVACManager[i].u8VACType)
          {
          case 1:// VIETTEL
          {
              privateMibBase.vacGroup.vacTable[i].vacID = i+1;
              if(VacResetOK == 0){
                privateMibBase.vacGroup.vacTable[i].vacAlarmStatus.all = (uint32_t) (sVACInfo[i].u32AlarmStatus);
              }
              privateMibBase.vacGroup.vacTable[i].vacInTemp = (int32_t) (sVACInfo[i].i16InTemp);
              privateMibBase.vacGroup.vacTable[i].vacExTemp = (int32_t) (sVACInfo[i].i16ExTemp);
              privateMibBase.vacGroup.vacTable[i].vacFrost1Temp = (int32_t) (sVACInfo[i].i16Frost1Temp);
              privateMibBase.vacGroup.vacTable[i].vacFrost2Temp = (int32_t) (sVACInfo[i].i16Frost2Temp);
              privateMibBase.vacGroup.vacTable[i].vacHumid = (uint32_t) (sVACInfo[i].u32Humid);
              privateMibBase.vacGroup.vacTable[i].vacFan1Duty = (uint32_t) (sVACInfo[i].u32Fan1Duty);
              privateMibBase.vacGroup.vacTable[i].vacFan1RPM = (uint32_t) (sVACInfo[i].u32Fan1RPM);
              privateMibBase.vacGroup.vacTable[i].vacFan2Duty = (uint32_t) (sVACInfo[i].u32Fan2Duty);
              privateMibBase.vacGroup.vacTable[i].vacFan2RPM = (uint32_t) (sVACInfo[i].u32Fan2RPM);
              
              privateMibBase.vacGroup.vacTable[i].vacSysMode = (uint32_t) (sVACInfo[i].u32SysMode);
              privateMibBase.vacGroup.vacTable[i].vacFanSpeedMode = (uint32_t) (sVACInfo[i].u32FanSpeedMode);
              privateMibBase.vacGroup.vacTable[i].vacFanStartTemp = (int32_t) (sVACInfo[i].i16FanStartTemp);
              privateMibBase.vacGroup.vacTable[i].vacSetTemp = (int32_t) (sVACInfo[i].i16SetTemp);
              privateMibBase.vacGroup.vacTable[i].vacPidOffsetTemp = (int32_t) (sVACInfo[i].i16PidOffsetTemp);
              privateMibBase.vacGroup.vacTable[i].vacFanMinSpeed = (uint32_t) (sVACInfo[i].u32FanMinSpeed);
              privateMibBase.vacGroup.vacTable[i].vacFanMaxSpeed = (uint32_t) (sVACInfo[i].u32FanMaxSpeed);
              privateMibBase.vacGroup.vacTable[i].vacFilterStuckTemp = (int32_t) (sVACInfo[i].i16FilterStuckTemp);
              privateMibBase.vacGroup.vacTable[i].vacNightModeEn = (uint32_t) (sVACInfo[i].u32NightModeEn);
              privateMibBase.vacGroup.vacTable[i].vacNightModeStart = (uint32_t) (sVACInfo[i].u32NightModeStart);
              privateMibBase.vacGroup.vacTable[i].vacNightModeEnd = (uint32_t) (sVACInfo[i].u32NightModeEnd);
              privateMibBase.vacGroup.vacTable[i].vacNightMaxSpeed = (uint32_t) (sVACInfo[i].u32NightMaxSpeed);
              privateMibBase.vacGroup.vacTable[i].vacManualMode = (uint32_t) (sVACInfo[i].u32ManualMode);
              privateMibBase.vacGroup.vacTable[i].vacManualMaxSpeed = (uint32_t) (sVACInfo[i].u32ManualMaxSpeed);
              privateMibBase.vacGroup.vacTable[i].vacInMaxTemp = (int32_t) (sVACInfo[i].i16InMaxTemp);
              privateMibBase.vacGroup.vacTable[i].vacExMaxTemp = (int32_t) (sVACInfo[i].i16ExMaxTemp);
              privateMibBase.vacGroup.vacTable[i].vacFrostMaxTemp = (int32_t) (sVACInfo[i].i16FrostMaxTemp);
              privateMibBase.vacGroup.vacTable[i].vacInMinTemp = (int32_t) (sVACInfo[i].i16InMinTemp);
              privateMibBase.vacGroup.vacTable[i].vacExMinTemp = (int32_t) (sVACInfo[i].i16ExMinTemp);
              privateMibBase.vacGroup.vacTable[i].vacFrostMinTemp = (int32_t) (sVACInfo[i].i16FrostMinTemp);
              privateMibBase.vacGroup.vacTable[i].vacMinOutTemp = (int32_t) (sVACInfo[i].i16MinOutTemp);
              privateMibBase.vacGroup.vacTable[i].vacDeltaTemp = (int32_t) (sVACInfo[i].i16DeltaTemp);
              privateMibBase.vacGroup.vacTable[i].vacPanicTemp = (int32_t) (sVACInfo[i].i16PanicTemp);
              privateMibBase.vacGroup.vacTable[i].vacACU1OnTemp = (int32_t) (sVACInfo[i].i16ACU1OnTemp);
              privateMibBase.vacGroup.vacTable[i].vacACU2OnTemp = (int32_t) (sVACInfo[i].i16ACU2OnTemp);
              privateMibBase.vacGroup.vacTable[i].vacACU2En = (uint32_t) (sVACInfo[i].u32ACU2En);
              
 //             privateMibBase.vacGroup.vacTable[i].vacActiveFan = (uint32_t) (sVACInfo[i].u16ActiveFan);
              if(privateMibBase.vacGroup.vacTable[i].vacFan1RPM == 0) privateMibBase.vacGroup.vacTable[i].vacActiveFan = 0;
              else privateMibBase.vacGroup.vacTable[i].vacActiveFan = 1;
              privateMibBase.vacGroup.vacTable[i].vacInstalledFan = (uint32_t) (sVACInfo[i].u16InstalledFan);
              privateMibBase.vacGroup.vacTable[i].vacInstalledAirCon = (uint32_t) (sVACInfo[i].u16InstalledAirCon);
              
              sprintf(&privateMibBase.vacGroup.vacTable[i].cSyncTime[0], "%02d:%02d:%02d-%02d/%02d/%02d",
                  SyncTime.hour, 
                  SyncTime.min, 
                  SyncTime.sec, 
                  SyncTime.date, 
                  SyncTime.month, 
                  SyncTime.year);
              privateMibBase.vacGroup.vacTable[i].cSyncTimeLen = 20;
              
              sprintf(&privateMibBase.vacGroup.vacTable[i].vacSerial[0], "%04d%05d",
                  sVACInfo[i].u16SN_Hi, 
                  sVACInfo[i].u16SN_Lo);                  
              privateMibBase.vacGroup.vacTable[i].vacSerialLen = 10;
              
              privateMibBase.vacGroup.vacTable[i].vacAirCon1Model = (uint32_t) (sVACInfo[i].u32AirCon1Model);
              privateMibBase.vacGroup.vacTable[i].vacAirCon1Type = (uint32_t) (sVACInfo[i].u32AirCon1Type);
              privateMibBase.vacGroup.vacTable[i].vacAirCon2Model = (uint32_t) (sVACInfo[i].u32AirCon2Model);
              privateMibBase.vacGroup.vacTable[i].vacAirCon2Type = (uint32_t) (sVACInfo[i].u32AirCon2Type);
              privateMibBase.vacGroup.vacTable[i].vacAirConOnOff = (uint32_t) (sVACInfo[i].u32AirConOnOff);
              privateMibBase.vacGroup.vacTable[i].vacAirConMode = (uint32_t) (sVACInfo[i].u32AirConMode);
              privateMibBase.vacGroup.vacTable[i].vacAirConTemp = (int32_t) (sVACInfo[i].i16AirConTemp);
              privateMibBase.vacGroup.vacTable[i].vacAirConSpeed = (uint32_t) (sVACInfo[i].u32AirConSpeed);
              privateMibBase.vacGroup.vacTable[i].vacAircondDir = (uint32_t) (sVACInfo[i].u32AircondDir);
              privateMibBase.vacGroup.vacTable[i].vacAirConVolt = (uint32_t) (sVACInfo[i].u32AirConVolt);
              privateMibBase.vacGroup.vacTable[i].vacAirConCurrent = (uint32_t) (sVACInfo[i].u32AirConCurrent);
              privateMibBase.vacGroup.vacTable[i].vacAirConPower = (uint32_t) (sVACInfo[i].u32AirConPower);
              privateMibBase.vacGroup.vacTable[i].vacAirConFrequency = (uint32_t) (sVACInfo[i].u32AirConFrequency);
              privateMibBase.vacGroup.vacTable[i].vacAirConPF = (uint32_t) (sVACInfo[i].u32AirConPF);
              privateMibBase.vacGroup.vacTable[i].vacFirmVersion = (uint32_t) (sVACInfo[i].u16FirmVersion);
              privateMibBase.vacGroup.vacTable[i].vacWEnable = (uint32_t) (sVACInfo[i].u32WEnable);
              privateMibBase.vacGroup.vacTable[i].vacWSerial1 = (uint32_t) (sVACInfo[i].u32WSerial1);
              privateMibBase.vacGroup.vacTable[i].vacWSerial2 = (uint32_t) (sVACInfo[i].u32WSerial2);
              
              privateMibBase.vacGroup.vacTable[i].vacType = sModbusManager.sVACManager[i].u8VACType;
              privateMibBase.vacGroup.vacTable[i].vacModbusID = sModbusManager.sVACManager[i].u32VACAbsSlaveID;   
          }
          break;
          };
      }
      else
      {
//              memset(&privateMibBase.vacGroup.vacTable[i], 0, 1 * sizeof(PrivateMibVACInfoEntry));
              privateMibBase.vacGroup.vacTable[i].vacAlarmStatus.all = 0;
              privateMibBase.vacGroup.vacTable[i].vacStatus = 170;
              privateMibBase.vacGroup.vacTable[i].vacType = sModbusManager.sVACManager[i].u8VACType;
              privateMibBase.vacGroup.vacTable[i].vacModbusID = sModbusManager.sVACManager[i].u32VACAbsSlaveID;
              
              privateMibBase.vacGroup.vacTable[i].vacInTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacExTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacFrost1Temp = 0;
              privateMibBase.vacGroup.vacTable[i].vacFrost2Temp = 0;
              privateMibBase.vacGroup.vacTable[i].vacHumid = 0;
              privateMibBase.vacGroup.vacTable[i].vacFan1Duty = 0;
              privateMibBase.vacGroup.vacTable[i].vacFan1RPM = 0;
              privateMibBase.vacGroup.vacTable[i].vacFan2Duty = 0;
              privateMibBase.vacGroup.vacTable[i].vacFan2RPM = 0;
              privateMibBase.vacGroup.vacTable[i].vacSysMode = 0;
              privateMibBase.vacGroup.vacTable[i].vacFanSpeedMode = 0;
              privateMibBase.vacGroup.vacTable[i].vacFanStartTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacSetTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacPidOffsetTemp =0;
              privateMibBase.vacGroup.vacTable[i].vacFanMinSpeed = 0;
              privateMibBase.vacGroup.vacTable[i].vacFanMaxSpeed = 0;
              privateMibBase.vacGroup.vacTable[i].vacFilterStuckTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacNightModeEn = 0;
              privateMibBase.vacGroup.vacTable[i].vacNightModeStart = 0;
              privateMibBase.vacGroup.vacTable[i].vacNightModeEnd = 0;
              privateMibBase.vacGroup.vacTable[i].vacNightMaxSpeed = 0;
              privateMibBase.vacGroup.vacTable[i].vacManualMode = 0;
              privateMibBase.vacGroup.vacTable[i].vacManualMaxSpeed = 0;
              privateMibBase.vacGroup.vacTable[i].vacInMaxTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacExMaxTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacFrostMaxTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacInMinTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacExMinTemp =0;
              privateMibBase.vacGroup.vacTable[i].vacFrostMinTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacMinOutTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacDeltaTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacPanicTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacACU1OnTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacACU2OnTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacACU2En = 0;
              
              privateMibBase.vacGroup.vacTable[i].vacActiveFan = 0;
              privateMibBase.vacGroup.vacTable[i].vacInstalledFan = 0;
              privateMibBase.vacGroup.vacTable[i].vacInstalledAirCon = 0;
              
              sprintf(&privateMibBase.vacGroup.vacTable[i].cSyncTime[0], "%02d:%02d:%02d-%02d/%02d/%02d",
                  SyncTime.hour, 
                  SyncTime.min, 
                  SyncTime.sec, 
                  SyncTime.date, 
                  SyncTime.month, 
                  SyncTime.year);
              privateMibBase.vacGroup.vacTable[i].cSyncTimeLen = 20;
              
              sprintf(&privateMibBase.vacGroup.vacTable[i].vacSerial[0], " ");                  
              privateMibBase.vacGroup.vacTable[i].vacSerialLen = 1;
              
              privateMibBase.vacGroup.vacTable[i].vacAirCon1Model = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirCon1Type = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirCon2Model = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirCon2Type = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirConOnOff = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirConMode = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirConTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirConSpeed = 0;
              privateMibBase.vacGroup.vacTable[i].vacAircondDir = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirConVolt = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirConCurrent = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirConPower = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirConFrequency = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirConPF = 0;
              privateMibBase.vacGroup.vacTable[i].vacFirmVersion = 0;
              privateMibBase.vacGroup.vacTable[i].vacWEnable = 0;
              privateMibBase.vacGroup.vacTable[i].vacWSerial1 = 0;
              privateMibBase.vacGroup.vacTable[i].vacWSerial2 = 0;
              
              
      }
  }
  //clean VAC-------------------------------------------------------------------
  for (i= privateMibBase.vacGroup.vacInstalledVAC;i<1;i++){
              privateMibBase.vacGroup.vacTable[i].vacAlarmStatus.all = 0;
              privateMibBase.vacGroup.vacTable[i].vacStatus = 0;//170;
              privateMibBase.vacGroup.vacTable[i].vacType = sModbusManager.sVACManager[i].u8VACType;
              privateMibBase.vacGroup.vacTable[i].vacModbusID = sModbusManager.sVACManager[i].u32VACAbsSlaveID;
              
              privateMibBase.vacGroup.vacTable[i].vacInTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacExTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacFrost1Temp = 0;
              privateMibBase.vacGroup.vacTable[i].vacFrost2Temp = 0;
              privateMibBase.vacGroup.vacTable[i].vacHumid = 0;
              privateMibBase.vacGroup.vacTable[i].vacFan1Duty = 0;
              privateMibBase.vacGroup.vacTable[i].vacFan1RPM = 0;
              privateMibBase.vacGroup.vacTable[i].vacFan2Duty = 0;
              privateMibBase.vacGroup.vacTable[i].vacFan2RPM = 0;
              privateMibBase.vacGroup.vacTable[i].vacSysMode = 0;
              privateMibBase.vacGroup.vacTable[i].vacFanSpeedMode = 0;
              privateMibBase.vacGroup.vacTable[i].vacFanStartTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacSetTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacPidOffsetTemp =0;
              privateMibBase.vacGroup.vacTable[i].vacFanMinSpeed = 0;
              privateMibBase.vacGroup.vacTable[i].vacFanMaxSpeed = 0;
              privateMibBase.vacGroup.vacTable[i].vacFilterStuckTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacNightModeEn = 0;
              privateMibBase.vacGroup.vacTable[i].vacNightModeStart = 0;
              privateMibBase.vacGroup.vacTable[i].vacNightModeEnd = 0;
              privateMibBase.vacGroup.vacTable[i].vacNightMaxSpeed = 0;
              privateMibBase.vacGroup.vacTable[i].vacManualMode = 0;
              privateMibBase.vacGroup.vacTable[i].vacManualMaxSpeed = 0;
              privateMibBase.vacGroup.vacTable[i].vacInMaxTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacExMaxTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacFrostMaxTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacInMinTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacExMinTemp =0;
              privateMibBase.vacGroup.vacTable[i].vacFrostMinTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacMinOutTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacDeltaTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacPanicTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacACU1OnTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacACU2OnTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacACU2En = 0;
              
              privateMibBase.vacGroup.vacTable[i].vacActiveFan = 0;
              privateMibBase.vacGroup.vacTable[i].vacInstalledFan = 0;
              privateMibBase.vacGroup.vacTable[i].vacInstalledAirCon = 0;
              
              sprintf(&privateMibBase.vacGroup.vacTable[i].cSyncTime[0], "%02d:%02d:%02d-%02d/%02d/%02d",
                  SyncTime.hour, 
                  SyncTime.min, 
                  SyncTime.sec, 
                  SyncTime.date, 
                  SyncTime.month, 
                  SyncTime.year);
              privateMibBase.vacGroup.vacTable[i].cSyncTimeLen = 20;
              
              sprintf(&privateMibBase.vacGroup.vacTable[i].vacSerial[0], " ");                  
              privateMibBase.vacGroup.vacTable[i].vacSerialLen = 1;
              
              privateMibBase.vacGroup.vacTable[i].vacAirCon1Model = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirCon1Type = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirCon2Model = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirCon2Type = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirConOnOff = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirConMode = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirConTemp = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirConSpeed = 0;
              privateMibBase.vacGroup.vacTable[i].vacAircondDir = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirConVolt = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirConCurrent = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirConPower = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirConFrequency = 0;
              privateMibBase.vacGroup.vacTable[i].vacAirConPF = 0;
              privateMibBase.vacGroup.vacTable[i].vacFirmVersion = 0;
              privateMibBase.vacGroup.vacTable[i].vacWEnable = 0;
              privateMibBase.vacGroup.vacTable[i].vacWSerial1 = 0;
              privateMibBase.vacGroup.vacTable[i].vacWSerial2 = 0;
  }

}
#endif
#if (USERDEF_MONITOR_SMCB == ENABLED) //smcb
void UpdateSMCBInfo (void)
{
  uint32_t j,i=0;
  
  privateMibBase.smcbGroup.SmcbInstalledSMCB = sModbusManager.u8NumberOfSMCB;
  privateMibBase.connAlarmGroup.alarmSMCBConnect = 0;
  privateMibBase.mainAlarmGroup.alarmSMCB = 0;
  privateMibBase.smcbGroup.SmcbActiveSMCB = privateMibBase.smcbGroup.SmcbInstalledSMCB;
  for (i = 0; i < privateMibBase.smcbGroup.SmcbInstalledSMCB; i++)
  {
      privateMibBase.mainAlarmGroup.alarmSMCBPack[i] = 0;
      if (sSMCBInfo[i].u8ErrorFlag == 1)
      {
        if(cntDisConnectSMCB[i] > 2){ // mat ket noi 3 lan moi bao len
          cntDisConnectSMCB[i] = 3;
          privateMibBase.smcbGroup.SmcbActiveSMCB--;
          if (privateMibBase.smcbGroup.SmcbActiveSMCB <= 0)
          {
            privateMibBase.smcbGroup.SmcbActiveSMCB = 0;
          }
          privateMibBase.connAlarmGroup.alarmSMCBConnect |= (sSMCBInfo[i].u8ErrorFlag << i);
          privateMibBase.smcbGroup.SmcbTable[i].SmcbStatus = 170;
        }
      }
      else
      {        
        cntDisConnectSMCB[i] = 0;
          privateMibBase.smcbGroup.SmcbTable[i].SmcbStatus = 255;
      }

      if (privateMibBase.smcbGroup.SmcbTable[i].SmcbStatus != 170)
      {
          switch(sModbusManager.sSMCBManager[i].u8SMCBType)
          {
          case 1:// OPEN
          {
              privateMibBase.smcbGroup.SmcbTable[i].SmcbStatusID = i+1;
              privateMibBase.smcbGroup.SmcbTable[i].SmcbType = sModbusManager.sSMCBManager[i].u8SMCBType;
              if(sModbusManager.SettingCommand != SET_SMCB_STATE){
                privateMibBase.smcbGroup.SmcbTable[i].SmcbState = sSMCBInfo[i].u32State;
              }
              privateMibBase.smcbGroup.SmcbTable[i].SmcbModbusID = sModbusManager.sSMCBManager[i].u32SMCBAbsSlaveID;
              privateMibBase.mainAlarmGroup.alarmSMCBPack[i] = privateMibBase.smcbGroup.SmcbTable[i].SmcbState;
          }
          break;
          case 2:// MATIS
          {
              privateMibBase.smcbGroup.SmcbTable[i].SmcbStatusID = i+1;
              privateMibBase.smcbGroup.SmcbTable[i].SmcbType = sModbusManager.sSMCBManager[i].u8SMCBType;
              if(sModbusManager.SettingCommand != SET_SMCB_STATE){
                if((sSMCBInfo[i].u32State == 1)||(sSMCBInfo[i].u32State == 2)||(sSMCBInfo[i].u32State == 20))
                  privateMibBase.smcbGroup.SmcbTable[i].SmcbState = 0;
                else if((sSMCBInfo[i].u32State == 10)||(sSMCBInfo[i].u32State == 11)||(sSMCBInfo[i].u32State == 30))
                  privateMibBase.smcbGroup.SmcbTable[i].SmcbState = 1;
                else
                  privateMibBase.smcbGroup.SmcbTable[i].SmcbState = 2;
              }
              privateMibBase.smcbGroup.SmcbTable[i].SmcbModbusID = sModbusManager.sSMCBManager[i].u32SMCBAbsSlaveID;
              privateMibBase.mainAlarmGroup.alarmSMCBPack[i] = privateMibBase.smcbGroup.SmcbTable[i].SmcbState;
          }
          break;
          case 3:// GOL
          {
              privateMibBase.smcbGroup.SmcbTable[i].SmcbStatusID = i+1;
              privateMibBase.smcbGroup.SmcbTable[i].SmcbType = sModbusManager.sSMCBManager[i].u8SMCBType;
              if(sModbusManager.SettingCommand != SET_SMCB_STATE){
                if(sSMCBInfo[i].u32State == 1)
                  privateMibBase.smcbGroup.SmcbTable[i].SmcbState = 0;
                else if(sSMCBInfo[i].u32State == 2)
                  privateMibBase.smcbGroup.SmcbTable[i].SmcbState = 1;
                else
                  privateMibBase.smcbGroup.SmcbTable[i].SmcbState = 2;
              }
              privateMibBase.smcbGroup.SmcbTable[i].SmcbModbusID = sModbusManager.sSMCBManager[i].u32SMCBAbsSlaveID;
              privateMibBase.mainAlarmGroup.alarmSMCBPack[i] = privateMibBase.smcbGroup.SmcbTable[i].SmcbState;
          }
          break;
          };
      }
      else
      {
          privateMibBase.smcbGroup.SmcbTable[i].SmcbStatus = 170;   
          privateMibBase.smcbGroup.SmcbTable[i].SmcbType = sModbusManager.sSMCBManager[i].u8SMCBType;
          privateMibBase.smcbGroup.SmcbTable[i].SmcbState = 0;
          privateMibBase.smcbGroup.SmcbTable[i].SmcbModbusID = sModbusManager.sSMCBManager[i].u32SMCBAbsSlaveID;    
      }
  }

}
#endif
#if (USERDEF_MONITOR_FUEL == ENABLED) //fuel
void UpdateFUELInfo (void)
{
  uint32_t j,i=0;
  
  privateMibBase.fuelGroup.FuelInstalledFUEL = sModbusManager.u8NumberOfFUEL;
  privateMibBase.connAlarmGroup.alarmFUELConnect = 0;
  privateMibBase.mainAlarmGroup.alarmFUEL = 0;
  privateMibBase.fuelGroup.FuelActiveFUEL = privateMibBase.fuelGroup.FuelInstalledFUEL;
  for (i = 0; i < privateMibBase.fuelGroup.FuelInstalledFUEL; i++)
  {
      privateMibBase.mainAlarmGroup.alarmFUELPack[i] = 0;
      if (sFUELInfo[i].u8ErrorFlag == 1)
      {
        if(cntDisConnectFUEL[i] > 2){ // mat ket noi 3 lan moi bao len
          cntDisConnectFUEL[i] = 3;
          privateMibBase.fuelGroup.FuelActiveFUEL--;
          if (privateMibBase.fuelGroup.FuelActiveFUEL <= 0)
          {
            privateMibBase.fuelGroup.FuelActiveFUEL = 0;
          }
          privateMibBase.connAlarmGroup.alarmFUELConnect |= (sFUELInfo[i].u8ErrorFlag << i);
          privateMibBase.fuelGroup.FuelTable[i].FuelStatus = 170;
        }
      }
      else
      {        
        cntDisConnectFUEL[i] = 0;
          privateMibBase.fuelGroup.FuelTable[i].FuelStatus = 255;
      }

      if (privateMibBase.fuelGroup.FuelTable[i].FuelStatus != 170)
      {
          switch(sModbusManager.sFUELManager[i].u8FUELType)
          {
          case 1:// HPT621
          {
              privateMibBase.fuelGroup.FuelTable[i].FuelStatusID = i+1;
              privateMibBase.fuelGroup.FuelTable[i].FuelType = sModbusManager.sFUELManager[i].u8FUELType;
              privateMibBase.fuelGroup.FuelTable[i].FuelLevel = sFUELInfo[i].u32FuelLevel;
              privateMibBase.fuelGroup.FuelTable[i].FuelModbusID = sModbusManager.sFUELManager[i].u32FUELAbsSlaveID;
          }
          break;
          };
      }
      else
      {
          privateMibBase.fuelGroup.FuelTable[i].FuelStatus = 170;   
          privateMibBase.fuelGroup.FuelTable[i].FuelType = sModbusManager.sFUELManager[i].u8FUELType;
          privateMibBase.fuelGroup.FuelTable[i].FuelModbusID = sModbusManager.sFUELManager[i].u32FUELAbsSlaveID;    
          privateMibBase.fuelGroup.FuelTable[i].FuelLevel = 0;
      }
      
  }
  //clean-------------------------------------------------------------------------------------------------------
  for(i = privateMibBase.fuelGroup.FuelInstalledFUEL; i<2;i++){
          privateMibBase.fuelGroup.FuelTable[i].FuelStatus = 0;   
          privateMibBase.fuelGroup.FuelTable[i].FuelType = sModbusManager.sFUELManager[i].u8FUELType;
          privateMibBase.fuelGroup.FuelTable[i].FuelModbusID = sModbusManager.sFUELManager[i].u32FUELAbsSlaveID;    
          privateMibBase.fuelGroup.FuelTable[i].FuelLevel = 0; 
  }

}
#endif

#if (USERDEF_MONITOR_ISENSE == ENABLED)

uint32_t cntOutFrq2, cntInFrq2;
DELTA_STRUCT delta_f_isense_struct; 
void UpdateISENSEInfo (void)
{
  uint32_t j,i=0;
  
  privateMibBase.isenseGroup.isenseInstalledISENSE = sModbusManager.u8NumberOfISENSE;
  privateMibBase.connAlarmGroup.alarmISENSEConnect = 0;
  privateMibBase.mainAlarmGroup.alarmISENSE = 0;
  privateMibBase.isenseGroup.isenseActiveISENSE = privateMibBase.isenseGroup.isenseInstalledISENSE;
  for (i = 0; i < privateMibBase.isenseGroup.isenseInstalledISENSE; i++)
  {
    privateMibBase.mainAlarmGroup.alarmISENSEPack[i] = 0;
      if (sISENSEInfo[i].u8ErrorFlag == 1)
      {
        if(cntDisConnectISENSE[i] > 2){ // mat ket noi 3 lan moi bao len
          cntDisConnectISENSE[i] = 3;
          privateMibBase.isenseGroup.isenseActiveISENSE--;
          if (privateMibBase.isenseGroup.isenseActiveISENSE <= 0)
          {
            privateMibBase.isenseGroup.isenseActiveISENSE = 0;
          }
          privateMibBase.connAlarmGroup.alarmISENSEConnect |= (sISENSEInfo[i].u8ErrorFlag << i);
          privateMibBase.isenseGroup.isenseTable.isenseStatus = 170;
          privateMibBase.mainAlarmGroup.alarmISENSEPack[i]=privateMibBase.connAlarmGroup.alarmISENSEConnect; // them trap bao mat ket noi
        }
      }
      else
      {
        cntDisConnectISENSE[i] = 0;
          privateMibBase.isenseGroup.isenseTable.isenseStatus = 255;
      }

      if (privateMibBase.isenseGroup.isenseTable.isenseStatus != 170)
      {
          if (privateMibBase.isenseGroup.isenseTable.isenseStatus == 165)
          {
              privateMibBase.mainAlarmGroup.alarmISENSE = 1;
          }

          switch(sModbusManager.sISENSEManager[i].u8ISENSEType)
          {
          case 1://FORLONG
            {
              privateMibBase.isenseGroup.isenseTable.isenseID = i+1;
              
              privateMibBase.isenseGroup.isenseTable.isenseImportActiveEnergy = 0;
              privateMibBase.isenseGroup.isenseTable.isenseExportActiveEnergy = 0;
              privateMibBase.isenseGroup.isenseTable.isenseImportReactiveEnergy = 0 ;
              privateMibBase.isenseGroup.isenseTable.isenseExportReactiveEnergy = 0 ;              
              privateMibBase.isenseGroup.isenseTable.isenseTotalActiveEnergy = (uint32_t) (sISENSEInfo[i].u32TotalActiveE) ;             
              privateMibBase.isenseGroup.isenseTable.isenseTotalReactiveEnergy = (uint32_t) (sISENSEInfo[i].u32TotalReActiveE) ;             
              privateMibBase.isenseGroup.isenseTable.isensePowerFactor = (uint32_t) (sISENSEInfo[i].fPowerFactor/10) ; 
              privateMibBase.isenseGroup.isenseTable.isenseFrequency = (uint32_t) (sISENSEInfo[i].fFrequency/10);
              privateMibBase.isenseGroup.isenseTable.isenseCurrent = (uint32_t) (sISENSEInfo[i].fCurrent);
              privateMibBase.isenseGroup.isenseTable.isenseActivePower = (int32_t) (sISENSEInfo[i].fActivePower*10);
              privateMibBase.isenseGroup.isenseTable.isenseReactivePower = 0;
              privateMibBase.isenseGroup.isenseTable.isenseVoltage = (uint32_t) (sISENSEInfo[i].fVoltage*10);
              
              
              privateMibBase.isenseGroup.isenseTable.isenseType = sModbusManager.sISENSEManager[i].u8ISENSEType;
              sprintf(&privateMibBase.isenseGroup.isenseTable.isenseModel[0],"DRS-205C");
              privateMibBase.isenseGroup.isenseTable.isenseModelLen = 8;
              sprintf(&privateMibBase.isenseGroup.isenseTable.isenseSerial[0],"%08d",sISENSEInfo[i].u32SerialNumber);
              privateMibBase.isenseGroup.isenseTable.isenseSerialLen = 8;
              privateMibBase.isenseGroup.isenseTable.isenseModbusID = sModbusManager.sISENSEManager[i].u32ISENSEAbsSlaveID;
              
              if((privateMibBase.isenseGroup.isenseTable.isenseFrequency != 0) && (privateMibBase.isenseGroup.isenseTable.isenseEnableFreqTrap == 1))
              {
                //thanhcm3 fix -------------------------------------------------
                if(privateMibBase.isenseGroup.isenseTable.isenseFrequency>=500){
                  delta_f_isense_struct._0 = privateMibBase.isenseGroup.isenseTable.isenseFrequency - 500;
                }else if(privateMibBase.isenseGroup.isenseTable.isenseFrequency<500){
                  delta_f_isense_struct._0 = 500 - privateMibBase.isenseGroup.isenseTable.isenseFrequency;
                }
                //thanhcm3 fix -------------------------------------------------
                if(delta_f_isense_struct._0<=privateMibBase.isenseGroup.isenseTable.isense_delta_freq_v){
                  
                  if ((privateMibBase.isenseGroup.isenseTable.isenseFrequency >= (500 + privateMibBase.isenseGroup.isenseTable.isenseDeltaFreqDisConnect)) || 
                      (privateMibBase.isenseGroup.isenseTable.isenseFrequency <= (500 - privateMibBase.isenseGroup.isenseTable.isenseDeltaFreqDisConnect))){
                        cntInFrq2 = 0;
                        if(cntOutFrq2++ > privateMibBase.isenseGroup.isenseTable.isense_cnt_timeout_freq_out){
                          cntOutFrq2 = privateMibBase.isenseGroup.isenseTable.isense_cnt_timeout_freq_out +1;
                          privateMibBase.isenseGroup.isenseTable.isenseOutOfRangeFreq = 1;
                        }
                      }
                  else if ((privateMibBase.isenseGroup.isenseTable.isenseFrequency <= (500 + privateMibBase.isenseGroup.isenseTable.isenseDeltaFreqReConnect)) && 
                           (privateMibBase.isenseGroup.isenseTable.isenseFrequency >= (500 - privateMibBase.isenseGroup.isenseTable.isenseDeltaFreqReConnect))){
                             cntOutFrq2 = 0;
                             if(cntInFrq2++ > privateMibBase.isenseGroup.isenseTable.isense_cnt_timeout_freq_in){
                               cntInFrq2 = privateMibBase.isenseGroup.isenseTable.isense_cnt_timeout_freq_in +1;
                               privateMibBase.isenseGroup.isenseTable.isenseOutOfRangeFreq = 0;
                             }
                           }
                }else{
                  
                  cntOutFrq2 = 0;
                  cntInFrq2  = 0;
                  privateMibBase.isenseGroup.isenseTable.isenseOutOfRangeFreq = 0;
                  
                }
                
              }
              else {
                cntOutFrq2 = 0;
                cntInFrq2  = 0;
                privateMibBase.isenseGroup.isenseTable.isenseOutOfRangeFreq = 0;
              }
              
              privateMibBase.mainAlarmGroup.alarmOutOfRangeFreqPack2[i] = privateMibBase.isenseGroup.isenseTable.isenseOutOfRangeFreq;
            }
            break;
          case 2:// IVY_DDS353H_2
            {
              privateMibBase.isenseGroup.isenseTable.isenseID = i+1;
              privateMibBase.isenseGroup.isenseTable.isenseImportActiveEnergy = 0;
              privateMibBase.isenseGroup.isenseTable.isenseExportActiveEnergy = 0;
              privateMibBase.isenseGroup.isenseTable.isenseImportReactiveEnergy = 0 ;
              privateMibBase.isenseGroup.isenseTable.isenseExportReactiveEnergy = 0 ; 
                           
              privateMibBase.isenseGroup.isenseTable.isenseTotalActiveEnergy = (uint32_t) (sISENSEInfo[i].u32TotalActiveE) ;             
              privateMibBase.isenseGroup.isenseTable.isenseTotalReactiveEnergy = (uint32_t) (sISENSEInfo[i].u32TotalReActiveE) ;             
              privateMibBase.isenseGroup.isenseTable.isensePowerFactor = (uint32_t) (sISENSEInfo[i].fPowerFactor) ; 
              privateMibBase.isenseGroup.isenseTable.isenseFrequency = (uint32_t) (sISENSEInfo[i].fFrequency);
              privateMibBase.isenseGroup.isenseTable.isenseCurrent = (uint32_t) (sISENSEInfo[i].fCurrent*10);
              privateMibBase.isenseGroup.isenseTable.isenseActivePower = (int32_t) (sISENSEInfo[i].fActivePower);
              privateMibBase.isenseGroup.isenseTable.isenseReactivePower = (int32_t)sISENSEInfo[i].fReactivePower;
              privateMibBase.isenseGroup.isenseTable.isenseVoltage = (uint32_t) (sISENSEInfo[i].fVoltage);
              privateMibBase.isenseGroup.isenseTable.isenseApparentPower = (int32_t)sISENSEInfo[i].fApparentPower;
              
              privateMibBase.isenseGroup.isenseTable.isenseType = sModbusManager.sISENSEManager[i].u8ISENSEType;
              privateMibBase.isenseGroup.isenseTable.isenseModbusID = sModbusManager.sISENSEManager[i].u32ISENSEAbsSlaveID;
              sprintf(&privateMibBase.isenseGroup.isenseTable.isenseSerial[0],"%05d%05d%05d%05d%05d%05d",sISENSEInfo[i].serial_IVY[0],sISENSEInfo[i].serial_IVY[1],sISENSEInfo[i].serial_IVY[2],sISENSEInfo[i].serial_IVY[3],sISENSEInfo[i].serial_IVY[4],sISENSEInfo[i].serial_IVY[5]);  
              privateMibBase.isenseGroup.isenseTable.isenseSerialLen = 15;
              sprintf(&privateMibBase.isenseGroup.isenseTable.isenseModel[0],"DDS353H-2");
              privateMibBase.isenseGroup.isenseTable.isenseModelLen = 9;
              
              if((privateMibBase.isenseGroup.isenseTable.isenseFrequency != 0) && (privateMibBase.isenseGroup.isenseTable.isenseEnableFreqTrap == 1))
              {
                //thanhcm3 fix -------------------------------------------------
                if(privateMibBase.isenseGroup.isenseTable.isenseFrequency>=500){
                  delta_f_isense_struct._0 = privateMibBase.isenseGroup.isenseTable.isenseFrequency - 500;
                }else if(privateMibBase.isenseGroup.isenseTable.isenseFrequency<500){
                  delta_f_isense_struct._0 = 500 - privateMibBase.isenseGroup.isenseTable.isenseFrequency;
                }
                //thanhcm3 fix -------------------------------------------------
                if(delta_f_isense_struct._0<=privateMibBase.isenseGroup.isenseTable.isense_delta_freq_v){
                  
                  if ((privateMibBase.isenseGroup.isenseTable.isenseFrequency >= (500 + privateMibBase.isenseGroup.isenseTable.isenseDeltaFreqDisConnect)) || 
                      (privateMibBase.isenseGroup.isenseTable.isenseFrequency <= (500 - privateMibBase.isenseGroup.isenseTable.isenseDeltaFreqDisConnect))){
                        cntInFrq2 = 0;
                        if(cntOutFrq2++ > privateMibBase.isenseGroup.isenseTable.isense_cnt_timeout_freq_out){
                          cntOutFrq2 = privateMibBase.isenseGroup.isenseTable.isense_cnt_timeout_freq_out +1;
                          privateMibBase.isenseGroup.isenseTable.isenseOutOfRangeFreq = 1;
                        }
                      }
                  else if ((privateMibBase.isenseGroup.isenseTable.isenseFrequency <= (500 + privateMibBase.isenseGroup.isenseTable.isenseDeltaFreqReConnect)) && 
                           (privateMibBase.isenseGroup.isenseTable.isenseFrequency >= (500 - privateMibBase.isenseGroup.isenseTable.isenseDeltaFreqReConnect))){
                             cntOutFrq2 = 0;
                             if(cntInFrq2++ > privateMibBase.isenseGroup.isenseTable.isense_cnt_timeout_freq_in){
                               cntInFrq2 = privateMibBase.isenseGroup.isenseTable.isense_cnt_timeout_freq_in +1 ;
                               privateMibBase.isenseGroup.isenseTable.isenseOutOfRangeFreq = 0;
                             }
                           }
                }else{
                  
                  cntOutFrq2 = 0;
                  cntInFrq2  = 0;
                  privateMibBase.isenseGroup.isenseTable.isenseOutOfRangeFreq = 0;
                  
                }
                
              }
              else {
                cntOutFrq2 = 0;
                cntInFrq2  = 0;
                privateMibBase.isenseGroup.isenseTable.isenseOutOfRangeFreq = 0;
              }
              
              privateMibBase.mainAlarmGroup.alarmOutOfRangeFreqPack2[i] = privateMibBase.isenseGroup.isenseTable.isenseOutOfRangeFreq;
            }
            break;
          default:
            break;
          };
      }
      else
      {
              privateMibBase.isenseGroup.isenseTable.isenseID = i+1;
              privateMibBase.isenseGroup.isenseTable.isenseImportActiveEnergy = 0;
              privateMibBase.isenseGroup.isenseTable.isenseExportActiveEnergy = 0;
              privateMibBase.isenseGroup.isenseTable.isenseImportReactiveEnergy = 0;
              privateMibBase.isenseGroup.isenseTable.isenseExportReactiveEnergy = 0;            
              privateMibBase.isenseGroup.isenseTable.isenseTotalActiveEnergy = 0;          
              privateMibBase.isenseGroup.isenseTable.isenseTotalReactiveEnergy = 0;          
              privateMibBase.isenseGroup.isenseTable.isensePowerFactor = 0;
              privateMibBase.isenseGroup.isenseTable.isenseFrequency = 0;
              privateMibBase.isenseGroup.isenseTable.isenseCurrent = 0;
              privateMibBase.isenseGroup.isenseTable.isenseActivePower = 0;
              privateMibBase.isenseGroup.isenseTable.isenseVoltage = 0;
         
              sprintf(&privateMibBase.isenseGroup.isenseTable.isenseModel[0]," ");
              privateMibBase.isenseGroup.isenseTable.isenseSerialLen = 1;
              privateMibBase.isenseGroup.isenseTable.isenseModelLen = 1;
              sprintf(&privateMibBase.isenseGroup.isenseTable.isenseSerial[0]," ");
              
              privateMibBase.mainAlarmGroup.alarmOutOfRangeFreqPack2[i] = 0;
              
              privateMibBase.isenseGroup.isenseTable.isenseType = sModbusManager.sISENSEManager[i].u8ISENSEType;
              privateMibBase.isenseGroup.isenseTable.isenseModbusID = sModbusManager.sISENSEManager[i].u32ISENSEAbsSlaveID;
      }
  }
  //clean----------------------------------------------------------------------------------------------------------------
  for(i = privateMibBase.isenseGroup.isenseInstalledISENSE; i<1;i++){
              privateMibBase.isenseGroup.isenseTable.isenseStatus =0;
                
              privateMibBase.isenseGroup.isenseTable.isenseID = i+1;
              privateMibBase.isenseGroup.isenseTable.isenseImportActiveEnergy = 0;
              privateMibBase.isenseGroup.isenseTable.isenseExportActiveEnergy = 0;
              privateMibBase.isenseGroup.isenseTable.isenseImportReactiveEnergy = 0;
              privateMibBase.isenseGroup.isenseTable.isenseExportReactiveEnergy = 0;            
              privateMibBase.isenseGroup.isenseTable.isenseTotalActiveEnergy = 0;          
              privateMibBase.isenseGroup.isenseTable.isenseTotalReactiveEnergy = 0;          
              privateMibBase.isenseGroup.isenseTable.isensePowerFactor = 0;
              privateMibBase.isenseGroup.isenseTable.isenseFrequency = 0;
              privateMibBase.isenseGroup.isenseTable.isenseCurrent = 0;
              privateMibBase.isenseGroup.isenseTable.isenseActivePower = 0;
              privateMibBase.isenseGroup.isenseTable.isenseVoltage = 0;
         
              sprintf(&privateMibBase.isenseGroup.isenseTable.isenseModel[0]," ");
              privateMibBase.isenseGroup.isenseTable.isenseSerialLen = 1;
              privateMibBase.isenseGroup.isenseTable.isenseModelLen = 1;
              sprintf(&privateMibBase.isenseGroup.isenseTable.isenseSerial[0]," ");
              
              privateMibBase.mainAlarmGroup.alarmOutOfRangeFreqPack2[i] = 0;
              
              privateMibBase.isenseGroup.isenseTable.isenseType = sModbusManager.sISENSEManager[i].u8ISENSEType;
              privateMibBase.isenseGroup.isenseTable.isenseModbusID = sModbusManager.sISENSEManager[i].u32ISENSEAbsSlaveID;
    
  }

}
#endif

#if (USERDEF_MONITOR_PM_DC == ENABLED)
void Update_PM_DC_Info (void){
  uint32_t j,i=0;
  privateMibBase.pm_dc_group.pm_dc_installed_PM_DC = sModbusManager.u8_number_of_pm_dc;
  privateMibBase.connAlarmGroup.alarm_pm_dc_connect = 0;
  privateMibBase.mainAlarmGroup.alarm_pm_dc = 0;
  privateMibBase.pm_dc_group.pm_dc_active_PM_DC = privateMibBase.pm_dc_group.pm_dc_installed_PM_DC;
  for(i = 0; i < privateMibBase.pm_dc_group.pm_dc_installed_PM_DC; i++){
     privateMibBase.mainAlarmGroup.alarm_pm_dc_pack[i] = 0;
      if (s_pm_dc_info[i].u8_error_flag == 1)
      {
        if(cnt_disConnect_pm_dc[i] > 2){ // mat ket noi 3 lan moi bao len
          cnt_disConnect_pm_dc[i] = 3;
          privateMibBase.pm_dc_group.pm_dc_active_PM_DC--;
          if (privateMibBase.pm_dc_group.pm_dc_active_PM_DC <= 0)
          {
            privateMibBase.pm_dc_group.pm_dc_active_PM_DC = 0;
          }
          privateMibBase.connAlarmGroup.alarm_pm_dc_connect |= (s_pm_dc_info[i].u8_error_flag << i);
          privateMibBase.pm_dc_group.pm_dc_table.pm_dc_status = 170;
          privateMibBase.mainAlarmGroup.alarm_pm_dc_pack[i]=privateMibBase.connAlarmGroup.alarm_pm_dc_connect; // them trap bao mat ket noi
        }
      }
      else {
        cnt_disConnect_pm_dc[i] = 0;
        privateMibBase.pm_dc_group.pm_dc_table.pm_dc_status = 255;
      }
      
      if (privateMibBase.pm_dc_group.pm_dc_table.pm_dc_status != 170)
      {
          if (privateMibBase.pm_dc_group.pm_dc_table.pm_dc_status == 165)
          {
              privateMibBase.mainAlarmGroup.alarm_pm_dc = 1;
          }

          switch(sModbusManager.s_pm_dc_manager[i].u8_pm_dc_type)
          {
          case 1://YADA_DC
          {
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_id                   = i+1;
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_voltage              = (uint32_t)s_pm_dc_info[i].f_voltage;
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_current              = (uint32_t)s_pm_dc_info[i].f_current;
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_active_power         = (uint32_t)s_pm_dc_info[i].f_active_power;
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_active_energy        = (uint32_t)s_pm_dc_info[i].f_active_energy;
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_type                 = sModbusManager.s_pm_dc_manager[i].u8_pm_dc_type;
              sprintf(&privateMibBase.pm_dc_group.pm_dc_table.pm_dc_model_[0],"YADA");
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_model_len            = 4;
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_modbus_ID            = sModbusManager.s_pm_dc_manager[i].u32_pm_dc_abs_slaveID; 
          }
          break;
          default:
            break;
          };
      }
      else
      {
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_id                   = i+1;
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_voltage              = 0;
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_current              = 0;
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_active_power         = 0;
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_active_energy        = 0;
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_type                 = sModbusManager.s_pm_dc_manager[i].u8_pm_dc_type;
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_modbus_ID            = sModbusManager.s_pm_dc_manager[i].u32_pm_dc_abs_slaveID;
              sprintf(&privateMibBase.pm_dc_group.pm_dc_table.pm_dc_model_[0]," ");
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_model_len            = 1;       
      }
    
  }
  //clean--------------------------------------------------------------------------------------------------------------------------------------
  for (i = privateMibBase.pm_dc_group.pm_dc_installed_PM_DC; i<1; i++){
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_status = 0;
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_id                   = i+1;
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_voltage              = 0;
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_current              = 0;
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_active_power         = 0;
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_active_energy        = 0;
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_type                 = sModbusManager.s_pm_dc_manager[i].u8_pm_dc_type;
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_modbus_ID            = sModbusManager.s_pm_dc_manager[i].u32_pm_dc_abs_slaveID;
              sprintf(&privateMibBase.pm_dc_group.pm_dc_table.pm_dc_model_[0]," ");
              privateMibBase.pm_dc_group.pm_dc_table.pm_dc_model_len            = 1;
  }
  
}
#endif 
#if (USERDEF_RS485_DKD51_BDP == ENABLED)
void update_fan_dpc_info(void){
  if(TYPE == DKD51_BDP){
  //FAN ===========================================================================================================
    //parameter setting----------------------------------------------------------------------------------
    privateMibBase.fan_dpc_info.mib.para_dc_starting_point   = fan_info_dpc_t.para_dc_starting_point;
    privateMibBase.fan_dpc_info.mib.para_dc_sensivive_point  = fan_info_dpc_t.para_dc_sensivive_point;
    privateMibBase.fan_dpc_info.mib.para_dc_heating_start_point = fan_info_dpc_t.para_dc_heating_start_point;
    privateMibBase.fan_dpc_info.mib.para_dc_heating_sensivive   = fan_info_dpc_t.para_dc_heating_sensivive;
    privateMibBase.fan_dpc_info.mib.para_alarm_high_temp     = fan_info_dpc_t.para_alarm_high_temp;
    privateMibBase.fan_dpc_info.mib.para_alarm_low_temp      = fan_info_dpc_t.para_alarm_low_temp;
    //parameter-----------------------------------------------------------------------------------------
    privateMibBase.fan_dpc_info.mib.env_temp                 = fan_info_dpc_t.env_temp;
    privateMibBase.fan_dpc_info.mib.airflow_temp             = fan_info_dpc_t.airflow_temp;
    privateMibBase.fan_dpc_info.mib.voltage                  = fan_info_dpc_t.voltage;
    privateMibBase.fan_dpc_info.mib.working_current          = fan_info_dpc_t.working_current;
    privateMibBase.fan_dpc_info.mib.state_device             = fan_info_dpc_t.state_device;
    privateMibBase.fan_dpc_info.mib.state_fan                = fan_info_dpc_t.state_fan;
    //alarm----------------------------------------------------------------------------------------------
    privateMibBase.fan_dpc_info.mib.alarm_fail_refrigeration = fan_info_dpc_t.alarm_fail_refrigeration;
    privateMibBase.fan_dpc_info.mib.alarm_high_temp          = fan_info_dpc_t.alarm_high_temp;
    privateMibBase.fan_dpc_info.mib.alarm_low_temp           = fan_info_dpc_t.alarm_low_temp;   
    privateMibBase.fan_dpc_info.mib.alarm_fail_sensor_temp   = fan_info_dpc_t.alarm_fail_sensor_temp;
    privateMibBase.fan_dpc_info.mib.alarm_high_voltage       = fan_info_dpc_t.alarm_high_voltage;
    privateMibBase.fan_dpc_info.mib.alarm_low_voltage        = fan_info_dpc_t.alarm_low_voltage;
  }else{
    //parameter setting----------------------------------------------------------------------------------
    privateMibBase.fan_dpc_info.mib.para_dc_starting_point   = 0;
    privateMibBase.fan_dpc_info.mib.para_dc_sensivive_point  = 0;
    privateMibBase.fan_dpc_info.mib.para_dc_heating_start_point = 0;
    privateMibBase.fan_dpc_info.mib.para_dc_heating_sensivive   = 0;
    privateMibBase.fan_dpc_info.mib.para_alarm_high_temp     = 0;
    privateMibBase.fan_dpc_info.mib.para_alarm_low_temp      = 0;
    //parameter-----------------------------------------------------------------------------------------
    privateMibBase.fan_dpc_info.mib.env_temp                 = 0;
    privateMibBase.fan_dpc_info.mib.airflow_temp             = 0;
    privateMibBase.fan_dpc_info.mib.voltage                  = 0;
    privateMibBase.fan_dpc_info.mib.working_current          = 0;
    privateMibBase.fan_dpc_info.mib.state_device             = 0;
    privateMibBase.fan_dpc_info.mib.state_fan                = 0;
    //alarm-----------------------------------------------------------------------------------------------
    privateMibBase.fan_dpc_info.mib.alarm_fail_refrigeration = 0;
    privateMibBase.fan_dpc_info.mib.alarm_high_temp          = 0;
    privateMibBase.fan_dpc_info.mib.alarm_low_temp           = 0;  
    privateMibBase.fan_dpc_info.mib.alarm_fail_sensor_temp   = 0;
    privateMibBase.fan_dpc_info.mib.alarm_high_voltage       = 0;
    privateMibBase.fan_dpc_info.mib.alarm_low_voltage        = 0;
    //alarm connect----------------------------------------------------------------------------------------
    privateMibBase.connAlarmGroup.alarm_dpc_fan_connect      = 0;
  }
  
}
#endif 



#if (USER_DEF_CHECK_GEN ==  ENABLED)


void check_gen()
{
  
  if(sModbusManager.u8NumberOfISENSE != 0)
  {
    
    if(privateMibBase.connAlarmGroup.alarmISENSEConnect!=0 )
    {
      //isense connect off---------------------------------------------------------
      
      if(sModbusManager.u8NumberOfPM!=0)
      {
        if(privateMibBase.connAlarmGroup.alarmPMConnect==0)
        {
          //pm connect on---------------------------------------------------------
          if(privateMibBase.pmGroup.pmTable[0].pmOutOfRangeFreq == 1)
          {
            remainning_gen     = 1;
            cnt_remainning_gen = 0; 
          }
          else if(privateMibBase.pmGroup.pmTable[0].pmOutOfRangeFreq == 0)
          {
            if(remainning_gen == 0){
              remainning_gen     = 0;
              cnt_remainning_gen = 0;
            }
            else if (remainning_gen == 1)
            {
              if(cnt_remainning_gen++>privateMibBase.pmGroup.pmTable[0].pm_set_cnt_remaining_gen){
                remainning_gen     = 0;
                cnt_remainning_gen = 0;
              }
            }
          }
          
        }
        else
        {
          //pm connect off--------------------------------------------------------
          remainning_gen     = 0;
          cnt_remainning_gen = 0;
        } 
      }
      else
      {
        remainning_gen     = 0;
        cnt_remainning_gen = 0;
      }
      
    }
    else
    {
      //isense on----------------------------------------------------------- 
      remainning_gen     = 0;
      cnt_remainning_gen = 0;
    }
    
  }
  else
  {
    remainning_gen     = 0;
    cnt_remainning_gen = 0;
  }
}


#endif



