#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "Header.h"
#include "ZTEHistorylog.h"

#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
SemaphoreHandle_t xUart1Semaphore = NULL;

uart_rtos_handle_t handle;
struct _uart_handle t_handle;

//uint32_t t1,t2,t3,t4,t5,t6,t7,t8;
uint8_t RecvBuff[300];
uint8_t RS2321Buff[300];
uint8_t RS2322Buff[300];
uint8_t RS232Cfg[200];
uint32_t RX2321Count=0,RX2322Count=0,Recv_Count =0, RS232CfgCount =0, RecvCount =0;
uint32_t PMUConnectCount =0;
uint32_t requestcode = 1;
uint8_t u8InterruptCode = 0;
uint16_t g_testchecksum=0xFFFF,g_checksum=0;
uint16_t CheckSum =0, TestCheckSum =0;
unsigned char g_setInfo[32];
unsigned char g_getInfo[24];
uint8_t TYPE=0;
//uint8_t setCmd_flag = 0;
//uint32_t setCmd_mask = 0;
//uint32_t settingCommand = 0;
uint32_t reverror =0;
uint8_t g_AlrmMan;
float a=53.5;
uint32_t u32IPAddr;
uint32_t u32GWAddr;
uint32_t u32SNAddr;
uint32_t u32SIPAddr;
uint8_t val;
uint8_t Rectlosedtest=0;
uint8_t RectNumber = 0;
uint8_t ui8User[6];

unsigned char g_UARTRxBuf[300];
unsigned char g_UARTRxBuf2[300];
uint32_t RecvCntStart=0;
uint32_t RecvCntStart2 =0;
tMESGState MESGState_v21;
//uint8_t athao[100];
tMESGState MESGStatetest[20];
uint8_t k=0;
tTimeInfo  ZTETime;
tTimeInfo  EMERV21Time;

//Thanhcm3 TEST=====================
uint32_t EMER_cnt=0;
uint32_t ZTE_cnt=0;
uint32_t EMERV21_cnt=0;

void Emerson_Data_Process(void *pvParameters)
{
    uint32_t i,j;
    uint32_t fuse_flag;
    uint32_t temp[2];
    MESGState = SYSINFO_REQ;

    do
    {
      EMER_cnt++;
      privateMibBase.cntGroup.EMERcnt=EMER_cnt;
      privateMibBase.cntGroup.EMERState= MESGState;
      
      
        if (sInterruptMesg.u8Checkbit == 1)
        {
          g_testchecksum = hex2byte2(&g_UARTRxBuf2[0],RecvCntStart2-5);
          g_checksum = Checksum16(&g_UARTRxBuf2[0],RecvCntStart2);
          if (g_checksum == g_testchecksum)
          {
            u8InterruptCode = hex2byte(&g_UARTRxBuf2[0],3);
            switch(u8InterruptCode)
            {
              case 0x01:
              {
                      MESGState = CALIB_MESG_1;

              } break;
              case 0x02:
              {
                      u32IPAddr = hex2byte4(&g_UARTRxBuf2[0],13);
                      u32SNAddr = hex2byte4(&g_UARTRxBuf2[0],21);
                      u32GWAddr = hex2byte4(&g_UARTRxBuf2[0],29);
                      u32SIPAddr = hex2byte4(&g_UARTRxBuf2[0],37);

                      //
                      // If we are now using static IP, check for modifications to the IP
                      // addresses and mask.
                      //

                      if((g_sParameters.u32StaticIP != u32IPAddr) ||
                         (g_sParameters.u32GatewayIP != u32GWAddr) ||
                         (g_sParameters.u32SubnetMask != u32SNAddr) ||
                         (g_sParameters.u32SnmpIP != u32SIPAddr))
                      {
                        g_sParameters.u32StaticIP = u32IPAddr;
                        g_sParameters.u32GatewayIP = u32GWAddr;
                        g_sParameters.u32SubnetMask = u32SNAddr;
                        g_sParameters.u32SnmpIP = u32SIPAddr;
                        //
                        // Yes - save these settings as the defaults.
                        //
                        g_sWorkingDefaultParameters.u32StaticIP = g_sParameters.u32StaticIP;
                        g_sWorkingDefaultParameters.u32GatewayIP = g_sParameters.u32GatewayIP;
                        g_sWorkingDefaultParameters.u32SubnetMask = g_sParameters.u32SubnetMask;
                        g_sWorkingDefaultParameters.u32SnmpIP = g_sParameters.u32SnmpIP;
                        ConfigSave();  
                      }
                      MESGState = CALIB_MESG_2;
              } break;
              case 0x03:
              {
                g_sParameters2.u8IsWritten = 0x53;
                g_sParameters2.u8UserMAC[0] = hex2byte(&g_UARTRxBuf2[0],13);
                g_sParameters2.u8UserMAC[1] = hex2byte(&g_UARTRxBuf2[0],15);
                g_sParameters2.u8UserMAC[2] = hex2byte(&g_UARTRxBuf2[0],17);
                g_sParameters2.u8UserMAC[3] = hex2byte(&g_UARTRxBuf2[0],19);
                g_sParameters2.u8UserMAC[4] = hex2byte(&g_UARTRxBuf2[0],21);
                g_sParameters2.u8UserMAC[5] = hex2byte(&g_UARTRxBuf2[0],23);
                snprintf((char*)g_sParameters2.u8UserSerialNo,20,"%s",&g_UARTRxBuf2[25]);                
                g_sParameters2.u32FirmwareCRC_old = hex2byte4(&g_UARTRxBuf2[0],45);
                g_sParameters2.u32FirmwareCRC = g_sParameters2.u32FirmwareCRC_old;
                sFLASH_EraseSector(FLASH_CFG_START_SN);
                sFLASH_WritePage((uint8_t *)g_psDefaultParameters2, FLASH_CFG_START_SN, FLASH_PB_SIZE);
                               
                
                sFLASH_ReadBuffer((uint8_t *)&g_u8ReadBackBuffer[0],FLASH_CFG_START_SN, FLASH_PB_SIZE);

                if (g_u8ReadBackBuffer[0] == 0x53)
                {
                  g_sWorkingDefaultParameters2 = *(tConfigParameters2 *)g_u8ReadBackBuffer;
                      MESGState = CALIB_MESG_3;                  
                }

              } break;
              case 0x04:
              {
                 sTestRS485.requesttestRS485 =1;
                 if(sTestRS485.InitUart==0)
                  {
                         Init_RS485_UART_TEST();
                         GPIO_ClearPinsOutput(GPIOD,1u << 0u);
                         GPIO_ClearPinsOutput(GPIOD,1u << 1u); 
                         sTestRS485.InitUart = 0xAA;
                  } 
                  sTestRS485.Cnt=0;
                  sTestRS485.RS485CabRecv =0;
                  sTestRS485.RS485GenRecv =0;
                  sTestRS485.CabRecvCnt=0;
                  sTestRS485.GenRecvCnt=0;
                     for(i=0;i<15;i++)
                      {
                        sTestRS485.CabBuff[i]=0;
                      }
                     for(i=0;i<15;i++)
                      {
                        sTestRS485.GenBuff[i]=0;
                      }
                      MESGState = CALIB_MESG_4;

              } break;
              default:
                      break;
            }

          }
          sInterruptMesg.u8Checkbit = 0;

        }
        else if (sInterruptMesg.u8Checkbit == 0)
        {
            if (setCmd_flag == 1)
            {
                settingCommand |= setCmd_mask;
                switch(settingCommand)
                {
                case SET_FLTVOL:
                    sBattInfo.sRAWBattInfo.fFltVoltCfg = (float)privateMibBase.configGroup.cfgFloatVolt/100;
                    setCommand(&g_setInfo[0],&cmdCodeTable[0][0], sBattInfo.sRAWBattInfo.fFltVoltCfg,0);
                    for (i = 0; i < 32; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_BSTVOL:
                    sBattInfo.sRAWBattInfo.fBotVoltCfg = (float)privateMibBase.configGroup.cfgBoostVolt/100;
                    setCommand(&g_setInfo[0],&cmdCodeTable[1][0], sBattInfo.sRAWBattInfo.fBotVoltCfg,0);
                    for (i = 0; i < 32; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_TEMPCOMP_VAL:
                    sBattInfo.sRAWBattInfo.fTempCompVal = (float)privateMibBase.configGroup.cfgTempCompValue;
                    setCommand(&g_setInfo[0],&cmdCodeTable[2][0], sBattInfo.sRAWBattInfo.fTempCompVal,0);
                    for (i = 0; i < 32; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_LLVD:
                    sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg = (float)privateMibBase.configGroup.cfgLLVDVolt/100;
                    setCommand(&g_setInfo[0],&cmdCodeTable[3][0], sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg,0);
                    for (i = 0; i < 32; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_BLVD:
                    sBattInfo.sRAWBattInfo.fLVDDV = (float)privateMibBase.configGroup.cfgBLVDVolt/100;
                    setCommand(&g_setInfo[0],&cmdCodeTable[4][0], sBattInfo.sRAWBattInfo.fLVDDV,0);
                    for (i = 0; i < 32; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_DCLOW:
                    sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg = (float)privateMibBase.configGroup.cfgDCLowVolt/100;
                    setCommand(&g_setInfo[0],&cmdCodeTable[5][0], sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg,0);
                    for (i = 0; i < 32; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_DCUNDER:
                    sBattInfo.sRAWBattInfo.fDCUnderCfg = (float)privateMibBase.configGroup.cfgDCUnderVolt/100;
                    setCommand(&g_setInfo[0],&cmdCodeTable[6][0], sBattInfo.sRAWBattInfo.fDCUnderCfg,0);
                    for (i = 0; i < 32; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;

                case SET_LLVD_EN:
                    sBattInfo.sRAWBattInfo.u8LLVDE = privateMibBase.configGroup.cfgLLVDEn;
                    setCommand(&g_setInfo[0],&cmdCodeTable[72][0], sBattInfo.sRAWBattInfo.u8LLVDE,1);
                    for (i = 0; i < 32; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;

                case SET_BLVD_EN:
                    sBattInfo.sRAWBattInfo.u8BLVDE = privateMibBase.configGroup.cfgBLVDEn;
                    setCommand(&g_setInfo[0],&cmdCodeTable[73][0], sBattInfo.sRAWBattInfo.u8BLVDE,1);
                    for (i = 0; i < 32; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;

                case SET_BATTSTDCAP:
                    sBattInfo.sRAWBattInfo.fCapTotal = (float)privateMibBase.configGroup.cfgBattCapacityTotal;
                    setCommand(&g_setInfo[0],&cmdCodeTable[15][0], sBattInfo.sRAWBattInfo.fCapTotal,0);
                    for (i = 0; i < 32; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_CCL:
                    sBattInfo.sRAWBattInfo.fCCLVal = (float)privateMibBase.configGroup.cfgCurrentLimit/1000;
                    setCommand(&g_setInfo[0],&cmdCodeTable[17][0], sBattInfo.sRAWBattInfo.fCCLVal,0);
                    for (i = 0; i < 32; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_OVERTEMP:
                    sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel = (float)privateMibBase.configGroup.cfgHighMajorTempLevel/100;
                    setCommand(&g_setInfo[0],&cmdCodeTable[30][0], sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel,0);
                    for (i = 0; i < 32; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;

                case SET_WIT_VAL:
                    sRectInfo.u8WITI = privateMibBase.configGroup.cfgWalkInTimeDuration;
                    setCommand(&g_setInfo[0],&cmdCodeTable[36][0], sRectInfo.u8WITI,0);
                    for (i = 0; i < 32; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;

                case SET_WIT_EN:
                    sRectInfo.u8WITE = privateMibBase.configGroup.cfgWalkInTimeEn;
                    setCommand(&g_setInfo[0],&cmdCodeTable[37][0], sRectInfo.u8WITE,1);
                    for (i = 0; i < 32; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;

                case SET_AC_THRES:
                    sAcInfo.fAcLowThres = (float)privateMibBase.configGroup.cfgAcLowLevel;
                    setCommand(&g_setInfo[0],&cmdCodeTable[45][0], sAcInfo.fAcLowThres,0);
                    for (i = 0; i < 32; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                //======================================== BATTERY TEST SETTING ============================================//

                case SET_BT_ENDVOLT:
                    sBattTestInfo.fBattTestVolt = (float) privateMibBase.cfgBTGroup.cfgBTEndVolt/10;
                    setCommand(&g_setInfo[0],&cmdCodeTable[48][0], sBattTestInfo.fBattTestVolt,0);
                    for (i = 0; i < 32; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_BT_ENDCAP:
                    sBattTestInfo.fTestEndCap = (float) privateMibBase.cfgBTGroup.cfgBTEndCap/1000;
                    setCommand(&g_setInfo[0],&cmdCodeTable[50][0], sBattTestInfo.fTestEndCap,0);
                    for (i = 0; i < 32; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_BT_ENDTIME:
                    sBattTestInfo.fBattTestDur = (float) privateMibBase.cfgBTGroup.cfgBTEndTime;
                    setCommand(&g_setInfo[0],&cmdCodeTable[49][0], sBattTestInfo.fBattTestDur,0);
                    for (i = 0; i < 32; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_BT_PLEN:
                    setCommand(&g_setInfo[0],&cmdCodeTable[51][0], privateMibBase.cfgBTGroup.cfgBTPlanTestEn,1);
                    for (i = 0; i < 32; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_BT_PLANTEST:
                    if (sBattTestInfo.u8SetPlan == 1)
                    {
                        switch(sBattTestInfo.u8CheckSentAll)
                        {
                        case 1:
                        {
                            sBattTestInfo.u8CheckValidAll = 0;
                            sBattTestInfo.u8CheckSentAll = 2;
                        }
                        break;
                        case 2:
                        {
                            switch(privateMibBase.cfgBTGroup.cfgBTPlanTestTable[0].cfgBTPlanTestMonth)
                            {
                            case 1:
                            case 3:
                            case 5:
                            case 7:
                            case 8:
                            case 10:
                            case 12:
                            {
                                sBattTestInfo.u8CheckValidAll = 1;
                            }
                            break;
                            case 4:
                            case 6:
                            case 9:
                            case 11:
                            {
                                if (privateMibBase.cfgBTGroup.cfgBTPlanTestTable[0].cfgBTPlanTestDate == 31)
                                {
                                    sBattTestInfo.u8CheckValidAll = 0;
                                }
                                else
                                {
                                    sBattTestInfo.u8CheckValidAll = 1;
                                }
                            }
                            break;
                            case 2:
                            {
                                if ((privateMibBase.cfgBTGroup.cfgBTPlanTestTable[0].cfgBTPlanTestDate == 30) || (privateMibBase.cfgBTGroup.cfgBTPlanTestTable[0].cfgBTPlanTestDate == 31))
                                {
                                    sBattTestInfo.u8CheckValidAll = 0;
                                }
                                else
                                {
                                    sBattTestInfo.u8CheckValidAll = 1;
                                }
                            }
                            break;
                            }

                            if (sBattTestInfo.u8CheckValidAll == 1)
                            {
                                setCommand(&g_setInfo[0],&cmdCodeTable[60][0], privateMibBase.cfgBTGroup.cfgBTPlanTestTable[0].cfgBTPlanTestMonth,1);
                                for (i = 0; i < 32; i++)
                                {
                                    UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                                }
                                sBattTestInfo.u8CheckSentAll = 3;
                            }
                            else
                            {

                                sBattTestInfo.u8CheckSentAll = 0;
                                sBattTestInfo.u8SetPlan = 0;
                            }

                        }
                        break;
                        case 3:
                        {
                            setCommand(&g_setInfo[0],&cmdCodeTable[61][0], privateMibBase.cfgBTGroup.cfgBTPlanTestTable[0].cfgBTPlanTestDate,1);
                            for (i = 0; i < 32; i++)
                            {
                                UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                            }
                            sBattTestInfo.u8CheckSentAll = 4;
                        }
                        break;
                        case 4:
                        {
                            setCommand(&g_setInfo[0],&cmdCodeTable[62][0], privateMibBase.cfgBTGroup.cfgBTPlanTestTable[0].cfgBTPlanTestHour,1);
                            for (i = 0; i < 32; i++)
                            {
                                UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                            }
                            sBattTestInfo.u8CheckSentAll = 0;
                            sBattTestInfo.u8SetPlan = 0;
                        }
                        break;
                        };
                    }
                    else if (sBattTestInfo.u8SetPlan == 2)
                    {
                        switch(sBattTestInfo.u8CheckSentAll)
                        {
                        case 1:
                        {
                            sBattTestInfo.u8CheckValidAll = 0;
                            sBattTestInfo.u8CheckSentAll = 2;
                        }
                        break;
                        case 2:
                        {
                            switch(privateMibBase.cfgBTGroup.cfgBTPlanTestTable[1].cfgBTPlanTestMonth)
                            {
                            case 1:
                            case 3:
                            case 5:
                            case 7:
                            case 8:
                            case 10:
                            case 12:
                            {
                                sBattTestInfo.u8CheckValidAll = 1;
                            }
                            break;
                            case 4:
                            case 6:
                            case 9:
                            case 11:
                            {
                                if (privateMibBase.cfgBTGroup.cfgBTPlanTestTable[1].cfgBTPlanTestDate == 31)
                                {
                                    sBattTestInfo.u8CheckValidAll = 0;
                                }
                                else
                                {
                                    sBattTestInfo.u8CheckValidAll = 1;
                                }
                            }
                            break;
                            case 2:
                            {
                                if ((privateMibBase.cfgBTGroup.cfgBTPlanTestTable[1].cfgBTPlanTestDate == 30) || (privateMibBase.cfgBTGroup.cfgBTPlanTestTable[1].cfgBTPlanTestDate == 31))
                                {
                                    sBattTestInfo.u8CheckValidAll = 0;
                                }
                                else
                                {
                                    sBattTestInfo.u8CheckValidAll = 1;
                                }
                            }
                            break;
                            }

                            if (sBattTestInfo.u8CheckValidAll == 1)
                            {
                                setCommand(&g_setInfo[0],&cmdCodeTable[63][0], privateMibBase.cfgBTGroup.cfgBTPlanTestTable[1].cfgBTPlanTestMonth,1);
                                for (i = 0; i < 32; i++)
                                {
                                    UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                                }
                                sBattTestInfo.u8CheckSentAll = 3;
                            }
                            else
                            {
                                sBattTestInfo.u8CheckSentAll = 0;
                                sBattTestInfo.u8SetPlan = 0;
                            }

                        }
                        break;
                        case 3:
                        {
                            setCommand(&g_setInfo[0],&cmdCodeTable[64][0], privateMibBase.cfgBTGroup.cfgBTPlanTestTable[1].cfgBTPlanTestDate,1);
                            for (i = 0; i < 32; i++)
                            {
                                UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                            }
                            sBattTestInfo.u8CheckSentAll = 4;
                        }
                        break;
                        case 4:
                        {
                            setCommand(&g_setInfo[0],&cmdCodeTable[65][0], privateMibBase.cfgBTGroup.cfgBTPlanTestTable[1].cfgBTPlanTestHour,1);
                            for (i = 0; i < 32; i++)
                            {
                                UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                            }
                            sBattTestInfo.u8CheckSentAll = 0;
                            sBattTestInfo.u8SetPlan = 0;
                        }
                        break;
                        };
                    }
                    else if (sBattTestInfo.u8SetPlan == 3)
                    {
                        switch(sBattTestInfo.u8CheckSentAll)
                        {
                        case 1:
                        {
                            sBattTestInfo.u8CheckValidAll = 0;
                            sBattTestInfo.u8CheckSentAll = 2;
                        }
                        break;
                        case 2:
                        {
                            switch(privateMibBase.cfgBTGroup.cfgBTPlanTestTable[2].cfgBTPlanTestMonth)
                            {
                            case 1:
                            case 3:
                            case 5:
                            case 7:
                            case 8:
                            case 10:
                            case 12:
                            {
                                sBattTestInfo.u8CheckValidAll = 1;
                            }
                            break;
                            case 4:
                            case 6:
                            case 9:
                            case 11:
                            {
                                if (privateMibBase.cfgBTGroup.cfgBTPlanTestTable[2].cfgBTPlanTestDate == 31)
                                {
                                    sBattTestInfo.u8CheckValidAll = 0;
                                }
                                else
                                {
                                    sBattTestInfo.u8CheckValidAll = 1;
                                }
                            }
                            break;
                            case 2:
                            {
                                if ((privateMibBase.cfgBTGroup.cfgBTPlanTestTable[2].cfgBTPlanTestDate == 30) || (privateMibBase.cfgBTGroup.cfgBTPlanTestTable[2].cfgBTPlanTestDate == 31))
                                {
                                    sBattTestInfo.u8CheckValidAll = 0;
                                }
                                else
                                {
                                    sBattTestInfo.u8CheckValidAll = 1;
                                }
                            }
                            break;
                            }

                            if (sBattTestInfo.u8CheckValidAll == 1)
                            {
                                setCommand(&g_setInfo[0],&cmdCodeTable[66][0], privateMibBase.cfgBTGroup.cfgBTPlanTestTable[2].cfgBTPlanTestMonth,1);
                                for (i = 0; i < 32; i++)
                                {
                                    UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                                }
                                sBattTestInfo.u8CheckSentAll = 3;
                            }
                            else
                            {
                                sBattTestInfo.u8CheckSentAll = 0;
                                sBattTestInfo.u8SetPlan = 0;
                            }

                        }
                        break;
                        case 3:
                        {
                            setCommand(&g_setInfo[0],&cmdCodeTable[67][0], privateMibBase.cfgBTGroup.cfgBTPlanTestTable[2].cfgBTPlanTestDate,1);
                            for (i = 0; i < 32; i++)
                            {
                                UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                            }
                            sBattTestInfo.u8CheckSentAll = 4;
                        }
                        break;
                        case 4:
                        {
                            setCommand(&g_setInfo[0],&cmdCodeTable[68][0], privateMibBase.cfgBTGroup.cfgBTPlanTestTable[2].cfgBTPlanTestHour,1);
                            for (i = 0; i < 32; i++)
                            {
                                UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                            }
                            sBattTestInfo.u8CheckSentAll = 0;
                            sBattTestInfo.u8SetPlan = 0;
                        }
                        break;
                        };
                    }
                    else if (sBattTestInfo.u8SetPlan == 4)
                    {
                        switch(sBattTestInfo.u8CheckSentAll)
                        {
                        case 1:
                        {
                            sBattTestInfo.u8CheckValidAll = 0;
                            sBattTestInfo.u8CheckSentAll = 2;
                        }
                        break;
                        case 2:
                        {
                            switch(privateMibBase.cfgBTGroup.cfgBTPlanTestTable[3].cfgBTPlanTestMonth)
                            {
                            case 1:
                            case 3:
                            case 5:
                            case 7:
                            case 8:
                            case 10:
                            case 12:
                            {
                                sBattTestInfo.u8CheckValidAll = 1;
                            }
                            break;
                            case 4:
                            case 6:
                            case 9:
                            case 11:
                            {
                                if (privateMibBase.cfgBTGroup.cfgBTPlanTestTable[3].cfgBTPlanTestDate == 31)
                                {
                                    sBattTestInfo.u8CheckValidAll = 0;
                                }
                                else
                                {
                                    sBattTestInfo.u8CheckValidAll = 1;
                                }
                            }
                            break;
                            case 2:
                            {
                                if ((privateMibBase.cfgBTGroup.cfgBTPlanTestTable[3].cfgBTPlanTestDate == 30) || (privateMibBase.cfgBTGroup.cfgBTPlanTestTable[3].cfgBTPlanTestDate == 31))
                                {
                                    sBattTestInfo.u8CheckValidAll = 0;
                                }
                                else
                                {
                                    sBattTestInfo.u8CheckValidAll = 1;
                                }
                            }
                            break;
                            }

                            if (sBattTestInfo.u8CheckValidAll == 1)
                            {
                                setCommand(&g_setInfo[0],&cmdCodeTable[69][0], privateMibBase.cfgBTGroup.cfgBTPlanTestTable[3].cfgBTPlanTestMonth,1);
                                for (i = 0; i < 32; i++)
                                {
                                    UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                                }
                                sBattTestInfo.u8CheckSentAll = 3;
                            }
                            else
                            {
                                sBattTestInfo.u8CheckSentAll = 0;
                                sBattTestInfo.u8SetPlan = 0;
                            }

                        }
                        break;
                        case 3:
                        {
                            setCommand(&g_setInfo[0],&cmdCodeTable[70][0], privateMibBase.cfgBTGroup.cfgBTPlanTestTable[3].cfgBTPlanTestDate,1);
                            for (i = 0; i < 32; i++)
                            {
                                UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                            }
                            sBattTestInfo.u8CheckSentAll = 4;
                        }
                        break;
                        case 4:
                        {
                            setCommand(&g_setInfo[0],&cmdCodeTable[71][0], privateMibBase.cfgBTGroup.cfgBTPlanTestTable[3].cfgBTPlanTestHour,1);
                            for (i = 0; i < 32; i++)
                            {
                                UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                            }
                            sBattTestInfo.u8CheckSentAll = 0;
                            sBattTestInfo.u8SetPlan = 0;
                        }
                        break;
                        };
                    }
                    break;
                //======================================== BATTERY TEST SETTING ============================================//
                case SET_BT_SCUTIME:
                {
                    if (sBattTestInfo.u8SetPlan == 5)
                    {
                        switch(sBattTestInfo.u8CheckSentAll)
                        {
                        case 1:
                        {
                            sBattTestInfo.u8CheckValidAll = 0;
                            sBattTestInfo.u8CheckSentAll = 2;
                        }
                        break;
                        case 2:
                        {
                            switch(privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUMonth)
                            {
                            case 1:
                            case 3:
                            case 5:
                            case 7:
                            case 8:
                            case 10:
                            case 12:
                            {
                                sBattTestInfo.u8CheckValidAll = 1;
                            }
                            break;
                            case 4:
                            case 6:
                            case 9:
                            case 11:
                            {
                                if (privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUDate == 31)
                                {
                                    sBattTestInfo.u8CheckValidAll = 0;
                                }
                                else
                                {
                                    sBattTestInfo.u8CheckValidAll = 1;
                                }
                            }
                            break;
                            case 2:
                            {
                                if ((privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUDate == 30) || (privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUDate == 31))
                                {
                                    sBattTestInfo.u8CheckValidAll = 0;
                                }
                                else
                                {
                                    sBattTestInfo.u8CheckValidAll = 1;
                                }
                            }
                            break;
                            }

                            if (sBattTestInfo.u8CheckValidAll == 1)
                            {
                                u82char(&scuTimeSet[13], (uint8_t)(privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUYear/100), 2);
                                u82char(&scuTimeSet[15], (uint8_t)(privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUYear%100), 2);
                                u82char(&scuTimeSet[17], privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUMonth, 2);
                                u82char(&scuTimeSet[19], privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUDate, 2);
                                u82char(&scuTimeSet[21], privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUHour, 2);
                                u82char(&scuTimeSet[23], privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUMinute, 2);
                                u82char(&scuTimeSet[25], privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUSecond, 2);

                                u162char(&scuTimeSet[27],Checksum16(&scuTimeSet[0],32),4);

                                for (i = 0; i < 32; i++)
                                {
                                    UART_WriteBlocking(RS2321_UART,&scuTimeSet[i],1);;
                                }
                            }
                            sBattTestInfo.u8CheckSentAll = 0;
                            sBattTestInfo.u8SetPlan = 0;

                        }
                        break;
                        };
                    }
                }
                break;

                default:
                    break;
                }
                if (sBattTestInfo.u8SetPlan == 0)
                {
                    setCmd_mask = 0;
                    setCmd_flag = 0;
                    settingCommand = 0;
                }
            }
            else if (setCmd_flag == 0)
            {
                switch(MESGState)
                {
                  //====================================== HISTORYINFO MESSAGE ======================================//
						
                 case HISTORYMESG_REQ:
                           
                  u82char(&GetHistoryInfo[17], sHistoryInfo.ucSequenceNum, 2);
                  u162char(&GetHistoryInfo[19],Checksum16(&GetHistoryInfo[0],24),4);
                    
                    for (i = 0; i < GetHistoryInfoLen; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&GetHistoryInfo[i],1);
                    }

                    MESGState = HISTORYMESG_RES;
                    break;

                case HISTORYMESG_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                      if((sHistoryInfo.ucSequenceNum == hex2byte(&RecvBuff[0],17)) && (sHistoryInfo.ucSequenceNum >= 0) && (sHistoryInfo.ucSequenceNum < 201))
                      {
                        sHistoryInfo.ucSequenceNum++;
                        sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.u8ID = hex2byte(&RecvBuff[0],25);
                        if (sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.u8ID != 0)
                        {                          
                          snprintf(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sTimeFrame[0].cTimeFrame[0],18,"%02d-%02d-%02d,%02d:%02d:%02d",
                                          hex2byte(&RecvBuff[0],27),
                                          hex2byte(&RecvBuff[0],29),
                                          hex2byte(&RecvBuff[0],31),
                                          hex2byte(&RecvBuff[0],33),
                                          hex2byte(&RecvBuff[0],35),
                                          hex2byte(&RecvBuff[0],37)
                                          );
                          sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sTimeFrame[0].cTimeFrame[18] = '\0';


                          snprintf(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sTimeFrame[1].cTimeFrame[0],18,"%02d-%02d-%02d,%02d:%02d:%02d",
                                          hex2byte(&RecvBuff[0],39),
                                          hex2byte(&RecvBuff[0],41),
                                          hex2byte(&RecvBuff[0],43),
                                          hex2byte(&RecvBuff[0],45),
                                          hex2byte(&RecvBuff[0],47),
                                          hex2byte(&RecvBuff[0],49)
                                          );

                          sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sTimeFrame[1].cTimeFrame[18] = '\0';
                          RectID_Calculation(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.cRectID[0], &RecvBuff[51]);
                        }
//                        else
//                        {
//                          memset(sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sTimeFrame[1].cTimeFrame[0],0,19);
//                          memset(sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sTimeFrame[1].cTimeFrame[0],0,19);
//                          memset(sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sTimeFrame[1].cTimeFrame[0],0,19);
//                        }

//                        snprintf(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sTimeFrame[0].cTimeFrame[0],18,"%02d-%02d-%02d,%02d:%02d:%02d",
//                                        hex2byte(&RecvBuff[0],27),
//                                        hex2byte(&RecvBuff[0],29),
//                                        hex2byte(&RecvBuff[0],31),
//                                        hex2byte(&RecvBuff[0],33),
//                                        hex2byte(&RecvBuff[0],35),
//                                        hex2byte(&RecvBuff[0],37)
//                                        );
//                        sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sTimeFrame[0].cTimeFrame[18] = '\0';
//
//
//                        snprintf(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sTimeFrame[1].cTimeFrame[0],18,"%02d-%02d-%02d,%02d:%02d:%02d",
//                                        hex2byte(&RecvBuff[0],39),
//                                        hex2byte(&RecvBuff[0],41),
//                                        hex2byte(&RecvBuff[0],43),
//                                        hex2byte(&RecvBuff[0],45),
//                                        hex2byte(&RecvBuff[0],47),
//                                        hex2byte(&RecvBuff[0],49)
//                                        );
//
//                        sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sTimeFrame[1].cTimeFrame[18] = '\0';
//                        RectID_Calculation(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.cRectID[0], &RecvBuff[51]);
                        switch (sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.u8ID)
                        {
                                case 0x00:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"               ",UCNAMESIZE+2);
                                break;
                                case 0x01:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Fuse1BreakAlarm",UCNAMESIZE+2);
                                break;
                                case 0x02:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Fuse2BreakAlarm",UCNAMESIZE+2);
                                break;
                                case 0x03:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Fuse3BreakAlarm",UCNAMESIZE+2);
                                break;
                                case 0x04:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Fuse4BreakAlarm",UCNAMESIZE+2);
                                break;
                                case 0x05:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Fuse5BreakAlarm",UCNAMESIZE+2);
                                break;
                                case 0x06:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Fuse6BreakAlarm",UCNAMESIZE+2);
                                break;
                                case 0x07://************ Interpolating ***********//
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Fuse7BreakAlarm",UCNAMESIZE+2);
                                break;
                                case 0x08://************ Interpolating ***********//
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Fuse8BreakAlarm",UCNAMESIZE+2);
                                break;
                                case 0x09://************ Interpolating ***********//
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Fuse9BreakAlarm",UCNAMESIZE+2);
                                break;
                                case 0x0A:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"AuxLoadAlarm",UCNAMESIZE+2);
                                break;
                                case 0x0B:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Digital1Alarm",UCNAMESIZE+2);
                                break;
                                case 0x0C://************ Interpolating ***********//
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Digital2Alarm",UCNAMESIZE+2);
                                break;
                                case 0x0D://************ Interpolating ***********//
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Digital3Alarm",UCNAMESIZE+2);
                                break;
                                case 0x0E:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Digital4Alarm",UCNAMESIZE+2);
                                break;
                                case 0x0F://************ Interpolating ***********//
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Digital5Alarm",UCNAMESIZE+2);
                                break;
                                case 0x10://************ Interpolating ***********//
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Digital6Alarm",UCNAMESIZE+2);
                                break;
                                case 0x11://************ Interpolating ***********//
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Digital7Alarm",UCNAMESIZE+2);
                                break;
                                case 0x12://************ Interpolating ***********//
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Digital8Alarm",UCNAMESIZE+2);
                                break;
                                case 0x13:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DCVoltAlarm",UCNAMESIZE+2);
                                break;
                                case 0x14:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"AC1/UaVoltAlarm",UCNAMESIZE+2);
                                break;
                                case 0x15:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"AC1/UbVoltAlarm",UCNAMESIZE+2);
                                break;
                                case 0x16:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"AC1/UcVoltAlarm",UCNAMESIZE+2);
                                break;
                                case 0x1B:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Batt1OCAlarm",UCNAMESIZE+2);
                                break;
                                case 0x1C://************ Interpolating ***********//
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Batt2OCAlarm",UCNAMESIZE+2);
                                break;
                                case 0x1D://************ Interpolating ***********//
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"BattFuse1Alarm",UCNAMESIZE+2);
                                break;
                                case 0x1E://************ Interpolating ***********//
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"BattFuse2Alarm",UCNAMESIZE+2);
                                break;
                                case 0x1F://************ Interpolating ***********//
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"BattFuse3Alarm",UCNAMESIZE+2);
                                break;
                                case 0x20://************ Interpolating ***********//
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"BattFuse4Alarm",UCNAMESIZE+2);
                                break;
                                case 0x21:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"TemperatureAlarm",UCNAMESIZE+2);
                                break;
                                case 0x22:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"TemperatureAlarm",UCNAMESIZE+2);
                                break;
                                case 0x25:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"ManualAlarm",UCNAMESIZE+2);
                                break;
                                case 0x27:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"BattDischargeAlarm",UCNAMESIZE+2);
                                break;
                                case 0x28:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"CurrImbalanceAlarm",UCNAMESIZE+2);
                                break;
                                case 0x29:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Rect2FanFailsAlarm",UCNAMESIZE+2);
                                break;
                                case 0x2B:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"LVD1",UCNAMESIZE+2);
                                break;
                                case 0x2C:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"LVD2",UCNAMESIZE+2);
                                break;
                                case 0x2D:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"VoltDiscrepancy",UCNAMESIZE+2);
                                break;
                                case 0x2E:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"ACFailsAlarm",UCNAMESIZE+2);
                                break;
                                case 0x2F:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"MultiRectFailsAlarm",UCNAMESIZE+2);
                                break;
                                case 0x38:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Rect1CommFailsAlarm",UCNAMESIZE+2);
                                break;
                                case 0x39:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Rect2CommFailsAlarm",UCNAMESIZE+2);
                                break;
                                case 0x3A://************ Interpolating ***********//
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Rect3CommFailsAlarm",UCNAMESIZE+2);
                                break;
                                case 0x58:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Rect1DeratedAlarm",UCNAMESIZE+2);
                                break;
                                case 0x59://************ Interpolating ***********//
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Rect2DeratedAlarm",UCNAMESIZE+2);
                                break;
                                case 0x5A://************ Interpolating ***********//
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Rect3DeratedAlarm",UCNAMESIZE+2);
                                break;
                                case 0x68:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Rect1ACFailsAlarm",UCNAMESIZE+2);
                                break;
                                case 0x69:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Rect2ACFailsAlarm",UCNAMESIZE+2);
                                break;
                                case 0x6A://************ Interpolating ***********//
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Rect3ACFailsAlarm",UCNAMESIZE+2);
                                break;
                                case 0xAB:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"RectLostAlarm",UCNAMESIZE+2);
                                break;
                                case 0xC8:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Rect1FailsAlarm",UCNAMESIZE+2);
                                break;
                                case 0xC9://************ Interpolating ***********//
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Rect2FailsAlarm",UCNAMESIZE+2);
                                break;
                                case 0xCA://************ Interpolating ***********//
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Rect3FailsAlarm",UCNAMESIZE+2);
                                break;
                                case 0xF8:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Rect1ProtectAlarm",UCNAMESIZE+2);
                                break;
                                case 0xF9://************ Interpolating ***********//
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Rect2ProtectAlarm",UCNAMESIZE+2);
                                break;
                                case 0xFA://************ Interpolating ***********//
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"Rect3ProtectAlarm",UCNAMESIZE+2);
                                break;
                                default:
                                        strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"ObservationAlarm",UCNAMESIZE+2);
                                break;
                        };                        
                      }
                    }

                    RecvCntStart = 0;
                    if (sHistoryInfo.ucSequenceNum < 200)
                    {
                      MESGState = HISTORYMESG_REQ;
                    } 
                    else
                    {
                      sHistoryInfo.ucSequenceNum = 0;
                      MESGState = SYSINFO_REQ;
                    }
                    
                    break;
                //====================================== HISTORYINFO MESSAGE ======================================//
                //====================================== SYSINFO MESSAGE ======================================//

                case  SYSINFO_REQ:
                    for (i = 0; i < GetSysInfoLen; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&GetSysInfo[i],1);
                    }
                    MESGState = SYSINFO_RES;
                    
                    vTaskDelay(300);
                    break;

                case  SYSINFO_RES:
                  
