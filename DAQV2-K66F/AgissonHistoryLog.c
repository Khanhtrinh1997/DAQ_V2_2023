#include"Header.h"

void GetHistoryLog(void)

{

  int32_t i,j;
  RS485Timer =1;
   sHistoryInfo.ucSequenceNum=0;
   for(j=200;j>-1;j--)  // gan tat ca gia tri bang 0
   {
     sHistoryInfo.sHistoryParam[j].sAlarmContent.u8ID =0;
     for(i=0;i<19;i++)
     {
        sHistoryInfo.sHistoryParam[j].sTimeFrame[0].cTimeFrame[i] =0;
     }
    
       sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sTimeFrame[0].cTimeFrame[18] = '\0';
       strncpy(&sHistoryInfo.sHistoryParam[j].sAlarmContent.ucName[0],"           ",UCNAMESIZE+2);
   }

  for(i=4;i>-1;i--)
  {
    for(j=(sHisFlashLog[i].Count-1);j>-1;j--)
    {
      sHistoryInfo.ucSequenceNum++;
      sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.u8ID = sHisFlashLog[i].HisFlashEvent[j].Hiscode;

      snprintf(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sTimeFrame[0].cTimeFrame[0],18,"%02d-%02d-%02d,%02d:%02d:%02d",
                       sHisFlashLog[i].HisFlashEvent[j].Day,
                       sHisFlashLog[i].HisFlashEvent[j].Month,
                       sHisFlashLog[i].HisFlashEvent[j].Year,
                       sHisFlashLog[i].HisFlashEvent[j].Hour,
                       sHisFlashLog[i].HisFlashEvent[j].Minute,
                       0
//                       sHisFlashLog[i].HisFlashEvent[j].Minute
                      );
      sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sTimeFrame[0].cTimeFrame[18] = '\0';
      switch (sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.u8ID)
      {
              case 0x01:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"ACFailsAlarm",UCNAMESIZE+2);
              break;
              case 0x02:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"ACFailsAlarmResume",UCNAMESIZE+2);
              break;
              case 0x03:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"ACLowAlarm",UCNAMESIZE+2);
              break;
              case 0x04:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"ACLowAlarmResume",UCNAMESIZE+2);
              break;
              case 0x05:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"ACSPDAlarm",UCNAMESIZE+2);
              break;
              case 0x06:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"ACSPDAlarmResume",UCNAMESIZE+2);
              break;
              case 0x07://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DCLowAlarm",UCNAMESIZE+2);
              break;
              case 0x08://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DCLowAlarmResume",UCNAMESIZE+2);
              break;
              case 0x09://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DCSPDAlarm",UCNAMESIZE+2);
              break;
              case 0x0A:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DCSPDAlarmResume",UCNAMESIZE+2);
              break;
              case 0x0B:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"LLVDAlarm",UCNAMESIZE+2);
              break;
              case 0x0C://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"LLVDAlarmResume",UCNAMESIZE+2);
              break;
              case 0x0D://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"BLVDAlarm",UCNAMESIZE+2);
              break;
              case 0x0E:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"BLVDAlarmResume",UCNAMESIZE+2);
              break;
              case 0x0F://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"BatFuseAlarm",UCNAMESIZE+2);
              break;
              case 0x10://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"BatFuseAlarmResume",UCNAMESIZE+2);
              break;
              case 0x11://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"BatHighTemp",UCNAMESIZE+2);
              break;
              case 0x12://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"BatHighTempResume",UCNAMESIZE+2);
              break;
              case 0x13:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"RectFail",UCNAMESIZE+2);
              break;
              case 0x14:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"RectFailResume",UCNAMESIZE+2);
              break;
              case 0x15:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"RectNoRespond",UCNAMESIZE+2);
              break;
              case 0x16:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"RectNoRespondResume",UCNAMESIZE+2);
              break;
              case 0x17:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"LoadFuse",UCNAMESIZE+2);
              break;
              case 0x18://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"LoadFuseResume",UCNAMESIZE+2);
              break;
              case 0x19://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DI1Alarm",UCNAMESIZE+2);
              break;
              case 0x1A://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DI1AlarmResume",UCNAMESIZE+2);
              break;
              case 0x1B://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DI2Alarm",UCNAMESIZE+2);
              break;
              case 0x1C://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DI2AlarmResume",UCNAMESIZE+2);
              break;
              case 0x1D:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"CommunicationFail",UCNAMESIZE+2);
              break;
              case 0x1E:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"CommnunicationResume",UCNAMESIZE+4);
              break;
      default:
        break;
      };                        
    }
  }
  
}


