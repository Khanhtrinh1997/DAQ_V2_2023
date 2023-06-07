#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "Header.h"
#if (USERDEF_RS485_AGISSON == ENABLED)
uint32_t iSet =0;
//uint32_t WD1_Cnt = 0;
uint8_t Program_Status =0;
uint8_t Program_Status_old =0;
uint8_t  writeFlash=0;
uint32_t RS485Timer =0;
uint32_t RS485Command =1;
//uint32_t resetWD = 1;
unsigned char g_UARTRxBuf485[300];
uint32_t RecvCntStart485=0;
uint32_t RecvCntEnd485=0;
uint32_t RecvOK=0;
uint32_t RecvCodeOK =0;      
uint8_t u8HisBuffer[256];
uint32_t SetupFlagComplete =1;
uint32_t setuptimer =5000;
uint32_t setuprepeat =0;

uint32_t HW_cnt=0;
void Agisson_Data_Process(void *pvParameters)
{
  uint32_t i;
  uint32_t iSet =0;
  for(;;)
  {
    HW_cnt++;
    privateMibBase.cntGroup.HWcnt=HW_cnt;
    privateMibBase.cntGroup.HWState =Program_Status;
    privateMibBase.cntGroup.HWRs485Cmd =RS485Command;
    
    
//    WD1_Cnt++;
//    if (sSiteInfo.u32RemoteReboot == 0x53)
//    {
//       resetWD = 22;
//    }
//    else resetWD = 1;
//    if(WD1_Cnt >= (resetWD * 2))
//    {
//      WDOG_Refresh(wdog_base);
//      WD1_Cnt = 0;
//    }
       setuptimer++;
       RS485Timer++;
             if(setuptimer < 6000)
               {
                  SetupFlagComplete =0;
               }
             else 
               {
                 setuptimer =6500;
                 if(setuptimer>10000)setuptimer=6500;
                 SetupFlagComplete =1;
               }
    if (Program_Status != 0)
    {
      privateMibBase.connAlarmGroup.alarmPMUConnect = 0;
    } 
    else
    {
      privateMibBase.connAlarmGroup.alarmPMUConnect = 1;    
    }
  
   if((privateMibBase.connAlarmGroup.alarmPMUConnect_old3 ==0)&& (privateMibBase.connAlarmGroup.alarmPMUConnect !=0))
   {
     if(Uart_data_get.Year!=0)
       {
        sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =29;
        sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Year  =   Uart_data_get.Year;
        sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Month =   Uart_data_get.Month;
        sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Day   =   Uart_data_get.Day;
        sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hour  =   Uart_data_get.Hour;
        sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Minute=   Uart_data_get.Minute;
        NextHis();
        privateMibBase.connAlarmGroup.alarmPMUConnect_old3 = privateMibBase.connAlarmGroup.alarmPMUConnect;
        HisCount_old[ih] = sHisFlashLog[ih].Count;
       }

   }
   if((privateMibBase.connAlarmGroup.alarmPMUConnect_old3 !=0)&& (privateMibBase.connAlarmGroup.alarmPMUConnect ==0))
   {
     if(Uart_data_get.Year!=0)
       {
        sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =30;
        sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Year  =   Uart_data_get.Year;
        sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Month =   Uart_data_get.Month;
        sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Day   =   Uart_data_get.Day;
        sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hour  =   Uart_data_get.Hour;
        sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Minute=   Uart_data_get.Minute;
        NextHis();
        privateMibBase.connAlarmGroup.alarmPMUConnect_old3 = privateMibBase.connAlarmGroup.alarmPMUConnect;

       }
   }
    if(MESGState == HISTORYMESG_REQ)             //
    {
       GetHistoryLog();
       MESGState = HISTORYMESG_RES;
    }
    if(Program_Status ==0)
    {
      setCmd_Ready =0;
      Send_StartMesg0(RS485Timer);
      if(RS485Timer>1000)
      {
        RX485Count=0;
        RecvCntStart485=0;
//        GPIO_SetPinsOutput(GPIOD,1u << 1u);
//
//        UART_9Bit_Send(RS485T_UART,0x23,1);
//        delay();
//        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
        RS485Timer=0;
        RS485Command++;
        if(RS485Command>40) RS485Command =0;

      }    
    }
    if(Program_Status ==1)
    {
      setCmd_Ready =0;
      if(RS485Timer >1000)
       {
          RS485Command++;
          RS485Timer =0;
       }
      if(RS485Timer ==600)
       {
          Recv_Mesg();
       }
      
      switch(RS485Command)
      {
        
        case 1:
          Send_StartMesg1(RS485Timer);
          break;
        case 2:
          Send_StartMesg23(RS485Timer);
          break;
        case 3:
          Send_StartMesg23(RS485Timer);
          break;
        case 4:
          Send_StartMesg4(RS485Timer);
          break;
        case 5:
          if(CPMU01_Ver_IP ==1){Send_StartMesg10(RS485Timer);}
          else {Send_StartMesg56789(RS485Timer);}
//          Send_StartMesg56789(RS485Timer);
          break;
        case 6:
          Send_StartMesg56789(RS485Timer);
          break;
        case 7:
          Send_StartMesg56789(RS485Timer);
          break;
        case 8:
          Send_StartMesg56789(RS485Timer);
          break;
        case 9:
          Send_StartMesg56789(RS485Timer);
          break;
        case 10:
          Send_StartMesg10(RS485Timer);
          break;
        default:
          break;
      }
      if(CPMU01_Ver_IP ==1){
        if(RS485Command >5)
        {
          Program_Status =2;
          RS485Command =0;
          RS485Timer =0;
        }
      }
      else {
        
        if(RS485Command >10)
        {
          Program_Status =2;
          RS485Command =0;
          RS485Timer =0;
        }
      }  
    }
    if(Program_Status ==2)
    { 
//      if(  setCmd_flag == 1)
//      {
//        thao++;
//      }

      if ((setCmd_flag == 1)&&(setCmd_Ready==2)&&(RecvComplete==1))            //&&(RS485Command <31)
       {
         setuptimer =0;
         settingCommandmask |= setCmd_mask;
         setCmd_mask=0;
         for(i=0;i<32;i++)
           {
             if(settingCommand !=0)
               {
                       i=32;
               }
             else
             {
                settingCommand = settingCommandmask & (0x00000001<<i); 
             }
           }
         
           
           switch(settingCommand)
             {
              case SET_FLTVOL:
               SetFloatVolt();
                break;
              case SET_BSTVOL:
                SetBoostVolt();
                break;
              case SET_CCL:
                SetCCL();
                break;
              case SET_BATTSTDCAP:
                SetBattCap1();
                break;
              case SET_BATTSTDCAP2:
                SetBattCap2();
                break;
                
              case SET_LLVD_EN:
                SetLLVD_EN();
                break;
              case SET_BLVD_EN:
                SetBLVD_EN();
                break;
                
              case SET_LLVD:
                SetLLVD();
                break;
              case SET_BLVD:
                SetBLVD();
                break;
              case SET_TEMPCOMP_VAL:
                SetTempComVal();
                break;
              case SET_OVERTEMP:
                 SetOverTemp();
                break;
              case SET_DCLOW:
                SetDClow();
                break;
              case SET_AC_THRES:
                SetACLow();
                break;
             case SET_BT_ENDVOLT:
               SetBattTestEndVolt();
              break;
             case SET_BT_CELLNO:
               SetBattStdCellNo();
               break;
             case SET_BT_ENDTIME:
               SetBattStdTestTime();
               break;
    //         case SET_BATT_SHORT_TEST_END_VOLT:
    //           SetBattShortTestEndVolt();
    //           break;
    //         case SET_BATT_SHORT_TEST_TIME:
    //           SetBattShortTestTime();
    //           break;  
             case START_BT_TEST:
               Start_Batt_Manual_Test();
               break;
    //         case START_BATT_SHORT_TEST:
    //           Start_Batt_Short_Test();
    //           break;
             case STOP_BT_TEST:
               Set_Batt_Test_Stop();
               break;
             default:
               break;
                 
             }
                  
       }
      else //if (((setCmd_flag == 0)||(setCmd_Ready!=2)))   //||((RS485Command >30)&&(setCmd_flag == 1))
       {
         if(RS485Timer > 200)
         {
            if(RecvOK ==1)
            {
            RS485Command++;
            RS485Timer =0;
            }
            else 
            {
                if(RS485Timer>300)
                {
                RS485Command++;
                RS485Timer =0;
                }
            }
            if(CPMU01_Ver_IP ==1)
              {
                if((RS485Command>3)&&(RS485Command<9))RS485Command=9;
              }

         }
         if(RS485Command > 40) 
         {
           RS485Command =0;
           setCmd_Ready++;
           if(setCmd_Ready>2)setCmd_Ready=2;
         }
         if(RS485Command < 31)
         {
           Send_RequestMesg(RS485Timer, RS485Command);
           if(RS485Timer ==150)
           {
             if(CPMU01_Ver_IP ==1)
               {
                 if(SetupFlagComplete ==1)
                 {
                    Recv_Mesg();
                 }
               }
             else
               {
                     Recv_Mesg();
               }
           }
         }
         if(RS485Command==32)
         {
            if(writeFlash>10)
              {
                writeFlash=0;
                for(i=0;i<5;i++)
                {
                   
                   if(sHisFlashLog[i].Count_old< sHisFlashLog[i].Count)
                  {  
                    pHisFlashLog =    &sHisFlashLog[i];
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
                    sHisFlashLog[i].Count_old = sHisFlashLog[i].Count;

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
         }
       } 
    }
     vTaskDelay(2); 
  }
}



#endif
//DKD51 BDP data process======================================================================

#if (USERDEF_RS485_DKD51_BDP == ENABLED)
uint32_t DKD51_BDP_cnt=0;
uint8_t  flag_test=0;

DPC_T dpc_t;
uint8_t buff_tesst[20];
void DKD51_BDP_data_process(void *pvParameters)
{
  dpc_t.check_sum_calculator = 0;
  dpc_t.check_sum_read       = 0xFFFF;  
  MESGState                  = ACINPUTINFO_REQ;    
  for(;;){
    //===========================================================
    //code here
    DKD51_BDP_cnt++;
  
    //CMD set -----------------------------------------------------------
    if(setCmd_flag == 1){
      settingCommand |= setCmd_mask;
      
      switch(settingCommand){
        //---------------------------------------------------------------
        case SET_FLTVOL:
          DKD51_set_float_volt();
          break;
        //---------------------------------------------------------------
        case SET_BSTVOL:
          DKD51_set_boost_volt();
          break;
        //---------------------------------------------------------------
        case SET_TEMPCOMP_VAL:
          DKD51_set_temp_compensation();
          break;
        //---------------------------------------------------------------
        case SET_LLVD:
          break;
        //---------------------------------------------------------------
        case SET_BLVD:
          break;
        //---------------------------------------------------------------
        case SET_DCOVER:
          
          DKD51_set_dc_over();
          break;
        //---------------------------------------------------------------
        case SET_DCLOW:
          DKD51_set_dc_low();
          break;
        //---------------------------------------------------------------
        case SET_BATTSTDCAP:
          DKD51_set_batt_capacity_1();
          break;
        //---------------------------------------------------------------
        case SET_BATTSTDCAP2:
          DKD51_set_batt_capacity_2();
          break;
        //---------------------------------------------------------------
        case SET_BATTSTDCAP3:
          DKD51_set_batt_capacity_3();
          break;
        //---------------------------------------------------------------
        case SET_BATTSTDCAP4:
          DKD51_set_batt_capacity_4();
          break;
        //---------------------------------------------------------------
        case SET_CCL:
          DKD51_set_charge_current_limit();
          break;
        //---------------------------------------------------------------
        case SET_OVERTEMP://space
          break;
        //---------------------------------------------------------------
        case SET_AC_LOW_THRES:
          DKD51_set_ac_low();
          break;
        //---------------------------------------------------------------
        case SET_AC_HIGH_THRES:
          DKD51_set_ac_high();
          break;
        //---------------------------------------------------------------
        case SET_AC_INPUT_CURR_LIMIT://space
          break;
        //--------------------------------------------------------------
        default:
          break; 
      }
      setCmd_mask = 0;
      setCmd_flag = 0;
      settingCommand = 0;
    }else if(setCmd_flag == 0){
      //check disconnect-------------------------------------------------
      if(PMUConnectCount++>30){
        PMUConnectCount =50;
        privateMibBase.connAlarmGroup.alarmPMUConnect = 1;
      }
//      //thanhcm3 test====================================================
//      //1.------------------------------------------
//      if(flag_test == 20){
//        flag_test =0;
//        DKD51_clearn_buff(&dpc_t);
//      }
//      //1.------------------------------------------
//      if(flag_test==1){
//        DKD51_clearn_buff(&dpc_t);
//        flag_test =0;
//        DKD51_send_message_resquest((uint8_t*)&DKD51_REQ_ac_input_info[0],(uint8_t)DKD51_REQ_ac_input_info_len);
//      }
//      //2.------------------------------------------
//      if(flag_test==2){
//        DKD51_clearn_buff(&dpc_t);
//        flag_test =0;
//        DKD51_send_message_resquest((uint8_t*)&DKD51_REQ_ac_parameter[0],(uint8_t)DKD51_REQ_ac_parameter_len);
//      }
//      //3.-----------------------------------------
//      if(flag_test==3){
//        DKD51_clearn_buff(&dpc_t);
//        flag_test =0;
//        DKD51_send_message_resquest((uint8_t*)&DKD51_REQ_ac_alarm[0],(uint8_t)DKD51_REQ_ac_alarm_len);
//      }
//      //4.----------------------------------------
//      if(flag_test==4){
//        DKD51_clearn_buff(&dpc_t);
//        flag_test =0;
//        DKD51_send_message_resquest((uint8_t*)&DKD51_REQ_rect_sts[0],(uint8_t)DKD51_REQ_rect_sts_len);
//      }
//      //5.----------------------------------------
//      if(flag_test==5){
//        DKD51_clearn_buff(&dpc_t);
//        flag_test =0;
//        DKD51_send_message_resquest((uint8_t*)&DKD51_REQ_rect_info[0],(uint8_t)DKD51_REQ_rect_info_len);
//      }
//      //6.-----------------------------------------
//      if(flag_test==6){
//        DKD51_clearn_buff(&dpc_t);
//        flag_test =0;
//        DKD51_send_message_resquest((uint8_t*)&DKD51_REQ_rect_alarm[0],(uint8_t)DKD51_REQ_rect_alarm_len);
//      }
//      //7.-----------------------------------------
//      if(flag_test==7){
//        DKD51_clearn_buff(&dpc_t);
//        flag_test =0;
//        DKD51_send_message_resquest((uint8_t*)&DKD51_REQ_dc_output[0],(uint8_t)DKD51_REQ_dc_output_len);
//      }
//      //8.-----------------------------------------
//      if(flag_test==8){
//        DKD51_clearn_buff(&dpc_t);
//        flag_test =0;
//        DKD51_send_message_resquest((uint8_t*)&DKD51_REQ_dc_parameter[0],(uint8_t)DKD51_REQ_dc_parameter_len);
//      }
//      //9.-----------------------------------------
//      if(flag_test==9){
//        DKD51_clearn_buff(&dpc_t);
//        flag_test =0;
//        DKD51_send_message_resquest((uint8_t*)&DKD51_REQ_dc_alarm[0],(uint8_t)DKD51_REQ_dc_alarm_len);
//      }
//      
//      //=================================================================
      //question DKD51---------------------------------------------------------------------------------------------
      switch(MESGState){
        // AC MESSAGE=====================================================================================
        case ACINPUTINFO_REQ://ac info request---------------------------------------------------------------------
          DKD51_send_message_resquest((uint8_t*)&DKD51_REQ_ac_input_info[0],(uint8_t)DKD51_REQ_ac_input_info_len);
          MESGState = ACINPUTINFO_RES;
          break;
          
        case ACINPUTINFO_RES://ac info response--------------------------------------------------------------------
          if(DKD51_check_sum(&dpc_t) == NO_ERR){
            sDcInfo.u32DCNoResponse      = 0;
            privateMibBase.connAlarmGroup.alarmPMUConnect = 0;
            if(DKD51_check_RTN(&dpc_t) == NO_ERR){
              sAcInfo.facVolt[0]           = ((float)hex2byte2(&dpc_t.rx.buff_485[0],19))/100;
              sAcInfo.facVolt[1]           = ((float)hex2byte2(&dpc_t.rx.buff_485[0],23))/100;
              sAcInfo.facVolt[2]           = ((float)hex2byte2(&dpc_t.rx.buff_485[0],27))/100;
              sAcInfo.f_hz_in              = ((float)hex2byte2(&dpc_t.rx.buff_485[0],31))/100;
              sAcInfo.f_ac_curr[0]         = ((float)hex2byte2(&dpc_t.rx.buff_485[0],79))/100;
              sAcInfo.f_ac_curr[1]         = ((float)hex2byte2(&dpc_t.rx.buff_485[0],83))/100;
              sAcInfo.f_ac_curr[2]         = ((float)hex2byte2(&dpc_t.rx.buff_485[0],87))/100;    
              flag_test = 10;
            }
          }else{
            dpc_t.check_sum_calculator = 0;
            dpc_t.check_sum_read       = 0xFFFF;
            sDcInfo.u32DCNoResponse++;
            if(sDcInfo.u32DCNoResponse > 3){
              privateMibBase.connAlarmGroup.alarmPMUConnect = 1;
              sDcInfo.u32DCNoResponse = 10;
                      // xoa khi checksum sai
                      sAcInfo.facVolt[0] = 0;
                      sAcInfo.facVolt[1] = 0;
                      sAcInfo.facVolt[2] = 0;
                      sAcInfo.f_hz_in    = 0;
                      sAcInfo.f_ac_curr[0]= 0;
                      sAcInfo.f_ac_curr[1]= 0;
                      sAcInfo.f_ac_curr[2]= 0;
                      sAcInfo.fAcHighThres = 0;
                      sAcInfo.fAcLowThres = 0;
                      sAcInfo.fAcInputCurrLimit = 0;
                      sRectInfo.fAllRectDcVolt  = 0;
                      sAcInfo.f_ac_high_hz      = 0;  
                      sAcInfo.f_ac_low_hz       = 0;
                      for(uint8_t i=0; i<24; i++)
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
          dpc_t.uart_rx.cnt =0;
          MESGState = ACPARAMETER_REQ;
          break;
          
        case ACPARAMETER_REQ://ac parametter request------------------------------------------------------------
          DKD51_send_message_resquest((uint8_t*)&DKD51_REQ_ac_parameter[0],(uint8_t)DKD51_REQ_ac_parameter_len);
          MESGState = ACPARAMETER_RES;
          break;
          
        case ACPARAMETER_RES://ac parametter response------------------------------------------------------------
          if(DKD51_check_sum(&dpc_t) == NO_ERR){
            if(DKD51_check_RTN(&dpc_t) == NO_ERR){
              sAcInfo.fAcHighThres         = ((float)hex2byte2(&dpc_t.rx.buff_485[0],13))/100;
              sAcInfo.fAcLowThres          = ((float)hex2byte2(&dpc_t.rx.buff_485[0],17))/100;
              sAcInfo.fAcInputCurrLimit    = ((float)hex2byte2(&dpc_t.rx.buff_485[0],21))/100;
              sAcInfo.f_ac_high_hz         = ((float)hex2byte2(&dpc_t.rx.buff_485[0],25))/100;
              sAcInfo.f_ac_low_hz          = ((float)hex2byte2(&dpc_t.rx.buff_485[0],29))/100;
              flag_test = 11;
            }
          }else{
            dpc_t.check_sum_calculator = 0;
            dpc_t.check_sum_read       = 0xFFFF;
            
          }
          dpc_t.uart_rx.cnt =0;
          MESGState = ACALARM_REQ;
          break;
          
        case ACALARM_REQ://ac alarm request--------------------------------------------------------------
          DKD51_send_message_resquest((uint8_t*)&DKD51_REQ_ac_alarm[0],(uint8_t)DKD51_REQ_ac_alarm_len);
          MESGState = ACALARM_RES;
          break;
          
        case ACALARM_RES://ac alarm response-------------------------------------------------------------
          if(DKD51_check_sum(&dpc_t) == NO_ERR){
            if(DKD51_check_RTN(&dpc_t) == NO_ERR){
              sAcInfo.u8Thres[0]           = hex2byte(&dpc_t.rx.buff_485[0],19);
              sAcInfo.u8Thres[1]           = hex2byte(&dpc_t.rx.buff_485[0],21);
              sAcInfo.u8Thres[2]           = hex2byte(&dpc_t.rx.buff_485[0],23);
              sAcInfo.u8_hz                = hex2byte(&dpc_t.rx.buff_485[0],25);  
              sAcInfo.u8_I_Thres[0]        = hex2byte(&dpc_t.rx.buff_485[0],55);
              sAcInfo.u8_I_Thres[1]        = hex2byte(&dpc_t.rx.buff_485[0],57);
              sAcInfo.u8_I_Thres[2]        = hex2byte(&dpc_t.rx.buff_485[0],59);
              sAcInfo.u8MainFail           = hex2byte(&dpc_t.rx.buff_485[0],31);
              sAcInfo.u8ACSPD              = hex2byte(&dpc_t.rx.buff_485[0],33);
              flag_test = 12;
            }
          }else{
            dpc_t.check_sum_calculator = 0;
            dpc_t.check_sum_read       = 0xFFFF;
            
          }
          dpc_t.uart_rx.cnt =0;
          MESGState = RECTSTSZTE_REQ;
          break;
          
        //RECT MESSAGE==================================================================================
        case RECTSTSZTE_REQ://rect status request--------------------------------------------------------
          DKD51_send_message_resquest((uint8_t*)&DKD51_REQ_rect_sts[0],(uint8_t)DKD51_REQ_rect_sts_len);
          MESGState = RECTSTSZTE_RES;
          break;
          
        case RECTSTSZTE_RES://rect status response--------------------------------------------------------
          if(DKD51_check_sum(&dpc_t) == NO_ERR){
            if(DKD51_check_RTN(&dpc_t) == NO_ERR){
              dpc_t.rect_number =0;
              for (uint8_t i=0; i<6;i++){
                sRectInfo.sRAWRectParam[i].u8Rect_OnPosition = hex2byte(&dpc_t.rx.buff_485[0],25+12*i);
                if(sRectInfo.sRAWRectParam[i].u8Rect_OnPosition == 0x00){  // 0x00H: rect installed; 0x80H: rect uninstalled
                  sRectInfo.sRAWRectParam[i].u8Rect_Sts = hex2byte(&dpc_t.rx.buff_485[0],17+12*i); // 0x00H: rect on; 0x01H: rect off
                  sRectInfo.sRAWRectParam[i].rect_sts_current_limit = hex2byte(&dpc_t.rx.buff_485[0],19+12*i);//0x00H:current limit; 0x01H: no current limit
                  sRectInfo.sRAWRectParam[i].rect_sts_fl_eq_testing = hex2byte(&dpc_t.rx.buff_485[0],21+12*i);//0x00H:FL; 0x01H: EQ ;0x02H: TESTING;
                  dpc_t.rect_number++;
                }
                else{
                  sRectInfo.sRAWRectParam[i].u8Rect_Sts =1;
                }
              }
               sRectInfo.u8Rect_Num = dpc_t.rect_number;
              flag_test = 13;
            }
          }else{
            dpc_t.check_sum_calculator = 0;
            dpc_t.check_sum_read       = 0xFFFF;
            
          }
          dpc_t.uart_rx.cnt =0;
          MESGState = RECTINFOZTE_REQ;
          break;
          
        case RECTINFOZTE_REQ://rect info request----------------------------------------------------------
          DKD51_send_message_resquest((uint8_t*)&DKD51_REQ_rect_info[0],(uint8_t)DKD51_REQ_rect_info_len);
          MESGState = RECTINFOZTE_RES;
          break;
          
        case RECTINFOZTE_RES://rect info response----------------------------------------------------------
          if(DKD51_check_sum(&dpc_t) == NO_ERR){
            if(DKD51_check_RTN(&dpc_t) == NO_ERR){
              sRectInfo.fAllRectDcVolt = ((float)hex2byte2(&dpc_t.rx.buff_485[0],15))/100;
              dpc_t.var = hex2byte(&dpc_t.rx.buff_485[0],19);
              for(uint8_t i=0; i<dpc_t.var;i++){
                sRectInfo.sRAWRectParam[i].fRect_DcVolt = sRectInfo.fAllRectDcVolt; // dung tam DCall
                sRectInfo.sRAWRectParam[i].fRect_DcCurr = ((float)hex2byte2(&dpc_t.rx.buff_485[0],21+6*i))/100;
              }
              flag_test = 14;
            }
          }else{
            dpc_t.check_sum_calculator = 0;
            dpc_t.check_sum_read       = 0xFFFF;
            
          }
          dpc_t.uart_rx.cnt =0;
          MESGState = RECTALARM_REQ;
          break;
          
        case RECTALARM_REQ://rect alarm request-------------------------------------------------------------
          DKD51_send_message_resquest((uint8_t*)&DKD51_REQ_rect_alarm[0],(uint8_t)DKD51_REQ_rect_alarm_len);
          MESGState = RECTALARM_RES;
          break;
          
        case RECTALARM_RES://rect alarm response-------------------------------------------------------------
          if(DKD51_check_sum(&dpc_t) == NO_ERR){
            if(DKD51_check_RTN(&dpc_t) == NO_ERR){
              dpc_t.var = hex2byte(&dpc_t.rx.buff_485[0],15);
              for(uint8_t i=0; i<dpc_t.var;i++){
                sRectInfo.sRAWRectParam[i].u8Rect_Fail           = hex2byte(&dpc_t.rx.buff_485[0],17+18*i);//0x00H:OK; 0x01H:fault
                sRectInfo.sRAWRectParam[i].rect_dc_over_volt     = hex2byte(&dpc_t.rx.buff_485[0],21+18*i);//0x00H:OK; 0x80H:fault
                sRectInfo.sRAWRectParam[i].rect_ac_over_volt     = hex2byte(&dpc_t.rx.buff_485[0],23+18*i);//0x00H:OK; 0x81H:fault
                sRectInfo.sRAWRectParam[i].rect_fan_fault        = hex2byte(&dpc_t.rx.buff_485[0],25+18*i);//0x00H:OK; 0x82H:fault
                sRectInfo.sRAWRectParam[i].rect_over_temp        = hex2byte(&dpc_t.rx.buff_485[0],27+18*i);//0x00H:OK; 0x83H:fault
                sRectInfo.sRAWRectParam[i].rect_dc_over_curr     = hex2byte(&dpc_t.rx.buff_485[0],29+18*i);//0x00H:OK; 0x84H:fault
                sRectInfo.sRAWRectParam[i].u8Rect_NoResp         = hex2byte(&dpc_t.rx.buff_485[0],31+18*i);//0x00H:OK; 0x85H:fault
                if(sRectInfo.sRAWRectParam[i].u8Rect_NoResp == 0) sRectInfo.sRAWRectParam[i].u8Rect_NoResp = 0;
                else sRectInfo.sRAWRectParam[i].u8Rect_NoResp =1;
              }
              flag_test = 15;
            }
          }else{
            dpc_t.check_sum_calculator = 0;
            dpc_t.check_sum_read       = 0xFFFF;
            
          }
          dpc_t.uart_rx.cnt =0;
          MESGState = DCOUTPUT_REQ;
          break;
          
        //DC MESSAGE===================================================================================
        case DCOUTPUT_REQ://dc ouput info request-------------------------------------------------------
          DKD51_send_message_resquest((uint8_t*)&DKD51_REQ_dc_output[0],(uint8_t)DKD51_REQ_dc_output_len);
          MESGState = DCOUTPUT_RES;
          break;
          
        case DCOUTPUT_RES://dc ouput info response------------------------------------------------------
          if(DKD51_check_sum(&dpc_t) == NO_ERR){
            if(DKD51_check_RTN(&dpc_t) == NO_ERR){
              sDcInfo.sINFODcInfo.fVoltage      = ((float)hex2byte2(&dpc_t.rx.buff_485[0],17))/100;
              sDcInfo.sINFODcInfo.fCurrent      = ((float)hex2byte2(&dpc_t.rx.buff_485[0],21))/100;
              
              sDcInfo.sINFODcInfo.fBatt1Curr    = ((float)iiihex2byte2(&dpc_t.rx.buff_485[0],27))/100;
              sDcInfo.sINFODcInfo.fBatt2Curr    = ((float)iiihex2byte2(&dpc_t.rx.buff_485[0],31))/100;
              sDcInfo.sINFODcInfo.fBatt3Curr    = ((float)iiihex2byte2(&dpc_t.rx.buff_485[0],35))/100;
              sDcInfo.sINFODcInfo.fBatt4Curr    = ((float)iiihex2byte2(&dpc_t.rx.buff_485[0],39))/100;
              
              sDcInfo.sINFODcInfo.fBatt1Volt    = ((float)hex2byte2(&dpc_t.rx.buff_485[0],207))/100;
              sDcInfo.sINFODcInfo.fBatt2Volt    = ((float)hex2byte2(&dpc_t.rx.buff_485[0],211))/100;
              sDcInfo.sINFODcInfo.fBatt3Volt    = ((float)hex2byte2(&dpc_t.rx.buff_485[0],215))/100;
              sDcInfo.sINFODcInfo.fBatt4Volt    = ((float)hex2byte2(&dpc_t.rx.buff_485[0],219))/100;
              
              sDcInfo.sINFODcInfo.fSen1BattTemp = ((float)hex2byte2(&dpc_t.rx.buff_485[0],151))/100;
              sDcInfo.sINFODcInfo.fSen2BattTemp = ((float)hex2byte2(&dpc_t.rx.buff_485[0],155))/100;
              sDcInfo.sINFODcInfo.fSen3BattTemp = ((float)hex2byte2(&dpc_t.rx.buff_485[0],159))/100;
              sDcInfo.sINFODcInfo.fSen4BattTemp = ((float)hex2byte2(&dpc_t.rx.buff_485[0],163))/100;
              
              sDcInfo.sINFODcInfo.fBatt1RmnCap  = ((float)hex2byte2(&dpc_t.rx.buff_485[0],131))/100;
              sDcInfo.sINFODcInfo.fBatt2RmnCap  = ((float)hex2byte2(&dpc_t.rx.buff_485[0],135))/100;
              sDcInfo.sINFODcInfo.fBatt3RmnCap  = ((float)hex2byte2(&dpc_t.rx.buff_485[0],139))/100;
              sDcInfo.sINFODcInfo.fBatt4RmnCap  = ((float)hex2byte2(&dpc_t.rx.buff_485[0],143))/100;
              
              
              flag_test = 16;
            }
          }else{
            dpc_t.check_sum_calculator = 0;
            dpc_t.check_sum_read       = 0xFFFF;
            
          }
          dpc_t.uart_rx.cnt =0;
          MESGState = DCPARAMETER_REQ;
          break;
          
        case DCPARAMETER_REQ://dc parameter request-----------------------------------------------------------
          DKD51_send_message_resquest((uint8_t*)&DKD51_REQ_dc_parameter[0],(uint8_t)DKD51_REQ_dc_parameter_len);
          MESGState = DCPARAMETER_RES;
          break;
          
        case DCPARAMETER_RES://dc parameter response----------------------------------------------------------
          if(DKD51_check_sum(&dpc_t) == NO_ERR){
            if(DKD51_check_RTN(&dpc_t) == NO_ERR){
              
              sBattInfo.sRAWBattInfo.fDCOverCfg         = ((float)hex2byte2(&dpc_t.rx.buff_485[0],13))/100;
              sBattInfo.sRAWBattInfo.fFltVoltCfg        = ((float)hex2byte2(&dpc_t.rx.buff_485[0],31))/100;
              sBattInfo.sRAWBattInfo.fBotVoltCfg        = ((float)hex2byte2(&dpc_t.rx.buff_485[0],35))/100;
            //sBattInfo.sRAWBattInfo.fTestVoltCfg       = ((float)hex2byte2(&dpc_t.rx.buff_485[0],143))/100;
              sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg   = ((float)hex2byte2(&dpc_t.rx.buff_485[0],17))/100;
              sBattInfo.sRAWBattInfo.fCapCCLVal         = ((float)hex2byte2(&dpc_t.rx.buff_485[0],23))/100;// current limit A
              sBattInfo.sRAWBattInfo.fTempCompVal       = ((float)hex2byte2(&dpc_t.rx.buff_485[0],51))/100;
              
              sBattInfo.sRAWBattInfo.fCapTotal          = ((float)hex2byte2(&dpc_t.rx.buff_485[0],59))/100;
              sBattInfo.sRAWBattInfo.fCapTotal2         = ((float)hex2byte2(&dpc_t.rx.buff_485[0],63))/100;
              sBattInfo.sRAWBattInfo.fCapTotal3         = ((float)hex2byte2(&dpc_t.rx.buff_485[0],67))/100;
              sBattInfo.sRAWBattInfo.fCapTotal4         = ((float)hex2byte2(&dpc_t.rx.buff_485[0],71))/100;
              sBattInfo.sRAWBattInfo.fCapTotal_all      = sBattInfo.sRAWBattInfo.fCapTotal+ sBattInfo.sRAWBattInfo.fCapTotal2
                                                          + sBattInfo.sRAWBattInfo.fCapTotal3+sBattInfo.sRAWBattInfo.fCapTotal4;
              //sBattInfo.sRAWBattInfo.fCCLVal = sBattInfo.sRAWBattInfo.fCapCCLVal/ sBattInfo.sRAWBattInfo.fCapTotal_all;// current limit C
              
              flag_test = 17;
            }
          }else{
            dpc_t.check_sum_calculator = 0;
            dpc_t.check_sum_read       = 0xFFFF;
            
          }
          dpc_t.uart_rx.cnt =0;
          MESGState = DCALARM_REQ;
          break;
          
        case DCALARM_REQ://dc alarm request------------------------------------------------------------
          DKD51_send_message_resquest((uint8_t*)&DKD51_REQ_dc_alarm[0],(uint8_t)DKD51_REQ_dc_alarm_len);
          MESGState = DCALARM_RES;
          break;
          
        case DCALARM_RES://dc alarm response------------------------------------------------------------
          if(DKD51_check_sum(&dpc_t) == NO_ERR){
            if(DKD51_check_RTN(&dpc_t) == NO_ERR){
              sDcInfo.sALARMDcInfo.u8DC = hex2byte(&dpc_t.rx.buff_485[0],17); // 0x00H:OK; 0x01H: LOW; 0x02H: High;
              sDcInfo.sALARMDcInfo.u8DC_NoFuse = hex2byte(&dpc_t.rx.buff_485[0],19);
              dpc_t.var = sDcInfo.sALARMDcInfo.u8DC_NoFuse -2;
              for(uint_t i=0;i<dpc_t.var;i++){
                sDcInfo.sALARMDcInfo.u8LoadFuse[i] = hex2byte(&dpc_t.rx.buff_485[0],25+i*2);//0x03H:fulse off
                if((i<10)&&(sDcInfo.sALARMDcInfo.u8LoadFuse[i] == 0x03))privateMibBase.loadGroup.loadStatus[i]=1;
              }
              sDcInfo.sALARMDcInfo.u8LLVD              = hex2byte(&dpc_t.rx.buff_485[0],187);//0x80
              sDcInfo.sALARMDcInfo.u8LLVD2             = hex2byte(&dpc_t.rx.buff_485[0],189);//0x81
              sDcInfo.sALARMDcInfo.batt_low_volt_alarm = hex2byte(&dpc_t.rx.buff_485[0],191);//0x82
              
              sDcInfo.sALARMDcInfo.u8BattFuse[0]       = hex2byte(&dpc_t.rx.buff_485[0],21); //0x03
              sDcInfo.sALARMDcInfo.u8BattFuse[1]       = hex2byte(&dpc_t.rx.buff_485[0],23); //0x03
              sDcInfo.sALARMDcInfo.u8BattFuse[2]       = hex2byte(&dpc_t.rx.buff_485[0],195);//0x03
              sDcInfo.sALARMDcInfo.u8BattFuse[3]       = hex2byte(&dpc_t.rx.buff_485[0],197);//0x03
              flag_test = 18;
            }
          }else{
            dpc_t.check_sum_calculator = 0;
            dpc_t.check_sum_read       = 0xFFFF;
            
          }
          dpc_t.uart_rx.cnt =0;
          MESGState = ENVDATA_REQ;
          break;
        //ENV MESSAGE==================================================================================
        case ENVDATA_REQ:// env data request------------------------------------------------------------
          DKD51_send_message_resquest((uint8_t*)&DKD51_REQ_env_data[0],(uint8_t)DKD51_REQ_env_data_len);
          MESGState = ENVDATA_RES;
          break;
        case ENVDATA_RES://env data response------------------------------------------------------------
          if(DKD51_check_sum(&dpc_t) == NO_ERR){
            if(DKD51_check_RTN(&dpc_t) == NO_ERR){
              sDcInfo.sINFODcInfo.fSen1AmbTemp = ((float)hex2byte2(&dpc_t.rx.buff_485[0],17))/100;
              sDcInfo.sINFODcInfo.fHumidity    = ((float)hex2byte2(&dpc_t.rx.buff_485[0],23))/100;
              sDcInfo.sINFODcInfo.fcustomized  = ((float)hex2byte2(&dpc_t.rx.buff_485[0],29))/100;
            } 
          }
          else{
            dpc_t.check_sum_calculator = 0;
            dpc_t.check_sum_read       = 0xFFFF;
          }
          dpc_t.uart_rx.cnt =0;
          MESGState = ENVALARM_REQ;
          break;
        case ENVALARM_REQ://env alarm request----------------------------------------------------------
          DKD51_send_message_resquest((uint8_t*)&DKD51_REQ_env_alarm[0],(uint8_t)DKD51_REQ_env_alarm_len);
          MESGState = ENVALARM_RES;
          break;
        case ENVALARM_RES://env laarm response----------------------------------------------------------
          if(DKD51_check_sum(&dpc_t) == NO_ERR){
            if(DKD51_check_RTN(&dpc_t) == NO_ERR){
              sAlarmEnvInfo.u8EnvTemp          = hex2byte(&dpc_t.rx.buff_485[0],15); //0x00H: OK;0x04H: ALARM;
              sAlarmEnvInfo.u8EnvHumidity      = hex2byte(&dpc_t.rx.buff_485[0],17); //0x00H: OK;0x04H: ALARM;
              sAlarmEnvInfo.u8EnvSmoke         = hex2byte(&dpc_t.rx.buff_485[0],19); //0x00H: OK;0x04H: ALARM;
              sAlarmEnvInfo.u8EnvWater         = hex2byte(&dpc_t.rx.buff_485[0],21); //0x00H: OK;0x04H: ALARM;
              sAlarmEnvInfo.u8EnvInfra         = hex2byte(&dpc_t.rx.buff_485[0],23); //0x00H: OK;0x04H: ALARM;
              sAlarmEnvInfo.u8EnvDoor          = hex2byte(&dpc_t.rx.buff_485[0],25); //0x00H: OK;0x04H: ALARM;
              sAlarmEnvInfo.u8EnvGlass         = hex2byte(&dpc_t.rx.buff_485[0],27); //0x00H: OK;0x04H: ALARM;
            } 
          }
          else{
            dpc_t.check_sum_calculator = 0;
            dpc_t.check_sum_read       = 0xFFFF;
          }
          dpc_t.uart_rx.cnt =0;
          MESGState = TIMEINFO_REQ;
          break;
        //TIME MESSAGE=================================================================================
        case TIMEINFO_REQ://time info request-----------------------------------------------------------
          DKD51_send_message_resquest((uint8_t*)&DKD51_REQ_time_info[0],(uint8_t)DKD51_REQ_time_info_len);
          MESGState = TIMEINFO_RES;
          break;
          
        case TIMEINFO_RES://time info response---------------------------------------------------------
          if(DKD51_check_sum(&dpc_t) == NO_ERR){
            if(DKD51_check_RTN(&dpc_t) == NO_ERR){
              //code here
            } 
          }
          else{
            dpc_t.check_sum_calculator = 0;
            dpc_t.check_sum_read       = 0xFFFF;
          }
          dpc_t.uart_rx.cnt =0;
          MESGState = UPDATE_OK;
          break;
        //UPDATE data SNMP===================================================================================
        case UPDATE_OK:
          //batt======================
          if(sDcInfo.sINFODcInfo.fBatt1Volt>30){
            privateMibBase.batteryGroup.battVolt = (int32_t) (sDcInfo.sINFODcInfo.fBatt1Volt * 100);
          }else if(sDcInfo.sINFODcInfo.fBatt2Volt>30){
            privateMibBase.batteryGroup.battVolt = (int32_t) (sDcInfo.sINFODcInfo.fBatt2Volt * 100);
          }else if(sDcInfo.sINFODcInfo.fBatt3Volt>30){
            privateMibBase.batteryGroup.battVolt = (int32_t) (sDcInfo.sINFODcInfo.fBatt3Volt * 100);
          }else if(sDcInfo.sINFODcInfo.fBatt4Volt>30){
            privateMibBase.batteryGroup.battVolt = (int32_t) (sDcInfo.sINFODcInfo.fBatt4Volt * 100);
          }else{
            privateMibBase.batteryGroup.battVolt =0;
          }
          
          privateMibBase.batteryGroup.battCurr = (int32_t) ((sDcInfo.sINFODcInfo.fBatt1Curr + sDcInfo.sINFODcInfo.fBatt2Curr 
                                                            +sDcInfo.sINFODcInfo.fBatt3Curr + sDcInfo.sINFODcInfo.fBatt4Curr) * 100);
          if(sBattInfo.sRAWBattInfo.fCapTotal !=0){
            privateMibBase.batteryGroup.battCapLeft1 = (uint32_t) ((sDcInfo.sINFODcInfo.fBatt1RmnCap/sBattInfo.sRAWBattInfo.fCapTotal)* 10000);
          }else {
            privateMibBase.batteryGroup.battCapLeft1 =0;
          }
          if(sBattInfo.sRAWBattInfo.fCapTotal2 !=0){
          privateMibBase.batteryGroup.battCapLeft2 = (uint32_t) ((sDcInfo.sINFODcInfo.fBatt2RmnCap/sBattInfo.sRAWBattInfo.fCapTotal2) * 10000);
          }
          else{
            privateMibBase.batteryGroup.battCapLeft2 = 0;
          }
          if(sBattInfo.sRAWBattInfo.fCapTotal3 !=0){
            privateMibBase.batteryGroup.battCapLeft3 = (uint32_t) ((sDcInfo.sINFODcInfo.fBatt3RmnCap/sBattInfo.sRAWBattInfo.fCapTotal3) * 10000);
          }else{
            privateMibBase.batteryGroup.battCapLeft3 =0;
          }
          if(sBattInfo.sRAWBattInfo.fCapTotal4 !=0)
          {
            privateMibBase.batteryGroup.battCapLeft4 = (uint32_t) ((sDcInfo.sINFODcInfo.fBatt4RmnCap/sBattInfo.sRAWBattInfo.fCapTotal4) * 10000);
          }else {
            privateMibBase.batteryGroup.battCapLeft4 = 0;
          }
          privateMibBase.batteryGroup.battTemp = (uint32_t) (sDcInfo.sINFODcInfo.fSen1BattTemp*10); //0x55AA : Invalid
          if(privateMibBase.batteryGroup.battTemp>= 2000){
            privateMibBase.batteryGroup.battTemp = 0;
          }
          
          privateMibBase.batteryGroup.battBanksNumofBanks = 4;
          //alarm load breaker open-----------------------------------------------------------------
          privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen = 0;
          for (uint8_t i = 0; i < 10; i++)
          {
              if(sDcInfo.sALARMDcInfo.u8LoadFuse[i] == 0x03)
              {
                  dpc_t.fuse_flag = 1;
                  privateMibBase.loadGroup.loadStatus[i] = 1;
              }
              else
              {
                  dpc_t.fuse_flag = 0;
                  privateMibBase.loadGroup.loadStatus[i] = 0;
              }
              privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen |= (dpc_t.fuse_flag << i);
          }
          
          if (privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen != 0)
          {
             privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen = 1;
          }
          else
          {
             privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen = 0;
          }
          //alarm batt breaker open---------------------------------------------------------------------
          privateMibBase.mainAlarmGroup.alarmBattBreakerOpen = 0;
          for (uint8_t i = 0; i < 4; i++)
          {
               if(sDcInfo.sALARMDcInfo.u8BattFuse[i] == 0x03)
               {
                  dpc_t.fuse_flag = 1;
                  privateMibBase.batteryGroup.battBrkStatus[i] = 1;
               }
               else
               {
                  dpc_t.fuse_flag = 0;
                  privateMibBase.batteryGroup.battBrkStatus[i] = 0;
               }
               privateMibBase.mainAlarmGroup.alarmBattBreakerOpen |= (dpc_t.fuse_flag << i);
          }
          if (privateMibBase.mainAlarmGroup.alarmBattBreakerOpen != 0)
          {
              privateMibBase.mainAlarmGroup.alarmBattBreakerOpen = 1;
          }
          else
          {
              privateMibBase.mainAlarmGroup.alarmBattBreakerOpen = 0;
          }
          //ac main fail------------------------------------------------------------------------
          if((sAcInfo.u8MainFail==0x80))
          {
            privateMibBase.mainAlarmGroup.alarmACmains =1;
          }
          else privateMibBase.mainAlarmGroup.alarmACmains =0;
          // ac spd-------------------------------------------------------------------------------
          if(sAcInfo.u8ACSPD==0x81)
          {
            privateMibBase.mainAlarmGroup.alarmACSPD =1;
          }
          else privateMibBase.mainAlarmGroup.alarmACSPD =0;
          //llvd---------------------------------------------------------------------------------
          if(sDcInfo.sALARMDcInfo.u8LLVD==0x00)
            privateMibBase.mainAlarmGroup.alarmLLVD = 0;
          else privateMibBase.mainAlarmGroup.alarmLLVD = 1;
          //blvd--------------------------------------------------------------------------------
          //sDcInfo.sALARMDcInfo.batt_low_volt_alarm
          if(sDcInfo.sALARMDcInfo.batt_low_volt_alarm == 0x82)privateMibBase.mainAlarmGroup.alarmBLVD = 1;
          else privateMibBase.mainAlarmGroup.alarmBLVD = 0;
          
          //DC low-------------------------------------------------------------------------------
          if(sDcInfo.sALARMDcInfo.u8DC==0x01)
            privateMibBase.mainAlarmGroup.alarmDCLow = 0x01;
          else privateMibBase.mainAlarmGroup.alarmDCLow = 0x00;
          //rect============
          privateMibBase.mainAlarmGroup.alarmRectACFault = 0;
          privateMibBase.mainAlarmGroup.alarmRectNoResp = 0;
          for(uint8_t i=0; i<6; i++)  
          {
            privateMibBase.rectGroup.rectTable[i].rectStatus = 0;
            privateMibBase.mainAlarmGroup.alarmRectNoResp |= sRectInfo.sRAWRectParam[i].u8Rect_NoResp << i;    
            privateMibBase.mainAlarmGroup.alarmRectACFault |= sRectInfo.sRAWRectParam[i].u8Rect_Fail << i;    							
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
            if(sRectInfo.sRAWRectParam[i].u8Rect_Sts==0)
            {
               privateMibBase.rectGroup.rectTable[i].rectOutputVoltage =(uint32_t) (sRectInfo.fAllRectDcVolt * 100);
               privateMibBase.rectGroup.rectTable[i].rectOutputVoltage =(uint32_t) (sRectInfo.sRAWRectParam[i].fRect_DcVolt * 100);
               
           }
                           
           else
           {
               privateMibBase.rectGroup.rectTable[i].rectOutputVoltage =0;
               
           }
         }
         if (privateMibBase.mainAlarmGroup.alarmRectNoResp != 0) privateMibBase.mainAlarmGroup.alarmRectNoResp = 1;
         else privateMibBase.mainAlarmGroup.alarmRectNoResp = 0;
         if (privateMibBase.mainAlarmGroup.alarmRectACFault != 0) privateMibBase.mainAlarmGroup.alarmRectACFault = 1;
         else privateMibBase.mainAlarmGroup.alarmRectACFault = 0;
         privateMibBase.rectGroup.rectTotalCurrent = 0;
         for (uint8_t i = 0; i < 6; i++)     
         {
            privateMibBase.rectGroup.rectTotalCurrent += (uint32_t) (privateMibBase.rectGroup.rectTable[i].rectOutputCurrent);
         }
         privateMibBase.rectGroup.rectInstalledRect = sRectInfo.u8Rect_Num;    
         privateMibBase.rectGroup.rectActiveRect = sRectInfo.u8Rect_Num;     
         privateMibBase.loadGroup.loadCurrent = (uint32_t) (sDcInfo.sINFODcInfo.fCurrent*100);
         //AC====================================
          privateMibBase.acPhaseGroup.acPhaseTable[0].acPhaseVolt = (int32_t) (sAcInfo.facVolt[0] * 100);
          privateMibBase.acPhaseGroup.acPhaseTable[1].acPhaseVolt = (int32_t) (sAcInfo.facVolt[1] * 100);
          privateMibBase.acPhaseGroup.acPhaseTable[2].acPhaseVolt = (int32_t) (sAcInfo.facVolt[2] * 100);
          //ac low-------------------------------------------
          if ((sAcInfo.u8Thres[0] == 0x01) || (sAcInfo.u8Thres[1] ==0x01) || (sAcInfo.u8Thres[2]==0x01))
          {
            privateMibBase.mainAlarmGroup.alarmACLow = 1;
          }
          else
          {
            privateMibBase.mainAlarmGroup.alarmACLow = 0;
          }
          //
          privateMibBase.configGroup.cfgFloatVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fFltVoltCfg * 100);
          privateMibBase.configGroup.cfgBoostVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fBotVoltCfg * 100);
          privateMibBase.configGroup.cfgTempCompValue = (uint32_t) sBattInfo.sRAWBattInfo.fTempCompVal;
          //privateMibBase.configGroup.cfgLLVDVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg * 100);
          //privateMibBase.configGroup.cfgBLVDVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fLVDDV * 100);
          privateMibBase.configGroup.cfgDCLowVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg * 100);
          //privateMibBase.configGroup.cfgBattTestVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fTestVoltCfg * 100);
          
          //privateMibBase.cfgBTGroup.cfgBTEndTime = (uint32_t) (sBattTestInfo.fBattTestDur);
          //privateMibBase.cfgBTGroup.cfgBTEndCap = (uint32_t) (sBattTestInfo.fTestEndCap * 1000);
          //privateMibBase.cfgBTGroup.cfgTestVoltCfg = (uint32_t)(sBattInfo.sRAWBattInfo.fTestVoltCfg*10);
          privateMibBase.configGroup.cfgBattCapacityTotal  = (uint32_t) sBattInfo.sRAWBattInfo.fCapTotal;
          privateMibBase.configGroup.cfgBattCapacityTotal2 = (uint32_t) sBattInfo.sRAWBattInfo.fCapTotal2;
          privateMibBase.configGroup.cfgBattCapacityTotal3 = (uint32_t) sBattInfo.sRAWBattInfo.fCapTotal3;
          privateMibBase.configGroup.cfgBattCapacityTotal4 = (uint32_t) sBattInfo.sRAWBattInfo.fCapTotal4;
          privateMibBase.configGroup.cfgCurrentlimitA      = (uint32_t) ( sBattInfo.sRAWBattInfo.fCapCCLVal * 100);
          privateMibBase.configGroup.cfgDCOverVolt         = (uint32_t)(sBattInfo.sRAWBattInfo.fDCOverCfg * 100);                    
          //privateMibBase.configGroup.cfgOverMajorTempLevel = (uint32_t) (sBattInfo.sRAWBattInfo.fOvMjTempAlrmLevel * 100);
          //privateMibBase.configGroup.cfgHighMajorTempLevel = (uint32_t) (sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel *100);
          privateMibBase.configGroup.cfgAcLowLevel = (uint32_t) sAcInfo.fAcLowThres;
          privateMibBase.configGroup.cfgAcHighLevel = (uint32_t) sAcInfo.fAcHighThres;
          //alarm env
          privateMibBase.mainAlarmGroup.alarmSmoke = 0;
          if(sAlarmEnvInfo.u8EnvSmoke==0x00)
          {
            privateMibBase.mainAlarmGroup.alarmSmoke = 0x00;
          }
          else{
            privateMibBase.mainAlarmGroup.alarmSmoke = 0x01;
          }
          
          privateMibBase.mainAlarmGroup.alarmEnvDoor = 0;
          if(sAlarmEnvInfo.u8EnvDoor == 0x00){
            privateMibBase.mainAlarmGroup.alarmEnvDoor = 0x00;
          }else{
            privateMibBase.mainAlarmGroup.alarmEnvDoor = 0x01;
          }
          
          
          
          
          
          MESGState = ACINPUTINFO_REQ;
          break;
          
        default: 
          MESGState = ACINPUTINFO_REQ;
          break;  
      } 
    }
    //-----------------------------------------------------------
    //===========================================================
    
    vTaskDelay(300);//500
  }
  //vTaskSuspend(NULL);
}
#endif
# if (USERDEF_RS485_VERTIV_M830B == ENABLED)
extern sMODBUSRTU_struct Modbus_vertiv;
uint8_t u8return = 1;
float Var_setup;
uint16_t ttt;
void VERTIV_M830B_data_process(void *pvParameters)
{
  vertiv_t.ID = VERTIV_SLAVE_ID;
  MESGState = SYSINFO_REQ;
  for(;;){
  if(setCmd_flag == 1){
     settingCommand |= setCmd_mask;
      switch(settingCommand){
        uint16_t temp;       
        //---------------------------------------------------------------
        case SET_FLTVOL: //16
          sBattInfo.sRAWBattInfo.fFltVoltCfg = (float)privateMibBase.configGroup.cfgFloatVolt /100;
          temp = (uint16_t)(sBattInfo.sRAWBattInfo.fFltVoltCfg*10);
          SetVar_i16_vertiv(vertiv_t.ID,16,temp);
          MESGState = VERTIV_WAIT_RES_SETT; 
          break;
        //---------------------------------------------------------------
        case SET_BSTVOL:
          sBattInfo.sRAWBattInfo.fBotVoltCfg = ((float)privateMibBase.configGroup.cfgBoostVolt)/100;;
          temp =(uint16_t)(sBattInfo.sRAWBattInfo.fBotVoltCfg*10);
          SetVar_i16_vertiv(vertiv_t.ID,36,temp);
          MESGState = VERTIV_WAIT_RES_SETT; 
          break;
        //---------------------------------------------------------------
        case SET_TEMPCOMP_VAL:
          sBattInfo.sRAWBattInfo.fTempCompVal = ((float)privateMibBase.configGroup.cfgTempCompValue);
          temp =(uint16_t)(sBattInfo.sRAWBattInfo.fTempCompVal*10);
          SetVar_i16_vertiv(vertiv_t.ID,45,temp);
          MESGState = VERTIV_WAIT_RES_SETT;                    
          break;
//        //---------------------------------------------------------------
//        case SET_LLVD: //17
//          sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg = (float)privateMibBase.configGroup.cfgLLVDVolt/100;
//          temp =(uint16_t)(sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg*10);
//          SetVar_i16_vertiv(vertiv_t.ID,17,temp);
//          MESGState = VERTIV_WAIT_RES_SETT;                    
//          break;
//        //---------------------------------------------------------------
//        case SET_BLVD://18
//          sBattInfo.sRAWBattInfo.fLVDDV = (float)privateMibBase.configGroup.cfgBLVDVolt/100;
//          temp =(uint16_t)(sBattInfo.sRAWBattInfo.fLVDDV*10);
//          SetVar_i16_vertiv(vertiv_t.ID,18,temp);
//          MESGState = VERTIV_WAIT_RES_SETT;
//          break;
        //---------------------------------------------------------------
        case SET_DCLOW: //17
          sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg = (float)privateMibBase.configGroup.cfgDCLowVolt/100;
          temp =(uint16_t)(sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg*10);
          SetVar_i16_vertiv(vertiv_t.ID,17,temp);
          MESGState = VERTIV_WAIT_RES_SETT;                    
          break;
        //---------------------------------------------------------------
        case SET_DCUNDER://18
          sBattInfo.sRAWBattInfo.fDCUnderCfg = (float)privateMibBase.configGroup.cfgDCUnderVolt/100;
          temp =(uint16_t)(sBattInfo.sRAWBattInfo.fDCUnderCfg*10);
          SetVar_i16_vertiv(vertiv_t.ID,18,temp);
          MESGState = VERTIV_WAIT_RES_SETT;
          break;                                                        
        //---------------------------------------------------------------
        case SET_DCOVER://19
          sBattInfo.sRAWBattInfo.fDCOverCfg = (float)(privateMibBase.configGroup.cfgDCOverVolt)/100;
          temp =(uint16_t)(sBattInfo.sRAWBattInfo.fDCOverCfg*10);
          SetVar_i16_vertiv(vertiv_t.ID,19,temp);
          MESGState = VERTIV_WAIT_RES_SETT;                                                
          break;
        //---------------------------------------------------------------
        case SET_BATTSTDCAP:
          sBattInfo.sRAWBattInfo.fCapTotal = (float)privateMibBase.configGroup.cfgBattCapacityTotal;
          temp =(uint16_t)(sBattInfo.sRAWBattInfo.fCapTotal);
          SetVar_i16_vertiv(vertiv_t.ID,54,temp);
          MESGState = VERTIV_WAIT_RES_SETT;    
          
          break;
//        //---------------------------------------------------------------
//        case SET_BATTSTDCAP2:
//          sBattInfo.sRAWBattInfo.fCapTotal2 = (float)privateMibBase.configGroup.cfgBattCapacityTotal2;
//          temp =(uint16_t)(sBattInfo.sRAWBattInfo.fCapTotal2);
//          SetVar_i16_vertiv(vertiv_t.ID,59,temp);
//          MESGState = VERTIV_WAIT_RES_SETT;
//          break;
        //---------------------------------------------------------------
        case SET_CCL: //35
          sBattInfo.sRAWBattInfo.fCCLVal = (float)privateMibBase.configGroup.cfgCurrentLimit;                            //privateMibBase.configGroup.cfgCurrentLimit/1000;
          temp =(uint16_t)(sBattInfo.sRAWBattInfo.fCCLVal);
          SetVar_i16_vertiv(vertiv_t.ID,35,temp);
          MESGState = VERTIV_WAIT_RES_SETT;          
          break;
        //---------------------------------------------------------------
        case SET_OVERTEMP:
          sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel = (float)privateMibBase.configGroup.cfgHighMajorTempLevel/100;
          temp =(uint16_t)(sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel*10 + 32000);
          SetVar_i16_vertiv(vertiv_t.ID,32,temp);
          MESGState = VERTIV_WAIT_RES_SETT;            
          break;
        //---------------------------------------------------------------
        case SET_AC_LOW_THRES:

          break;
        //---------------------------------------------------------------
        case SET_AC_HIGH_THRES:

          break;
        //---------------------------------------------------------------
        case SET_AC_INPUT_CURR_LIMIT://space
          break;
   //======================================== BATTERY TEST SETTING ============================================//
        case SET_BT_ENDVOLT://40
          break;          
        case SET_BT_ENDTIME://41
          break;          
        case SET_WIT_EN:
          sRectInfo.u8WITE = privateMibBase.configGroup.cfgWalkInTimeEn;
          temp =(uint16_t)(sRectInfo.u8WITE);
          SetVar_i16_vertiv(vertiv_t.ID,662,temp);
          MESGState = VERTIV_WAIT_RES_SETT;
          break;
        case SET_WIT_VAL:
          sRectInfo.u8WITE = privateMibBase.configGroup.cfgWalkInTimeDuration;
          temp =(uint16_t)(sRectInfo.u8WITE);
          SetVar_i16_vertiv(vertiv_t.ID,659,temp);
          MESGState = VERTIV_WAIT_RES_SETT;
          break;                    
        default:
          break; 
      }  
   Modbus_vertiv.u8MosbusEn = 0;
   setCmd_flag = 0;
   settingCommand=0;
  
  }else if(setCmd_flag == 0){
        if(PMUConnectCount++>30){
        PMUConnectCount =50;
        privateMibBase.connAlarmGroup.alarmPMUConnect = 1;
        }
    switch (MESGState){
   // Systerm massage
     case SYSINFO_REQ://systern info request---------------------------------------------------------------------
        Read_Regs_Query_Vertiv(vertiv_t.ID,0,16, FUNC_03);
        MESGState =SYSINFO_RES;
        break;
     case SYSINFO_RES:
          u8return = Vertiv_Check_Respond_Data(); 
          if(u8return != 1){
          sDcInfo.u32DCNoResponse++;
            if(sDcInfo.u32DCNoResponse > 3){
              privateMibBase.connAlarmGroup.alarmPMUConnect = 1;
              sDcInfo.u32DCNoResponse = 10;
                 // xoa khi khong nhan duoc ban tin hoac ban tin loi
              sAcInfo.facVolt[0] = 0;
              sAcInfo.facVolt[1] = 0;
              sAcInfo.facVolt[2] = 0;
              sAcInfo.f_hz_in    = 0;
              sAcInfo.f_ac_curr[0]= 0;
              sAcInfo.f_ac_curr[1]= 0;
              sAcInfo.f_ac_curr[2]= 0;
              sAcInfo.fAcHighThres = 0;
              sAcInfo.fAcLowThres = 0;
              sAcInfo.fAcInputCurrLimit = 0;              
              sAcInfo.f_ac_high_hz      = 0;  
              sAcInfo.f_ac_low_hz       = 0;
              for(int i=0;i<9;i++){
                 sAcInfo.acAlarm[i]=0;
              }              
              
              sRectInfo.fTotal_Curr = 0;
              sRectInfo.fAllRectDcVolt  = 0;
              sRectInfo.u8WITI = 0;
              sRectInfo.u8WITE = 0;
              sRectInfo.u8Rect_Num = 0;
              for(uint8_t i=0; i<24; i++)
              {
                 sRectInfo.sRAWRectParam[i].fRect_DcVolt = 0;
                 sRectInfo.sRAWRectParam[i].fRect_DcCurr = 0;
                 sRectInfo.sRAWRectParam[i].fRect_Temp = 0;
                 for(int j=0;j<3;j++){
                  sRectInfo.sRAWRectParam[i].Alarm[j]=0;
                 }
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
              sDcInfo.sINFODcInfo.u8BatNo = 0;
              for(uint8_t i=0;i<7;i++){
                sDcInfo.sALARMDcInfo.u16SysAlarm[i] = 0;      
              }
              
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
              sBattInfo.sRAWBattInfo.fDCUnderCfg =0;
              sBattInfo.sRAWBattInfo.fDcVolt =0;
              sBattInfo.sRAWBattInfo.fTotalCurr =0;
              sBattInfo.sRAWBattInfo.fCapTotal2 = 0;
              sDcInfo.sINFODcInfo.fBatt1RmnCap=0;
              sDcInfo.sINFODcInfo.fBatt2RmnCap=0;
              for(uint8_t i=0;i<5;i++){
                sBattInfo.sRAWBattInfo.u16BattAlarm[i] = 0;
              }
            }          
           }else{
            sDcInfo.u32DCNoResponse = 0;
             privateMibBase.connAlarmGroup.alarmPMUConnect = 0;          
          }       
          MESGState = SYSPARAMETER_REQ;
        break;
     case SYSPARAMETER_REQ://Systern parameter info request---------------------------------------------------------------------          
          Read_Regs_Query_Vertiv(vertiv_t.ID,16,7, FUNC_03);          
          MESGState = SYSPARAMETER_RES;
          break;  
     case SYSPARAMETER_RES:          
          u8return = Vertiv_Check_Respond_Data();
          MESGState = BATTGROUPINFO_REQ;
          break;
     case BATTGROUPINFO_REQ://Battery group info request---------------------------------------------------------------------          
          Read_Regs_Query_Vertiv(vertiv_t.ID,22,9, FUNC_03);          
          MESGState = BATTGROUPINFO_RES;
          break;  
     case BATTGROUPINFO_RES:          
          u8return = Vertiv_Check_Respond_Data();
          MESGState = BATTPARAMETTER_REQ;
          break;
     case BATTPARAMETTER_REQ://Battery parametter request---------------------------------------------------------------------          
          Read_Regs_Query_Vertiv(vertiv_t.ID,31,18, FUNC_03);          
          MESGState = BATTPARAMETTER_RES;
          break;  
     case BATTPARAMETTER_RES:          
          u8return = Vertiv_Check_Respond_Data();
          MESGState = BATTINFO_REQ;
          break;                                      
     case BATTINFO_REQ://Battery info request---------------------------------------------------------------------          
          Read_Regs_Query_Vertiv(vertiv_t.ID,50,10, FUNC_03);          
          MESGState = BATTINFO_RES;
          break;  
     case BATTINFO_RES:          
          u8return = Vertiv_Check_Respond_Data();
          MESGState = RECTGROUPINFO_REQ;
          break; 
          
  // RECT MESSAGE=====================================================================================         
          
     case RECTGROUPINFO_REQ://rect group info request---------------------------------------------------------------------   
          Read_Regs_Query_Vertiv(vertiv_t.ID,650,13, FUNC_03);
          MESGState = RECTGROUPINFO_RES;
          break;
     case RECTGROUPINFO_RES:  
         Vertiv_Check_Respond_Data();
         MESGState = RECTINFO_REQ;         
         break;
         
         
     case RECTINFO_REQ://rect group info request---------------------------------------------------------------------   
          Read_Regs_Query_Vertiv(vertiv_t.ID,663,9*sRectInfo.u8Rect_Num, FUNC_03);
          MESGState = RECTINFO_RES;
          break;
     case RECTINFO_RES:  
         Vertiv_Check_Respond_Data();
         MESGState = ACINPUTINFO_REQ;         
         break; 
         
//     case RECTPARAMETTER_REQ://rect parametter request---------------------------------------------------------------------   
//          Read_Regs_Query_Vertiv(vertiv_t.ID,650,1, FUNC_03);
//          MESGState =RECTGROUPINFO_RES;
//          break;
//     case RECTPARAMETTER_RES:  
//         Vertiv_Check_Respond_Data();
//         MESGState = DCINFO_REQ;         
//         break;         
         
         
//     case DCINFO_REQ://DC info request---------------------------------------------------------------------   
//          Read_Regs_Query_Vertiv(vertiv_t.ID,1743,3, FUNC_03);
//          MESGState =DCINFO_RES;
//          break;
//     case DCINFO_RES:   
//         Vertiv_Check_Respond_Data();
//         MESGState = ACINPUTINFO_REQ;         
//         break;          
        // AC MESSAGE=====================================================================================
     case ACINPUTINFO_REQ://ac info request---------------------------------------------------------------------          
          Read_Regs_Query_Vertiv(vertiv_t.ID,1749,12, FUNC_03);          
          MESGState = ACINPUTINFO_RES;
          break;         
     case ACINPUTINFO_RES://ac info response--------------------------------------------------------------------
          u8return = Vertiv_Check_Respond_Data();           
          MESGState = UPDATE_OK;
          break;
     case VERTIV_WAIT_RES_SETT://ac info request---------------------------------------------------------------------   
        u8return = Vertiv_Check_Respond_Setting_Data();
        MESGState = SYSINFO_REQ;
        break;  
     case UPDATE_OK:
       //*...........................LOAD......................................*//
       privateMibBase.loadGroup.loadCurrent             = (uint32_t)(sDcInfo.sINFODcInfo.fCurrent *100);
       //*...........................BATT......................................*//
       privateMibBase.batteryGroup.battVolt             = (uint32_t)(sBattInfo.sRAWBattInfo.fDcVolt *100);
       privateMibBase.batteryGroup.battCurr             = (uint32_t)(sBattInfo.sRAWBattInfo.fTotalCurr *100);
       privateMibBase.batteryGroup.battTemp             = (int32_t) (sDcInfo.sINFODcInfo.fSen1BattTemp*100);
       privateMibBase.batteryGroup.battCapLeft1         = (uint32_t)(sDcInfo.sINFODcInfo.fBatt1RmnCap*100);
       privateMibBase.batteryGroup.battBanksNumofBanks  = 1;
       //privateMibBase.batteryGroup.battCapLeft2         = (uint32_t)(sDcInfo.sINFODcInfo.fBatt2RmnCap*100);
       //*...........................Config......................................*//
       privateMibBase.configGroup.cfgFloatVolt          = (uint32_t) (sBattInfo.sRAWBattInfo.fFltVoltCfg * 100);
       privateMibBase.configGroup.cfgBoostVolt          = (uint32_t) (sBattInfo.sRAWBattInfo.fBotVoltCfg * 100);
       privateMibBase.configGroup.cfgTempCompValue      = (uint32_t) (sBattInfo.sRAWBattInfo.fTempCompVal);
       //privateMibBase.configGroup.cfgLLVDVolt           = (uint32_t) (sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg * 100);
       //privateMibBase.configGroup.cfgBLVDVolt           = (uint32_t) (sBattInfo.sRAWBattInfo.fLVDDV * 100);
       privateMibBase.configGroup.cfgDCLowVolt          = (uint32_t) (sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg * 100);
       privateMibBase.configGroup.cfgDCUnderVolt        = (uint32_t) (sBattInfo.sRAWBattInfo.fDCUnderCfg * 100);
       privateMibBase.configGroup.cfgDCOverVolt         = (uint32_t) (sBattInfo.sRAWBattInfo.fDCOverCfg * 100);                
       //privateMibBase.configGroup.cfgLLVDEn             = sBattInfo.sRAWBattInfo.u8LLVDE;
       //privateMibBase.configGroup.cfgBLVDEn             = sBattInfo.sRAWBattInfo.u8BLVDE;
       privateMibBase.configGroup.cfgCurrentLimit       =  (uint32_t)sBattInfo.sRAWBattInfo.fCCLVal;           
       privateMibBase.configGroup.cfgHighMajorTempLevel = (int32_t)(sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel *100);
       privateMibBase.configGroup.cfgBattCapacityTotal  = (uint32_t)(sBattInfo.sRAWBattInfo.fCapTotal);
       privateMibBase.configGroup.cfgTempCompValue      = (uint32_t)(sBattInfo.sRAWBattInfo.fTempCompVal);      
         //privateMibBase.configGroup.cfgOverMajorTempLevel = 10000;
       privateMibBase.configGroup.cfgWalkInTimeDuration = sRectInfo.u8WITI;
       privateMibBase.configGroup.cfgWalkInTimeEn       = sRectInfo.u8WITE;
       //*...........................AC......................................*//
       privateMibBase.acPhaseGroup.acPhaseTable[0].acPhaseVolt = (int32_t) (sAcInfo.facVolt[0] * 100);
       privateMibBase.acPhaseGroup.acPhaseTable[1].acPhaseVolt = (int32_t) (sAcInfo.facVolt[1] * 100);
       privateMibBase.acPhaseGroup.acPhaseTable[2].acPhaseVolt = (int32_t) (sAcInfo.facVolt[2] * 100); 
       
       //*...........................RECT......................................*//
       privateMibBase.rectGroup.rectInstalledRect       = sRectInfo.u8Rect_Num;
       privateMibBase.rectGroup.rectActiveRect          = sRectInfo.u8Rect_Num;
       privateMibBase.mainAlarmGroup.alarmRectACFault = 0;
       privateMibBase.mainAlarmGroup.alarmRectNoResp = 0;
       for(int i =0;i < sRectInfo.u8Rect_Num; i++){
         privateMibBase.rectGroup.rectTable[i].rectStatus = 0xFF;
         privateMibBase.rectGroup.rectTable[i].rectAlarmStatus =0; 

         if((sRectInfo.sRAWRectParam[i].Alarm[1] & 0x0100) == 0x0100)
            {
               privateMibBase.rectGroup.rectTable[i].rectStatus = 0xAA; // Rect no respond
               privateMibBase.mainAlarmGroup.alarmRectNoResp = 1;
            }
         if((sRectInfo.sRAWRectParam[i].Alarm[0] & 0x1000) == 0x1000)
            {
               privateMibBase.rectGroup.rectTable[i].rectStatus = 0x0A; // Rect dc off
            }
         if((sRectInfo.sRAWRectParam[i].Alarm[0] & 0x0001) == 0x0001)
            {
               privateMibBase.rectGroup.rectTable[i].rectStatus = 0xA5; // Rect fail  
               privateMibBase.mainAlarmGroup.alarmRectACFault = 1;
            }
         if((sRectInfo.sRAWRectParam[i].Alarm[0] & 0x0010) == 0x0010)
            {
               privateMibBase.mainAlarmGroup.alarmRectACFault = 1; // Rect Main fail   
               privateMibBase.rectGroup.rectTable[i].rectAlarmStatus =1;
            }
         if(privateMibBase.rectGroup.rectTable[i].rectStatus != 0xFF){
          privateMibBase.rectGroup.rectTable[i].rectOutputVoltage = 0;
          privateMibBase.rectGroup.rectTable[i].rectOutputCurrent = 0;
          privateMibBase.rectGroup.rectActiveRect--;
         }else{
          privateMibBase.rectGroup.rectTable[i].rectOutputVoltage = (uint32_t)(sRectInfo.sRAWRectParam[i].fRect_DcVolt*100);
          privateMibBase.rectGroup.rectTable[i].rectOutputCurrent = (int32_t)(sRectInfo.sRAWRectParam[i].fRect_DcCurr*100); 
         }        
       }
       privateMibBase.rectGroup.rectTotalCurrent = (uint32_t)(sRectInfo.fTotal_Curr *10);
       //*...........................Alarm Main......................................*//
       // Dc alarm
       if((sDcInfo.sALARMDcInfo.u16SysAlarm[0] & 0x0010) == 0x10){
         privateMibBase.mainAlarmGroup.alarmDCLow = 0x01;
       }else privateMibBase.mainAlarmGroup.alarmDCLow = 0x00; 
       //AC alarm
       if(sAcInfo.acAlarm[4] & 0x01 == 0x01){privateMibBase.mainAlarmGroup.alarmACmains = 1;}
       else privateMibBase.mainAlarmGroup.alarmACmains = 0;
       if((sAcInfo.acAlarm[5] & 0x10 == 0x10)||(sAcInfo.acAlarm[6] & 0x10 == 0x10)||(sAcInfo.acAlarm[7] & 0x100 == 0x100)){
        privateMibBase.mainAlarmGroup.alarmACLow = 1;
       }else privateMibBase.mainAlarmGroup.alarmACLow = 0;
       // Bat Alarm
       privateMibBase.mainAlarmGroup.alarmBattHighTemp_old = 0;
       if((sBattInfo.sRAWBattInfo.u16BattAlarm[3] &0x100) == 0x100 ){
          privateMibBase.mainAlarmGroup.alarmBattHighTemp = 2;
          privateMibBase.mainAlarmGroup.alarmBattHighTemp_old =2;
       }else privateMibBase.mainAlarmGroup.alarmBattHighTemp = privateMibBase.mainAlarmGroup.alarmBattHighTemp_old;   
       if((sBattInfo.sRAWBattInfo.u16BattAlarm[3] &0x1) == 0x1 ){
          privateMibBase.mainAlarmGroup.alarmBattHighTemp = 7;
          privateMibBase.mainAlarmGroup.alarmBattHighTemp_old = 7;
       }else privateMibBase.mainAlarmGroup.alarmBattHighTemp = privateMibBase.mainAlarmGroup.alarmBattHighTemp_old;        
       //DI alarm
       if((sDcInfo.sALARMDcInfo.u16SysAlarm[4] & 0x001)==0x001) privateMibBase.diAlarmGroup.alarmDigitalInput[0] = 1;
       if((sDcInfo.sALARMDcInfo.u16SysAlarm[4] & 0x010)==0x010) privateMibBase.diAlarmGroup.alarmDigitalInput[1] = 1;
       if((sDcInfo.sALARMDcInfo.u16SysAlarm[4] & 0x100)==0x100) privateMibBase.diAlarmGroup.alarmDigitalInput[2] = 1;
       if((sDcInfo.sALARMDcInfo.u16SysAlarm[5] & 0x001)==0x010) privateMibBase.diAlarmGroup.alarmDigitalInput[3] = 1;
       if((sDcInfo.sALARMDcInfo.u16SysAlarm[5] & 0x010)==0x001) privateMibBase.diAlarmGroup.alarmDigitalInput[4] = 1;
       if((sDcInfo.sALARMDcInfo.u16SysAlarm[5] & 0x100)==0x100) privateMibBase.diAlarmGroup.alarmDigitalInput[5] = 1;
       if((sDcInfo.sALARMDcInfo.u16SysAlarm[6] & 0x001)==0x001) privateMibBase.diAlarmGroup.alarmDigitalInput[6] = 1;
       if((sDcInfo.sALARMDcInfo.u16SysAlarm[6] & 0x010)==0x010) privateMibBase.diAlarmGroup.alarmDigitalInput[7] = 1;
        //*.................................................................*//
       MESGState = SYSINFO_REQ;                 
       break;

      
        
    } 
  }
        vTaskDelay(100);
 }

}
#endif