//                    memset(&RecvBuff[0],0,300);
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    
//                    memset(&g_UARTRxBuf[0],0,300);
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                      sDcInfo.u32DCNoResponse = 0;
                      privateMibBase.connAlarmGroup.alarmPMUConnect = 0;
                        sDcInfo.sINFODcInfo.fVoltage = hex2float(&RecvBuff[0],17);
                        sDcInfo.sINFODcInfo.fCurrent = hex2float(&RecvBuff[0],25);
                        sDcInfo.sINFODcInfo.fBatt1Curr = hex2float(&RecvBuff[0],35);
                        sDcInfo.sINFODcInfo.fBatt2Curr = hex2float(&RecvBuff[0],43);
                        if((RecvBuff[43]==0x20)&&(RecvBuff[44]==0x20)&&(RecvBuff[45]==0x20)&&(RecvBuff[46]==0x20))
                        {
                            sDcInfo.sINFODcInfo.fBatt2Curr = 0;
                        }

                        sDcInfo.sINFODcInfo.fBatt1Volt = hex2float(&RecvBuff[0],55);
                        sDcInfo.sINFODcInfo.fBatt2Volt = hex2float(&RecvBuff[0],63);
                        sDcInfo.sINFODcInfo.fBatt1RmnCap = hex2float(&RecvBuff[0],71);
                        sDcInfo.sINFODcInfo.fBatt2RmnCap = hex2float(&RecvBuff[0],79);
                        sDcInfo.sINFODcInfo.fSen1BattTemp = hex2float(&RecvBuff[0],87);
                        sDcInfo.sINFODcInfo.fSen2BattTemp = hex2float(&RecvBuff[0],95);
                        sDcInfo.sINFODcInfo.fSen1AmbTemp = hex2float(&RecvBuff[0],103);
                        sDcInfo.sINFODcInfo.fSen2AmbTemp = hex2float(&RecvBuff[0],111);
                    }
                    else
                    {
                      sDcInfo.u32DCNoResponse++;
                      if (sDcInfo.u32DCNoResponse > 3)
                      {
                        privateMibBase.connAlarmGroup.alarmPMUConnect = 1;
                        sDcInfo.u32DCNoResponse = 10;
                      // xoa khi checksum sai
                      sDcInfo.sINFODcInfo.fVoltage = 0;
                      sDcInfo.sINFODcInfo.fCurrent = 0;
                      sDcInfo.sINFODcInfo.fBatt1Curr = 0;
                      sDcInfo.sINFODcInfo.fBatt2Curr = 0;
                      sDcInfo.sINFODcInfo.fBatt2Curr = 0;
                      sDcInfo.sINFODcInfo.fBatt1Volt = 0;
                      sDcInfo.sINFODcInfo.fBatt2Volt = 0;
                      sDcInfo.sINFODcInfo.fBatt1RmnCap = 0;
                      sDcInfo.sINFODcInfo.fBatt2RmnCap = 0;
                      sDcInfo.sINFODcInfo.fSen1BattTemp = 0;
                      sDcInfo.sINFODcInfo.fSen2BattTemp = 0;
                      sDcInfo.sINFODcInfo.fSen1AmbTemp = 0;
                      sDcInfo.sINFODcInfo.fSen2AmbTemp = 0; 
                      for (j = 24; j > 0; j--)
                      sRectInfo.sRAWRectParam[j-1].fRect_DcCurr= 0;
                      sRectInfo.sRAWRectParam[j-1].fRect_Temp = 0;
                      sRectInfo.sRAWRectParam[j-1].fRect_LimCurr = 0;
                      sRectInfo.sRAWRectParam[j-1].fRect_DcVolt = 0;
                      sRectInfo.sRAWRectParam[j-1].fRect_AcVolt = 0;
                      sAcInfo.facVolt[0] = 0;
                      sAcInfo.facVolt[1] = 0;
                      sAcInfo.facVolt[2] = 0;
                      sBattInfo.sRAWBattInfo.fFltVoltCfg = 0;
                      sBattInfo.sRAWBattInfo.fBotVoltCfg = 0;
                      sBattInfo.sRAWBattInfo.fTempCompVal = 0;
                      sBattInfo.sRAWBattInfo.u8LLVDE = 0;
                      sBattInfo.sRAWBattInfo.u8BLVDE = 0;
                      sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg = 0;
                      sBattInfo.sRAWBattInfo.fLVDDV = 0;
                      sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg = 0;
                      sBattInfo.sRAWBattInfo.fDCUnderCfg = 0;  
                      sBattInfo.sRAWBattInfo.fDCOverCfg = 0;
                      sBattInfo.sRAWBattInfo.u8BankNo = 0;
                      sBattInfo.sRAWBattInfo.fCapTotal = 0;
                      sBattInfo.sRAWBattInfo.fCCLVal = 0; 
                      sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel = 0;
                      sBattInfo.sRAWBattInfo.fOvMjTempAlrmLevel = 0;
                      sRectInfo.u8WITI = 0;
                      sRectInfo.u8WITE = 0;
                      sAcInfo.fAcLowThres = 0;
                      sAcInfo.fAcUnderThres = 0;
                      sAcInfo.fAcHighThres = 0;
                      sBattTestInfo.fBattTestVolt = 0;
                      sBattTestInfo.fBattTestDur = 0;
                      sBattTestInfo.fBattTestDur = 0;
                      sBattTestInfo.u8TimeTestEn = 0;
                      privateMibBase.cfgBTGroup.cfgBTPlanTestTable[0].cfgBTPlanTestMonth = 0;
                      privateMibBase.cfgBTGroup.cfgBTPlanTestTable[0].cfgBTPlanTestDate = 0;
                      privateMibBase.cfgBTGroup.cfgBTPlanTestTable[0].cfgBTPlanTestHour = 0;
                      privateMibBase.cfgBTGroup.cfgBTPlanTestTable[1].cfgBTPlanTestMonth = 0;
                      privateMibBase.cfgBTGroup.cfgBTPlanTestTable[1].cfgBTPlanTestDate = 0;
                      privateMibBase.cfgBTGroup.cfgBTPlanTestTable[1].cfgBTPlanTestHour = 0;
                      privateMibBase.cfgBTGroup.cfgBTPlanTestTable[2].cfgBTPlanTestMonth = 0;
                      privateMibBase.cfgBTGroup.cfgBTPlanTestTable[2].cfgBTPlanTestDate = 0;
                      privateMibBase.cfgBTGroup.cfgBTPlanTestTable[2].cfgBTPlanTestHour = 0;
                      privateMibBase.cfgBTGroup.cfgBTPlanTestTable[3].cfgBTPlanTestMonth = 0;
                      privateMibBase.cfgBTGroup.cfgBTPlanTestTable[3].cfgBTPlanTestDate = 0;
                      privateMibBase.cfgBTGroup.cfgBTPlanTestTable[3].cfgBTPlanTestHour = 0;
                      }
                    }
                    

                    RecvCntStart = 0;
                    MESGState = RECTINFO_REQ;
                    
                    break;
                //====================================== SYSINFO MESSAGE ======================================//

                //====================================== RECTINFO MESSAGE ======================================//
                case  RECTINFO_REQ:
                    for (i = 0; i < GetRecInfoLen; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&GetRecInfo[i],1);
                    }
                    MESGState = RECTINFO_RES;
                    
                    vTaskDelay(300);
                    break;
                case  RECTINFO_RES:
//                    memset(&RecvBuff[0],0,300);
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    
//                    memset(&g_UARTRxBuf[0],0,300);
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sRectInfo.fAllRectDcVolt = hex2float(&RecvBuff[0],15);
                        sRectInfo.u8Rect_Num = hex2byte(&RecvBuff[0],23);   
                        
                        j = sRectInfo.u8Rect_Num;
                        if ((j <= 24) && (j > 0))
                        {
                          sRectInfo.u8Rect_InfoNum = hex2byte(&RecvBuff[0],33);                          
                          
                          while (j > 0)
                          {  
                            switch(sRectInfo.u8Rect_InfoNum)
                            {
                            case 0x03:
                              {
                                sRectInfo.sRAWRectParam[j-1].fRect_DcCurr= hex2float(&RecvBuff[0],25 + 34*(j-1));
                                sRectInfo.sRAWRectParam[j-1].fRect_Temp = 0;
                                sRectInfo.sRAWRectParam[j-1].fRect_LimCurr = hex2float(&RecvBuff[0],35 + 34*(j-1));
                                sRectInfo.sRAWRectParam[j-1].fRect_DcVolt = hex2float(&RecvBuff[0],43 + 34*(j-1));
                                sRectInfo.sRAWRectParam[j-1].fRect_AcVolt = hex2float(&RecvBuff[0],51 + 34*(j-1));

                                j--;
                              }break;
                            case 0x04:
                              {
                                sRectInfo.sRAWRectParam[j-1].fRect_DcCurr= hex2float(&RecvBuff[0],25 + 42*(j-1));
                                sRectInfo.sRAWRectParam[j-1].fRect_Temp = hex2float(&RecvBuff[0],35 + 42*(j-1));
                                sRectInfo.sRAWRectParam[j-1].fRect_LimCurr = hex2float(&RecvBuff[0],43 + 42*(j-1));
                                sRectInfo.sRAWRectParam[j-1].fRect_DcVolt = hex2float(&RecvBuff[0],51 + 42*(j-1));
                                sRectInfo.sRAWRectParam[j-1].fRect_AcVolt = hex2float(&RecvBuff[0],59 + 42*(j-1));

                                j--;
                              }break;
                            default:
                              break;
                            }; 
                          }
                        }
                    }


                    RecvCntStart = 0;
                    MESGState = ACINFO_REQ;
                    break;
                //====================================== RECTINFO MESSAGE ======================================//

                //====================================== ACINFO MESSAGE ======================================//
                case  ACINFO_REQ:
                    for (i = 0; i < GetACInfoLen; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&GetACInfo[i],1);
                    }
                    MESGState = ACINFO_RES;
                    break;
                case  ACINFO_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sAcInfo.facVolt[0] = hex2float(&RecvBuff[0],19);
                        sAcInfo.facVolt[1] = hex2float(&RecvBuff[0],27);
                        sAcInfo.facVolt[2] = hex2float(&RecvBuff[0],35);
                    }

                    RecvCntStart = 0;
                    MESGState = ALARM_DC_REQ;
                    break;
                //====================================== ACINFO MESSAGE ======================================//

                //====================================== ALARM DC MESSAGE ======================================//
                case  ALARM_DC_REQ:
                    for (i = 0; i < GetAlarmDCLen; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&GetAlarmDC[i],1);
                    }
                    MESGState = ALARM_DC_RES;
                    break;
                case  ALARM_DC_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sDcInfo.sALARMDcInfo.u8DC = hex2byte(&RecvBuff[0],17);
                        sDcInfo.sALARMDcInfo.u8DC_NoFuse = hex2byte(&RecvBuff[0],19);
                        sDcInfo.sALARMDcInfo.u8NumOfUsrDefInfo = hex2byte(&RecvBuff[0],41);

                        for (i = 0; i < 10; i++)
                        {
                            sDcInfo.sALARMDcInfo.u8LoadFuse[i] = hex2byte(&RecvBuff[0],21 + 2*i);
                        }

                        for (i = 0; i < 4; i++)
                        {
                            sDcInfo.sALARMDcInfo.u8BattFuse[i] = hex2byte(&RecvBuff[0],43 + 2*i);
                        }
                        
                        switch(sDcInfo.sALARMDcInfo.u8NumOfUsrDefInfo)
                        {
                        case 0x1A:
                          {
                            for (i = 0; i < 8; i++)
                            {
                                sDcInfo.sALARMDcInfo.u8DIFuse[i] = hex2byte(&RecvBuff[0],79 + 2*i);
                            }
                            sDcInfo.sALARMDcInfo.u8Batt1_OC = hex2byte(&RecvBuff[0],51);
                            sDcInfo.sALARMDcInfo.u8Batt2_OC = hex2byte(&RecvBuff[0],53);
                            sDcInfo.sALARMDcInfo.u8BLVD = hex2byte(&RecvBuff[0],55);
                            sDcInfo.sALARMDcInfo.u8LLVD = hex2byte(&RecvBuff[0],57);
                            sDcInfo.sALARMDcInfo.u8Sen1_BattTemp = hex2byte(&RecvBuff[0],59);
                            sDcInfo.sALARMDcInfo.u8Sen2_BattTemp = hex2byte(&RecvBuff[0],61);
                            sDcInfo.sALARMDcInfo.u8Sen1_AmbTemp = hex2byte(&RecvBuff[0],63);
                            sDcInfo.sALARMDcInfo.u8Sen2_AmbTemp = hex2byte(&RecvBuff[0],65);
                            sDcInfo.sALARMDcInfo.u8Batt_Discharge = hex2byte(&RecvBuff[0],67);
                            sDcInfo.sALARMDcInfo.u8OutVoltFault = hex2byte(&RecvBuff[0],77);                            
                          }break;
                        case 0x1F:
                          {
                            for (i = 0; i < 8; i++)
                            {
                                sDcInfo.sALARMDcInfo.u8DIFuse[i] = hex2byte(&RecvBuff[0],89 + 2*i);
                            }
                            sDcInfo.sALARMDcInfo.u8Batt1_OC = hex2byte(&RecvBuff[0],51);
                            sDcInfo.sALARMDcInfo.u8Batt2_OC = hex2byte(&RecvBuff[0],53);
                            sDcInfo.sALARMDcInfo.u8BLVD = hex2byte(&RecvBuff[0],55);
                            sDcInfo.sALARMDcInfo.u8LLVD = hex2byte(&RecvBuff[0],59);
                            sDcInfo.sALARMDcInfo.u8Sen1_BattTemp = hex2byte(&RecvBuff[0],63);
                            sDcInfo.sALARMDcInfo.u8Sen2_BattTemp = hex2byte(&RecvBuff[0],65);
                            sDcInfo.sALARMDcInfo.u8Sen1_AmbTemp = hex2byte(&RecvBuff[0],67);
                            sDcInfo.sALARMDcInfo.u8Sen2_AmbTemp = hex2byte(&RecvBuff[0],69);
                            sDcInfo.sALARMDcInfo.u8Batt_Discharge = hex2byte(&RecvBuff[0],79);
                            sDcInfo.sALARMDcInfo.u8OutVoltFault = hex2byte(&RecvBuff[0],87);  
                            
                          }break;
                        default:
                          break;
                        };        
                    }

                    RecvCntStart = 0;
                    MESGState = ALARM_RECT_REQ;
                    break;
                //====================================== ALARM DC MESSAGE ======================================//

                //====================================== ALARM RECT MESSAGE ======================================//
                case  ALARM_RECT_REQ:
                    for (i = 0; i < GetAlarmRectLen; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&GetAlarmRect[i],1);
                    }
                    MESGState = ALARM_RECT_RES;
                    break;
                case  ALARM_RECT_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
