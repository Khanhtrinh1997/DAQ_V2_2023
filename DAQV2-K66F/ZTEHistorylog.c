#include"Header.h"
#include "ZTEHistorylog.h"
sztelog ZTEHIS;
tmodulehis ModuleHis;
void GetACDataLog(void)
{   int duplicate=5;
    int i,j;
    ZTEHIS.ACThes[0] = hex2byte(&RecvBuff[0],33);
    ZTEHIS.ACThes[1] = hex2byte(&RecvBuff[0],35);
    ZTEHIS.ACThes[2] = hex2byte(&RecvBuff[0],37);
    ZTEHIS.ACFail = hex2byte(&RecvBuff[0],45);
    ZTEHIS.ZTEYear = hex2byte2(&RecvBuff[0],17);
    ZTEHIS.ZTEMon =  hex2byte(&RecvBuff[0],21);
    ZTEHIS.ZTEDay =  hex2byte(&RecvBuff[0],23);
    ZTEHIS.ZTEHour = hex2byte(&RecvBuff[0],25);
    ZTEHIS.ZTEMin =  hex2byte(&RecvBuff[0],27);
    ZTEHIS.ZTESec =  hex2byte(&RecvBuff[0],29);
    for(i=0;i<=ih;i++)
    {
      for(j=0;j< sHisFlashLog[i].Count;j++)
      {
         if(ZTEHIS.ZTEYear-2000==sHisFlashLog[i].HisFlashEvent[j].Year)
         {
           duplicate--;
         }
         if(ZTEHIS.ZTEMon==sHisFlashLog[i].HisFlashEvent[j].Month)
         {
           duplicate--;
         }
         if(ZTEHIS.ZTEDay==sHisFlashLog[i].HisFlashEvent[j].Day)
         {
           duplicate--;
         }
         if(ZTEHIS.ZTEHour==sHisFlashLog[i].HisFlashEvent[j].Hour)
         {
           duplicate--;
         }
         if(ZTEHIS.ZTEMin==sHisFlashLog[i].HisFlashEvent[j].Minute)
         {
           duplicate--;
         }
         if(duplicate!=0) duplicate =5;
         else
         {
           j= sHisFlashLog[i].Count;
           i = ih+1; 
         }
      }
    }
    if(duplicate>0)
    {
        if(ZTEHIS.ACFail!=ZTEHIS.ACFail_old)
        {
            if(ZTEHIS.ACFail!=0)
            {
               sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =1;
               NextHis();
            }
            else
            {
               sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =2;
               NextHis();
            }
        }
        if(ZTEHIS.ACThes[0]!=ZTEHIS.ACThes_old[0])
        {
            if(ZTEHIS.ACThes[0]!=0)
            {
               sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =3;
               NextHis();
            }
            else
            {
               sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =4;
               NextHis();
            }
        }
        if(ZTEHIS.ACThes[1]!=ZTEHIS.ACThes_old[1])
        {
            if(ZTEHIS.ACThes[1]!=0)
            {
               sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =3;
               NextHis();
            }
            else
            {
               sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =4;
               NextHis();
            }
        }
        if(ZTEHIS.ACThes[2]!=ZTEHIS.ACThes_old[2])
        {
            if(ZTEHIS.ACThes[2]!=0)
            {
               sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =3;
               NextHis();
            }
            else
            {
               sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =4;
               NextHis();
            }
        }

        ZTEHIS.ACFail_old=ZTEHIS.ACFail;
        ZTEHIS.ACThes_old[0]=ZTEHIS.ACThes[0];
        ZTEHIS.ACThes_old[1]=ZTEHIS.ACThes[1];
        ZTEHIS.ACThes_old[2]=ZTEHIS.ACThes[2];
         for(i=0;i<(ih+1);i++)
         {
            if(HisCount_old[i] <sHisFlashLog[i].Count)
            {
              for(j=HisCount_old[i];j<(sHisFlashLog[i].Count);j++)
                {
                 
                    sHisFlashLog[i].HisFlashEvent[j].Year  =   (uint8_t)(ZTEHIS.ZTEYear-2000);
                    sHisFlashLog[i].HisFlashEvent[j].Month =  ZTEHIS.ZTEMon;
                    sHisFlashLog[i].HisFlashEvent[j].Day   =  ZTEHIS.ZTEDay;
                    sHisFlashLog[i].HisFlashEvent[j].Hour  =  ZTEHIS.ZTEHour;
                    sHisFlashLog[i].HisFlashEvent[j].Minute=  ZTEHIS.ZTEMin;
    //                sHisFlashLog[i].HisFlashEvent[j].Sec   =  ZTEHIS.ZTESec;
                }
              HisCount_old[i] = sHisFlashLog[i].Count;
            }
         }
    }

//    GetHistoryLog();
}
void GetRectDataLog(void)
{
    int duplicate=5;
    int i =0,j=0;
    ZTEHIS.ZTEYear = hex2byte2(&RecvBuff[0],17);
    ZTEHIS.ZTEMon =  hex2byte(&RecvBuff[0],21);
    ZTEHIS.ZTEDay =  hex2byte(&RecvBuff[0],23);
    ZTEHIS.ZTEHour = hex2byte(&RecvBuff[0],25);
    ZTEHIS.ZTEMin =  hex2byte(&RecvBuff[0],27);
    ZTEHIS.ZTESec =  hex2byte(&RecvBuff[0],29);
    for(i=0;i<=ih;i++)
    {
      for(j=0;j< sHisFlashLog[i].Count;j++)
      {
         if(ZTEHIS.ZTEYear-2000==sHisFlashLog[i].HisFlashEvent[j].Year)
         {
           duplicate--;
         }
         if(ZTEHIS.ZTEMon==sHisFlashLog[i].HisFlashEvent[j].Month)
         {
           duplicate--;
         }
         if(ZTEHIS.ZTEDay==sHisFlashLog[i].HisFlashEvent[j].Day)
         {
           duplicate--;
         }
         if(ZTEHIS.ZTEHour==sHisFlashLog[i].HisFlashEvent[j].Hour)
         {
           duplicate--;
         }
         if(ZTEHIS.ZTEMin==sHisFlashLog[i].HisFlashEvent[j].Minute)
         {
           duplicate--;
         }
         if(duplicate!=0) duplicate =5;
         else
         {
           j= sHisFlashLog[i].Count;
           i = ih+1; 
         }
      }
    }
    if(duplicate>0)
    {
        for(i=0; i<6; i++)
        {
          if(RecvBuff[2] == 0x30) // version 2.0
          {
            ZTEHIS.RectFail[i] = hex2byte(&RecvBuff[0],(i*4+33)); // 00H normal, 01H Fail
            if(ZTEHIS.RectFail[i]!=ZTEHIS.RectFail_old[i])
            {
                if(ZTEHIS.RectFail[i]!=0)
                {
                   sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =19;
                   NextHis();
                }
                else
                {
                   sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =20;
                   NextHis();
                }
            }
            ZTEHIS.RectFail_old[i]=ZTEHIS.RectFail[i];
          }
          else // version 2.1
          {
            ZTEHIS.RectFail[i] = hex2byte(&RecvBuff[0],(i*6+33)); // 00H normal, 01H Fail
            if(ZTEHIS.RectFail[i]!=ZTEHIS.RectFail_old[i])
            {
                if(ZTEHIS.RectFail[i]!=0)
                {
                   sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =19;
                   NextHis();
                }
                else
                {
                   sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =20;
                   NextHis();
                }
            }
            ZTEHIS.RectFail_old[i]=ZTEHIS.RectFail[i];
          }
        }
         for(i=0;i<(ih+1);i++)
         {
            if(HisCount_old[i] <sHisFlashLog[i].Count)
            {
              for(j=HisCount_old[i];j<(sHisFlashLog[i].Count);j++)
                {
                 
                    sHisFlashLog[i].HisFlashEvent[j].Year  =   (uint8_t)(ZTEHIS.ZTEYear-2000);
                    sHisFlashLog[i].HisFlashEvent[j].Month =  ZTEHIS.ZTEMon;
                    sHisFlashLog[i].HisFlashEvent[j].Day   =  ZTEHIS.ZTEDay;
                    sHisFlashLog[i].HisFlashEvent[j].Hour  =  ZTEHIS.ZTEHour;
                    sHisFlashLog[i].HisFlashEvent[j].Minute=  ZTEHIS.ZTEMin;
    //                sHisFlashLog[i].HisFlashEvent[j].Sec   =  ZTEHIS.ZTESec;
                }
              HisCount_old[i] = sHisFlashLog[i].Count;
            }
         }
    }

//        GetHistoryLog();
}
void GetDCDataLog(void)
{
    int duplicate=5;
    int i =0,j=0;
    ZTEHIS.ZTEYear = hex2byte2(&RecvBuff[0],17);
    ZTEHIS.ZTEMon =  hex2byte(&RecvBuff[0],21);
    ZTEHIS.ZTEDay =  hex2byte(&RecvBuff[0],23);
    ZTEHIS.ZTEHour = hex2byte(&RecvBuff[0],25);
    ZTEHIS.ZTEMin =  hex2byte(&RecvBuff[0],27);
    ZTEHIS.ZTESec =  hex2byte(&RecvBuff[0],29);
    for(i=0;i<=ih;i++)
    {
      for(j=0;j< sHisFlashLog[i].Count;j++)
      {
         if(ZTEHIS.ZTEYear-2000==sHisFlashLog[i].HisFlashEvent[j].Year)
         {
           duplicate--;
         }
         if(ZTEHIS.ZTEMon==sHisFlashLog[i].HisFlashEvent[j].Month)
         {
           duplicate--;
         }
         if(ZTEHIS.ZTEDay==sHisFlashLog[i].HisFlashEvent[j].Day)
         {
           duplicate--;
         }
         if(ZTEHIS.ZTEHour==sHisFlashLog[i].HisFlashEvent[j].Hour)
         {
           duplicate--;
         }
         if(ZTEHIS.ZTEMin==sHisFlashLog[i].HisFlashEvent[j].Minute)
         {
           duplicate--;
         }
         thao = duplicate;
         if(duplicate>0) duplicate =5;
         else
         {
           j= sHisFlashLog[i].Count;
           i = ih+1; 
         }
      }
    }
    if(duplicate>0)
    {
      if(RecvBuff[2] == 0x30) // version 2.0
      {
        for (i = 0; i < val; i++)
        {
            ZTEHIS.Load[i] = hex2byte(&RecvBuff[0],9 + 2*i+12+16);
            if((i<10)&&(ZTEHIS.Load[i] == 0x03))ZTEHIS.LoadFuse=1;
        }
        for (i = 0; i < 3; i++)
        {
           ZTEHIS.Batt[i] = hex2byte(&RecvBuff[0],((val+7)*2+1+i*6+12+16));
            if(ZTEHIS.Batt[i] == 0x03)
              {
                ZTEHIS.Battfuse = 1;
              }
        }
        ZTEHIS.LLVDHis = hex2byte(&RecvBuff[0],(val+5)*2+13+16);
        ZTEHIS.BLVDHis = hex2byte(&RecvBuff[0],(val+6)*2+13+16);
        ZTEHIS.BattHiTemp = hex2byte(&RecvBuff[0],(val+9)*2+13+16);  // Canh bao nhiet do acquy1 cao
        ZTEHIS.BattLowVolt[0]= hex2byte(&RecvBuff[0],(val+8)*2+13+16);
        ZTEHIS.BattLowVolt[1]= hex2byte(&RecvBuff[0],(val+11)*2+13+16);
      }
      else
      {
        for (i = 0; i < val; i++)
        {
            ZTEHIS.Load[i] = hex2byte(&RecvBuff[0],9 + 2*i+12+16);
            if((i<10)&&(ZTEHIS.Load[i] == 0x03))ZTEHIS.LoadFuse=1;
        }
        for (i = 0; i < 3; i++)
        {
           ZTEHIS.Batt[i] = hex2byte(&RecvBuff[0],((val+7)*2+1+i*10+12+16));
            if(ZTEHIS.Batt[i] == 0x03)
              {
                ZTEHIS.Battfuse = 1;
              }
        }
        ZTEHIS.LLVDHis = hex2byte(&RecvBuff[0],(val+5)*2+13+16);
        ZTEHIS.BLVDHis = hex2byte(&RecvBuff[0],(val+6)*2+13+16);
        ZTEHIS.BattHiTemp = hex2byte(&RecvBuff[0],(val+9)*2+13+16);  // Canh bao nhiet do acquy1 cao
        ZTEHIS.BattLowVolt[0]= hex2byte(&RecvBuff[0],(val+8)*2+13+16);
        ZTEHIS.BattLowVolt[1]= hex2byte(&RecvBuff[0],(val+13)*2+13+16);
      }
        if( (ZTEHIS.BattLowVolt[0]==1) || (ZTEHIS.BattLowVolt[1]==1))
          {
              ZTEHIS.DCLow = 1;
          }
        else
          {
              ZTEHIS.DCLow = 0;
          }
        if(ZTEHIS.LoadFuse!=ZTEHIS.LoadFuse_old)
        {
            if(ZTEHIS.LoadFuse!=0)
            {
               sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =23;
               NextHis();
            }
            else
            {
               sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =24;
               NextHis();
            }
        }
        if(ZTEHIS.Battfuse!=ZTEHIS.Battfuse_old)
        {
            if(ZTEHIS.Battfuse!=0)
            {
               sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =15;
               NextHis();
            }
            else
            {
               sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =16;
               NextHis();
            }
        }
        if(ZTEHIS.LLVDHis!=ZTEHIS.LLVDHis_old)
        {
            if(ZTEHIS.LLVDHis!=0)
            {
               sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =11;
               NextHis();
            }
            else
            {
               sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =12;
               NextHis();
            }
        }  
        
        if(ZTEHIS.BLVDHis!=ZTEHIS.BLVDHis_old)
        {
            if(ZTEHIS.BLVDHis!=0)
            {
               sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =13;
               NextHis();
            }
            else
            {
               sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =14;
               NextHis();
            }
        } 
        if(ZTEHIS.BattHiTemp!=ZTEHIS.BattHiTemp_old)
        {
            if(ZTEHIS.BattHiTemp!=0)
            {
               sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =17;
               NextHis();
            }
            else
            {
               sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =18;
               NextHis();
            }
        } 
        if(ZTEHIS.DCLow!=ZTEHIS.DCLow_old)
        {
            if(ZTEHIS.DCLow!=0)
            {
               sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =7;
               NextHis();
            }
            else
            {
               sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =8;
               NextHis();
            }
        } 
        ZTEHIS.LoadFuse_old=ZTEHIS.LoadFuse ;
        ZTEHIS.Battfuse_old=ZTEHIS.Battfuse;
        ZTEHIS.LLVDHis_old=ZTEHIS.LLVDHis ;
        ZTEHIS.BLVDHis_old=ZTEHIS.BLVDHis;
        ZTEHIS.BattHiTemp_old=ZTEHIS.BattHiTemp; 
        ZTEHIS.DCLow_old=ZTEHIS.DCLow;
         for(i=0;i<(ih+1);i++)
         {
            if(HisCount_old[i] <sHisFlashLog[i].Count)
            {
              for(j=HisCount_old[i];j<(sHisFlashLog[i].Count);j++)
                {
                 
                    sHisFlashLog[i].HisFlashEvent[j].Year  =   (uint8_t)(ZTEHIS.ZTEYear-2000);
                    sHisFlashLog[i].HisFlashEvent[j].Month =  ZTEHIS.ZTEMon;
                    sHisFlashLog[i].HisFlashEvent[j].Day   =  ZTEHIS.ZTEDay;
                    sHisFlashLog[i].HisFlashEvent[j].Hour  =  ZTEHIS.ZTEHour;
                    sHisFlashLog[i].HisFlashEvent[j].Minute=  ZTEHIS.ZTEMin;
    //                sHisFlashLog[i].HisFlashEvent[j].Sec   =  ZTEHIS.ZTESec;
                }
              HisCount_old[i] = sHisFlashLog[i].Count;
            }
         }

    }
             
        GetHistoryLog();
          for(i=0;i<5;i++)
             {
             if(sHisFlashLog[i].Count_old< sHisFlashLog[i].Count)
              {  
                pHisFlashLog =    &sHisFlashLog[i];
                switch(i)
                {
                case 0:
                  thao = sHisFlashLog[i].Count;
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
                   sFLASH_WriteBuffer((uint8_t *)pHisFlashLog, FLASH_PB3_LOG_START, 256);
                  break;
                default:
                  break;
                }
                sHisFlashLog[i].Count_old = sHisFlashLog[i].Count;

              }
            }
    
}
void GetEnvDataLog(void)
{
   int i =0,j=0;
    ZTEHIS.ZTEYear = hex2byte2(&RecvBuff[0],17);
    ZTEHIS.ZTEMon =  hex2byte(&RecvBuff[0],21);
    ZTEHIS.ZTEDay =  hex2byte(&RecvBuff[0],23);
    ZTEHIS.ZTEHour = hex2byte(&RecvBuff[0],25);
    ZTEHIS.ZTEMin =  hex2byte(&RecvBuff[0],27);
    ZTEHIS.ZTESec =  hex2byte(&RecvBuff[0],29);
    ZTEHIS.SmokeHis = hex2byte(&RecvBuff[0],25+16);
  
}