void GetHistoryLog_v21(void)
{
  int32_t i,j;
  sHistoryInfo.ucSequenceNum=0;
  for(j=200;j>-1;j--)  // gan tat ca gia tri bang 0
   {
     sHistoryInfo.sHistoryParam[j].sAlarmContent.u8ID =0;
     for(i=0;i<19;i++)
     {
        sHistoryInfo.sHistoryParam[j].sTimeFrame[0].cTimeFrame[i] =0;
     }
    
       sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sTimeFrame[0].cTimeFrame[18] = '\0';
       strncpy(&sHistoryInfo.sHistoryParam[j].sAlarmContent.ucName[0],"           ",UCNAMESIZE+2);
   }

  for(i=4;i>-1;i--)
  {
    for(j=(sHisFlashLog[i].Count-1);j>-1;j--)
    {
      sHistoryInfo.ucSequenceNum++;
      sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.u8ID = sHisFlashLog[i].HisFlashEvent[j].Hiscode;

      snprintf(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sTimeFrame[0].cTimeFrame[0],18,"%02d-%02d-%02d,%02d:%02d:%02d",
                       sHisFlashLog[i].HisFlashEvent[j].Day,
                       sHisFlashLog[i].HisFlashEvent[j].Month,
                       sHisFlashLog[i].HisFlashEvent[j].Year,
                       sHisFlashLog[i].HisFlashEvent[j].Hour,
                       sHisFlashLog[i].HisFlashEvent[j].Minute,
                       0
//                       sHisFlashLog[i].HisFlashEvent[j].Minute
                      );
      sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sTimeFrame[0].cTimeFrame[18] = '\0';
      switch (sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.u8ID)
      {
              case 0x01:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"ACFailsAlarm",UCNAMESIZE+2);
              break;
              case 0x02:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"ACFailsAlarmResume",UCNAMESIZE+2);
              break;
              case 0x03:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"ACLowAlarm",UCNAMESIZE+2);
              break;
              case 0x04:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"ACLowAlarmResume",UCNAMESIZE+2);
              break;
              case 0x05:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"ACSPDAlarm",UCNAMESIZE+2);
              break;
              case 0x06:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"ACSPDAlarmResume",UCNAMESIZE+2);
              break;
              case 0x07://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DCLowAlarm",UCNAMESIZE+2);
              break;
              case 0x08://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DCLowAlarmResume",UCNAMESIZE+2);
              break;
              case 0x09://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DCSPDAlarm",UCNAMESIZE+2);
              break;
              case 0x0A:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DCSPDAlarmResume",UCNAMESIZE+2);
              break;
              case 0x0B:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"LLVDAlarm",UCNAMESIZE+2);
              break;
              case 0x0C://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"LLVDAlarmResume",UCNAMESIZE+2);
              break;
              case 0x0D://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"BLVDAlarm",UCNAMESIZE+2);
              break;
              case 0x0E:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"BLVDAlarmResume",UCNAMESIZE+2);
              break;
              case 0x0F://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"BatFuseAlarm",UCNAMESIZE+2);
              break;
              case 0x10://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"BatFuseAlarmResume",UCNAMESIZE+2);
              break;
              case 0x11://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"BatHighTemp",UCNAMESIZE+2);
              break;
              case 0x12://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"BatHighTempResume",UCNAMESIZE+2);
              break;
              case 0x13:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"RectFail",UCNAMESIZE+2);
              break;
              case 0x14:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"RectFailResume",UCNAMESIZE+2);
              break;
              case 0x15:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"RectNoRespond",UCNAMESIZE+2);
              break;
              case 0x16:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"RectNoRespondResume",UCNAMESIZE+2);
              break;
              case 0x17:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"LoadFuse",UCNAMESIZE+2);
              break;
              case 0x18://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"LoadFuseResume",UCNAMESIZE+2);
              break;
              case 0x19://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"SmokeAlarm",UCNAMESIZE+2);
              break;
              case 0x1A://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"SmokeAlarmResume",UCNAMESIZE+2);
              break;
              case 0x1D://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"CommunicationFail",UCNAMESIZE+2);
              break;
              case 0x1E://************ Interpolating ***********//
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"CommunicationFailResume",UCNAMESIZE+2);
              break;
              case 0x1F:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DI1Alarm",UCNAMESIZE+2);
              break;
              case 0x20:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DI1AlarmResume",UCNAMESIZE+2);
              break;
              case 0x21:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DI2Alarm",UCNAMESIZE+2);
              break;
              case 0x22:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DI2AlarmResume",UCNAMESIZE+2);
              break;
              case 0x23:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DI3Alarm",UCNAMESIZE+2);
              break;
              case 0x24:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DI3AlarmResume",UCNAMESIZE+2);
              break;
              case 0x25:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DI4Alarm",UCNAMESIZE+2);
              break;
              case 0x26:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DI4AlarmResume",UCNAMESIZE+2);
              break;
              case 0x27:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DI5Alarm",UCNAMESIZE+2);
              break;
              case 0x28:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DI5AlarmResume",UCNAMESIZE+2);
              break;
              case 0x29:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DI6Alarm",UCNAMESIZE+2);
              break;
              case 0x2A:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DI6AlarmResume",UCNAMESIZE+2);
              break;
              case 0x2B:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DI7Alarm",UCNAMESIZE+2);
              break;
              case 0x2C:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DI7AlarmResume",UCNAMESIZE+2);
              break;
              case 0x2D:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DI8Alarm",UCNAMESIZE+2);
              break;
              case 0x2E:
                      strncpy(&sHistoryInfo.sHistoryParam[sHistoryInfo.ucSequenceNum - 1].sAlarmContent.ucName[0],"DI8AlarmResume",UCNAMESIZE+2);
              break;
      default:
        break;                            
    }
}
  }
}