//							sRectInfo.u8Rect_Num = hex2byte(&RecvBuff[0],15);
                        j = sRectInfo.u8Rect_Num;
                        if (j <= 24)
                        {
                            while (j > 0)
                            {
                              switch(sRectInfo.u8Rect_InfoNum)
                              {
                              case 0x03:
                                {
//                                  sRectInfo.sRAWRectParam[j-1].u8Rect_ACFault = hex2byte(&RecvBuff[0],29 + 16*(j-1));//RecvBuff[30 + 16*(j-1)];
                                  sRectInfo.sRAWRectParam[j-1].u8Rect_Fail = hex2byte(&RecvBuff[0],23 + 16*(j-1));
                                  sRectInfo.sRAWRectParam[j-1].u8Rect_NoResp = hex2byte(&RecvBuff[0],27 + 16*(j-1));
                                  j--;
                                }break;
                              case 0x04:
                                {
                                  sRectInfo.sRAWRectParam[j-1].u8Rect_ACFault = hex2byte(&RecvBuff[0],29 + 16*(j-1));//RecvBuff[30 + 16*(j-1)];
                                  sRectInfo.sRAWRectParam[j-1].u8Rect_Fail = hex2byte(&RecvBuff[0],23 + 16*(j-1));
                                  sRectInfo.sRAWRectParam[j-1].u8Rect_NoResp = hex2byte(&RecvBuff[0],31 + 16*(j-1));
                                  j--;
                                }break;
                              default:
                                j--;
                                break;
                              }; 
                                
                            }
                        }
                    }


                    RecvCntStart = 0;
                    MESGState = ALARM_AC_REQ;
                    break;
                //====================================== ALARM RECT MESSAGE ======================================//

                //====================================== ALARM AC MESSAGE ======================================//
                case  ALARM_AC_REQ:
                    for (i = 0; i < GetAlarmACLen; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&GetAlarmAC[i],1);
                    }
                    MESGState = ALARM_AC_RES;
                    break;
                case  ALARM_AC_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sAcInfo.u8Thres[0] = hex2byte(&RecvBuff[0],19);
                        sAcInfo.u8Thres[1] = hex2byte(&RecvBuff[0],21);
                        sAcInfo.u8Thres[2] = hex2byte(&RecvBuff[0],23);
                        sAcInfo.u8MainFail = hex2byte(&RecvBuff[0],37);
                    }


                    RecvCntStart = 0;
                    MESGState = BATT_FLOATVOLTAGE_REQ;//
                    break;
                //====================================== ALARM AC MESSAGE ======================================//

                //====================================== ALARM MAN MESSAGE ======================================//
                case  ALARM_MAN_REQ:
                    for (i = 0; i < GetAlarmManLen; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&GetAlarmMan[i],1);
                    }
                    MESGState = ALARM_MAN_RES;
                    break;
                case  ALARM_MAN_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        g_AlrmMan = hex2byte(&RecvBuff[0],13);
                    }


                    RecvCntStart = 0;
                    MESGState = SYSINFO_REQ;//BATT_FLOATVOLTAGE_REQ;
                    break;
                //====================================== ALARM MAN MESSAGE ======================================//


                //====================================== BatteryFloatVoltage MESSAGE ======================================//
                case BATT_FLOATVOLTAGE_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[0][0]);

                    for (i = 0; i < 24; i++)
                    {
//                        athao[i]= g_getInfo[i];
                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = BATT_FLOATVOLTAGE_RES;
                    break;

                case BATT_FLOATVOLTAGE_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sBattInfo.sRAWBattInfo.fFltVoltCfg = roundf(hex2float(&RecvBuff[0],13)*10)/10;
                    }


                    RecvCntStart = 0;
                    MESGState = BATT_BOOSTVOLTAGE_REQ;
                    break;
                //====================================== BatteryFloatVoltage MESSAGE ======================================//

                //====================================== BatteryBoostVoltage MESSAGE ======================================//
                case BATT_BOOSTVOLTAGE_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[1][0]);

                    for (i = 0; i < 24; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = BATT_BOOSTVOLTAGE_RES;
                    break;

                case BATT_BOOSTVOLTAGE_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sBattInfo.sRAWBattInfo.fBotVoltCfg = roundf(hex2float(&RecvBuff[0],13)*10)/10;
                    }


                    RecvCntStart = 0;
                    MESGState = BATT_TEMPCOMPEN_REQ;
                    break;
                //====================================== BatteryBoostVoltage MESSAGE ======================================//

                //====================================== BatteryTemperatureCompensation MESSAGE ======================================//
                case BATT_TEMPCOMPEN_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[2][0]);

                    for (i = 0; i < 24; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = BATT_TEMPCOMPEN_RES;
                    break;

                case BATT_TEMPCOMPEN_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sBattInfo.sRAWBattInfo.fTempCompVal = hex2float(&RecvBuff[0],13);
                    }


                    RecvCntStart = 0;
                    MESGState = BATT_LLVDEN_REQ;
                    break;
                //====================================== BatteryTemperatureCompensation MESSAGE ======================================//
                //====================================== LLVDEnable MESSAGE ======================================//
                case BATT_LLVDEN_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[72][0]);

                    for (i = 0; i < 24; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = BATT_LLVDEN_RES;
                    break;

                case BATT_LLVDEN_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sBattInfo.sRAWBattInfo.u8LLVDE = hex2byte(&RecvBuff[0],13);
                    }


                    RecvCntStart = 0;
                    MESGState = BATT_BLVDEN_REQ;
                    break;
                //====================================== LLVDEnable MESSAGE ======================================//
                //====================================== BLVDEnable MESSAGE ======================================//
                case BATT_BLVDEN_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[73][0]);

                    for (i = 0; i < 24; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = BATT_BLVDEN_RES;
                    break;

                case BATT_BLVDEN_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sBattInfo.sRAWBattInfo.u8BLVDE = hex2byte(&RecvBuff[0],13);
                    }


                    RecvCntStart = 0;
                    MESGState = BATT_LLVD_REQ;
                    break;
                //====================================== BLVDEnable MESSAGE ======================================//
                //   ====================================== BatteryLLVD MESSAGE ======================================//
                case BATT_LLVD_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[3][0]);

                    for (i = 0; i < 24; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = BATT_LLVD_RES;
                    break;

                case BATT_LLVD_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg = roundf(hex2float(&RecvBuff[0],13)*10)/10;
                    }


                    RecvCntStart = 0;
                    MESGState = BATT_BLVD_REQ;
                    break;
                //====================================== BatteryLLVD MESSAGE ======================================//

                //====================================== BatteryBLVD MESSAGE ======================================//
                case BATT_BLVD_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[4][0]);

                    for (i = 0; i < 24; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = BATT_BLVD_RES;
                    break;

                case BATT_BLVD_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sBattInfo.sRAWBattInfo.fLVDDV = roundf(hex2float(&RecvBuff[0],13)*10)/10;
                    }


                    RecvCntStart = 0;
                    MESGState = BATT_DCLOW_REQ;
                    break;
                //====================================== BatteryBLVD MESSAGE ======================================//
                //====================================== BatteryDCLOW MESSAGE ======================================//
                case BATT_DCLOW_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[5][0]);

                    for (i = 0; i < 24; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = BATT_DCLOW_RES;
                    break;

                case BATT_DCLOW_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg = roundf(hex2float(&RecvBuff[0],13)*10)/10;
                    }


                    RecvCntStart = 0;
                    MESGState = BATT_DCUNDER_REQ;
                    break;
                //====================================== BatteryDCLOW MESSAGE ======================================//
                //====================================== BatteryDCUNDER MESSAGE ======================================//
                case BATT_DCUNDER_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[6][0]);

                    for (i = 0; i < 24; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = BATT_DCUNDER_RES;
                    break;

                case BATT_DCUNDER_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sBattInfo.sRAWBattInfo.fDCUnderCfg = roundf(hex2float(&RecvBuff[0],13)*10)/10;
                    }


                    RecvCntStart = 0;
                    MESGState = BATT_DCOVER_REQ;
                    break;
                //====================================== BatteryDCUNDER MESSAGE ======================================//
                //====================================== BatteryDCOVER MESSAGE ======================================//
                case BATT_DCOVER_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[7][0]);

                    for (i = 0; i < 24; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = BATT_DCOVER_RES;
                    break;

                case BATT_DCOVER_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sBattInfo.sRAWBattInfo.fDCOverCfg = roundf(hex2float(&RecvBuff[0],13)*10)/10;
                    }


                    RecvCntStart = 0;
                    MESGState = BATT_STRING_REQ;
                    break;
                //====================================== BatteryDCOVER MESSAGE ======================================//
                //====================================== BatteryStringNo MESSAGE ======================================//
                case BATT_STRING_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[9][0]);

                    for (i = 0; i < 24; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = BATT_STRING_RES;
                    break;

                case BATT_STRING_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sBattInfo.sRAWBattInfo.u8BankNo = hex2byte(&RecvBuff[0],13);
                    }


                    RecvCntStart = 0;
                    MESGState = BATT_STDCAP_REQ;
                    break;
                //====================================== BatteryStringNo MESSAGE ======================================//

                //====================================== BatteryStandardCapacity MESSAGE ======================================//
                case BATT_STDCAP_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[15][0]);

                    for (i = 0; i < 24; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = BATT_STDCAP_RES;
                    break;

                case BATT_STDCAP_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sBattInfo.sRAWBattInfo.fCapTotal = hex2float(&RecvBuff[0],13);
                    }


                    RecvCntStart = 0;
                    MESGState = BATT_CURRLIMIT_REQ;
                    break;
                //====================================== BatteryStandardCapacity MESSAGE ======================================//

                //====================================== BatteryCurrentLimitPoint MESSAGE ======================================//
                case BATT_CURRLIMIT_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[17][0]);

                    for (i = 0; i < 24; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = BATT_CURRLIMIT_RES;
                    break;

                case BATT_CURRLIMIT_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sBattInfo.sRAWBattInfo.fCCLVal = roundf(hex2float(&RecvBuff[0],13)*1000)/1000;
                    }


                    RecvCntStart = 0;
                    MESGState = BATT_HIGHTEMPALARM_REQ;
                    break;
                //====================================== BatteryCurrentLimitPoint MESSAGE ======================================//

                //====================================== BatteryHighTempAlarm MESSAGE ======================================//
                case BATT_HIGHTEMPALARM_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[30][0]);

                    for (i = 0; i < 24; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = BATT_HIGHTEMPALARM_RES;
                    break;

                case BATT_HIGHTEMPALARM_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel = roundf(hex2float(&RecvBuff[0],13)*10)/10;
                    }


                    RecvCntStart = 0;
                    MESGState = BATT_OVERTEMPALARM_REQ;
                    break;
                //====================================== BatteryHighTempAlarm MESSAGE ======================================//
                //====================================== BatteryOverTemp MESSAGE ======================================//
                case BATT_OVERTEMPALARM_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[31][0]);

                    for (i = 0; i < 24; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = BATT_OVERTEMPALARM_RES;
                    break;

                case BATT_OVERTEMPALARM_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sBattInfo.sRAWBattInfo.fOvMjTempAlrmLevel = roundf(hex2float(&RecvBuff[0],13)*10)/10;
                    }


                    RecvCntStart = 0;
                    MESGState = SYS_WALKINTIMEDUR_REQ;
                    break;
                //====================================== BatteryOverTemp MESSAGE ======================================//
                //====================================== SystemWalkInTimeDuration MESSAGE ======================================//
                case SYS_WALKINTIMEDUR_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[36][0]);

                    for (i = 0; i < 24; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = SYS_WALKINTIMEDUR_RES;
                    break;

                case SYS_WALKINTIMEDUR_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sRectInfo.u8WITI = (uint8_t)hex2float(&RecvBuff[0],13);
                    }


                    RecvCntStart = 0;
                    MESGState = SYS_WALKINTIMEEN_REQ;
                    break;
                //====================================== SystemWalkInTimeDuration MESSAGE ======================================//

                //====================================== SystemWalkInTimeEnable MESSAGE ======================================//
                case SYS_WALKINTIMEEN_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[37][0]);

                    for (i = 0; i < 24; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = SYS_WALKINTIMEEN_RES;
                    break;

                case SYS_WALKINTIMEEN_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sRectInfo.u8WITE = hex2byte(&RecvBuff[0],13);
                    }


                    RecvCntStart = 0;
                    MESGState = AC_LOWTHRES_REQ;//ATSINFO_REQ;
                    break;

                //====================================== AC Low Thres MESSAGE ======================================//
                case AC_LOWTHRES_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[45][0]);

                    for (i = 0; i < 24; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = AC_LOWTHRES_RES;
                    break;

                case AC_LOWTHRES_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sAcInfo.fAcLowThres = hex2float(&RecvBuff[0],13);
                    }


                    RecvCntStart = 0;
                    MESGState = AC_UNDERTHRES_REQ;//ATSINFO_REQ;
                    break;
                //====================================== AC Low Thres MESSAGE ======================================//
                //====================================== AC Under Thres MESSAGE ======================================//
                case AC_UNDERTHRES_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[46][0]);
                    for (i = 0; i < 24; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = AC_UNDERTHRES_RES;
                    break;

                case AC_UNDERTHRES_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
//								sAcInfo.fAcHighThres = hex2float(&RecvBuff[0],13);
                        sAcInfo.fAcUnderThres = hex2float(&RecvBuff[0],13);
//								sAcInfo.fAcLowThres = hex2float(&RecvBuff[0],2);
                    }


                    RecvCntStart = 0;
                    MESGState = AC_HITHRES_REQ;//ATSINFO_REQ;
                    break;
                //====================================== AC Under Thres MESSAGE ======================================//
                //====================================== AC High Thres MESSAGE ======================================//
                case AC_HITHRES_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[47][0]);

                    for (i = 0; i < 24; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = AC_HITHRES_RES;
                    break;

                case AC_HITHRES_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sAcInfo.fAcHighThres = hex2float(&RecvBuff[0],13);
//								sAcInfo.fAcHighThres = 280;
                    }


                    RecvCntStart = 0;
                    MESGState = BATT_TESTVOLT_REQ ;//ATSINFO_REQ;
                    break;
                //====================================== AC High Thres MESSAGE ======================================//
                //====================================== Battery Test Voltage MESSAGE ======================================//
                case BATT_TESTVOLT_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[48][0]);

                    for (i = 0; i < 24; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = BATT_TESTVOLT_RES;
                    break;

                case BATT_TESTVOLT_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sBattTestInfo.fBattTestVolt = roundf(hex2float(&RecvBuff[0],13)*10)/10;
                    }

                    RecvCntStart = 0;
                    MESGState = BATT_TESTDUR_REQ;//ATSINFO_REQ;
                    break;
                //====================================== Battery Test Voltage MESSAGE ======================================//
                //====================================== Battery Test Duration MESSAGE ======================================//
                case BATT_TESTDUR_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[49][0]);

                    for (i = 0; i < 24; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = BATT_TESTDUR_RES;
                    break;

                case BATT_TESTDUR_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sBattTestInfo.fBattTestDur = roundf(hex2float(&RecvBuff[0],13)*10)/10;
                    }

                    RecvCntStart = 0;
                    MESGState = TEST_ENDCAP_REQ;//ATSINFO_REQ;
                    break;
                //====================================== Battery Test Duration MESSAGE ======================================//
                //====================================== Test End Capacity ======================================//
                case TEST_ENDCAP_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[50][0]);

                    for (i = 0; i < 24; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = TEST_ENDCAP_RES;
                    break;

                case TEST_ENDCAP_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sBattTestInfo.fTestEndCap = roundf(hex2float(&RecvBuff[0],13)*1000)/1000;
                    }

                    RecvCntStart = 0;
                    MESGState = TIME_TESTEN_REQ;//ATSINFO_REQ;
                    break;
                //====================================== Test End Capacity ======================================//
                //====================================== Time Test Enable ======================================//
                case TIME_TESTEN_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[51][0]);

                    for (i = 0; i < 24; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = TIME_TESTEN_RES;
                    break;

                case TIME_TESTEN_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        sBattTestInfo.u8TimeTestEn = hex2byte(&RecvBuff[0],13);
//								sAcInfo.fAcHighThres = 280;
                    }

                    RecvCntStart = 0;
                    MESGState = TEST1_MONTH_REQ;//ATSINFO_REQ;
                    break;
                //====================================== Time Test Enable ======================================//
                //====================================== Test Time 1 Month ======================================//
                case TEST1_MONTH_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[60][0]);

                    for (i = 0; i < 24; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = TEST1_MONTH_RES;
                    break;

                case TEST1_MONTH_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        privateMibBase.cfgBTGroup.cfgBTPlanTestTable[0].cfgBTPlanTestMonth = hex2byte(&RecvBuff[0],13);
                    }

                    RecvCntStart = 0;
                    MESGState = TEST1_DAY_REQ;
                    break;
                //====================================== Test Time 1 Month ======================================//
                //====================================== Test Time 1 Day ======================================//
                case TEST1_DAY_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[61][0]);

                    for (i = 0; i < 24; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = TEST1_DAY_RES;
                    break;

                case TEST1_DAY_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        privateMibBase.cfgBTGroup.cfgBTPlanTestTable[0].cfgBTPlanTestDate = hex2byte(&RecvBuff[0],13);
                    }

                    RecvCntStart = 0;
                    MESGState = TEST1_HOUR_REQ;
                    break;
                //====================================== Test Time 1 Day ======================================//
                //====================================== Test Time 1 Hour ======================================//
                case TEST1_HOUR_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[62][0]);

                    for (i = 0; i < 24; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = TEST1_HOUR_RES;
                    break;

                case TEST1_HOUR_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        privateMibBase.cfgBTGroup.cfgBTPlanTestTable[0].cfgBTPlanTestHour = hex2byte(&RecvBuff[0],13);
                    }

                    RecvCntStart = 0;
                    MESGState = TEST2_MONTH_REQ;
                    break;
                //====================================== Test Time 1 Hour ======================================//
                //====================================== Test Time 2 Month ======================================//
                case TEST2_MONTH_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[63][0]);

                    for (i = 0; i < 24; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = TEST2_MONTH_RES;
                    break;

                case TEST2_MONTH_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        privateMibBase.cfgBTGroup.cfgBTPlanTestTable[1].cfgBTPlanTestMonth = hex2byte(&RecvBuff[0],13);
                    }

                    RecvCntStart = 0;
                    MESGState = TEST2_DAY_REQ;
                    break;
                //====================================== Test Time 2 Month ======================================//
                //====================================== Test Time 2 Day ======================================//
                case TEST2_DAY_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[64][0]);

                    for (i = 0; i < 24; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = TEST2_DAY_RES;
                    break;

                case TEST2_DAY_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        privateMibBase.cfgBTGroup.cfgBTPlanTestTable[1].cfgBTPlanTestDate = hex2byte(&RecvBuff[0],13);
                    }

                    RecvCntStart = 0;
                    MESGState = TEST2_HOUR_REQ;
                    break;
                //====================================== Test Time 2 Day ======================================//
                //====================================== Test Time 2 Hour ======================================//
                case TEST2_HOUR_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[65][0]);

                    for (i = 0; i < 24; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = TEST2_HOUR_RES;
                    break;

                case TEST2_HOUR_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        privateMibBase.cfgBTGroup.cfgBTPlanTestTable[1].cfgBTPlanTestHour = hex2byte(&RecvBuff[0],13);
                    }

                    RecvCntStart = 0;
                    MESGState = TEST3_MONTH_REQ;
                    break;
                //====================================== Test Time 2 Hour ======================================//
                //====================================== Test Time 3 Month ======================================//
                case TEST3_MONTH_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[66][0]);

                    for (i = 0; i < 24; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = TEST3_MONTH_RES;
                    break;

                case TEST3_MONTH_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        privateMibBase.cfgBTGroup.cfgBTPlanTestTable[2].cfgBTPlanTestMonth = hex2byte(&RecvBuff[0],13);
                    }

                    RecvCntStart = 0;
                    MESGState = TEST3_DAY_REQ;
                    break;
                //====================================== Test Time 3 Month ======================================//
                //====================================== Test Time 3 Day ======================================//
                case TEST3_DAY_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[67][0]);

                    for (i = 0; i < 24; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = TEST3_DAY_RES;
                    break;

                case TEST3_DAY_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        privateMibBase.cfgBTGroup.cfgBTPlanTestTable[2].cfgBTPlanTestDate = hex2byte(&RecvBuff[0],13);
                    }

                    RecvCntStart = 0;
                    MESGState = TEST3_HOUR_REQ;
                    break;
                //====================================== Test Time 3 Day ======================================//
                //====================================== Test Time 3 Hour ======================================//
                case TEST3_HOUR_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[68][0]);

                    for (i = 0; i < 24; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = TEST3_HOUR_RES;
                    break;

                case TEST3_HOUR_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        privateMibBase.cfgBTGroup.cfgBTPlanTestTable[2].cfgBTPlanTestHour = hex2byte(&RecvBuff[0],13);
                    }

                    RecvCntStart = 0;
                    MESGState = TEST4_MONTH_REQ;
                    break;
                //====================================== Test Time 3 Hour ======================================//
                //====================================== Test Time 4 Month ======================================//
                case TEST4_MONTH_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[69][0]);

                    for (i = 0; i < 24; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = TEST4_MONTH_RES;
                    break;

                case TEST4_MONTH_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        privateMibBase.cfgBTGroup.cfgBTPlanTestTable[3].cfgBTPlanTestMonth = hex2byte(&RecvBuff[0],13);
                    }

                    RecvCntStart = 0;
                    MESGState = TEST4_DAY_REQ;
                    break;
                //====================================== Test Time 4 Month ======================================//
                //====================================== Test Time 4 Day ======================================//
                case TEST4_DAY_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[70][0]);

                    for (i = 0; i < 24; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = TEST4_DAY_RES;
                    break;

                case TEST4_DAY_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        privateMibBase.cfgBTGroup.cfgBTPlanTestTable[3].cfgBTPlanTestDate = hex2byte(&RecvBuff[0],13);
                    }

                    RecvCntStart = 0;
                    MESGState = TEST4_HOUR_REQ;
                    break;
                //====================================== Test Time 4 Day ======================================//
                //====================================== Test Time 4 Hour ======================================//
                case TEST4_HOUR_REQ:

                    getCommand(&g_getInfo[0],&cmdCodeTable[71][0]);

                    for (i = 0; i < 24; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_getInfo[i],1);
                    }

                    MESGState = TEST4_HOUR_RES;
                    break;

                case TEST4_HOUR_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        privateMibBase.cfgBTGroup.cfgBTPlanTestTable[3].cfgBTPlanTestHour = hex2byte(&RecvBuff[0],13);
                    }

                    RecvCntStart = 0;
                    MESGState = SYS_TIME_REQ;
                    break;
                //====================================== Test Time 4 Hour ======================================//
                //====================================== Sys Time  ======================================//
                case SYS_TIME_REQ:
                    for (i = 0; i < GetTimeInfoLen; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&GetTimeInfo[i],1);
                    }

                    MESGState = SYS_TIME_RES;
                    break;

                case SYS_TIME_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        temp[0] = hex2byte(&RecvBuff[0],13);
                        temp[1] = hex2byte(&RecvBuff[0],15);

                        privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUYear = temp[0] * 100 + temp[1];
                        privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUMonth = hex2byte(&RecvBuff[0],17);
                        privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUDate = hex2byte(&RecvBuff[0],19);
                        privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUHour = hex2byte(&RecvBuff[0],21);
                        privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUMinute = hex2byte(&RecvBuff[0],23);
                        privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUSecond = hex2byte(&RecvBuff[0],25);
                        temp[0] = 0;
                        temp[1] = 0;
                    }

                    RecvCntStart = 0;
                    MESGState = UPDATE_OK;
                    break;
                //====================================== Sys Time ======================================//
//#if	IPS_VERSION
//                //====================================== ETHERNET INFO MESSAGE ======================================//
//                case  ETHSYNC_REQ:
//
//                        u322char(&EthInfoSync[13], g_sParameters.ulStaticIP, 8);
//                        u322char(&EthInfoSync[21], g_sParameters.ulSubnetMask, 8);
//                        u322char(&EthInfoSync[29], g_sParameters.ulGatewayIP, 8);
//                        u322char(&EthInfoSync[37], g_sParameters.sPort[1].ulTelnetIPAddr, 8);
//                        hex2char(&EthInfoSync[45],checksum(&EthInfoSync[0],50),4);
//
//                        for (i = 0; i < 50; i++)
//                        {
//                                SerialSend(EthInfoSync[i]);
//                        }
//                        MESGState = ETHSYNC_RES;
//                break;
//                case  ETHSYNC_RES:
//                        RingBufWrite(&g_sRxBuf, g_UARTRxBuf,RecvCntStart);
//                        RecvCount = RingBufUsed(&g_sRxBuf);
//                        RingBufRead(&g_sRxBuf,RecvBuff,RecvCount);
//
//                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
//                        g_checksum = checksum(&RecvBuff[0],RecvCount);
//                        if (g_checksum == g_testchecksum)
//                        {
//
//                        }
//
//
//                        RecvCntStart = 0;
//                        MESGState = UPDATE_OK;
//                break;
//                //====================================== ETHERNET INFO MESSAGE ======================================//
//#endif
                //====================================== SystemWalkInTimeEnable MESSAGE ======================================//

                case UPDATE_OK:

                    privateMibBase.batteryGroup.battVolt = (int32_t) (sDcInfo.sINFODcInfo.fBatt1Volt * 100);//privateMibBase.batteryGroup.battVolt
                    privateMibBase.batteryGroup.battCurr = (int32_t) ((sDcInfo.sINFODcInfo.fBatt1Curr + sDcInfo.sINFODcInfo.fBatt2Curr) * 100);//privateMibBase.batteryGroup.battCurr
                    privateMibBase.batteryGroup.battCapLeft1 = (uint32_t) (sDcInfo.sINFODcInfo.fBatt1RmnCap * 100);//sBattInfosSNMPBattInfo.u32CapLeft[0]
                    privateMibBase.batteryGroup.battCapLeft2 = (uint32_t) (sDcInfo.sINFODcInfo.fBatt2RmnCap * 100);//sBattInfo.sSNMPBattInfo.u32CapLeft[1]
                    // ======================== Update rectifier information to SNMP resource ==========================//
                    j = sRectInfo.u8Rect_Num;
                    privateMibBase.mainAlarmGroup.alarmRectACFault = 0;
                    privateMibBase.mainAlarmGroup.alarmRectNoResp = 0;
                    
                    privateMibBase.rectGroup.rectInstalledRect = sRectInfo.u8Rect_Num;
                    privateMibBase.rectGroup.rectActiveRect = sRectInfo.u8Rect_Num;
                    if (j <= 24)
                    {
                        while (j > 0)
                        {
//                            privateMibBase.rectGroup.rectTable[j-1].rectStatus = 0;
//
//                            privateMibBase.mainAlarmGroup.alarmRectACFault |= sRectInfo.sRAWRectParam[j-1].u8Rect_ACFault << ((j-1)*8);
//                            privateMibBase.mainAlarmGroup.alarmRectACFault |= sRectInfo.sRAWRectParam[j-1].u8Rect_Fail << ((j-1)*8);
//                            privateMibBase.mainAlarmGroup.alarmRectNoResp |= sRectInfo.sRAWRectParam[j-1].u8Rect_NoResp << ((j-1)*8);
//
//                            if (sRectInfo.sRAWRectParam[j-1].u8Rect_NoResp == 0)
//                            {
//                                privateMibBase.rectGroup.rectTable[j-1].rectFault = sRectInfo.sRAWRectParam[j-1].u8Rect_ACFault;
//                                privateMibBase.rectGroup.rectTable[j-1].rectNoResp = sRectInfo.sRAWRectParam[j-1].u8Rect_NoResp;
//                                privateMibBase.rectGroup.rectTable[j-1].rectOutputCurrent = (uint32_t) (sRectInfo.sRAWRectParam[j-1].fRect_DcCurr * 100);
//                                privateMibBase.rectGroup.rectTable[j-1].rectTemp = (uint32_t) (sRectInfo.sRAWRectParam[j-1].fRect_Temp * 100);
//                                privateMibBase.rectGroup.rectTable[j-1].rectOutputVoltage = (uint32_t) (sRectInfo.sRAWRectParam[j-1].fRect_DcVolt * 100);
//
//                                if(sRectInfo.sRAWRectParam[j-1].u8Rect_ACFault==1)
//                                {
//                                    privateMibBase.rectGroup.rectTable[j-1].rectStatus = 0xA5; // Rect fail
//                                }
//                                else
//                                {
//                                    privateMibBase.rectGroup.rectTable[j-1].rectStatus = 0xff;
//                                }
////                                                usnprintf(&sRectInfo.sREPORTRectParam[j-1].cRect_SN[0],15,"%u",
////                                                                sRectInfo.sRAWRectParam[j-1].u32Rect_SN
////                                                );
////                                sRectInfo.sREPORTRectParam[j-1].cRect_SN[15] = '\0';
//
//                            }
//                            else if (sRectInfo.sRAWRectParam[j-1].u8Rect_NoResp == 0xe2)
//                            {
//                                privateMibBase.rectGroup.rectTable[j-1].rectFault = sRectInfo.sRAWRectParam[j-1].u8Rect_ACFault;
//                                privateMibBase.rectGroup.rectTable[j-1].rectNoResp = sRectInfo.sRAWRectParam[j-1].u8Rect_NoResp;
//                                privateMibBase.rectGroup.rectTable[j-1].rectStatus = 0xaa;
////                                                usnprintf(&sRectInfo.sREPORTRectParam[j-1].cRect_SN[0],15,"%u",
////                                                                sRectInfo.sRAWRectParam[j-1].u32Rect_SN
////                                                );
////                                sRectInfo.sREPORTRectParam[j-1].cRect_SN[15] = '\0';
//
//                                privateMibBase.rectGroup.rectTable[j-1].rectOutputCurrent = 0;
//                                privateMibBase.rectGroup.rectTable[j-1].rectTemp = 0;
//                                privateMibBase.rectGroup.rectTable[j-1].rectOutputVoltage = 0;
//                            }
                          
                            privateMibBase.rectGroup.rectTable[j-1].rectStatus = 0;

                            switch(sRectInfo.sRAWRectParam[j-1].u8Rect_NoResp)
                            {
                              case 0:
                              {
                                privateMibBase.rectGroup.rectTable[j-1].rectFault = sRectInfo.sRAWRectParam[j-1].u8Rect_ACFault;
                                privateMibBase.rectGroup.rectTable[j-1].rectNoResp = sRectInfo.sRAWRectParam[j-1].u8Rect_NoResp;
                                privateMibBase.rectGroup.rectTable[j-1].rectOutputCurrent = (uint32_t) (sRectInfo.sRAWRectParam[j-1].fRect_DcCurr * 100);
                                privateMibBase.rectGroup.rectTable[j-1].rectTemp = (uint32_t) (sRectInfo.sRAWRectParam[j-1].fRect_Temp * 100);
                                privateMibBase.rectGroup.rectTable[j-1].rectOutputVoltage = (uint32_t) (sRectInfo.sRAWRectParam[j-1].fRect_DcVolt * 100);

                                if(sRectInfo.sRAWRectParam[j-1].u8Rect_ACFault==1)
                                {
                                  privateMibBase.rectGroup.rectTable[j-1].rectStatus = 0xA5; // Rect fail
                                  privateMibBase.mainAlarmGroup.alarmRectACFault |= (1 << (j-1));
                                }
                                else
                                {
                                  privateMibBase.rectGroup.rectTable[j-1].rectStatus = 0xff;
                                  privateMibBase.mainAlarmGroup.alarmRectACFault |= (0 << (j-1));
                                }
                                if(sRectInfo.sRAWRectParam[j-1].u8Rect_Fail==1)
                                {
                                  privateMibBase.rectGroup.rectTable[j-1].rectStatus = 0xA5; // Rect fail
                                  privateMibBase.mainAlarmGroup.alarmRectACFault |= (1 << (j-1));
                                }
                                else
                                {
                                  privateMibBase.rectGroup.rectTable[j-1].rectStatus = 0xff;
                                  privateMibBase.mainAlarmGroup.alarmRectACFault |= (0 << (j-1));
                                }
                                privateMibBase.mainAlarmGroup.alarmRectNoResp |= (0 << (j-1));
                              }break;
                              case 0xe2:
                              {
                                privateMibBase.rectGroup.rectTable[j-1].rectFault = sRectInfo.sRAWRectParam[j-1].u8Rect_ACFault;
                                privateMibBase.rectGroup.rectTable[j-1].rectNoResp = sRectInfo.sRAWRectParam[j-1].u8Rect_NoResp;
                                privateMibBase.rectGroup.rectTable[j-1].rectStatus = 0xaa;
                                privateMibBase.rectGroup.rectTable[j-1].rectOutputCurrent = 0;
                                privateMibBase.rectGroup.rectTable[j-1].rectTemp = 0;
                                privateMibBase.rectGroup.rectTable[j-1].rectOutputVoltage = 0;
                                privateMibBase.mainAlarmGroup.alarmRectNoResp |= (1 << (j-1));
                                privateMibBase.rectGroup.rectActiveRect--;
                                if (privateMibBase.rectGroup.rectActiveRect < 0)
                                {
                                  privateMibBase.rectGroup.rectActiveRect = 0;
                                }
                              }break;
                            };
                            j--;
                        }
                    }
                    if (privateMibBase.mainAlarmGroup.alarmRectACFault != 0) privateMibBase.mainAlarmGroup.alarmRectACFault = 1;
                    else privateMibBase.mainAlarmGroup.alarmRectACFault = 0;
                    if (privateMibBase.mainAlarmGroup.alarmRectNoResp != 0) privateMibBase.mainAlarmGroup.alarmRectNoResp = 1;
                    else privateMibBase.mainAlarmGroup.alarmRectNoResp = 0;
                    privateMibBase.rectGroup.rectTotalCurrent = 0;

                    for (i = 0; i < sRectInfo.u8Rect_Num; i++)
                    {
                        privateMibBase.rectGroup.rectTotalCurrent += (uint32_t) (privateMibBase.rectGroup.rectTable[i].rectOutputCurrent);
                    }

                    privateMibBase.loadGroup.loadCurrent = (uint32_t) (sDcInfo.sINFODcInfo.fCurrent * 100);
//							privateMibBase.loadGroup.loadCurrent = privateMibBase.rectGroup.rectTotalCurrent - privateMibBase.batteryGroup.battCurr;
//                        if (privateMibBase.loadGroup.loadCurrent >= 0) privateMibBase.loadGroup.loadCurrent = privateMibBase.loadGroup.loadCurrent;
//                        else if (privateMibBase.loadGroup.loadCurrent < 0) privateMibBase.loadGroup.loadCurrent = -privateMibBase.loadGroup.loadCurrent;

                    // ======================== Update rectifier information to SNMP resource ==========================//
                    // ======================== Update ac information to SNMP resource ==========================//
                    privateMibBase.acPhaseGroup.acPhaseTable[0].acPhaseVolt = (int32_t) (sAcInfo.facVolt[0] * 100);
                    privateMibBase.acPhaseGroup.acPhaseTable[1].acPhaseVolt = (int32_t) (sAcInfo.facVolt[1] * 100);
                    privateMibBase.acPhaseGroup.acPhaseTable[2].acPhaseVolt = (int32_t) (sAcInfo.facVolt[2] * 100);
                    // ======================== Update ac information to SNMP resource ==========================//
                    // ======================== Update alarm information to SNMP resource ==========================//
                    if(g_AlrmMan == 0xe1)privateMibBase.mainAlarmGroup.alarmManualMode =1;
                    else privateMibBase.mainAlarmGroup.alarmManualMode =0;
//			privateMibBase.mainAlarmGroup.alarmManualMode = g_AlrmMan;
                    if(sAcInfo.u8MainFail !=0)privateMibBase.mainAlarmGroup.alarmACmains =1;
                    else privateMibBase.mainAlarmGroup.alarmACmains =0;
//			privateMibBase.mainAlarmGroup.alarmACmains = sAcInfo.u8MainFail;
                    privateMibBase.mainAlarmGroup.alarmLLVD =0;
                    if(sDcInfo.sALARMDcInfo.u8LLVD == 0xE3)privateMibBase.mainAlarmGroup.alarmLLVD =1;
//			privateMibBase.mainAlarmGroup.alarmLLVD = sDcInfo.sALARMDcInfo.u8LLVD;
                    privateMibBase.mainAlarmGroup.alarmBLVD = 0;
                    if(sDcInfo.sALARMDcInfo.u8BLVD == 0xE4)privateMibBase.mainAlarmGroup.alarmBLVD =1;
//			privateMibBase.mainAlarmGroup.alarmBLVD = sDcInfo.sALARMDcInfo.u8BLVD;
//			sAlarmInfo.u32DCopen = sDcInfo.sALARMDcInfo.u8DC;
//                    if(sDcInfo.sINFODcInfo.fBatt1Volt<sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg)
//                     sAlarmInfo.u32DCopen = 0x01;
//                    else sAlarmInfo.u32DCopen = 0x00;
                    if(sDcInfo.sALARMDcInfo.u8DC == 1) privateMibBase.mainAlarmGroup.alarmDCLow = 1;
                    else privateMibBase.mainAlarmGroup.alarmDCLow = 0;
                       
                    if (sAcInfo.u8Thres[0] || sAcInfo.u8Thres[1] || sAcInfo.u8Thres[2])
                    {
                        privateMibBase.mainAlarmGroup.alarmACLow = 1;
                    }
                    else
                    {
                        privateMibBase.mainAlarmGroup.alarmACLow = 0;
                    }

                    privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen = 0;
                    for (i = 0; i < 10; i++)
                    {
                        if(sDcInfo.sALARMDcInfo.u8LoadFuse[i] == 0x03)
                        {
                            fuse_flag = 1;
                            privateMibBase.loadGroup.loadStatus[i] = 1;
                        }
                        else
                        {
                            fuse_flag = 0;
                            privateMibBase.loadGroup.loadStatus[i] = 0;
                        }
                        privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen |= (fuse_flag << i);
                    }
                    if (privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen != 0)
                    {
                        privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen = 1;
                    }
                    else
                    {
                        privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen = 0;
                    }

                    privateMibBase.mainAlarmGroup.alarmBattBreakerOpen = 0;
                    for (i = 0; i < 4; i++)
                    {
                        if(sDcInfo.sALARMDcInfo.u8BattFuse[i] == 0x03)
                        {
                            fuse_flag = 1;
                            privateMibBase.batteryGroup.battBrkStatus[i] = 1;
                        }
                        else
                        {
                            fuse_flag = 0;
                            privateMibBase.batteryGroup.battBrkStatus[i] = 0;
                        }
                        privateMibBase.mainAlarmGroup.alarmBattBreakerOpen |= (fuse_flag << i);
                    }
                    if (privateMibBase.mainAlarmGroup.alarmBattBreakerOpen != 0)
                    {
                        privateMibBase.mainAlarmGroup.alarmBattBreakerOpen = 1;
                    }
                    else
                    {
                        privateMibBase.mainAlarmGroup.alarmBattBreakerOpen = 0;
                    }

                    for (i = 0; i < 8; i++)
                    {
                        if(sDcInfo.sALARMDcInfo.u8DIFuse[i] == 0xEA)
                        {
                            fuse_flag = 1;
                            privateMibBase.diAlarmGroup.alarmDigitalInput[i] = 1;
                        }
                        else
                        {
                            fuse_flag = 0;
                            privateMibBase.diAlarmGroup.alarmDigitalInput[i] = 0;
                        }
                    }

                    switch(sDcInfo.sALARMDcInfo.u8Sen1_BattTemp)
                    {
                    case 0x20:
                        if ((sDcInfo.sALARMDcInfo.u8Sen2_BattTemp == 0x20 ) || (sDcInfo.sALARMDcInfo.u8Sen2_BattTemp == 0x05))
                        {
                            privateMibBase.batteryGroup.battTemp = 0;
                            if (sDcInfo.sALARMDcInfo.u8Sen2_BattTemp == 0x20)
                            {
                                privateMibBase.mainAlarmGroup.alarmBattHighTemp = 0x20;
                            }
                            else if (sDcInfo.sALARMDcInfo.u8Sen2_BattTemp == 0x05)
                            {
                                privateMibBase.mainAlarmGroup.alarmBattHighTemp = 0x05;
                            }
                        }
                        break;
                    case 0x00:
                        privateMibBase.batteryGroup.battTemp = (uint32_t)(sDcInfo.sINFODcInfo.fSen1BattTemp * 10);
                        privateMibBase.mainAlarmGroup.alarmBattHighTemp = 0x00;
                        break;
                    case 0x05:
                        if ((sDcInfo.sALARMDcInfo.u8Sen2_BattTemp == 0x20 ) || (sDcInfo.sALARMDcInfo.u8Sen2_BattTemp == 0x05))
                        {
                            privateMibBase.batteryGroup.battTemp = 0;
                            privateMibBase.mainAlarmGroup.alarmBattHighTemp = 0x05;
                        }
                        break;
                    case 0x02:
                        privateMibBase.batteryGroup.battTemp = (uint32_t)(sDcInfo.sINFODcInfo.fSen1BattTemp * 10);
                        privateMibBase.mainAlarmGroup.alarmBattHighTemp = 0x02;
                        break;
                    case 0x01:
                        privateMibBase.batteryGroup.battTemp = (uint32_t)(sDcInfo.sINFODcInfo.fSen1BattTemp * 10);
                        privateMibBase.mainAlarmGroup.alarmBattHighTemp = 0x01;
                        break;
                    }

                    switch(sDcInfo.sALARMDcInfo.u8Sen2_BattTemp)
                    {
                    case 0x20:
                        if ((sDcInfo.sALARMDcInfo.u8Sen1_BattTemp == 0x20 ) || (sDcInfo.sALARMDcInfo.u8Sen1_BattTemp == 0x05))
                        {
                            privateMibBase.batteryGroup.battTemp = 0;
                            if (sDcInfo.sALARMDcInfo.u8Sen1_BattTemp == 0x20)
                            {
                                privateMibBase.mainAlarmGroup.alarmBattHighTemp = 0x20;
                            }
                            else if (sDcInfo.sALARMDcInfo.u8Sen1_BattTemp == 0x05)
                            {
                                privateMibBase.mainAlarmGroup.alarmBattHighTemp = 0x05;
                            }
                        }
                        break;
                    case 0x00:
                        privateMibBase.batteryGroup.battTemp = (uint32_t)(sDcInfo.sINFODcInfo.fSen2BattTemp * 10);
                        privateMibBase.mainAlarmGroup.alarmBattHighTemp = 0x00;
                        break;
                    case 0x05:
                        if ((sDcInfo.sALARMDcInfo.u8Sen1_BattTemp == 0x20 ) || (sDcInfo.sALARMDcInfo.u8Sen1_BattTemp == 0x05))
                        {
                            privateMibBase.batteryGroup.battTemp = 0;
                            privateMibBase.mainAlarmGroup.alarmBattHighTemp = 0x05;
                        }
                        break;
                    case 0x02:
                        privateMibBase.batteryGroup.battTemp = (uint32_t)(sDcInfo.sINFODcInfo.fSen2BattTemp * 10);
                        privateMibBase.mainAlarmGroup.alarmBattHighTemp = 0x02;
                        break;
                    case 0x01:
                        privateMibBase.batteryGroup.battTemp = (uint32_t)(sDcInfo.sINFODcInfo.fSen2BattTemp * 10);
                        privateMibBase.mainAlarmGroup.alarmBattHighTemp = 0x01;
                        break;
                    }

                    switch(sDcInfo.sALARMDcInfo.u8Sen1_AmbTemp)
                    {
                    case 0x20:
                        if ((sDcInfo.sALARMDcInfo.u8Sen2_AmbTemp == 0x20 ) || (sDcInfo.sALARMDcInfo.u8Sen2_AmbTemp == 0x05))
                        {
                            privateMibBase.siteGroup.siteAmbientTemp = 0;
                        }
                        break;
                    case 0x00:
                        privateMibBase.siteGroup.siteAmbientTemp = (uint32_t)(sDcInfo.sINFODcInfo.fSen1AmbTemp * 10);
                        break;
                    case 0x05:
                        if ((sDcInfo.sALARMDcInfo.u8Sen2_AmbTemp == 0x20 ) || (sDcInfo.sALARMDcInfo.u8Sen2_AmbTemp == 0x05))
                        {
                            privateMibBase.siteGroup.siteAmbientTemp = 0;
                        }
                        break;
                    case 0x02:
                        privateMibBase.siteGroup.siteAmbientTemp = (uint32_t)(sDcInfo.sINFODcInfo.fSen1AmbTemp * 10);
                        break;
                    case 0x01:
                        privateMibBase.siteGroup.siteAmbientTemp = (uint32_t)(sDcInfo.sINFODcInfo.fSen1AmbTemp * 10);
                        break;
                    }

                    switch(sDcInfo.sALARMDcInfo.u8Sen2_AmbTemp)
                    {
                    case 0x20:
                        if ((sDcInfo.sALARMDcInfo.u8Sen1_AmbTemp == 0x20 ) || (sDcInfo.sALARMDcInfo.u8Sen1_AmbTemp == 0x05))
                        {
                            privateMibBase.siteGroup.siteAmbientTemp = 0;
                        }
                        break;
                    case 0x00:
                        privateMibBase.siteGroup.siteAmbientTemp = (uint32_t)(sDcInfo.sINFODcInfo.fSen2AmbTemp * 10);
                        break;
                    case 0x05:
                        if ((sDcInfo.sALARMDcInfo.u8Sen1_AmbTemp == 0x20 ) || (sDcInfo.sALARMDcInfo.u8Sen1_AmbTemp == 0x05))
                        {
                            privateMibBase.siteGroup.siteAmbientTemp = 0;
                        }
                        break;
                    case 0x02:
                        privateMibBase.siteGroup.siteAmbientTemp = (uint32_t)(sDcInfo.sINFODcInfo.fSen2AmbTemp * 10);
                        break;
                    case 0x01:
                        privateMibBase.siteGroup.siteAmbientTemp = (uint32_t)(sDcInfo.sINFODcInfo.fSen2AmbTemp * 10);
                        break;
                    }
                    // ======================== Update alarm information to SNMP resource ==========================//
                    // ======================== Update Config information to SNMP resource ==========================//
                    privateMibBase.configGroup.cfgFloatVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fFltVoltCfg * 100);
                    privateMibBase.configGroup.cfgBoostVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fBotVoltCfg * 100);
                    privateMibBase.configGroup.cfgTempCompValue = (uint32_t) (sBattInfo.sRAWBattInfo.fTempCompVal);
                    privateMibBase.configGroup.cfgLLVDVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg * 100);
                    privateMibBase.configGroup.cfgBLVDVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fLVDDV * 100);
                    privateMibBase.configGroup.cfgDCLowVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg * 100);
                    privateMibBase.configGroup.cfgDCUnderVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fDCUnderCfg * 100);
                    privateMibBase.configGroup.cfgDCOverVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fDCOverCfg * 100);                
                    privateMibBase.configGroup.cfgLLVDEn = sBattInfo.sRAWBattInfo.u8LLVDE;
                    privateMibBase.configGroup.cfgBLVDEn = sBattInfo.sRAWBattInfo.u8BLVDE;
                    

                    privateMibBase.batteryGroup.battBanksNumofBanks = sBattInfo.sRAWBattInfo.u8BankNo;
                    privateMibBase.configGroup.cfgBattCapacityTotal = (uint32_t) sBattInfo.sRAWBattInfo.fCapTotal;
                    privateMibBase.configGroup.cfgCurrentLimit = (uint32_t) (sBattInfo.sRAWBattInfo.fCCLVal * 1000);

                    privateMibBase.configGroup.cfgOverMajorTempLevel = (uint32_t)(sBattInfo.sRAWBattInfo.fOvMjTempAlrmLevel * 100);
                    privateMibBase.configGroup.cfgHighMajorTempLevel = (uint32_t) (sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel *100);

                    privateMibBase.configGroup.cfgWalkInTimeDuration = sRectInfo.u8WITI;
                    privateMibBase.configGroup.cfgWalkInTimeEn = sRectInfo.u8WITE;
                    privateMibBase.configGroup.cfgAcLowLevel = (uint32_t) sAcInfo.fAcLowThres;
                    privateMibBase.configGroup.cfgAcUnderLevel = (uint32_t) sAcInfo.fAcUnderThres;
                    privateMibBase.configGroup.cfgAcHighLevel = (uint32_t) sAcInfo.fAcHighThres;

                    // ======================== Update Config information to SNMP resource ==========================//
                    //======== Update Planned Test ============//
                    privateMibBase.cfgBTGroup.cfgBTPlanTestEn = sBattTestInfo.u8TimeTestEn;
                    privateMibBase.cfgBTGroup.cfgBTEndCap = (uint32_t) (sBattTestInfo.fTestEndCap * 1000);
                    privateMibBase.cfgBTGroup.cfgBTEndTime = (uint32_t) sBattTestInfo.fBattTestDur;
                    privateMibBase.cfgBTGroup.cfgBTEndVolt = (uint32_t) (sBattTestInfo.fBattTestVolt * 10);
                    for (i = 0; i < 4; i++)
                    {
                        snprintf(privateMibBase.cfgBTGroup.cfgBTPlanTestTable[i].cfgBTPlanTestString,14,"%02d/%02d - %02d:00 ",
                                 privateMibBase.cfgBTGroup.cfgBTPlanTestTable[i].cfgBTPlanTestMonth,
                                 privateMibBase.cfgBTGroup.cfgBTPlanTestTable[i].cfgBTPlanTestDate,
                                 privateMibBase.cfgBTGroup.cfgBTPlanTestTable[i].cfgBTPlanTestHour
                                );
                        privateMibBase.cfgBTGroup.cfgBTPlanTestTable[i].cfgBTPlanTestString[14] = '\0';
                    }
                    
                    snprintf(privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUString,20,"%04d-%02d-%02d@%02d:%02d:%02d",
                             privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUYear,
                             privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUMonth,
                             privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUDate,
                             privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUHour,
                             privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUMinute,
                             privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUSecond
                            );
                    privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUString[20] = '\0';


                    //======== Update Planned Test ============//

                    MESGState = SYSINFO_REQ;
                    break;
                case  TEST_MODE_REQ:

                    for (i = 0; i < GetMACAddrLen; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&GetMACAddr[i],1);
                    }

                    MESGState = TEST_MODE_RES;
                    break;

                case  TEST_MODE_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                    g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    if (g_checksum == g_testchecksum)
                    {
                        ui8User[0] = hex2byte(&RecvBuff[0],13);
                        ui8User[1] = hex2byte(&RecvBuff[0],15);
                        ui8User[2] = hex2byte(&RecvBuff[0],17);
                        ui8User[3] = hex2byte(&RecvBuff[0],19);
                        ui8User[4] = hex2byte(&RecvBuff[0],21);
                        ui8User[5] = hex2byte(&RecvBuff[0],23);

//                                eeprom_write_byte(MAC_ALREADY_WRITTEN,0x53);
//
//                                eeprom_write_byte(EEPROM_MACADDR1,ui8User[0]);
//                                eeprom_write_byte(EEPROM_MACADDR2,ui8User[1]);
//                                eeprom_write_byte(EEPROM_MACADDR3,ui8User[2]);
//                                eeprom_write_byte(EEPROM_MACADDR4,ui8User[3]);
//                                eeprom_write_byte(EEPROM_MACADDR5,ui8User[4]);
//                                eeprom_write_byte(EEPROM_MACADDR6,ui8User[5]);

                        sSiteInfo.u32RemoteReboot = 0x53;
                    }
                    else
                    {
                        MESGState = TEST_MODE_REQ;
//					test_mode = 1;
                    }

                    RecvCntStart = 0;

                    break;
                case  CALIB_MESG_1:    
                  {
                        u82char(&calibMesg_1[3], 0x01, 2);
                        u322char(&calibMesg_1[13], g_sParameters.u32StaticIP, 8);
                        u322char(&calibMesg_1[21], g_sParameters.u32SubnetMask, 8);
                        u322char(&calibMesg_1[29], g_sParameters.u32GatewayIP, 8);
                        u322char(&calibMesg_1[37], g_sParameters.u32SnmpIP, 8);
                        
                        u82char(&calibMesg_1[45], g_sWorkingDefaultParameters2.u8IsWritten, 2);
                        u82char(&calibMesg_1[47], g_sWorkingDefaultParameters2.u8UserMAC[0], 2);
                        u82char(&calibMesg_1[49], g_sWorkingDefaultParameters2.u8UserMAC[1], 2);
                        u82char(&calibMesg_1[51], g_sWorkingDefaultParameters2.u8UserMAC[2], 2);
                        u82char(&calibMesg_1[53], g_sWorkingDefaultParameters2.u8UserMAC[3], 2);
                        u82char(&calibMesg_1[55], g_sWorkingDefaultParameters2.u8UserMAC[4], 2);
                        u82char(&calibMesg_1[57], g_sWorkingDefaultParameters2.u8UserMAC[5], 2);
                        u322char(&calibMesg_1[59], g_sParameters2.u32FirmwareCRC, 8);
                        
                        u82char(&calibMesg_1[67], sTestRS485.Test485LoopOk, 2);
                        

//                        u82char(&calibMesg_1[45], eeprom_read_byte(MAC_ALREADY_WRITTEN), 2);
//                        u82char(&calibMesg_1[47], eeprom_read_byte(EEPROM_MACADDR1), 2);
//                        u82char(&calibMesg_1[49], eeprom_read_byte(EEPROM_MACADDR2), 2);
//                        u82char(&calibMesg_1[51], eeprom_read_byte(EEPROM_MACADDR3), 2);
//                        u82char(&calibMesg_1[53], eeprom_read_byte(EEPROM_MACADDR4), 2);
//                        u82char(&calibMesg_1[55], eeprom_read_byte(EEPROM_MACADDR5), 2);
//                        u82char(&calibMesg_1[57], eeprom_read_byte(EEPROM_MACADDR6), 2);
                        u162char(&calibMesg_1[69],Checksum16(&calibMesg_1[0],74),4);

                        for (i = 0; i < 74; i++)
                        {

                                 UART_WriteBlocking(RS2321_UART,&calibMesg_1[i],1);

                        }
//                        u8IsRebootFlag = 1;
                        MESGState = SYSINFO_REQ;
                  }

                break;
                case  CALIB_MESG_2:
                  {
                        u82char(&calibMesg_1[3], 0x02, 2);
                        u322char(&calibMesg_1[13], g_sParameters.u32StaticIP, 8);
                        u322char(&calibMesg_1[21], g_sParameters.u32SubnetMask, 8);
                        u322char(&calibMesg_1[29], g_sParameters.u32GatewayIP, 8);
                        u322char(&calibMesg_1[37], g_sParameters.u32SnmpIP, 8);
                        
                        u82char(&calibMesg_1[45], g_sWorkingDefaultParameters2.u8IsWritten, 2);
                        u82char(&calibMesg_1[47], g_sWorkingDefaultParameters2.u8UserMAC[0], 2);
                        u82char(&calibMesg_1[49], g_sWorkingDefaultParameters2.u8UserMAC[1], 2);
                        u82char(&calibMesg_1[51], g_sWorkingDefaultParameters2.u8UserMAC[2], 2);
                        u82char(&calibMesg_1[53], g_sWorkingDefaultParameters2.u8UserMAC[3], 2);
                        u82char(&calibMesg_1[55], g_sWorkingDefaultParameters2.u8UserMAC[4], 2);
                        u82char(&calibMesg_1[57], g_sWorkingDefaultParameters2.u8UserMAC[5], 2);
                        u322char(&calibMesg_1[59], g_sParameters2.u32FirmwareCRC, 8);
                        
                        u82char(&calibMesg_1[67], sTestRS485.Test485LoopOk, 2);
                        

//                        u82char(&calibMesg_1[45], eeprom_read_byte(MAC_ALREADY_WRITTEN), 2);
//                        u82char(&calibMesg_1[47], eeprom_read_byte(EEPROM_MACADDR1), 2);
//                        u82char(&calibMesg_1[49], eeprom_read_byte(EEPROM_MACADDR2), 2);
//                        u82char(&calibMesg_1[51], eeprom_read_byte(EEPROM_MACADDR3), 2);
//                        u82char(&calibMesg_1[53], eeprom_read_byte(EEPROM_MACADDR4), 2);
//                        u82char(&calibMesg_1[55], eeprom_read_byte(EEPROM_MACADDR5), 2);
//                        u82char(&calibMesg_1[57], eeprom_read_byte(EEPROM_MACADDR6), 2);
                        u162char(&calibMesg_1[69],Checksum16(&calibMesg_1[0],74),4);

                        for (i = 0; i < 74; i++)
                        {

                                 UART_WriteBlocking(RS2321_UART,&calibMesg_1[i],1);

                        }
                        u8IsRebootFlag = 1;
                        MESGState = SYSINFO_REQ;
                  }

                break;
                case  CALIB_MESG_3:
                  {
                        u82char(&calibMesg_1[3], 0x03, 2);
                        u322char(&calibMesg_1[13], g_sParameters.u32StaticIP, 8);
                        u322char(&calibMesg_1[21], g_sParameters.u32SubnetMask, 8);
                        u322char(&calibMesg_1[29], g_sParameters.u32GatewayIP, 8);
                        u322char(&calibMesg_1[37], g_sParameters.u32SnmpIP, 8);
                        
                        u82char(&calibMesg_1[45], g_sWorkingDefaultParameters2.u8IsWritten, 2);
                        u82char(&calibMesg_1[47], g_sWorkingDefaultParameters2.u8UserMAC[0], 2);
                        u82char(&calibMesg_1[49], g_sWorkingDefaultParameters2.u8UserMAC[1], 2);
                        u82char(&calibMesg_1[51], g_sWorkingDefaultParameters2.u8UserMAC[2], 2);
                        u82char(&calibMesg_1[53], g_sWorkingDefaultParameters2.u8UserMAC[3], 2);
                        u82char(&calibMesg_1[55], g_sWorkingDefaultParameters2.u8UserMAC[4], 2);
                        u82char(&calibMesg_1[57], g_sWorkingDefaultParameters2.u8UserMAC[5], 2);
                        u322char(&calibMesg_1[59], g_sParameters2.u32FirmwareCRC, 8);
                        
                        u82char(&calibMesg_1[67], sTestRS485.Test485LoopOk, 2);
                        

//                        u82char(&calibMesg_1[45], eeprom_read_byte(MAC_ALREADY_WRITTEN), 2);
//                        u82char(&calibMesg_1[47], eeprom_read_byte(EEPROM_MACADDR1), 2);
//                        u82char(&calibMesg_1[49], eeprom_read_byte(EEPROM_MACADDR2), 2);
//                        u82char(&calibMesg_1[51], eeprom_read_byte(EEPROM_MACADDR3), 2);
//                        u82char(&calibMesg_1[53], eeprom_read_byte(EEPROM_MACADDR4), 2);
//                        u82char(&calibMesg_1[55], eeprom_read_byte(EEPROM_MACADDR5), 2);
//                        u82char(&calibMesg_1[57], eeprom_read_byte(EEPROM_MACADDR6), 2);
                        u162char(&calibMesg_1[69],Checksum16(&calibMesg_1[0],74),4);

                        for (i = 0; i < 74; i++)
                        {

                                 UART_WriteBlocking(RS2321_UART,&calibMesg_1[i],1);

                        }
                        u8IsRebootFlag = 1;
                        MESGState = SYSINFO_REQ;
                  }

                break;
                case  CALIB_MESG_4:
                  {
                    if(sTestRS485.requesttestRS485 == 0)
                    {
                        u82char(&calibMesg_1[3], 0x04, 2);
                        u322char(&calibMesg_1[13], g_sParameters.u32StaticIP, 8);
                        u322char(&calibMesg_1[21], g_sParameters.u32SubnetMask, 8);
                        u322char(&calibMesg_1[29], g_sParameters.u32GatewayIP, 8);
                        u322char(&calibMesg_1[37], g_sParameters.u32SnmpIP, 8);
                        
                        u82char(&calibMesg_1[45], g_sWorkingDefaultParameters2.u8IsWritten, 2);
                        u82char(&calibMesg_1[47], g_sWorkingDefaultParameters2.u8UserMAC[0], 2);
                        u82char(&calibMesg_1[49], g_sWorkingDefaultParameters2.u8UserMAC[1], 2);
                        u82char(&calibMesg_1[51], g_sWorkingDefaultParameters2.u8UserMAC[2], 2);
                        u82char(&calibMesg_1[53], g_sWorkingDefaultParameters2.u8UserMAC[3], 2);
                        u82char(&calibMesg_1[55], g_sWorkingDefaultParameters2.u8UserMAC[4], 2);
                        u82char(&calibMesg_1[57], g_sWorkingDefaultParameters2.u8UserMAC[5], 2);
                        u322char(&calibMesg_1[59], g_sParameters2.u32FirmwareCRC, 8);
                        
                        u82char(&calibMesg_1[67], sTestRS485.Test485LoopOk, 2);
                        

//                        u82char(&calibMesg_1[45], eeprom_read_byte(MAC_ALREADY_WRITTEN), 2);
//                        u82char(&calibMesg_1[47], eeprom_read_byte(EEPROM_MACADDR1), 2);
//                        u82char(&calibMesg_1[49], eeprom_read_byte(EEPROM_MACADDR2), 2);
//                        u82char(&calibMesg_1[51], eeprom_read_byte(EEPROM_MACADDR3), 2);
//                        u82char(&calibMesg_1[53], eeprom_read_byte(EEPROM_MACADDR4), 2);
//                        u82char(&calibMesg_1[55], eeprom_read_byte(EEPROM_MACADDR5), 2);
//                        u82char(&calibMesg_1[57], eeprom_read_byte(EEPROM_MACADDR6), 2);
                        u162char(&calibMesg_1[69],Checksum16(&calibMesg_1[0],74),4);

                        for (i = 0; i < 74; i++)
                        {

                                 UART_WriteBlocking(RS2321_UART,&calibMesg_1[i],1);

                        }
//                        u8IsRebootFlag = 1;
                        MESGState = SYSINFO_REQ;
                    }
                  }
                break;
                default:
                    break;
                }

            }
        }


        vTaskDelay(200);
    }
    while (1);
//    vTaskSuspend(NULL);
}


void EMER_V21_Data_Process(void *pvParameters)
{
 
//  sFLASH_EraseSector(FLASH_PB_LOG_START);
//  sFLASH_EraseSector(FLASH_PB1_LOG_START);
//  sFLASH_EraseSector(FLASH_PB2_LOG_START);
//  sFLASH_EraseSector(FLASH_PB3_LOG_START);
//  sFLASH_EraseSector(FLASH_PB4_LOG_START);
    uint32_t i,j;
    uint32_t fuse_flag;
    uint32_t temp[2];
    MESGState = ACINPUTINFO_REQ;        

    do
    {
      EMERV21_cnt++;
      privateMibBase.cntGroup.EMERV21cnt=EMERV21_cnt;
      privateMibBase.cntGroup.EMERV21State= MESGState;
      
      
        if (sInterruptMesg.u8Checkbit == 1)
        {
            sInterruptMesg.u8Checkbit = 0;
        }
        else if (sInterruptMesg.u8Checkbit == 0)
        {

          
//====================================== HISTORY MESSAGE ======================================//                
                if(MESGState_v21 == HISTORYMESG_REQ)             
                {
                   GetHistoryLog_v21();
                   MESGState_v21 = HISTORYMESG_RES;
                }
                
//====================================== HISTORY MESSAGE ======================================// 
            if (setCmd_flag == 1)
            {
                settingCommand |= setCmd_mask;

                switch(settingCommand)
                {
                case SET_FLTVOL:
                    sBattInfo.sRAWBattInfo.fFltVoltCfg = (float)privateMibBase.configGroup.cfgFloatVolt/100;
//                    sBattInfo.sRAWBattInfo.fFltVoltCfg = a;
                    EMERV21_SetCommand(&g_setInfo[0],EMER_FLOATVOLT,sBattInfo.sRAWBattInfo.fFltVoltCfg,DC);    
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_BSTVOL:
                    sBattInfo.sRAWBattInfo.fBotVoltCfg = (float)privateMibBase.configGroup.cfgBoostVolt/100;
                    EMERV21_SetCommand(&g_setInfo[0],EMER_BOOSTVOLT,sBattInfo.sRAWBattInfo.fBotVoltCfg,DC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_TEMPCOMP_VAL:
                    sBattInfo.sRAWBattInfo.fTempCompVal = (float)privateMibBase.configGroup.cfgTempCompValue;
                    EMERV21_SetCommand(&g_setInfo[0],EMER_TEMP_COMPENSATION,sBattInfo.sRAWBattInfo.fTempCompVal,DC);
                    for (i = 0; i < 28; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_LLVD:
                    sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg = (float)privateMibBase.configGroup.cfgLLVDVolt/100;
                    EMERV21_SetCommand(&g_setInfo[0],EMER_LLVD,sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg,DC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_BLVD:
                    sBattInfo.sRAWBattInfo.fLVDDV = (float)privateMibBase.configGroup.cfgBLVDVolt/100;
                    EMERV21_SetCommand(&g_setInfo[0],EMER_BLVD,sBattInfo.sRAWBattInfo.fLVDDV,DC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                 case SET_DCOVER:
                    sBattInfo.sRAWBattInfo.fDCOverCfg = (float)privateMibBase.configGroup.cfgDCOverVolt/100;
                    EMERV21_SetCommand(&g_setInfo[0],EMER_DCVOLT_UPPERLIMIT,sBattInfo.sRAWBattInfo.fDCOverCfg,DC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_DCLOW:
                    sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg = (float)privateMibBase.configGroup.cfgDCLowVolt/100;
                    EMERV21_SetCommand(&g_setInfo[0],EMER_DCVOLT_LOWERLIMIT,sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg,DC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;

                case SET_BATTSTDCAP:
                    sBattInfo.sRAWBattInfo.fCapTotal = (float)privateMibBase.configGroup.cfgBattCapacityTotal;
                    EMERV21_SetCommand(&g_setInfo[0],EMER_BATT_CAP,sBattInfo.sRAWBattInfo.fCapTotal,DC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;

                case SET_CCL:
                    sBattInfo.sRAWBattInfo.fCCLVal = (float)privateMibBase.configGroup.cfgCurrentLimit/1000;
                    sBattInfo.sRAWBattInfo.fCapCCLVal  = sBattInfo.sRAWBattInfo.fCCLVal* sBattInfo.sRAWBattInfo.fCapTotal;
                    EMERV21_SetCommand(&g_setInfo[0],EMER_CHARGINGFACTOR,sBattInfo.sRAWBattInfo.fCapCCLVal,DC);

                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_OVERTEMP:
                    sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel = (float)privateMibBase.configGroup.cfgHighMajorTempLevel/100;
                    EMERV21_SetCommand(&g_setInfo[0],EMER_BATT_TEMP_OVER,sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel,DC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);

                    }
                    break;

//				case SET_WIT_VAL:
////					g_sParameters.ucReserved2[11] = privateMibBase.configGroup.cfgWalkInTimeDuration;
//					sRectInfo.u8WITI = (float)privateMibBase.configGroup.cfgWalkInTimeDuration;
//					setCommand(&g_setInfo[0],&cmdCodeTable[36][0], sRectInfo.u8WITI,0);
//					for (i = 0; i < 28; i++)
//					{
//
//                                                UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
//					}
//				break;

//				case SET_WIT_EN:
//					g_sParameters.ucReserved2[12] = privateMibBase.configGroup.cfgWalkInTimeEn;
//					sRectInfo.u8WITE = (float)privateMibBase.configGroup.cfgWalkInTimeEn;
//					setCommand(&g_setInfo[0],&cmdCodeTable[37][0], sRectInfo.u8WITE,1);
//					for (i = 0; i < 28; i++)
//					{
//
//                                                 UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
//					}
//				break;

                case SET_AC_THRES:
                    sAcInfo.fAcLowThres = (float)privateMibBase.configGroup.cfgAcLowLevel;
                    EMERV21_SetCommand(&g_setInfo[0],EMER_ACVOLT_LOWERLIMIT,sAcInfo.fAcLowThres,AC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_AC_HIGH_THRES:
                    sAcInfo.fAcHighThres = (float)privateMibBase.configGroup.cfgAcHighLevel;
                    EMERV21_SetCommand(&g_setInfo[0],EMER_ACVOLT_UPPERLIMIT,sAcInfo.fAcHighThres,AC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_AC_INPUT_CURR_LIMIT:           // ????????????????????
//					sAcInfo.fAcHighThres = (float)privateMibBase.configGroup.cfgAcLowLevel;
                    EMERV21_SetCommand(&g_setInfo[0],EMER_ACINPUTCUR_UPPERLIMIT,sAcInfo.fAcInputCurrLimit,AC);
                    for (i = 0; i < 28; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_TEST_VOLT:
                    sBattInfo.sRAWBattInfo.fTestVoltCfg = (float)privateMibBase.cfgBTGroup.cfgTestVoltCfg/10;
                    EMERV21_SetCommand(&g_setInfo[0],EMER_TESTVOLT,sBattInfo.sRAWBattInfo.fTestVoltCfg,DC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);

                    }
                    break;
                case SET_BT_ENDTIME:           
                    sBattTestInfo.fBattTestDur = (float) privateMibBase.cfgBTGroup.cfgBTEndTime;
                    EMERV21_SetCommand(&g_setInfo[0],EMER_TESTTERMINATION,sBattTestInfo.fBattTestDur,DC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);

                    }
                    break;
                 case SET_BT_ENDCAP:           
                    sBattTestInfo.fTestEndCap = (float) privateMibBase.cfgBTGroup.cfgBTEndCap/1000;
                    EMERV21_SetCommand(&g_setInfo[0],EMER_AFTERTESTCAP,sBattTestInfo.fTestEndCap,DC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);

                    }
                    break;

                default:
                    break;
                }
                setCmd_mask = 0;
                setCmd_flag = 0;
                settingCommand = 0;

            }
            else if (setCmd_flag == 0)
            {
                if(PMUConnectCount++>30)
                {
                  PMUConnectCount =50;
                  privateMibBase.connAlarmGroup.alarmPMUConnect = 1;
                }               

                switch(MESGState)
                {
//====================================== ACINFO MESSAGE ======================================//
                case  ACINPUTINFO_REQ:
//						thao++;
                    for (i = 0; i < GetACAnalogInfoLen; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&GetACAnalogInfo[i],1);

                    }
                    MESGState = ACINPUTINFO_RES;
                    break;
                case  ACINPUTINFO_RES:
                    for(i=0; i < RecvCntStart; i++)  
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;
                    if(RecvCount>6)
                    {
                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                        g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    }

                    if (g_checksum == g_testchecksum)
                    {
                      sDcInfo.u32DCNoResponse = 0;
                      privateMibBase.connAlarmGroup.alarmPMUConnect = 0;
                        sAcInfo.facVolt[0] = hex2float(&RecvBuff[0],17);
                        sAcInfo.facVolt[1] = hex2float(&RecvBuff[0],25);
                        sAcInfo.facVolt[2] = hex2float(&RecvBuff[0],33);
                    }
                    else
                    {
                      sDcInfo.u32DCNoResponse++;
                      if (sDcInfo.u32DCNoResponse > 3)
                      {
                        privateMibBase.connAlarmGroup.alarmPMUConnect = 1;
                        g_testchecksum=0xFFFF;g_checksum=0;
                        sDcInfo.u32DCNoResponse = 10;
                      // xoa khi checksum sai
                      sAcInfo.facVolt[0] = 0;
                      sAcInfo.facVolt[1] = 0;
                      sAcInfo.facVolt[2] = 0;
                      sAcInfo.fAcHighThres = 0;
                      sAcInfo.fAcLowThres = 0;
                      sAcInfo.fAcInputCurrLimit = 0;
                      sRectInfo.fAllRectDcVolt = 0;
                      for(i=0; i<24; i++)
                      {
                         sRectInfo.sRAWRectParam[i].fRect_DcVolt = 0;
                         sRectInfo.sRAWRectParam[i].fRect_DcCurr = 0;
                         sRectInfo.sRAWRectParam[i].fRect_Temp = 0;
                      }
                      sDcInfo.sINFODcInfo.fVoltage = 0;
                      sDcInfo.sINFODcInfo.fCurrent = 0;
                      sDcInfo.sINFODcInfo.fBatt1Curr = 0;
                      sDcInfo.sINFODcInfo.fBatt2Curr = 0;
                      sDcInfo.sINFODcInfo.fBatt1Volt = 0;
                      sDcInfo.sINFODcInfo.fBatt2Volt = 0;                     
                      sDcInfo.sINFODcInfo.fSen1BattTemp = 0;
                      sDcInfo.sINFODcInfo.fSen2BattTemp = 0;
                      sDcInfo.sINFODcInfo.fBatt1RmnCap = 0;
                      sDcInfo.sINFODcInfo.fBatt2RmnCap = 0;
                      sBattInfo.sRAWBattInfo.fDCOverCfg = 0;
                      sBattInfo.sRAWBattInfo.fFltVoltCfg = 0;
                      sBattInfo.sRAWBattInfo.fBotVoltCfg = 0;
                      sBattInfo.sRAWBattInfo.fTestVoltCfg = 0;
                      sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg = 0;
                      sBattInfo.sRAWBattInfo.fLVDDV = 0;
                      sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg = 0;
                      sBattInfo.sRAWBattInfo.fCapCCLVal= 0;
                      sBattInfo.sRAWBattInfo.fTempCompVal = 0;
                      sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel = 0;
                      sBattInfo.sRAWBattInfo.fOvMjTempAlrmLevel = 0;
                      sBattInfo.sRAWBattInfo.fCapTotal = 0;
                      sBattInfo.sRAWBattInfo.fCCLVal = 0;
                      sBattTestInfo.fBattTestDur = 0;
                      sBattTestInfo.fTestEndCap =  0;
                      sDcInfo.sINFODcInfo.u8BatNo = 0;
                      }
                    }


                    RecvCntStart = 0;
                    MESGState = ACPARAMETER_REQ;

                    break;
                case  ACPARAMETER_REQ:
                    for (i = 0; i < GetACParameterInfoLen; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&GetACParameterInfo[i],1);
                    }
                    MESGState = ACPARAMETER_RES;
                    break;
                case  ACPARAMETER_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;
                    if(RecvCount>6)
                    {
                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                        g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    }
                    if (g_checksum == g_testchecksum)
                    {
                        sAcInfo.fAcHighThres = hex2float(&RecvBuff[0],13);
                        sAcInfo.fAcLowThres = hex2float(&RecvBuff[0],21);
                        sAcInfo.fAcInputCurrLimit = hex2float(&RecvBuff[0],29);
                    }

                    RecvCntStart = 0;
                    MESGState = ACALARM_REQ;
                    break;
                case  ACALARM_REQ:
                    for (i = 0; i < GetACACAlarmInfoLen; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&GetACAlarmInfo[i],1);
                    }
                    MESGState = ACALARM_RES;
                    break;
                case  ACALARM_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    if(RecvCount>6)
                    {
                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                        g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    }
                    if (g_checksum == g_testchecksum)
                    {

                        sAcInfo.u8Thres[0] = hex2byte(&RecvBuff[0],17);
                        sAcInfo.u8Thres[1] = hex2byte(&RecvBuff[0],19);
                        sAcInfo.u8Thres[2] = hex2byte(&RecvBuff[0],21);

                        sAcInfo.u8_I_Thres[0] = hex2byte(&RecvBuff[0],57);
                        sAcInfo.u8_I_Thres[1] = hex2byte(&RecvBuff[0],59);
                        sAcInfo.u8_I_Thres[2] = hex2byte(&RecvBuff[0],61);
                        sAcInfo.u8MainFail = hex2byte(&RecvBuff[0],29);
                        sAcInfo.u8PowerOff = hex2byte(&RecvBuff[0],35);
                        sAcInfo.u8ACSPD  =  hex2byte(&RecvBuff[0],33);
                        // Chen lenh bao alarm ac tai day
                    }


                    RecvCntStart = 0;
                    MESGState = RECTSTSZTE_REQ;
                    break;
                //====================================== RECTINFO MESSAGE ======================================//
                case RECTSTSZTE_REQ:
                    for (i = 0; i < GetRectStateInfoLen; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&GetRectStateInfo[i],1);

                    }
                    MESGState = RECTSTSZTE_RES;
                    break;
                case RECTSTSZTE_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;
                    if(RecvCount>6)
                    {
                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                        g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    }
                    if (g_checksum == g_testchecksum)
                    {
                        RectNumber = 0;
                        for(i=0; i<5; i++)     
                        {
                            Rectlosedtest = sRectInfo.sRAWRectParam[i].u8Rect_Sts;
                            sRectInfo.sRAWRectParam[i].u8Rect_OnPosition = hex2byte(&RecvBuff[0],25+20*i); 
                            if((sRectInfo.sRAWRectParam[i].u8Rect_OnPosition==0xE0)|(sRectInfo.sRAWRectParam[i].u8Rect_OnPosition==0xE1))
                            {
                                sRectInfo.sRAWRectParam[i].u8Rect_Sts = hex2byte(&RecvBuff[0],17+20*i); 

                                RectNumber++;
                            }
                            else
                                sRectInfo.sRAWRectParam[i].u8Rect_Sts =1;

//                            if((Rectlosedtest ==1)&&(sRectInfo.sRAWRectParam[i].u8Rect_Sts==0))
//                            {
//                                Rectlosedtest =0;
//                                sRectInfo.sRAWRectParam[i].u8Rect_NoResp =1;
//                            }
//                            if((sRectInfo.sRAWRectParam[i].u8Rect_NoResp ==1)&&(sRectInfo.sRAWRectParam[i].u8Rect_Sts==1))
//                                sRectInfo.sRAWRectParam[i].u8Rect_NoResp =0;
                        }
                        sRectInfo.u8Rect_Num =RectNumber;
                    }

                    RecvCntStart = 0;
                    MESGState = RECTINFOZTE_REQ;
                    break;
                case  RECTINFOZTE_REQ:
                    for (i = 0; i < GetRectAnalogInfoLen; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&GetRectAnalogInfo[i],1);
                    }
                    MESGState = RECTINFOZTE_RES;
                    break;
                case  RECTINFOZTE_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    if(RecvCount>6)
                    {
                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                        g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    }
                    if (g_checksum == g_testchecksum)
                    {
                        sRectInfo.fAllRectDcVolt = hex2float(&RecvBuff[0],15);
                        val =     hex2byte(&RecvBuff[0],23);
                        for(i=0; i<val; i++)
                        {
//                          sRectInfo.sRAWRectParam[i].u8Rect_Sts=1;
                           sRectInfo.sRAWRectParam[i].fRect_DcVolt = hex2float(&RecvBuff[0],43+i*66);
                           sRectInfo.sRAWRectParam[i].fRect_DcCurr = hex2float(&RecvBuff[0],25+i*66);
                           sRectInfo.sRAWRectParam[i].fRect_Temp = hex2float(&RecvBuff[0],59+i*66);

                        }
                    }


                    RecvCntStart = 0;
                    MESGState = RECTALARM_REQ;
                    break;
                case  RECTALARM_REQ:
                    for (i = 0; i < GetRectAlarmInfoLen; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&GetRectAlarmInfo[i],1);
                    }
                    MESGState = RECTALARM_RES;
                    break;
                case  RECTALARM_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    if(RecvCount>6)
                    {
                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                        g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    }
                    if (g_checksum == g_testchecksum)
                    {
                      val =        hex2byte(&RecvBuff[0],15);
                        for(i=0; i<val; i++)
                        {
//                            sRectInfo.sRAWRectParam[i].u8Rect_Sts=1;
                            sRectInfo.sRAWRectParam[i].u8Rect_Fail = hex2byte(&RecvBuff[0],i*20+17); // 00H normal, 01H Fail
                            sRectInfo.sRAWRectParam[i].u8Rect_Out = hex2byte(&RecvBuff[0],i*20+35); // 00H normal, 01H Fail
                            sRectInfo.sRAWRectParam[i].u8Rect_NoResp = hex2byte(&RecvBuff[0],i*20+27);
                            if(sRectInfo.sRAWRectParam[i].u8Rect_NoResp == 0) sRectInfo.sRAWRectParam[i].u8Rect_NoResp = 0;
                            else sRectInfo.sRAWRectParam[i].u8Rect_NoResp =1;
                          
                        }
                    }


                    RecvCntStart = 0;
                    MESGState = DCOUTPUT_REQ;
                    break;
                //====================================== DCINFO MESSAGE ======================================//

                case  DCOUTPUT_REQ:
                    for (i = 0; i < GetDCAnalogInfoLen; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&GetDCAnalogInfo[i],1);
                    }
                    MESGState = DCOUTPUT_RES;
                    break;
                case  DCOUTPUT_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    if(RecvCount>6)
                    {
                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                        g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    }
                    if (g_checksum == g_testchecksum)
                    {
                        sDcInfo.sINFODcInfo.fVoltage = hex2float(&RecvBuff[0],17);
                        sDcInfo.sINFODcInfo.fCurrent = hex2float(&RecvBuff[0],25);
//                        sDcInfo.sINFODcInfo.u8BatNo = hex2byte(&RecvBuff[0],33);
                        sDcInfo.sINFODcInfo.fBatt1Curr = hex2float(&RecvBuff[0],35);
                        sDcInfo.sINFODcInfo.fBatt2Curr = hex2float(&RecvBuff[0],43);
                        sDcInfo.sINFODcInfo.fBatt1Volt = hex2float(&RecvBuff[0],55);
                        sDcInfo.sINFODcInfo.fBatt2Volt = hex2float(&RecvBuff[0],63);                     
                        sDcInfo.sINFODcInfo.fSen1BattTemp = hex2float(&RecvBuff[0],87);
                        sDcInfo.sINFODcInfo.fSen2BattTemp = hex2float(&RecvBuff[0],95);
                        sDcInfo.sINFODcInfo.fBatt1RmnCap = hex2float(&RecvBuff[0],71);
                        sDcInfo.sINFODcInfo.fBatt2RmnCap = hex2float(&RecvBuff[0],79);
                    }

                    RecvCntStart = 0;
                    MESGState = DCPARAMETER_REQ;
                    break;

                case  DCPARAMETER_REQ:
                    for (i = 0; i < GetDCParameterInfoLen; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&GetDCParameterInfo[i],1);
//                        athao[i]=REQdcparameter[i];
                    }
                    MESGState = DCPARAMETER_RES;
                    break;
                case  DCPARAMETER_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    if(RecvCount>6)
                    {
                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                        g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    }
                    if (g_checksum == g_testchecksum)
                    {
                        sBattInfo.sRAWBattInfo.fDCOverCfg = roundf(hex2float(&RecvBuff[0],13)*10)/10;
                        sBattInfo.sRAWBattInfo.fFltVoltCfg = roundf(hex2float(&RecvBuff[0],127)*10)/10;
                        sBattInfo.sRAWBattInfo.fBotVoltCfg = roundf(hex2float(&RecvBuff[0],135)*10)/10;
                        sBattInfo.sRAWBattInfo.fTestVoltCfg = roundf(hex2float(&RecvBuff[0],191)*10)/10;
                        sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg = roundf(hex2float(&RecvBuff[0],21)*10)/10;
                        sBattInfo.sRAWBattInfo.fLVDDV = roundf(hex2float(&RecvBuff[0],183)*10)/10;
                        sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg = roundf(hex2float(&RecvBuff[0],143)*10)/10;
                        sBattInfo.sRAWBattInfo.fCapCCLVal= roundf(hex2float(&RecvBuff[0],159)*1000)/1000;
                        sBattInfo.sRAWBattInfo.fTempCompVal = hex2float(&RecvBuff[0],175);
                        sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel = roundf(hex2float(&RecvBuff[0],79)*10)/10;
                        sBattInfo.sRAWBattInfo.fOvMjTempAlrmLevel = roundf(hex2float(&RecvBuff[0],87)*10)/10;
                        sBattInfo.sRAWBattInfo.fCapTotal = hex2float(&RecvBuff[0],151);
                        sBattInfo.sRAWBattInfo.fCCLVal = sBattInfo.sRAWBattInfo.fCapCCLVal/ sBattInfo.sRAWBattInfo.fCapTotal;
                        sBattTestInfo.fBattTestDur = hex2float(&RecvBuff[0],199);
                        sBattTestInfo.fTestEndCap =  hex2float(&RecvBuff[0],207);
                        sDcInfo.sINFODcInfo.u8BatNo = hex2float(&RecvBuff[0],215);
                    }

                    RecvCntStart = 0;
                    MESGState = DCALARM_REQ;
//                        MESGState = DCPARAMETER_REQ;
                    break;
                case  DCALARM_REQ:
                    for (i = 0; i < GetDCACAlarmInfoLen; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&GetDCAlarmInfo[i],1);
                    }
                    MESGState = DCALARM_RES;
                    break;
                case  DCALARM_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    if(RecvCount>6)
                    {
                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                        g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    }
                    if (g_checksum == g_testchecksum)
                    {
                        sDcInfo.sALARMDcInfo.u8DC = hex2byte(&RecvBuff[0],17);
                        sDcInfo.sALARMDcInfo.u8DC_NoFuse = hex2byte(&RecvBuff[0],19); // So switch trong tu
                        val = sDcInfo.sALARMDcInfo.u8DC_NoFuse;
                        for (i = 0; i < val; i++)
                        {
                            sDcInfo.sALARMDcInfo.u8LoadFuse[i] = hex2byte(&RecvBuff[0],9 + 2*i+12);
                            if((i<10)&&(sDcInfo.sALARMDcInfo.u8LoadFuse[i] == 0x03))privateMibBase.loadGroup.loadStatus[i]=1;
                        }
                        for (i = 0; i < 4; i++)
                        {
                            sDcInfo.sALARMDcInfo.u8BattFuse[i] = hex2byte(&RecvBuff[0],43+i*2);
                            if(sDcInfo.sALARMDcInfo.u8BattFuse[i] == 0x03)
                                privateMibBase.batteryGroup.battBrkStatus[i] = 1;
                        }
                        
                        
                        sDcInfo.sALARMDcInfo.u8LLVD = hex2byte(&RecvBuff[0],59);
                        sDcInfo.sALARMDcInfo.u8BLVD = hex2byte(&RecvBuff[0],61);
                        sDcInfo.sALARMDcInfo.u8Sen1_BattTemp = hex2byte(&RecvBuff[0],63);  // Canh bao nhiet do acquy1 cao
//                        sDcInfo.sALARMDcInfo.u8BatVol[0] = hex2byte(&RecvBuff[0],(val+8)*2+13);   // Canh bao bat 1 low voltage
//                        sDcInfo.sALARMDcInfo.u8Sen1_BattInvalid = hex2byte(&RecvBuff[0],(val+11)*2+13); // Canh bao batt temp invalid
//                        sDcInfo.sALARMDcInfo.u8Sen2_BattTemp = hex2byte(&RecvBuff[0],(val+14)*2+13); // Canh bao nhiet do acquy2 cao
//                        sDcInfo.sALARMDcInfo.u8BatVol[1] = hex2byte(&RecvBuff[0],(val+13)*2+13);   // Canh bao bat 2 low voltage
//                        sDcInfo.sALARMDcInfo.u8Sen2_BattInvalid = hex2byte(&RecvBuff[0],(val+16)*2+13); // Canh bao batt temp invalid
//                        sDcInfo.sALARMDcInfo.u8Sen3_BattTemp = hex2byte(&RecvBuff[0],(val+19)*2+13); // Canh bao nhiet do acquy3 cao
//                        sDcInfo.sALARMDcInfo.u8Sen3_BattInvalid = hex2byte(&RecvBuff[0],(val+21)*2+13); // Canh bao batt temp invalid
                        for (i = 0; i < 8; i++)
                        {
                            sDcInfo.sALARMDcInfo.u8DIFuse[i] = hex2byte(&RecvBuff[0],89+i*2);
                        }
                        sDcInfo.sALARMDcInfo.u8DCSPD = hex2byte(&RecvBuff[0],215);
//                                             
//                        sDcInfo.sALARMDcInfo.u8Batt_Discharge = hex2byte(&RecvBuff[0],(val+26)*2+13);								

                    }


                    RecvCntStart = 0;
                    MESGState = TIMEINFO_REQ;                          
                    break;
                case  TIMEINFO_REQ:
                    for (i = 0; i < GetEMERTimeInfoLen; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&GetEMERTimeInfo[i],1);
                    }
                    MESGState = TIMEINFO_RES;
                    break;
                case  TIMEINFO_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    if(RecvCount>6)
                    {
                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-5);
                        g_checksum = Checksum16(&RecvBuff[0],RecvCount);
                    }
                    if (g_checksum == g_testchecksum)
                    {
                       // nhan thong so thoi gian
                      if(( RecvBuff[7]=0x34)&&( RecvBuff[8]=0x44))      // ko sua
                      {
                         EMERV21Time.Year= hex2byte2(&RecvBuff[0],13);
                         EMERV21Time.Month = hex2byte(&RecvBuff[0],17);
                         EMERV21Time.Date = hex2byte(&RecvBuff[0],19);
                         EMERV21Time.Hour = hex2byte(&RecvBuff[0],21);
                         EMERV21Time.Minute = hex2byte(&RecvBuff[0],23);
                         EMERV21Time.Second = hex2byte(&RecvBuff[0],25);
                      }               
                    }

                    RecvCntStart = 0;
                    MESGState = UPDATE_OK;
                    break;
                case UPDATE_OK:
                    // ======================== Update dc information to SNMP resource ==========================//
                    privateMibBase.batteryGroup.battVolt = (int32_t) (sDcInfo.sINFODcInfo.fBatt1Volt * 100);
                    privateMibBase.batteryGroup.battCurr = (int32_t) ((sDcInfo.sINFODcInfo.fBatt1Curr + sDcInfo.sINFODcInfo.fBatt2Curr) * 100);
                    privateMibBase.batteryGroup.battCapLeft1 = (uint32_t) (sDcInfo.sINFODcInfo.fBatt1RmnCap * 100);
                    privateMibBase.batteryGroup.battTemp = (uint32_t) (sDcInfo.sINFODcInfo.fSen1BattTemp*10);
                    
                    if(sDcInfo.sALARMDcInfo.u8Sen1_BattInvalid==0xF5)         
                    {
//			privateMibBase.batteryGroup.battTemp = sDcInfo.sINFODcInfo.fSen2BattTemp*10;
                        privateMibBase.batteryGroup.battTemp =0;
                    }

                    privateMibBase.batteryGroup.battBanksNumofBanks = sDcInfo.sINFODcInfo.u8BatNo;

                    privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen = 0;
                    for (i = 0; i < 10; i++)
                    {
                        if(sDcInfo.sALARMDcInfo.u8LoadFuse[i] == 0x03)
                        {
                            fuse_flag = 1;
                            privateMibBase.loadGroup.loadStatus[i] = 1;
                        }
                        else
                        {
                            fuse_flag = 0;
                            privateMibBase.loadGroup.loadStatus[i] = 0;
                        }
                        privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen |= (fuse_flag << i);
                    }
                    if (privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen != 0)
                    {
                        privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen = 1;
                    }
                    else
                    {
                        privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen = 0;
                    }

                    privateMibBase.mainAlarmGroup.alarmBattBreakerOpen = 0;
                    for (i = 0; i < 4; i++)
                    {
                        if(sDcInfo.sALARMDcInfo.u8BattFuse[i] == 0x03)
                        {
                            fuse_flag = 1;
                            privateMibBase.batteryGroup.battBrkStatus[i] = 1;
                        }
                        else
                        {
                            fuse_flag = 0;
                            privateMibBase.batteryGroup.battBrkStatus[i] = 0;
                        }
                        privateMibBase.mainAlarmGroup.alarmBattBreakerOpen |= (fuse_flag << i);
                    }
                    for (i = 0; i < 8; i++)
                    {
                   
                        if(sDcInfo.sALARMDcInfo.u8DIFuse[i] == 0xEA)
                        {
                            fuse_flag = 1;
                            privateMibBase.diAlarmGroup.alarmDigitalInput[i] = 1;
                        }
                        else
                        {
                            fuse_flag = 0;
                            privateMibBase.diAlarmGroup.alarmDigitalInput[i] = 0;
                        }
                    }

                    if (privateMibBase.mainAlarmGroup.alarmBattBreakerOpen != 0)
                    {
                        privateMibBase.mainAlarmGroup.alarmBattBreakerOpen = 1;
                    }
                    else
                    {
                        privateMibBase.mainAlarmGroup.alarmBattBreakerOpen = 0;
                    }
                    if((sAcInfo.u8MainFail==0x05)||(sAcInfo.u8PowerOff==0xE1))
                    {
                        privateMibBase.mainAlarmGroup.alarmACmains =1;
                    }
                    else privateMibBase.mainAlarmGroup.alarmACmains =0;
                    if(sAcInfo.u8ACSPD==0x05)
                    {
                        privateMibBase.mainAlarmGroup.alarmACSPD =1;
                    }
                    else privateMibBase.mainAlarmGroup.alarmACSPD =0;
                    if(sDcInfo.sALARMDcInfo.u8DCSPD==0x05)
                    {
                        privateMibBase.mainAlarmGroup.alarmDCSPD =1;
                    }
                    else privateMibBase.mainAlarmGroup.alarmDCSPD =0;
                    if(sDcInfo.sALARMDcInfo.u8LLVD==0x00)
                        privateMibBase.mainAlarmGroup.alarmLLVD = 0;
                    else privateMibBase.mainAlarmGroup.alarmLLVD = 1;
//						privateMibBase.mainAlarmGroup.alarmLLVD = sDcInfo.sALARMDcInfo.u8LLVD;
                    if(sDcInfo.sALARMDcInfo.u8BLVD == 0x00)privateMibBase.mainAlarmGroup.alarmBLVD = 0;
                    else privateMibBase.mainAlarmGroup.alarmBLVD = 1;
//						privateMibBase.mainAlarmGroup.alarmBLVD = sDcInfo.sALARMDcInfo.u8BLVD;
                    if(sDcInfo.sALARMDcInfo.u8DC==1)
                        privateMibBase.mainAlarmGroup.alarmDCLow = 0x01;
                    else privateMibBase.mainAlarmGroup.alarmDCLow = 0x00;
                    privateMibBase.mainAlarmGroup.alarmBattHighTemp = 0;
                    if(sDcInfo.sALARMDcInfo.u8Sen1_BattTemp == 0x02)
                    {
                        privateMibBase.mainAlarmGroup.alarmBattHighTemp = 0x02;
                        privateMibBase.batteryGroup.battTemp = (uint32_t) (sDcInfo.sINFODcInfo.fSen1BattTemp * 10);
                    }
                 

                    // ======================== Update dc information to SNMP resource ==========================//
                    // ======================== Update rect information to SNMP resource ==========================//
                    privateMibBase.mainAlarmGroup.alarmRectACFault = 0;
                    privateMibBase.mainAlarmGroup.alarmRectNoResp = 0;
                    
//                    sRectInfo.sSNMPRectParam[0].u32Rect_State = 0;
                    for(i=0; i<6; i++)  
                    {
                        privateMibBase.rectGroup.rectTable[i].rectStatus = 0;
                        privateMibBase.mainAlarmGroup.alarmRectNoResp |= sRectInfo.sRAWRectParam[i].u8Rect_NoResp << i;    
                        privateMibBase.mainAlarmGroup.alarmRectACFault |= sRectInfo.sRAWRectParam[i].u8Rect_Fail << i;    
//							sRectInfo.sSNMPRectParam[i].u32Rect_Fail = sRectInfo.sRAWRectParam[i].u8Rect_Fail;
//                        sRectInfo.sSNMPRectParam[i].u32Rect_Out = sRectInfo.sRAWRectParam[i].u8Rect_Out;
                        if(sRectInfo.sRAWRectParam[i].u8Rect_Sts==0)
                        {
                            privateMibBase.rectGroup.rectTable[i].rectStatus = 0xFF; // Rect dang hoat dong
                        }
                        if(sRectInfo.sRAWRectParam[i].u8Rect_NoResp==1)
                        {
                            privateMibBase.rectGroup.rectTable[i].rectStatus = 0xAA; // Rect no respond
                        }
                        if(sRectInfo.sRAWRectParam[i].u8Rect_Out==1)
                        {
                            privateMibBase.rectGroup.rectTable[i].rectStatus = 0x0A; // Rect dc off
                        }
                        if(sRectInfo.sRAWRectParam[i].u8Rect_Fail==1)
                        {
                            privateMibBase.rectGroup.rectTable[i].rectStatus = 0xA5; // Rect fail
                        }

                        privateMibBase.rectGroup.rectTable[i].rectOutputCurrent = (uint32_t) (sRectInfo.sRAWRectParam[i].fRect_DcCurr * 100);
//							sRectInfo.sSNMPRectParam[i].u32Rect_Temp = sRectInfo.sRAWRectParam[i].fRect_Temp * 100;
//							if(privateMibBase.rectGroup.rectTable[i].rectOutputCurrent >0)
                        if(sRectInfo.sRAWRectParam[i].u8Rect_Sts==0)
                        {
//                           privateMibBase.rectGroup.rectTable[i].rectOutputVoltage =(uint32_t) (sRectInfo.fAllRectDcVolt * 100);
                           privateMibBase.rectGroup.rectTable[i].rectOutputVoltage =(uint32_t) (sRectInfo.sRAWRectParam[i].fRect_DcVolt * 100);
                           privateMibBase.rectGroup.rectTable[i].rectTemp =(uint32_t) (sRectInfo.sRAWRectParam[i].fRect_Temp * 100);
                        }
                           
                        else
                        {
                          privateMibBase.rectGroup.rectTable[i].rectOutputVoltage =0;
                         privateMibBase.rectGroup.rectTable[i].rectTemp =0;
                        }
//
                    }
                    if (privateMibBase.mainAlarmGroup.alarmRectNoResp != 0) privateMibBase.mainAlarmGroup.alarmRectNoResp = 1;
                    else privateMibBase.mainAlarmGroup.alarmRectNoResp = 0;
                    if (privateMibBase.mainAlarmGroup.alarmRectACFault != 0) privateMibBase.mainAlarmGroup.alarmRectACFault = 1;
                    else privateMibBase.mainAlarmGroup.alarmRectACFault = 0;
                    privateMibBase.rectGroup.rectTotalCurrent = 0;
                    for (i = 0; i < 6; i++)     
                    {
                        privateMibBase.rectGroup.rectTotalCurrent += (uint32_t) (privateMibBase.rectGroup.rectTable[i].rectOutputCurrent);
                    }
                    privateMibBase.rectGroup.rectInstalledRect = 5;    
                    privateMibBase.rectGroup.rectActiveRect = sRectInfo.u8Rect_Num; 
                    
                    privateMibBase.loadGroup.loadCurrent = (uint32_t) (sDcInfo.sINFODcInfo.fCurrent*100);
                    // ======================== Update rect information to SNMP resource ==========================//
                    // ======================== Update ac information to SNMP resource ==========================//
                    privateMibBase.acPhaseGroup.acPhaseTable[0].acPhaseVolt = (int32_t) (sAcInfo.facVolt[0] * 100);
                    privateMibBase.acPhaseGroup.acPhaseTable[1].acPhaseVolt = (int32_t) (sAcInfo.facVolt[1] * 100);
                    privateMibBase.acPhaseGroup.acPhaseTable[2].acPhaseVolt = (int32_t) (sAcInfo.facVolt[2] * 100);
                    if (sAcInfo.u8Thres[0] || sAcInfo.u8Thres[1] || sAcInfo.u8Thres[2])
                    {
                        privateMibBase.mainAlarmGroup.alarmACLow = 1;
                    }
                    else
                    {
                        privateMibBase.mainAlarmGroup.alarmACLow = 0;
                    }

                    // ======================== Update ac information to SNMP resource ==========================//

                    // ======================== Update Config information to SNMP resource ==========================//
                    privateMibBase.configGroup.cfgFloatVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fFltVoltCfg * 100);
                    privateMibBase.configGroup.cfgBoostVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fBotVoltCfg * 100);
                    privateMibBase.configGroup.cfgTempCompValue = (uint32_t) sBattInfo.sRAWBattInfo.fTempCompVal;
                    privateMibBase.configGroup.cfgLLVDVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg * 100);
                    privateMibBase.configGroup.cfgBLVDVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fLVDDV * 100);
                    privateMibBase.configGroup.cfgDCLowVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg * 100);
                    privateMibBase.configGroup.cfgBattTestVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fTestVoltCfg * 100);
                    
                    privateMibBase.cfgBTGroup.cfgBTEndTime = (uint32_t) (sBattTestInfo.fBattTestDur);
                    privateMibBase.cfgBTGroup.cfgBTEndCap = (uint32_t) (sBattTestInfo.fTestEndCap * 1000);
                    privateMibBase.cfgBTGroup.cfgTestVoltCfg = (uint32_t)(sBattInfo.sRAWBattInfo.fTestVoltCfg*10);
//                    privateMibBase.cfgBTGroup.cfgAutoTestDay = sBattInfo.sRAWBattInfo.fAutoTestDay;
//                    privateMibBase.cfgBTGroup.cfgTestStartTime = sBattInfo.sRAWBattInfo.fTestStartTime;
                    privateMibBase.configGroup.cfgBattCapacityTotal = (uint32_t) sBattInfo.sRAWBattInfo.fCapTotal;
                    privateMibBase.configGroup.cfgBattCapacityTotal2 = (uint32_t) sBattInfo.sRAWBattInfo.fCapTotal2;
                    privateMibBase.configGroup.cfgCurrentLimit = (uint32_t) ( sBattInfo.sRAWBattInfo.fCCLVal * 1000);
                    privateMibBase.configGroup.cfgDCOverVolt = (uint32_t)(sBattInfo.sRAWBattInfo.fDCOverCfg * 100);                    

//                    sBattInfo.sRAWBattInfo.fOvMjTempAlrmLevel = 100;
                    privateMibBase.configGroup.cfgOverMajorTempLevel = (uint32_t) (sBattInfo.sRAWBattInfo.fOvMjTempAlrmLevel * 100);
                    privateMibBase.configGroup.cfgHighMajorTempLevel = (uint32_t) (sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel *100);
                    privateMibBase.configGroup.cfgAcLowLevel = (uint32_t) sAcInfo.fAcLowThres;
                    privateMibBase.configGroup.cfgAcHighLevel = (uint32_t) sAcInfo.fAcHighThres;

                    // ======================== Update Config information to SNMP resource ==========================//
                    // ======================== Update Env information to SNMP resource ==========================//
                    privateMibBase.siteGroup.siteAmbientTemp = (uint32_t) (sDcInfo.sINFODcInfo.fSen2BattTemp*10);
                    sAlarmInfo.u32AmbTemp = 0;
                    if(sAlarmEnvInfo.u8EnvTemp == 0x04)
                    {
                        sAlarmInfo.u32AmbTemp = 0x02;
                    }
                    if((sAlarmEnvInfo.u8EnvTemp == 0x05)||(sAlarmEnvInfo.u8EnvTemp == 0xF5))
                    {
                        sAlarmInfo.u32AmbTemp = 0x05;
                    }
                    privateMibBase.mainAlarmGroup.alarmSmoke = 0;
                    if(sAlarmEnvInfo.u8EnvSmoke==0x04)
                    {
                         privateMibBase.mainAlarmGroup.alarmSmoke = 0x01;
                    }
                    sAlarmInfo.u32EnvDoor = 0;
                    if(sAlarmEnvInfo.u8EnvDoor==0x04)
                    {
                        sAlarmInfo.u32EnvDoor = 0x01;
                    }
                    sAlarmInfo.u32HeatExch = 0;
                    if(sAlarmEnvInfo.u8HeatExch==0x04)
                    {
                        sAlarmInfo.u32HeatExch = 0x01;
                    }
                 
                    
                    // ======================== Update alarm hitory ==========================//                    
//                    if((t1 ==1)&&(t2==0))
//                     {
//                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =9;
//                          NextHis();
//                          for(k=0;k<10;k++){NextHis();}
//
//
//     
//                     }
//                    if((t1 ==0)&&(t2==1))
//                    {     
//                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =10;
//                          NextHis();
//                          for(k=0;k<20;k++){NextHis();}
//                    }
//                     if((t3 ==1)&&(t4==0))
//                     {
//                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =11;
//                          NextHis();
//                          for(k=0;k<170;k++){NextHis();}
//                     }
//                     if((t3 ==0)&&(t4==1))
//                     { 
//                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =12;
//                          NextHis();
// 
//                     }
//                     
//                     if((t5 ==1)&&(t6==0))
//                     {    
//                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =13;
//                          NextHis();
//                     }  
//                     if((t5==0)&&(t6==1))
//                     {
//                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =14;
//                          NextHis();
//
//                     }
//                     if((t7 ==1)&&(t8==0))
//                     {    
//                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =15;
//                          NextHis();
//
//                     }  
//                     if((t7==0)&&(t8==1))
//                     {
//                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =16;
//                          NextHis();
//
//                     }
//                    
//                    t2=t1;
//                    t4=t3;
//                    t6=t5;
//                    t8=t7;
                     
                     if((privateMibBase.mainAlarmGroup.alarmACmains ==1)&&(privateMibBase.mainAlarmGroup.alarmACmains_old==0))
                     {
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =1;
                          NextHis();
     
                     }
                    if((privateMibBase.mainAlarmGroup.alarmACmains ==0)&&(privateMibBase.mainAlarmGroup.alarmACmains_old==1))
                    {     
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =2;
                          NextHis();
                    }
                     if((privateMibBase.mainAlarmGroup.alarmACLow ==1)&&(privateMibBase.mainAlarmGroup.alarmACLow_old==0))
                     {
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =3;
                          NextHis();

                     }
                     if((privateMibBase.mainAlarmGroup.alarmACLow ==0)&&(privateMibBase.mainAlarmGroup.alarmACLow_old==1))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =4;
                          NextHis();

                     }
                     
                     if((privateMibBase.mainAlarmGroup.alarmACSPD ==1)&&(privateMibBase.mainAlarmGroup.alarmACSPD_old==0))
                     {    
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =5;
                          NextHis();

                     }  
                     if((privateMibBase.mainAlarmGroup.alarmACSPD==0)&&(privateMibBase.mainAlarmGroup.alarmACSPD_old==1))
                     {
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =6;
                          NextHis();

                     }
                     if((privateMibBase.mainAlarmGroup.alarmDCLow ==1)&&(privateMibBase.mainAlarmGroup.alarmDCLow_old==0))
                     {    
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =7;
                          NextHis();

                     }  
                     if((privateMibBase.mainAlarmGroup.alarmDCLow==0)&&(privateMibBase.mainAlarmGroup.alarmDCLow_old==1))
                     {
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =8;
                          NextHis();

                     }
                     
                     if((privateMibBase.mainAlarmGroup.alarmDCSPD ==1)&&(privateMibBase.mainAlarmGroup.alarmDCSPD_old==0))
                     {    
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =9;
                          NextHis();
 
                     } 
                     if((privateMibBase.mainAlarmGroup.alarmDCSPD==0)&&(privateMibBase.mainAlarmGroup.alarmDCSPD_old==1))
                     {
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =10;
                          NextHis();

                     }
                     
                     if((privateMibBase.mainAlarmGroup.alarmLLVD ==1)&&(privateMibBase.mainAlarmGroup.alarmLLVD_old==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =11;
                          NextHis();

                     }
                    if((privateMibBase.mainAlarmGroup.alarmLLVD==0)&&(privateMibBase.mainAlarmGroup.alarmLLVD_old==1))
                    {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =12;
                          NextHis();

                    }
                    
                     if((privateMibBase.mainAlarmGroup.alarmBLVD ==1)&&(privateMibBase.mainAlarmGroup.alarmBLVD_old==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =13;
                          NextHis();

                     }  
                     if((privateMibBase.mainAlarmGroup.alarmBLVD==0)&&(privateMibBase.mainAlarmGroup.alarmBLVD_old==1))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =14;
                          NextHis();

                     }
                     if((privateMibBase.mainAlarmGroup.alarmBattBreakerOpen ==1)&&(privateMibBase.mainAlarmGroup.alarmBattBreakerOpen_old==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =15;
                          NextHis();

                     }  
                     if((privateMibBase.mainAlarmGroup.alarmBattBreakerOpen==0)&&(privateMibBase.mainAlarmGroup.alarmBattBreakerOpen_old==1))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =16;
                          NextHis();

                     }
                     if((privateMibBase.mainAlarmGroup.alarmBattHighTemp ==2)&&(privateMibBase.mainAlarmGroup.alarmBattHighTemp_old==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =17;
                          NextHis();

                     }  
                     if((privateMibBase.mainAlarmGroup.alarmBattHighTemp==0)&&(privateMibBase.mainAlarmGroup.alarmBattHighTemp_old==2))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =18;
                          NextHis();

                     }
                     
                     if((privateMibBase.mainAlarmGroup.alarmRectACFault ==1)&&(privateMibBase.mainAlarmGroup.alarmRectACFault_old==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =19;
                          NextHis();

                     }  
                     if((privateMibBase.mainAlarmGroup.alarmRectACFault==0)&&(privateMibBase.mainAlarmGroup.alarmRectACFault_old==1))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =20;
                          NextHis();

                     }
                     if((privateMibBase.mainAlarmGroup.alarmRectNoResp ==1)&&(privateMibBase.mainAlarmGroup.alarmRectNoResp_old==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =21;
                          NextHis();

                     }  
                     if((privateMibBase.mainAlarmGroup.alarmRectNoResp==0)&&(privateMibBase.mainAlarmGroup.alarmRectNoResp_old==1))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =22;
                          NextHis();

                     }
                     
                     if((privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen ==1)&&(privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen_old==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =23;
                          NextHis();

                     }  
                     if((privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen==0)&&(privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen_old==1))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =24;
                          NextHis();

                     }
                     
                     if((privateMibBase.mainAlarmGroup.alarmSmoke==1)&&(privateMibBase.mainAlarmGroup.alarmSmoke_old==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =25;
                          NextHis();

                     }  
                     if((privateMibBase.mainAlarmGroup.alarmSmoke==0)&&(privateMibBase.mainAlarmGroup.alarmSmoke_old==1))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =26;
                          NextHis();

                     }
                     
                     if((privateMibBase.diAlarmGroup.alarmDigitalInput[0]==1)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[0]==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =31;
                          NextHis();

                     }  
                     if((privateMibBase.diAlarmGroup.alarmDigitalInput[0]==0)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[0]==1))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =32;
                          NextHis();

                     }
                    if((privateMibBase.diAlarmGroup.alarmDigitalInput[1]==1)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[1]==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =33;
                          NextHis();

                     }  
                     if((privateMibBase.diAlarmGroup.alarmDigitalInput[1]==0)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[1]==1))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =34;
                          NextHis();

                     }
                    if((privateMibBase.diAlarmGroup.alarmDigitalInput[2]==1)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[2]==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =35;
                          NextHis();

                     }  
                     if((privateMibBase.diAlarmGroup.alarmDigitalInput[2]==0)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[2]==1))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =36;
                          NextHis();

                     }
                    if((privateMibBase.diAlarmGroup.alarmDigitalInput[3]==1)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[3]==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =37;
                          NextHis();

                     }  
                     if((privateMibBase.diAlarmGroup.alarmDigitalInput[3]==0)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[3]==1))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =38;
                          NextHis();

                     }
                    if((privateMibBase.diAlarmGroup.alarmDigitalInput[4]==1)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[4]==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =39;
                          NextHis();

                     }  
                     if((privateMibBase.diAlarmGroup.alarmDigitalInput[4]==0)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[4]==1))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =40;
                          NextHis();

                     }
                    if((privateMibBase.diAlarmGroup.alarmDigitalInput[5]==1)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[5]==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =41;
                          NextHis();

                     }  
                     if((privateMibBase.diAlarmGroup.alarmDigitalInput[5]==0)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[5]==1))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =42;
                          NextHis();

                     }
                    if((privateMibBase.diAlarmGroup.alarmDigitalInput[6]==1)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[6]==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =43;
                          NextHis();

                     }  
                     if((privateMibBase.diAlarmGroup.alarmDigitalInput[6]==0)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[6]==1))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =44;
                          NextHis();

                     }
                    if((privateMibBase.diAlarmGroup.alarmDigitalInput[7]==1)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[7]==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =45;
                          NextHis();

                     }  
                     if((privateMibBase.diAlarmGroup.alarmDigitalInput[7]==0)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[7]==1))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =46;
                          NextHis();

                     }
                     if((privateMibBase.connAlarmGroup.alarmPMUConnect_old ==0)&& (privateMibBase.connAlarmGroup.alarmPMUConnect !=0))
                     {
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =29;
                          NextHis();
                         }
                     if((privateMibBase.connAlarmGroup.alarmPMUConnect_old !=0)&& (privateMibBase.connAlarmGroup.alarmPMUConnect ==0))
                     {
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =30;
                          NextHis();                          
                     }
                     
                     privateMibBase.mainAlarmGroup.alarmACmains_old  =  privateMibBase.mainAlarmGroup.alarmACmains;
                     privateMibBase.mainAlarmGroup.alarmACLow_old  =  privateMibBase.mainAlarmGroup.alarmACLow;
                     privateMibBase.mainAlarmGroup.alarmACSPD_old  = privateMibBase.mainAlarmGroup.alarmACSPD;                                      
                     privateMibBase.mainAlarmGroup.alarmDCLow_old  =  privateMibBase.mainAlarmGroup.alarmDCLow;
                     privateMibBase.mainAlarmGroup.alarmDCSPD_old = privateMibBase.mainAlarmGroup.alarmDCSPD;
                     privateMibBase.mainAlarmGroup.alarmLLVD_old  =  privateMibBase.mainAlarmGroup.alarmLLVD;
                     privateMibBase.mainAlarmGroup.alarmBLVD_old  =  privateMibBase.mainAlarmGroup.alarmBLVD;
                     privateMibBase.mainAlarmGroup.alarmBattBreakerOpen_old =  privateMibBase.mainAlarmGroup.alarmBattBreakerOpen;
                     privateMibBase.mainAlarmGroup.alarmBattHighTemp_old  =  privateMibBase.mainAlarmGroup.alarmBattHighTemp;
                     privateMibBase.mainAlarmGroup.alarmRectACFault_old  =  privateMibBase.mainAlarmGroup.alarmRectACFault;
                     privateMibBase.mainAlarmGroup.alarmRectNoResp_old  =  privateMibBase.mainAlarmGroup.alarmRectNoResp;
                     privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen_old  =  privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen;
                     privateMibBase.mainAlarmGroup.alarmSmoke_old  =  privateMibBase.mainAlarmGroup.alarmSmoke;
                     privateMibBase.connAlarmGroup.alarmPMUConnect_old = privateMibBase.connAlarmGroup.alarmPMUConnect;
                     for(i=0;i<8;i++)
                     {
                       privateMibBase.diAlarmGroup.alarmDigitalInput_old[i]=  privateMibBase.diAlarmGroup.alarmDigitalInput[i];
                     }
                    // ======================== Update alarm hitory ==========================//                   

                    // ======================== Update Env information to SNMP resource ==========================//

                     MESGState = WRITE_FLASH;
                    break;
                case WRITE_FLASH:
                  {if(writeFlash>10)
              {
                writeFlash=0;
                for(i=0;i<5;i++)
                {
                   
                   if(sHisFlashLog[i].Count_old != sHisFlashLog[i].Count)
                  {  
                    pHisFlashLog =    &sHisFlashLog[i];
                    sHisFlashLog[i].Count_old = sHisFlashLog[i].Count;
                    switch(i)
                    {
                    case 0:
                        sFLASH_EraseSector(FLASH_PB_LOG_START);
                        sFLASH_WriteBuffer((uint8_t *)pHisFlashLog, FLASH_PB_LOG_START, 256);
                      break;
                    case 1:
                        sFLASH_EraseSector(FLASH_PB1_LOG_START);
                        sFLASH_WriteBuffer((uint8_t *)pHisFlashLog, FLASH_PB1_LOG_START, 256);
                      break;
                    case 2:
                      sFLASH_EraseSector(FLASH_PB2_LOG_START);
                      sFLASH_WriteBuffer((uint8_t *)pHisFlashLog, FLASH_PB2_LOG_START, 256);
                      break;
                    case 3:
                       sFLASH_EraseSector(FLASH_PB3_LOG_START);
                       sFLASH_WriteBuffer((uint8_t *)pHisFlashLog, FLASH_PB3_LOG_START, 256);
                      break;
                    case 4:
                       sFLASH_EraseSector(FLASH_PB4_LOG_START);
                       sFLASH_WriteBuffer((uint8_t *)pHisFlashLog, FLASH_PB4_LOG_START, 256);
                      break;
                    default:
                      break;
                    }


                  }
                }
              
              }
            if( writeFlash==5)
            {
//              sFLASH_ReadBuffer((uint8_t *)&u8HisBuffer[0],FLASH_PB_LOG_START, 256);
//                 sFLASH_EraseSector(FLASH_PB_LOG_START);
//                 sFLASH_EraseSector(FLASH_PB1_LOG_START);
//                 sFLASH_EraseSector(FLASH_PB2_LOG_START);
//                 sFLASH_EraseSector(FLASH_PB3_LOG_START);
//                 sFLASH_EraseSector(FLASH_PB4_LOG_START);
            }
             writeFlash++;                  
                    MESGState = ACINPUTINFO_REQ;
                    break;
                  }
                    break;
                default:
                    break;
                }
                

                
                if(g_testchecksum != g_checksum)
                {
                    reverror++;
                    MESGStatetest[k] =  MESGState;
                    k++;
                    if(k>19) k =0;
                }
                g_testchecksum = 0xFFFF;
            g_checksum = 0;
            }
            
        }
        vTaskDelay(400);
    }
    while (1);
    vTaskSuspend(NULL);
}



void ZTE_Data_Process(void *pvParameters)
{
    uint32_t i,j;
    uint32_t fuse_flag;
    uint32_t temp[2];
    MESGState = ACINPUTINFO_REQ;

    do
    {
       ZTE_cnt++;
       privateMibBase.cntGroup.ZTEcnt = ZTE_cnt;
       privateMibBase.cntGroup.ZTEState= MESGState;
       
        if (sInterruptMesg.u8Checkbit == 1)
        {
//
//        g_testchecksum = hex2byte2(&g_UARTRxBuf2[0],RecvCntStart2-5);
//        g_checksum = Checksum16(&g_UARTRxBuf2[0],RecvCntStart2);
//        if (g_checksum == g_testchecksum)
//        {
//          u8InterruptCode = hex2byte(&g_UARTRxBuf2[0],3);
//          switch(u8InterruptCode)
//          {
//            case 0x01:
//            {
//              MESGState = CALIB_MESG_1;
//
//            } break;
//            case 0x02:
//            {
//              u32IPAddr = hex2byte4(&g_UARTRxBuf2[0],13);
//              u32SNAddr = hex2byte4(&g_UARTRxBuf2[0],21);
//              u32GWAddr = hex2byte4(&g_UARTRxBuf2[0],29);
//              u32SIPAddr = hex2byte4(&g_UARTRxBuf2[0],37);
//
//              //
//              // If we are now using static IP, check for modifications to the IP
//              // addresses and mask.
//              //
//
//              if((g_sParameters.ulStaticIP != u32IPAddr) ||
//                 (g_sParameters.ulGatewayIP != u32GWAddr) ||
//                 (g_sParameters.ulSubnetMask != u32SNAddr) ||
//                 (g_sParameters.sPort[1].ulTelnetIPAddr != u32SIPAddr))
//              {
//                      //
//                      // Something changed so update the parameter block.
//                      //
//                      g_sParameters.ulStaticIP = u32IPAddr;
//                      g_sParameters.ulGatewayIP = u32GWAddr;
//                      g_sParameters.ulSubnetMask = u32SNAddr;
//                      g_sParameters.sPort[1].ulTelnetIPAddr = u32SIPAddr;
//
//                      //
//                      // Apply all the changes to the working parameter set.
//                      //
//                      ConfigUpdatePortParameters(1, 0, 1);
//
//                      //
//                      // Update the working default set and save the parameter block.
//                      //
//                      g_sWorkingDefaultParameters = g_sParameters;
//                      u8SaveConfigFlag |= 1;
//
//                      //
//                      // Tell the main loop that a IP address update has been requested.
//                      //
//                      g_cUpdateRequired |= UPDATE_IP_ADDR;
//              }
//              MESGState = CALIB_MESG_2;
//            } break;
//            case 0x03:
//            {
//              ui8User[0] = hex2byte(&g_UARTRxBuf2[0],13);
//              ui8User[1] = hex2byte(&g_UARTRxBuf2[0],15);
//              ui8User[2] = hex2byte(&g_UARTRxBuf2[0],17);
//              ui8User[3] = hex2byte(&g_UARTRxBuf2[0],19);
//              ui8User[4] = hex2byte(&g_UARTRxBuf2[0],21);
//              ui8User[5] = hex2byte(&g_UARTRxBuf2[0],23);
//
//              eeprom_write_byte(MAC_ALREADY_WRITTEN,0x53);
//
//              eeprom_write_byte(EEPROM_MACADDR1,ui8User[0]);
//              eeprom_write_byte(EEPROM_MACADDR2,ui8User[1]);
//              eeprom_write_byte(EEPROM_MACADDR3,ui8User[2]);
//              eeprom_write_byte(EEPROM_MACADDR4,ui8User[3]);
//              eeprom_write_byte(EEPROM_MACADDR5,ui8User[4]);
//              eeprom_write_byte(EEPROM_MACADDR6,ui8User[5]);
//
//              if (eeprom_read_byte(MAC_ALREADY_WRITTEN) == 0x53)
//              {
//                      MESGState = CALIB_MESG_3;
//              }
//
//
//
//            } break;
//            default:
//             break;
//          }
//
//        }
            sInterruptMesg.u8Checkbit = 0;

        }
        else if (sInterruptMesg.u8Checkbit == 0)
        {
             if((privateMibBase.connAlarmGroup.alarmPMUConnect_old3 ==0)&& (privateMibBase.connAlarmGroup.alarmPMUConnect !=0))
           {
             if(ZTETime.Year!=0)
               {
                sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =29;
                sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Year  =   ZTETime.Year-2000;
                sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Month =   ZTETime.Month;
                sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Day   =   ZTETime.Date;
                sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hour  =   ZTETime.Hour;
                sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Minute=   ZTETime.Minute;
                NextHis();
                privateMibBase.connAlarmGroup.alarmPMUConnect_old3 = privateMibBase.connAlarmGroup.alarmPMUConnect;
//                HisCount_old[ih] = sHisFlashLog[ih].Count;
               }

           }
           if((privateMibBase.connAlarmGroup.alarmPMUConnect_old3 !=0)&& (privateMibBase.connAlarmGroup.alarmPMUConnect ==0))
           {
             if(ZTETime.Year!=0)
               {
                sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =30;
                sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Year  =   ZTETime.Year-2000;
                sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Month =   ZTETime.Month;
                sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Day   =   ZTETime.Date;
                sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hour  =   ZTETime.Hour;
                sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Minute=   ZTETime.Minute;
                NextHis();
                privateMibBase.connAlarmGroup.alarmPMUConnect_old3 = privateMibBase.connAlarmGroup.alarmPMUConnect;

               }
           }
            if (setCmd_flag == 1)
            {
                settingCommand |= setCmd_mask;

                switch(settingCommand)
                {
                case SET_FLTVOL:
                    sBattInfo.sRAWBattInfo.fFltVoltCfg = (float)privateMibBase.configGroup.cfgFloatVolt/100;
//                    sBattInfo.sRAWBattInfo.fFltVoltCfg = a;
                    ZTE_SetCommand(&g_setInfo[0],FLOATVOLT,sBattInfo.sRAWBattInfo.fFltVoltCfg,DC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_BSTVOL:
                    sBattInfo.sRAWBattInfo.fBotVoltCfg = (float)privateMibBase.configGroup.cfgBoostVolt/100;
                    ZTE_SetCommand(&g_setInfo[0],BOOSTVOLT,sBattInfo.sRAWBattInfo.fBotVoltCfg,DC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_TEMPCOMP_VAL:
                    sBattInfo.sRAWBattInfo.fTempCompVal = (float)privateMibBase.configGroup.cfgTempCompValue;
                    ZTE_SetCommand(&g_setInfo[0],TEMP_COMPENSATION,sBattInfo.sRAWBattInfo.fTempCompVal,DC);
                    for (i = 0; i < 28; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_LLVD:
                    sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg = (float)privateMibBase.configGroup.cfgLLVDVolt/100;
                    ZTE_SetCommand(&g_setInfo[0],LLVD,sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg,DC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_BLVD:
                    sBattInfo.sRAWBattInfo.fLVDDV = (float)privateMibBase.configGroup.cfgBLVDVolt/100;
                    ZTE_SetCommand(&g_setInfo[0],BLVD,sBattInfo.sRAWBattInfo.fLVDDV,DC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_DCLOW:
                    sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg = (float)privateMibBase.configGroup.cfgDCLowVolt/100;
                    ZTE_SetCommand(&g_setInfo[0],DCLOW,sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg,DC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;

                case SET_BATTSTDCAP:
                    sBattInfo.sRAWBattInfo.fCapTotal = (float)privateMibBase.configGroup.cfgBattCapacityTotal;
                    ZTE_SetCommand(&g_setInfo[0],BATT_CAP1,sBattInfo.sRAWBattInfo.fCapTotal,DC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_BATTSTDCAP2:
                    sBattInfo.sRAWBattInfo.fCapTotal2 = (float)privateMibBase.configGroup.cfgBattCapacityTotal2;
                    ZTE_SetCommand(&g_setInfo[0],BATT_CAP2,sBattInfo.sRAWBattInfo.fCapTotal2,DC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_CCL:
                    sBattInfo.sRAWBattInfo.fCCLVal = (float)privateMibBase.configGroup.cfgCurrentLimit/1000;
                    ZTE_SetCommand(&g_setInfo[0],CHARGINGFACTOR,sBattInfo.sRAWBattInfo.fCCLVal,DC);

                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_OVERTEMP:
                    sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel = (float)privateMibBase.configGroup.cfgHighMajorTempLevel/100;
                    ZTE_SetCommand(&g_setInfo[0],BATT_TEMP_OVER,sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel,DC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);

                    }
                    break;

//				case SET_WIT_VAL:
////					g_sParameters.ucReserved2[11] = privateMibBase.configGroup.cfgWalkInTimeDuration;
//					sRectInfo.u8WITI = (float)privateMibBase.configGroup.cfgWalkInTimeDuration;
//					setCommand(&g_setInfo[0],&cmdCodeTable[36][0], sRectInfo.u8WITI,0);
//					for (i = 0; i < 28; i++)
//					{
//
//                                                UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
//					}
//				break;

//				case SET_WIT_EN:
//					g_sParameters.ucReserved2[12] = privateMibBase.configGroup.cfgWalkInTimeEn;
//					sRectInfo.u8WITE = (float)privateMibBase.configGroup.cfgWalkInTimeEn;
//					setCommand(&g_setInfo[0],&cmdCodeTable[37][0], sRectInfo.u8WITE,1);
//					for (i = 0; i < 28; i++)
//					{
//
//                                                 UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
//					}
//				break;

                case SET_AC_LOW_THRES:
                    sAcInfo.fAcLowThres = (float)privateMibBase.configGroup.cfgAcLowLevel;
                    ZTE_SetCommand(&g_setInfo[0],ACVOLT_LOWERLIMIT,sAcInfo.fAcLowThres,AC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_AC_HIGH_THRES:
                    sAcInfo.fAcHighThres = (float)privateMibBase.configGroup.cfgAcHighLevel;
                    ZTE_SetCommand(&g_setInfo[0],ACVOLT_UPPERLIMIT,sAcInfo.fAcHighThres,AC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                case SET_AC_INPUT_CURR_LIMIT:
//					sAcInfo.fAcHighThres = (float)privateMibBase.configGroup.cfgAcLowLevel;
                    ZTE_SetCommand(&g_setInfo[0],ACINPUTCUR_UPPERLIMIT,sAcInfo.fAcInputCurrLimit,AC);
                    for (i = 0; i < 28; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);
                    }
                    break;
                 case SET_TEST_VOLT:
                    sBattInfo.sRAWBattInfo.fTestVoltCfg = (float)privateMibBase.cfgBTGroup.cfgTestVoltCfg/10;
                    ZTE_SetCommand(&g_setInfo[0],TESTVOLT,sBattInfo.sRAWBattInfo.fTestVoltCfg,DC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);

                    }
                    break;
                case SET_AUTO_TEST_DAY:
                    sBattInfo.sRAWBattInfo.fAutoTestDay = (float)privateMibBase.cfgBTGroup.cfgAutoTestDay;
                    ZTE_SetCommand(&g_setInfo[0],AUTOTESTDAY,sBattInfo.sRAWBattInfo.fAutoTestDay,DC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);

                    }
                    break;
                 case SET_TEST_START_TIME:
                    sBattInfo.sRAWBattInfo.fTestStartTime = (float)privateMibBase.cfgBTGroup.cfgTestStartTime;
                    ZTE_SetCommand(&g_setInfo[0],TESTSTARTTIME,sBattInfo.sRAWBattInfo.fTestStartTime,DC);
                    for (i = 0; i < 28; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&g_setInfo[i],1);

                    }
                    break;

                default:
                    break;
                }
                setCmd_mask = 0;
                setCmd_flag = 0;
                settingCommand = 0;

            }
            else if (setCmd_flag == 0)
            {
                if(PMUConnectCount++>30)
                {
                  PMUConnectCount =50;
                  privateMibBase.connAlarmGroup.alarmPMUConnect = 1;
                  g_testchecksum=0xFFFF;g_checksum=0;
                }
                switch(MESGState)
                {
                 case HISTORYMESG_REQ:
//                  thao++;
                  switch(ModuleHis)
                    {
                    case ACHis:
                       if(ZTEHIS.ZTEHisCommand==3)
                        {
                          for (i = 0; i < 22; i++)
                          {
                              UART_WriteBlocking(RS2321_UART,&REQachistory03[i],1);
                          }
//                          thao =1;
                        }
                       if(ZTEHIS.ZTEHisCommand==1)
                        {
                          for (i = 0; i < 22; i++)
                          {
                              UART_WriteBlocking(RS2321_UART,&REQachistory01[i],1);
                          }
//                           thao =2;
                        }
                        if(ZTEHIS.ZTEHisCommand==0)
                          {
                            for (i = 0; i < 22; i++)
                            {
                                UART_WriteBlocking(RS2321_UART,&REQachistory00[i],1);
                                
                            }
//                               thao++;
//                             thao =3;
                          }
                      break;
                    case RectHis:
                       if(ZTEHIS.ZTEHisCommand==3)
                        {
                          for (i = 0; i < 20; i++)
                          {
                              UART_WriteBlocking(RS2321_UART,&REQrecthistory03[i],1);
                          }
//                           thao =4;
                        }
                       if(ZTEHIS.ZTEHisCommand==1)
                        {
                          for (i = 0; i < 20; i++)
                          {
                              UART_WriteBlocking(RS2321_UART,&REQrecthistory01[i],1);
                          }
//                           thao =5;
                        }
                        if(ZTEHIS.ZTEHisCommand==0)
                          {
                            for (i = 0; i < 20; i++)
                            {
                                UART_WriteBlocking(RS2321_UART,&REQrecthistory00[i],1);
                                
                            }
//                             thao++;
//                            ZTEHIS.ZTEHisCommand =1;
//                             thao =6;
                          }
                      
                      break;
                    case DCHis:
                       if(ZTEHIS.ZTEHisCommand==3)
                        {
                          for (i = 0; i < 20; i++)
                          {
                              UART_WriteBlocking(RS2321_UART,&REQdchistory03[i],1);
                          }
//                           thao =7;
                        }
                       if(ZTEHIS.ZTEHisCommand==1)
                        {
                          for (i = 0; i < 20; i++)
                          {
                              UART_WriteBlocking(RS2321_UART,&REQdchistory01[i],1);
                          }
//                           thao =8;
                        }
                        if(ZTEHIS.ZTEHisCommand==0)
                          {
                            for (i = 0; i < 20; i++)
                            {
                                UART_WriteBlocking(RS2321_UART,&REQdchistory00[i],1);
                                
                            }
//                            ZTEHIS.ZTEHisCommand =1;
//                             thao =9;
                          }
                      
                      break;
                    case EnvHis:
                       if(ZTEHIS.ZTEHisCommand==3)
                        {
                          for (i = 0; i < 20; i++)
                          {
                              UART_WriteBlocking(RS2321_UART,&REQenvhistory03[i],1);
                          }
//                           thao =10;
                        }
                       if(ZTEHIS.ZTEHisCommand==1)
                        {
                          for (i = 0; i < 20; i++)
                          {
                              UART_WriteBlocking(RS2321_UART,&REQenvhistory01[i],1);
                          }
//                           thao =11;
                        }
                        if(ZTEHIS.ZTEHisCommand==0)
                          {
                            for (i = 0; i < 20; i++)
                            {
                                UART_WriteBlocking(RS2321_UART,&REQenvhistory00[i],1);
                                
                            }
//                             thao =12;
//                            ZTEHIS.ZTEHisCommand =1;
                          }
                      break;
                    default:
                      break;
                    }
                 
                    MESGState = HISTORYMESG_RES;
                    break;

                case HISTORYMESG_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
//                        athao[i] =   g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;
                    if(RecvCount>6)
                    {
                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-6);
                        g_checksum = Checksum16(&RecvBuff[0],RecvCount-1);
                    }
                    if (g_checksum == g_testchecksum)
                    {
                      ZTEHIS.ZTEHisStatus = hex2byte(&RecvBuff[0],13);
                      if(ZTEHIS.ZTEHisStatus<2)
                        {
                          switch(ModuleHis)
                            {
                            case ACHis:
                              GetACDataLog();
                              break;
                            case RectHis:
                              GetRectDataLog();
                              break;
                            case DCHis:
                              GetDCDataLog();
                              break;
                            case EnvHis:
                              GetEnvDataLog();
                              break;
                            default:
                              break;
                            } 
                        if(ZTEHIS.ZTEHisStatus==1)
                          {  
    //                        MESGState = HISTORYMESG_REQ;
                             ZTEHIS.ZTEHisCommand =3;
                          }
                        if((ZTEHIS.ZTEHisCommand ==0)&& (ZTEHIS.ZTEHisStatus==0) )
                          {  
                             ZTEHIS.ZTEHisCommand =1;
    //                        MESGState = HISTORYMESG_REQ;
    //                         ZTEHIS.ZTEHisCommand =3;
                          }

    //                       ZTEHIS.ZTEHisCommand =1;
       
                        }

                    }


                    RecvCntStart = 0;
                    MESGState = HISTORYMESG_REQ;
                     if(ZTEHIS.ZTEHisStatus==2)
                      {

                        if(ModuleHis!=DCHis)
                          {
                            switch(ModuleHis)
                            {
                            case ACHis:
                              ModuleHis = RectHis;
                              break;
                            case RectHis:
                              ModuleHis = DCHis;
                              break;
                            case DCHis:
                              ModuleHis = ACHis;
                              break;
                            case EnvHis:
                              ModuleHis = ACHis;
                              break;
                            }
                         
                            
                          }
                        else 
                          {
                            MESGState = ACINPUTINFO_REQ;
                            ModuleHis=ACHis;
                          }
                         ZTEHIS.ZTEHisCommand =0;
                         ZTEHIS.ZTEHisStatus =0;     
                         
                      }    
                  break;


//====================================== ACINFO MESSAGE ======================================//
                case  ACINPUTINFO_REQ:
//						thao++;
                    for (i = 0; i < REQacinputinfoLen; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&REQacinputinfo[i],1);

                    }
                    MESGState = ACINPUTINFO_RES;
                    break;
                case  ACINPUTINFO_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;
                    if(RecvCount>6)
                    {
                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-6);
                        g_checksum = Checksum16(&RecvBuff[0],RecvCount-1);
                    }

                    if (g_checksum == g_testchecksum)
                    {
                      sDcInfo.u32DCNoResponse = 0;
                      privateMibBase.connAlarmGroup.alarmPMUConnect = 0;
                        sAcInfo.facVolt[0] = hex2float(&RecvBuff[0],19);
                        sAcInfo.facVolt[1] = hex2float(&RecvBuff[0],27);
                        sAcInfo.facVolt[2] = hex2float(&RecvBuff[0],35);
                    }
                    else
                    {
                      sDcInfo.u32DCNoResponse++;
                      if (sDcInfo.u32DCNoResponse > 3)
                      {
                        privateMibBase.connAlarmGroup.alarmPMUConnect = 1;
                        g_testchecksum=0xFFFF;g_checksum=0;
                        sDcInfo.u32DCNoResponse = 10;
                        
                      // xoa khi checksum sai
                      sAcInfo.facVolt[0] = 0;
                      sAcInfo.facVolt[1] = 0;
                      sAcInfo.facVolt[2] = 0;
                      sAcInfo.fAcHighThres = 0;
                      sAcInfo.fAcLowThres = 0;
                      sAcInfo.fAcInputCurrLimit = 0;
                      sRectInfo.fAllRectDcVolt = 0;
                      for(i=0; i<10; i++)
                      {
                        sRectInfo.sRAWRectParam[i].fRect_DcCurr = 0;
                      }
                      sDcInfo.sINFODcInfo.fVoltage = 0;
                      sDcInfo.sINFODcInfo.fCurrent = 0;
                      sDcInfo.sINFODcInfo.u8BatNo = 0;                          
                      sDcInfo.sINFODcInfo.fBatt1Curr = 0;
                      sDcInfo.sINFODcInfo.fBatt2Curr = 0;
                      sDcInfo.sINFODcInfo.fBatt3Curr = 0;
                      sDcInfo.sINFODcInfo.fSen1BattTemp = 0;
                      sDcInfo.sINFODcInfo.fBatt1Volt = 0;
                      sDcInfo.sINFODcInfo.fSen2BattTemp = 0;
                      sDcInfo.sINFODcInfo.fBatt2Volt = 0;
                      sDcInfo.sINFODcInfo.fSen3BattTemp = 0;
                      sDcInfo.sINFODcInfo.fBatt3Volt = 0;
                      sDcInfo.sINFODcInfo.fBatt1RmnCap = 0;
                      sBattInfo.sRAWBattInfo.fFltVoltCfg = 0;
                      sBattInfo.sRAWBattInfo.fBotVoltCfg = 0;
                      sBattInfo.sRAWBattInfo.fTestVoltCfg = 0;
                      sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg = 0;
                      sBattInfo.sRAWBattInfo.fLVDDV = 0;
                      sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg = 0;
                      sBattInfo.sRAWBattInfo.fCCLVal = 0;
                      sBattInfo.sRAWBattInfo.fTempCompVal = 0;
                      sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel = 0;
                      sBattInfo.sRAWBattInfo.fCapTotal = 0;
                      sBattInfo.sRAWBattInfo.fCapTotal2 = 0;
                      sBattInfo.sRAWBattInfo.fAutoTestDay = 0;
                      sBattInfo.sRAWBattInfo.fTestStartTime =  0;
                      }
                    }


                    RecvCntStart = 0;
                    MESGState = ACPARAMETER_REQ;

                    break;
                case  ACPARAMETER_REQ:
                    for (i = 0; i < REQacinputinfoLen; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&REQacparameter[i],1);
                    }
                    MESGState = ACPARAMETER_RES;
                    break;
                case  ACPARAMETER_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;
                    if(RecvCount>6)
                    {
                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-6);
                        g_checksum = Checksum16(&RecvBuff[0],RecvCount-1);
                    }
                    if (g_checksum == g_testchecksum)
                    {
                        sAcInfo.fAcHighThres = hex2float(&RecvBuff[0],13);
                        sAcInfo.fAcLowThres = hex2float(&RecvBuff[0],21);
                        sAcInfo.fAcInputCurrLimit = hex2float(&RecvBuff[0],29);
                    }

                    RecvCntStart = 0;
                    MESGState = ACALARM_REQ;
                    break;
                case  ACALARM_REQ:
                    for (i = 0; i < REQacalarmLen; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&REQacalarm[i],1);
                    }
                    MESGState = ACALARM_RES;
                    break;
                case  ACALARM_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    if(RecvCount>6)
                    {
                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-6);
                        g_checksum = Checksum16(&RecvBuff[0],RecvCount-1);
                    }
                    if (g_checksum == g_testchecksum)
                    {

                        sAcInfo.u8Thres[0] = hex2byte(&RecvBuff[0],19);
                        sAcInfo.u8Thres[1] = hex2byte(&RecvBuff[0],21);
                        sAcInfo.u8Thres[2] = hex2byte(&RecvBuff[0],23);

                        sAcInfo.u8_I_Thres[0] = hex2byte(&RecvBuff[0],39);
                        sAcInfo.u8_I_Thres[1] = hex2byte(&RecvBuff[0],41);
                        sAcInfo.u8_I_Thres[2] = hex2byte(&RecvBuff[0],43);
                        sAcInfo.u8MainFail = hex2byte(&RecvBuff[0],31);
                        sAcInfo.u8PowerOff = hex2byte(&RecvBuff[0],33);
                        sAcInfo.u8ACSPD  =  hex2byte(&RecvBuff[0],35);
                        // Chen lenh bao alarm ac tai day
                    }


                    RecvCntStart = 0;
                    MESGState = RECTSTSZTE_REQ;
                    break;
                //====================================== RECTINFO MESSAGE ======================================//
                case RECTSTSZTE_REQ:
                    for (i = 0; i < REQrectstsLen; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&REQrectstszte[i],1);

                    }
                    MESGState = RECTSTSZTE_RES;
                    break;
                case RECTSTSZTE_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;
                    if(RecvCount>6)
                    {
                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-6);
                        g_checksum = Checksum16(&RecvBuff[0],RecvCount-1);
                    }
                    if (g_checksum == g_testchecksum)
                    {
                        RectNumber = 0;
                        for(i=0; i<10; i++)
                        {
                            Rectlosedtest = sRectInfo.sRAWRectParam[i].u8Rect_Sts;
                            sRectInfo.sRAWRectParam[i].u8Rect_OnPosition = hex2byte(&RecvBuff[0],25+10*i);
                            if(sRectInfo.sRAWRectParam[i].u8Rect_OnPosition==1)
                            {
                                sRectInfo.sRAWRectParam[i].u8Rect_Sts = hex2byte(&RecvBuff[0],17+10*i);

                                RectNumber++;
                            }
                            else
                                sRectInfo.sRAWRectParam[i].u8Rect_Sts =0;

                            if((Rectlosedtest ==1)&&(sRectInfo.sRAWRectParam[i].u8Rect_Sts==0))
                            {
                                Rectlosedtest =0;
                                sRectInfo.sRAWRectParam[i].u8Rect_NoResp =1;
                            }
                            if((sRectInfo.sRAWRectParam[i].u8Rect_NoResp ==1)&&(sRectInfo.sRAWRectParam[i].u8Rect_Sts==1))
                                sRectInfo.sRAWRectParam[i].u8Rect_NoResp =0;
                        }
                        sRectInfo.u8Rect_Num =RectNumber;
                    }

                    RecvCntStart = 0;
                    MESGState = RECTINFOZTE_REQ;
                    break;
                case  RECTINFOZTE_REQ:
                    for (i = 0; i < REQrectinfoLen; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&REQrectinfo[i],1);
                    }
                    MESGState = RECTINFOZTE_RES;
                    break;
                case  RECTINFOZTE_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    if(RecvCount>6)
                    {
                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-6);
                        g_checksum = Checksum16(&RecvBuff[0],RecvCount-1);
                    }
                    if (g_checksum == g_testchecksum)
                    {
                        sRectInfo.fAllRectDcVolt = hex2float(&RecvBuff[0],15);
                        for(i=0; i<10; i++)
                        {
                            if(sRectInfo.sRAWRectParam[i].u8Rect_Sts==1)
                            {
                                sRectInfo.sRAWRectParam[i].fRect_DcCurr = hex2float(&RecvBuff[0],25+10*i);
                                if(sRectInfo.sRAWRectParam[i].fRect_DcCurr == 0x20202020)sRectInfo.sRAWRectParam[i].fRect_DcCurr =0;
                            }
                            else
                            {
                                sRectInfo.sRAWRectParam[i].fRect_DcCurr = 0;
                            }

                        }
                    }


                    RecvCntStart = 0;
                    MESGState = RECTALARM_REQ;
                    break;
                case  RECTALARM_REQ:
                    for (i = 0; i < REQrectalarmLen; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&REQrectalarm[i],1);
                    }
                    MESGState = RECTALARM_RES;
                    break;
                case  RECTALARM_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    if(RecvCount>6)
                    {
                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-6);
                        g_checksum = Checksum16(&RecvBuff[0],RecvCount-1);
                    }
                    if (g_checksum == g_testchecksum)
                    {
                      if(RecvBuff[2] == 0x30) // version 2.0
                      {
                        for(i=1; i<10; i++)
                          {
                              sRectInfo.sRAWRectParam[i-1].u8Rect_Fail = hex2byte(&RecvBuff[0],((i-1)*4+17)); // 00H normal, 01H Fail
                              sRectInfo.sRAWRectParam[i-1].u8Rect_Out = 0; // 00H normal, 01H Fail
                          }
                      }
                      else // version 2.1
                      {
                        for(i=1; i<10; i++)
                        {
                            sRectInfo.sRAWRectParam[i-1].u8Rect_Fail = hex2byte(&RecvBuff[0],((i-1)*6+17)); // 00H normal, 01H Fail
                            sRectInfo.sRAWRectParam[i-1].u8Rect_Out = hex2byte(&RecvBuff[0],((i-1)*6+17+4)); // 00H normal, 01H Fail
                        }
                      }
                    }


                    RecvCntStart = 0;
                    MESGState = DCOUTPUT_REQ;
                    break;
                //====================================== DCINFO MESSAGE ======================================//

                case  DCOUTPUT_REQ:
                    for (i = 0; i < REQdcoutputLen; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&REQdcoutput[i],1);
                    }
                    MESGState = DCOUTPUT_RES;
                    break;
                case  DCOUTPUT_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    if(RecvCount>6)
                    {
                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-6);
                        g_checksum = Checksum16(&RecvBuff[0],RecvCount-1);
                    }
                    if (g_checksum == g_testchecksum)
                    {
                        sDcInfo.sINFODcInfo.fVoltage = hex2float(&RecvBuff[0],17);
                        sDcInfo.sINFODcInfo.fCurrent = hex2float(&RecvBuff[0],25);
                        sDcInfo.sINFODcInfo.u8BatNo = 2;                          //hex2float(&RecvBuff[0],33);
                        sDcInfo.sINFODcInfo.fBatt1Curr = hex2float(&RecvBuff[0],35);
                        sDcInfo.sINFODcInfo.fBatt2Curr = hex2float(&RecvBuff[0],43);
                        sDcInfo.sINFODcInfo.fBatt3Curr = hex2float(&RecvBuff[0],51);
                        sDcInfo.sINFODcInfo.fSen1BattTemp = hex2float(&RecvBuff[0],63);
                        sDcInfo.sINFODcInfo.fBatt1Volt = hex2float(&RecvBuff[0],71);

                        sDcInfo.sINFODcInfo.fSen2BattTemp = hex2float(&RecvBuff[0],79);
                        sDcInfo.sINFODcInfo.fBatt2Volt = hex2float(&RecvBuff[0],87);
                        sDcInfo.sINFODcInfo.fSen3BattTemp = hex2float(&RecvBuff[0],95);
                        sDcInfo.sINFODcInfo.fBatt3Volt = hex2float(&RecvBuff[0],103);

                        if(hex2byte(&RecvBuff[0],101)==1)
                        {
                            sDcInfo.sINFODcInfo.fBatt1RmnCap = hex2float(&RecvBuff[0],125);
                        }

//							sDcInfo.sINFODcInfo.fBatt2RmnCap = hex2float(&RecvBuff[0],80);
//							sDcInfo.sINFODcInfo.fSen1AmbTemp = hex2float(&RecvBuff[0],104);
//							sDcInfo.sINFODcInfo.fSen2AmbTemp = hex2float(&RecvBuff[0],112);
                    }

                    RecvCntStart = 0;
                    MESGState = DCPARAMETER_REQ;
                    break;

                case  DCPARAMETER_REQ:
                    for (i = 0; i < REQdcparameterLen; i++)
                    {
                        UART_WriteBlocking(RS2321_UART,&REQdcparameter[i],1);
//                        athao[i]=REQdcparameter[i];
                    }
                    MESGState = DCPARAMETER_RES;
                    break;
                case  DCPARAMETER_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    if(RecvCount>6)
                    {
                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-6);
                        g_checksum = Checksum16(&RecvBuff[0],RecvCount-1);
                    }
                    if (g_checksum == g_testchecksum)
                    {
                      if(RecvBuff[2] == 0x30) // version 2.0
                      {
                        sBattInfo.sRAWBattInfo.fFltVoltCfg = roundf(hex2float(&RecvBuff[0],31)*10)/10;
                        sBattInfo.sRAWBattInfo.fBotVoltCfg = roundf(hex2float(&RecvBuff[0],39)*10)/10;
                        sBattInfo.sRAWBattInfo.fTestVoltCfg = roundf(hex2float(&RecvBuff[0],47)*10)/10;
                        sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg = roundf(hex2float(&RecvBuff[0],55)*10)/10;
                        sBattInfo.sRAWBattInfo.fLVDDV = roundf(hex2float(&RecvBuff[0],71)*10)/10;
                        sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg = roundf(hex2float(&RecvBuff[0],63)*10)/10;
                        sBattInfo.sRAWBattInfo.fCCLVal = roundf(hex2float(&RecvBuff[0],79)*1000)/1000;
                        sBattInfo.sRAWBattInfo.fTempCompVal = hex2float(&RecvBuff[0],95);
                        sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel = roundf(hex2float(&RecvBuff[0],103)*10)/10;
                        sBattInfo.sRAWBattInfo.fCapTotal = hex2float(&RecvBuff[0],111);
                        sBattInfo.sRAWBattInfo.fCapTotal2 = hex2float(&RecvBuff[0],119);
                        sBattInfo.sRAWBattInfo.fAutoTestDay = 0;
                        sBattInfo.sRAWBattInfo.fTestStartTime =  0;
                      }
                      else // version 2.1
                      {
                        sBattInfo.sRAWBattInfo.fFltVoltCfg = roundf(hex2float(&RecvBuff[0],31)*10)/10;
                        sBattInfo.sRAWBattInfo.fBotVoltCfg = roundf(hex2float(&RecvBuff[0],39)*10)/10;
                        sBattInfo.sRAWBattInfo.fTestVoltCfg = roundf(hex2float(&RecvBuff[0],47)*10)/10;
                        sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg = roundf(hex2float(&RecvBuff[0],55)*10)/10;
                        sBattInfo.sRAWBattInfo.fLVDDV = roundf(hex2float(&RecvBuff[0],71)*10)/10;
                        sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg = roundf(hex2float(&RecvBuff[0],63)*10)/10;
                        sBattInfo.sRAWBattInfo.fCCLVal = roundf(hex2float(&RecvBuff[0],79)*1000)/1000;
                        sBattInfo.sRAWBattInfo.fTempCompVal = hex2float(&RecvBuff[0],95);
                        sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel = roundf(hex2float(&RecvBuff[0],103)*10)/10;
                        sBattInfo.sRAWBattInfo.fCapTotal = hex2float(&RecvBuff[0],111);
                        sBattInfo.sRAWBattInfo.fCapTotal2 = hex2float(&RecvBuff[0],119);
                        sBattInfo.sRAWBattInfo.fAutoTestDay = hex2float(&RecvBuff[0],183);
                        sBattInfo.sRAWBattInfo.fTestStartTime =  hex2float(&RecvBuff[0],199);
                      }
                    }

                    RecvCntStart = 0;
                    MESGState = DCALARM_REQ;
//                        MESGState = DCPARAMETER_REQ;
                    break;
                case  DCALARM_REQ:
                    for (i = 0; i < REQdcalarmLen; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&REQdcalarm[i],1);
                    }
                    MESGState = DCALARM_RES;
                    break;
                case  DCALARM_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    if(RecvCount>6)
                    {
                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-6);
                        g_checksum = Checksum16(&RecvBuff[0],RecvCount-1);
                    }
                    if (g_checksum == g_testchecksum)
                    {
                      if(RecvBuff[2] == 0x30) // version 2.0
                      {
                        sDcInfo.sALARMDcInfo.u8DC = hex2byte(&RecvBuff[0],17);
                        sDcInfo.sALARMDcInfo.u8DC_NoFuse = hex2byte(&RecvBuff[0],19); // So switch trong tu
                        val = sDcInfo.sALARMDcInfo.u8DC_NoFuse;
                        for (i = 0; i < val; i++)
                        {
                            sDcInfo.sALARMDcInfo.u8LoadFuse[i] = hex2byte(&RecvBuff[0],9 + 2*i+12);
                            if((i<10)&&(sDcInfo.sALARMDcInfo.u8LoadFuse[i] == 0x03))privateMibBase.loadGroup.loadStatus[i]=1;
                        }
                        for (i = 0; i < 3; i++)
                        {
                            sDcInfo.sALARMDcInfo.u8BattFuse[i] = hex2byte(&RecvBuff[0],((val+7)*2+1+i*6+12));
                            if(sDcInfo.sALARMDcInfo.u8BattFuse[i] == 0x03)
                                privateMibBase.batteryGroup.battBrkStatus[i] = 1;
                        }
                        sDcInfo.sALARMDcInfo.u8LLVD = hex2byte(&RecvBuff[0],(val+5)*2+13);
                        sDcInfo.sALARMDcInfo.u8BLVD = hex2byte(&RecvBuff[0],(val+6)*2+13);
                        sDcInfo.sALARMDcInfo.u8BatVol[0] = hex2byte(&RecvBuff[0],(val+8)*2+13);   // Canh bao bat 1 low voltage
                        sDcInfo.sALARMDcInfo.u8Sen1_BattTemp = hex2byte(&RecvBuff[0],(val+9)*2+13);  // Canh bao nhiet do acquy1 cao
                        sDcInfo.sALARMDcInfo.u8Sen1_BattInvalid = 0; // Canh bao batt temp invalid
                        sDcInfo.sALARMDcInfo.u8BatVol[1] = hex2byte(&RecvBuff[0],(val+11)*2+13);   // Canh bao bat 2 low voltage
                        sDcInfo.sALARMDcInfo.u8Sen2_BattTemp = hex2byte(&RecvBuff[0],(val+12)*2+13); // Canh bao nhiet do acquy2 cao
                        sDcInfo.sALARMDcInfo.u8Sen2_BattInvalid = 0; // Canh bao batt temp invalid
                        sDcInfo.sALARMDcInfo.u8Sen3_BattTemp = hex2byte(&RecvBuff[0],(val+15)*2+13); // Canh bao nhiet do acquy3 cao
                        sDcInfo.sALARMDcInfo.u8Sen3_BattInvalid = 0; // Canh bao batt temp invalid
                        sDcInfo.sALARMDcInfo.u8DCSPD = hex2byte(&RecvBuff[0],(val+16)*2+13);
                      }
                      else // version 2.1
                      {
                        sDcInfo.sALARMDcInfo.u8DC = hex2byte(&RecvBuff[0],17);
                        sDcInfo.sALARMDcInfo.u8DC_NoFuse = hex2byte(&RecvBuff[0],19); // So switch trong tu
                        val = sDcInfo.sALARMDcInfo.u8DC_NoFuse;
                        for (i = 0; i < val; i++)
                        {
                            sDcInfo.sALARMDcInfo.u8LoadFuse[i] = hex2byte(&RecvBuff[0],9 + 2*i+12);
                            if((i<10)&&(sDcInfo.sALARMDcInfo.u8LoadFuse[i] == 0x03))privateMibBase.loadGroup.loadStatus[i]=1;
                        }
//
                        for (i = 0; i < 3; i++)
                        {
                            sDcInfo.sALARMDcInfo.u8BattFuse[i] = hex2byte(&RecvBuff[0],((val+7)*2+1+i*10+12));
                            if(sDcInfo.sALARMDcInfo.u8BattFuse[i] == 0x03)
                                privateMibBase.batteryGroup.battBrkStatus[i] = 1;
                        }
//									for (i = 0; i < 8; i++)
//									{
//										sDcInfo.sALARMDcInfo.u8DIFuse[i] = hex2byte(&RecvBuff[0],79 + 2*i);
//									}
//
//									sDcInfo.sALARMDcInfo.u8Batt1_OC = hex2byte(&RecvBuff[0],51);
//									sDcInfo.sALARMDcInfo.u8Batt2_OC = hex2byte(&RecvBuff[0],53);
//							sDcInfo.sALARMDcInfo.u8BLVD = hex2byte(&RecvBuff[0],(val+8)*2+13);
                        sDcInfo.sALARMDcInfo.u8LLVD = hex2byte(&RecvBuff[0],(val+5)*2+13);
                        sDcInfo.sALARMDcInfo.u8BLVD = hex2byte(&RecvBuff[0],(val+6)*2+13);
                        sDcInfo.sALARMDcInfo.u8Sen1_BattTemp = hex2byte(&RecvBuff[0],(val+9)*2+13);  // Canh bao nhiet do acquy1 cao
                        sDcInfo.sALARMDcInfo.u8BatVol[0] = hex2byte(&RecvBuff[0],(val+8)*2+13);   // Canh bao bat 1 low voltage
                        sDcInfo.sALARMDcInfo.u8Sen1_BattInvalid = hex2byte(&RecvBuff[0],(val+11)*2+13); // Canh bao batt temp invalid
                        sDcInfo.sALARMDcInfo.u8Sen2_BattTemp = hex2byte(&RecvBuff[0],(val+14)*2+13); // Canh bao nhiet do acquy2 cao
                        sDcInfo.sALARMDcInfo.u8BatVol[1] = hex2byte(&RecvBuff[0],(val+13)*2+13);   // Canh bao bat 2 low voltage
                        sDcInfo.sALARMDcInfo.u8Sen2_BattInvalid = hex2byte(&RecvBuff[0],(val+16)*2+13); // Canh bao batt temp invalid
                        sDcInfo.sALARMDcInfo.u8Sen3_BattTemp = hex2byte(&RecvBuff[0],(val+19)*2+13); // Canh bao nhiet do acquy3 cao
                        sDcInfo.sALARMDcInfo.u8Sen3_BattInvalid = hex2byte(&RecvBuff[0],(val+21)*2+13); // Canh bao batt temp invalid
                        sDcInfo.sALARMDcInfo.u8DCSPD = hex2byte(&RecvBuff[0],(val+22)*2+13);
//									sDcInfo.sALARMDcInfo.u8Sen1_AmbTemp = hex2byte(&RecvBuff[0],63);
//									sDcInfo.sALARMDcInfo.u8Sen2_AmbTemp = hex2byte(&RecvBuff[0],65);
                        sDcInfo.sALARMDcInfo.u8Batt_Discharge = hex2byte(&RecvBuff[0],(val+26)*2+13);
//									sDcInfo.sALARMDcInfo.u8OutVoltFault = hex2byte(&RecvBuff[0],77);
//							sDcInfo.sALARMDcInfo.u8BatVol[0]=hex2byte(&RecvBuff[0],17);

                      }
                    }


                    RecvCntStart = 0;
                    MESGState = ENVALARM_REQ;
                    break;


                case  ENVALARM_REQ:
                    for (i = 0; i < REQenvalarmLen; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&REQenvalarm[i],1);
                    }
                    MESGState = ENVALARM_RES;
                    break;
                case  ENVALARM_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    if(RecvCount>6)
                    {
                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-6);
                        g_checksum = Checksum16(&RecvBuff[0],RecvCount-1);
                    }
                    if (g_checksum == g_testchecksum)
                    {
                      if(RecvBuff[2] == 0x30) // version 2.0
                      {
                        sAlarmEnvInfo.u8EnvHumidity = hex2byte(&RecvBuff[0],21);
                        sAlarmEnvInfo.u8EnvSmoke = hex2byte(&RecvBuff[0],25);
                        sAlarmEnvInfo.u8EnvWater = hex2byte(&RecvBuff[0],29);
                        sAlarmEnvInfo.u8EnvInfra = hex2byte(&RecvBuff[0],33);
                        sAlarmEnvInfo.u8EnvDoor = hex2byte(&RecvBuff[0],37);
                        sAlarmEnvInfo.u8EnvGlass = hex2byte(&RecvBuff[0],41);
                        sAlarmEnvInfo.u8HeatExch = hex2byte(&RecvBuff[0],47);
                      }
                      else // version 2.1
                      {
                        sAlarmEnvInfo.u8EnvTemp = hex2byte(&RecvBuff[0],49);
                        sAlarmEnvInfo.u8EnvHumidity = hex2byte(&RecvBuff[0],21);
                        sAlarmEnvInfo.u8EnvSmoke = hex2byte(&RecvBuff[0],25);
                        sAlarmEnvInfo.u8EnvWater = hex2byte(&RecvBuff[0],29);
                        sAlarmEnvInfo.u8EnvInfra = hex2byte(&RecvBuff[0],33);
                        sAlarmEnvInfo.u8EnvDoor = hex2byte(&RecvBuff[0],37);
                        sAlarmEnvInfo.u8EnvGlass = hex2byte(&RecvBuff[0],41);
                        sAlarmEnvInfo.u8HeatExch = hex2byte(&RecvBuff[0],47);
                      }
                    }


                    RecvCntStart = 0;
                    MESGState = ENVDATA_REQ;
                    break;
                case  ENVDATA_REQ:
                    for (i = 0; i < REQenvdataLen; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&REQenvdata[i],1);
                    }
                    MESGState = ENVDATA_RES;
                    break;
                case  ENVDATA_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    if(RecvCount>6)
                    {
                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-6);
                        g_checksum = Checksum16(&RecvBuff[0],RecvCount-1);
                    }
                    if (g_checksum == g_testchecksum)
                    {
                        if((sAlarmEnvInfo.u8EnvTemp!=0x05)&&(sAlarmEnvInfo.u8EnvTemp!=0xF5))
                        {
                            sDcInfo.sINFODcInfo.fSen1AmbTemp = hex2float(&RecvBuff[0],17);

                        }
                        else
                        {
                            sDcInfo.sINFODcInfo.fSen1AmbTemp=0;
                            sAlarmInfo.u32AmbTemp = 1;
                        }

                    }

                    RecvCntStart = 0;
                    MESGState = TIMEINFO_REQ;
                    break;
                case  TIMEINFO_REQ:
                    for (i = 0; i < REQtimeinfoLen; i++)
                    {

                        UART_WriteBlocking(RS2321_UART,&REQtimeinfo[i],1);
                    }
                    MESGState = TIMEINFO_RES;
                    break;
                case  TIMEINFO_RES:
                    for(i=0; i < RecvCntStart; i++)
                    {
                        RecvBuff[i]=g_UARTRxBuf[i];
                        g_UARTRxBuf[i] =0;
                    }
                    RecvCount = RecvCntStart;

                    if(RecvCount>6)
                    {
                        g_testchecksum = hex2byte2(&RecvBuff[0],RecvCount-6);
                        g_checksum = Checksum16(&RecvBuff[0],RecvCount-1);
                    }
                    if (g_checksum == g_testchecksum)
                    {
                       // nhan thong so thoi gian
                      if(( RecvBuff[7]==0x34)&&( RecvBuff[8]==0x44))
                      {
                         ZTETime.Year= hex2byte2(&RecvBuff[0],13);
                         ZTETime.Month = hex2byte(&RecvBuff[0],17);
                         ZTETime.Date = hex2byte(&RecvBuff[0],19);
                         ZTETime.Hour = hex2byte(&RecvBuff[0],21);
                         ZTETime.Minute = hex2byte(&RecvBuff[0],23);
                         ZTETime.Second = hex2byte(&RecvBuff[0],25);
                      }               
                    }

                    RecvCntStart = 0;
                    MESGState = UPDATE_OK;
                    break;
                case UPDATE_OK:
                    // ======================== Update dc information to SNMP resource ==========================//
                    privateMibBase.batteryGroup.battVolt = (int32_t) (sDcInfo.sINFODcInfo.fBatt1Volt * 100);
                    privateMibBase.batteryGroup.battCurr = (int32_t) ((sDcInfo.sINFODcInfo.fBatt1Curr + sDcInfo.sINFODcInfo.fBatt2Curr) * 100);
                    privateMibBase.batteryGroup.battCapLeft1 = (uint32_t) (sDcInfo.sINFODcInfo.fBatt1RmnCap * 100);
                    privateMibBase.batteryGroup.battTemp = (uint32_t) (sDcInfo.sINFODcInfo.fSen1BattTemp*10);
                    if(sDcInfo.sALARMDcInfo.u8Sen1_BattInvalid==0xF5)
                    {
//							privateMibBase.batteryGroup.battTemp = sDcInfo.sINFODcInfo.fSen2BattTemp*10;
                        privateMibBase.batteryGroup.battTemp =0;
                    }

                    privateMibBase.batteryGroup.battBanksNumofBanks = sDcInfo.sINFODcInfo.u8BatNo;

                    privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen = 0;
                    for (i = 0; i < 10; i++)
                    {
                        if(sDcInfo.sALARMDcInfo.u8LoadFuse[i] == 0x03)
                        {
                            fuse_flag = 1;
                            privateMibBase.loadGroup.loadStatus[i] = 1;
                        }
                        else
                        {
                            fuse_flag = 0;
                            privateMibBase.loadGroup.loadStatus[i] = 0;
                        }
                        privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen |= (fuse_flag << i);
                    }
                    if (privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen != 0)
                    {
                        privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen = 1;
                    }
                    else
                    {
                        privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen = 0;
                    }

                    privateMibBase.mainAlarmGroup.alarmBattBreakerOpen = 0;
                    for (i = 0; i < 4; i++)
                    {
                        if(sDcInfo.sALARMDcInfo.u8BattFuse[i] == 0x03)
                        {
                            fuse_flag = 1;
                            privateMibBase.batteryGroup.battBrkStatus[i] = 1;
                        }
                        else
                        {
                            fuse_flag = 0;
                            privateMibBase.batteryGroup.battBrkStatus[i] = 0;
                        }
                        privateMibBase.mainAlarmGroup.alarmBattBreakerOpen |= (fuse_flag << i);
                    }
                    if (privateMibBase.mainAlarmGroup.alarmBattBreakerOpen != 0)
                    {
                        privateMibBase.mainAlarmGroup.alarmBattBreakerOpen = 1;
                    }
                    else
                    {
                        privateMibBase.mainAlarmGroup.alarmBattBreakerOpen = 0;
                    }
                    if((sAcInfo.u8MainFail==0x05)||(sAcInfo.u8PowerOff==0xE0))
                    {
                        privateMibBase.mainAlarmGroup.alarmACmains =1;
                    }
                    else privateMibBase.mainAlarmGroup.alarmACmains =0;
                    if(sAcInfo.u8ACSPD==0x05)
                    {
                        privateMibBase.mainAlarmGroup.alarmACSPD =1;
                    }
                    else privateMibBase.mainAlarmGroup.alarmACSPD =0;
                    if(sDcInfo.sALARMDcInfo.u8DCSPD==0x05)
                    {
                        privateMibBase.mainAlarmGroup.alarmDCSPD =1;
                    }
                    else privateMibBase.mainAlarmGroup.alarmDCSPD =0;
                    if(sDcInfo.sALARMDcInfo.u8LLVD==0xF0)
                        privateMibBase.mainAlarmGroup.alarmLLVD = 1;
                    else privateMibBase.mainAlarmGroup.alarmLLVD = 0;
//						privateMibBase.mainAlarmGroup.alarmLLVD = sDcInfo.sALARMDcInfo.u8LLVD;
                    if(sDcInfo.sALARMDcInfo.u8BLVD == 0xF1)privateMibBase.mainAlarmGroup.alarmBLVD = 1;
                    else privateMibBase.mainAlarmGroup.alarmBLVD = 0;
//						privateMibBase.mainAlarmGroup.alarmBLVD = sDcInfo.sALARMDcInfo.u8BLVD;
                    if((sDcInfo.sALARMDcInfo.u8BatVol[0]==1)||(sDcInfo.sALARMDcInfo.u8BatVol[1]==1))
                        privateMibBase.mainAlarmGroup.alarmDCLow = 0x01;
                    else privateMibBase.mainAlarmGroup.alarmDCLow = 0x00;
                    privateMibBase.mainAlarmGroup.alarmBattHighTemp = 0;
                    if(sDcInfo.sALARMDcInfo.u8Sen1_BattTemp == 0x02)
                    {
                        privateMibBase.mainAlarmGroup.alarmBattHighTemp = 0x02;
                        privateMibBase.batteryGroup.battTemp = (uint32_t) (sDcInfo.sINFODcInfo.fSen1BattTemp * 10);
                    }
//						else if((sDcInfo.sALARMDcInfo.u8Sen1_BattInvalid!=0xF5)&&(sDcInfo.sALARMDcInfo.u8Sen1_BattInvalid!=0x05))
//						{
//							privateMibBase.mainAlarmGroup.alarmBattHighTemp = 0x00;
//							privateMibBase.batteryGroup.battTemp = sDcInfo.sINFODcInfo.fSen1BattTemp * 10;
//						}
//						if(sDcInfo.sALARMDcInfo.u8Sen2_BattTemp == 0x02)
//						{
//							privateMibBase.mainAlarmGroup.alarmBattHighTemp = 0x02;
//							privateMibBase.batteryGroup.battTemp = sDcInfo.sINFODcInfo.fSen2BattTemp * 10;
//						}
//						else if((sDcInfo.sALARMDcInfo.u8Sen2_BattInvalid!=0xF5)&&(sDcInfo.sALARMDcInfo.u8Sen2_BattInvalid!=0x05))
//						{
//							privateMibBase.mainAlarmGroup.alarmBattHighTemp = 0x00;
//							privateMibBase.batteryGroup.battTemp = sDcInfo.sINFODcInfo.fSen2BattTemp * 10;
//						}
//						if((sDcInfo.sALARMDcInfo.u8Sen1_BattInvalid==0xF5)&&(sDcInfo.sALARMDcInfo.u8Sen2_BattInvalid==0xF5))
//						{
//							privateMibBase.mainAlarmGroup.alarmBattHighTemp = 0x05;
//							privateMibBase.batteryGroup.battTemp = 0;
//						}


                    // ======================== Update dc information to SNMP resource ==========================//
                    // ======================== Update rect information to SNMP resource ==========================//
                    privateMibBase.mainAlarmGroup.alarmRectACFault = 0;
                    privateMibBase.mainAlarmGroup.alarmRectNoResp = 0;
                    
//                    sRectInfo.sSNMPRectParam[0].u32Rect_State = 0;
                    for(i=0; i<10; i++)
                    {
                        privateMibBase.rectGroup.rectTable[i].rectStatus = 0;
                        privateMibBase.mainAlarmGroup.alarmRectNoResp |= sRectInfo.sRAWRectParam[i].u8Rect_NoResp << i;
                        privateMibBase.mainAlarmGroup.alarmRectACFault |= sRectInfo.sRAWRectParam[i].u8Rect_Fail << i;
//							sRectInfo.sSNMPRectParam[i].u32Rect_Fail = sRectInfo.sRAWRectParam[i].u8Rect_Fail;
//                        sRectInfo.sSNMPRectParam[i].u32Rect_Out = sRectInfo.sRAWRectParam[i].u8Rect_Out;
                        if(sRectInfo.sRAWRectParam[i].u8Rect_Sts==1)
                        {
                            privateMibBase.rectGroup.rectTable[i].rectStatus = 0xFF; // Rect dang hoat dong
                        }
                        if(sRectInfo.sRAWRectParam[i].u8Rect_NoResp==1)
                        {
                            privateMibBase.rectGroup.rectTable[i].rectStatus = 0xAA; // Rect no respond
                        }
                        if(sRectInfo.sRAWRectParam[i].u8Rect_Out==1)
                        {
                            privateMibBase.rectGroup.rectTable[i].rectStatus = 0x0A; // Rect dc off
                        }
                        if(sRectInfo.sRAWRectParam[i].u8Rect_Fail==1)
                        {
                            privateMibBase.rectGroup.rectTable[i].rectStatus = 0xA5; // Rect fail
                        }

                        privateMibBase.rectGroup.rectTable[i].rectOutputCurrent = (uint32_t) (sRectInfo.sRAWRectParam[i].fRect_DcCurr * 100);
//							sRectInfo.sSNMPRectParam[i].u32Rect_Temp = sRectInfo.sRAWRectParam[i].fRect_Temp * 100;
//							if(privateMibBase.rectGroup.rectTable[i].rectOutputCurrent >0)
                        if(sRectInfo.sRAWRectParam[i].u8Rect_Sts==1)
                            privateMibBase.rectGroup.rectTable[i].rectOutputVoltage =(uint32_t) (sRectInfo.fAllRectDcVolt * 100);
                        else privateMibBase.rectGroup.rectTable[i].rectOutputVoltage =0;
//
                    }
                    if (privateMibBase.mainAlarmGroup.alarmRectNoResp != 0) privateMibBase.mainAlarmGroup.alarmRectNoResp = 1;
                    else privateMibBase.mainAlarmGroup.alarmRectNoResp = 0;
                    if (privateMibBase.mainAlarmGroup.alarmRectACFault != 0) privateMibBase.mainAlarmGroup.alarmRectACFault = 1;
                    else privateMibBase.mainAlarmGroup.alarmRectACFault = 0;
                    privateMibBase.rectGroup.rectTotalCurrent = 0;
                    for (i = 0; i < 10; i++)
                    {
                        privateMibBase.rectGroup.rectTotalCurrent += (uint32_t) (privateMibBase.rectGroup.rectTable[i].rectOutputCurrent);
                    }
                    privateMibBase.rectGroup.rectInstalledRect = 10;
                    privateMibBase.rectGroup.rectActiveRect = sRectInfo.u8Rect_Num;
                    
                    privateMibBase.loadGroup.loadCurrent = (uint32_t) (sDcInfo.sINFODcInfo.fCurrent*100);
//						privateMibBase.loadGroup.loadCurrent = privateMibBase.rectGroup.rectTotalCurrent - privateMibBase.batteryGroup.battCurr;
//						if (privateMibBase.loadGroup.loadCurrent >= 0) privateMibBase.loadGroup.loadCurrent = privateMibBase.loadGroup.loadCurrent;
//						else if (privateMibBase.loadGroup.loadCurrent < 0) privateMibBase.loadGroup.loadCurrent = -privateMibBase.loadGroup.loadCurrent;
//						privateMibBase.mainAlarmGroup.alarmManualMode = g_AlrmMan;

                    // ======================== Update rect information to SNMP resource ==========================//
                    // ======================== Update ac information to SNMP resource ==========================//
                    privateMibBase.acPhaseGroup.acPhaseTable[0].acPhaseVolt = (int32_t) (sAcInfo.facVolt[0] * 100);
                    privateMibBase.acPhaseGroup.acPhaseTable[1].acPhaseVolt = (int32_t) (sAcInfo.facVolt[1] * 100);
                    privateMibBase.acPhaseGroup.acPhaseTable[2].acPhaseVolt = (int32_t) (sAcInfo.facVolt[2] * 100);
                    if (sAcInfo.u8Thres[0] || sAcInfo.u8Thres[1] || sAcInfo.u8Thres[2])
                    {
                        privateMibBase.mainAlarmGroup.alarmACLow = 1;
                    }
                    else
                    {
                        privateMibBase.mainAlarmGroup.alarmACLow = 0;
                    }

                    // ======================== Update ac information to SNMP resource ==========================//

                    // ======================== Update Config information to SNMP resource ==========================//
                    privateMibBase.configGroup.cfgFloatVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fFltVoltCfg * 100);
                    privateMibBase.configGroup.cfgBoostVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fBotVoltCfg * 100);
                    privateMibBase.configGroup.cfgTempCompValue = (uint32_t) sBattInfo.sRAWBattInfo.fTempCompVal;
                    privateMibBase.configGroup.cfgLLVDVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg * 100);
                    privateMibBase.configGroup.cfgBLVDVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fLVDDV * 100);
                    privateMibBase.configGroup.cfgDCLowVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg * 100);
                    privateMibBase.configGroup.cfgBattTestVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fTestVoltCfg * 100);
                    
                    privateMibBase.cfgBTGroup.cfgTestVoltCfg = (uint32_t)(sBattInfo.sRAWBattInfo.fTestVoltCfg*10);
                    privateMibBase.cfgBTGroup.cfgAutoTestDay = (uint32_t)sBattInfo.sRAWBattInfo.fAutoTestDay;
                    privateMibBase.cfgBTGroup.cfgTestStartTime = (uint32_t)sBattInfo.sRAWBattInfo.fTestStartTime;
//						privateMibBase.batteryGroup.battBanksNumofBanks = sBattInfo.sRAWBattInfo.u8BankNo;
                    privateMibBase.configGroup.cfgBattCapacityTotal = (uint32_t) sBattInfo.sRAWBattInfo.fCapTotal;
                    privateMibBase.configGroup.cfgBattCapacityTotal2 = (uint32_t) sBattInfo.sRAWBattInfo.fCapTotal2;
                    privateMibBase.configGroup.cfgCurrentLimit = (uint32_t) (sBattInfo.sRAWBattInfo.fCCLVal * 1000);

                    sBattInfo.sRAWBattInfo.fOvMjTempAlrmLevel = 100;
                    privateMibBase.configGroup.cfgOverMajorTempLevel = (uint32_t) (sBattInfo.sRAWBattInfo.fOvMjTempAlrmLevel * 100);
                    privateMibBase.configGroup.cfgHighMajorTempLevel = (uint32_t) (sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel *100);

//						privateMibBase.configGroup.cfgWalkInTimeDuration = sRectInfo.u8WITI;
//						privateMibBase.configGroup.cfgWalkInTimeEn = sRectInfo.u8WITE;
                    privateMibBase.configGroup.cfgAcLowLevel = (uint32_t) sAcInfo.fAcLowThres;

                    // ======================== Update Config information to SNMP resource ==========================//
                    // ======================== Update Env information to SNMP resource ==========================//
                    privateMibBase.siteGroup.siteAmbientTemp = (uint32_t) (sDcInfo.sINFODcInfo.fSen1AmbTemp*10);
                    sAlarmInfo.u32AmbTemp = 0;
                    if(sAlarmEnvInfo.u8EnvTemp == 0x04)
                    {
                        sAlarmInfo.u32AmbTemp = 0x02;
                    }
                    if((sAlarmEnvInfo.u8EnvTemp == 0x05)||(sAlarmEnvInfo.u8EnvTemp == 0xF5))
                    {
                        sAlarmInfo.u32AmbTemp = 0x05;
                    }
                    privateMibBase.mainAlarmGroup.alarmSmoke = 0;
                    if(sAlarmEnvInfo.u8EnvSmoke==0x04)
                    {
                         privateMibBase.mainAlarmGroup.alarmSmoke = 0x01;
                    }
//						if(sAlarmEnvInfo.u8EnvSmoke==0x05)
//						{
//							sAlarmInfo.u32Smoke = 0x05;
//						}

                    sAlarmInfo.u32EnvDoor = 0;
                    if(sAlarmEnvInfo.u8EnvDoor==0x04)
                    {
                        sAlarmInfo.u32EnvDoor = 0x01;
                    }
//						if(sAlarmEnvInfo.u8EnvDoor==0x05)
//						{
//							sAlarmInfo.u32EnvDoor = 0x05;
//						}
                    sAlarmInfo.u32HeatExch = 0;
                    if(sAlarmEnvInfo.u8HeatExch==0x04)
                    {
                        sAlarmInfo.u32HeatExch = 0x01;
                    }
//						if(sAlarmEnvInfo.u8HeatExch==0x05)
//						{
//							sAlarmInfo.u32HeatExch = 0x05;
//						}
                    privateMibBase.diAlarmGroup.alarmDigitalInput[0] = sAlarmInfo.u32HeatExch;

                    // ======================== Update Env information to SNMP resource ==========================//                            
                    MESGState = ACINPUTINFO_REQ;
                    break;
                default:
                    break;
                }
                if(g_testchecksum != g_checksum)
                {
                    reverror++;
                    MESGStatetest[k] =  MESGState;
                    k++;
                    if(k>19) k =0;
                }



            }
        }


        vTaskDelay(400);
    }
    while (1);
    vTaskSuspend(NULL);
}


