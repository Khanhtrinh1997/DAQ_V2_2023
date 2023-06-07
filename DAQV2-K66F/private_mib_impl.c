/**
 * @file private_mib_impl.c
 * @brief Private MIB module implementation
 *
 * @section License
 *
 * ^^(^____^)^^
 **/

//Dependencies
#include "mk66f18.h"
#include "frdm_k66f.h"
#include "core/net.h"
#include "private_mib_module.h"
#include "private_mib_impl.h"
#include "crypto.h"
#include "asn1.h"
#include "oid.h"
#include "debug.h"
#include "variables.h"
#include "Header.h"
#include "ftp_handler.h"

uint32_t setCount_test;
extern tConfigHandler configHandle;
//Mutex preventing simultaneous access to the private MIB base
static OsMutex privateMibMutex;


/**
 * @brief Private MIB module initialization
 * @return Error code
 **/

error_t privateMibInit(void)
{
  uint8_t i;
    //Debug message
    TRACE_INFO("Initializing private MIB base...\r\n");
    
     //Clear memory
    memset(&sLiionBattInfo, 0, 16 * sizeof(tLiionBattInfo));
    memset(&sGenInfo, 0, 2 * sizeof(tGenInfo));
    memset(&sBMInfo, 0, 16 * sizeof(tBMInfo));
    memset(&sVACInfo, 0, 1 * sizeof(tVACInfo));
    memset(&sPMInfo, 0, 1 * sizeof(tPMInfo));
    memset(&sSMCBInfo, 0, 5 * sizeof(sSMCBInfo)); //smcb
    
    //Clear private MIB base
    memset(&privateMibBase, 0, sizeof(privateMibBase));
    
    privateMibBase.cfgNetworkGroup.cSyncTimeLen = 20;
    sprintf(&privateMibBase.cfgNetworkGroup.cSyncTime[0], "12:00:00-20/02/18");
    
    privateMibBase.cfgNetworkGroup.siteNetworkInfo.u32IP = revertIP(g_sParameters.u32StaticIP);
    privateMibBase.cfgNetworkGroup.siteNetworkInfo.u32GW = revertIP(g_sParameters.u32GatewayIP);
    privateMibBase.cfgNetworkGroup.siteNetworkInfo.u32SN = revertIP(g_sParameters.u32SubnetMask);
    privateMibBase.cfgNetworkGroup.siteNetworkInfo.u32SIP = revertIP(g_sParameters.u32SnmpIP);
    privateMibBase.cfgNetworkGroup.u8TrapMode = g_sParameters.u8TrapMode;
    privateMibBase.cfgNetworkGroup.u32TrapInterval = g_sParameters.u32TrapPeriod;
    privateMibBase.siteGroup.siteControllerModel[0] = g_sParameters.sPort[0].u8ControllerType;
    privateMibBase.siteGroup.siteFirmwareCRC = g_sWorkingDefaultParameters2.u32FirmwareCRC;
    
    privateMibBase.pmGroup.pmTable[0].pmDeltaFreqDisConnect = (uint32_t)g_sParameters.u8DeltaFreqDisConnect[0];
    privateMibBase.pmGroup.pmTable[1].pmDeltaFreqDisConnect = (uint32_t)g_sParameters.u8DeltaFreqDisConnect[1];
    privateMibBase.pmGroup.pmTable[0].pmDeltaFreqReConnect  = (uint32_t)g_sParameters.u8DeltaFreqReConnect[0];
    privateMibBase.pmGroup.pmTable[1].pmDeltaFreqReConnect  = (uint32_t)g_sParameters.u8DeltaFreqReConnect[1];
    privateMibBase.pmGroup.pmTable[0].pmEnableFreqTrap      = (uint32_t)g_sParameters.u8EnableFreqTrap;
    privateMibBase.pmGroup.pmTable[0].pm_delta_freq_v       = (uint32_t)g_sParameters.pm_delta_freq_v;
    privateMibBase.pmGroup.pmTable[0].pm_cnt_timeout_freq_out   = g_sParameters.pm_cnt_timeout_freq_out;
    privateMibBase.pmGroup.pmTable[0].pm_cnt_timeout_freq_in    = g_sParameters.pm_cnt_timeout_freq_in;
    privateMibBase.pmGroup.pmTable[0].pm_set_cnt_remaining_gen  = g_sParameters.pm_set_cnt_remaining_gen;
    
    privateMibBase.isenseGroup.isenseTable.isenseDeltaFreqDisConnect = (uint32_t)g_sParameters.u8DeltaFreqDisConnect2;
    privateMibBase.isenseGroup.isenseTable.isenseDeltaFreqReConnect  = (uint32_t)g_sParameters.u8DeltaFreqReConnect2;
    privateMibBase.isenseGroup.isenseTable.isenseEnableFreqTrap      = (uint32_t)g_sParameters.u8EnableFreqTrap2;
    privateMibBase.isenseGroup.isenseTable.isense_delta_freq_v       = (uint32_t)g_sParameters.isense_delta_freq_v;
    privateMibBase.isenseGroup.isenseTable.isense_cnt_timeout_freq_out = g_sParameters.isense_cnt_timeout_freq_out;
    privateMibBase.isenseGroup.isenseTable.isense_cnt_timeout_freq_in  = g_sParameters.isense_cnt_timeout_freq_in;
    
    switch(privateMibBase.siteGroup.siteControllerModel[0])
    {
    case 0:
      {
         strcpy(privateMibBase.siteGroup.siteDCsystemVendor, "Emerson");
         privateMibBase.siteGroup.siteDCsystemVendorLen = sizeof(privateMibBase.siteGroup.siteDCsystemVendor);
      }break;
    case 1:
      {
         strcpy(privateMibBase.siteGroup.siteDCsystemVendor, "ZTE");
         privateMibBase.siteGroup.siteDCsystemVendorLen = sizeof(privateMibBase.siteGroup.siteDCsystemVendor);
      }break;
    case 2:
      {
         strcpy(privateMibBase.siteGroup.siteDCsystemVendor, "Agisson");
         privateMibBase.siteGroup.siteDCsystemVendorLen = sizeof(privateMibBase.siteGroup.siteDCsystemVendor);
      }break;
      case 3:
      {
         strcpy(privateMibBase.siteGroup.siteDCsystemVendor, "Emerson");
         privateMibBase.siteGroup.siteDCsystemVendorLen = sizeof(privateMibBase.siteGroup.siteDCsystemVendor);
      }break;
    case 4:
      {
         strcpy(privateMibBase.siteGroup.siteDCsystemVendor, "DPC");
         privateMibBase.siteGroup.siteDCsystemVendorLen = sizeof(privateMibBase.siteGroup.siteDCsystemVendor);
      }
      break;
    case 5:
      {
         strcpy(privateMibBase.siteGroup.siteDCsystemVendor, "Vertiv");
         privateMibBase.siteGroup.siteDCsystemVendorLen = sizeof(privateMibBase.siteGroup.siteDCsystemVendor);
      }
      break;      
    default:
      break;
    };

    strcpy(privateMibBase.siteGroup.siteDevModel, "DAQ-V2");
    privateMibBase.siteGroup.siteDevModelLen = 7;
    
    strcpy(privateMibBase.genGroup.genTable[0].genSerial, "   ");
    privateMibBase.genGroup.genTable[0].genSerialLen = 3;
    strcpy(privateMibBase.genGroup.genTable[1].genSerial, "   ");
    privateMibBase.genGroup.genTable[1].genSerialLen = 3;
   
    strcpy((char*)privateMibBase.cfgNetworkGroup.siteNetworkInfo.ucIP,ipv4AddrToString((Ipv4Addr)privateMibBase.cfgNetworkGroup.siteNetworkInfo.u32IP,0));
    strcpy((char*)privateMibBase.cfgNetworkGroup.siteNetworkInfo.ucGW,ipv4AddrToString((Ipv4Addr)privateMibBase.cfgNetworkGroup.siteNetworkInfo.u32GW,0));
    strcpy((char*)privateMibBase.cfgNetworkGroup.siteNetworkInfo.ucSN,ipv4AddrToString((Ipv4Addr)privateMibBase.cfgNetworkGroup.siteNetworkInfo.u32SN,0));
    strcpy((char*)privateMibBase.cfgNetworkGroup.siteNetworkInfo.ucSIP,ipv4AddrToString((Ipv4Addr)privateMibBase.cfgNetworkGroup.siteNetworkInfo.u32SIP,0));

    sAlarmValue[0].u8AlarmValue = 2;
    sAlarmValue[0].u8NormalValue = 0;
    for (i=1; i < 14;i++)
    {
      sAlarmValue[i].u8AlarmValue = 1;
      sAlarmValue[i].u8NormalValue = 0;      
    }
    sAlarmValue[9].u8AlarmValue = 0xff;
    sAlarmValue[9].u8NormalValue = 0;
    for (i=50; i < 55;i++)
    {
      sAlarmValue[i].u8AlarmValue = 1;
      sAlarmValue[i].u8NormalValue = 0;      
    }
    for (i=100; i < 115;i++)
    {
      sAlarmValue[i].u8AlarmValue = 1;
      sAlarmValue[i].u8NormalValue = 0;      
    }

    privateMibBase.siteGroup.siteTrapCounter = 0;
    strcpy((char*)privateMibBase.siteGroup.siteSerialNumber,(const char*) g_sWorkingDefaultParameters2.u8UserSerialNo);
    privateMibBase.siteGroup.siteSerialNumberLen = strlen(privateMibBase.siteGroup.siteSerialNumber);    
    
    strcpy((char*)privateMibBase.cfgNetworkGroup.cFTPServer,(const char*) g_sParameters.ucFTPServer);
    privateMibBase.cfgNetworkGroup.cFTPServerLen = sizeof(privateMibBase.cfgNetworkGroup.cFTPServer);
    
    snprintf(privateMibBase.siteGroup.siteMACInfo,18,"%02X:%02X:%02X:%02X:%02X:%02X",
             g_sWorkingDefaultParameters2.u8UserMAC[0],
             g_sWorkingDefaultParameters2.u8UserMAC[1],
             g_sWorkingDefaultParameters2.u8UserMAC[2],
             g_sWorkingDefaultParameters2.u8UserMAC[3],
             g_sWorkingDefaultParameters2.u8UserMAC[4],
             g_sWorkingDefaultParameters2.u8UserMAC[5]
               );
    if (g_sWorkingDefaultParameters2.u32FirmwareCRC_old != g_sWorkingDefaultParameters2.u32FirmwareCRC)
    {
      privateMibBase.cfgNetworkGroup.u8NewFWFlag = 1;
      g_sWorkingDefaultParameters2.u32FirmwareCRC_old = g_sWorkingDefaultParameters2.u32FirmwareCRC;      
      u8IsRewriteSN = 1;
    }
//    strcpy(privateMibBase.siteGroup.siteMACInfo, "00:1a:b6:22:03:91");
    privateMibBase.siteGroup.siteMACInfoLen = strlen(privateMibBase.siteGroup.siteMACInfo);
    strcpy(privateMibBase.siteGroup.siteBTSCode,(const char*) g_sParameters.siteName);
    privateMibBase.siteGroup.siteBTSCodeLen = sizeof(privateMibBase.siteGroup.siteBTSCode);
    snprintf(privateMibBase.siteGroup.siteCRCStr,11,"0x%x",
             privateMibBase.siteGroup.siteFirmwareCRC);
    privateMibBase.siteGroup.siteCRCStrLen = 11;

    privateMibBase.acPhaseGroup.acPhaseNumber = 3;
    for (i = 0; i < 4; i++)
    {
      strcpy(privateMibBase.rectGroup.rectTable[i].rectType, "DC48V");
      privateMibBase.rectGroup.rectTable[i].rectTypeLen = strlen(privateMibBase.rectGroup.rectTable[i].rectType);     
    }
//    privateMibBase.acPhaseGroup.acPhaseTable[0].acPhaseIndex = 3;
//    privateMibBase.acPhaseGroup.acPhaseTable[0].acPhaseVolt = 222;
#if (USERDEF_MONITOR_LIB == ENABLED)          
    if(g_sParameters.u8DevNum[TOTAL_LIB_INDEX] <= 16)
    {
      sModbusManager.u8NumberOfLIB = g_sParameters.u8DevNum[TOTAL_LIB_INDEX];                
      privateMibBase.liBattGroup.liBattInstalledPack = sModbusManager.u8NumberOfLIB;
      for (i = 0; i < privateMibBase.liBattGroup.liBattInstalledPack; i++)
      {
        sModbusManager.sLIBManager[i].u8LIBType = g_sParameters.sModbusParameters[i].u8DevVendor;
        sModbusManager.sLIBManager[i].u32LIBAbsSlaveID = g_sParameters.sModbusParameters[i].u32SlaveID;
        switch(sModbusManager.sLIBManager[i].u8LIBType)
        {
          case 13:// COSLIGHT_CF4850T
          case 1:// COSLIGHT
          {
              sModbusManager.sLIBManager[i].u32LIBSlaveOffset = 0;
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[0] = 0;      // FC 04
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[0] = 58;      
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[1] = 120;//128;    // FC 02
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[1] = 38;  //30
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[2] = 48;    // FC 03
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[2] = 4;
          }	
          break;
          case 2:// COSLIGHT_OLD_V1.1
          {
              sModbusManager.sLIBManager[i].u32LIBSlaveOffset = 0;
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[0] = 30;      // FC 04
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[0] = 24;      
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[1] = 128;    // FC 02
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[1] = 28;
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[2] = 48;    // FC 03
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[2] = 2;
          }	
          break;
          case 12:// SHOTO_SDA10_48100
          case 3:// SHOTO_2019
          {
              sModbusManager.sLIBManager[i].u32LIBSlaveOffset = 0;
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[0] = 0x1000;      // FC 04
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[0] = 10;      
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[1] = 0x2000;    
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[1] = 25;
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[2] = 0x3040;    
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[2] = 56;
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[3] = 0x800f;    
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[3] = 16;
          }	
          break;
          case 4:// HUAWEI
          {
              if(i<8)
                sModbusManager.sLIBManager[i].u32LIBSlaveOffset = 213;
              else
                sModbusManager.sLIBManager[i].u32LIBSlaveOffset = 215;
//              sModbusManager.sLIBManager[i].u32LIBAbsSlaveID = sModbusManager.sLIBManager[i].u32LIBSlaveOffset + i + 1;
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[0] = 0;
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[0] = 50;
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[1] = 67;
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[1] = 8;
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[2] = 257;
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[2] = 7;
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[3] = 529;
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[3] = 10;
          }
          break;  
          case 8:// HUAWEI_A1
          {
              if(i<8)
                sModbusManager.sLIBManager[i].u32LIBSlaveOffset = 213;
              else
                sModbusManager.sLIBManager[i].u32LIBSlaveOffset = 215;
//              sModbusManager.sLIBManager[i].u32LIBAbsSlaveID = sModbusManager.sLIBManager[i].u32LIBSlaveOffset + i + 1;
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[0] = 0;
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[0] = 50;
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[1] = 67;
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[1] = 8;
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[2] = 257;
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[2] = 7;
          }
          break;  
          case 5:// M1Viettel50
          {
              sModbusManager.sLIBManager[i].u32LIBSlaveOffset = 0;
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[0] = 0; // FC:0x04
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[0] = 100;   
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[1] = 130;// FC:0x04
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[1] = 22; //20
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[2] = 0;// FC:0x03
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[2] = 80; 
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[3] = 162;// FC:0x03
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[3] = 3;//2   
          }
          break;          
          case 14:// HUAFU_HF48100C
          case 6:// ZTT_2020   
          {
              sModbusManager.sLIBManager[i].u32LIBSlaveOffset = 0;
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[0] = 0;
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[0] = 42;
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[1] = 150;
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[1] = 10;
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[2] = 170;
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[2] = 10;
          }
          break;       
          case 9:// SAFT
          {
              sModbusManager.sLIBManager[i].u32LIBSlaveOffset = 0;
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[0] = 23764;
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[0] = 16;
          }
          break;
          case 10:// Narada75
          {
              sModbusManager.sLIBManager[i].u32LIBSlaveOffset = 38;
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[0] = 4095;
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[0] = 9;
          }
          break;
          case 7:// ZTT50
          {
              sModbusManager.sLIBManager[i].u32LIBSlaveOffset = 38;
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[0] = 4096;
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[0] = 9;
          }
          break;
          case 11:// EVE
          {
              sModbusManager.sLIBManager[i].u32LIBSlaveOffset = 0;
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[0] = 0;
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[0] = 42;
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[1] = 150;
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[1] = 10;
              sModbusManager.sLIBManager[i].u32LIBBaseAddr[2] = 170;
              sModbusManager.sLIBManager[i].u8LIBNumOfReg[2] = 10;
          }
          break;
         };
      }      
    }
#endif
#if (USERDEF_MONITOR_GEN == ENABLED)      
    if(g_sParameters.u8DevNum[TOTAL_GEN_INDEX] <= 2)
    {                
      sModbusManager.u8NumberOfGen = g_sParameters.u8DevNum[TOTAL_GEN_INDEX];
      privateMibBase.genGroup.genInstalledGen = sModbusManager.u8NumberOfGen;
      for (i = 0; i < privateMibBase.genGroup.genInstalledGen; i++)
      {
        sModbusManager.sGenManager[i].u8GenType = g_sParameters.sModbusParameters[i+MAX_NUM_OF_LIB].u8DevVendor;;
        sModbusManager.sGenManager[i].u32GenAbsSlaveID = g_sParameters.sModbusParameters[i+MAX_NUM_OF_LIB].u32SlaveID;;
        switch(sModbusManager.sGenManager[i].u8GenType)
        {
          case 1:// KUBOTA
          {
              sModbusManager.sGenManager[i].u32GenSlaveOffset = 60;
              sModbusManager.sGenManager[i].u32GenBaseAddr[0] = 0;
              sModbusManager.sGenManager[i].u8GenNumOfReg[0] = 85;
              
              sModbusManager.sGenManager[i].u32GenBaseAddr[1] = 235;
              sModbusManager.sGenManager[i].u8GenNumOfReg[1] = 1;
              sModbusManager.sGenManager[i].u8GenRunningStep = GEN_KUBOTA_INFO_1;
          }
          break;  
          case 2:// BE142
          {
              sModbusManager.sGenManager[i].u32GenSlaveOffset = 60;
              sModbusManager.sGenManager[i].u32GenBaseAddr[0] = 30010;
              sModbusManager.sGenManager[i].u8GenNumOfReg[0] = 38;
              
              sModbusManager.sGenManager[i].u32GenBaseAddr[1] = 30135;
              sModbusManager.sGenManager[i].u8GenNumOfReg[1] = 4;
              
              sModbusManager.sGenManager[i].u32GenBaseAddr[1] = 30079;
              sModbusManager.sGenManager[i].u8GenNumOfReg[1] = 27;
              
              sModbusManager.sGenManager[i].u8GenRunningStep = GEN_BE142_INFO_1;
          }
          break; 
          case 3:// DEEPSEA
          {
              sModbusManager.sGenManager[i].u32GenSlaveOffset = 60;
              sModbusManager.sGenManager[i].u32GenBaseAddr[0] = 0x400;
              sModbusManager.sGenManager[i].u8GenNumOfReg[0] = 34;
              
              sModbusManager.sGenManager[i].u32GenBaseAddr[1] = 0x700;
              sModbusManager.sGenManager[i].u8GenNumOfReg[1] = 8;
              
              sModbusManager.sGenManager[i].u32GenBaseAddr[2] = 0x800;
              sModbusManager.sGenManager[i].u8GenNumOfReg[2] = 8;
              
              sModbusManager.sGenManager[i].u32GenBaseAddr[3] = 0x304;
              sModbusManager.sGenManager[i].u8GenNumOfReg[3] = 1;
              sModbusManager.sGenManager[i].u8GenRunningStep = GEN_DEEPSEA_INFO_1;
          }
          break; 
          case 4:// LR2057
          {
              sModbusManager.sGenManager[i].u32GenSlaveOffset = 60;
              sModbusManager.sGenManager[i].u32GenBaseAddr[0] = 6;
              sModbusManager.sGenManager[i].u8GenNumOfReg[0] = 19;
              
              sModbusManager.sGenManager[i].u32GenBaseAddr[1] = 42;
              sModbusManager.sGenManager[i].u8GenNumOfReg[1] = 13;
//              
              sModbusManager.sGenManager[i].u32GenBaseAddr[2] = 12;
              sModbusManager.sGenManager[i].u8GenNumOfReg[2] = 66;
              sModbusManager.sGenManager[i].u8GenRunningStep = GEN_LR2057_INFO_1;
          }
          break; 
          case 5:// HIMOINSA
          {
              sModbusManager.sGenManager[i].u32GenSlaveOffset = 86;
              sModbusManager.sGenManager[i].u32GenBaseAddr[0] = 0;
              sModbusManager.sGenManager[i].u8GenNumOfReg[0] = 23;
              
              sModbusManager.sGenManager[i].u32GenBaseAddr[1] = 7;
              sModbusManager.sGenManager[i].u8GenNumOfReg[1] = 36;
           
              sModbusManager.sGenManager[i].u8GenRunningStep = GEN_HIMOINSA_INFO_1;
          }
          break; 
          case 6:// QC315
          {
              sModbusManager.sGenManager[i].u32GenSlaveOffset = 60;
              
              sModbusManager.sGenManager[i].u32GenBaseAddr[0] = 20;
              sModbusManager.sGenManager[i].u8GenNumOfReg[0] = 61;
              
              sModbusManager.sGenManager[i].u32GenBaseAddr[1] = 708;
              sModbusManager.sGenManager[i].u8GenNumOfReg[1] = 6;
              
              sModbusManager.sGenManager[i].u32GenBaseAddr[2] = 800;
              sModbusManager.sGenManager[i].u8GenNumOfReg[2] = 7;
              sModbusManager.sGenManager[i].u8GenRunningStep = GEN_QC315_INFO_1;
          }
          break; 
          case 7:// CUMMIN
          {
              sModbusManager.sGenManager[i].u32GenSlaveOffset = 60;
              
              sModbusManager.sGenManager[i].u32GenBaseAddr[0] = 12;
              sModbusManager.sGenManager[i].u8GenNumOfReg[0] = 60;
//              
//              sModbusManager.sGenManager[i].u32GenBaseAddr[1] = 708;
//              sModbusManager.sGenManager[i].u8GenNumOfReg[1] = 6;
//              
//              sModbusManager.sGenManager[i].u32GenBaseAddr[2] = 800;
//              sModbusManager.sGenManager[i].u8GenNumOfReg[2] = 7;
              sModbusManager.sGenManager[i].u8GenRunningStep = GEN_CUMMIN_INFO_1;
          }
          break; 
        };
      }
    }
#endif
#if (USERDEF_MONITOR_BM == ENABLED)  
    //========================== RS485 Batt Monitor Initialization ===========================//
    if(g_sParameters.u8DevNum[TOTAL_BM_INDEX] <= 16)
    {    
      sModbusManager.u8NumberOfBM = g_sParameters.u8DevNum[TOTAL_BM_INDEX];                
      privateMibBase.bmGroup.bmInstalledBM = sModbusManager.u8NumberOfBM;
      for (i = 0; i < privateMibBase.bmGroup.bmInstalledBM; i++)
      {
        sModbusManager.sBMManager[i].u8BMType = g_sParameters.sModbusParameters[i+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN].u8DevVendor;//1
        sModbusManager.sBMManager[i].u32BMAbsSlaveID = g_sParameters.sModbusParameters[i+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN].u32SlaveID;//i+70
        switch(sModbusManager.sBMManager[i].u8BMType)
        {
          case 1:// VIETTEL
          {
              sModbusManager.sBMManager[i].u32BMSlaveOffset = 69;
              sModbusManager.sBMManager[i].u32BMBaseAddr[0] = 0;
              sModbusManager.sBMManager[i].u8BMNumOfReg[0] = 56;
              sModbusManager.sBMManager[i].u8BMRunningStep = BM_VIETTEL_INFO_1;
          }
          break;     
         };
      } 
    }
    //========================== RS485 Batt Monitor Initialization ===========================//  
#endif
#if (USERDEF_MONITOR_PM == ENABLED)  
    //========================== RS485 Power Meter Initialization ===========================//
    privateMibBase.pmGroup.pmTable[0].pmSerialLen = 1;
    sprintf(&privateMibBase.pmGroup.pmTable[0].pmSerial[0], "");
    sModbusManager.u8PMBootPassFlag = 0;

    if(g_sParameters.u8DevNum[TOTAL_PM_INDEX] <= 2)
    {                
      sModbusManager.u8NumberOfPM = g_sParameters.u8DevNum[TOTAL_PM_INDEX];
      privateMibBase.pmGroup.pmInstalledPM = sModbusManager.u8NumberOfPM;
      for (i = 0; i < privateMibBase.pmGroup.pmInstalledPM; i++)
      {
        sModbusManager.sPMManager[i].u8PMType = g_sParameters.sModbusParameters[i+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM].u8DevVendor;  
        sModbusManager.sPMManager[i].u32PMAbsSlaveID = g_sParameters.sModbusParameters[i+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM].u32SlaveID;     
        switch(sModbusManager.sPMManager[i].u8PMType)   //switch(sModbusManager.sPMManager[i].u8PMType)
        {
          case 1:// FINECO
          {
              sModbusManager.sPMManager[i].u32PMSlaveOffset = 16;
              sModbusManager.sPMManager[i].u32PMBaseAddr[0] = 16;
              sModbusManager.sPMManager[i].u8PMNumOfReg[0] = 50;
              sModbusManager.sPMManager[i].u32PMBaseAddr[1] = 0x700;
              sModbusManager.sPMManager[i].u8PMNumOfReg[1] = 2;
              sModbusManager.sPMManager[i].u8PMRunningStep = PM_FINECO_INFO_1;
          }
          break;  
          case 2:// ASCENT
          {
              sModbusManager.sPMManager[i].u32PMSlaveOffset = 16;
              sModbusManager.sPMManager[i].u32PMBaseAddr[0] = 0;
              sModbusManager.sPMManager[i].u8PMNumOfReg[0] = 2;
              sModbusManager.sPMManager[i].u32PMBaseAddr[1] = 100;
              sModbusManager.sPMManager[i].u8PMNumOfReg[1] = 2;
              sModbusManager.sPMManager[i].u32PMBaseAddr[2] = 106;
              sModbusManager.sPMManager[i].u8PMNumOfReg[2] = 2;
              sModbusManager.sPMManager[i].u32PMBaseAddr[3] = 118;
              sModbusManager.sPMManager[i].u8PMNumOfReg[3] = 2;
              sModbusManager.sPMManager[i].u32PMBaseAddr[4] = 142;
              sModbusManager.sPMManager[i].u8PMNumOfReg[4] = 2;
              sModbusManager.sPMManager[i].u32PMBaseAddr[5] = 144;
              sModbusManager.sPMManager[i].u8PMNumOfReg[5] = 2;
              sModbusManager.sPMManager[i].u8PMRunningStep = PM_ASCENT_INFO_1;
          }
          break;    
          case 3:// EASTRON
          {
              sModbusManager.sPMManager[i].u32PMSlaveOffset = 16;
              sModbusManager.sPMManager[i].u32PMBaseAddr[0] = 0;
              sModbusManager.sPMManager[i].u8PMNumOfReg[0] = 32;
              sModbusManager.sPMManager[i].u32PMBaseAddr[1] = 70;
              sModbusManager.sPMManager[i].u8PMNumOfReg[1] = 10;
              sModbusManager.sPMManager[i].u32PMBaseAddr[2] = 342;
              sModbusManager.sPMManager[i].u8PMNumOfReg[2] = 4;
              sModbusManager.sPMManager[i].u32PMBaseAddr[3] = 64512;
              sModbusManager.sPMManager[i].u8PMNumOfReg[3] = 2;
              sModbusManager.sPMManager[i].u8PMRunningStep = PM_EASTRON_INFO_1;
          }
          break;    
          case 4:// CET1
          {
              sModbusManager.sPMManager[i].u32PMSlaveOffset = 52;
              sModbusManager.sPMManager[i].u32PMBaseAddr[0] = 0;
              sModbusManager.sPMManager[i].u8PMNumOfReg[0] = 14;
              sModbusManager.sPMManager[i].u32PMBaseAddr[1] = 40;
              sModbusManager.sPMManager[i].u8PMNumOfReg[1] = 18;
              sModbusManager.sPMManager[i].u32PMBaseAddr[2] = 9800;
              sModbusManager.sPMManager[i].u8PMNumOfReg[2] = 27;
              sModbusManager.sPMManager[i].u8PMRunningStep = PM_CET1_INFO_1;
          }
          break;  
        case 5://PILOT
          {
              sModbusManager.sPMManager[i].u32PMSlaveOffset = 16;   
              sModbusManager.sPMManager[i].u32PMBaseAddr[0] = 0;
              sModbusManager.sPMManager[i].u8PMNumOfReg[0] = 17;
              sModbusManager.sPMManager[i].u32PMBaseAddr[1] = 19;
              sModbusManager.sPMManager[i].u8PMNumOfReg[1] = 6;              
              sModbusManager.sPMManager[i].u8PMRunningStep = PM_PILOT_INFO_1;
          }
          break; 
        case 15://PILOT_3PHASE
          {
              sModbusManager.sPMManager[i].u32PMSlaveOffset = 16;   
              sModbusManager.sPMManager[i].u32PMBaseAddr[0] = 100;
              sModbusManager.sPMManager[i].u8PMNumOfReg[0] = 43;
              sModbusManager.sPMManager[i].u32PMBaseAddr[1] = 1000;
              sModbusManager.sPMManager[i].u8PMNumOfReg[1] = 12;   
              sModbusManager.sPMManager[i].u32PMBaseAddr[2] = 9000;
              sModbusManager.sPMManager[i].u8PMNumOfReg[2] = 2;            
              sModbusManager.sPMManager[i].u8PMRunningStep = PM_PILOT_3PHASE_INFO_1;
          }
          break;  
        case 16://YADA_3PHASE_DPC
          {
              sModbusManager.sPMManager[i].u32PMSlaveOffset  = 16;
              sModbusManager.sPMManager[i].u32PMBaseAddr[0]  = 4240;//0x03 U
              sModbusManager.sPMManager[i].u8PMNumOfReg[0]   = 6;
              sModbusManager.sPMManager[i].u32PMBaseAddr[1]  = 4288;//0x03 I
              sModbusManager.sPMManager[i].u8PMNumOfReg[1]   = 6;
              sModbusManager.sPMManager[i].u32PMBaseAddr[2]  = 4496;//0x03 cosphi
              sModbusManager.sPMManager[i].u8PMNumOfReg[2]   = 4;
              sModbusManager.sPMManager[i].u32PMBaseAddr[3]  = 4528;//0x03 HZ
              sModbusManager.sPMManager[i].u8PMNumOfReg[3]   = 1;
              sModbusManager.sPMManager[i].u32PMBaseAddr[4]  = 4192;//0x03 kvarh
              sModbusManager.sPMManager[i].u8PMNumOfReg[4]   = 2;
              sModbusManager.sPMManager[i].u32PMBaseAddr[5]  = 4144;//0x03 kwh
              sModbusManager.sPMManager[i].u8PMNumOfReg[5]   = 2;
              sModbusManager.sPMManager[i].u32PMBaseAddr[6]  = 4352;//0x03 P
              sModbusManager.sPMManager[i].u8PMNumOfReg[6]   = 8;
              sModbusManager.sPMManager[i].u32PMBaseAddr[7]  = 4416;//0x03 Q
              sModbusManager.sPMManager[i].u8PMNumOfReg[7]   = 8;
              sModbusManager.sPMManager[i].u8PMRunningStep   = PM_YADA_3PHASE_DPC_INFO_1;
          }
          break;
        case 17://Schneider 2022
          {
             sModbusManager.sPMManager[i].u32PMSlaveOffset  = 16;
              
            sModbusManager.sPMManager[i].u32PMBaseAddr[0]  = 29;
            sModbusManager.sPMManager[i].u8PMNumOfReg[0]   = 20; 
            
            sModbusManager.sPMManager[i].u32PMBaseAddr[1]  = 128;
            sModbusManager.sPMManager[i].u8PMNumOfReg[1]   = 3;  
            
            sModbusManager.sPMManager[i].u32PMBaseAddr[2]  = 2699; //E
            sModbusManager.sPMManager[i].u8PMNumOfReg[2]   = 24;
            
            sModbusManager.sPMManager[i].u32PMBaseAddr[3]  = 2999;
            sModbusManager.sPMManager[i].u8PMNumOfReg[3]   = 6; 
            
            sModbusManager.sPMManager[i].u32PMBaseAddr[4]  = 3027;
            sModbusManager.sPMManager[i].u8PMNumOfReg[4]   = 6;   
            
            sModbusManager.sPMManager[i].u32PMBaseAddr[5]  = 3053;
            sModbusManager.sPMManager[i].u8PMNumOfReg[5]   = 24;   
                    
            sModbusManager.sPMManager[i].u32PMBaseAddr[6]  = 3077; 
            sModbusManager.sPMManager[i].u8PMNumOfReg[6]   = 8;
            
            sModbusManager.sPMManager[i].u32PMBaseAddr[7]  = 3109; // F
            sModbusManager.sPMManager[i].u8PMNumOfReg[7]   = 2; 
                         
            
            sModbusManager.sPMManager[i].u8PMRunningStep   = PM_SCHNEDER_3PHASE_INFO_1;
          }
          break;  
        case 18: //EASTRON SMD72D 2022
          {
            sModbusManager.sPMManager[i].u32PMSlaveOffset  = 16;
 
                    
            sModbusManager.sPMManager[i].u32PMBaseAddr[0]  = 0;
            sModbusManager.sPMManager[i].u8PMNumOfReg[0]   = 30;
                    
            sModbusManager.sPMManager[i].u32PMBaseAddr[1]  = 30;
            sModbusManager.sPMManager[i].u8PMNumOfReg[1]   = 30;                    
                    
            sModbusManager.sPMManager[i].u32PMBaseAddr[2]  = 60;
            sModbusManager.sPMManager[i].u8PMNumOfReg[2]   = 30;
            
            sModbusManager.sPMManager[i].u32PMBaseAddr[3]  = 342;
            sModbusManager.sPMManager[i].u8PMNumOfReg[3]   = 30;   
            
            sModbusManager.sPMManager[i].u32PMBaseAddr[4]  = 11;  //Fn03
            sModbusManager.sPMManager[i].u8PMNumOfReg[4]   = 30;     
            
             sModbusManager.sPMManager[i].u32PMBaseAddr[5]  = 64512;  //Fn03
            sModbusManager.sPMManager[i].u8PMNumOfReg[5]   = 4;                
            sModbusManager.sPMManager[i].u8PMRunningStep   = PM_EASTRON_SDM72D_INFO_1;
          }
          break;         
        };
      }
    }
    
    //========================== RS485 Power Meter Initialization ===========================//
#endif   
#if (USERDEF_MONITOR_VAC == ENABLED)  
    //========================== RS485 VAC Initialization ===========================//
    
    if(g_sParameters.u8DevNum[TOTAL_VAC_INDEX] <= 2)
    {                
      sModbusManager.u8NumberOfVAC = g_sParameters.u8DevNum[TOTAL_VAC_INDEX];         
      privateMibBase.vacGroup.vacInstalledVAC = sModbusManager.u8NumberOfVAC;
      for (i = 0; i < privateMibBase.vacGroup.vacInstalledVAC; i++)
      {
        sModbusManager.sVACManager[i].u8VACType = g_sParameters.sModbusParameters[i+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM].u8DevVendor;
        sModbusManager.sVACManager[i].u32VACAbsSlaveID = g_sParameters.sModbusParameters[i+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM].u32SlaveID;
        switch(sModbusManager.sVACManager[i].u8VACType)
        {
          case 1:// VIETTEL
          {
              sModbusManager.sVACManager[i].u32VACSlaveOffset = 34;
              sModbusManager.sVACManager[i].u32VACBaseAddr[0] = 0;
              sModbusManager.sVACManager[i].u8VACNumOfReg[0] = 14;//22
              
              sModbusManager.sVACManager[i].u32VACBaseAddr[1] = 0;
              sModbusManager.sVACManager[i].u8VACNumOfReg[1] = 39;//29;//94;
              
              sModbusManager.sVACManager[i].u32VACBaseAddr[2] = 80;
              sModbusManager.sVACManager[i].u8VACNumOfReg[2] = 3;//29;//94;
              sModbusManager.sVACManager[i].u8VACRunningStep = VAC_VIETTEL_INFO_1;
          }
          break;     
         };
      } 
    }    
    //========================== RS485  Initialization ===========================//    
#endif
#if (USERDEF_MONITOR_FUEL == ENABLED)  
    //========================== RS485 FUEL Initialization ===========================//
    
    if(g_sParameters.u8DevNum[TOTAL_FUEL_INDEX] <= 2)
    {                
      sModbusManager.u8NumberOfFUEL = g_sParameters.u8DevNum[TOTAL_FUEL_INDEX];         
      privateMibBase.fuelGroup.FuelInstalledFUEL = sModbusManager.u8NumberOfFUEL;
      for (i = 0; i < privateMibBase.fuelGroup.FuelInstalledFUEL; i++)
      {
        sModbusManager.sFUELManager[i].u8FUELType = g_sParameters.sModbusParameters[i+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB].u8DevVendor;
        sModbusManager.sFUELManager[i].u32FUELAbsSlaveID = g_sParameters.sModbusParameters[i+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB].u32SlaveID;
        switch(sModbusManager.sFUELManager[i].u8FUELType)
        {
          case 1:// HPT621
          {
              sModbusManager.sFUELManager[i].u32FUELSlaveOffset = 23;
              sModbusManager.sFUELManager[i].u32FUELBaseAddr[0] = 0;
              sModbusManager.sFUELManager[i].u8FUELNumOfReg[0] = 1;
             
              sModbusManager.sFUELManager[i].u8FUELRunningStep = FUEL_HPT621_INFO_1;
          }
          break;     
         };
      } 
    }    
    //========================== RS485 FUEL Initialization ===========================//    
#endif
#if (USERDEF_MONITOR_SMCB == ENABLED) // smcb  
    //========================== RS485 SMCB Initialization ===========================//    
    if(g_sParameters.u8DevNum[TOTAL_SMCB_INDEX] <= 5)
    {                
      sModbusManager.u8NumberOfSMCB = g_sParameters.u8DevNum[TOTAL_SMCB_INDEX];         
      privateMibBase.smcbGroup.SmcbInstalledSMCB = sModbusManager.u8NumberOfSMCB;
      for (i = 0; i < privateMibBase.smcbGroup.SmcbInstalledSMCB; i++)
      {
        sModbusManager.sSMCBManager[i].u8SMCBType = g_sParameters.sModbusParameters[i+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC].u8DevVendor;
        sModbusManager.sSMCBManager[i].u32SMCBAbsSlaveID = g_sParameters.sModbusParameters[i+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC].u32SlaveID;
        switch(sModbusManager.sSMCBManager[i].u8SMCBType)
        {
          case 1:// OPEN
          {
              sModbusManager.sSMCBManager[i].u32SMCBSlaveOffset = 18;
              sModbusManager.sSMCBManager[i].u32SMCBBaseAddr[0] = 0;
              sModbusManager.sSMCBManager[i].u8SMCBNumOfReg[0] = 1;

              sModbusManager.sSMCBManager[i].u8SMCBRunningStep = SMCB_OPEN_INFO_1;
          }
          break; 
          case 2:// MATIS
          {
              sModbusManager.sSMCBManager[i].u32SMCBSlaveOffset = 18;
              sModbusManager.sSMCBManager[i].u32SMCBBaseAddr[0] = 16;
              sModbusManager.sSMCBManager[i].u8SMCBNumOfReg[0] = 1;

              sModbusManager.sSMCBManager[i].u8SMCBRunningStep = SMCB_MATIS_INFO_1;
          }
          break;
          case 3:// GOL
          {
              sModbusManager.sSMCBManager[i].u32SMCBSlaveOffset = 18;
              sModbusManager.sSMCBManager[i].u32SMCBBaseAddr[0] = 32768;
              sModbusManager.sSMCBManager[i].u8SMCBNumOfReg[0] = 1;

              sModbusManager.sSMCBManager[i].u8SMCBRunningStep = SMCB_GOL_INFO_1;
          }
          break;    
         };
      } 
    }    
    //========================== RS485 SMCB Initialization ===========================//    
#endif
//RS485 ISENSE INITIALIZATION============================================================
#if (USERDEF_MONITOR_ISENSE == ENABLED)  
   
    privateMibBase.isenseGroup.isenseTable.isenseSerialLen = 1;
    sprintf(&privateMibBase.isenseGroup.isenseTable.isenseSerial[0], "");

    if(g_sParameters.u8DevNum[TOTAL_ISENSE_INDEX] <= 1)
    {                
      sModbusManager.u8NumberOfISENSE = g_sParameters.u8DevNum[TOTAL_ISENSE_INDEX];
      privateMibBase.isenseGroup.isenseInstalledISENSE = sModbusManager.u8NumberOfISENSE;
      for (i = 0; i < privateMibBase.isenseGroup.isenseInstalledISENSE; i++)
      {
        sModbusManager.sISENSEManager[i].u8ISENSEType = g_sParameters.sModbusParameters[i+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB+MAX_NUM_OF_FUEL].u8DevVendor;;  
        sModbusManager.sISENSEManager[i].u32ISENSEAbsSlaveID = g_sParameters.sModbusParameters[i+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB+MAX_NUM_OF_FUEL].u32SlaveID;;     
        switch(sModbusManager.sISENSEManager[i].u8ISENSEType) 
        {
        case 1://FORLONG
          {
            sModbusManager.sISENSEManager[i].u32ISENSESlaveOffset = 26;   
            sModbusManager.sISENSEManager[i].u32ISENSEBaseAddr[0] = 0;
            sModbusManager.sISENSEManager[i].u8ISENSENumOfReg[0] = 22;             
            sModbusManager.sISENSEManager[i].u8ISENSERunningStep = ISENSE_FORLONG_INFO_1;
          }
          break; 
        case 2:// IVY_DDS353H_2
          {
            sModbusManager.sISENSEManager[i].u32ISENSESlaveOffset  = 26;
            
            sModbusManager.sISENSEManager[i].u32ISENSEBaseAddr[0]  = 256;               
            sModbusManager.sISENSEManager[i].u8ISENSENumOfReg[0]   = 16;  
            
            sModbusManager.sISENSEManager[i].u32ISENSEBaseAddr[1]  = 290;               
            sModbusManager.sISENSEManager[i].u8ISENSENumOfReg[1]   = 36; 
//            
            sModbusManager.sISENSEManager[i].u32ISENSEBaseAddr[2]  = 4096;               
            sModbusManager.sISENSEManager[i].u8ISENSENumOfReg[2]   = 6; 
//            
//            sModbusManager.sISENSEManager[i].u32ISENSEBaseAddr[3]  = 328;               // Q
//            sModbusManager.sISENSEManager[i].u8ISENSENumOfReg[3]   = 2;                
//            
//            sModbusManager.sISENSEManager[i].u32ISENSEBaseAddr[4]  = 336;               // S
//            sModbusManager.sISENSEManager[i].u8ISENSENumOfReg[4]   = 2;  
//            
//            sModbusManager.sISENSEManager[i].u32ISENSEBaseAddr[5]  = 344;               // Factor
//            sModbusManager.sISENSEManager[i].u8ISENSENumOfReg[5]   = 1; 
//            
//            sModbusManager.sISENSEManager[i].u32ISENSEBaseAddr[6]  = 40960;             // PE
//            sModbusManager.sISENSEManager[i].u8ISENSENumOfReg[6]   = 2;  
//            
//            sModbusManager.sISENSEManager[i].u32ISENSEBaseAddr[7]  = 40990;             // QE
//            sModbusManager.sISENSEManager[i].u8ISENSENumOfReg[7]   = 2;    
//            
//            sModbusManager.sISENSEManager[i].u32ISENSEBaseAddr[8]  = 277;   //SN
//            sModbusManager.sISENSEManager[i].u8ISENSENumOfReg[8]   = 3;
            
            sModbusManager.sISENSEManager[i].u8ISENSERunningStep   = ISENSE_IVY_INFO_1;
          }
          break;
        default:
          break;
        };
      }
    }
#endif
//RS485 PM DC INITIALIZATION===============================================================
#if (USERDEF_MONITOR_PM_DC == ENABLED)
    if(g_sParameters.u8DevNum[TOTAL_PM_DC_INDEX] <= 1){
      sModbusManager.u8_number_of_pm_dc = g_sParameters.u8DevNum[TOTAL_PM_DC_INDEX];
      privateMibBase.pm_dc_group.pm_dc_installed_PM_DC = sModbusManager.u8_number_of_pm_dc;
      for (i = 0; i < privateMibBase.pm_dc_group.pm_dc_installed_PM_DC; i++)
      {
        sModbusManager.s_pm_dc_manager[i].u8_pm_dc_type = g_sParameters.sModbusParameters[i+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB+MAX_NUM_OF_FUEL+MAX_NUM_OF_ISENSE].u8DevVendor;  
        sModbusManager.s_pm_dc_manager[i].u32_pm_dc_abs_slaveID = g_sParameters.sModbusParameters[i+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB+MAX_NUM_OF_FUEL+MAX_NUM_OF_ISENSE].u32SlaveID;     
        switch(sModbusManager.s_pm_dc_manager[i].u8_pm_dc_type) 
        {
        case 1://YADA_DC
          {
             sModbusManager.s_pm_dc_manager[i].u32_pm_dc_slave_offset    = 30;
             sModbusManager.s_pm_dc_manager[i].u32_pm_dc_base_addr[0]    = 0;       //0x03
             sModbusManager.s_pm_dc_manager[i].u8_pm_dc_num_of_reg[0]    = 2;
             sModbusManager.s_pm_dc_manager[i].u32_pm_dc_base_addr[1]    = 12;       //0x03
             sModbusManager.s_pm_dc_manager[i].u8_pm_dc_num_of_reg[1]    = 2;
             sModbusManager.s_pm_dc_manager[i].u32_pm_dc_base_addr[2]    = 26;       //0x03
             sModbusManager.s_pm_dc_manager[i].u8_pm_dc_num_of_reg[2]    = 2;
             sModbusManager.s_pm_dc_manager[i].u32_pm_dc_base_addr[3]    = 40;       //0x03
             sModbusManager.s_pm_dc_manager[i].u8_pm_dc_num_of_reg[3]    = 2;
             sModbusManager.s_pm_dc_manager[i].u8_pm_dc_running_step     = PM_DC_YADA_INFO_1;
          }
          break; 
        };
      }
    }
    
#endif
    
    privateMibBase.cfgBTGroup.cfgBTPlanTestNumber = 4;
    privateMibBase.cfgBTGroup.cfgBTSCUNumber = 1;    

    //Create a mutex to prevent simultaneous access to the private MIB base
    if(!osCreateMutex(&privateMibMutex))
    {
        //Failed to create mutex
        return ERROR_OUT_OF_RESOURCES;
    }

    //Successful processing
    return NO_ERROR;
}


/**
 * @brief Lock private MIB base
 **/

void privateMibLock(void)
{
    //Enter critical section
    osAcquireMutex(&privateMibMutex);
}


/**
 * @brief Unlock private MIB base
 **/

void privateMibUnlock(void)
{
    //Leave critical section
    osReleaseMutex(&privateMibMutex);
}


/**
 * @brief Get currentTime object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t privateMibGetCurrentTime(const MibObject *object, const uint8_t *oid,
                                 size_t oidLen, MibVariant *value, size_t *valueLen)
{
    //Get object value
    value->timeTicks = osGetSystemTime() / 10;
    //Successful processing
    return NO_ERROR;
}

error_t privateMibSetStringEntry(const MibObject *object, const uint8_t *oid,
                                 size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    char_t *entry;

    //Point to the end of the OID prefix
    n = object->oidLen;

    //Get the instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Point to the LED table entry
    entry = &privateMibBase.testString[0];

    //ledState object?
    if(!strcmp(object->name, "testString"))
    {
        //Make sure the buffer is large enough to hold the entire object
        if(valueLen >= sizeof(privateMibBase.testString))
            return ERROR_BUFFER_OVERFLOW;

        //Copy object value
        memset(entry,0,sizeof(privateMibBase.testString));
        memcpy(entry, value->octetString, valueLen);
        //Debug message
        TRACE_ERROR("New testString:%s!\r\n",&privateMibBase.testString[0]);
    }
    //Unknown object?
    else
    {
        TRACE_ERROR("ERROR_OBJECT_NOT_FOUND!\r\n");
        //The specified object does not exist
        return ERROR_OBJECT_NOT_FOUND;
    }

    //Successful processing
    return NO_ERROR;
}
//
///**
// * @brief Set ledEntry object value
// * @param[in] object Pointer to the MIB object descriptor
// * @param[in] oid Object identifier (object name and instance identifier)
// * @param[in] oidLen Length of the OID, in bytes
// * @param[in] value Object value
// * @param[in] valueLen Length of the object value, in bytes
// * @return Error code
// **/
//
//error_t privateMibSetLedEntry(const MibObject *object, const uint8_t *oid,
//                              size_t oidLen, const MibVariant *value, size_t valueLen)
//{
//    error_t error;
//    size_t n;
//    uint_t index;
//    PrivateMibLedEntry *entry;
//
//    //Point to the end of the OID prefix
//    n = object->oidLen;
//
//    //Get the instance identifier
//    error = mibDecodeIndex(oid, oidLen, &n, &index);
//    //Invalid instance identifier?
//    if(error) return error;
//
//    //Check index range
//    if(index < 1 || index > PRIVATE_MIB_LED_COUNT)
//        return ERROR_INSTANCE_NOT_FOUND;
//
//    //Point to the LED table entry
//    entry = &privateMibBase.ledTable[index - 1];
//
//    //ledState object?
//    if(!strcmp(object->name, "ledState"))
//    {
//        //Set object value
//        entry->ledState = value->integer;
//
//        //Update LED state
//        if(index == 1)
//        {
//            if(entry->ledState)
//                GPIO_LED_R->PCOR = LED_R_MASK;
//            else
//                GPIO_LED_R->PSOR = LED_R_MASK;
//        }
//        else if(index == 2)
//        {
//            if(entry->ledState)
//                GPIO_LED_G->PCOR = LED_G_MASK;
//            else
//                GPIO_LED_G->PSOR = LED_G_MASK;
//        }
//        else if(index == 3)
//        {
//            if(entry->ledState)
//                GPIO_LED_B->PCOR = LED_B_MASK;
//            else
//                GPIO_LED_B->PSOR = LED_B_MASK;
//        }
//    }
//    //Unknown object?
//    else
//    {
//        //The specified object does not exist
//        return ERROR_OBJECT_NOT_FOUND;
//    }
//
//    //Successful processing
//    return NO_ERROR;
//}
//
//
///**
// * @brief Get ledEntry object value
// * @param[in] object Pointer to the MIB object descriptor
// * @param[in] oid Object identifier (object name and instance identifier)
// * @param[in] oidLen Length of the OID, in bytes
// * @param[out] value Object value
// * @param[in,out] valueLen Length of the object value, in bytes
// * @return Error code
// **/
//
//error_t privateMibGetLedEntry(const MibObject *object, const uint8_t *oid,
//                              size_t oidLen, MibVariant *value, size_t *valueLen)
//{
//    error_t error;
//    size_t n;
//    uint_t index;
//    PrivateMibLedEntry *entry;
//
//    //Point to the end of the OID prefix
//    n = object->oidLen;
//
//    //Get the instance identifier
//    error = mibDecodeIndex(oid, oidLen, &n, &index);
//    //Invalid instance identifier?
//    if(error) return error;
//
//    //Check index range
//    if(index < 1 || index > PRIVATE_MIB_LED_COUNT)
//        return ERROR_INSTANCE_NOT_FOUND;
//
//    //Point to the LED table entry
//    entry = &privateMibBase.ledTable[index - 1];
//
//    //ledColor object?
//    if(!strcmp(object->name, "ledColor"))
//    {
//        //Make sure the buffer is large enough to hold the entire object
//        if(*valueLen < entry->ledColorLen)
//            return ERROR_BUFFER_OVERFLOW;
//
//        //Copy object value
//        memcpy(value->octetString, entry->ledColor, entry->ledColorLen);
//        //Return object length
//        *valueLen = entry->ledColorLen;
//    }
//    //ledState object?
//    else if(!strcmp(object->name, "ledState"))
//    {
//        //Get object value
//        value->integer = entry->ledState;
//    }
//    //Unknown object?
//    else
//    {
//        //The specified object does not exist
//        return ERROR_OBJECT_NOT_FOUND;
//    }
//
//    //Successful processing
//    return NO_ERROR;
//}
//
//
///**
// * @brief Get next ledEntry object
// * @param[in] object Pointer to the MIB object descriptor
// * @param[in] oid Object identifier
// * @param[in] oidLen Length of the OID, in bytes
// * @param[out] nextOid OID of the next object in the MIB
// * @param[out] nextOidLen Length of the next object identifier, in bytes
// * @return Error code
// **/
//
//error_t privateMibGetNextLedEntry(const MibObject *object, const uint8_t *oid,
//                                  size_t oidLen, uint8_t *nextOid, size_t *nextOidLen)
//{
//    error_t error;
//    size_t n;
//    uint_t index;
//
//    //Make sure the buffer is large enough to hold the OID prefix
//    if(*nextOidLen < object->oidLen)
//        return ERROR_BUFFER_OVERFLOW;
//
//    //Copy OID prefix
//    memcpy(nextOid, object->oid, object->oidLen);
//
//    //Loop through LED table
//    for(index = 1; index <= PRIVATE_MIB_LED_COUNT; index++)
//    {
//        //Point to the end of the OID prefix
//        n = object->oidLen;
//
//        //Append the instance identifier to the OID prefix
//        error = mibEncodeIndex(nextOid, *nextOidLen, &n, index);
//        //Any error to report?
//        if(error) return error;
//
//        //Check whether the resulting object identifier lexicographically
//        //follows the specified OID
//        if(oidComp(nextOid, n, oid, oidLen) > 0)
//        {
//            //Save the length of the resulting object identifier
//            *nextOidLen = n;
//            //Next object found
//            return NO_ERROR;
//        }
//    }
//
//    //The specified OID does not lexicographically precede the name
//    //of some object
//    return ERROR_OBJECT_NOT_FOUND;
//}

//========================================== SiteInfo Function ==========================================//

TimeFormat SyncTime;
     

/**
 * @brief Set SiteInfoGroup object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[in] value Object value
 * @param[in] valueLen Length of the object value, in bytes
 * @return Error code
 **/
error_t privateMibSetSiteInfoGroup(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    uint8_t charIdx = 0;
    uint8_t setChar = 0;
    uint8_t token = 0;
    uint8_t buff[6][4];
    uint8_t const *pui8Char;
    PrivateMibSiteInfoGroup *entry;
    Ipv4Addr tempIP;
    
    memset(&tempIP,0,sizeof(Ipv4Addr));

    //Point to the end of the OID prefix
    n = object->oidLen;
//	Point to the siteInfoGroup entry
    entry = &privateMibBase.siteGroup;
    //siteSystemReset object?
    if(!strcmp(object->name, "siteSystemReset"))
    {
      if(value->integer==0x53)
      {
        //Get object value
        entry->siteSystemReset= value->integer;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //siteSNMPServer object?
    else if(!strcmp(object->name, "siteSNMPServer"))
    {
 
      //Make sure the buffer is large enough to hold the entire object
      if(valueLen > 15)
//      return ERROR_BUFFER_OVERFLOW;
        valueLen = 15;
      //===========xu ly dau cach nhanh ve===============
      uint32_t cntSIP;
      uint8_t tempSIP[16];
      strncpy((char*)tempSIP,(char*)value->octetString,valueLen); 
      for(cntSIP = 0; cntSIP < 16 ; cntSIP++)
      {
        if(tempSIP[cntSIP] == ' ') tempSIP[cntSIP] = '\0'; 
      }
      tempSIP[15] = '\0';
      //===========xu ly dau cach nhanh ve===============
      //Copy object value
      memset(privateMibBase.cfgNetworkGroup.siteNetworkInfo.ucSIP,0,16);
      strncpy((char*)privateMibBase.cfgNetworkGroup.siteNetworkInfo.ucSIP,(const char*)tempSIP,valueLen); 
      ipv4StringToAddr((const char*)&privateMibBase.cfgNetworkGroup.siteNetworkInfo.ucSIP[0],&tempIP);
             
      if  (g_sParameters.u32SnmpIP != revertIP(tempIP))
      {
        //
        // Update the current parameters with the new settings.
        //
        g_sParameters.u32SnmpIP = revertIP(tempIP);
        privateMibBase.cfgNetworkGroup.siteNetworkInfo.u32SIP = revertIP(g_sParameters.u32SnmpIP);

        // Yes - save these settings as the defaults.
        //
        g_sWorkingDefaultParameters.u32SnmpIP = g_sParameters.u32SnmpIP;
         
        u8SaveConfigFlag |= 1;  
      }
      
//      privateMibBase.cfgNetworkGroup.cFTPServerLen = valueLen;
    }
    //siteFTPServer object?
    else if(!strcmp(object->name, "siteFTPServer"))
    {
      //Make sure the buffer is large enough to hold the entire object
      if(valueLen > 40)
      return ERROR_BUFFER_OVERFLOW;

      //Copy object value
      memset(privateMibBase.cfgNetworkGroup.cFTPServer,0,40);
      strncpy(privateMibBase.cfgNetworkGroup.cFTPServer,(const char*)value->octetString,valueLen);    
      privateMibBase.cfgNetworkGroup.cFTPServerLen = valueLen;
      
      strncpy((char*)&g_sWorkingDefaultParameters.ucFTPServer[0],(char*)&privateMibBase.cfgNetworkGroup.cFTPServer[0],privateMibBase.cfgNetworkGroup.cFTPServerLen);
      strncpy((char*)&g_sParameters.ucFTPServer[0],(char*)&g_sWorkingDefaultParameters.ucFTPServer[0],privateMibBase.cfgNetworkGroup.cFTPServerLen);
      privateMibBase.cfgNetworkGroup.cFTPServer[privateMibBase.cfgNetworkGroup.cFTPServerLen] ='\0';
      g_sParameters.ucFTPServer[privateMibBase.cfgNetworkGroup.cFTPServerLen] ='\0';
      g_sWorkingDefaultParameters.ucFTPServer[privateMibBase.cfgNetworkGroup.cFTPServerLen] ='\0';
      
      u8SaveConfigFlag |= 1;
    }
    //siteFirmwareUpdate object?
    else if(!strcmp(object->name, "siteFirmwareUpdate"))
    {
      if((value->integer == 0x53)||(value->integer==0x00))
      {
        //Get object value
        entry->siteFirmwareUpdate= value->integer;
        if((u8FwUpdateCompleteFlag == 0) && (entry->siteFirmwareUpdate == 0x53))
        {
          //save Vac firm version--------------------------------------------------------------------
            if((configHandle.devType==_VAC_FW_)||(configHandle.devType==_VACV2_FW_))
            {
              configHandle.Check_vac_firm_version = privateMibBase.vacGroup.vacTable[0].vacFirmVersion;
            }else
            {
              configHandle.Check_vac_firm_version=0;
            }
        //flag update-------------------------------------------------------------------------------
            if(configHandle.devType==_NO_FW_UPDATE){
            }else{
              u8FwUpdateFlag=1;
            }
          
        }
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //siteFWType object?
    else if(!strcmp(object->name, "siteFWType"))
    {
      if((value->integer >= 0) && (value->integer <= 3))
      {
        //Get object value
        privateMibBase.siteGroup.siteFWType = value->integer;
        configHandle.devType = (eUpdateDevType)privateMibBase.siteGroup.siteFWType;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //siteTrapEnable object?
    else if(!strcmp(object->name, "siteTrapEnable"))
    {
      if((value->integer >= 0) && (value->integer <= 2))
      {
        tConfigParameters sConfigParams1;
        
        //Get object value
        privateMibBase.siteGroup.siteTrapEnable = value->integer;

        sConfigParams1.u8TrapMode = privateMibBase.siteGroup.siteTrapEnable;
                //
        // Did parameters change?
        //
        if  (g_sParameters.u8TrapMode != sConfigParams1.u8TrapMode)
        {
          //
          // Update the current parameters with the new settings.
          //
          g_sParameters.u8TrapMode = sConfigParams1.u8TrapMode;
          privateMibBase.cfgNetworkGroup.u8TrapMode = g_sParameters.u8TrapMode;
          //
          // Yes - save these settings as the defaults.
          //
          g_sWorkingDefaultParameters.u8TrapMode = g_sParameters.u8TrapMode;
           
          u8SaveConfigFlag |= 1;
        }
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //siteSyncTime object?
    else if(!strcmp(object->name, "siteSyncTime"))
    {
//        //Make sure the buffer is large enough to hold the entire object
//        if(40 >= valueLen)
//        {
//          pui8Char = &value->octetString[0];        
//          charIdx = 0;
//          for (uint8_t i = 0; i < valueLen; i++)
//          {
//            setChar = *pui8Char++;
//            switch (setChar)
//            {
//            case '/':
//            case ':':
//            case '-':
//              {
//                token++;
//                charIdx = 0;
//              }
//              break;
//            case ' ':
//              break;
//            default:
//              {
//                buff[token][charIdx]= setChar;
//                charIdx++;
//              }
//              break;
//            };
//          }
//       
//          SyncTime.hour = atol(&buff[0][0]);
//          SyncTime.min = atol(&buff[1][0]);
//          SyncTime.sec = atol(&buff[2][0]);
//          SyncTime.date = atol(&buff[3][0]);
//          SyncTime.month = atol(&buff[4][0]);
//          SyncTime.year = atol(&buff[5][0]);
//          if((SyncTime.sec > 59)||
//             (SyncTime.min > 59) || 
//             (SyncTime.hour > 23) || 
//             (SyncTime.year > 99)  || 
//             (SyncTime.month > 12) || 
//             (SyncTime.date > 31))
//          {
//            return ERROR_PARAMETER_OUT_OF_RANGE;
//          }
//          token = 0;
//          charIdx = 0;
//          memset(&buff, 0, sizeof(buff));
//          
//          sprintf(&privateMibBase.cfgNetworkGroup.cSyncTime[0], "%02d:%02d:%02d-%02d/%02d/%02d",
//                  SyncTime.hour, 
//                  SyncTime.min, 
//                  SyncTime.sec, 
//                  SyncTime.date, 
//                  SyncTime.month, 
//                  SyncTime.year);
//         
//        
//            //Copy object value
//  //          memcpy(entry->siteDCsystemVendor, value->octetString, valueLen);
//            //Return object length
//            privateMibBase.cfgNetworkGroup.cSyncTimeLen = 20;
//            configHandle.isSyncTime = 1;
//            sModbusManager.SettingCommand = SET_VAC_SYNCTIME;
//        }
//        else
//        {
//            //Report an error
//            error = ERROR_BUFFER_OVERFLOW;
//        }
      return ERROR_OBJECT_NOT_FOUND;
    }
    //Unknown object?
    else
    {
        TRACE_ERROR("ERROR_OBJECT_NOT_FOUND!\r\n");
        //The specified object does not exist
        return ERROR_OBJECT_NOT_FOUND;
    }

    //Successful processing
    return NO_ERROR;
}

/**
 * @brief Get siteInfo object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t privateMibGetSiteInfoGroup(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error = NO_ERROR;
    size_t n;
    PrivateMibSiteInfoGroup *entry;

    //	Point to the siteInfoGroup entry
    entry = &privateMibBase.siteGroup;

    //siteInfoBTSCode object?
    if(!strcmp(object->name, "siteBTSCode"))
    {
        //Make sure the buffer is large enough to hold the entire object
        if(*valueLen >= entry->siteBTSCodeLen)
        {
            //Copy object value
            memcpy(value->octetString, entry->siteBTSCode, entry->siteBTSCodeLen);
            //Return object length
            *valueLen = entry->siteBTSCodeLen;
        }
        else
        {
            //Report an error
            error = ERROR_BUFFER_OVERFLOW;
        }
    }
    //siteDCsystemVendor object?
    else if(!strcmp(object->name, "siteDCsystemVendor"))
    {
        //Make sure the buffer is large enough to hold the entire object
        if(*valueLen >= entry->siteDCsystemVendorLen)
        {
            //Copy object value
            memcpy(value->octetString, entry->siteDCsystemVendor, entry->siteDCsystemVendorLen);
            //Return object length
            *valueLen = entry->siteDCsystemVendorLen;
        }
        else
        {
            //Report an error
            error = ERROR_BUFFER_OVERFLOW;
        }
    }
    //siteControllerModel object?
    else if(!strcmp(object->name, "siteControllerModel"))
    {
        //Get object value
        value->integer = privateMibBase.siteGroup.siteControllerModel[0];
    }
    //siteCRCStr object?
    else if(!strcmp(object->name, "siteCRCStr"))
    {
        //Make sure the buffer is large enough to hold the entire object
        if(*valueLen >= entry->siteCRCStrLen)
        {
            //Copy object value
            memcpy(value->octetString, entry->siteCRCStr, entry->siteCRCStrLen);
            //Return object length
            *valueLen = entry->siteCRCStrLen;
        }
        else
        {
            //Report an error
            error = ERROR_BUFFER_OVERFLOW;
        }
    }
    //siteAmbientTemp object?
    else if(!strcmp(object->name, "siteAmbientTemp"))
    {
        //Get object value
        value->integer = privateMibBase.siteGroup.siteAmbientTemp;
    }
    //siteInfoTrapCounter object?
    else if(!strcmp(object->name, "siteTrapCounter"))
    {
        //Get object value
        value->integer = privateMibBase.siteGroup.siteTrapCounter;
    }
    //siteSystemReset object?
    else if(!strcmp(object->name, "siteSystemReset"))
    {
        //Get object value
        value->integer = privateMibBase.siteGroup.siteSystemReset;
    }
    //siteMACInfo object?
    else if(!strcmp(object->name, "siteMACInfo"))
    {
        //Make sure the buffer is large enough to hold the entire object
        if(*valueLen >= entry->siteMACInfoLen)
        {
            //Copy object value
            memcpy(value->octetString, entry->siteMACInfo, entry->siteMACInfoLen);
            //Return object length
            *valueLen = entry->siteMACInfoLen;
        }
        else
        {
            //Report an error
            error = ERROR_BUFFER_OVERFLOW;
        }
    }
    //siteSerialNumber object?
    else if(!strcmp(object->name, "siteSerialNumber"))
    {
        //Make sure the buffer is large enough to hold the entire object
        if(*valueLen >= entry->siteSerialNumberLen)
        {
            //Copy object value
            memcpy(value->octetString, entry->siteSerialNumber, entry->siteSerialNumberLen);
            //Return object length
            *valueLen = entry->siteSerialNumberLen;
        }
        else
        {
            //Report an error
            error = ERROR_BUFFER_OVERFLOW;
        }
    }
    //siteSNMPServer object?
    else if(!strcmp(object->name, "siteSNMPServer"))
    {
        //Make sure the buffer is large enough to hold the entire object
        if(*valueLen >= 16)
        {
            //Copy object value
            memcpy(value->octetString, &privateMibBase.cfgNetworkGroup.siteNetworkInfo.ucSIP[0], 16);            
            //Return object length
            *valueLen = 16;
        }
        else
        {
            //Report an error
            error = ERROR_BUFFER_OVERFLOW;
        }
    }
    //siteFTPServer object?
    else if(!strcmp(object->name, "siteFTPServer"))
    {
        //Make sure the buffer is large enough to hold the entire object
        if(*valueLen >= privateMibBase.cfgNetworkGroup.cFTPServerLen)
        {
            //Copy object value
            memcpy(value->octetString, &privateMibBase.cfgNetworkGroup.cFTPServer[0], privateMibBase.cfgNetworkGroup.cFTPServerLen);
            //Return object length
            *valueLen = privateMibBase.cfgNetworkGroup.cFTPServerLen;
        }
        else
        {
            //Report an error
            error = ERROR_BUFFER_OVERFLOW;
        }
    }
    //siteFirmwareUpdate object?
    else if(!strcmp(object->name, "siteFirmwareUpdate"))
    {
        //Get object value
        value->integer = privateMibBase.siteGroup.siteFirmwareUpdate;
    }
    //siteFWType object?
    else if(!strcmp(object->name, "siteFWType"))
    {
        privateMibBase.siteGroup.siteFWType = configHandle.devType;
        //Get object value
        value->integer = privateMibBase.siteGroup.siteFWType;
    }
    //siteDeviceModel object?
    else if(!strcmp(object->name, "siteDeviceModel"))
    {
        //Make sure the buffer is large enough to hold the entire object
        if(*valueLen >= privateMibBase.siteGroup.siteDevModelLen)
        {
            //Copy object value
            memcpy(value->octetString, privateMibBase.siteGroup.siteDevModel, privateMibBase.siteGroup.siteDevModelLen);
            //Return object length
            *valueLen = privateMibBase.siteGroup.siteDevModelLen;
        }
        else
        {
            //Report an error
            error = ERROR_BUFFER_OVERFLOW;
        }
    }
    //siteTrapEnable object?
    else if(!strcmp(object->name, "siteTrapEnable"))
    {
        //Get object value
        value->integer = privateMibBase.cfgNetworkGroup.u8TrapMode;
    }
    //Unknown object?
    else
    {
        //The specified object does not exist
        error = ERROR_OBJECT_NOT_FOUND;
    }

    //Return status code
    return error;
}
//========================================== SiteInfo Function ==========================================//
//========================================== RectInfo Function ==========================================//
/**
 * @brief Get rectInfoEntry object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t privateMibGetRectInfoEntry(const MibObject *object, const uint8_t *oid,
                                  size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibRectInfoEntry *entry;

    //Point to the instance identifier
    n = object->oidLen;

    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index < 1 || index > 24)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the interface table entry
    entry = &privateMibBase.rectGroup.rectTable[index - 1];
    //rectIndex object?
    if(!strcmp(object->name, "rectIndex"))
    {
        //Get object value
        value->integer = entry->rectIndex;
    }
    //rectStatus object?
    else if(!strcmp(object->name, "rectStatus"))
    {
        //Get object value
        value->integer = entry->rectStatus;
    }
    //rectOutputCurrent object?
    else if(!strcmp(object->name, "rectOutputCurrent"))
    {
        //Get object value
        value->integer = entry->rectOutputCurrent;
    }
    //rectOutputVoltage object?
    else if(!strcmp(object->name, "rectOutputVoltage"))
    {
        //Get object value
        value->integer = entry->rectOutputVoltage;
    }
    //rectTemp object?
    else if(!strcmp(object->name, "rectTemp"))
    {
        //Get object value
        value->integer = entry->rectTemp;
    }
    //rectType object?
    else if(!strcmp(object->name, "rectType"))
    {
      //Make sure the buffer is large enough to hold the entire object
      if(*valueLen >= entry->rectTypeLen)
      {
          //Copy object value
          memcpy(value->octetString, entry->rectType, entry->rectTypeLen);
          //Return object length
          *valueLen = entry->rectTypeLen;
      }
      else
      {
          //Report an error
          error = ERROR_BUFFER_OVERFLOW;
      }
    }
    //rectAlarmStatus object?
    else if(!strcmp(object->name, "rectAlarmStatus"))
    {
        //Get object value
        value->integer = entry->rectAlarmStatus;
    }
    //rectSerialNo object?
    else if(!strcmp(object->name, "rectSerialNo"))
    {
        //Make sure the buffer is large enough to hold the entire object
        if(*valueLen >= entry->rectSerialNoLen)
        {
            //Copy object value
            memcpy(value->octetString, entry->rectSerialNo, entry->rectSerialNoLen);
            //Return object length
            *valueLen = entry->rectSerialNoLen;
        }
        else
        {
            //Report an error
            error = ERROR_BUFFER_OVERFLOW;
        }
    }
    //rectRevisionLevel object?
    else if(!strcmp(object->name, "rectRevisionLevel"))
    {
        //Get object value
        value->integer = entry->rectRevisionLevel;
    }
    //Unknown object?
    else
    {
        //The specified object does not exist
        error = ERROR_OBJECT_NOT_FOUND;
    }

    //Return status code
    return error;
}


/**
 * @brief Get next rectInfoEntry object
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] nextOid OID of the next object in the MIB
 * @param[out] nextOidLen Length of the next object identifier, in bytes
 * @return Error code
 **/
error_t privateMibGetNextRectInfoEntry(const MibObject *object, const uint8_t *oid,
                                      size_t oidLen, uint8_t *nextOid, size_t *nextOidLen)
{
    error_t error;
    size_t n;
    uint_t index;

    //Make sure the buffer is large enough to hold the OID prefix
    if(*nextOidLen < object->oidLen)
        return ERROR_BUFFER_OVERFLOW;

    //Copy OID prefix
    memcpy(nextOid, object->oid, object->oidLen);

    //Loop through network interfaces
    for(index = 1; index <= privateMibBase.rectGroup.rectInstalledRect; index++)
    {
        //Append the instance identifier to the OID prefix
        n = object->oidLen;

        //The ifIndex is used as instance identifier
        error = mibEncodeIndex(nextOid, *nextOidLen, &n, index);
        //Any error to report?
        if(error) return error;

        //Check whether the resulting object identifier lexicographically
        //follows the specified OID
        if(oidComp(nextOid, n, oid, oidLen) > 0)
        {
            //Save the length of the resulting object identifier
            *nextOidLen = n;
            //Next object found
            return NO_ERROR;
        }
    }

    //The specified OID does not lexicographically precede the name
    //of some object
    return ERROR_OBJECT_NOT_FOUND;
}
//========================================== RectInfo Function ==========================================//
//========================================== AcInfo Function ==========================================//
/**
 * @brief Get acInfoEntry object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t privateMibGetAcPhaseEntry(const MibObject *object, const uint8_t *oid,
                                  size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibAcPhaseEntry *entry;

    //Point to the instance identifier
    n = object->oidLen;

    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index < 1 || index > 3)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the interface table entry
    entry = &privateMibBase.acPhaseGroup.acPhaseTable[index - 1];

    //ifIndex object?
    if(!strcmp(object->name, "acPhaseIndex"))
    {
        //Get object value
        value->integer = entry->acPhaseIndex;
    }
    //acPhaseVolt object?
    else if(!strcmp(object->name, "acPhaseVolt"))
    {
        //Get object value
        value->integer = entry->acPhaseVolt;
    }
    //Unknown object?
    else
    {
        //The specified object does not exist
        error = ERROR_OBJECT_NOT_FOUND;
    }

    //Return status code
    return error;
}


/**
 * @brief Get next acInfoEntry object
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] nextOid OID of the next object in the MIB
 * @param[out] nextOidLen Length of the next object identifier, in bytes
 * @return Error code
 **/
error_t privateMibGetNextAcPhaseEntry(const MibObject *object, const uint8_t *oid,
                                      size_t oidLen, uint8_t *nextOid, size_t *nextOidLen)
{
    error_t error;
    size_t n;
    uint_t index;

    //Make sure the buffer is large enough to hold the OID prefix
    if(*nextOidLen < object->oidLen)
        return ERROR_BUFFER_OVERFLOW;

    //Copy OID prefix
    memcpy(nextOid, object->oid, object->oidLen);

    //Loop through network interfaces
    for(index = 1; index <= privateMibBase.acPhaseGroup.acPhaseNumber; index++)
    {
        //Append the instance identifier to the OID prefix
        n = object->oidLen;

        //The ifIndex is used as instance identifier
        error = mibEncodeIndex(nextOid, *nextOidLen, &n, index);
        //Any error to report?
        if(error) return error;

        //Check whether the resulting object identifier lexicographically
        //follows the specified OID
        if(oidComp(nextOid, n, oid, oidLen) > 0)
        {
            //Save the length of the resulting object identifier
            *nextOidLen = n;
            //Next object found
            return NO_ERROR;
        }
    }

    //The specified OID does not lexicographically precede the name
    //of some object
    return ERROR_OBJECT_NOT_FOUND;
}
//========================================== AcInfo Function ==========================================//
//========================================== BatteryInfo Function ==========================================//
/**
 * @brief Get BatteryInfo object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t privateMibGetBatteryGroup(const MibObject *object, const uint8_t *oid,
                                  size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error = NO_ERROR;
    size_t n;
    PrivateMibBatteryGroup *entry;
    entry = &privateMibBase.batteryGroup;

    //battVolt object?
    if(!strcmp(object->name, "battVolt"))
    {
        //Get object value
        value->integer = entry->battVolt;
    }
    //battCurr object?
    else if(!strcmp(object->name, "battCurr"))
    {
        //Get object value
        value->integer = entry->battCurr;
    }
    //battTemp object?
    else if(!strcmp(object->name, "battTemp"))
    {
        //Get object value
        value->integer = entry->battTemp;
    }
    //battBanksNumofBanks object?
    else if(!strcmp(object->name, "battBanksNumofBanks"))
    {
        //Get object value
        value->integer = entry->battBanksNumofBanks;
    }
    //battCapLeft1 object?
    else if(!strcmp(object->name, "battCapLeft1"))
    {
        //Get object value
        value->integer = entry->battCapLeft1;
    }
    //battCapLeft2 object?
    else if(!strcmp(object->name, "battCapLeft2"))
    {
        //Get object value
        value->integer = entry->battCapLeft2;
    }
    //battBrk1Status object?
    else if(!strcmp(object->name, "battBrk1Status"))
    {
        //Get object value
        value->integer = entry->battBrkStatus[0];
    }
    //battBrk2Status object?
    else if(!strcmp(object->name, "battBrk2Status"))
    {
        //Get object value
        value->integer = entry->battBrkStatus[1];
    }
    //battBrk3Status object?
    else if(!strcmp(object->name, "battBrk3Status"))
    {
        //Get object value
        value->integer = entry->battBrkStatus[2];
    }
    //battBrk4Status object?
    else if(!strcmp(object->name, "battBrk4Status"))
    {
        //Get object value
        value->integer = entry->battBrkStatus[3];
    }
    //battCapLeft3 object?
    else if(!strcmp(object->name, "battCapLeft3"))
    {
        //Get object value
        value->integer = entry->battCapLeft3;
    }
    //battCapLeft4 object?
    else if(!strcmp(object->name, "battCapLeft4"))
    {
        //Get object value
        value->integer = entry->battCapLeft4;
    }
    //Unknown object?
    else
    {
        //The specified object does not exist
        error = ERROR_OBJECT_NOT_FOUND;
    }

    //Return status code
    return error;
}
//========================================== BatteryInfo Function ==========================================//
//========================================== LoadInfo Function ==========================================//
/**
 * @brief Get LoadInfo object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t privateMibGetLoadGroup(const MibObject *object, const uint8_t *oid,
                                      size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error = NO_ERROR;
    size_t n;
    PrivateMibLoadGroup *entry;

    entry = &privateMibBase.loadGroup;
    //loadCurrent object?
    if(!strcmp(object->name, "loadCurrent"))
    {
        //Get object value
        value->integer = entry->loadCurrent;
    }
    //load1Status object?
    else if(!strcmp(object->name, "load1Status"))
    {
        //Get object value
        value->integer = entry->loadStatus[0];
    }
    //load2Status object?
    else if(!strcmp(object->name, "load2Status"))
    {
        //Get object value
        value->integer = entry->loadStatus[1];
    }
    //load3Status object?
    else if(!strcmp(object->name, "load3Status"))
    {
        //Get object value
        value->integer = entry->loadStatus[2];
    }
    //load4Status object?
    else if(!strcmp(object->name, "load4Status"))
    {
        //Get object value
        value->integer = entry->loadStatus[3];
    }
    //load5Status object?
    else if(!strcmp(object->name, "load5Status"))
    {
        //Get object value
        value->integer = entry->loadStatus[4];
    }
    //load6Status object?
    else if(!strcmp(object->name, "load6Status"))
    {
        //Get object value
        value->integer = entry->loadStatus[5];
    }
    //load7Status object?
    else if(!strcmp(object->name, "load7Status"))
    {
        //Get object value
        value->integer = entry->loadStatus[6];
    }
    //load8Status object?
    else if(!strcmp(object->name, "load8Status"))
    {
        //Get object value
        value->integer = entry->loadStatus[7];
    }
    //load9Status object?
    else if(!strcmp(object->name, "load9Status"))
    {
        //Get object value
        value->integer = entry->loadStatus[8];
    }
    //load10Status object?
    else if(!strcmp(object->name, "load10Status"))
    {
        //Get object value
        value->integer = entry->loadStatus[9];
    }
    //Unknown object?
    else
    {
        //The specified object does not exist
        error = ERROR_OBJECT_NOT_FOUND;
    }

    //Return status code
    return error;
}
//========================================== LoadInfo Function ==========================================//

//==========================================Cnt=========================================================//
error_t privateMibGetCntGroup(const MibObject *object, const uint8_t *oid,
                                      size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error = NO_ERROR;
    size_t n;
    PrivateMibCntGroup *entry;

    entry = &privateMibBase.cntGroup;
    //ZTEcnt object?
    if(!strcmp(object->name, "ZTEcnt"))
    {
        //Get object value
        value->integer = entry->ZTEcnt;
    }
    // EMERcnt object?
    else if(!strcmp(object->name, "EMERcnt"))
    {
        //Get object value
        value->integer = entry->EMERcnt;
    }
    // EMERV21cnt object?
    else if(!strcmp(object->name, "EMERV21cnt"))
    {
        //Get object value
        value->integer = entry->EMERV21cnt;
    }
    // HWcnt object?
    else if(!strcmp(object->name, "HWcnt"))
    {
        //Get object value
        value->integer = entry->HWcnt;
    }
    // ModbusRTUcnt object?
    else if(!strcmp(object->name, "ModbusRTUcnt"))
    {
        //Get object value
        value->integer = entry->ModbusRTUcnt;
    }
    // EMERState object?
    else if(!strcmp(object->name, "EMERState"))
    {
        //Get object value
        value->integer = entry->EMERState;
    }
    // EMERV21State object?
    else if(!strcmp(object->name, "EMERV21State"))
    {
        //Get object value
        value->integer = entry->EMERV21State;
    }
    // ZTEState object?
    else if(!strcmp(object->name, "ZTEState"))
    {
        //Get object value
        value->integer = entry->ZTEState;
    }
    // HWState object?
    else if(!strcmp(object->name, "HWState"))
    {
        //Get object value
        value->integer = entry->HWState;
    }
    // HWRs485Cmd object?
    else if(!strcmp(object->name, "HWRs485Cmd"))
    {
        //Get object value
        value->integer = entry->HWRs485Cmd;
    }
    else if(!strcmp(object->name, "RS232ReInitEnable"))
    {
        //Get object value
        value->integer = entry->RS232ReInitEnable;
    }
    //Unknown object?
    else
    {
        //The specified object does not exist
        error = ERROR_OBJECT_NOT_FOUND;
    }

  //Return status code
    return error;
}
error_t privateMibSetCntGroup(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
   error_t error;
    size_t n;
    uint_t index;
    PrivateMibCntGroup *entry;
    
    //	Point to the cntGroup entry
    entry = &privateMibBase.cntGroup;
    
    //RS232ReInitEnable object?
    if(!strcmp(object->name, "RS232ReInitEnable"))
    {
      if((value->integer == 0)||(value->integer == 1))
      {
        //Get object value
        entry->RS232ReInitEnable= value->integer;
        //flag_ReInitRs232=1;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    
    
    return NO_ERROR; 
}



//==========================================Cnt=========================================================//
//dpc fan info============================================================================================
error_t privateMibGet_fan_dpc_Group(const MibObject *object, const uint8_t *oid,
                                      size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error = NO_ERROR;
    size_t n;
    PrivateMibFANDPCInfo *entry;

    entry = &privateMibBase.fan_dpc_info;
    //dpc_fan_env_temp object?
    if(!strcmp(object->name, "dpc_fan_env_temp"))
    {
        //Get object value
        value->integer = entry->mib.env_temp;
    }
    // dpc_fan_airflow_temp object?
    else if(!strcmp(object->name, "dpc_fan_airflow_temp"))
    {
        //Get object value
        value->integer = entry->mib.airflow_temp;
    }
    // dpc_fan_voltage object?
    else if(!strcmp(object->name, "dpc_fan_voltage"))
    {
        //Get object value
        value->integer = entry->mib.voltage;
    }
    // dpc_fan_working_current object?
    else if(!strcmp(object->name, "dpc_fan_working_current"))
    {
        //Get object value
        value->integer = entry->mib.working_current;
    }
    // dpc_fan_state_device object?
    else if(!strcmp(object->name, "dpc_fan_state_device"))
    {
        //Get object value
        value->integer = entry->mib.state_device;
    }
    // dpc_fan_state_fan object?
    else if(!strcmp(object->name, "dpc_fan_state_fan"))
    {
        //Get object value
        value->integer = entry->mib.state_fan;
    }
    // dpc_fan_alarm_fail_refrigeration object?
    else if(!strcmp(object->name, "dpc_fan_alarm_fail_refrigeration"))
    {
        //Get object value
        value->integer = entry->mib.alarm_fail_refrigeration;
    }
    // dpc_fan_alarm_high_temp object?
    else if(!strcmp(object->name, "dpc_fan_alarm_high_temp"))
    {
        //Get object value
        value->integer = entry->mib.alarm_high_temp;
    }
    // dpc_fan_alarm_low_temp object?
    else if(!strcmp(object->name, "dpc_fan_alarm_low_temp"))
    {
        //Get object value
        value->integer = entry->mib.alarm_low_temp;
    }
    // dpc_fan_alarm_fail_sensor_temp object?
    else if(!strcmp(object->name, "dpc_fan_alarm_fail_sensor_temp"))
    {
        //Get object value
        value->integer = entry->mib.alarm_fail_sensor_temp;
    }
    //dpc_fan_alarm_high_voltage object?
    else if(!strcmp(object->name, "dpc_fan_alarm_high_voltage"))
    {
        //Get object value
        value->integer = entry->mib.alarm_high_voltage;
    }
    //dpc_fan_alarm_low_voltage object?
    else if(!strcmp(object->name, "dpc_fan_alarm_low_voltage"))
    {
        //Get object value
        value->integer = entry->mib.alarm_low_voltage;
    }
    
    //dpc_fan_para_dc_starting_point object?
    else if(!strcmp(object->name, "dpc_fan_para_dc_starting_point"))
    {
        //Get object value
        value->integer = entry->mib.para_dc_starting_point;
    }
    //dpc_fan_para_dc_sensivive_point object?
    else if(!strcmp(object->name, "dpc_fan_para_dc_sensivive_point"))
    {
        //Get object value
        value->integer = entry->mib.para_dc_sensivive_point;
    }
    //dpc_fan_para_alarm_high_temp object?
    else if(!strcmp(object->name, "dpc_fan_para_alarm_high_temp"))
    {
        //Get object value
        value->integer = entry->mib.para_alarm_high_temp;
    }
    //dpc_fan_para_alarm_low_temp object?
    else if(!strcmp(object->name, "dpc_fan_para_alarm_low_temp"))
    {
        //Get object value
        value->integer = entry->mib.para_alarm_low_temp;
    }
    //dpc_fan_para_dc_heating_start_point object?
    else if(!strcmp(object->name, "dpc_fan_para_dc_heating_start_point"))
    {
        //Get object value
        value->integer = entry->mib.para_dc_heating_start_point;
    }
    //dpc_fan_para_dc_heating_sensitive_point object?
    else if(!strcmp(object->name, "dpc_fan_para_dc_heating_sensitive_point"))
    {
        //Get object value
        value->integer = entry->mib.para_dc_heating_sensivive;
    }
    //Unknown object?    
    else
    {
        //The specified object does not exist
        error = ERROR_OBJECT_NOT_FOUND;
    }

  //Return status code
    return error;
}
//-------------------------------------------------------------------------
error_t privateMibSet_fan_dpc_Group(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
   error_t error;
    size_t n;
    uint_t index;
    PrivateMibFANDPCInfo *entry;
    
    //	Point to the cntGroup entry
    entry = &privateMibBase.fan_dpc_info;
    
    //RS232ReInitEnable object?
    if(!strcmp(object->name, "dpc_fan_para_dc_starting_point"))
    {
      if((value->integer >= 2000) && (value->integer <= 4000)){
        
        entry->mib.temp_set = value->integer;
        entry->mib.para_dc_starting_point = entry->mib.temp_set;
        fan_info_dpc_t.para_dc_starting_point = entry->mib.temp_set;
        sModbusManager.SettingCommand = SET_FAN_DPC_STARTING_POINT;
      }
      else{
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    else if(!strcmp(object->name, "dpc_fan_para_dc_sensivive_point"))
    {
      if((value->integer >= 0) && (value->integer <= 2500)){
        entry->mib.temp_set = value->integer;
        entry->mib.para_dc_sensivive_point = entry->mib.temp_set;
        fan_info_dpc_t.para_dc_sensivive_point = entry->mib.temp_set;
        sModbusManager.SettingCommand = SET_FAN_DPC_SENSITIVE_POINT; 
      }
      else{
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }else if(!strcmp(object->name, "dpc_fan_para_alarm_high_temp"))
    {
      if((value->integer >= 3000) && (value->integer <= 6000)){
        entry->mib.temp_set = value->integer;
        entry->mib.para_alarm_high_temp = entry->mib.temp_set;
        fan_info_dpc_t.para_alarm_high_temp = entry->mib.temp_set;
        sModbusManager.SettingCommand = SET_FAN_DPC_H_TEMP_W_POINT;
      }
      else{
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }else if(!strcmp(object->name, "dpc_fan_para_alarm_low_temp"))
    {
      if((value->integer >= -4500) && (value->integer <= 1000)){
        entry->mib.i_tem_set = value->integer;
        entry->mib.para_alarm_low_temp = entry->mib.i_tem_set;
        fan_info_dpc_t.para_alarm_low_temp = entry->mib.i_tem_set;
        sModbusManager.SettingCommand = SET_FAN_DPC_L_TEMP_W_POINT; 
      }
      else{
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }      
    }
     if(!strcmp(object->name, "dpc_fan_para_dc_heating_start_point"))
    {
      if((value->integer >= -1000) && (value->integer <= 1500)){
        entry->mib.i_tem_set = value->integer;
        entry->mib.para_dc_heating_start_point = entry->mib.i_tem_set;
        fan_info_dpc_t.para_dc_heating_start_point = entry->mib.i_tem_set;
        sModbusManager.SettingCommand = SET_FAN_DPC_HEATING_START_POINT;
      }
      else{
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
     if(!strcmp(object->name, "dpc_fan_para_dc_heating_sensitive_point"))
    {
      if((value->integer >= 0) && (value->integer <= 3000)){
        entry->mib.i_tem_set = value->integer;
        entry->mib.para_dc_heating_sensivive = entry->mib.i_tem_set;
        fan_info_dpc_t.para_dc_heating_sensivive = entry->mib.i_tem_set;
        sModbusManager.SettingCommand = SET_FAN_DPC_HEATING_SENSITIVE_POINT;
      }
      else{
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    else
    {
      
    }
    
    
    return NO_ERROR; 
}
//dpc fan info============================================================================================

#if (USERDEF_MONITOR_BM == ENABLED)
//========================================== BMInfo Function ==========================================//
/**
 * @brief Set BMGroup object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[in] value Object value
 * @param[in] valueLen Length of the object value, in bytes
 * @return Error code
 **/
error_t privateMibSetBMInfoEntry(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibBMInfoEntry *entry;

    //Point to the instance identifier
    n = object->oidLen;

    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index < 1 || index > 16)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the bmTable entry
    entry = &privateMibBase.bmGroup.bmTable[0];//[index - 1];
    
    //bmVoltDiff object?
    if(!strcmp(object->name, "bmVoltDiff"))
    {
      if((value->integer >= 1) && (value->integer <= 1000))
      {
        //Get object value
        entry->bmVoltDiff= value->integer;
        sBMInfo[0].u32VoltDiff = entry->bmVoltDiff;
        sModbusManager.SettingCommand = SET_BM_VOLTDIFF;
        sModbusManager.u8BMCurrentIndex = 0;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //bmMaxTemp object?
    else if(!strcmp(object->name, "bmMaxTemp"))
    {
      if((value->integer >= 1000) && (value->integer <= 8000))
      {
        //Get object value
        entry->bmMaxTemp= value->integer;
        sBMInfo[0].u32MaxTemp = entry->bmMaxTemp;
        sModbusManager.SettingCommand = SET_BM_MAXTEMP;
        sModbusManager.u8BMCurrentIndex = 0;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //bmVoltThres object?
    else if(!strcmp(object->name, "bmVoltThres"))
    {
      if((value->integer >= 5000) && (value->integer <= 5900))
      {
        //Get object value
        entry->bmVoltThres= value->integer;
        sBMInfo[0].u32VoltThres = entry->bmVoltThres;
        sModbusManager.SettingCommand = SET_BM_VOLTTHRES;
        sModbusManager.u8BMCurrentIndex = 0;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //bmCurrThres object?
    else if(!strcmp(object->name, "bmCurrThres"))
    {
      if((value->integer >= 0) && (value->integer <= 1000))
      {
        //Get object value
        entry->bmCurrThres= value->integer;
        sBMInfo[0].u32CurrThres = entry->bmCurrThres;
        sModbusManager.SettingCommand = SET_BM_CURRTHRES;
        sModbusManager.u8BMCurrentIndex = 0;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //bmTimeThres object?
    else if(!strcmp(object->name, "bmTimeThres"))
    {
      if((value->integer >= 1) && (value->integer <= 300))
      {
        //Get object value
        entry->bmTimeThres= value->integer;
        sBMInfo[0].u32TimeThres = entry->bmTimeThres;
        sModbusManager.SettingCommand = SET_BM_TIMETHRES;
        sModbusManager.u8BMCurrentIndex = 0;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //bmSOCThres object?
    else if(!strcmp(object->name, "bmSOCThres"))
    {
      if((value->integer >= 0) && (value->integer <= 100))
      {
        //Get object value
        entry->bmSOCThres= value->integer;
        sBMInfo[0].u32SOCThres = entry->bmSOCThres;
        sModbusManager.SettingCommand = SET_BM_SOCTHRES;
        sModbusManager.u8BMCurrentIndex = 0;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //bmMinTemp object?
    else if(!strcmp(object->name, "bmMinTemp"))
    {
      if((value->integer >= -3000) && (value->integer <= 3000))
      {
        //Get object value
        entry->bmMinTemp= value->integer;
        sBMInfo[0].u32MinTemp = entry->bmMinTemp;
        sModbusManager.SettingCommand = SET_BM_MINTEMP;
        sModbusManager.u8BMCurrentIndex = 0;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //bmLowCapTime object?
    else if(!strcmp(object->name, "bmLowCapTime"))
    {
      if((value->integer >= 0) && (value->integer <= 1000))
      {
        //Get object value
        entry->bmLowCapTime= value->integer;
        sBMInfo[0].u32LowCapTime = entry->bmLowCapTime;
        sModbusManager.SettingCommand = SET_BM_LOWCAPTIME;
        sModbusManager.u8BMCurrentIndex = 0;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //bmHalfVoltAlarm object?
    else if(!strcmp(object->name, "bmHalfVoltAlarm"))
    {
      if((value->integer >= 1000) && (value->integer <= 6000))
      {
        //Get object value
        entry->bmHalfVoltAlarm= value->integer;
        sBMInfo[0].u32HalfVoltAlarm = entry->bmHalfVoltAlarm;
        sModbusManager.SettingCommand = SET_BM_HALFVOLTALARM;
        sModbusManager.u8BMCurrentIndex = 0;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //bmRechargeVolt object?
    else if(!strcmp(object->name, "bmRechargeVolt"))
    {
      if((value->integer >= 5200) && (value->integer <= 5800))
      {
        //Get object value
        entry->bmRechargeVolt= value->integer;
        sBMInfo[0].u32RechargeVolt = entry->bmRechargeVolt;
        sModbusManager.SettingCommand = SET_BM_RECHARGEVOLT;
        sModbusManager.u8BMCurrentIndex = 0;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //bmCurrBalanceThres object?
    else if(!strcmp(object->name, "bmCurrBalanceThres"))
    {
      if((value->integer >= 0) && (value->integer <= 200))
      {
        //Get object value
        entry->bmCurrBalanceThres= value->integer;
        sBMInfo[0].u32CurrBalanceThres = entry->bmCurrBalanceThres;
        sModbusManager.SettingCommand = SET_BM_CURRBALANCETHRES;
        sModbusManager.u8BMCurrentIndex = 0;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //Unknown object?
    else
    {
        //The specified object does not exist
        error = ERROR_OBJECT_NOT_FOUND;
    }    
   
    //Successful processing
    return NO_ERROR;
}

/**
 * @brief Get BMInfoEntry object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t privateMibGetBMInfoEntry(const MibObject *object, const uint8_t *oid,
                                  size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibBMInfoEntry *entry;

    //Point to the instance identifier
    n = object->oidLen;

    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index < 1 || index > 16)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the interface table entry
    entry = &privateMibBase.bmGroup.bmTable[index - 1];
    //bmID object?
    if(!strcmp(object->name, "bmID"))
    {
        //Get object value
        value->integer = entry->bmID;
    }
    //bmStatus object?
    else if(!strcmp(object->name, "bmStatus"))
    {
        //Get object value
        value->integer = entry->bmStatus;
    }
    //bmBattVolt object?
    else if(!strcmp(object->name, "bmBattVolt"))
    {
        //Get object value
        value->integer = entry->bmBattVolt;
    }
    //bmPackVolt object?
    else if(!strcmp(object->name, "bmPackVolt"))
    {
        //Get object value
        value->integer = entry->bmPackVolt;
    }
    //bmPackCurr object?
    else if(!strcmp(object->name, "bmPackCurr"))
    {
        //Get object value
        value->integer = entry->bmPackCurr;
    }
    //bmCellVolt1 object?
    else if(!strcmp(object->name, "bmCellVolt1"))
    {
        //Get object value
        value->integer = entry->bmCellVolt1;
    }
    //bmCellVolt2 object?
    else if(!strcmp(object->name, "bmCellVolt2"))
    {
        //Get object value
        value->integer = entry->bmCellVolt2;
    }
    //bmCellVolt3 object?
    else if(!strcmp(object->name, "bmCellVolt3"))
    {
        //Get object value
        value->integer = entry->bmCellVolt3;
    }
    //bmCellVolt4 object?
    else if(!strcmp(object->name, "bmCellVolt4"))
    {
        //Get object value
        value->integer = entry->bmCellVolt4;
    }
    //bmCellTemp1 object?
    else if(!strcmp(object->name, "bmCellTemp1"))
    {
        //Get object value
        value->integer = entry->bmCellTemp1;
    }
    //bmCellTemp2 object?
    else if(!strcmp(object->name, "bmCellTemp2"))
    {
        //Get object value
        value->integer = entry->bmCellTemp2;
    }
    //bmCellTemp3 object?
    else if(!strcmp(object->name, "bmCellTemp3"))
    {
        //Get object value
        value->integer = entry->bmCellTemp3;
    }
    //bmCellTemp4 object?
    else if(!strcmp(object->name, "bmCellTemp4"))
    {
        //Get object value
        value->integer = entry->bmCellTemp4;
    }
    //bmCellTemp5 object?
    else if(!strcmp(object->name, "bmCellTemp5"))
    {
        //Get object value
        value->integer = entry->bmCellTemp5;
    }
    //bmCellTemp6 object?
    else if(!strcmp(object->name, "bmCellTemp6"))
    {
        //Get object value
        value->integer = entry->bmCellTemp6;
    }
    //bmAlarmStatus object?
    else if(!strcmp(object->name, "bmAlarmStatus"))
    {
        //Get object value
        value->counter32 = entry->bmAlarmStatus.all;
    }
    //bmBalanceStatus object?
    else if(!strcmp(object->name, "bmBalanceStatus"))
    {
        //Get object value
        value->integer = entry->bmBalanceStatus;
    }
    //bmBattStatus object?
    else if(!strcmp(object->name, "bmBattStatus"))
    {
        //Get object value
        value->integer = entry->bmBattStatus;
    }
    //bmSOC object?
    else if(!strcmp(object->name, "bmSOC"))
    {
        //Get object value
        value->integer = entry->bmSOC;
    }
    //bmSOH object?
    else if(!strcmp(object->name, "bmSOH"))
    {
        //Get object value
        value->integer = entry->bmSOH;
    }
    //bmDischargeTime object?
    else if(!strcmp(object->name, "bmDischargeTime"))
    {
        //Get object value
        value->integer = entry->bmDischargeTime;
    }
    //bmTotalRunTime object?
    else if(!strcmp(object->name, "bmTotalRunTime"))
    {
        //Get object value
        value->integer = entry->bmTotalRunTime;
    }
    //bmVoltDiff object?
    else if(!strcmp(object->name, "bmVoltDiff"))
    {
        //Get object value
        value->integer = entry->bmVoltDiff;
    }
    //bmMaxTemp object?
    else if(!strcmp(object->name, "bmMaxTemp"))
    {
        //Get object value
        value->integer = entry->bmMaxTemp;
    }
    //bmVoltThres object?
    else if(!strcmp(object->name, "bmVoltThres"))
    {
        //Get object value
        value->integer = entry->bmVoltThres;
    }
    //bmCurrThres object?
    else if(!strcmp(object->name, "bmCurrThres"))
    {
        //Get object value
        value->integer = entry->bmCurrThres;
    }
    //bmTimeThres object?
    else if(!strcmp(object->name, "bmTimeThres"))
    {
        //Get object value
        value->integer = entry->bmTimeThres;
    }
    //bmSOCThres object?
    else if(!strcmp(object->name, "bmSOCThres"))
    {
        //Get object value
        value->integer = entry->bmSOCThres;
    }
    //bmMinTemp object?
    else if(!strcmp(object->name, "bmMinTemp"))
    {
        //Get object value
        value->integer = entry->bmMinTemp;
    }
    //bmLowCapTime object?
    else if(!strcmp(object->name, "bmLowCapTime"))
    {
        //Get object value
        value->integer = entry->bmLowCapTime;
    }
    //bmTotalDisAH object?
    else if(!strcmp(object->name, "bmTotalDisAH"))
    {
        //Get object value
        value->integer = entry->bmTotalDisAH;
    }
    //bmHalfVoltAlarm object?
    else if(!strcmp(object->name, "bmHalfVoltAlarm"))
    {
        //Get object value
        value->integer = entry->bmHalfVoltAlarm;
    }
    //bmRechargeVolt object?
    else if(!strcmp(object->name, "bmRechargeVolt"))
    {
        //Get object value
        value->integer = entry->bmRechargeVolt;
    }
    //bmCurrBalanceThres object?
    else if(!strcmp(object->name, "bmCurrBalanceThres"))
    {
        //Get object value
        value->integer = entry->bmCurrBalanceThres;
    }

    //Return status code
    return error;
}


/**
 * @brief Get next BMInfoEntry object
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] nextOid OID of the next object in the MIB
 * @param[out] nextOidLen Length of the next object identifier, in bytes
 * @return Error code
 **/
error_t privateMibGetNextBMInfoEntry(const MibObject *object, const uint8_t *oid,
                                      size_t oidLen, uint8_t *nextOid, size_t *nextOidLen)
{
    error_t error;
    size_t n;
    uint_t index;

    //Make sure the buffer is large enough to hold the OID prefix
    if(*nextOidLen < object->oidLen)
        return ERROR_BUFFER_OVERFLOW;

    //Copy OID prefix
    memcpy(nextOid, object->oid, object->oidLen);

    //Loop through network interfaces
    for(index = 1; index <= privateMibBase.bmGroup.bmInstalledBM; index++)
    {
        //Append the instance identifier to the OID prefix
        n = object->oidLen;

        //The ifIndex is used as instance identifier
        error = mibEncodeIndex(nextOid, *nextOidLen, &n, index);
        //Any error to report?
        if(error) return error;

        //Check whether the resulting object identifier lexicographically
        //follows the specified OID
        if(oidComp(nextOid, n, oid, oidLen) > 0)
        {
            //Save the length of the resulting object identifier
            *nextOidLen = n;
            //Next object found
            return NO_ERROR;
        }
    }

    //The specified OID does not lexicographically precede the name
    //of some object
    return ERROR_OBJECT_NOT_FOUND;
}
//========================================== BMInfo Function ==========================================//
#endif
//========================================== LIBattInfo Function ==========================================//
error_t privateMibSetLIBattGroup(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibLIBattInfoGroup *entry;
    
    //	Point to the liBattGroup entry
    entry = &privateMibBase.liBattGroup;
    
    if(!strcmp(object->name, "liBattInstalledPack"))
    {
      if((value->integer >= 0) && (value->integer <= 16))
      {
          //Get object value
          entry->liBattInstalledPack = value->integer; 
          if (entry->liBattInstalledPack != sModbusManager.u8NumberOfLIB)
          {
            sModbusManager.u8NumberOfLIB = entry->liBattInstalledPack;
            g_sParameters.u8DevNum[TOTAL_LIB_INDEX] = sModbusManager.u8NumberOfLIB;
            g_sWorkingDefaultParameters.u8DevNum[TOTAL_LIB_INDEX] = g_sParameters.u8DevNum[TOTAL_LIB_INDEX];
            u8SaveConfigFlag |= 1;
          }
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    else
    {
        TRACE_ERROR("ERROR_OBJECT_NOT_FOUND!\r\n");
        //The specified object does not exist
        return ERROR_OBJECT_NOT_FOUND;
    }
    
    //Successful processing
    return NO_ERROR;    
}
/**
 * @brief Set LiBattGroup object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[in] value Object value
 * @param[in] valueLen Length of the object value, in bytes
 * @return Error code
 **/
error_t privateMibSetLIBattInfoEntry(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    uint_t j;
    tModbusParameters sModbusDev[20];
    PrivateMibLIBattInfoEntry *entry;

    //Point to the instance identifier
    n = object->oidLen;

    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index < 1 || index > 16)
        return ERROR_INSTANCE_NOT_FOUND;

    LibM1Addr = index;
    //Point to the libTable entry
    entry = &privateMibBase.liBattGroup.liBattTable[index - 1];
    //liionBattStatusType object?
    if(!strcmp(object->name, "liionBattStatusType"))
    {
      if((value->integer >= 0) && (value->integer <= 14))//trinh_fix
      {
        //Set object value
        entry->liBattType= value->integer;
        j = index;
        sModbusManager.sLIBManager[j-1].u8LIBType = entry->liBattType;
        
        switch(sModbusManager.sLIBManager[j-1].u8LIBType)
        {
        case 13:// COSLIGHT_CF4850T  
        case 1:// COSLIGHT
        {
            sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 0;
            sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = j;
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 0;      // FC 04
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 58;      
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 120;//128;    // FC 02
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 38;//30; 
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 48;    // FC 03
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 4;
        }
        break;
        case 2:// COSLIGHT_OLD_V1.1
        {
            sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 0;
            sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = j;
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 30;      // FC 04
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 24;      
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 128;    // FC 02
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 28; 
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 48;    // FC 03
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 2;
        }
        break;
        case 12:// SHOTO_SDA10_48100
        case 3:// SHOTO_2019
        {
            sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 0;
            sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = j;
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 0x1000;      // FC 04
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 10;      
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 0x2000;   
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 25; 
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 0x3040;   
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 56; 
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[3] = 0x800f;   
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[3] = 16; 
        }
        break;
        case 4:// HUAWEI
        {
            if(j<9)
              sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 213;
            else
              sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 215;
            sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset + j;
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 0;
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 50; 
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 67;
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 8;  
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 257;
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 7;  
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[3] = 529;
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[3] = 10; 
        }
        break;
        case 8:// HUAWEI_A1
        {
            if(j<9)
              sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 213;
            else
              sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 215;
            sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset + j;
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 0;
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 50; 
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 67;
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 8;  
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 257;
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 7;  
        }
        break;
        case 5:// M1Viettel50
        {
            sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 0;
            sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = j;
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 0; // FC:0x04
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 100;   
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 130;// FC:0x04
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 22; //20
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 0;// FC:0x03
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 80; 
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[3] = 162;// FC:0x03
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[3] = 3;//2                 
        }
        break;
        case 14:// HUAFU_HF48100C
        case 6:// ZTT_2020
        {
            sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 0;
            sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = j;
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 0;
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 42;
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 150;
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 10;
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 170;
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 10;
        }
        break;
        case 9:// SAFT
        {
            sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 0;
            sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = j;
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 23764;
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 16;
        }
        break;
        case 10:// Narada75
        {
            sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 38;
            sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = 38 + j;
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 4095;
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 9;
            
        }
        break;
        case 7:// ZTT50
        {
            sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 38;
            sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = 38 + j;
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 4096;
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 9;
        }
        break;
        case 11:// EVE
        {
            sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 0;
            sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = j;
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 0;
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 42;
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 150;
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 10;
            sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 170;
            sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 10;
        }
        break;
        };
        sModbusDev[j-1].u8DevVendor = sModbusManager.sLIBManager[j-1].u8LIBType;
        sModbusDev[j-1].u32SlaveID = sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID;

        //
        // Did parameters change?
        //
        if  (g_sParameters.sModbusParameters[j-1].u8DevVendor != sModbusDev[j-1].u8DevVendor)
        {
            //
            // Update the current parameters with the new settings.
            //
            g_sParameters.sModbusParameters[j-1].u8DevVendor = sModbusDev[j-1].u8DevVendor;
            g_sParameters.sModbusParameters[j-1].u32SlaveID = sModbusDev[j-1].u32SlaveID;
            //
            // Yes - save these settings as the defaults.
            //
            g_sWorkingDefaultParameters.sModbusParameters[j-1].u8DevVendor = g_sParameters.sModbusParameters[j-1].u8DevVendor;
            g_sWorkingDefaultParameters.sModbusParameters[j-1].u32SlaveID = g_sParameters.sModbusParameters[j-1].u32SlaveID;
             
            u8SaveConfigFlag |= 1;
        }        
        
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32PackHighVolt_A object?
    else if(!strcmp(object->name, "i32PackHighVolt_A"))
    {
      if((value->integer >= 4100) && (value->integer <= 6000))
      {
        //Get object value
        entry->i32PackHighVolt_A= value->integer;
        entry->i32PackHighVolt_AWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_PACKHIGHVOLT;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32BattHighVolt_A object?
    else if(!strcmp(object->name, "i32BattHighVolt_A"))
    {
      if((value->integer >= 4100) && (value->integer <= 6000))
      {
        //Get object value
        entry->i32BattHighVolt_A= value->integer;
        entry->i32BattHighVolt_AWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_BATTHIGHVOLT;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32CellHighVolt_A object?
    else if(!strcmp(object->name, "i32CellHighVolt_A"))
    {
      if((value->integer >= 3200) && (value->integer <= 4550))
      {
        //Get object value
        entry->i32CellHighVolt_A= value->integer;
        entry->i32CellHighVolt_AWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_CELLHIGHVOLT;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32BattLowVolt_A object?
    else if(!strcmp(object->name, "i32BattLowVolt_A"))
    {
      if((value->integer >= 3300) && (value->integer <= 5100))
      {
        //Get object value
        entry->i32BattLowVolt_A= value->integer;
        entry->i32BattLowVolt_AWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_BATTLOWVOLT;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32CellLowVolt_A object?
    else if(!strcmp(object->name, "i32CellLowVolt_A"))
    {
      if((value->integer >= 2500) && (value->integer <= 3400))
      {
        //Get object value
        entry->i32CellLowVolt_A= value->integer;
        entry->i32CellLowVolt_AWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_CELLLOWVOLT;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32CharHighCurr_A object?
    else if(!strcmp(object->name, "i32CharHighCurr_A"))
    {
      if((value->integer >= 50) && (value->integer <= 1000))
      {
        //Get object value
        entry->i32CharHighCurr_A= value->integer;
        entry->i32CharHighCurr_AWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_CHARHIGHCURR;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32DisCharHighCurr_A object?
    else if(!strcmp(object->name, "i32DisCharHighCurr_A"))
    {
      if((value->integer >= 50) && (value->integer <= 1500))
      {
        //Get object value
        entry->i32DisCharHighCurr_A= value->integer;
        entry->i32DisCharHighCurr_AWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_DISCHARHIGHCURR;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32CharHighTemp_A object?
    else if(!strcmp(object->name, "i32CharHighTemp_A"))
    {
      if((value->integer >= 2000) && (value->integer <= 6000))
      {
        //Get object value
        entry->i32CharHighTemp_A= value->integer;
        entry->i32CharHighTemp_AWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_CHARHIGHTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32DisCharHighTemp_A object?
    else if(!strcmp(object->name, "i32DisCharHighTemp_A"))
    {
      if((value->integer >= 2000) && (value->integer <= 6000))
      {
        //Get object value
        entry->i32DisCharHighTemp_A= value->integer;
        entry->i32DisCharHighTemp_AWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_DISCHARHIGHTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32CharLowTemp_A object?
    else if(!strcmp(object->name, "i32CharLowTemp_A"))
    {
      if((value->integer >= -2000) && (value->integer <= 4000))
      {
        //Get object value
        entry->i32CharLowTemp_A= value->integer;
        entry->i32CharLowTemp_AWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_CHARLOWTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32DisCharLowtemp_A object?
    else if(!strcmp(object->name, "i32DisCharLowtemp_A"))
    {
      if((value->integer >= -2000) && (value->integer <= 4000))
      {
        //Get object value
        entry->i32DisCharLowtemp_A= value->integer;
        entry->i32DisCharLowtemp_AWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_DISCHARLOWTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32LowCap_A object?
    else if(!strcmp(object->name, "i32LowCap_A"))
    {
      if((value->integer >= 0) && (value->integer <= 100))
      {
        //Get object value
        entry->i32LowCap_A= value->integer;
        entry->i32LowCap_AWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_LOWCAP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32BMSHighTemp_A object?
    else if(!strcmp(object->name, "i32BMSHighTemp_A"))
    {
      if((value->integer >= 6000) && (value->integer <= 12000))
      {
        //Get object value
        entry->i32BMSHighTemp_A= value->integer;
        entry->i32BMSHighTemp_AWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_BMSHIGHTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32BMSLowTemp_A object?
    else if(!strcmp(object->name, "i32BMSLowTemp_A"))
    {
      if((value->integer >= 0) && (value->integer <= 4000))
      {
        //Get object value
        entry->i32BMSLowTemp_A= value->integer;
        entry->i32BMSLowTemp_AWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_BMSLOWTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32PackOverVolt_P object?
    else if(!strcmp(object->name, "i32PackOverVolt_P"))
    {
      if((value->integer >= 4000) && (value->integer <= 6000))
      {
        //Get object value
        entry->i32PackOverVolt_P= value->integer;
        entry->i32PackOverVolt_PWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_PACKOVERVOLT;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32BattOverVolt_P object?
    else if(!strcmp(object->name, "i32BattOverVolt_P"))
    {
      if((value->integer >= 4000) && (value->integer <= 6000))
      {
        //Get object value
        entry->i32BattOverVolt_P= value->integer;
        entry->i32BattOverVolt_PWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_BATTOVERVOLT;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32CellOverVolt_P object?
    else if(!strcmp(object->name, "i32CellOverVolt_P"))
    {
      if((value->integer >= 3200) && (value->integer <= 4550))
      {
        //Get object value
        entry->i32CellOverVolt_P= value->integer;
        entry->i32CellOverVolt_PWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_CELLOVERVOLT;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32BattUnderVolt_P object?
    else if(!strcmp(object->name, "i32BattUnderVolt_P"))
    {
      if((value->integer >= 3300) && (value->integer <= 5000))
      {
        //Get object value
        entry->i32BattUnderVolt_P= value->integer;
        entry->i32BattUnderVolt_PWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_BATTUNDERVOLT;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32CellUnderVolt_P object?
    else if(!strcmp(object->name, "i32CellUnderVolt_P"))
    {
      if((value->integer >= 2500) && (value->integer <= 3400))
      {
        //Get object value
        entry->i32CellUnderVolt_P= value->integer;
        entry->i32CellUnderVolt_PWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_CELLUNDERVOLT;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32CharOverCurr_P object?
    else if(!strcmp(object->name, "i32CharOverCurr_P"))
    {
      if((value->integer >= 50) && (value->integer <= 1000))
      {
        //Get object value
        entry->i32CharOverCurr_P= value->integer;
        entry->i32CharOverCurr_PWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_CHAROVERCURR;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32DisCharOverCurr_P object?
    else if(!strcmp(object->name, "i32DisCharOverCurr_P"))
    {
      if((value->integer >= 50) && (value->integer <= 1500))
      {
        //Get object value
        entry->i32DisCharOverCurr_P= value->integer;
        entry->i32DisCharOverCurr_PWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_DISCHAROVERCURR;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32CharOverTemp_P object?
    else if(!strcmp(object->name, "i32CharOverTemp_P"))
    {
      if((value->integer >= 2000) && (value->integer <= 8000))
      {
        //Get object value
        entry->i32CharOverTemp_P= value->integer;
        entry->i32CharOverTemp_PWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_CHAROVERTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32DisCharOverTemp_P object?
    else if(!strcmp(object->name, "i32DisCharOverTemp_P"))
    {
      if((value->integer >= 2000) && (value->integer <= 8000))
      {
        //Get object value
        entry->i32DisCharOverTemp_P= value->integer;
        entry->i32DisCharOverTemp_PWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_DISCHAROVERTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32CharUnderTemp_P object?
    else if(!strcmp(object->name, "i32CharUnderTemp_P"))
    {
      if((value->integer >= -3000) && (value->integer <= 4000))
      {
        //Get object value
        entry->i32CharUnderTemp_P= value->integer;
        entry->i32CharUnderTemp_PWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_CHARUNDERTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32DisCharUnderTemp_P object?
    else if(!strcmp(object->name, "i32DisCharUnderTemp_P"))
    {
      if((value->integer >= -3000) && (value->integer <= 4000))
      {
        //Get object value
        entry->i32DisCharUnderTemp_P= value->integer;
        entry->i32DisCharUnderTemp_PWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_DISCHARUNDERTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32UnderCap_P object?
    else if(!strcmp(object->name, "i32UnderCap_P"))
    {
      if((value->integer >= 0) && (value->integer <= 100))
      {
        //Get object value
        entry->i32UnderCap_P= value->integer;
        entry->i32UnderCap_PWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_UNDERCAP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32BMSOverTemp_P object?
    else if(!strcmp(object->name, "i32BMSOverTemp_P"))
    {
      if((value->integer >= 6000) && (value->integer <= 15000))
      {
        //Get object value
        entry->i32BMSOverTemp_P= value->integer;
        entry->i32BMSOverTemp_PWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_BMSOVERTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32BMSUnderTemp_P object?
    else if(!strcmp(object->name, "i32BMSUnderTemp_P"))
    {
      if((value->integer >= -2000) && (value->integer <= 4000))
      {
        //Get object value
        entry->i32BMSUnderTemp_P= value->integer;
        entry->i32BMSUnderTemp_PWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_BMSUNDERTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //i32DifferentVolt_P object?
    else if(!strcmp(object->name, "i32DifferentVolt_P"))
    {
      if((value->integer >= 50) && (value->integer <= 700))
      {
        //Get object value
        entry->i32DifferentVolt_P= value->integer;
        entry->i32DifferentVolt_PWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_DIFFERENTVOLT;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //u32VoltBalance object?
    else if(!strcmp(object->name, "u32VoltBalance"))
    {
      if((value->integer >= 4000) && (value->integer <= 5800))
      {
        //Get object value
        entry->u32VoltBalance= value->integer;
        entry->u32VoltBalanceWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_VOLTBALANCE;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //u32DeltaVoltBalance object?
    else if(!strcmp(object->name, "u32DeltaVoltBalance"))
    {
      if((value->integer >= 0) && (value->integer <= 500))
      {
        //Get object value
        entry->u32DeltaVoltBalance= value->integer;
        entry->u32DeltaVoltBalanceWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_DELTAVOLTBALANCE;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //u32DisCharCurrLimit object?
    else if(!strcmp(object->name, "u32DisCharCurrLimit"))
    {
      if((value->integer >= 0) && (value->integer <= 99))
      {
        //Get object value
        entry->u32DisCharCurrLimit= value->integer;
        entry->u32DisCharCurrLimitWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_DISCHARCURRLIMIT;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //u32CharCurrLimit object?
    else if(!strcmp(object->name, "u32CharCurrLimit"))
    {
      if((value->integer >= 0) && (value->integer <= 60))
      {
        //Get object value
        entry->u32CharCurrLimit= value->integer;
        entry->u32CharCurrLimitWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_CHARCURRLIMIT;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //u32VoltDisCharRef object?
    else if(!strcmp(object->name, "u32VoltDisCharRef"))
    {
      if((value->integer >= 4000) && (value->integer <= 6000))
      {
        //Get object value
        entry->u32VoltDisCharRef= value->integer;
        entry->u32VoltDisCharRefWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_VOLTDISCHARREF;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //u32VoltDisCharCMD object?
    else if(!strcmp(object->name, "u32VoltDisCharCMD"))
    {
      if((value->integer >= 0) && (value->integer <= 4))
      {
        //Get object value
        entry->u32VoltDisCharCMD= value->integer;
        entry->u32VoltDisCharCMDWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_VOLTDISCHARCMD;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //u32SystemByte object?
    else if(!strcmp(object->name, "u32SystemByte"))
    {
//      if((value->integer >= 0) && (value->integer <= 300))
//      {
        //Get object value
        entry->u32SystemByte= value->integer;
        entry->u32SystemByteWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_SYSTEMBYTE;
//      }
//      else
//      {
//        return ERROR_PARAMETER_OUT_OF_RANGE;
//      }
    }
    //u32KeyTime object?
    else if(!strcmp(object->name, "u32KeyTime"))
    {
      if((value->integer >= 0) && (value->integer <= 64))
      {
        //Get object value
        entry->u32KeyTime= value->integer;
        entry->u32KeyTimeWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_KEYTIME;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //u32IniMaxim object?
    else if(!strcmp(object->name, "u32IniMaxim"))
    {
      if((value->integer >= 0) && (value->integer <= 1))
      {
        //Get object value
        entry->u32IniMaxim= value->integer;
        entry->u32IniMaximWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_INIMAXIM;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //u32EnableKCS object?
    else if(!strcmp(object->name, "u32EnableKCS"))
    {
      if((value->integer >= 0) && (value->integer <= 1))
      {
        //Get object value
        entry->u32EnableKCS= value->integer;
        entry->u32EnableKCSWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_ENABLEKCS;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //u32EnableLock object?
    else if(!strcmp(object->name, "u32EnableLock"))
    {
      if((value->integer >= 0) && (value->integer <= 1))
      {
        //Get object value
        entry->u32EnableLock= value->integer;
        entry->u32EnableLockWrite= value->integer;
        sModbusManager.SettingCommand = SET_LIB_ENABLELOCK;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //liionCapInit object?
    else if(!strcmp(object->name, "liionCapInit"))
    {
      if((value->integer >= 30) && (value->integer <= 150))
      {
        //Get object value
        entry->u16_liionCapInit       = value->integer;
        entry->u16_liionCapInitWrite  = value->integer;
        sModbusManager.SettingCommand = SET_LIB_CAPINIT;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //Successful processing
    return NO_ERROR;
}
/**
 * @brief Get liBattInfoEntry object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t privateMibGetLIBattInfoEntry(const MibObject *object, const uint8_t *oid,
                                  size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibLIBattInfoEntry *entry;

    //Point to the instance identifier
    n = object->oidLen;

    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index < 1 || index > 16)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the interface table entry
    entry = &privateMibBase.liBattGroup.liBattTable[index - 1];
    //liBattIndex object?
    if(!strcmp(object->name, "liBattIndex"))
    {
        //Get object value
        value->integer = entry->liBattIndex;
    }
    //liBattStatus object?
    else if(!strcmp(object->name, "liBattStatus"))
    {
        //Get object value
        value->integer = entry->liBattStatus;
    }
    //liBattPackVolt object?
    else if(!strcmp(object->name, "liBattPackVolt"))
    {
        //Get object value
        value->integer = entry->liBattPackVolt;
    }
    //liBattPackCurr object?
    else if(!strcmp(object->name, "liBattPackCurr"))
    {
        //Get object value
        value->integer = entry->liBattPackCurr;
    }
    //liBattRemainCap object?
    else if(!strcmp(object->name, "liBattRemainCap"))
    {
        //Get object value
        value->integer = entry->liBattRemainCap;
    }
    //liBattAvrCellTemp object?
    else if(!strcmp(object->name, "liBattAvrCellTemp"))
    {
        //Get object value
        value->integer = entry->liBattAvrCellTemp;
    }
    //liBattAmbTemp object?
    else if(!strcmp(object->name, "liBattAmbTemp"))
    {
        //Get object value
        value->integer = entry->liBattAmbTemp;
    }
    //liBattWarningFlag object?
    else if(!strcmp(object->name, "liBattWarningFlag"))
    {
        //Get object value
        value->integer = entry->liBattWarningFlag;
    }
    //liBattProtectFlag object?
    else if(!strcmp(object->name, "liBattProtectFlag"))
    {
        //Get object value
        value->integer = entry->liBattProtectFlag;
    }
    //liBattFaultStat object?
    else if(!strcmp(object->name, "liBattFaultStat"))
    {
        //Get object value
        value->integer = entry->liBattFaultStat;
    }
    //u32SOCMaxim object?
    else if(!strcmp(object->name, "u32SOCMaxim"))
    {
        //Get object value
        value->integer = entry->u32SOCMaxim;
    }
    //liBattSOH object?
    else if(!strcmp(object->name, "liBattSOH"))
    {
        //Get object value
        value->integer = entry->liBattSOH;
    }
    //liBattPeriod object?
    else if(!strcmp(object->name, "liBattPeriod"))
    {
        //Get object value
        value->integer = entry->liBattPeriod;
    }
    //liBattHWVer object?
    else if(!strcmp(object->name, "liBattHWVer"))
    {
      //Make sure the buffer is large enough to hold the entire object
      if(*valueLen >= entry->liBattHWVerLen)
      {
          //Copy object value
          memcpy(value->octetString, entry->liBattHWVer, entry->liBattHWVerLen);
          //Return object length
          *valueLen = entry->liBattHWVerLen;
      }
      else
      {
          //Report an error
          error = ERROR_BUFFER_OVERFLOW;
      }
    }
    //liBattSWVer object?
    else if(!strcmp(object->name, "liBattSWVer"))
    {
      //Make sure the buffer is large enough to hold the entire object
      if(*valueLen >= entry->liBattSWVerLen)
      {
          //Copy object value
          memcpy(value->octetString, entry->liBattSWVer, entry->liBattSWVerLen);
          //Return object length
          *valueLen = entry->liBattSWVerLen;
      }
      else
      {
          //Report an error
          error = ERROR_BUFFER_OVERFLOW;
      }
    }
    //liBattSerialNo object?
    else if(!strcmp(object->name, "liBattSerialNo"))
    {
      //Make sure the buffer is large enough to hold the entire object
      if(*valueLen >= entry->liBattSerialNoLen)
      {
          //Copy object value
          memcpy(value->octetString, entry->liBattSerialNo, entry->liBattSerialNoLen);
          //Return object length
          *valueLen = entry->liBattSerialNoLen;
      }
      else
      {
          //Report an error
          error = ERROR_BUFFER_OVERFLOW;
      }
    }
    //liBattModel object?        // tmt
    else if(!strcmp(object->name, "liBattModel"))
    {
      //Make sure the buffer is large enough to hold the entire object
      if(*valueLen >= entry->liBattModelLen)
      {
          //Copy object value
          memcpy(value->octetString, entry->liBattModel, entry->liBattModelLen);
          //Return object length
          *valueLen = entry->liBattModelLen;
      }
      else
      {
          //Report an error
          error = ERROR_BUFFER_OVERFLOW;
      }
    }
    //liBattCellVolt1 object?
    else if(!strcmp(object->name, "liBattCellVolt1"))
    {
        //Get object value
        value->integer = entry->liBattCellVolt[0];
    }
    //liBattCellVolt2 object?
    else if(!strcmp(object->name, "liBattCellVolt2"))
    {
        //Get object value
        value->integer = entry->liBattCellVolt[1];
    }
    //liBattCellVolt3 object?
    else if(!strcmp(object->name, "liBattCellVolt3"))
    {
        //Get object value
        value->integer = entry->liBattCellVolt[2];
    }
    //liBattCellVolt4 object?
    else if(!strcmp(object->name, "liBattCellVolt4"))
    {
        //Get object value
        value->integer = entry->liBattCellVolt[3];
    }
    //liBattCellVolt5 object?
    else if(!strcmp(object->name, "liBattCellVolt5"))
    {
        //Get object value
        value->integer = entry->liBattCellVolt[4];
    }
    //liBattCellVolt6 object?
    else if(!strcmp(object->name, "liBattCellVolt6"))
    {
        //Get object value
        value->integer = entry->liBattCellVolt[5];
    }
    //liBattCellVolt7 object?
    else if(!strcmp(object->name, "liBattCellVolt7"))
    {
        //Get object value
        value->integer = entry->liBattCellVolt[6];
    }
    //liBattCellVolt8 object?
    else if(!strcmp(object->name, "liBattCellVolt8"))
    {
        //Get object value
        value->integer = entry->liBattCellVolt[7];
    }
    //liBattCellVolt9 object?
    else if(!strcmp(object->name, "liBattCellVolt9"))
    {
        //Get object value
        value->integer = entry->liBattCellVolt[8];
    }
    //liBattCellVolt10 object?
    else if(!strcmp(object->name, "liBattCellVolt10"))
    {
        //Get object value
        value->integer = entry->liBattCellVolt[9];
    }
    //liBattCellVolt11 object?
    else if(!strcmp(object->name, "liBattCellVolt11"))
    {
        //Get object value
        value->integer = entry->liBattCellVolt[10];
    }
    //liBattCellVolt12 object?
    else if(!strcmp(object->name, "liBattCellVolt12"))
    {
        //Get object value
        value->integer = entry->liBattCellVolt[11];
    }
    //liBattCellVolt13 object?
    else if(!strcmp(object->name, "liBattCellVolt13"))
    {
        //Get object value
        value->integer = entry->liBattCellVolt[12];
    }
    //liBattCellVolt14 object?
    else if(!strcmp(object->name, "liBattCellVolt14"))
    {
        //Get object value
        value->integer = entry->liBattCellVolt[13];
    }
    //liBattCellVolt15 object?
    else if(!strcmp(object->name, "liBattCellVolt15"))
    {
        //Get object value
        value->integer = entry->liBattCellVolt[14];
    }
    //liBattCellVolt16 object?
    else if(!strcmp(object->name, "liBattCellVolt16"))
    {
        //Get object value
        value->integer = entry->liBattCellVolt[15];
    }
    //liBattCellTemp1 object?
    else if(!strcmp(object->name, "liBattCellTemp1"))
    {
        //Get object value
        value->integer = entry->liBattCellTemp[0];
    }
    //liBattCellTemp2 object?
    else if(!strcmp(object->name, "liBattCellTemp2"))
    {
        //Get object value
        value->integer = entry->liBattCellTemp[1];
    }
    //liBattCellTemp3 object?
    else if(!strcmp(object->name, "liBattCellTemp3"))
    {
        //Get object value
        value->integer = entry->liBattCellTemp[2];
    }
    //liBattCellTemp4 object?
    else if(!strcmp(object->name, "liBattCellTemp4"))
    {
        //Get object value
        value->integer = entry->liBattCellTemp[3];
    }
    //liBattCellTemp5 object?
    else if(!strcmp(object->name, "liBattCellTemp5"))
    {
        //Get object value
        value->integer = entry->liBattCellTemp[4];
    }
    //liBattCellTemp6 object?
    else if(!strcmp(object->name, "liBattCellTemp6"))
    {
        //Get object value
        value->integer = entry->liBattCellTemp[5];
    }
    //liBattCellTemp7 object?
    else if(!strcmp(object->name, "liBattCellTemp7"))
    {
        //Get object value
        value->integer = entry->liBattCellTemp[6];
    }
    //liBattCellTemp8 object?
    else if(!strcmp(object->name, "liBattCellTemp8"))
    {
        //Get object value
        value->integer = entry->liBattCellTemp[7];
    }
    //liBattCellTemp9 object?
    else if(!strcmp(object->name, "liBattCellTemp9"))
    {
        //Get object value
        value->integer = entry->liBattCellTemp[8];
    }
    //liBattCellTemp10 object?
    else if(!strcmp(object->name, "liBattCellTemp10"))
    {
        //Get object value
        value->integer = entry->liBattCellTemp[9];
    }
    //liBattCellTemp11 object?
    else if(!strcmp(object->name, "liBattCellTemp11"))
    {
        //Get object value
        value->integer = entry->liBattCellTemp[10];
    }
    //liBattCellTemp12 object?
    else if(!strcmp(object->name, "liBattCellTemp12"))
    {
        //Get object value
        value->integer = entry->liBattCellTemp[11];
    }
    //liBattCellTemp13 object?
    else if(!strcmp(object->name, "liBattCellTemp13"))
    {
        //Get object value
        value->integer = entry->liBattCellTemp[12];
    }
    //liBattCellTemp14 object?
    else if(!strcmp(object->name, "liBattCellTemp14"))
    {
        //Get object value
        value->integer = entry->liBattCellTemp[13];
    }
    //liBattCellTemp15 object?
    else if(!strcmp(object->name, "liBattCellTemp15"))
    {
        //Get object value
        value->integer = entry->liBattCellTemp[14];
    }
    //liBattCellTemp16 object?
    else if(!strcmp(object->name, "liBattCellTemp16"))
    {
        //Get object value
        value->integer = entry->liBattCellTemp[15];
    }
    //liBattModbusID object?
    else if(!strcmp(object->name, "liBattModbusID"))
    {
        //Get object value
        value->integer = entry->liBattModbusID;
    }
    //liBattAlarmStatus object?
    else if(!strcmp(object->name, "liBattAlarmStatus"))
    {
        //Get object value
        value->counter32 = entry->liBattAlarmStatus.all;
    }
    //liionBattStatusType object?
    else if(!strcmp(object->name, "liionBattStatusType"))
    {
        //Get object value
        value->integer = entry->liBattType;
    }
    //liionBattStatusCCL object?
    else if(!strcmp(object->name, "liionBattStatusCCL"))
    {
        //Get object value
        value->integer = entry->liBattStatusCCL;
    }
    //liBattMode object?
    else if(!strcmp(object->name, "liBattMode"))
    {
        //Get object value
        value->integer = entry->liBattMode;
    }
    //u32ConverterState object?
    else if(!strcmp(object->name, "u32ConverterState"))
    {
        //Get object value
        value->integer = entry->u32ConverterState;
    }
    //u32ErrCode object?
    else if(!strcmp(object->name, "u32ErrCode"))
    {
        //Get object value
        value->integer = entry->u32ErrCode;
    }
    //u32BalanceStatus object?
    else if(!strcmp(object->name, "u32BalanceStatus"))
    {
        //Get object value
        value->integer = entry->u32BalanceStatus;
    }
    //u32MosfetMode object?
    else if(!strcmp(object->name, "u32MosfetMode"))
    {
        //Get object value
        value->integer = entry->u32MosfetMode;
    }
    //u32Mcu2McuErr object?
    else if(!strcmp(object->name, "u32Mcu2McuErr"))
    {
        //Get object value
        value->integer = entry->u32Mcu2McuErr;
    }
    //u32CHGCapacity object?
    else if(!strcmp(object->name, "u32CHGCapacity"))
    {
        //Get object value
        value->integer = entry->u32CHGCapacity;
    }
    //u32DSGCapacity object?
    else if(!strcmp(object->name, "u32DSGCapacity"))
    {
        //Get object value
        value->integer = entry->u32DSGCapacity;
    }
    //u32Efficiency object?
    else if(!strcmp(object->name, "u32Efficiency"))
    {
        //Get object value
        value->integer = entry->u32Efficiency;
    }
    //u32NumberOfCan object?
    else if(!strcmp(object->name, "u32NumberOfCan"))
    {
        //Get object value
        value->integer = entry->u32NumberOfCan;
    }
    //i32PackHighVolt_A
    else if(!strcmp(object->name, "i32PackHighVolt_A"))
    {
        //Get object value
        value->integer = entry->i32PackHighVolt_A;
    }
    
    //i32BattHighVolt_A object?
    else if(!strcmp(object->name, "i32BattHighVolt_A"))
    {
        //Get object value
        value->integer = entry->i32BattHighVolt_A;
    }
    //i32CellHighVolt_A object?
    else if(!strcmp(object->name, "i32CellHighVolt_A"))
    {
        //Get object value
        value->integer = entry->i32CellHighVolt_A;
    }
    //i32BattLowVolt_A object?
    else if(!strcmp(object->name, "i32BattLowVolt_A"))
    {
        //Get object value
        value->integer = entry->i32BattLowVolt_A;
    }
    //i32CellLowVolt_A object?
    else if(!strcmp(object->name, "i32CellLowVolt_A"))
    {
        //Get object value
        value->integer = entry->i32CellLowVolt_A;
    }
    //i32CharHighCurr_A object?
    else if(!strcmp(object->name, "i32CharHighCurr_A"))
    {
        //Get object value
        value->integer = entry->i32CharHighCurr_A;
    }
    //i32DisCharHighCurr_A object?
    else if(!strcmp(object->name, "i32DisCharHighCurr_A"))
    {
        //Get object value
        value->integer = entry->i32DisCharHighCurr_A;
    }
    //i32CharHighTemp_A object?
    else if(!strcmp(object->name, "i32CharHighTemp_A"))
    {
        //Get object value
        value->integer = entry->i32CharHighTemp_A;
    }
    //i32DisCharHighTemp_A object?
    else if(!strcmp(object->name, "i32DisCharHighTemp_A"))
    {
        //Get object value
        value->integer = entry->i32DisCharHighTemp_A;
    }
    //i32CharLowTemp_A object?
    else if(!strcmp(object->name, "i32CharLowTemp_A"))
    {
        //Get object value
        value->integer = entry->i32CharLowTemp_A;
    }
    //i32DisCharLowtemp_A object?
    else if(!strcmp(object->name, "i32DisCharLowtemp_A"))
    {
        //Get object value
        value->integer = entry->i32DisCharLowtemp_A;
    }
    //i32LowCap_A object?
    else if(!strcmp(object->name, "i32LowCap_A"))
    {
        //Get object value
        value->integer = entry->i32LowCap_A;
    }
    //i32BMSHighTemp_A object?
    else if(!strcmp(object->name, "i32BMSHighTemp_A"))
    {
        //Get object value
        value->integer = entry->i32BMSHighTemp_A;
    }
    //i32BMSLowTemp_A object?
    else if(!strcmp(object->name, "i32BMSLowTemp_A"))
    {
        //Get object value
        value->integer = entry->i32BMSLowTemp_A;
    }
    //i32PackOverVolt_P object?
    else if(!strcmp(object->name, "i32PackOverVolt_P"))
    {
        //Get object value
        value->integer = entry->i32PackOverVolt_P;
    }
    //i32BattOverVolt_P object?
    else if(!strcmp(object->name, "i32BattOverVolt_P"))
    {
        //Get object value
        value->integer = entry->i32BattOverVolt_P;
    }
    //i32CellOverVolt_P object?
    else if(!strcmp(object->name, "i32CellOverVolt_P"))
    {
        //Get object value
        value->integer = entry->i32CellOverVolt_P;
    }
    //i32BattUnderVolt_P object?
    else if(!strcmp(object->name, "i32BattUnderVolt_P"))
    {
        //Get object value
        value->integer = entry->i32BattUnderVolt_P;
    }
    //i32CellUnderVolt_P object?
    else if(!strcmp(object->name, "i32CellUnderVolt_P"))
    {
        //Get object value
        value->integer = entry->i32CellUnderVolt_P;
    }
    //i32CharOverCurr_P object?
    else if(!strcmp(object->name, "i32CharOverCurr_P"))
    {
        //Get object value
        value->integer = entry->i32CharOverCurr_P;
    }
    //i32DisCharOverCurr_P object?
    else if(!strcmp(object->name, "i32DisCharOverCurr_P"))
    {
        //Get object value
        value->integer = entry->i32DisCharOverCurr_P;
    }
    //i32CharOverTemp_P object?
    else if(!strcmp(object->name, "i32CharOverTemp_P"))
    {
        //Get object value
        value->integer = entry->i32CharOverTemp_P;
    }
    //i32DisCharOverTemp_P object?
    else if(!strcmp(object->name, "i32DisCharOverTemp_P"))
    {
        //Get object value
        value->integer = entry->i32DisCharOverTemp_P;
    }
    //i32CharUnderTemp_P object?
    else if(!strcmp(object->name, "i32CharUnderTemp_P"))
    {
        //Get object value
        value->integer = entry->i32CharUnderTemp_P;
    }
    //i32DisCharUnderTemp_P object?
    else if(!strcmp(object->name, "i32DisCharUnderTemp_P"))
    {
        //Get object value
        value->integer = entry->i32DisCharUnderTemp_P;
    }
    //i32UnderCap_P object?
    else if(!strcmp(object->name, "i32UnderCap_P"))
    {
        //Get object value
        value->integer = entry->i32UnderCap_P;
    }
    //i32BMSOverTemp_P object?
    else if(!strcmp(object->name, "i32BMSOverTemp_P"))
    {
        //Get object value
        value->integer = entry->i32BMSOverTemp_P;
    }
    //i32BMSUnderTemp_P object?
    else if(!strcmp(object->name, "i32BMSUnderTemp_P"))
    {
        //Get object value
        value->integer = entry->i32BMSUnderTemp_P;
    }
    //i32DifferentVolt_P object?
    else if(!strcmp(object->name, "i32DifferentVolt_P"))
    {
        //Get object value
        value->integer = entry->i32DifferentVolt_P;
    }
    //u32VoltBalance object?
    else if(!strcmp(object->name, "u32VoltBalance"))
    {
        //Get object value
        value->integer = entry->u32VoltBalance;
    }
    //u32DeltaVoltBalance object?
    else if(!strcmp(object->name, "u32DeltaVoltBalance"))
    {
        //Get object value
        value->integer = entry->u32DeltaVoltBalance;
    }
    //u32DisCharCurrLimit object?
    else if(!strcmp(object->name, "u32DisCharCurrLimit"))
    {
        //Get object value
        value->integer = entry->u32DisCharCurrLimit;
    }
    //u32CharCurrLimit object?
    else if(!strcmp(object->name, "u32CharCurrLimit"))
    {
        //Get object value
        value->integer = entry->u32CharCurrLimit;
    }
    //u32VoltDisCharRef object?
    else if(!strcmp(object->name, "u32VoltDisCharRef"))
    {
        //Get object value
        value->integer = entry->u32VoltDisCharRef;
    }
    //u32VoltDisCharCMD object?
    else if(!strcmp(object->name, "u32VoltDisCharCMD"))
    {
        //Get object value
        value->integer = entry->u32VoltDisCharCMD;
    }
    //u32SystemByte object?
    else if(!strcmp(object->name, "u32SystemByte"))
    {
        //Get object value
        value->integer = entry->u32SystemByte;
    }
    //u32KeyTime object?
    else if(!strcmp(object->name, "u32KeyTime"))
    {
        //Get object value
        value->integer = entry->u32KeyTime;
    }
    //u32IniMaxim object?
    else if(!strcmp(object->name, "u32IniMaxim"))
    {
        //Get object value
        value->integer = entry->u32IniMaxim;
    }
    //u32EnableKCS object?
    else if(!strcmp(object->name, "u32EnableKCS"))
    {
        //Get object value
        value->integer = entry->u32EnableKCS;
    }
    //u32EnableLock object?
    else if(!strcmp(object->name, "u32EnableLock"))
    {
        //Get object value
        value->integer = entry->u32EnableLock;
    }
    //u32ADCPackVolt object?
    else if(!strcmp(object->name, "u32ADCPackVolt"))
    {
        //Get object value
        value->integer = entry->u32ADCPackVolt;
    }
    //u32ADCBattVolt object?
    else if(!strcmp(object->name, "u32ADCBattVolt"))
    {
        //Get object value
        value->integer = entry->u32ADCBattVolt;
    }
    //i32ADCPackCurr object?
    else if(!strcmp(object->name, "i32ADCPackCurr"))
    {
        //Get object value
        value->integer = entry->i32ADCPackCurr;
    }
    //i32ADCBattCurr object?
    else if(!strcmp(object->name, "i32ADCBattCurr"))
    {
        //Get object value
        value->integer = entry->i32ADCBattCurr;
    }
    //liBattSOC object?
    else if(!strcmp(object->name, "liBattSOC"))
    {
        //Get object value
        value->integer = entry->liBattSOC;
    }
    //u32SOHMaxim object?
    else if(!strcmp(object->name, "u32SOHMaxim"))
    {
        //Get object value
        value->integer = entry->u32SOHMaxim;
    }
    //u32FullCapRepMaxim object?
    else if(!strcmp(object->name, "u32FullCapRepMaxim"))
    {
        //Get object value
        value->integer = entry->u32FullCapRepMaxim;
    }
    //u32VoltMaxim object?
    else if(!strcmp(object->name, "u32VoltMaxim"))
    {
        //Get object value
        value->integer = entry->u32VoltMaxim;
    }
    //u32CurrMaxim object?
    else if(!strcmp(object->name, "u32CurrMaxim"))
    {
        //Get object value
        value->integer = entry->u32CurrMaxim;
    }
    //u32IKalamn object?
    else if(!strcmp(object->name, "u32IKalamn"))
    {
        //Get object value
        value->integer = entry->u32IKalamn;
    }
    //u32SOCKalamn object?
    else if(!strcmp(object->name, "u32SOCKalamn"))
    {
        //Get object value
        value->integer = entry->u32SOCKalamn;
    }
    //u32VpriKalamn object?
    else if(!strcmp(object->name, "u32VpriKalamn"))
    {
        //Get object value
        value->integer = entry->u32VpriKalamn;
    }
    //u32VmesKalamn object?
    else if(!strcmp(object->name, "u32VmesKalamn"))
    {
        //Get object value
        value->integer = entry->u32VmesKalamn;
    }
    //u32CapKalamn object?
    else if(!strcmp(object->name, "u32CapKalamn"))
    {
        //Get object value
        value->integer = entry->u32CapKalamn;
    }
    //u32InternalR0Kalamn object?
    else if(!strcmp(object->name, "u32InternalR0Kalamn"))
    {
        //Get object value
        value->integer = entry->u32InternalR0Kalamn;
    }
    //liionCapInit object?
    else if(!strcmp(object->name, "liionCapInit"))
    {
        //Get object value
        value->integer = entry->u16_liionCapInit;
    }

    //Return status code
    return error;
}


/**
 * @brief Get next liBattInfoEntry object
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] nextOid OID of the next object in the MIB
 * @param[out] nextOidLen Length of the next object identifier, in bytes
 * @return Error code
 **/
error_t privateMibGetNextLIBattInfoEntry(const MibObject *object, const uint8_t *oid,
                                      size_t oidLen, uint8_t *nextOid, size_t *nextOidLen)
{
    error_t error;
    size_t n;
    uint_t index;

    //Make sure the buffer is large enough to hold the OID prefix
    if(*nextOidLen < object->oidLen)
        return ERROR_BUFFER_OVERFLOW;

    //Copy OID prefix
    memcpy(nextOid, object->oid, object->oidLen);

    //Loop through network interfaces
    for(index = 1; index <= privateMibBase.liBattGroup.liBattInstalledPack; index++)
    {
        //Append the instance identifier to the OID prefix
        n = object->oidLen;

        //The ifIndex is used as instance identifier
        error = mibEncodeIndex(nextOid, *nextOidLen, &n, index);
        //Any error to report?
        if(error) return error;

        //Check whether the resulting object identifier lexicographically
        //follows the specified OID
        if(oidComp(nextOid, n, oid, oidLen) > 0)
        {
            //Save the length of the resulting object identifier
            *nextOidLen = n;
            //Next object found
            return NO_ERROR;
        }
    }

    //The specified OID does not lexicographically precede the name
    //of some object
    return ERROR_OBJECT_NOT_FOUND;
}
//========================================== LIBattInfo Function ==========================================//
//========================================== GenInfo Function ==========================================//
error_t privateMibSetGenGroup(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibGenInfoGroup *entry;
    
    //	Point to the liBattGroup entry
    entry = &privateMibBase.genGroup;
    
    if(!strcmp(object->name, "genInstalledGen"))
    {
      if((value->integer >= 0) && (value->integer <= 2))
      {
          //Get object value
          entry->genInstalledGen = value->integer; 
          if (entry->genInstalledGen != sModbusManager.u8NumberOfGen)
          {
              sModbusManager.u8NumberOfGen = entry->genInstalledGen;
              g_sParameters.u8DevNum[TOTAL_GEN_INDEX] = sModbusManager.u8NumberOfGen;
              g_sWorkingDefaultParameters.u8DevNum[TOTAL_GEN_INDEX] = g_sParameters.u8DevNum[TOTAL_GEN_INDEX];
              u8SaveConfigFlag |= 1;
          }
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    else
    {
        TRACE_ERROR("ERROR_OBJECT_NOT_FOUND!\r\n");
        //The specified object does not exist
        return ERROR_OBJECT_NOT_FOUND;
    }
    
    //Successful processing
    return NO_ERROR;    
}
/**
 * @brief Set LiBattGroup object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[in] value Object value
 * @param[in] valueLen Length of the object value, in bytes
 * @return Error code
 **/
error_t privateMibSetGenInfoEntry(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    uint_t j;
    tModbusParameters sModbusDev[3];
    PrivateMibGenInfoEntry *entry;

    //Point to the instance identifier
    n = object->oidLen;
    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);//1
    //error = mibDecodeIndex(oid, oidLen, &n, &index);//2
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index < 1 || index > 2)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the libTable entry
    entry = &privateMibBase.genGroup.genTable[index - 1];
    //genStatusType object?
    if(!strcmp(object->name, "genStatusType"))
    {
      if((value->integer >= 0) && (value->integer <= 5))
      {
        //Set object value
        entry->genStatusType= value->integer;
        j = index;
        sModbusManager.sGenManager[j-1].u8GenType = entry->genStatusType;
        
        switch(sModbusManager.sGenManager[j-1].u8GenType)
        {
        case 1:// KUBOTA
        {
            sModbusManager.sGenManager[j-1].u32GenSlaveOffset = 60;
            sModbusManager.sGenManager[j-1].u32GenAbsSlaveID = 60+j;
            sModbusManager.sGenManager[j-1].u32GenBaseAddr[0] = 0;
            sModbusManager.sGenManager[j-1].u32GenBaseAddr[1] = 235;
            sModbusManager.sGenManager[j-1].u8GenNumOfReg[0] = 85;    
            sModbusManager.sGenManager[j-1].u8GenNumOfReg[1] = 1; 
        }
        break;
        case 2:// BE142
        {
            sModbusManager.sGenManager[j-1].u32GenSlaveOffset = 60;
            sModbusManager.sGenManager[j-1].u32GenAbsSlaveID = 60+j;
            sModbusManager.sGenManager[j-1].u32GenBaseAddr[0] = 30010;
            sModbusManager.sGenManager[j-1].u32GenBaseAddr[1] = 30135;
            sModbusManager.sGenManager[j-1].u32GenBaseAddr[2] = 30079;
            sModbusManager.sGenManager[j-1].u8GenNumOfReg[0] = 38;    
            sModbusManager.sGenManager[j-1].u8GenNumOfReg[1] = 4;    
            sModbusManager.sGenManager[j-1].u8GenNumOfReg[2] = 27;
        }
        break;
        case 3:// DEEPSEA
        {
            sModbusManager.sGenManager[j-1].u32GenSlaveOffset = 60;
            sModbusManager.sGenManager[j-1].u32GenAbsSlaveID = 60+j;
            sModbusManager.sGenManager[j-1].u32GenBaseAddr[0] = 0x400;
            sModbusManager.sGenManager[j-1].u32GenBaseAddr[1] = 0x700;
            sModbusManager.sGenManager[j-1].u32GenBaseAddr[2] = 0x800;
            sModbusManager.sGenManager[j-1].u32GenBaseAddr[3] = 0x304;
            sModbusManager.sGenManager[j-1].u8GenNumOfReg[0] = 34;    
            sModbusManager.sGenManager[j-1].u8GenNumOfReg[1] = 8; 
            sModbusManager.sGenManager[j-1].u8GenNumOfReg[2] = 8;   
            sModbusManager.sGenManager[j-1].u8GenNumOfReg[3] = 1; 
        }
        break;
        case 4:// LR2057
        {
            sModbusManager.sGenManager[j-1].u32GenSlaveOffset = 60;
            sModbusManager.sGenManager[j-1].u32GenAbsSlaveID = 60+j;
            sModbusManager.sGenManager[j-1].u32GenBaseAddr[0] = 6;
            sModbusManager.sGenManager[j-1].u8GenNumOfReg[0] = 19;  
            sModbusManager.sGenManager[j-1].u32GenBaseAddr[1] = 42;
            sModbusManager.sGenManager[j-1].u8GenNumOfReg[1] = 13; 
            sModbusManager.sGenManager[j-1].u32GenBaseAddr[2] = 12;
            sModbusManager.sGenManager[j-1].u8GenNumOfReg[2] = 66; 

        }
        break;
        case 5:// HIMOINSA
        {
            sModbusManager.sGenManager[j-1].u32GenSlaveOffset = 86;
            sModbusManager.sGenManager[j-1].u32GenAbsSlaveID = 86+j;
            sModbusManager.sGenManager[j-1].u32GenBaseAddr[0] = 0;      // FC 0x02
            sModbusManager.sGenManager[j-1].u8GenNumOfReg[0] = 23;  
            sModbusManager.sGenManager[j-1].u32GenBaseAddr[1] = 7;      // FC 0x04
            sModbusManager.sGenManager[j-1].u8GenNumOfReg[1] = 36; 

        }
        break;
        case 6:// QC315
        {
            sModbusManager.sGenManager[j-1].u32GenSlaveOffset = 60;
            sModbusManager.sGenManager[j-1].u32GenAbsSlaveID = 60+j;
            sModbusManager.sGenManager[j-1].u32GenBaseAddr[0] = 20;
            sModbusManager.sGenManager[j-1].u32GenBaseAddr[1] = 708;
            sModbusManager.sGenManager[j-1].u32GenBaseAddr[2] = 800;
            sModbusManager.sGenManager[j-1].u8GenNumOfReg[0] = 61;   
            sModbusManager.sGenManager[j-1].u8GenNumOfReg[1] = 6;   
            sModbusManager.sGenManager[j-1].u8GenNumOfReg[2] = 7; 
        }
        break;
        case 7:// CUMMIN
        {
            sModbusManager.sGenManager[j-1].u32GenSlaveOffset = 60;
            sModbusManager.sGenManager[j-1].u32GenAbsSlaveID = 60+j;
            sModbusManager.sGenManager[j-1].u32GenBaseAddr[0] = 12;
//            sModbusManager.sGenManager[j-1].u32GenBaseAddr[1] = 708;
//            sModbusManager.sGenManager[j-1].u32GenBaseAddr[2] = 800;
            sModbusManager.sGenManager[j-1].u8GenNumOfReg[0] = 60;   
//            sModbusManager.sGenManager[j-1].u8GenNumOfReg[1] = 6;   
//            sModbusManager.sGenManager[j-1].u8GenNumOfReg[2] = 7; 
        }
        break;
        };
        sModbusDev[j-1].u8DevVendor = sModbusManager.sGenManager[j-1].u8GenType;
        sModbusDev[j-1].u32SlaveID = sModbusManager.sGenManager[j-1].u32GenAbsSlaveID;  
        //
        // Did parameters change?
        //
        if  (g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB].u8DevVendor != sModbusDev[j-1].u8DevVendor)
        {
          //
          // Update the current parameters with the new settings.
          //
          g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB].u8DevVendor = sModbusDev[j-1].u8DevVendor;
          g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB].u32SlaveID = sModbusDev[j-1].u32SlaveID;
          //
          // Yes - save these settings as the defaults.
          //
          g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB].u8DevVendor = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB].u8DevVendor;
          g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB].u32SlaveID = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB].u32SlaveID;
           
          u8SaveConfigFlag |= 1;
        }      
        
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //genAutoMode
    else if(!strcmp(object->name, "genAutoMode"))
    {
      if(value->integer == 1)
      {
        //Set object value
        entry->genAutoMode = 1;
        privateMibBase.genGroup.genflag = index - 1;
        sModbusManager.SettingCommand = SET_GEN_AUTOMODE;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //genManualMode
    else if(!strcmp(object->name, "genManualMode"))
    {
      if(value->integer == 1)
      {
        //Set object value
        entry->genManualMode = 1;
        privateMibBase.genGroup.genflag = index - 1;
        sModbusManager.SettingCommand = SET_GEN_MANUALMODE;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //genStartMode
    else if(!strcmp(object->name, "genStartMode"))
    {
      if(value->integer == 1)
      {
        //Set object value
        entry->genStartMode = 1;
        privateMibBase.genGroup.genflag = index - 1;
        sModbusManager.SettingCommand = SET_GEN_STARTMODE;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //genStopMode
    else if(!strcmp(object->name, "genStopMode"))
    {
      if (value->integer == 1)
      {
        //Set object value
        entry->genStopMode = 1;
        privateMibBase.genGroup.genflag = index - 1;
        sModbusManager.SettingCommand = SET_GEN_STOPMODE;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    
    //Successful processing
    return NO_ERROR;
}
/**
 * @brief Get GenInfoEntry object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t privateMibGetGenInfoEntry(const MibObject *object, const uint8_t *oid,
                                  size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibGenInfoEntry *entry;

    //Point to the instance identifier
    n = object->oidLen;
    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index < 1 || index > 10)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the interface table entry
    entry = &privateMibBase.genGroup.genTable[index - 1];
    //genStatusID object?
    if(!strcmp(object->name, "genStatusID"))
    {
        //Get object value
        value->integer = entry->genStatusID;
    }
    //genStatusStatus object?
    else if(!strcmp(object->name, "genStatusStatus"))
    {
        //Get object value
        value->integer = entry->genStatusStatus;
    }
    //genStatusLNVolt1 object?
    else if(!strcmp(object->name, "genStatusLNVolt1"))
    {
        //Get object value
        value->integer = entry->genStatusLNVolt1;
    }
    //genStatusLNVolt2 object?
    else if(!strcmp(object->name, "genStatusLNVolt2"))
    {
        //Get object value
        value->integer = entry->genStatusLNVolt2;
    }
    //genStatusLNVolt3 object?
    else if(!strcmp(object->name, "genStatusLNVolt3"))
    {
        //Get object value
        value->integer = entry->genStatusLNVolt3;
    }
    //genStatusLNCurr1 object?
    else if(!strcmp(object->name, "genStatusLNCurr1"))
    {
        //Get object value
        value->integer = entry->genStatusLNCurr1;
    }
    //genStatusLNCurr2 object?
    else if(!strcmp(object->name, "genStatusLNCurr2"))
    {
        //Get object value
        value->integer = entry->genStatusLNCurr2;
    }
    //genStatusLNCurr3 object?
    else if(!strcmp(object->name, "genStatusLNCurr3"))
    {
        //Get object value
        value->integer = entry->genStatusLNCurr3;
    }
    //genStatusLNPower1 object?
    else if(!strcmp(object->name, "genStatusLNPower1"))
    {
        //Get object value
        value->integer = entry->genStatusLNPower1;
    }
    //genStatusLNPower2 object?
    else if(!strcmp(object->name, "genStatusLNPower2"))
    {
        //Get object value
        value->integer = entry->genStatusLNPower2;
    }
    //genStatusLNPower3 object?
    else if(!strcmp(object->name, "genStatusLNPower3"))
    {
        //Get object value
        value->integer = entry->genStatusLNPower3;
    }
    //genStatusFrequency object?
    else if(!strcmp(object->name, "genStatusFrequency"))
    {
        //Get object value
        value->integer = entry->genStatusFrequency;
    }
    //genStatusBattVolt object?
    else if(!strcmp(object->name, "genStatusBattVolt"))
    {
        //Get object value
        value->integer = entry->genStatusBattVolt;
    }
    //genStatusCoolantTemp object?
    else if(!strcmp(object->name, "genStatusCoolantTemp"))
    {
        //Get object value
        value->integer = entry->genStatusCoolantTemp;
    }
    //genStatusOilPressure object?
    else if(!strcmp(object->name, "genStatusOilPressure"))
    {
        //Get object value
        value->integer = entry->genStatusOilPressure;
    }
    //genStatusFuelLevel object?
    else if(!strcmp(object->name, "genStatusFuelLevel"))
    {
        //Get object value
        value->integer = entry->genStatusFuelLevel;
    }
    //genStatusRunTime object?
    else if(!strcmp(object->name, "genStatusRunTime"))
    {
        //Get object value
        value->integer = entry->genStatusRunTime;
    }
    //genStatusOilTemp object?
    else if(!strcmp(object->name, "genStatusOilTemp"))
    {
        //Get object value
        value->integer = entry->genStatusOilTemp;
    }
    //genStatusMaintenanceTime object?
    else if(!strcmp(object->name, "genStatusMaintenanceTime"))
    {
        //Get object value
        value->integer = entry->genStatusMaintenanceTime;
    }
    //genStatusEngineSpeed object?
    else if(!strcmp(object->name, "genStatusEngineSpeed"))
    {
        //Get object value
        value->integer = entry->genStatusEngineSpeed;
    }
    //genStatusWorkingHour object?
    else if(!strcmp(object->name, "genStatusWorkingHour"))
    {
        //Get object value
        value->integer = entry->genStatusWorkingHour;
    }
    //genStatusWorkingMin object?
    else if(!strcmp(object->name, "genStatusWorkingMin"))
    {
        //Get object value
        value->integer = entry->genStatusWorkingMin;
    }
    //genStatusActiveEnergyLow object?
    else if(!strcmp(object->name, "genStatusActiveEnergyLow"))
    {
        //Get object value
        value->integer = entry->genStatusActiveEnergyLow;
    }
    //genStatusActiveEnergyHigh object?
    else if(!strcmp(object->name, "genStatusActiveEnergyHigh"))
    {
        //Get object value
        value->integer = entry->genStatusActiveEnergyHigh;
    }
    //genStatusAlarmStatus object?
    else if(!strcmp(object->name, "genStatusAlarmStatus"))
    {
        //Get object value
        value->counter32 = entry->genStatusAlarmStatus.all;
    } 
    //genStatusType object?
    else if(!strcmp(object->name, "genStatusType"))
    {
        //Get object value
        value->integer = entry->genStatusType;
    }   
    //genStatusSerial object?
    else if(!strcmp(object->name, "genStatusSerial"))
    {   
      //Make sure the buffer is large enough to hold the entire object
      if(*valueLen >= privateMibBase.genGroup.genTable[0].genSerialLen)
      {
          //Copy object value
          memcpy(value->octetString, privateMibBase.genGroup.genTable[0].genSerial, privateMibBase.genGroup.genTable[0].genSerialLen);
          //Return object length
          *valueLen = privateMibBase.genGroup.genTable[0].genSerialLen;
      }
      else
      {
          //Report an error
          error = ERROR_BUFFER_OVERFLOW;
      }
    }   
    //genStatusModbusID object?
    else if(!strcmp(object->name, "genStatusModbusID"))
    {
        //Get object value
        value->integer = entry->genStatusModbusID;
    }  
    //genStatusLNSPower1 object?
    else if(!strcmp(object->name, "genStatusLNSPower1"))
    {
        //Get object value
        value->integer = entry->genStatusLNSPower1;
    }  
    //genStatusLNSPower2 object?
    else if(!strcmp(object->name, "genStatusLNSPower2"))
    {
        //Get object value
        value->integer = entry->genStatusLNSPower2;
    }  
    //genStatusLNSPower3 object?
    else if(!strcmp(object->name, "genStatusLNSPower3"))
    {
        //Get object value
        value->integer = entry->genStatusLNSPower3;
    }  
    //genAutoMode object?
    else if(!strcmp(object->name, "genAutoMode"))
    {
        //Get object value
        value->integer = entry->genAutoMode;
    } 
    //genManualMode object?
    else if(!strcmp(object->name, "genManualMode"))
    {
        //Get object value
        value->integer = entry->genManualMode;
    }  
    //genStartMode object?
    else if(!strcmp(object->name, "genStartMode"))
    {
        //Get object value
        value->integer = entry->genStartMode;
    }  
    //genStopMode object?
    else if(!strcmp(object->name, "genStopMode"))
    {
        //Get object value
        value->integer = entry->genStopMode;
    }  
    //Return status code
    return error;
}


/**
 * @brief Get next GenInfoEntry object
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] nextOid OID of the next object in the MIB
 * @param[out] nextOidLen Length of the next object identifier, in bytes
 * @return Error code
 **/
error_t privateMibGetNextGenInfoEntry(const MibObject *object, const uint8_t *oid,
                                      size_t oidLen, uint8_t *nextOid, size_t *nextOidLen)
{
    error_t error;
    size_t n;
    uint_t index;

    //Make sure the buffer is large enough to hold the OID prefix
    if(*nextOidLen < object->oidLen)
        return ERROR_BUFFER_OVERFLOW;

    //Copy OID prefix
    memcpy(nextOid, object->oid, object->oidLen);

    //Loop through network interfaces
    for(index = 1; index <= privateMibBase.genGroup.genInstalledGen; index++)
    {
        //Append the instance identifier to the OID prefix
        n = object->oidLen;

        //The ifIndex is used as instance identifier
        error = mibEncodeIndex(nextOid, *nextOidLen, &n, index);
        //Any error to report?
        if(error) return error;

        //Check whether the resulting object identifier lexicographically
        //follows the specified OID
        if(oidComp(nextOid, n, oid, oidLen) > 0)
        {
            //Save the length of the resulting object identifier
            *nextOidLen = n;
            //Next object found
            return NO_ERROR;
        }
    }

    //The specified OID does not lexicographically precede the name
    //of some object
    return ERROR_OBJECT_NOT_FOUND;
}
//========================================== GenInfo Function ==========================================//

//========================================== PMInfo Function ==========================================//
error_t privateMibSetPMGroup(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibPMGroup *entry;
    
    //	Point to the liBattGroup entry
    entry = &privateMibBase.pmGroup;
    
    if(!strcmp(object->name, "pmInstalledPM"))
    {
      if((value->integer >= 0) && (value->integer <= 2))
      {
          //Get object value
          entry->pmInstalledPM = value->integer; 
          if (entry->pmInstalledPM != sModbusManager.u8NumberOfPM)
          {
              sModbusManager.u8NumberOfPM = entry->pmInstalledPM;
              g_sParameters.u8DevNum[TOTAL_PM_INDEX] = sModbusManager.u8NumberOfPM;
              g_sWorkingDefaultParameters.u8DevNum[TOTAL_PM_INDEX] = g_sParameters.u8DevNum[TOTAL_PM_INDEX];
              u8SaveConfigFlag |= 1;
          }
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    else
    {
        TRACE_ERROR("ERROR_OBJECT_NOT_FOUND!\r\n");
        //The specified object does not exist
        return ERROR_OBJECT_NOT_FOUND;
    }
    
    //Successful processing
    return NO_ERROR; 
}

error_t privateMibSetPMInfoEntry(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    uint_t j;
    tModbusParameters sModbusDev[3];
    PrivateMibPMInfoEntry *entry;

    //Point to the instance identifier
    n = object->oidLen;
    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index < 1 || index > 2)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the libTable entry
    entry = &privateMibBase.pmGroup.pmTable[index - 1];
    //pmType object?
    if(!strcmp(object->name, "pmType"))
    {
      if((value->integer >= 0) && (value->integer <= 18))
      {
        //Set object value
        entry->pmType= value->integer;
        j = index;
        sModbusManager.sPMManager[j-1].u8PMType = entry->pmType;        
        switch(sModbusManager.sPMManager[j-1].u8PMType)
        {
        case 1:// FINECO
        {
            sModbusManager.sPMManager[j-1].u32PMSlaveOffset = 16;
            sModbusManager.sPMManager[j-1].u32PMAbsSlaveID = 16+j;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[0] = 16;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[1] = 50;
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[0] = 0x700;    
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[1] = 2; 
        }
        break;
        
        case 2:// ASCENT
        {    
            sModbusManager.sPMManager[j-1].u32PMSlaveOffset = 16;
            sModbusManager.sPMManager[j-1].u32PMAbsSlaveID = 16+j;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[0] = 0;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[1] = 100;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[2] = 106;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[3] = 118;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[4] = 142;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[5] = 144;
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[0] = 2;    
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[1] = 2;    
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[2] = 2;   
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[3] = 2;   
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[4] = 2;   
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[5] = 2;                                 
        }
        break;
        
        case 3:// EASTRON
        {  

            sModbusManager.sPMManager[j-1].u32PMSlaveOffset = 16;
            sModbusManager.sPMManager[j-1].u32PMAbsSlaveID = 16+j;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[0] = 0;
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[0] = 32;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[1] = 70;//0x71
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[1] = 10;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[2] = 342;//0x156
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[2] = 4;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[3] = 64512;//0x156
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[3] = 2;
        }
        break;
        
        case 4:// CET1
        {  
            sModbusManager.sPMManager[j-1].u32PMSlaveOffset = 52;
            sModbusManager.sPMManager[j-1].u32PMAbsSlaveID = 52+j;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[0] = 0;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[1] = 40;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[2] = 9800;
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[0] = 14;    
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[1] = 18;    
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[2] = 27;                                  
        }
        break;
        
        case 5:// PILOT
        {  
            sModbusManager.sPMManager[j-1].u32PMSlaveOffset = 16;  
            sModbusManager.sPMManager[j-1].u32PMAbsSlaveID = 16+j;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[0] = 0;
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[0] = 17;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[1] = 19;
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[1] = 6;                                  
        }
        break;
        case 15:// PILOT_3PHASE
        {  
            sModbusManager.sPMManager[j-1].u32PMSlaveOffset = 16;  
            sModbusManager.sPMManager[j-1].u32PMAbsSlaveID = 16+j;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[0] = 100;
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[0] = 43;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[1] = 1000;
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[1] = 12;     
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[2] = 9000;
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[2] = 2;                                
        }
        break;
        case 16://YADA_3PHASE_DPC
        {
            sModbusManager.sPMManager[j-1].u32PMSlaveOffset  = 16;
            sModbusManager.sPMManager[j-1].u32PMAbsSlaveID   = 16+j;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[0]  = 4240;//0x03 U
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[0]   = 6;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[1]  = 4288;//0x03 I
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[1]   = 6;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[2]  = 4496;//0x03 cosphi
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[2]   = 4;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[3]  = 4528;//0x03 HZ
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[3]   = 1;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[4]  = 4192;//0x03 kvarh
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[4]   = 2;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[5]  = 4144;//0x03 kwh
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[5]   = 2;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[6]  = 4352;//0x03 P
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[6]   = 8;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[0]  = 4416;//0x03 Q
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[0]   = 8;
        }
        break;
         case 17://Schneider 2022
         {
            sModbusManager.sPMManager[j-1].u32PMSlaveOffset  = 16;
            sModbusManager.sPMManager[j-1].u32PMAbsSlaveID   = 16+j;
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[0]  = 29; //Model
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[0]   = 20; 
            
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[1]  = 128; //serial
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[1]   = 3;  
            
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[2]  = 2699; 
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[2]   = 24;
            
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[3]  = 2999; //I
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[3]   = 6; 
            
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[4]  = 3027; //V
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[4]   = 6;   
            
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[5]  = 3053; //KW, kVAR
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[5]   = 24;   
         
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[6]  = 3077; // Factor
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[6]   = 8;
            
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[7]  = 3109; // F
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[7]   = 2;
          }
           break; 
        case 18: //EASTRON SMD72D 2022
          {
            sModbusManager.sPMManager[j-1].u32PMSlaveOffset  = 16;
            sModbusManager.sPMManager[j-1].u32PMAbsSlaveID   = 16+j;
                    
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[0]  = 0;
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[0]   = 30;
                    
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[1]  = 30;
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[1]   = 30;                    
                    
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[2]  = 60;
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[2]   = 30;  
            
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[3]  = 342;
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[3]   = 30;   
            
            sModbusManager.sPMManager[j-1].u32PMBaseAddr[4]  = 11;              //Fn03
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[4]   = 30;  

            sModbusManager.sPMManager[j-1].u32PMBaseAddr[5]  = 64512;           //Fn03
            sModbusManager.sPMManager[j-1].u8PMNumOfReg[5]   = 4;              
          }
          break;          
        };
        sModbusDev[j-1].u8DevVendor = sModbusManager.sPMManager[j-1].u8PMType;
        sModbusDev[j-1].u32SlaveID = sModbusManager.sPMManager[j-1].u32PMAbsSlaveID;  
        //
        // Did parameters change?
        //
        if  (g_sParameters.sModbusParameters[j-1+ MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM].u8DevVendor != sModbusDev[j-1].u8DevVendor)
        {
          //
          // Update the current parameters with the new settings.
          //
          g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM].u8DevVendor = sModbusDev[j-1].u8DevVendor;
          g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM].u32SlaveID = sModbusDev[j-1].u32SlaveID;
          //
          // Yes - save these settings as the defaults.
          //
          g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM].u8DevVendor = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM].u8DevVendor;
          g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM].u32SlaveID = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM].u32SlaveID;
           
          u8SaveConfigFlag |= 1;
        }      
        
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    // pmDeltaFreqDisConnect
    else if(!strcmp(object->name, "pmDeltaFreqDisConnect"))
    {
      if((value->integer >= 1) && (value->integer <= 10) && (value->integer > entry->pmDeltaFreqReConnect ))
      {
        //Set object value
        entry->pmDeltaFreqDisConnect= value->integer;
        g_sParameters.u8DeltaFreqDisConnect[index - 1] = (uint8_t)value->integer;
        g_sWorkingDefaultParameters.u8DeltaFreqDisConnect [index - 1] = g_sParameters.u8DeltaFreqDisConnect[index - 1];
        u8SaveConfigFlag |= 1;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    // pmDeltaFreqReConnect
    else if(!strcmp(object->name, "pmDeltaFreqReConnect"))
    {
      if((value->integer >= 1) && (value->integer <= 10) && (value->integer < entry->pmDeltaFreqDisConnect ))
      {
        //Set object value
        entry->pmDeltaFreqReConnect= value->integer;
        g_sParameters.u8DeltaFreqReConnect[index - 1] = (uint8_t)value->integer;
        g_sWorkingDefaultParameters.u8DeltaFreqReConnect [index - 1] = g_sParameters.u8DeltaFreqReConnect[index - 1];
        u8SaveConfigFlag |= 1;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    // pmEnableFreqTrap
    else if(!strcmp(object->name, "pmEnableFreqTrap"))
    {
      if((value->integer == 0) || (value->integer == 1))
      {
        //Set object value
        privateMibBase.pmGroup.pmTable[0].pmEnableFreqTrap = value->integer;
        privateMibBase.pmGroup.pmTable[1].pmEnableFreqTrap = value->integer;
        g_sParameters.u8EnableFreqTrap = (uint8_t)value->integer;
        g_sWorkingDefaultParameters.u8EnableFreqTrap = g_sParameters.u8EnableFreqTrap;
        u8SaveConfigFlag |= 1;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    // pm_delta_freq_v
    else if(!strcmp(object->name, "pm_delta_freq_v"))
    {
      if((value->integer >= 20) && (value->integer < 500))
      {
        //Set object value
        entry->pm_delta_freq_v= value->integer;
        g_sParameters.pm_delta_freq_v = (uint16_t)value->integer;
        g_sWorkingDefaultParameters.pm_delta_freq_v = g_sParameters.pm_delta_freq_v;
        u8SaveConfigFlag |= 1;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    // pm_cnt_timeout_freq_out
    else if(!strcmp(object->name, "pm_cnt_timeout_freq_out"))
    {
      if((value->integer >= 0) && (value->integer <= 255))
      {
        //Set object value
        entry->pm_cnt_timeout_freq_out= value->integer;
        g_sParameters.pm_cnt_timeout_freq_out = (uint16_t)value->integer;
        g_sWorkingDefaultParameters.pm_cnt_timeout_freq_out = g_sParameters.pm_cnt_timeout_freq_out;
        u8SaveConfigFlag |= 1;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    // pm_cnt_timeout_freq_in
    else if(!strcmp(object->name, "pm_cnt_timeout_freq_in"))
    {
      if((value->integer >= 0) && (value->integer < 255))
      {
        //Set object value
        entry->pm_cnt_timeout_freq_in= value->integer;
        g_sParameters.pm_cnt_timeout_freq_in = (uint16_t)value->integer;
        g_sWorkingDefaultParameters.pm_cnt_timeout_freq_in = g_sParameters.pm_cnt_timeout_freq_in;
        u8SaveConfigFlag |= 1;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    // pm_set_cnt_remaining_gen
    else if(!strcmp(object->name, "pm_set_cnt_remaining_gen"))
    {
      if((value->integer >= 0) && (value->integer < 65535))
      {
        //Set object value
        entry->pm_set_cnt_remaining_gen= value->integer;
        g_sParameters.pm_set_cnt_remaining_gen = (uint16_t)value->integer;
        g_sWorkingDefaultParameters.pm_set_cnt_remaining_gen = g_sParameters.pm_set_cnt_remaining_gen;
        u8SaveConfigFlag |= 1;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //Successful processing
    return NO_ERROR;

}
/**
 * @brief Get PMInfoEntry object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t privateMibGetPMInfoEntry(const MibObject *object, const uint8_t *oid,
                                  size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibPMInfoEntry *entry;

    //Point to the instance identifier
    n = object->oidLen;
    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index < 1 || index > 10)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the interface table entry
    entry = &privateMibBase.pmGroup.pmTable[index - 1];
    //pmID object?
    if(!strcmp(object->name, "pmID"))
    {
        //Get object value
        value->integer = entry->pmID;
    }
    //pmStatus object?
    else if(!strcmp(object->name, "pmStatus"))
    {
        //Get object value
        value->integer = entry->pmStatus;
    }
    //pmImportActiveEnergy object?
    else if(!strcmp(object->name, "pmImportActiveEnergy"))
    {
        //Get object value
        value->integer = entry->pmImportActiveEnergy;
    }
    //pmExportActiveEnergy object?
    else if(!strcmp(object->name, "pmExportActiveEnergy"))
    {
        //Get object value
        value->integer = entry->pmExportActiveEnergy;
    }
    //pmImportReactiveEnergy object?
    else if(!strcmp(object->name, "pmImportReactiveEnergy"))
    {
        //Get object value
        value->integer = entry->pmImportReactiveEnergy;
    }
    //pmExportReactiveEnergy object?
    else if(!strcmp(object->name, "pmExportReactiveEnergy"))
    {
        //Get object value
        value->integer = entry->pmExportReactiveEnergy;
    }
    //pmTotalActiveEnergy object?
    else if(!strcmp(object->name, "pmTotalActiveEnergy"))
    {
        //Get object value
        value->integer = entry->pmTotalActiveEnergy;
    }
    //pmTotalReactiveEnergy object?
    else if(!strcmp(object->name, "pmTotalReactiveEnergy"))
    {
        //Get object value
        value->integer = entry->pmTotalReactiveEnergy;
    }
    //pmActivePower object?
    else if(!strcmp(object->name, "pmActivePower"))
    {
        //Get object value
        value->integer = entry->pmActivePower;
    }
    //pmReactivePower object?
    else if(!strcmp(object->name, "pmReactivePower"))
    {
        //Get object value
        value->integer = entry->pmReactivePower;
    }
    //pmPowerFactor object?
    else if(!strcmp(object->name, "pmPowerFactor"))
    {
        //Get object value
        value->integer = entry->pmPowerFactor;
    }
    //pmFrequency object?
    else if(!strcmp(object->name, "pmFrequency"))
    {
        //Get object value
        value->integer = entry->pmFrequency;
    }
    //pmPhase1Current object?
    else if(!strcmp(object->name, "pmPhase1Current"))
    {
        //Get object value
        value->integer = entry->pmPhase1Current;
    }
    //pmPhase1Voltage object?
    else if(!strcmp(object->name, "pmPhase1Voltage"))
    {
        //Get object value
        value->integer = entry->pmPhase1Voltage;
    }
    //pmPhase1RealPower object?
    else if(!strcmp(object->name, "pmPhase1RealPower"))
    {
        //Get object value
        value->integer = entry->pmPhase1RealPower;
    }
    //pmPhase1ApparentPower object?
    else if(!strcmp(object->name, "pmPhase1ApparentPower"))
    {
        //Get object value
        value->integer = entry->pmPhase1ApparentPower;
    }
    //pmPhase1ReactivePower object?
    else if(!strcmp(object->name, "pmPhase1ReactivePower"))
    {
        //Get object value
        value->integer = entry->pmPhase1ReactivePower;
    }
    //pmPhase1PowerFactor object?
    else if(!strcmp(object->name, "pmPhase1PowerFactor"))
    {
        //Get object value
        value->integer = entry->pmPhase1PowerFactor;
    }
    //pmPhase2Current object?
    else if(!strcmp(object->name, "pmPhase2Current"))
    {
        //Get object value
        value->integer = entry->pmPhase2Current;
    }
    //pmPhase2Voltage object?
    else if(!strcmp(object->name, "pmPhase2Voltage"))
    {
        //Get object value
        value->integer = entry->pmPhase2Voltage;
    }
    //pmPhase2RealPower object?
    else if(!strcmp(object->name, "pmPhase2RealPower"))
    {
        //Get object value
        value->integer = entry->pmPhase2RealPower;
    }
    //pmPhase2ApparentPower object?
    else if(!strcmp(object->name, "pmPhase2ApparentPower"))
    {
        //Get object value
        value->integer = entry->pmPhase2ApparentPower;
    }
    //pmPhase2ReactivePower object?
    else if(!strcmp(object->name, "pmPhase2ReactivePower"))
    {
        //Get object value
        value->integer = entry->pmPhase2ReactivePower;
    }
    //pmPhase2PowerFactor object?
    else if(!strcmp(object->name, "pmPhase2PowerFactor"))
    {
        //Get object value
        value->integer = entry->pmPhase2PowerFactor;
    }
    //pmPhase3Current object?
    else if(!strcmp(object->name, "pmPhase3Current"))
    {
        //Get object value
        value->integer = entry->pmPhase3Current;
    }
    //pmPhase3Voltage object?
    else if(!strcmp(object->name, "pmPhase3Voltage"))
    {
        //Get object value
        value->integer = entry->pmPhase3Voltage;
    }
    //pmPhase3RealPower object?
    else if(!strcmp(object->name, "pmPhase3RealPower"))
    {
        //Get object value
        value->integer = entry->pmPhase3RealPower;
    }
    //pmPhase3ApparentPower object?
    else if(!strcmp(object->name, "pmPhase3ApparentPower"))
    {
        //Get object value
        value->integer = entry->pmPhase3ApparentPower;
    }
    //pmPhase3ReactivePower object?
    else if(!strcmp(object->name, "pmPhase3ReactivePower"))
    {
        //Get object value
        value->integer = entry->pmPhase3ReactivePower;
    }
    //pmPhase3PowerFactor object?
    else if(!strcmp(object->name, "pmPhase3PowerFactor"))
    {
        //Get object value
        value->integer = entry->pmPhase3PowerFactor;
    }
    //pmType object?
    else if(!strcmp(object->name, "pmType"))
    {
        //Get object value
        value->integer = entry->pmType;
    }
    //pmSerial object?
    else if(!strcmp(object->name, "pmSerial"))
    {
        //Make sure the buffer is large enough to hold the entire object
        if(*valueLen >= entry->pmSerialLen)
        {
            //Copy object value
            memcpy(value->octetString, entry->pmSerial, entry->pmSerialLen);
            //Return object length
            *valueLen = entry->pmSerialLen;
        }
        else
        {
            //Report an error
            error = ERROR_BUFFER_OVERFLOW;
        }
    }
    //pmModbusID object?
    else if(!strcmp(object->name, "pmModbusID"))
    {
        //Get object value
        value->integer = entry->pmModbusID;
    }
    //pmModel object?
    else if(!strcmp(object->name, "pmModel"))
    {
        //Make sure the buffer is large enough to hold the entire object
        if(*valueLen >= entry->pmModelLen)
        {
            //Copy object value
            memcpy(value->octetString, entry->pmModel, entry->pmModelLen);
            //Return object length
            *valueLen = entry->pmModelLen;
        }
        else
        {
            //Report an error
            error = ERROR_BUFFER_OVERFLOW;
        }
    }
    //pmOutOfRangeFreq object?
    else if(!strcmp(object->name, "pmOutOfRangeFreq"))
    {
        //Get object value
        value->integer = entry->pmOutOfRangeFreq;
    }
    //pmDeltaFreqDisConnect object?
    else if(!strcmp(object->name, "pmDeltaFreqDisConnect"))
    {
        //Get object value
        value->integer = entry->pmDeltaFreqDisConnect;
    }
    //pmDeltaFreqReConnect object?
    else if(!strcmp(object->name, "pmDeltaFreqReConnect"))
    {
        //Get object value
        value->integer = entry->pmDeltaFreqReConnect;
    }
    //pmEnableFreqTrap object?
    else if(!strcmp(object->name, "pmEnableFreqTrap"))
    {
        //Get object value
        value->integer = entry->pmEnableFreqTrap;
    }
    //pmTotalCurrent object?
    else if(!strcmp(object->name, "pmTotalCurrent"))
    {
        //Get object value
        value->integer = entry->pmTotalCurrent;
    }
    //pm_delta_freq_v object?
    else if(!strcmp(object->name, "pm_delta_freq_v"))
    {
        //Get object value
        value->integer = entry->pm_delta_freq_v;
    }
    //pm_cnt_timeout_freq_out object?
    else if(!strcmp(object->name, "pm_cnt_timeout_freq_out"))
    {
        //Get object value
        value->integer = entry->pm_cnt_timeout_freq_out;
    }
    //pm_cnt_timeout_freq_in object?
    else if(!strcmp(object->name, "pm_cnt_timeout_freq_in"))
    {
        //Get object value
        value->integer = entry->pm_cnt_timeout_freq_in;
    }
    //pm_remaining_gen object?
    else if(!strcmp(object->name, "pm_remaining_gen"))
    {
        //Get object value
        value->integer = entry->pm_remaining_gen;
    }
    
    //pm_set_cnt_remaining_gen object?
    else if(!strcmp(object->name, "pm_set_cnt_remaining_gen"))
    {
        //Get object value
        value->integer = entry->pm_set_cnt_remaining_gen;
    }

    //Return status code
    return error;
}


/**
 * @brief Get next PMInfoEntry object
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] nextOid OID of the next object in the MIB
 * @param[out] nextOidLen Length of the next object identifier, in bytes
 * @return Error code
 **/
error_t privateMibGetNextPMInfoEntry(const MibObject *object, const uint8_t *oid,
                                      size_t oidLen, uint8_t *nextOid, size_t *nextOidLen)
{
    error_t error;
    size_t n;
    uint_t index;

    //Make sure the buffer is large enough to hold the OID prefix
    if(*nextOidLen < object->oidLen)
        return ERROR_BUFFER_OVERFLOW;

    //Copy OID prefix
    memcpy(nextOid, object->oid, object->oidLen);

    //Loop through network interfaces
    for(index = 1; index <= privateMibBase.pmGroup.pmInstalledPM; index++)
    {
        //Append the instance identifier to the OID prefix
        n = object->oidLen;

        //The ifIndex is used as instance identifier
        error = mibEncodeIndex(nextOid, *nextOidLen, &n, index);
        //Any error to report?
        if(error) return error;

        //Check whether the resulting object identifier lexicographically
        //follows the specified OID
        if(oidComp(nextOid, n, oid, oidLen) > 0)
        {
            //Save the length of the resulting object identifier
            *nextOidLen = n;
            //Next object found
            return NO_ERROR;
        }
    }

    //The specified OID does not lexicographically precede the name
    //of some object
    return ERROR_OBJECT_NOT_FOUND;
}
//========================================== PMInfo Function ==========================================//

//========================================== SMCBInfo Function ==========================================//
error_t privateMibSetSMCBGroup(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibSMCBInfoGroup *entry;
    
    //	Point to the liBattGroup entry
    entry = &privateMibBase.smcbGroup;
    
    if(!strcmp(object->name, "SmcbInstalledSMCB"))
    {
      if((value->integer >= 0) && (value->integer <= 5))
      {
          //Get object value
          entry->SmcbInstalledSMCB = value->integer; 
          if (entry->SmcbInstalledSMCB != sModbusManager.u8NumberOfSMCB)
          {
              sModbusManager.u8NumberOfSMCB = entry->SmcbInstalledSMCB;
              g_sParameters.u8DevNum[TOTAL_SMCB_INDEX] = sModbusManager.u8NumberOfSMCB;
              g_sWorkingDefaultParameters.u8DevNum[TOTAL_SMCB_INDEX] = g_sParameters.u8DevNum[TOTAL_SMCB_INDEX];
              u8SaveConfigFlag |= 1;
          }
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    else
    {
        TRACE_ERROR("ERROR_OBJECT_NOT_FOUND!\r\n");
        //The specified object does not exist
        return ERROR_OBJECT_NOT_FOUND;
    }
    
    //Successful processing
    return NO_ERROR;   
}

/**
 * @brief Set SMCBGroup object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[in] value Object value
 * @param[in] valueLen Length of the object value, in bytes
 * @return Error code
 **/
error_t privateMibSetSmcbInfoEntry(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    uint_t j;
    PrivateMibSMCBInfoEntry *entry;
    tModbusParameters sModbusDev[6];

    //Point to the instance identifier
    n = object->oidLen;

    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index < 1 || index > 5)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the smcbTable entry
    entry = &privateMibBase.smcbGroup.SmcbTable[index - 1]; //note
    //SmcbState object?
    if(!strcmp(object->name, "SmcbState"))
    {
      if((value->integer >= 0) && (value->integer <= 1))
      {
        //Get object value
        entry->SmcbState= value->integer;
        entry->SmcbStateWrite= value->integer;
        privateMibBase.smcbGroup.flag = index - 1;
        sSMCBInfo[index - 1].u32State = value->integer;
        sModbusManager.SettingCommand = SET_SMCB_STATE;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //SmcbType object?
    else if (!strcmp(object->name, "SmcbType"))
    {
      //Set object value
      entry->SmcbType= value->integer;
      j = index;
      sModbusManager.sSMCBManager[j-1].u8SMCBType = entry->SmcbType;
      
      switch(sModbusManager.sSMCBManager[j-1].u8SMCBType)
      {
      case 1:// OPEN
      {
          sModbusManager.sSMCBManager[j-1].u32SMCBSlaveOffset = 18;
          sModbusManager.sSMCBManager[j-1].u32SMCBAbsSlaveID = 18+j;
          sModbusManager.sSMCBManager[j-1].u32SMCBBaseAddr[0] = 0;
          sModbusManager.sSMCBManager[j-1].u8SMCBNumOfReg[0] = 1;    
      }
      break;
      
      case 2:// MATIS
      { 
          sModbusManager.sSMCBManager[j-1].u32SMCBSlaveOffset = 18;
          sModbusManager.sSMCBManager[j-1].u32SMCBAbsSlaveID = 18+j;
          sModbusManager.sSMCBManager[j-1].u32SMCBBaseAddr[0] = 16;
          sModbusManager.sSMCBManager[j-1].u8SMCBNumOfReg[0] = 1;                                       
      }
      break;
      case 3:// GOL
      {
          sModbusManager.sSMCBManager[j-1].u32SMCBSlaveOffset = 18;
          sModbusManager.sSMCBManager[j-1].u32SMCBAbsSlaveID = 18+j;
          sModbusManager.sSMCBManager[j-1].u32SMCBBaseAddr[0] = 32768;
          sModbusManager.sSMCBManager[j-1].u8SMCBNumOfReg[0] = 1;  
      }
      break;   
      };
      sModbusDev[j-1].u8DevVendor = sModbusManager.sSMCBManager[j-1].u8SMCBType;
      sModbusDev[j-1].u32SlaveID = sModbusManager.sSMCBManager[j-1].u32SMCBAbsSlaveID;  
      //
      // Did parameters change?
      // luu vendor vao flash 
      if  (g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC].u8DevVendor != sModbusDev[j-1].u8DevVendor)
      {
        //
        // Update the current parameters with the new settings.
        //
        g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC].u8DevVendor = sModbusDev[j-1].u8DevVendor;
        g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC].u32SlaveID = sModbusDev[j-1].u32SlaveID;
        //
        // Yes - save these settings as the defaults.
        //
        g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC].u8DevVendor = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC].u8DevVendor;
        g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC].u32SlaveID = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC].u32SlaveID;
         
        u8SaveConfigFlag |= 1;      }      
      
    }
    else
    {
        TRACE_ERROR("ERROR_OBJECT_NOT_FOUND!\r\n");
        //The specified object does not exist
        return ERROR_OBJECT_NOT_FOUND;
    }
    //Successful processing
    return NO_ERROR;
}

/**
 * @brief Get SMCBInfoEntry object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t privateMibGetSmcbInfoEntry(const MibObject *object, const uint8_t *oid,
                                  size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibSMCBInfoEntry *entry;

    //Point to the instance identifier
    n = object->oidLen;

    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index < 1 || index > 10)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the interface table entry
    entry = &privateMibBase.smcbGroup.SmcbTable[index - 1];
    //SmcbStatusID object?
    if(!strcmp(object->name, "SmcbStatusID"))
    {
        //Get object value
        value->integer = entry->SmcbStatusID;
    }
    //SmcbStatus object?
    else if(!strcmp(object->name, "SmcbStatus"))
    {
        //Get object value
        value->integer = entry->SmcbStatus;
    }
    //SmcbState object?
    else if(!strcmp(object->name, "SmcbState"))
    {
        //Get object value
        value->integer = entry->SmcbState;
    }
    //SmcbType object?
    else if(!strcmp(object->name, "SmcbType"))
    {
        //Get object value
        value->integer = entry->SmcbType;
    }
    //SmcbModbusID object?
    else if(!strcmp(object->name, "SmcbModbusID"))
    {
        //Get object value
        value->integer = entry->SmcbModbusID;
    }
      

    //Return status code
    return error;
}


/**
 * @brief Get next SMBCInfoEntry object
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] nextOid OID of the next object in the MIB
 * @param[out] nextOidLen Length of the next object identifier, in bytes
 * @return Error code
 **/
error_t privateMibGetNextSmcbInfoEntry(const MibObject *object, const uint8_t *oid,
                                      size_t oidLen, uint8_t *nextOid, size_t *nextOidLen)
{
    error_t error;
    size_t n;
    uint_t index;

    //Make sure the buffer is large enough to hold the OID prefix
    if(*nextOidLen < object->oidLen)
        return ERROR_BUFFER_OVERFLOW;

    //Copy OID prefix
    memcpy(nextOid, object->oid, object->oidLen);

    //Loop through network interfaces
    for(index = 1; index <= privateMibBase.smcbGroup.SmcbInstalledSMCB; index++)
    {
        //Append the instance identifier to the OID prefix
        n = object->oidLen;

        //The ifIndex is used as instance identifier
        error = mibEncodeIndex(nextOid, *nextOidLen, &n, index);
        //Any error to report?
        if(error) return error;

        //Check whether the resulting object identifier lexicographically
        //follows the specified OID
        if(oidComp(nextOid, n, oid, oidLen) > 0)
        {
            //Save the length of the resulting object identifier
            *nextOidLen = n;
            //Next object found
            return NO_ERROR;
        }
    }

    //The specified OID does not lexicographically precede the name
    //of some object
    return ERROR_OBJECT_NOT_FOUND;
}


//========================================== SMCBInfo Function ==========================================//

//========================================== FUELInfo Function ==========================================//
error_t privateMibSetFUELGroup(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibFUELInfoGroup *entry;
    
    //	Point to the fuelGroup entry
    entry = &privateMibBase.fuelGroup;
    
    if(!strcmp(object->name, "FuelInstalledFUEL"))
    {
      if((value->integer >= 0) && (value->integer <= 2))
      {
          //Get object value
          entry->FuelInstalledFUEL = value->integer; 
          if (entry->FuelInstalledFUEL != sModbusManager.u8NumberOfFUEL)
          {
              sModbusManager.u8NumberOfFUEL = entry->FuelInstalledFUEL;
              g_sParameters.u8DevNum[TOTAL_FUEL_INDEX] = sModbusManager.u8NumberOfFUEL;
              g_sWorkingDefaultParameters.u8DevNum[TOTAL_FUEL_INDEX] = g_sParameters.u8DevNum[TOTAL_FUEL_INDEX];
              u8SaveConfigFlag |= 1;
          }
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    else
    {
        TRACE_ERROR("ERROR_OBJECT_NOT_FOUND!\r\n");
        //The specified object does not exist
        return ERROR_OBJECT_NOT_FOUND;
    }
    
    //Successful processing
    return NO_ERROR;   
}

/**
 * @brief Set FUELGroup object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[in] value Object value
 * @param[in] valueLen Length of the object value, in bytes
 * @return Error code
 **/
error_t privateMibSetFuelInfoEntry(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    uint_t j;
    PrivateMibFUELInfoEntry *entry;
    tModbusParameters sModbusDev[6];

    //Point to the instance identifier
    n = object->oidLen;

    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index < 1 || index > 2)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the FuelTable entry
    entry = &privateMibBase.fuelGroup.FuelTable[index - 1];
    //FuelType object?
    if (!strcmp(object->name, "FuelType"))
    {
      //Set object value
      entry->FuelType= value->integer;
      j = index;
      sModbusManager.sFUELManager[j-1].u8FUELType = entry->FuelType;
      
      switch(sModbusManager.sFUELManager[j-1].u8FUELType)
      {
      case 1:// HPT621
      {
          sModbusManager.sFUELManager[j-1].u32FUELSlaveOffset = 23;
          sModbusManager.sFUELManager[j-1].u32FUELAbsSlaveID = 23+j;
          sModbusManager.sFUELManager[j-1].u32FUELBaseAddr[0] = 0;
          sModbusManager.sFUELManager[j-1].u8FUELNumOfReg[0] = 1;     
      }
      break;
      
      case 2:// 
      {                                     
      }
      break;
      };
      sModbusDev[j-1].u8DevVendor = sModbusManager.sFUELManager[j-1].u8FUELType;
      sModbusDev[j-1].u32SlaveID = sModbusManager.sFUELManager[j-1].u32FUELAbsSlaveID;  
      //
      // Did parameters change?
      // luu vendor vao flash 
      if  (g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB].u8DevVendor != sModbusDev[j-1].u8DevVendor)
      {
        //
        // Update the current parameters with the new settings.
        //
        g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB].u8DevVendor = sModbusDev[j-1].u8DevVendor;
        g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB].u32SlaveID = sModbusDev[j-1].u32SlaveID;
        //
        // Yes - save these settings as the defaults.
        //
        g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB].u8DevVendor = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB].u8DevVendor;
        g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB].u32SlaveID = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB].u32SlaveID;
         
        u8SaveConfigFlag |= 1;      }      
      
    }
    else
    {
        TRACE_ERROR("ERROR_OBJECT_NOT_FOUND!\r\n");
        //The specified object does not exist
        return ERROR_OBJECT_NOT_FOUND;
    }
    //Successful processing
    return NO_ERROR;
}

/**
 * @brief Get FUELInfoEntry object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t privateMibGetFuelInfoEntry(const MibObject *object, const uint8_t *oid,
                                  size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibFUELInfoEntry *entry;

    //Point to the instance identifier
    n = object->oidLen;

    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index < 1 || index > 10)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the interface table entry
    entry = &privateMibBase.fuelGroup.FuelTable[index - 1];
    //FuelStatusID object?
    if(!strcmp(object->name, "FuelStatusID"))
    {
        //Get object value
        value->integer = entry->FuelStatusID;
    }
    //FuelStatus object?
    else if(!strcmp(object->name, "FuelStatus"))
    {
        //Get object value
        value->integer = entry->FuelStatus;
    }
    //FuelType object?
    else if(!strcmp(object->name, "FuelType"))
    {
        //Get object value
        value->integer = entry->FuelType;
    }
    //FuelLevel object?
    else if(!strcmp(object->name, "FuelLevel"))
    {
        //Get object value
        value->integer = entry->FuelLevel;
    }
    //FuelModbusID object?
    else if(!strcmp(object->name, "FuelModbusID"))
    {
        //Get object value
        value->integer = entry->FuelModbusID;
    }
      

    //Return status code
    return error;
}


/**
 * @brief Get next FUELInfoEntry object
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] nextOid OID of the next object in the MIB
 * @param[out] nextOidLen Length of the next object identifier, in bytes
 * @return Error code
 **/
error_t privateMibGetNextFuelInfoEntry(const MibObject *object, const uint8_t *oid,
                                      size_t oidLen, uint8_t *nextOid, size_t *nextOidLen)
{
    error_t error;
    size_t n;
    uint_t index;

    //Make sure the buffer is large enough to hold the OID prefix
    if(*nextOidLen < object->oidLen)
        return ERROR_BUFFER_OVERFLOW;

    //Copy OID prefix
    memcpy(nextOid, object->oid, object->oidLen);

    //Loop through network interfaces
    for(index = 1; index <= privateMibBase.fuelGroup.FuelInstalledFUEL; index++)
    {
        //Append the instance identifier to the OID prefix
        n = object->oidLen;

        //The ifIndex is used as instance identifier
        error = mibEncodeIndex(nextOid, *nextOidLen, &n, index);
        //Any error to report?
        if(error) return error;

        //Check whether the resulting object identifier lexicographically
        //follows the specified OID
        if(oidComp(nextOid, n, oid, oidLen) > 0)
        {
            //Save the length of the resulting object identifier
            *nextOidLen = n;
            //Next object found
            return NO_ERROR;
        }
    }

    //The specified OID does not lexicographically precede the name
    //of some object
    return ERROR_OBJECT_NOT_FOUND;
}


//========================================== FUELInfo Function ==========================================//

//========================================== VACInfo Function ==========================================//
error_t privateMibSetVACGroup(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibVACGroup *entry;
    
    //	Point to the liBattGroup entry
    entry = &privateMibBase.vacGroup;
    
    if(!strcmp(object->name, "vacInstalledVAC"))
    {
      if((value->integer >= 0) && (value->integer <= 2))
      {
          //Get object value
          entry->vacInstalledVAC = value->integer; 
          if (entry->vacInstalledVAC != sModbusManager.u8NumberOfVAC)
          {
            sModbusManager.u8NumberOfVAC = entry->vacInstalledVAC;
            g_sParameters.u8DevNum[TOTAL_VAC_INDEX] = sModbusManager.u8NumberOfVAC;
            g_sWorkingDefaultParameters.u8DevNum[TOTAL_VAC_INDEX] = g_sParameters.u8DevNum[TOTAL_VAC_INDEX];
            u8SaveConfigFlag |= 1;
          }
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    else
    {
        TRACE_ERROR("ERROR_OBJECT_NOT_FOUND!\r\n");
        //The specified object does not exist
        return ERROR_OBJECT_NOT_FOUND;
    }
    
    //Successful processing
    return NO_ERROR;   
}
/**
 * @brief Set VACGroup object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[in] value Object value
 * @param[in] valueLen Length of the object value, in bytes
 * @return Error code
 **/
error_t privateMibSetVACInfoEntry(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    uint_t j;
    tModbusParameters sModbusDev[3];
    uint8_t charIdx = 0;
    uint8_t setChar = 0;
    uint8_t token = 0;
    uint8_t buff[6][4];
    uint8_t const *pui8Char;
    PrivateMibVACInfoEntry *entry;

    //Point to the instance identifier
    n = object->oidLen;

    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index < 1 || index > 2)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the vacTable entry
    entry = &privateMibBase.vacGroup.vacTable[index - 1];
    
    //vacSysMode object?
    if(!strcmp(object->name, "vacSysMode"))
    {
      if((value->integer >= 0) && (value->integer <= 2))
      {
        //Get object value
        entry->vacSysMode= value->integer;
        entry->vacSysModeWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_SYSMODE;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacFanSpeedMode object?
    else if(!strcmp(object->name, "vacFanSpeedMode"))
    {
      if((value->integer >= 0) && (value->integer <= 1))
      {
        //Get object value
        entry->vacFanSpeedMode= value->integer;
        entry->vacFanSpeedModeWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_FANSPEEDMODE;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacFanStartTemp object?
    else if(!strcmp(object->name, "vacFanStartTemp"))
    {
      if((value->integer >= 200) && (value->integer <= 300))
      {
        //Get object value
        entry->vacFanStartTemp= value->integer;
        entry->vacFanStartTempWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_FANSTARTTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacSetTemp object?
    else if(!strcmp(object->name, "vacSetTemp"))
    {
      if((value->integer >= 200) && (value->integer <= 400))
      {
        //Get object value
        entry->vacSetTemp= value->integer;
        entry->vacSetTempWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_SETTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacPidOffsetTemp object?
    else if(!strcmp(object->name, "vacPidOffsetTemp"))
    {
      if((value->integer >= 10) && (value->integer <= 100))
      {
        //Get object value
        entry->vacPidOffsetTemp= value->integer;
        entry->vacPidOffsetTempWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_PIDOFFSETTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacFanMinSpeed object?
    else if(!strcmp(object->name, "vacFanMinSpeed"))
    {
      if((value->integer >= 0) && (value->integer <= 50))
      {
        //Get object value
        entry->vacFanMinSpeed= value->integer;
        entry->vacFanMinSpeedWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_FANMINSPEED;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacFanMaxSpeed object?
    else if(!strcmp(object->name, "vacFanMaxSpeed"))
    {
      if((value->integer >= 70) && (value->integer <= 100))
      {
        //Get object value
        entry->vacFanMaxSpeed= value->integer;
        entry->vacFanMaxSpeedWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_FANMAXSPEED;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacFilterStuckTemp object?
    else if(!strcmp(object->name, "vacFilterStuckTemp"))
    {
      if((value->integer >= 10) && (value->integer <= 200))
      {
        //Get object value
        entry->vacFilterStuckTemp= value->integer;
        entry->vacFilterStuckTempWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_FILTERSTUCKTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacNightModeEn object?
    else if(!strcmp(object->name, "vacNightModeEn"))
    {
      if((value->integer >= 0) && (value->integer <= 1))
      {
        //Get object value
        entry->vacNightModeEn= value->integer;
        entry->vacNightModeEnWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_NIGHTMODEEN;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacNightModeStart object?
    else if(!strcmp(object->name, "vacNightModeStart"))
    {
      if((value->integer >= 0) && (value->integer <= 23))
      {
        //Get object value
        entry->vacNightModeStart= value->integer;
        entry->vacNightModeStartWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_NIGHTMODESTART;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacNightModeEnd object?
    else if(!strcmp(object->name, "vacNightModeEnd"))
    {
      if((value->integer >= 0) && (value->integer <= 23))
      {
        //Get object value
        entry->vacNightModeEnd= value->integer;
        entry->vacNightModeEndWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_NIGHTMODEEND;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacNightMaxSpeed object?
    else if(!strcmp(object->name, "vacNightMaxSpeed"))
    {
      if((value->integer >= 50) && (value->integer <= 100))
      {
        //Get object value
        entry->vacNightMaxSpeed= value->integer;
        entry->vacNightMaxSpeedWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_NIGHTMAXSPEED;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacManualMode object?
    else if(!strcmp(object->name, "vacManualMode"))
    {
      if((value->integer >= 0) && (value->integer <= 1))
      {
        //Get object value
        entry->vacManualMode= value->integer;
        entry->vacManualModeWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_MANUALMODE;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacManualMaxSpeed object?
    else if(!strcmp(object->name, "vacManualMaxSpeed"))
    {
      if((value->integer >= 0) && (value->integer <= 100))
      {
        //Get object value
        entry->vacManualMaxSpeed= value->integer;
        entry->vacManualMaxSpeedWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_MANUALMAXSPEED;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacInMaxTemp object?
    else if(!strcmp(object->name, "vacInMaxTemp"))
    {
      if((value->integer >= 300) && (value->integer <= 600))
      {
        //Get object value
        entry->vacInMaxTemp= value->integer;
        entry->vacInMaxTempWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_INMAXTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacExMaxTemp object?
    else if(!strcmp(object->name, "vacExMaxTemp"))
    {
      if((value->integer >= 300) && (value->integer <= 600))
      {
        //Get object value
        entry->vacExMaxTemp= value->integer;
        entry->vacExMaxTempWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_EXMAXTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacFrostMaxTemp object?
    else if(!strcmp(object->name, "vacFrostMaxTemp"))
    {
      if((value->integer >= 200) && (value->integer <= 500))
      {
        //Get object value
        entry->vacFrostMaxTemp= value->integer;
        entry->vacFrostMaxTempWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_FROSTMAXTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacInMinTemp object?
    else if(!strcmp(object->name, "vacInMinTemp"))
    {
      if((value->integer >= -100) && (value->integer <= 200))
      {
        //Get object value
        entry->vacInMinTemp= value->integer;
        entry->vacInMinTempWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_INMINTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacExMinTemp object?
    else if(!strcmp(object->name, "vacExMinTemp"))
    {
      if((value->integer >= -100) && (value->integer <= 200))
      {
        //Get object value
        entry->vacExMinTemp= value->integer;
        entry->vacExMinTempWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_EXMINTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacFrostMinTemp object?
    else if(!strcmp(object->name, "vacFrostMinTemp"))
    {
      if((value->integer >= -100) && (value->integer <= 200))
      {
        //Get object value
        entry->vacFrostMinTemp= value->integer;
        entry->vacFrostMinTempWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_FROSTMINTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacMinOutTemp object?
    else if(!strcmp(object->name, "vacMinOutTemp"))
    {
      if((value->integer >= -100) && (value->integer <= 100))
      {
        //Get object value
        entry->vacMinOutTemp= value->integer;
        entry->vacMinOutTempWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_MINOUTTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacDeltaTemp object?
    else if(!strcmp(object->name, "vacDeltaTemp"))
    {
      if((value->integer >= 0) && (value->integer <= 100))
      {
        //Get object value
        entry->vacDeltaTemp= value->integer;
        entry->vacDeltaTempWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_DELTATEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacPanicTemp object?
    else if(!strcmp(object->name, "vacPanicTemp"))
    {
      if((value->integer >= 300) && (value->integer <= 500))
      {
        //Get object value
        entry->vacPanicTemp= value->integer;
        entry->vacPanicTempWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_PANICTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
//    //vacACU1OnTemp object?
//    else if(!strcmp(object->name, "vacACU1OnTemp"))
//    {
//      if((value->integer >= 300) && (value->integer <= 400))
//      {
//        //Get object value
//        entry->vacACU1OnTemp= value->integer;
//        sModbusManager.SettingCommand = SET_VAC_ACU1ONTEMP;
//      }
//      else
//      {
//        return ERROR_PARAMETER_OUT_OF_RANGE;
//      }
//    }
//    //vacACU2OnTemp object?
//    else if(!strcmp(object->name, "vacACU2OnTemp"))
//    {
//      if((value->integer >= 300) && (value->integer <= 400))
//      {
//        //Get object value
//        entry->vacACU2OnTemp= value->integer;
//        sModbusManager.SettingCommand = SET_VAC_ACU2ONTEMP;
//      }
//      else
//      {
//        return ERROR_PARAMETER_OUT_OF_RANGE;
//      }
//    }
//    //vacACU2En object?
//    else if(!strcmp(object->name, "vacACU2En"))
//    {
//      if((value->integer >= 1) && (value->integer <= 2))
//      {
//        //Get object value
//        entry->vacACU2En= value->integer;
//        sModbusManager.SettingCommand = SET_VAC_ACU2EN;
//      }
//      else
//      {
//        return ERROR_PARAMETER_OUT_OF_RANGE;
//      }
//    }
    //vacAirCon1Model object?
    else if(!strcmp(object->name, "vacAirCon1Model"))
    {
      if((value->integer >= 0) && (value->integer <= 10))
      {
        //Get object value
        entry->vacAirCon1Model= value->integer;
        entry->vacAirCon1ModelWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_AIRCON1MODEL;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacAirCon1Type object?
    else if(!strcmp(object->name, "vacAirCon1Type"))
    {
      if((value->integer >= 0) && (value->integer <= 99))
      {
        //Get object value
        entry->vacAirCon1Type= value->integer;
        entry->vacAirCon1TypeWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_AIRCON1TYPE;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacAirCon2Model object?
    else if(!strcmp(object->name, "vacAirCon2Model"))
    {
      if((value->integer >= 0) && (value->integer <= 10))
      {
        //Get object value
        entry->vacAirCon2Model= value->integer;
        entry->vacAirCon2ModelWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_AIRCON2MODEL;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacAirCon2Type object?
    else if(!strcmp(object->name, "vacAirCon2Type"))
    {
      if((value->integer >= 0) && (value->integer <= 99))
      {
        //Get object value
        entry->vacAirCon2Type= value->integer;
        entry->vacAirCon2TypeWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_AIRCON2TYPE;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacAirConOnOff object?
    else if(!strcmp(object->name, "vacAirConOnOff"))
    {
      if((value->integer >= 0) && (value->integer <= 1))
      {
        //Get object value
        entry->vacAirConOnOff= value->integer;
        entry->vacAirConOnOffWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_AIRCONONOFF;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacAirConMode object?
    else if(!strcmp(object->name, "vacAirConMode"))
    {
      if((value->integer >= 1) && (value->integer <= 5))
      {
        //Get object value
        entry->vacAirConMode= value->integer;
        entry->vacAirConModeWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_AIRCONMODE;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacAirConTemp object?
    else if(!strcmp(object->name, "vacAirConTemp"))
    {
      if((value->integer >= 16) && (value->integer <= 30))
      {
        //Get object value
        entry->vacAirConTemp= value->integer;
        entry->vacAirConTempWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_AIRCONTEMP;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacAirConSpeed object?
    else if(!strcmp(object->name, "vacAirConSpeed"))
    {
      if((value->integer >= 1) && (value->integer <= 4))
      {
        //Get object value
        entry->vacAirConSpeed= value->integer;
        entry->vacAirConSpeedWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_AIRCONSPEED;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacAircondDir object?
    else if(!strcmp(object->name, "vacAircondDir"))
    {
      if((value->integer >= 0) && (value->integer <= 3))
      {
        //Get object value
        entry->vacAircondDir= value->integer;
        entry->vacAircondDirWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_AIRCONDDIR;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacType object?
    else if(!strcmp(object->name, "vacType"))
    {
      if((value->integer >= 0) && (value->integer <= 2))
      {
        //Get object value
        entry->vacType= value->integer;
        j = index;
        
        sModbusManager.sVACManager[j-1].u8VACType = entry->vacType;
        switch(sModbusManager.sVACManager[j-1].u8VACType)
        {
        case 1:// VIETTEL
        {
            sModbusManager.sVACManager[j-1].u32VACSlaveOffset = 34;
            sModbusManager.sVACManager[j-1].u32VACAbsSlaveID = 34+j;
            sModbusManager.sVACManager[j-1].u32VACBaseAddr[0] = 0;
            sModbusManager.sVACManager[j-1].u32VACBaseAddr[1] = 0;
            sModbusManager.sVACManager[j-1].u32VACBaseAddr[2] = 80;
            sModbusManager.sVACManager[j-1].u8VACNumOfReg[0] = 14;    
            sModbusManager.sVACManager[j-1].u8VACNumOfReg[1] = 39;   
            sModbusManager.sVACManager[j-1].u8VACNumOfReg[2] = 3;
        }
        break;
        
        case 2:// 
        {                                     
        }
        break;
        };
        sModbusDev[j-1].u8DevVendor = sModbusManager.sVACManager[j-1].u8VACType;
        sModbusDev[j-1].u32SlaveID = sModbusManager.sVACManager[j-1].u32VACAbsSlaveID;  
        //
        // Did parameters change?
        //
        if  (g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM].u8DevVendor != sModbusDev[j-1].u8DevVendor)
        {
          //
          // Update the current parameters with the new settings.
          //
          g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM].u8DevVendor = sModbusDev[j-1].u8DevVendor;
          g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM].u32SlaveID = sModbusDev[j-1].u32SlaveID;
          //
          // Yes - save these settings as the defaults.
          //
          g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM].u8DevVendor = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM].u8DevVendor;
          g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM].u32SlaveID = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM].u32SlaveID;
           
          u8SaveConfigFlag |= 1;
        }   
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacRealTimeSync object?
    else if(!strcmp(object->name, "vacRealTimeSync"))
    {
      //Make sure the buffer is large enough to hold the entire object
      if(40 >= valueLen)
      {
        pui8Char = &value->octetString[0];        
        charIdx = 0;
        for (uint8_t i = 0; i < valueLen; i++)
        {
          setChar = *pui8Char++;
          switch (setChar)
          {
          case '/':
          case ':':
          case '-':
            {
              token++;
              charIdx = 0;
            }
            break;
          case ' ':
            break;
          default:
            {
              buff[token][charIdx]= setChar;
              charIdx++;
            }
            break;
          };
        }
     
        SyncTime.hour = atol((const char*)&buff[0][0]);
        SyncTime.min = atol((const char*)&buff[1][0]);
        SyncTime.sec = atol((const char*)&buff[2][0]);
        SyncTime.date = atol((const char*)&buff[3][0]);
        SyncTime.month = atol((const char*)&buff[4][0]);
        SyncTime.year = atol((const char*)&buff[5][0]);
        if((SyncTime.sec > 59)||
           (SyncTime.min > 59) || 
           (SyncTime.hour > 23) || 
           (SyncTime.year > 99)  || 
           (SyncTime.month > 12) || 
           (SyncTime.date > 31))
        {
          return ERROR_PARAMETER_OUT_OF_RANGE;
        }
        token = 0;
        charIdx = 0;
        memset(&buff, 0, sizeof(buff));
        
        sprintf(&privateMibBase.vacGroup.vacTable[0].cSyncTime[0], "%02d:%02d:%02d-%02d/%02d/%02d",
                SyncTime.hour, 
                SyncTime.min, 
                SyncTime.sec, 
                SyncTime.date, 
                SyncTime.month, 
                SyncTime.year);
       
      
          //Copy object value
//          memcpy(entry->siteDCsystemVendor, value->octetString, valueLen);
          //Return object length
          privateMibBase.vacGroup.vacTable[0].cSyncTimeLen = 20;
          configHandle.isSyncTime = 1;
          sModbusManager.SettingCommand = SET_VAC_SYNCTIME;
      }
      else
      {
          //Report an error
          error = ERROR_BUFFER_OVERFLOW;
      }
    }
    //vacWEnable object?
    else if(!strcmp(object->name, "vacWEnable"))
    {
      if((value->integer == 0)||((value->integer == 1)&&(entry->vacWSerial2 == 1)))
      {
        //Get object value
        entry->vacWEnable= value->integer;
        entry->vacWEnableWrite= value->integer;
        sModbusManager.SettingCommand = SET_VAC_W_ENABLE;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacWSerial1 object?
    else if(!strcmp(object->name, "vacWSerial1"))
    {
      if((value->integer >= 0) && (value->integer <= 9999))
      {
        //Get object value
        entry->vacWSerial1= value->integer;
        entry->vacWSerial1Write= value->integer;
        sModbusManager.SettingCommand = SET_VAC_W_SERIAL1;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacWSerial2 object?
    else if(!strcmp(object->name, "vacWSerial2"))
    {
      if((value->integer >= 0) && (value->integer <= 99999))
      {
        //Get object value
        entry->vacWSerial2= value->integer;
        entry->vacWSerial2Write= value->integer;
        sModbusManager.SettingCommand = SET_VAC_W_SERIAL2;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //vacEnableReset object?
    else if(!strcmp(object->name, "vacEnableReset"))
    {
      if(value->integer == 1)
      {
        //Get object value
        sModbusManager.SettingCommand = SET_VAC_RESET;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
   
    //Successful processing
    return NO_ERROR;
}

/**
 * @brief Get VACInfoEntry object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t privateMibGetVACInfoEntry(const MibObject *object, const uint8_t *oid,
                                  size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibVACInfoEntry *entry;

    //Point to the instance identifier
    n = object->oidLen;

    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index < 1 || index > 10)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the interface table entry
    entry = &privateMibBase.vacGroup.vacTable[index - 1];
    //vacID object?
    if(!strcmp(object->name, "vacID"))
    {
        //Get object value
        value->integer = entry->vacID;
    }
    //vacStatus object?
    else if(!strcmp(object->name, "vacStatus"))
    {
        //Get object value
        value->integer = entry->vacStatus;
    }
    //vacInTemp object?
    else if(!strcmp(object->name, "vacInTemp"))
    {
        //Get object value
        value->integer = entry->vacInTemp;
    }
    //vacExTemp object?
    else if(!strcmp(object->name, "vacExTemp"))
    {
        //Get object value
        value->integer = entry->vacExTemp;
    }
    //vacFrost1Temp object?
    else if(!strcmp(object->name, "vacFrost1Temp"))
    {
        //Get object value
        value->integer = entry->vacFrost1Temp;
    }
    //vacFrost2Temp object?
    else if(!strcmp(object->name, "vacFrost2Temp"))
    {
        //Get object value
        value->integer = entry->vacFrost2Temp;
    }
    //vacHumid object?
    else if(!strcmp(object->name, "vacHumid"))
    {
        //Get object value
        value->integer = entry->vacHumid;
    }
    //vacFan1Duty object?
    else if(!strcmp(object->name, "vacFan1Duty"))
    {
        //Get object value
        value->integer = entry->vacFan1Duty;
    }
    //vacFan1RPM object?
    else if(!strcmp(object->name, "vacFan1RPM"))
    {
        //Get object value
        value->integer = entry->vacFan1RPM;
    }
    //vacFan2Duty object?
    else if(!strcmp(object->name, "vacFan2Duty"))
    {
        //Get object value
        value->integer = entry->vacFan2Duty;
    }
    //vacFan2RPM object?
    else if(!strcmp(object->name, "vacFan2RPM"))
    {
        //Get object value
        value->integer = entry->vacFan2RPM;
    }
    //vacAlarmStatus object?
    else if(!strcmp(object->name, "vacAlarmStatus"))
    {
        //Get object value
        value->integer = entry->vacAlarmStatus.all;
    }
    //vacSysMode object?
    else if(!strcmp(object->name, "vacSysMode"))
    {
        //Get object value
        value->integer = entry->vacSysMode;
    }
    //vacFanSpeedMode object?
    else if(!strcmp(object->name, "vacFanSpeedMode"))
    {
        //Get object value
        value->integer = entry->vacFanSpeedMode;
    }
    //vacFanStartTemp object?
    else if(!strcmp(object->name, "vacFanStartTemp"))
    {
        //Get object value
        value->integer = entry->vacFanStartTemp;
    }
    //vacSetTemp object?
    else if(!strcmp(object->name, "vacSetTemp"))
    {
        //Get object value
        value->integer = entry->vacSetTemp;
    }
    //vacPidOffsetTemp object?
    else if(!strcmp(object->name, "vacPidOffsetTemp"))
    {
        //Get object value
        value->integer = entry->vacPidOffsetTemp;
    }
    //vacFanMinSpeed object?
    else if(!strcmp(object->name, "vacFanMinSpeed"))
    {
        //Get object value
        value->integer = entry->vacFanMinSpeed;
    }
    //vacFanMaxSpeed object?
    else if(!strcmp(object->name, "vacFanMaxSpeed"))
    {
        //Get object value
        value->integer = entry->vacFanMaxSpeed;
    }
    //vacFilterStuckTemp object?
    else if(!strcmp(object->name, "vacFilterStuckTemp"))
    {
        //Get object value
        value->integer = entry->vacFilterStuckTemp;
    }
    //vacNightModeEn object?
    else if(!strcmp(object->name, "vacNightModeEn"))
    {
        //Get object value
        value->integer = entry->vacNightModeEn;
    }
    //vacNightModeStart object?
    else if(!strcmp(object->name, "vacNightModeStart"))
    {
        //Get object value
        value->integer = entry->vacNightModeStart;
    }
    //vacNightModeEnd object?
    else if(!strcmp(object->name, "vacNightModeEnd"))
    {
        //Get object value
        value->integer = entry->vacNightModeEnd;
    }
    //vacNightMaxSpeed object?
    else if(!strcmp(object->name, "vacNightMaxSpeed"))
    {
        //Get object value
        value->integer = entry->vacNightMaxSpeed;
    }
    //vacManualMode object?
    else if(!strcmp(object->name, "vacManualMode"))
    {
        //Get object value
        value->integer = entry->vacManualMode;
    }
    //vacManualMaxSpeed object?
    else if(!strcmp(object->name, "vacManualMaxSpeed"))
    {
        //Get object value
        value->integer = entry->vacManualMaxSpeed;
    }
    //vacInMaxTemp object?
    else if(!strcmp(object->name, "vacInMaxTemp"))
    {
        //Get object value
        value->integer = entry->vacInMaxTemp;
    }
    //vacExMaxTemp object?
    else if(!strcmp(object->name, "vacExMaxTemp"))
    {
        //Get object value
        value->integer = entry->vacExMaxTemp;
    }
    //vacFrostMaxTemp object?
    else if(!strcmp(object->name, "vacFrostMaxTemp"))
    {
        //Get object value
        value->integer = entry->vacFrostMaxTemp;
    }
    //vacInMinTemp object?
    else if(!strcmp(object->name, "vacInMinTemp"))
    {
        //Get object value
        value->integer = entry->vacInMinTemp;
    }
    //vacExMinTemp object?
    else if(!strcmp(object->name, "vacExMinTemp"))
    {
        //Get object value
        value->integer = entry->vacExMinTemp;
    }
    //vacFrostMinTemp object?
    else if(!strcmp(object->name, "vacFrostMinTemp"))
    {
        //Get object value
        value->integer = entry->vacFrostMinTemp;
    }
    //vacMinOutTemp object?
    else if(!strcmp(object->name, "vacMinOutTemp"))
    {
        //Get object value
        value->integer = entry->vacMinOutTemp;
    }
    //vacDeltaTemp object?
    else if(!strcmp(object->name, "vacDeltaTemp"))
    {
        //Get object value
        value->integer = entry->vacDeltaTemp;
    }
    //vacPanicTemp object?
    else if(!strcmp(object->name, "vacPanicTemp"))
    {
        //Get object value
        value->integer = entry->vacPanicTemp;
    }
    //vacActiveFan object?
    else if(!strcmp(object->name, "vacActiveFan"))
    {
        //Get object value
        value->integer = entry->vacActiveFan;
    }
    //vacInstalledFan object?
    else if(!strcmp(object->name, "vacInstalledFan"))
    {
        //Get object value
        value->integer = entry->vacInstalledFan;
    }
    //vacInstalledAirCon object?
    else if(!strcmp(object->name, "vacInstalledAirCon"))
    {
        //Get object value
        value->integer = entry->vacInstalledAirCon;
    }
    //vacAirCon1Model object?
    else if(!strcmp(object->name, "vacAirCon1Model"))
    {
        //Get object value
        value->integer = entry->vacAirCon1Model;
    }
    //vacAirCon1Type object?
    else if(!strcmp(object->name, "vacAirCon1Type"))
    {
        //Get object value
        value->integer = entry->vacAirCon1Type;
    }
    //vacAirCon2Model object?
    else if(!strcmp(object->name, "vacAirCon2Model"))
    {
        //Get object value
        value->integer = entry->vacAirCon2Model;
    }
    //vacAirCon2Type object?
    else if(!strcmp(object->name, "vacAirCon2Type"))
    {
        //Get object value
        value->integer = entry->vacAirCon2Type;
    }
    //vacAirConOnOff object?
    else if(!strcmp(object->name, "vacAirConOnOff"))
    {
        //Get object value
        value->integer = entry->vacAirConOnOff;
    }
    //vacAirConMode object?
    else if(!strcmp(object->name, "vacAirConMode"))
    {
        //Get object value
        value->integer = entry->vacAirConMode;
    }
    //vacAirConTemp object?
    else if(!strcmp(object->name, "vacAirConTemp"))
    {
        //Get object value
        value->integer = entry->vacAirConTemp;
    }
    //vacAirConSpeed object?
    else if(!strcmp(object->name, "vacAirConSpeed"))
    {
        //Get object value
        value->integer = entry->vacAirConSpeed;
    }
    //vacAircondDir object?
    else if(!strcmp(object->name, "vacAircondDir"))
    {
        //Get object value
        value->integer = entry->vacAircondDir;
    }
    //vacAirConVolt object?
    else if(!strcmp(object->name, "vacAirConVolt"))
    {
        //Get object value
        value->integer = entry->vacAirConVolt;
    }
    //vacAirConCurrent object?
    else if(!strcmp(object->name, "vacAirConCurrent"))
    {
        //Get object value
        value->integer = entry->vacAirConCurrent;
    }
    //vacAirConPower object?
    else if(!strcmp(object->name, "vacAirConPower"))
    {
        //Get object value
        value->integer = entry->vacAirConPower;
    }
    //vacAirConFrequency object?
    else if(!strcmp(object->name, "vacAirConFrequency"))
    {
        //Get object value
        value->integer = entry->vacAirConFrequency;
    }
    //vacAirConPF object?
    else if(!strcmp(object->name, "vacAirConPF"))
    {
        //Get object value
        value->integer = entry->vacAirConPF;
    }
    //vacRealTimeSync object?
    else if(!strcmp(object->name, "vacRealTimeSync"))
    {
        //Make sure the buffer is large enough to hold the entire object
        if(*valueLen >= privateMibBase.vacGroup.vacTable[0].cSyncTimeLen)
        {
            //Copy object value
            memcpy(value->octetString, privateMibBase.vacGroup.vacTable[0].cSyncTime, privateMibBase.vacGroup.vacTable[0].cSyncTimeLen);
            //Return object length
            *valueLen = privateMibBase.vacGroup.vacTable[0].cSyncTimeLen;
        }
        else
        {
            //Report an error
            error = ERROR_BUFFER_OVERFLOW;
        }
    }
    //vacType object?
    else if(!strcmp(object->name, "vacType"))
    {
        //Get object value
        value->integer = entry->vacType;
    }
    //vacSerial object?
    else if(!strcmp(object->name, "vacSerial"))
    {
        //Make sure the buffer is large enough to hold the entire object
        if(*valueLen >= entry->vacSerialLen)
        {
            //Copy object value
            memcpy(value->octetString, entry->vacSerial, entry->vacSerialLen);
            //Return object length
            *valueLen = entry->vacSerialLen;
        }
        else
        {
            //Report an error
            error = ERROR_BUFFER_OVERFLOW;
        }
    }
    //vacModbusID object?
    else if(!strcmp(object->name, "vacModbusID"))
    {
        //Get object value
        value->integer = entry->vacModbusID;
    }
    //vacFirmVersion object?
    else if(!strcmp(object->name, "vacFirmVersion"))
    {
        //Get object value
        value->integer = entry->vacFirmVersion;
    }
    //vacWEnable object?
    else if(!strcmp(object->name, "vacWEnable"))
    {
        //Get object value
        value->integer = entry->vacWEnable;
    }
    //vacWSerial1 object?
    else if(!strcmp(object->name, "vacWSerial1"))
    {
        //Get object value
        value->integer = entry->vacWSerial1;
    }
    //vacWSerial2 object?
    else if(!strcmp(object->name, "vacWSerial2"))
    {
        //Get object value
        value->integer = entry->vacWSerial2;
    }
    //vacEnableReset object?
    else if(!strcmp(object->name, "vacEnableReset"))
    {
        //Get object value
        value->integer = entry->vacEnableReset;
    }
    //vacUpdateStep object?
    else if(!strcmp(object->name, "vacUpdateStep"))
    {
        //Get object value
        value->integer = entry->vacUpdateStep;
    }
      

    //Return status code
    return error;
}


/**
 * @brief Get next VACInfoEntry object
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] nextOid OID of the next object in the MIB
 * @param[out] nextOidLen Length of the next object identifier, in bytes
 * @return Error code
 **/
error_t privateMibGetNextVACInfoEntry(const MibObject *object, const uint8_t *oid,
                                      size_t oidLen, uint8_t *nextOid, size_t *nextOidLen)
{
    error_t error;
    size_t n;
    uint_t index;

    //Make sure the buffer is large enough to hold the OID prefix
    if(*nextOidLen < object->oidLen)
        return ERROR_BUFFER_OVERFLOW;

    //Copy OID prefix
    memcpy(nextOid, object->oid, object->oidLen);

    //Loop through network interfaces
    for(index = 1; index <= privateMibBase.vacGroup.vacInstalledVAC; index++)
    {
        //Append the instance identifier to the OID prefix
        n = object->oidLen;

        //The ifIndex is used as instance identifier
        error = mibEncodeIndex(nextOid, *nextOidLen, &n, index);
        //Any error to report?
        if(error) return error;

        //Check whether the resulting object identifier lexicographically
        //follows the specified OID
        if(oidComp(nextOid, n, oid, oidLen) > 0)
        {
            //Save the length of the resulting object identifier
            *nextOidLen = n;
            //Next object found
            return NO_ERROR;
        }
    }

    //The specified OID does not lexicographically precede the name
    //of some object
    return ERROR_OBJECT_NOT_FOUND;
}
//========================================== VACInfo Function ==========================================//
//========================================== CfgBTSCU Function ==========================================//
/**
 * @brief Set CfgBTSCUEntry object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/
error_t privateMibSetCfgBTSCUEntry(const MibObject *object, const uint8_t *oid,
        size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibCfgBTSCUEntry *entry;
    //Point to the instance identifier
    n = object->oidLen;

    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index < 1 || index > 5)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the cfgBTPlanTestTable entry
    entry = &privateMibBase.cfgBTGroup.cfgBTSCUTable[index - 1];

    //cfgBTSCUYear object?
    if(!strcmp(object->name, "cfgBTSCUYear"))
    {      
      if((value->integer >= 2000) && (value->integer <= 2099))
      {
        //Get object value
        entry->cfgBTSCUYear = value->integer;       
        sBattTestInfo.u8SetPlan = 5;       
        setCmd_flag = 1;
        setCmd_mask = SET_BT_SCUTIME;
        sBattTestInfo.u8CheckSentAll = 1;
      } 
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      } 
    }
    //cfgBTSCUMonth object?
    else if(!strcmp(object->name, "cfgBTSCUMonth"))
    {      
      if((value->integer >= 1) && (value->integer <= 12))
      {
        //Get object value
        entry->cfgBTSCUMonth = value->integer;       
        sBattTestInfo.u8SetPlan = 5;       
        setCmd_flag = 1;
        setCmd_mask = SET_BT_SCUTIME;
        sBattTestInfo.u8CheckSentAll = 1;
      } 
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }        
    }
    //cfgBTSCUDate object?
    else if(!strcmp(object->name, "cfgBTSCUDate"))
    {      
      if((value->integer >= 1) && (value->integer <= 31))
      {
        //Get object value
        entry->cfgBTSCUDate = value->integer;       
        sBattTestInfo.u8SetPlan = 5;       
        setCmd_flag = 1;
        setCmd_mask = SET_BT_SCUTIME;
        sBattTestInfo.u8CheckSentAll = 1;
      } 
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //cfgBTSCUHour object?
    else if(!strcmp(object->name, "cfgBTSCUHour"))
    {      
      if(value->integer <= 23)
      {
        //Get object value
        entry->cfgBTSCUHour = value->integer;       
        sBattTestInfo.u8SetPlan = 5;       
        setCmd_flag = 1;
        setCmd_mask = SET_BT_SCUTIME;
        sBattTestInfo.u8CheckSentAll = 1;
      } 
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //cfgBTSCUMinute object?
    else if(!strcmp(object->name, "cfgBTSCUMinute"))
    {      
      if(value->integer <= 59)
      {
        //Get object value
        entry->cfgBTSCUMinute = value->integer;       
        sBattTestInfo.u8SetPlan = 5;       
        setCmd_flag = 1;
        setCmd_mask = SET_BT_SCUTIME;
        sBattTestInfo.u8CheckSentAll = 1;
      } 
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //cfgBTSCUSecond object?
    else if(!strcmp(object->name, "cfgBTSCUSecond"))
    {      
      if(value->integer <= 59)
      {
        //Get object value
        entry->cfgBTSCUSecond = value->integer;       
        sBattTestInfo.u8SetPlan = 5;       
        setCmd_flag = 1;
        setCmd_mask = SET_BT_SCUTIME;
        sBattTestInfo.u8CheckSentAll = 1;
      } 
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    
//    //ifIndex object?
//    if(!strcmp(object->name, "configAccessIdCard"))
//    {
//        //Make sure the buffer is large enough to hold the entire object
//        if(valueLen >= sizeof(entry->configAccessIdCard))
//            return ERROR_BUFFER_OVERFLOW;
//
//        //Copy object value
//        memset(entry->configAccessIdCard,0,sizeof(entry->configAccessIdCard));
//        memcpy(entry->configAccessIdCard, value->octetString, valueLen);
//        entry->configAccessIdCardLen = 8;
////            for(j=0;j<8;j++)
////            {
////              sMenu_Variable.u8UserID[index-1][j] = entry->configAccessIdCard[j];
////            }
////            Save_User_ID(sMenu_Variable.u8UserIDAddr[index-1],entry->configAccessIdCard);
//    }

    //Unknown object?
    else
    {
        //The specified object does not exist
        error = ERROR_OBJECT_NOT_FOUND;
    }

    //Return status code
    return error;
}

/**
 * @brief Get CfgBTSCUEntry object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t privateMibGetCfgBTSCUEntry(const MibObject *object, const uint8_t *oid,
        size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibCfgBTSCUEntry *entry;

    //Point to the instance identifier
    n = object->oidLen;

    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index < 1 || index > 5)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the cfgBTPlanTestTable entry
    entry = &privateMibBase.cfgBTGroup.cfgBTSCUTable[index - 1];

    //cfgBTSCUIndex object?
    if(!strcmp(object->name, "cfgBTSCUIndex"))
    {
        //Get object value
        value->integer = entry->cfgBTSCUIndex;
    }
    //cfgBTSCUYear object?
    else if(!strcmp(object->name, "cfgBTSCUYear"))
    {
        //Get object value
        value->integer = entry->cfgBTSCUYear;
    }
    //cfgBTSCUMonth object?
    else if(!strcmp(object->name, "cfgBTSCUMonth"))
    {
        //Get object value
        value->integer = entry->cfgBTSCUMonth;
    }
    //cfgBTSCUDate object?
    else if(!strcmp(object->name, "cfgBTSCUDate"))
    {
        //Get object value
        value->integer = entry->cfgBTSCUDate;
    }
    //cfgBTSCUHour object?
    else if(!strcmp(object->name, "cfgBTSCUHour"))
    {
        //Get object value
        value->integer = entry->cfgBTSCUHour;
    }
    //cfgBTSCUMinute object?
    else if(!strcmp(object->name, "cfgBTSCUMinute"))
    {
        //Get object value
        value->integer = entry->cfgBTSCUMinute;
    }
    //cfgBTSCUSecond object?
    else if(!strcmp(object->name, "cfgBTSCUSecond"))
    {
        //Get object value
        value->integer = entry->cfgBTSCUSecond;
    }
//    //configAccessIdCard object?
//    else if(!strcmp(object->name, "configAccessIdCard"))
//    {
//        //Make sure the buffer is large enough to hold the entire object
//        if(*valueLen >= entry->configAccessIdCardLen)
//        {
//            //Copy object value
//            memcpy(value->octetString, entry->configAccessIdCard, entry->configAccessIdCardLen);
//            //Return object length
//            *valueLen = entry->configAccessIdCardLen;
//        }
//        else
//        {
//            //Report an error
//            error = ERROR_BUFFER_OVERFLOW;
//        }
//    }
    //Unknown object?
    else
    {
        //The specified object does not exist
        error = ERROR_OBJECT_NOT_FOUND;
    }

    //Return status code
    return error;
}


/**
 * @brief Get next CfgBTSCUEntry object
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] nextOid OID of the next object in the MIB
 * @param[out] nextOidLen Length of the next object identifier, in bytes
 * @return Error code
 **/
error_t privateMibGetNextCfgBTSCUEntry(const MibObject *object, const uint8_t *oid,
        size_t oidLen, uint8_t *nextOid, size_t *nextOidLen)
{
    error_t error;
    size_t n;
    uint_t index;

    //Make sure the buffer is large enough to hold the OID prefix
    if(*nextOidLen < object->oidLen)
        return ERROR_BUFFER_OVERFLOW;

    //Copy OID prefix
    memcpy(nextOid, object->oid, object->oidLen);

    //Loop through network interfaces
    for(index = 1; index <= privateMibBase.cfgBTGroup.cfgBTSCUNumber; index++)
    {
        //Append the instance identifier to the OID prefix
        n = object->oidLen;

        //The ifIndex is used as instance identifier
        error = mibEncodeIndex(nextOid, *nextOidLen, &n, index);
        //Any error to report?
        if(error) return error;

        //Check whether the resulting object identifier lexicographically
        //follows the specified OID
        if(oidComp(nextOid, n, oid, oidLen) > 0)
        {
            //Save the length of the resulting object identifier
            *nextOidLen = n;
            //Next object found
            return NO_ERROR;
        }
    }
    //The specified OID does not lexicographically precede the name
    //of some object
    return ERROR_OBJECT_NOT_FOUND;
}
//========================================== CfgBTSCU Function ==========================================//
//========================================== CfgBTPlanTest Function ==========================================//
/**
 * @brief Set privateMibSetCfgBTPlanTestEntry object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/
error_t privateMibSetCfgBTPlanTestEntry(const MibObject *object, const uint8_t *oid,
        size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibCfgBTPlanTestEntry *entry;
    //Point to the instance identifier
    n = object->oidLen;

    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index < 1 || index > 5)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the cfgBTPlanTestTable entry
    entry = &privateMibBase.cfgBTGroup.cfgBTPlanTestTable[index - 1];

    //cfgBTPlanTestMonth object?
    if(!strcmp(object->name, "cfgBTPlanTestMonth"))
    {      
      if((value->integer >= 1) && (value->integer <= 12))
      {
        //Get object value
        entry->cfgBTPlanTestMonth= value->integer;       
        sBattTestInfo.u8SetPlan = index;       
        setCmd_flag = 1;
        setCmd_mask = SET_BT_PLANTEST;
        sBattTestInfo.u8CheckSentAll = 1;
      } 
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //cfgBTPlanTestDate object?
    else if(!strcmp(object->name, "cfgBTPlanTestDate"))
    {      
      if((value->integer >= 1) && (value->integer <= 31))
      {
        //Get object value
        entry->cfgBTPlanTestDate= value->integer;
        sBattTestInfo.u8SetPlan = index;
        setCmd_flag = 1;
        setCmd_mask = SET_BT_PLANTEST;
        sBattTestInfo.u8CheckSentAll = 1;
      } 
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //cfgBTPlanTestHour object?
    else if(!strcmp(object->name, "cfgBTPlanTestHour"))
    {      
      if((value->integer >= 0) && (value->integer <= 23))
      {
        //Get object value
        entry->cfgBTPlanTestHour= value->integer;
        sBattTestInfo.u8SetPlan = index;
        setCmd_flag = 1;
        setCmd_mask = SET_BT_PLANTEST;
        sBattTestInfo.u8CheckSentAll = 1;
      } 
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    
//    //ifIndex object?
//    if(!strcmp(object->name, "configAccessIdCard"))
//    {
//        //Make sure the buffer is large enough to hold the entire object
//        if(valueLen >= sizeof(entry->configAccessIdCard))
//            return ERROR_BUFFER_OVERFLOW;
//
//        //Copy object value
//        memset(entry->configAccessIdCard,0,sizeof(entry->configAccessIdCard));
//        memcpy(entry->configAccessIdCard, value->octetString, valueLen);
//        entry->configAccessIdCardLen = 8;
////            for(j=0;j<8;j++)
////            {
////              sMenu_Variable.u8UserID[index-1][j] = entry->configAccessIdCard[j];
////            }
////            Save_User_ID(sMenu_Variable.u8UserIDAddr[index-1],entry->configAccessIdCard);
//    }

    //Unknown object?
    else
    {
        //The specified object does not exist
        error = ERROR_OBJECT_NOT_FOUND;
    }

    //Return status code
    return error;
}

/**
 * @brief Get privateMibGetCfgBTPlanTestEntry object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t privateMibGetCfgBTPlanTestEntry(const MibObject *object, const uint8_t *oid,
        size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibCfgBTPlanTestEntry *entry;

    //Point to the instance identifier
    n = object->oidLen;

    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index < 1 || index > 4)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the cfgBTPlanTestTable entry
    entry = &privateMibBase.cfgBTGroup.cfgBTPlanTestTable[index - 1];

    //cfgBTPlanTestIndex object?
    if(!strcmp(object->name, "cfgBTPlanTestIndex"))
    {
        //Get object value
        value->integer = entry->cfgBTPlanTestIndex;
    }
    //cfgBTPlanTestMonth object?
    else if(!strcmp(object->name, "cfgBTPlanTestMonth"))
    {
        //Get object value
        value->integer = entry->cfgBTPlanTestMonth;
    }
    //cfgBTPlanTestDate object?
    else if(!strcmp(object->name, "cfgBTPlanTestDate"))
    {
        //Get object value
        value->integer = entry->cfgBTPlanTestDate;
    }
    //cfgBTPlanTestHour object?
    else if(!strcmp(object->name, "cfgBTPlanTestHour"))
    {
        //Get object value
        value->integer = entry->cfgBTPlanTestHour;
    }
//    //configAccessIdCard object?
//    else if(!strcmp(object->name, "configAccessIdCard"))
//    {
//        //Make sure the buffer is large enough to hold the entire object
//        if(*valueLen >= entry->configAccessIdCardLen)
//        {
//            //Copy object value
//            memcpy(value->octetString, entry->configAccessIdCard, entry->configAccessIdCardLen);
//            //Return object length
//            *valueLen = entry->configAccessIdCardLen;
//        }
//        else
//        {
//            //Report an error
//            error = ERROR_BUFFER_OVERFLOW;
//        }
//    }
    //Unknown object?
    else
    {
        //The specified object does not exist
        error = ERROR_OBJECT_NOT_FOUND;
    }

    //Return status code
    return error;
}


/**
 * @brief Get next CfgBTPlanTestEntry object
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] nextOid OID of the next object in the MIB
 * @param[out] nextOidLen Length of the next object identifier, in bytes
 * @return Error code
 **/
error_t privateMibGetNextCfgBTPlanTestEntry(const MibObject *object, const uint8_t *oid,
        size_t oidLen, uint8_t *nextOid, size_t *nextOidLen)
{
    error_t error;
    size_t n;
    uint_t index;

    //Make sure the buffer is large enough to hold the OID prefix
    if(*nextOidLen < object->oidLen)
        return ERROR_BUFFER_OVERFLOW;

    //Copy OID prefix
    memcpy(nextOid, object->oid, object->oidLen);

    //Loop through network interfaces
    for(index = 1; index <= privateMibBase.cfgBTGroup.cfgBTPlanTestNumber; index++)
    {
        //Append the instance identifier to the OID prefix
        n = object->oidLen;

        //The ifIndex is used as instance identifier
        error = mibEncodeIndex(nextOid, *nextOidLen, &n, index);
        //Any error to report?
        if(error) return error;

        //Check whether the resulting object identifier lexicographically
        //follows the specified OID
        if(oidComp(nextOid, n, oid, oidLen) > 0)
        {
            //Save the length of the resulting object identifier
            *nextOidLen = n;
            //Next object found
            return NO_ERROR;
        }
    }
    //The specified OID does not lexicographically precede the name
    //of some object
    return ERROR_OBJECT_NOT_FOUND;
}
//========================================== CfgBTPlanTest Function ==========================================//
//========================================== CfgBTG Function ==========================================//
/**
 * @brief Set CfgBTG object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[in] value Object value
 * @param[in] valueLen Length of the object value, in bytes
 * @return Error code
 **/
error_t privateMibSetCfgBTGroup(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibCfgBTGroup *entry;

//	Point to the siteInfoGroup entry
    entry = &privateMibBase.cfgBTGroup;
    switch(TYPE)
    {
    case EMER:
      {
        //cfgBTEndVolt object?
        if(!strcmp(object->name, "cfgBTEndVolt"))
        {      
          if((value->integer >= 431) && (value->integer <= 579))
          {
            //Get object value
            entry->cfgBTEndVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BT_ENDVOLT;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBTEndCap object?
        else if(!strcmp(object->name, "cfgBTEndCap"))
        {      
          if((value->integer >= 0) && (value->integer <= 1000))
          {
            //Get object value
            entry->cfgBTEndCap= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BT_ENDCAP;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBTEndTime object?
        else if(!strcmp(object->name, "cfgBTEndTime"))
        {      
          if((value->integer >= 5) && (value->integer <= 1440))
          {
            //Get object value
            entry->cfgBTEndTime= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BT_ENDTIME;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBTPlanTestEn object?
        else if(!strcmp(object->name, "cfgBTPlanTestEn"))
        {      
          if((value->integer == 0) || (value->integer == 1))
          {
            //Get object value
            entry->cfgBTPlanTestEn= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BT_PLEN;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgTestVoltCfg object?
        else if(!strcmp(object->name, "cfgTestVoltCfg"))
        {     
          return ERROR_PARAMETER_OUT_OF_RANGE;
        }
        //cfgAutoTestDay object?
        else if(!strcmp(object->name, "cfgAutoTestDay"))
        {       
          return ERROR_PARAMETER_OUT_OF_RANGE;
        }
        //cfgTestStartTime object?
        else if(!strcmp(object->name, "cfgTestStartTime"))
        {      
          return ERROR_PARAMETER_OUT_OF_RANGE;
        }
        //cfgTestAgissonStartTest object?
        else if(!strcmp(object->name, "cfgTestAgissonStartTest"))
        {     
          return ERROR_PARAMETER_OUT_OF_RANGE;
        }
        //cfgTestAgissonStopTest object?
        else if(!strcmp(object->name, "cfgTestAgissonStopTest"))
        {     
          return ERROR_PARAMETER_OUT_OF_RANGE;
        }
        //Unknown object?
        else
        {
            TRACE_ERROR("ERROR_OBJECT_NOT_FOUND!\r\n");
            //The specified object does not exist
            return ERROR_OBJECT_NOT_FOUND;
        }
        
      }break;
    case ZTE:
      {
        //cfgBTEndVolt object?
        if(!strcmp(object->name, "cfgBTEndVolt"))
        {     
          return ERROR_PARAMETER_OUT_OF_RANGE;
        }
        //cfgBTEndCap object?
        else if(!strcmp(object->name, "cfgBTEndCap"))
        {     
          return ERROR_PARAMETER_OUT_OF_RANGE;
        }
        //cfgBTEndTime object?
        else if(!strcmp(object->name, "cfgBTEndTime"))
        {     
          return ERROR_PARAMETER_OUT_OF_RANGE;
        }
        //cfgBTPlanTestEn object?
        else if(!strcmp(object->name, "cfgBTPlanTestEn"))
        {     
          return ERROR_PARAMETER_OUT_OF_RANGE;
        }
        //cfgTestVoltCfg object?
        else if(!strcmp(object->name, "cfgTestVoltCfg"))
        {      
          if((value->integer >= (uint32_t)(privateMibBase.configGroup.cfgLLVDVolt/10) + 10) && (value->integer <= 500))
          {
            //Get object value
            entry->cfgTestVoltCfg= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_TEST_VOLT;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgAutoTestDay object?
        else if(!strcmp(object->name, "cfgAutoTestDay"))
        {      
          if((value->integer >= 15) && (value->integer <= 365))
          {
            //Get object value
            entry->cfgAutoTestDay= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_AUTO_TEST_DAY;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgTestStartTime object?
        else if(!strcmp(object->name, "cfgTestStartTime"))
        {      
          if((value->integer >= 0) && (value->integer <= 23))
          {
            //Get object value
            entry->cfgTestStartTime= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_TEST_START_TIME;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgTestAgissonStartTest object?
        else if(!strcmp(object->name, "cfgTestAgissonStartTest"))
        {     
          return ERROR_PARAMETER_OUT_OF_RANGE;
        }
        //cfgTestAgissonStopTest object?
        else if(!strcmp(object->name, "cfgTestAgissonStopTest"))
        {     
          return ERROR_PARAMETER_OUT_OF_RANGE;
        }
        //Unknown object?
        else
        {
            TRACE_ERROR("ERROR_OBJECT_NOT_FOUND!\r\n");
            //The specified object does not exist
            return ERROR_OBJECT_NOT_FOUND;
        }
      }break;
    case HW:
      {
        //cfgBTEndVolt object?
        if(!strcmp(object->name, "cfgBTEndVolt"))
        {      
          if((value->integer >= 170) && (value->integer <= 220))
          {
            //Get object value
            entry->cfgBTEndVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BT_ENDVOLT;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBTEndCap object?
        else if(!strcmp(object->name, "cfgBTEndCap"))
        { 
          return ERROR_PARAMETER_OUT_OF_RANGE;          
        }
        //cfgBTEndTime object?
        else if(!strcmp(object->name, "cfgBTEndTime"))
        {      
          if((value->integer >= 1) && (value->integer <= 100))
          {
            //Get object value
            entry->cfgBTEndTime= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BT_ENDTIME;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBTPlanTestEn object?
        else if(!strcmp(object->name, "cfgBTPlanTestEn"))
        {     
          return ERROR_PARAMETER_OUT_OF_RANGE;
        }
        //cfgTestVoltCfg object?
        else if(!strcmp(object->name, "cfgTestVoltCfg"))
        {     
          return ERROR_PARAMETER_OUT_OF_RANGE;
        }
        //cfgAutoTestDay object?
        else if(!strcmp(object->name, "cfgAutoTestDay"))
        {     
          return ERROR_PARAMETER_OUT_OF_RANGE;
        }
        //cfgTestStartTime object?
        else if(!strcmp(object->name, "cfgTestStartTime"))
        {     
          return ERROR_PARAMETER_OUT_OF_RANGE;
        }
        //cfgTestAgissonStartTest object?
        else if(!strcmp(object->name, "cfgTestAgissonStartTest"))
        {      
          if((value->integer == 1))
          {
            setCmd_flag = 1;
            setCmd_mask = START_BT_TEST;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgTestAgissonStopTest object?
        else if(!strcmp(object->name, "cfgTestAgissonStopTest"))
        {      
          if((value->integer == 1))
          {
            setCmd_flag = 1;
            setCmd_mask = STOP_BT_TEST;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //Unknown object?
        else
        {
            TRACE_ERROR("ERROR_OBJECT_NOT_FOUND!\r\n");
            //The specified object does not exist
            return ERROR_OBJECT_NOT_FOUND;
        }
      }break;
    case EMERV21:
      {
      //cfgTestVoltCfg object?
         if(!strcmp(object->name, "cfgTestVoltCfg"))
        {      
          if((value->integer >= 431) && (value->integer <= 579))
          {
            //Get object value
            entry->cfgTestVoltCfg= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_TEST_VOLT;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }        
        //cfgBTEndCap object?
        else if(!strcmp(object->name, "cfgBTEndCap"))
        {      
          if((value->integer >= 0) && (value->integer <= 1000))
          {
            //Get object value
            entry->cfgBTEndCap= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BT_ENDCAP;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBTEndTime object?
        else if(!strcmp(object->name, "cfgBTEndTime"))
        {      
          if((value->integer >= 5) && (value->integer <= 1440))
          {
            //Get object value
            entry->cfgBTEndTime= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BT_ENDTIME;
          } 
      }
        //Unknown object?
        else
        {
            TRACE_ERROR("ERROR_OBJECT_NOT_FOUND!\r\n");
            //The specified object does not exist
            return ERROR_OBJECT_NOT_FOUND;
        }
      }break;
    case DKD51_BDP:
      {
       //code DKD51 heare
      }break;
      
    default:
       break;
        };


    //Successful processing
    return NO_ERROR;
}

/**
 * @brief Get CfgBTG object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t privateMibGetCfgBTGroup(const MibObject *object, const uint8_t *oid,
                                     size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error = NO_ERROR;
    size_t n;
    PrivateMibCfgBTGroup *entry;
    entry = &privateMibBase.cfgBTGroup;

    //cfgBTEndVolt object?
    if(!strcmp(object->name, "cfgBTEndVolt"))
    {
        //Get object value
        value->integer = entry->cfgBTEndVolt;
    }
    //cfgBTEndCap object?
    else if(!strcmp(object->name, "cfgBTEndCap"))
    {
        //Get object value
        value->integer = entry->cfgBTEndCap;
    }
    //cfgBTEndTime object?
    else if(!strcmp(object->name, "cfgBTEndTime"))
    {
        //Get object value
        value->integer = entry->cfgBTEndTime;
    }
    //cfgBTPlanTestNumber object?
    else if(!strcmp(object->name, "cfgBTPlanTestNumber"))
    {
        //Get object value
        value->integer = entry->cfgBTPlanTestNumber;
    }
    //cfgBTPlanTestEn object?
    else if(!strcmp(object->name, "cfgBTPlanTestEn"))
    {
        //Get object value
        value->integer = entry->cfgBTPlanTestEn;
    }
    //cfgBTSCUNumber object?
    else if(!strcmp(object->name, "cfgBTSCUNumber"))
    {
        //Get object value
        value->integer = entry->cfgBTSCUNumber;
    }
    //cfgTestVoltCfg object?
    else if(!strcmp(object->name, "cfgTestVoltCfg"))
    {
        //Get object value
        value->integer = entry->cfgTestVoltCfg;
    }
    //cfgAutoTestDay object?
    else if(!strcmp(object->name, "cfgAutoTestDay"))
    {
        //Get object value
        value->integer = entry->cfgAutoTestDay;
    }
    //cfgTestStartTime object?
    else if(!strcmp(object->name, "cfgTestStartTime"))
    {
        //Get object value
        value->integer = entry->cfgTestStartTime;
    }
    //cfgTestAgissonStartTest object?
    else if(!strcmp(object->name, "cfgTestAgissonStartTest"))
    {
        //Get object value
        value->integer = entry->cfgAgissonBTRespond;
    }
    //cfgTestAgissonStopTest object?
    else if(!strcmp(object->name, "cfgTestAgissonStopTest"))
    {
        //Get object value
        value->integer = entry->cfgAgissonBTRespond;
    }
    //Unknown object?
    else
    {
        //The specified object does not exist
        error = ERROR_OBJECT_NOT_FOUND;
    }

    //Return status code
    return error;
}
//========================================== CfgBTG Function ==========================================//
//========================================== ConfigInfo Function ==========================================//
/**
 * @brief Set ConfigGroup object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[in] value Object value
 * @param[in] valueLen Length of the object value, in bytes
 * @return Error code
 **/
error_t privateMibSetConfigGroup(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibConfigGroup *entry;

//	Point to the siteInfoGroup entry
    entry = &privateMibBase.configGroup;
    
    switch(TYPE)
    {
    case EMER:
      {
        //cfgWalkInTimeEn object?
        if(!strcmp(object->name, "cfgWalkInTimeEn"))
        {
          if((value->integer == 0) || (value->integer == 1))
          {
            //Get object value
            entry->cfgWalkInTimeEn = value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_WIT_EN;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgWalkInTimeDuration object?
        else if(!strcmp(object->name, "cfgWalkInTimeDuration"))
        {      
          if((value->integer >= 8) && (value->integer <= 128))
          {
            //Get object value
            entry->cfgWalkInTimeDuration= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_WIT_VAL;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgCurrentLimit object?
        else if(!strcmp(object->name, "cfgCurrentLimit"))
        {       
            if((value->integer >= 100) && (value->integer <= 250))
            {
              //Get object value
              entry->cfgCurrentLimit= value->integer;
              setCmd_flag = 1;
              setCmd_mask = SET_CCL;
            } 
            else
            {
              return ERROR_PARAMETER_OUT_OF_RANGE;
            }
          
        }
        //cfgFloatVolt object?
        else if(!strcmp(object->name, "cfgFloatVolt"))
        {      
          if((value->integer >= 4200) && (value->integer <= privateMibBase.configGroup.cfgBoostVolt))
          {
            //Get object value
            entry->cfgFloatVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_FLTVOL;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBoostVolt object?
        else if(!strcmp(object->name, "cfgBoostVolt"))
        {      
          if((value->integer >= privateMibBase.configGroup.cfgFloatVolt) && (value->integer <= 5800))
          {
            //Get object value
            entry->cfgBoostVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BSTVOL;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgLLVDVolt object?
        else if(!strcmp(object->name, "cfgLLVDVolt"))
        {      
          if((value->integer >= privateMibBase.configGroup.cfgBLVDVolt) && (value->integer <= privateMibBase.configGroup.cfgDCUnderVolt))
          {
            //Get object value
            entry->cfgLLVDVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_LLVD;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgDCLowVolt object?
        else if(!strcmp(object->name, "cfgDCLowVolt"))
        {      
          if((value->integer >= privateMibBase.configGroup.cfgDCUnderVolt) && (value->integer <= privateMibBase.configGroup.cfgDCOverVolt))
          {
            //Get object value
            entry->cfgDCLowVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_DCLOW;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgStartManualTest object?
        else if(!strcmp(object->name, "cfgStartManualTest"))
        {
            //Get object value
            entry->cfgStartManualTest= value->integer;
        }
        //cfgTempCompValue object?
        else if(!strcmp(object->name, "cfgTempCompValue"))
        {      
          if((value->integer >= 0) && (value->integer <= 500))
          {
            //Get object value
            entry->cfgTempCompValue= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_TEMPCOMP_VAL;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBLVDVolt object?
        else if(!strcmp(object->name, "cfgBLVDVolt"))
        {      
          if((value->integer >= 4000) && (value->integer <= privateMibBase.configGroup.cfgLLVDVolt))
          {
            //Get object value
            entry->cfgBLVDVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BLVD;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBattCapacityTotal object?
        else if(!strcmp(object->name, "cfgBattCapacityTotal"))
        {      
          if((value->integer >= 50) && (value->integer <= 5000))
          {
            //Get object value
            entry->cfgBattCapacityTotal= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BATTSTDCAP;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgHighMajorTempLevel object?
        else if(!strcmp(object->name, "cfgHighMajorTempLevel"))
        {      
          if((value->integer >= 1000) && (value->integer <= 10000))
          {
            //Get object value
            entry->cfgHighMajorTempLevel= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_OVERTEMP;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgAcLowLevel object?
        else if(!strcmp(object->name, "cfgAcLowLevel"))
        {      
          if((value->integer >= privateMibBase.configGroup.cfgAcUnderLevel) && (value->integer <= privateMibBase.configGroup.cfgAcHighLevel))
          {
            //Get object value
            entry->cfgAcLowLevel= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_AC_THRES;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBattCapacityTotal2 object?
        else if(!strcmp(object->name, "cfgBattCapacityTotal2"))
        {      
          if((value->integer >= 50) && (value->integer <= 5000))
          {
            //Get object value
            entry->cfgBattCapacityTotal2= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BATTSTDCAP2;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgDCUnderVolt object?
        else if(!strcmp(object->name, "cfgDCUnderVolt"))
        {      
          if((value->integer >= privateMibBase.configGroup.cfgLLVDVolt) && (value->integer <= privateMibBase.configGroup.cfgDCLowVolt))
          {
            //Get object value
            entry->cfgDCUnderVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_DCUNDER;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgLLVDEn object?
        else if(!strcmp(object->name, "cfgLLVDEn"))
        {
          if((value->integer == 0) || (value->integer == 1))
          {
            //Get object value
            entry->cfgLLVDEn = value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_LLVD_EN;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }   
        //Unknown object?
        else
        {
            TRACE_ERROR("ERROR_OBJECT_NOT_FOUND!\r\n");
            //The specified object does not exist
            return ERROR_OBJECT_NOT_FOUND;
        }

        
      }break;
    case ZTE:
      {
        //cfgWalkInTimeEn object?
        if(!strcmp(object->name, "cfgWalkInTimeEn"))
        {
          return ERROR_PARAMETER_OUT_OF_RANGE;          
        }
        //cfgWalkInTimeDuration object?
        else if(!strcmp(object->name, "cfgWalkInTimeDuration"))
        {
          return ERROR_PARAMETER_OUT_OF_RANGE;
        }
        //cfgCurrentLimit object?
        else if(!strcmp(object->name, "cfgCurrentLimit"))
        {      
          if((value->integer >= 10) && (value->integer <= 400))
          {
            //Get object value
            entry->cfgCurrentLimit= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_CCL;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgFloatVolt object?
        else if(!strcmp(object->name, "cfgFloatVolt"))
        {
          if((value->integer >= 4200) && (value->integer <= 5800))
          {  
            if((value->integer >= privateMibBase.configGroup.cfgDCLowVolt + 100) && (value->integer <= privateMibBase.configGroup.cfgBoostVolt))
            {           
              //Get object value
              entry->cfgFloatVolt= value->integer;
              setCmd_flag = 1;
              setCmd_mask = SET_FLTVOL;                        
            } 
            else
            {
              return ERROR_PARAMETER_OUT_OF_RANGE;
            }          
          }
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBoostVolt object?
        else if(!strcmp(object->name, "cfgBoostVolt"))
        {
          if((value->integer >= 4200) && (value->integer <= 5800))
          {      
            if(value->integer >= privateMibBase.configGroup.cfgFloatVolt)
            {
              //Get object value
              entry->cfgBoostVolt= value->integer;
              setCmd_flag = 1;
              setCmd_mask = SET_BSTVOL;
            } 
            else
            {
              return ERROR_PARAMETER_OUT_OF_RANGE;
            }         
          }
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgLLVDVolt object?
        else if(!strcmp(object->name, "cfgLLVDVolt"))
        {
          if((value->integer >= 3800) && (value->integer <= 4900))
          {
            if (privateMibBase.configGroup.cfgDCLowVolt <= privateMibBase.configGroup.cfgBattTestVolt)
            {
              if((value->integer >= privateMibBase.configGroup.cfgBLVDVolt) && (value->integer <= privateMibBase.configGroup.cfgDCLowVolt - 100))
              {
                //Get object value
                entry->cfgLLVDVolt= value->integer;
                setCmd_flag = 1;
                setCmd_mask = SET_LLVD;            
              } 
              else
              {
                return ERROR_PARAMETER_OUT_OF_RANGE;
              }
            }
            else
            {
              if((value->integer >= privateMibBase.configGroup.cfgBLVDVolt) && (value->integer <= privateMibBase.configGroup.cfgBattTestVolt - 100))
              {               
                //Get object value
                entry->cfgLLVDVolt= value->integer;
                setCmd_flag = 1;
                setCmd_mask = SET_LLVD;               
              } 
              else
              {
                return ERROR_PARAMETER_OUT_OF_RANGE;
              }
            }
          }
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }     
        }
        //cfgDCLowVolt object?
        else if(!strcmp(object->name, "cfgDCLowVolt"))
        {
          if (privateMibBase.configGroup.cfgFloatVolt <= 5300)
          {            
            if((value->integer >= privateMibBase.configGroup.cfgLLVDVolt + 100) && (value->integer <= privateMibBase.configGroup.cfgFloatVolt - 100))
            {
              //Get object value
              entry->cfgDCLowVolt= value->integer;
              setCmd_flag = 1;
              setCmd_mask = SET_DCLOW;
            } 
            else
            {
              return ERROR_PARAMETER_OUT_OF_RANGE;
            }
          }
          else 
          {            
            if((value->integer >= privateMibBase.configGroup.cfgLLVDVolt + 100) && (value->integer <= 5200))
            {
              //Get object value
              entry->cfgDCLowVolt= value->integer;
              setCmd_flag = 1;
              setCmd_mask = SET_DCLOW;
            } 
            else
            {
              return ERROR_PARAMETER_OUT_OF_RANGE;
            }
          }
        }
        //cfgStartManualTest object?
        else if(!strcmp(object->name, "cfgStartManualTest"))
        {
            return ERROR_PARAMETER_OUT_OF_RANGE;
            //Get object value
//            entry->cfgStartManualTest= value->integer;
        }
        //cfgTempCompValue object?
        else if(!strcmp(object->name, "cfgTempCompValue"))
        {      
          if((value->integer >= 0) && (value->integer <= 8))
          {
            //Get object value
            entry->cfgTempCompValue= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_TEMPCOMP_VAL;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBLVDVolt object?
        else if(!strcmp(object->name, "cfgBLVDVolt"))
        {      
          if((value->integer >= 3800) && (value->integer <= privateMibBase.configGroup.cfgLLVDVolt))
          {
            //Get object value
            entry->cfgBLVDVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BLVD;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBattCapacityTotal object?
        else if(!strcmp(object->name, "cfgBattCapacityTotal"))
        {      
          if((value->integer >= 0) && (value->integer <= 9990))
          {
            //Get object value
            entry->cfgBattCapacityTotal= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BATTSTDCAP;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgHighMajorTempLevel object?
        else if(!strcmp(object->name, "cfgHighMajorTempLevel"))
        {      
          if((value->integer >= 3000) && (value->integer <= 6000))
          {
            //Get object value
            entry->cfgHighMajorTempLevel= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_OVERTEMP;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgAcLowLevel object?
        else if(!strcmp(object->name, "cfgAcLowLevel"))
        {      
          if((value->integer >= 80) && (value->integer <= 200))
          {
            //Get object value
            entry->cfgAcLowLevel= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_AC_THRES;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBattCapacityTotal2 object?
        else if(!strcmp(object->name, "cfgBattCapacityTotal2"))
        {      
          if((value->integer >= 0) && (value->integer <= 9990))
          {
            //Get object value
            entry->cfgBattCapacityTotal2= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BATTSTDCAP2;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgDCUnderVolt object?
        else if(!strcmp(object->name, "cfgDCUnderVolt"))
        {
          return ERROR_PARAMETER_OUT_OF_RANGE;
        }
        //cfgLLVDEn object?
        else if(!strcmp(object->name, "cfgLLVDEn"))
        {
          return ERROR_PARAMETER_OUT_OF_RANGE;
        }
        //cfgBLVDEn object?
        else if(!strcmp(object->name, "cfgBLVDEn"))
        {
          return ERROR_PARAMETER_OUT_OF_RANGE;
        }
        //Unknown object?
        else
        {
            TRACE_ERROR("ERROR_OBJECT_NOT_FOUND!\r\n");
            //The specified object does not exist
            return ERROR_OBJECT_NOT_FOUND;
        }
      }break;
    case HW:
      {
        //cfgWalkInTimeEn object?
        if(!strcmp(object->name, "cfgWalkInTimeEn"))
        {
          return ERROR_PARAMETER_OUT_OF_RANGE;
        }
        //cfgWalkInTimeDuration object?
        else if(!strcmp(object->name, "cfgWalkInTimeDuration"))
        {
          return ERROR_PARAMETER_OUT_OF_RANGE;
        }
        //cfgCurrentLimit object?
        else if(!strcmp(object->name, "cfgCurrentLimit"))
        {      
          if((value->integer >= 50) && (value->integer <= 250))
          {
            //Get object value
            entry->cfgCurrentLimit= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_CCL;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgFloatVolt object?
        else if(!strcmp(object->name, "cfgFloatVolt"))
        {      
          if((value->integer > privateMibBase.configGroup.cfgDCLowVolt + 200) && (value->integer < privateMibBase.configGroup.cfgBoostVolt))
          {
            //Get object value
            entry->cfgFloatVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_FLTVOL;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBoostVolt object?
        else if(!strcmp(object->name, "cfgBoostVolt"))
        {      
          if((value->integer > privateMibBase.configGroup.cfgFloatVolt) && (value->integer <= 5760))
          {
            //Get object value
            entry->cfgBoostVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BSTVOL;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgLLVDVolt object?
        else if(!strcmp(object->name, "cfgLLVDVolt"))
        {      
          if((value->integer > privateMibBase.configGroup.cfgBLVDVolt) && (value->integer <= privateMibBase.configGroup.cfgDCLowVolt))
          {
            //Get object value
            entry->cfgLLVDVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_LLVD;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgDCLowVolt object?
        else if(!strcmp(object->name, "cfgDCLowVolt"))
        {      
          if((value->integer > privateMibBase.configGroup.cfgLLVDVolt) && (value->integer < privateMibBase.configGroup.cfgFloatVolt - 200))
          {
            //Get object value
            entry->cfgDCLowVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_DCLOW;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgStartManualTest object?
        else if(!strcmp(object->name, "cfgStartManualTest"))
        {
            return ERROR_PARAMETER_OUT_OF_RANGE;
            //Get object value
//            entry->cfgStartManualTest= value->integer;
        }
        //cfgTempCompValue object?
        else if(!strcmp(object->name, "cfgTempCompValue"))
        {      
          if((value->integer >= 0) && (value->integer <= 500))
          {
            //Get object value
            entry->cfgTempCompValue= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_TEMPCOMP_VAL;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBLVDVolt object?
        else if(!strcmp(object->name, "cfgBLVDVolt"))
        {      
          if((value->integer >= 3500) && (value->integer < privateMibBase.configGroup.cfgLLVDVolt))
          {
            //Get object value
            entry->cfgBLVDVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BLVD;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBattCapacityTotal object?
        else if(!strcmp(object->name, "cfgBattCapacityTotal"))
        {      
          if((value->integer >= 5) && (value->integer <= 1000))
          {
            //Get object value
            entry->cfgBattCapacityTotal= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BATTSTDCAP;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgHighMajorTempLevel object?
        else if(!strcmp(object->name, "cfgHighMajorTempLevel"))
        {      
          if((value->integer > entry->cfgLowTempLevel) && (value->integer <= 10000))
          {
            //Get object value
            entry->cfgHighMajorTempLevel= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_OVERTEMP;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgAcLowLevel object?
        else if(!strcmp(object->name, "cfgAcLowLevel"))
        {      
          if((value->integer >= 60) && (value->integer <= 299))
          {
            //Get object value
            entry->cfgAcLowLevel= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_AC_THRES;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBattCapacityTotal2 object?
        else if(!strcmp(object->name, "cfgBattCapacityTotal2"))
        {      
          if((value->integer >= 5) && (value->integer <= 1000))
          {
            //Get object value
            entry->cfgBattCapacityTotal2= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BATTSTDCAP2;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgDCUnderVolt object?
        else if(!strcmp(object->name, "cfgDCUnderVolt"))
        {
            return ERROR_PARAMETER_OUT_OF_RANGE;
        }
        //cfgLLVDEn object?
        else if(!strcmp(object->name, "cfgLLVDEn"))
        {
          if((value->integer == 0) || (value->integer == 1))
          {
            //Get object value
            entry->cfgLLVDEn = value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_LLVD_EN;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBLVDEn object?
        else if(!strcmp(object->name, "cfgBLVDEn"))
        {
          if((value->integer == 0) || (value->integer == 1))
          {
            //Get object value
            entry->cfgBLVDEn = value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BLVD_EN;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //Unknown object?
        else
        {
            TRACE_ERROR("ERROR_OBJECT_NOT_FOUND!\r\n");
            //The specified object does not exist
            return ERROR_OBJECT_NOT_FOUND;
        }
      }break;
    case EMERV21:
      {     
     if(!strcmp(object->name, "cfgCurrentLimit"))
        {      
          if((value->integer >= 100) && (value->integer <= 250))
          {
            //Get object value
            entry->cfgCurrentLimit= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_CCL;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgFloatVolt object?
        else if(!strcmp(object->name, "cfgFloatVolt"))
        {      
          if((value->integer >= 4200) && (value->integer <= privateMibBase.configGroup.cfgBoostVolt))
          {
            //Get object value
            entry->cfgFloatVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_FLTVOL;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBoostVolt object?
        else if(!strcmp(object->name, "cfgBoostVolt"))
        {      
          if((value->integer >= privateMibBase.configGroup.cfgFloatVolt) && (value->integer <= 5800))
          {
            //Get object value
            entry->cfgBoostVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BSTVOL;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgLLVDVolt object?
        else if(!strcmp(object->name, "cfgLLVDVolt"))
        {      
          if((value->integer > privateMibBase.configGroup.cfgBLVDVolt) && (value->integer <= privateMibBase.configGroup.cfgDCLowVolt))
          {
            //Get object value
            entry->cfgLLVDVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_LLVD;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgDCLowVolt object?
        else if(!strcmp(object->name, "cfgDCLowVolt"))
        {      
          if((value->integer > privateMibBase.configGroup.cfgLLVDVolt) && (value->integer < privateMibBase.configGroup.cfgDCOverVolt))
          {
            //Get object value
            entry->cfgDCLowVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_DCLOW;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }//cfgTempCompValue object?
        else if(!strcmp(object->name, "cfgTempCompValue"))
        {      
          if((value->integer >= 0) && (value->integer <= 500))
          {
            //Get object value
            entry->cfgTempCompValue= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_TEMPCOMP_VAL;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBLVDVolt object?
        else if(!strcmp(object->name, "cfgBLVDVolt"))
        {      
          if((value->integer >= 4000) && (value->integer < privateMibBase.configGroup.cfgLLVDVolt))
          {
            //Get object value
            entry->cfgBLVDVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BLVD;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBattCapacityTotal object?
        else if(!strcmp(object->name, "cfgBattCapacityTotal"))
        {      
          if((value->integer >= 20) && (value->integer <= 5000))
          {
            //Get object value
            entry->cfgBattCapacityTotal= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BATTSTDCAP;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgHighMajorTempLevel object?
        else if(!strcmp(object->name, "cfgHighMajorTempLevel"))
        {      
          if((value->integer > 1000) && (value->integer <= 5000))
          {
            //Get object value
            entry->cfgHighMajorTempLevel= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_OVERTEMP;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgAcLowLevel object?
        else if(!strcmp(object->name, "cfgAcLowLevel"))
        {      
          if((value->integer >= 80) && (value->integer <= privateMibBase.configGroup.cfgAcHighLevel))
          {
            //Get object value
            entry->cfgAcLowLevel= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_AC_THRES;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
      //Unknown object?
        else
        {
            TRACE_ERROR("ERROR_OBJECT_NOT_FOUND!\r\n");
            //The specified object does not exist
            return ERROR_OBJECT_NOT_FOUND;
        }
      }break;
    case DKD51_BDP:
      {
        //cfgCurrentLimit dpc A object?
        if (!strcmp(object->name, "cfgCurrentLimit_dpc_A")){
          if(privateMibBase.siteGroup.siteControllerModel[0] == 4){
            if((value->integer >= 1000) && (value->integer <= 99900))
            {
              entry->cfgCurrentlimitA = value->integer;
              setCmd_flag = 1;
              setCmd_mask = SET_CCL;
            }
            else
            {
              return ERROR_PARAMETER_OUT_OF_RANGE;
            }
            
          }
          
        }
        //cfgFloatVolt object?
        else if(!strcmp(object->name, "cfgFloatVolt"))
        {      
          if((value->integer >= privateMibBase.configGroup.cfgDCLowVolt ) && (value->integer <= privateMibBase.configGroup.cfgBoostVolt))
          {
            //Get object value
            entry->cfgFloatVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_FLTVOL;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBoostVolt object?
        else if(!strcmp(object->name, "cfgBoostVolt"))
        {      
          if((value->integer >= privateMibBase.configGroup.cfgFloatVolt) && (value->integer <= privateMibBase.configGroup.cfgDCOverVolt))
          {
            //Get object value
            entry->cfgBoostVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BSTVOL;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgDCLowVolt object?
        else if(!strcmp(object->name, "cfgDCLowVolt"))
        {      
          if((value->integer >= 5000) && (value->integer <= privateMibBase.configGroup.cfgFloatVolt))
          {
            //Get object value
            entry->cfgDCLowVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_DCLOW;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }//cfgTempCompValue object?
        else if(!strcmp(object->name, "cfgTempCompValue"))
        {      
          if((value->integer >= 1) && (value->integer <= 120))
          {
            //Get object value
            entry->cfgTempCompValue= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_TEMPCOMP_VAL;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBattCapacityTotal object?
        else if(!strcmp(object->name, "cfgBattCapacityTotal"))
        {      
          if((value->integer >= 0) && (value->integer <= 600))
          {
            //Get object value
            entry->cfgBattCapacityTotal= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BATTSTDCAP;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBattCapacityTotal2 object?
        else if(!strcmp(object->name, "cfgBattCapacityTotal2"))
        {      
          if((value->integer >= 0) && (value->integer <= 600))
          {
            //Get object value
            entry->cfgBattCapacityTotal2= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BATTSTDCAP2;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBattCapacityTotal3 object?
        else if(!strcmp(object->name, "cfgBattCapacityTotal3"))
        {      
          if((value->integer >= 0) && (value->integer <= 600))
          {
            //Get object value
            entry->cfgBattCapacityTotal3= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BATTSTDCAP3;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBattCapacityTotal4 object?
        else if(!strcmp(object->name, "cfgBattCapacityTotal4"))
        {      
          if((value->integer >= 0) && (value->integer <= 600))
          {
            //Get object value
            entry->cfgBattCapacityTotal4= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BATTSTDCAP4;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgAcLowLevel object?
        else if(!strcmp(object->name, "cfgAcLowLevel"))
        {      
          if((value->integer >= 154) && (value->integer <= 230))
          {
            //Get object value
            entry->cfgAcLowLevel= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_AC_THRES;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
      //Unknown object?
        else
        {
            TRACE_ERROR("ERROR_OBJECT_NOT_FOUND!\r\n");
            //The specified object does not exist
            return ERROR_OBJECT_NOT_FOUND;
        }
      
      }break;
    case VERTIV_M830B: 
   {
        //cfgCurrentLimit object?
        if (!strcmp(object->name, "cfgCurrentLimit")){
          //if(privateMibBase.siteGroup.siteControllerModel[0] == 5){
            if((value->integer >= 5) && (value->integer <= 999))
            {
              entry->cfgCurrentLimit = value->integer;
              setCmd_flag = 1;
              setCmd_mask = SET_CCL;
            }
            else
            {
              return ERROR_PARAMETER_OUT_OF_RANGE;
            }
            
          //}
          
        }
        //cfgFloatVolt object?
        else if(!strcmp(object->name, "cfgFloatVolt"))
        {      
          if((value->integer >= privateMibBase.configGroup.cfgDCLowVolt ) && (value->integer <= privateMibBase.configGroup.cfgBoostVolt))
          {
            //Get object value
            entry->cfgFloatVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_FLTVOL;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBoostVolt object?
        else if(!strcmp(object->name, "cfgBoostVolt"))
        {      
          if((value->integer >= privateMibBase.configGroup.cfgFloatVolt) && (value->integer <= privateMibBase.configGroup.cfgDCOverVolt))
          {
            //Get object value
            entry->cfgBoostVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BSTVOL;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgDCLowVolt object?
        else if(!strcmp(object->name, "cfgDCLowVolt"))
        {      
          if((value->integer >= 4000) && (value->integer <= privateMibBase.configGroup.cfgFloatVolt))
          {
            //Get object value
            entry->cfgDCLowVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_DCLOW;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
       //cfgDCUnderVolt object?
        else if(!strcmp(object->name, "cfgDCUnderVolt"))
        {      
          if((value->integer >= 4000) && (value->integer <= privateMibBase.configGroup.cfgDCLowVolt))
          {
            //Get object value
            entry->cfgDCUnderVolt= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_DCUNDER;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }      
        //cfgTempCompValue object?
        else if(!strcmp(object->name, "cfgTempCompValue"))
        {      
          if((value->integer >= 1) && (value->integer <= 500))
          {
            //Get object value
            entry->cfgTempCompValue= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_TEMPCOMP_VAL;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgBattCapacityTotal object?
        else if(!strcmp(object->name, "cfgBattCapacityTotal"))
        {      
          if((value->integer >= 0) && (value->integer <= 600))
          {
            //Get object value
            entry->cfgBattCapacityTotal= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_BATTSTDCAP;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
//        //cfgBattCapacityTotal2 object?
//        else if(!strcmp(object->name, "cfgBattCapacityTotal2"))
//        {      
//          if((value->integer >= 0) && (value->integer <= 600))
//          {
//            //Get object value
//            entry->cfgBattCapacityTotal2= value->integer;
//            setCmd_flag = 1;
//            setCmd_mask = SET_BATTSTDCAP2;
//          } 
//          else
//          {
//            return ERROR_PARAMETER_OUT_OF_RANGE;
//          }
//        }
        //cfgAcLowLevel object?
//        else if(!strcmp(object->name, "cfgAcLowLevel"))
//        {      
//          if((value->integer >= 154) && (value->integer <= 230))
//          {
//            //Get object value
//            entry->cfgAcLowLevel= value->integer;
//            setCmd_flag = 1;
//            setCmd_mask = SET_AC_THRES;
//          } 
//          else
//          {
//            return ERROR_PARAMETER_OUT_OF_RANGE;
//          }
//        }
        //cfgWalkInTimeEn object?
        else if(!strcmp(object->name, "cfgWalkInTimeEn"))
        {
          if((value->integer == 0) || (value->integer == 1))
          {
            //Get object value
            entry->cfgWalkInTimeEn = value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_WIT_EN;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }
        //cfgWalkInTimeDuration object?
        else if(!strcmp(object->name, "cfgWalkInTimeDuration"))
        {      
          if((value->integer >= 8) && (value->integer <= 128))
          {
            //Get object value
            entry->cfgWalkInTimeDuration= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_WIT_VAL;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }        
//        //cfgBLVDVolt object?
//        else if(!strcmp(object->name, "cfgBLVDVolt"))
//        {      
//          if((value->integer >= 4000) && (value->integer <= privateMibBase.configGroup.cfgLLVDVolt))
//          {
//            //Get object value
//            entry->cfgBLVDVolt= value->integer;
//            setCmd_flag = 1;
//            setCmd_mask = SET_BLVD;
//          } 
//          else
//          {
//            return ERROR_PARAMETER_OUT_OF_RANGE;
//          }
//        }
//        //cfgLLVDVolt object?
//        else if(!strcmp(object->name, "cfgLLVDVolt"))
//        {      
//          if((value->integer >= privateMibBase.configGroup.cfgBLVDVolt) && (value->integer <= privateMibBase.configGroup.cfgDCUnderVolt))
//          {
//            //Get object value
//            entry->cfgLLVDVolt= value->integer;
//            setCmd_flag = 1;
//            setCmd_mask = SET_LLVD;
//          } 
//          else
//          {
//            return ERROR_PARAMETER_OUT_OF_RANGE;
//          }
//        }        
        //cfgHighMajorTempLevel object?
        else if(!strcmp(object->name, "cfgHighMajorTempLevel"))
        {      
          if((value->integer >= 1000) && (value->integer <= 10000))
          {
            //Get object value
            entry->cfgHighMajorTempLevel= value->integer;
            setCmd_flag = 1;
            setCmd_mask = SET_OVERTEMP;
          } 
          else
          {
            return ERROR_PARAMETER_OUT_OF_RANGE;
          }
        }        
        //Unknown object?
        else
        {
            TRACE_ERROR("ERROR_OBJECT_NOT_FOUND!\r\n");
            //The specified object does not exist
            return ERROR_OBJECT_NOT_FOUND;
        }      
      }break;
    default:
      break;
    };    

    //Successful processing
    return NO_ERROR;
}

/**
 * @brief Get ConfigGroup object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t privateMibGetConfigGroup(const MibObject *object, const uint8_t *oid,
                                     size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error = NO_ERROR;
    size_t n;
    PrivateMibConfigGroup *entry;
    entry = &privateMibBase.configGroup;

    //cfgWalkInTimeEn object?
    if(!strcmp(object->name, "cfgWalkInTimeEn"))
    {
        //Get object value
        value->integer = entry->cfgWalkInTimeEn;
    }
    //cfgWalkInTimeDuration object?
    else if(!strcmp(object->name, "cfgWalkInTimeDuration"))
    {
        //Get object value
        value->integer = entry->cfgWalkInTimeDuration;
    }
    //cfgCurrentLimit object?
    else if(!strcmp(object->name, "cfgCurrentLimit"))
    {
        //Get object value
        value->integer = entry->cfgCurrentLimit;
    }
    //cfgFloatVolt object?
    else if(!strcmp(object->name, "cfgFloatVolt"))
    {
        //Get object value
        value->integer = entry->cfgFloatVolt;
    }
    //cfgBoostVolt object?
    else if(!strcmp(object->name, "cfgBoostVolt"))
    {
        //Get object value
        value->integer = entry->cfgBoostVolt;
    }
    //cfgLLVDVolt object?
    else if(!strcmp(object->name, "cfgLLVDVolt"))
    {
        //Get object value
        value->integer = entry->cfgLLVDVolt;
    }
    //cfgDCLowVolt object?
    else if(!strcmp(object->name, "cfgDCLowVolt"))
    {
        //Get object value
        value->integer = entry->cfgDCLowVolt;
    }
    //cfgStartManualTest object?
    else if(!strcmp(object->name, "cfgStartManualTest"))
    {
        //Get object value
        value->integer = entry->cfgStartManualTest;
    }
    //cfgTempCompValue object?
    else if(!strcmp(object->name, "cfgTempCompValue"))
    {
        //Get object value
        value->integer = entry->cfgTempCompValue;
    }
    //cfgBLVDVolt object?
    else if(!strcmp(object->name, "cfgBLVDVolt"))
    {
        //Get object value
        value->integer = entry->cfgBLVDVolt;
    }
    //cfgBattCapacityTotal object?
    else if(!strcmp(object->name, "cfgBattCapacityTotal"))
    {
        //Get object value
        value->integer = entry->cfgBattCapacityTotal;
    }
    //cfgHighMajorTempLevel object?
    else if(!strcmp(object->name, "cfgHighMajorTempLevel"))
    {
        //Get object value
        value->integer = entry->cfgHighMajorTempLevel;
    }
    //cfgAcLowLevel object?
    else if(!strcmp(object->name, "cfgAcLowLevel"))
    {
        //Get object value
        value->integer = entry->cfgAcLowLevel;
    }
    //cfgBattCapacityTotal2 object?
    else if(!strcmp(object->name, "cfgBattCapacityTotal2"))
    {
        //Get object value
        value->integer = entry->cfgBattCapacityTotal2;
    }
    //cfgDCUnderVolt object?
    else if(!strcmp(object->name, "cfgDCUnderVolt"))
    {
        //Get object value
        value->integer = entry->cfgDCUnderVolt;
    }
    //cfgLLVDEn object?
    else if(!strcmp(object->name, "cfgLLVDEn"))
    {
        //Get object value
        value->integer = entry->cfgLLVDEn;
    }
    //cfgBLVDEn object?
    else if(!strcmp(object->name, "cfgBLVDEn"))
    {
        //Get object value
        value->integer = entry->cfgBLVDEn;
    }
    //current limit dpc A object?
    else if(!strcmp(object->name, "cfgCurrentLimit_dpc_A"))
    {
        //Get object value
        value->integer = entry->cfgCurrentlimitA;
    }
    //cfgBattCapacityTotal3 object?
    else if(!strcmp(object->name, "cfgBattCapacityTotal3"))
    {
        //Get object value
        value->integer = entry->cfgBattCapacityTotal3;
    }
    //cfgBattCapacityTotal4 object?
    else if(!strcmp(object->name, "cfgBattCapacityTotal4"))
    {
        //Get object value
        value->integer = entry->cfgBattCapacityTotal4;
    }
    //Unknown object?
    else
    {
        //The specified object does not exist
        error = ERROR_OBJECT_NOT_FOUND;
    }

    //Return status code
    return error;
}
//========================================== ConfigInfo Function ==========================================//
//========================================== MainAlarmInfo Function ==========================================//

/**
 * @brief Get MainAlarmGroup object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t privateMibGetMainAlarmGroup(const MibObject *object, const uint8_t *oid,
                                size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error = NO_ERROR;
    size_t n;
    PrivateMibMainAlarmGroup *entry;
    entry = &privateMibBase.mainAlarmGroup;
    //alarmBattHighTemp object?
    if(!strcmp(object->name, "alarmBattHighTemp"))
    {
        //Get object value
        value->integer = entry->alarmBattHighTemp;
    }
    //alarmLLVD object?
    else if(!strcmp(object->name, "alarmLLVD"))
    {
        //Get object value
        value->integer = entry->alarmLLVD;
    }
    //alarmBLVD object?
    else if(!strcmp(object->name, "alarmBLVD"))
    {
        //Get object value
        value->integer = entry->alarmBLVD;
    }
    //alarmDCLow object?
    else if(!strcmp(object->name, "alarmDCLow"))
    {
        //Get object value
        value->integer = entry->alarmDCLow;
    }
    //alarmACmains object?
    else if(!strcmp(object->name, "alarmACmains"))
    {
        //Get object value
        value->integer = entry->alarmACmains;
    }
    //alarmBattBreakerOpen object?
    else if(!strcmp(object->name, "alarmBattBreakerOpen"))
    {
        //Get object value
        value->integer = entry->alarmBattBreakerOpen;
    }
    //alarmLoadBreakerOpen object?
    else if(!strcmp(object->name, "alarmLoadBreakerOpen"))
    {
        //Get object value
        value->integer = entry->alarmLoadBreakerOpen;
    }
    //alarmRectACFault object?
    else if(!strcmp(object->name, "alarmRectACFault"))
    {
        //Get object value
        value->integer = entry->alarmRectACFault;
    }
    //alarmRectNoResp object?
    else if(!strcmp(object->name, "alarmRectNoResp"))
    {
        //Get object value
        value->integer = entry->alarmRectNoResp;
    }
    //alarmManualMode object?
    else if(!strcmp(object->name, "alarmManualMode"))
    {
        //Get object value
        value->integer = entry->alarmManualMode;
    }
    //alarmACLow object?
    else if(!strcmp(object->name, "alarmACLow"))
    {
        //Get object value
        value->integer = entry->alarmACLow;
    }
    //alarmSmoke object?
    else if(!strcmp(object->name, "alarmSmoke"))
    {
        //Get object value
        value->integer = entry->alarmSmoke;
    }
    //alarmEnvDoor object?
    else if(!strcmp(object->name, "alarmEnvDoor"))
    {
        //Get object value
        value->integer = entry->alarmEnvDoor;
    }
    //alarmLIB object?
    else if(!strcmp(object->name, "alarmLIB"))
    {
        //Get object value
        value->integer = entry->alarmLIB;
    }
    //alarmGen object?
    else if(!strcmp(object->name, "alarmGen"))
    {
        //Get object value
        value->integer = entry->alarmGen;
    }
    //alarmBM object?
    else if(!strcmp(object->name, "alarmBM"))
    {
        //Get object value
        value->integer = entry->alarmBM;
    }
    //alarmVAC object?
    else if(!strcmp(object->name, "alarmVAC"))
    {
        //Get object value
        value->integer = entry->alarmVAC;
    }
    else
    {
        //The specified object does not exist
        error = ERROR_OBJECT_NOT_FOUND;
    }

    //Return status code
    return error;
}
//========================================== MainAlarmInfo Function ==========================================//
//========================================== DIAlarmInfo Function ==========================================//

/**
 * @brief Get DIAlarmGroup object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t privateMibGetDIAlarmGroup(const MibObject *object, const uint8_t *oid,
                                size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error = NO_ERROR;
    size_t n;
    PrivateMibDIAlarmGroup *entry;
    entry = &privateMibBase.diAlarmGroup;
    //alarmDI1 object?
    if(!strcmp(object->name, "alarmDI1"))
    {
        //Get object value
        value->integer = entry->alarmDigitalInput[0];
    }
    //alarmDI2 object?
    else if(!strcmp(object->name, "alarmDI2"))
    {
        //Get object value
        value->integer = entry->alarmDigitalInput[1];
    }
    //alarmDI3 object?
    else if(!strcmp(object->name, "alarmDI3"))
    {
        //Get object value
        value->integer = entry->alarmDigitalInput[2];
    }
    //alarmDI4 object?
    else if(!strcmp(object->name, "alarmDI4"))
    {
        //Get object value
        value->integer = entry->alarmDigitalInput[3];
    }
    //alarmDI5 object?
    else if(!strcmp(object->name, "alarmDI5"))
    {
        //Get object value
        value->integer = entry->alarmDigitalInput[4];
    }
    //alarmDI6 object?
    else if(!strcmp(object->name, "alarmDI6"))
    {
        //Get object value
        value->integer = entry->alarmDigitalInput[5];
    }
    //alarmDI7 object?
    else if(!strcmp(object->name, "alarmDI7"))
    {
        //Get object value
        value->integer = entry->alarmDigitalInput[6];
    }
    //alarmDI8 object?
    else if(!strcmp(object->name, "alarmDI8"))
    {
        //Get object value
        value->integer = entry->alarmDigitalInput[7];
    }
    //Unknown object?
    else
    {
        //The specified object does not exist
        error = ERROR_OBJECT_NOT_FOUND;
    }

    //Return status code
    return error;
}
//========================================== DIAlarmInfo Function ==========================================//
//========================================== ConnAlarmInfo Function ==========================================//

/**
 * @brief Get ConnAlarmGroup object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t privateMibGetConnAlarmGroup(const MibObject *object, const uint8_t *oid,
                                size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error = NO_ERROR;
    size_t n;
    PrivateMibConnAlarmGroup *entry;
    entry = &privateMibBase.connAlarmGroup;
    //alarmLIBConnect object?
    if(!strcmp(object->name, "alarmLIBConnect"))
    {
        //Get object value
        value->integer = entry->alarmLIBConnect;
    }
    //alarmPMUConnect object?
    else if(!strcmp(object->name, "alarmPMUConnect"))
    {
        //Get object value
        value->integer = entry->alarmPMUConnect;
    }
    //alarmGenConnect object?
    else if(!strcmp(object->name, "alarmGenConnect"))
    {
        //Get object value
        value->integer = entry->alarmGenConnect;
    }
    //alarmBMConnect object?
    else if(!strcmp(object->name, "alarmBMConnect"))
    {
        //Get object value
        value->integer = entry->alarmBMConnect;
    }
    //alarmPMConnect object?
    else if(!strcmp(object->name, "alarmPMConnect"))
    {
        //Get object value
        value->integer = entry->alarmPMConnect;
    }
    //alarmVACConnect object?
    else if(!strcmp(object->name, "alarmVACConnect"))
    {
        //Get object value
        value->integer = entry->alarmVACConnect;
    }
    //alarmSMCBConnect object?
    else if(!strcmp(object->name, "alarmSMCBConnect"))
    {
        //Get object value
        value->integer = entry->alarmSMCBConnect;
    }
    //alarmFUELConnect object?
    else if(!strcmp(object->name, "alarmFUELConnect"))
    {
        //Get object value
        value->integer = entry->alarmFUELConnect;
    }
    //alarmIsenseConnect object?
    else if(!strcmp(object->name, "alarmISENSEConnect"))
    {
        //Get object value
        value->integer = entry->alarmISENSEConnect;
    }
    //alarm_pm_dc_Connect object?
    else if(!strcmp(object->name, "alarm_pm_dc_Connect")){
        //Get object value
        value->integer = entry->alarm_pm_dc_connect;
    }
    //alarm_dpc_fan_Connect object?
    else if(!strcmp(object->name, "alarm_dpc_fan_Connect")){
        //Get object value
        value->integer = entry->alarm_dpc_fan_connect;
    }
    //Unknown object?
    else
    {
        //The specified object does not exist
        error = ERROR_OBJECT_NOT_FOUND;
    }

    //Return status code
    return error;
}
//========================================== ConnAlarmInfo Function ==========================================//


//void UpdateInfo (void)
//{
//    uint8_t i,j;
//    privateMibBase.acPhaseGroup.acPhaseTable[0].acPhaseVolt = sATS_Variable.vRMS_u32/10;
//    privateMibBase.acPhaseGroup.acPhaseTable[0].acPhaseCurrent = sATS_Variable.iRMSA_u32/10;
//    privateMibBase.acPhaseGroup.acPhaseTable[0].acPhasePower = sATS_Variable.instan_ActPwA_i32/10;
//    privateMibBase.acPhaseGroup.acPhaseTable[0].acPhaseIndex = 1;
//    privateMibBase.acPhaseGroup.acPhaseTable[0].acPhaseFrequency = sATS_Variable.frequency_i16/10;
////  privateMibBase.acPhaseGroup.acPhaseTable[0].acPhaseThresVolt = sMenu_Variable.u16AcThresVolt[0];
//    if( sATS_Variable.vRMS_u32/100 < sMenu_Variable.u16AcThresVolt[0])
//    {
//        privateMibBase.acPhaseGroup.acPhaseTable[0].acPhaseAlarmStatus = 1;
//        privateMibBase.alarmGroup.alarmAcThresAlarms = 1;
//        sActive_Alarm[0].status = 1;
//    }
//    else
//    {
//        privateMibBase.acPhaseGroup.acPhaseTable[0].acPhaseAlarmStatus = 0;
//        privateMibBase.alarmGroup.alarmAcThresAlarms = 0;
//        sActive_Alarm[0].status = 0;
//    }
//
//    privateMibBase.batteryGroup.battery1Voltage = sATS_Variable.battVolt;
//    privateMibBase.batteryGroup.battery1AlarmStatus = sATS_Variable.batt_DCLow;
//    privateMibBase.batteryGroup.battery1ThresVolt = sMenu_Variable.u16BattThresVolt[0];
//
//    if (sATS_Variable.Gen1Fail == 1)
//    {
//        privateMibBase.alarmGroup.alarmGenFailureAlarms = 1;
//        sActive_Alarm[12].status = 1;
//    }
//    else
//    {
//        privateMibBase.alarmGroup.alarmGenFailureAlarms = 0;
//        sActive_Alarm[12].status = 0;
//    }
////  if (sATS_Variable.batt_DCLow)
////  {
////  privateMibBase.alarmGroup.alarmDcThresAlarms = 1;
////  sActive_Alarm[1].status = 1;
////  } else
////  {
////  privateMibBase.alarmGroup.alarmDcThresAlarms = 0;
////  sActive_Alarm[1].status = 0;
////  }
//    if (sATS_Variable.Gen1Voltage == 0) //0:Stop; 1:Run
//    {
//        privateMibBase.alarmGroup.alarmMachineStopAlarms = 1;
//        sActive_Alarm[14].status = 1;
//    }
//    else if (sATS_Variable.Gen1Voltage == 1)
//    {
//        privateMibBase.alarmGroup.alarmMachineStopAlarms = 0;
//        sActive_Alarm[14].status = 0;
//    }
//
//    privateMibBase.siteInfoGroup.siteInfoThresTemp1 = sMenu_Variable.u16ThresTemp[0];
//    privateMibBase.siteInfoGroup.siteInfoThresTemp2 = sMenu_Variable.u16ThresTemp[1];
//    privateMibBase.siteInfoGroup.siteInfoThresTemp3 = sMenu_Variable.u16ThresTemp[2];
//    privateMibBase.siteInfoGroup.siteInfoThresTemp4 = sMenu_Variable.u16ThresTemp[3];
//    privateMibBase.siteInfoGroup.siteInfoMeasuredTemp = sAirCon_Variable.indorTemp;
//    privateMibBase.siteInfoGroup.siteInfoMeasuredHumid = 80;
//
//    privateMibBase.accessoriesGroup.airCon1Status = sAirCon_Variable.airCon1Status; //1: Run/On; 0: Stop/Off
//    privateMibBase.accessoriesGroup.airCon2Status = sAirCon_Variable.airCon2Status; //1: Run/On; 0: Stop/Off
//    privateMibBase.accessoriesGroup.fan1Status = sAirCon_Variable.fanStatus; //1: Run/On; 0: Stop/Off
//    privateMibBase.accessoriesGroup.airConSetTemp1 = sMenu_Variable.u16AirConTemp[0];
//    privateMibBase.accessoriesGroup.airConSetTemp2 = sMenu_Variable.u16AirConTemp[1];
//    privateMibBase.accessoriesGroup.airConSetTemp3 = sMenu_Variable.u16AirConTemp[2];
//    privateMibBase.accessoriesGroup.airConSetTemp4 = sMenu_Variable.u16AirConTemp[3];
//
//    // Fire Alarm
//    if (DigitalInput[1] == 1)
//    {
//        privateMibBase.alarmGroup.alarmFireAlarms = 1;
//        sActive_Alarm[2].status = 1;
//        sActive_Alarm[3].status = 0;
//    }
//    else if (DigitalInput[1] == 2)
//    {
//        privateMibBase.alarmGroup.alarmFireAlarms = 2;
//        sActive_Alarm[2].status = 0;
//        sActive_Alarm[3].status = 1;
//    }
//    else
//    {
//        privateMibBase.alarmGroup.alarmFireAlarms = 0;
//        sActive_Alarm[2].status = 0;
//        sActive_Alarm[3].status = 0;
//    }
//    // Smoke Alarm
//    if (DigitalInput[2] == 1)
//    {
//        privateMibBase.alarmGroup.alarmSmokeAlarms = 1;
//        sActive_Alarm[4].status = 1;
//        sActive_Alarm[5].status = 0;
//    }
//    else if(DigitalInput[2] == 2)
//    {
//        privateMibBase.alarmGroup.alarmSmokeAlarms = 2;
//        sActive_Alarm[4].status = 0;
//        sActive_Alarm[5].status = 1;
//    }
//    else
//    {
//        privateMibBase.alarmGroup.alarmSmokeAlarms = 0;
//        sActive_Alarm[4].status = 0;
//        sActive_Alarm[5].status = 0;
//    }
//    // Move Alarm
//    if (DigitalInput[3] == 1)
//    {
//        privateMibBase.alarmGroup.alarmMotionDetectAlarms = 1;
//        sActive_Alarm[6].status = 1;
//        sActive_Alarm[7].status = 0;
//    }
//    else if(DigitalInput[3] == 2)
//    {
//        privateMibBase.alarmGroup.alarmMotionDetectAlarms = 2;
//        sActive_Alarm[6].status = 0;
//        sActive_Alarm[7].status = 1;
//    }
//    else
//    {
//        privateMibBase.alarmGroup.alarmMotionDetectAlarms = 0;
//        sActive_Alarm[6].status = 0;
//        sActive_Alarm[7].status = 0;
//    }
//    // FloodDetectAlarm
//    if (DigitalInput[4] == 1)
//    {
//        privateMibBase.alarmGroup.alarmFloodDetectAlarms = 1;
//        sActive_Alarm[8].status = 1;
//        sActive_Alarm[9].status = 0;
//    }
//    else if(DigitalInput[4] == 2)
//    {
//        privateMibBase.alarmGroup.alarmFloodDetectAlarms = 2;
//        sActive_Alarm[8].status = 0;
//        sActive_Alarm[9].status = 1;
//    }
//    else
//    {
//        privateMibBase.alarmGroup.alarmFloodDetectAlarms = 0;
//        sActive_Alarm[8].status = 0;
//        sActive_Alarm[9].status = 0;
//    }
//    // DoorOpenAlarm
//    if (DigitalInput[5] == 1)
//    {
//        privateMibBase.alarmGroup.alarmDoorOpenAlarms = 1;
//        sActive_Alarm[10].status = 1;
//        sActive_Alarm[11].status = 0;
//    }
//    else if(DigitalInput[5] == 2)
//    {
//        privateMibBase.alarmGroup.alarmDoorOpenAlarms = 2;
//        sActive_Alarm[10].status = 0;
//        sActive_Alarm[11].status = 1;
//    }
//    else
//    {
//        privateMibBase.alarmGroup.alarmDoorOpenAlarms = 0;
//        sActive_Alarm[10].status = 0;
//        sActive_Alarm[11].status = 0;
//    }
//
//    if(sMenu_Control.accessUID == -1)
//    {
//        privateMibBase.alarmGroup.alarmAccessAlarms = 1;
////    IO_OPENDOOR_MCU_ON();
//    }
//    else
//    {
//        privateMibBase.alarmGroup.alarmAccessAlarms = 0;
////    IO_OPENDOOR_MCU_OFF();
//    }
//    for (j=0; j<5; j++)
//    {
//        privateMibBase.configGroup.configAccessIdTable[j].configAccessIdIndex = j+1;
//        privateMibBase.configGroup.configAccessIdTable[j].configAccessIdCardLen = 8;
//        memset(&privateMibBase.configGroup.configAccessIdTable[j].configAccessIdCard,0,16);
//        for(i=0; i<8; i++)
//            privateMibBase.configGroup.configAccessIdTable[j].configAccessIdCard[i] = sMenu_Variable.u8UserID[j][i];
//    }
//    for(i=0; i<8; i++)
//    {
//        privateMibBase.siteInfoGroup.siteInfoAccessId[i] = AccessIdTemp[i];
//        privateMibBase.siteInfoGroup.siteInfoAccessIdLen = 8;
//    }
//
//    Alarm_Control();
//    Relay_Output();
//}

void Alarm_Control(void)
{
//    if((privateMibBase.alarmGroup.alarmFireAlarms != 0)  ||
//            (privateMibBase.alarmGroup.alarmSmokeAlarms != 0)  ||
//            (privateMibBase.alarmGroup.alarmMotionDetectAlarms != 0)  ||
//            (privateMibBase.alarmGroup.alarmFloodDetectAlarms != 0)  ||
//            (privateMibBase.alarmGroup.alarmDoorOpenAlarms != 0)  ||
//            (privateMibBase.alarmGroup.alarmGenFailureAlarms != 0)  ||
//            (privateMibBase.alarmGroup.alarmDcThresAlarms != 0)  ||
//            (privateMibBase.alarmGroup.alarmMachineStopAlarms != 0)  ||
//            (privateMibBase.alarmGroup.alarmAccessAlarms != 0)  ||
//            (privateMibBase.alarmGroup.alarmAcThresAlarms != 0)  )
//    {
//        privateMibBase.accessoriesGroup.ledControlStatus = 1;
//        privateMibBase.accessoriesGroup.speakerControlStatus = 1;
//    }
//    else
//    {
//        privateMibBase.accessoriesGroup.ledControlStatus = 0;
//        privateMibBase.accessoriesGroup.speakerControlStatus = 0;
//    }
}

void Relay_Output(void)
{
//    if (privateMibBase.accessoriesGroup.ledControlStatus == 1)
//    {
////    IO_ALARM_LIGHT_ON();
//    }
//    else if (privateMibBase.accessoriesGroup.ledControlStatus == 0)
//    {
////     IO_ALARM_LIGHT_OFF();
//    }
//
//    if (privateMibBase.accessoriesGroup.speakerControlStatus == 1)
//    {
////    IO_ALARM_SPEAKER_ON();
//    }
//    else if (privateMibBase.accessoriesGroup.speakerControlStatus == 0)
//    {
////     IO_ALARM_SPEAKER_OFF();
//    }
}


//========================================== ISENSEInfo Function ==========================================//
error_t privateMibSetISENSEGroup(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibISENSEGroup *entry;
    
    //	Point to the liBattGroup entry
    entry = &privateMibBase.isenseGroup;
    
    if(!strcmp(object->name, "isenseInstalledISENSE"))
    {
      if((value->integer >= 0) && (value->integer <= 1))
      {
          //Get object value
          entry->isenseInstalledISENSE = value->integer; 
          if (entry->isenseInstalledISENSE != sModbusManager.u8NumberOfISENSE)
          {
              sModbusManager.u8NumberOfISENSE = entry->isenseInstalledISENSE;
              g_sParameters.u8DevNum[TOTAL_ISENSE_INDEX] = sModbusManager.u8NumberOfISENSE;
              g_sWorkingDefaultParameters.u8DevNum[TOTAL_ISENSE_INDEX] = g_sParameters.u8DevNum[TOTAL_ISENSE_INDEX];
              u8SaveConfigFlag |= 1;
          }
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    else
    {
        TRACE_ERROR("ERROR_OBJECT_NOT_FOUND!\r\n");
        //The specified object does not exist
        return ERROR_OBJECT_NOT_FOUND;
    }
    
    //Successful processing
    return NO_ERROR; 
}

error_t privateMibSetISENSEInfoEntry(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    uint_t j;
    tModbusParameters sModbusDev[3];
    PrivateMibISENSEInfoEntry *entry;

    //Point to the instance identifier
    n = object->oidLen;

    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index != 1)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the libTable entry
    entry = &privateMibBase.isenseGroup.isenseTable;
    //isenseType object?
    if(!strcmp(object->name, "isenseType"))
    {
      if((value->integer >= 0) && (value->integer <= 15))
      {
        //Set object value
        entry->isenseType= value->integer;
        j = index;
        sModbusManager.sISENSEManager[j-1].u8ISENSEType = entry->isenseType;
        
        switch(sModbusManager.sISENSEManager[j-1].u8ISENSEType)
        {
        case 1:// FORLONG
        {  
            sModbusManager.sISENSEManager[j-1].u32ISENSESlaveOffset = 26;  
            sModbusManager.sISENSEManager[j-1].u32ISENSEAbsSlaveID = 26+j;
            sModbusManager.sISENSEManager[j-1].u32ISENSEBaseAddr[0] = 0;
            sModbusManager.sISENSEManager[j-1].u8ISENSENumOfReg[0] = 22;                                
        }
        break;
        case 2: // IVY_DDS353H_2
        {
            sModbusManager.sISENSEManager[j-1].u32ISENSESlaveOffset  = 26;
            sModbusManager.sISENSEManager[j-1].u32ISENSEAbsSlaveID   = 26+j;
                    
            sModbusManager.sISENSEManager[j-1].u32ISENSEBaseAddr[0]  = 256;             
            sModbusManager.sISENSEManager[j-1].u8ISENSENumOfReg[0]   = 16;  
                    
            sModbusManager.sISENSEManager[j-1].u32ISENSEBaseAddr[1]  = 290;            
            sModbusManager.sISENSEManager[j-1].u8ISENSENumOfReg[1]   = 36; 
                    
            sModbusManager.sISENSEManager[j-1].u32ISENSEBaseAddr[2]  = 4096;             
            sModbusManager.sISENSEManager[j-1].u8ISENSENumOfReg[2]   = 6; 
                    
//            sModbusManager.sISENSEManager[j-1].u32ISENSEBaseAddr[3]  = 328;             // Q
//            sModbusManager.sISENSEManager[j-1].u8ISENSENumOfReg[3]   = 2;                
//                   
//            sModbusManager.sISENSEManager[j-1].u32ISENSEBaseAddr[4]  = 336;             // S
//            sModbusManager.sISENSEManager[j-1].u8ISENSENumOfReg[4]   = 2;  
//                    
//            sModbusManager.sISENSEManager[j-1].u32ISENSEBaseAddr[5]  = 344;             // Factor
//            sModbusManager.sISENSEManager[j-1].u8ISENSENumOfReg[5]   = 1; 
//
//            sModbusManager.sISENSEManager[j-1].u32ISENSEBaseAddr[6]  = 40960;           // PE
//            sModbusManager.sISENSEManager[j-1].u8ISENSENumOfReg[6]   = 2;  
//                    
//            sModbusManager.sISENSEManager[j-1].u32ISENSEBaseAddr[7]  = 40990;           // QE
//            sModbusManager.sISENSEManager[j-1].u8ISENSENumOfReg[7]   = 2;
//            
//            sModbusManager.sISENSEManager[j-1].u32ISENSEBaseAddr[8]  = 277;   //SN
//            sModbusManager.sISENSEManager[j-1].u8ISENSENumOfReg[8]   = 3;
        }
        break;
        };
        sModbusDev[j-1].u8DevVendor = sModbusManager.sISENSEManager[j-1].u8ISENSEType;
        sModbusDev[j-1].u32SlaveID = sModbusManager.sISENSEManager[j-1].u32ISENSEAbsSlaveID;  
        //
        // Did parameters change?
        //
        if  (g_sParameters.sModbusParameters[j-1+ MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM + MAX_NUM_OF_VAC + MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL].u8DevVendor != sModbusDev[j-1].u8DevVendor)
        {
          //
          // Update the current parameters with the new settings.
          //
          g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM + MAX_NUM_OF_VAC + MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL].u8DevVendor = sModbusDev[j-1].u8DevVendor;
          g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM + MAX_NUM_OF_VAC + MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL].u32SlaveID = sModbusDev[j-1].u32SlaveID;
          //
          // Yes - save these settings as the defaults.
          //
          g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM + MAX_NUM_OF_VAC + MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL].u8DevVendor = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM + MAX_NUM_OF_VAC + MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL].u8DevVendor;
          g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM + MAX_NUM_OF_VAC + MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL].u32SlaveID = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM + MAX_NUM_OF_VAC + MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL].u32SlaveID;
           
          u8SaveConfigFlag |= 1;
        }      
        
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    // isenseDeltaFreqDisConnect
    else if(!strcmp(object->name, "isenseDeltaFreqDisConnect"))
    {
      if((value->integer >= 1) && (value->integer <= 10) && (value->integer > entry->isenseDeltaFreqReConnect ))
      {
        //Set object value
        entry->isenseDeltaFreqDisConnect= value->integer;
        g_sParameters.u8DeltaFreqDisConnect2 = (uint8_t)value->integer;
        g_sWorkingDefaultParameters.u8DeltaFreqDisConnect2 = g_sParameters.u8DeltaFreqDisConnect2;
        u8SaveConfigFlag |= 1;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    // isenseDeltaFreqReConnect
    else if(!strcmp(object->name, "isenseDeltaFreqReConnect"))
    {
      if((value->integer >= 1) && (value->integer <= 10) && (value->integer < entry->isenseDeltaFreqDisConnect ))
      {
        //Set object value
        entry->isenseDeltaFreqReConnect= value->integer;
        g_sParameters.u8DeltaFreqReConnect2 = (uint8_t)value->integer;
        g_sWorkingDefaultParameters.u8DeltaFreqReConnect2 = g_sParameters.u8DeltaFreqReConnect2;
        u8SaveConfigFlag |= 1;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    // isenseEnableFreqTrap
    else if(!strcmp(object->name, "isenseEnableFreqTrap"))
    {
      if((value->integer == 0) || (value->integer == 1))
      {
        //Set object value
        privateMibBase.isenseGroup.isenseTable.isenseEnableFreqTrap = value->integer;
        g_sParameters.u8EnableFreqTrap2 = (uint8_t)value->integer;
        g_sWorkingDefaultParameters.u8EnableFreqTrap2 = g_sParameters.u8EnableFreqTrap2;
        u8SaveConfigFlag |= 1;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    // isense_delta_freq_v
    else if(!strcmp(object->name, "isense_delta_freq_v"))
    {
      if((value->integer >= 20) && (value->integer < 500))
      {
        //Set object value
        entry->isense_delta_freq_v= value->integer;
        g_sParameters.isense_delta_freq_v = (uint16_t)value->integer;
        g_sWorkingDefaultParameters.isense_delta_freq_v = g_sParameters.isense_delta_freq_v;
        u8SaveConfigFlag |= 1;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    // isense_cnt_timeout_freq_out
    else if(!strcmp(object->name, "isense_cnt_timeout_freq_out"))
    {
      if((value->integer >= 0) && (value->integer <= 255))
      {
        //Set object value
        entry->isense_cnt_timeout_freq_out= value->integer;
        g_sParameters.isense_cnt_timeout_freq_out = (uint16_t)value->integer;
        g_sWorkingDefaultParameters.isense_cnt_timeout_freq_out = g_sParameters.isense_cnt_timeout_freq_out;
        u8SaveConfigFlag |= 1;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    // isense_cnt_timeout_freq_in
    else if(!strcmp(object->name, "isense_cnt_timeout_freq_in"))
    {
      if((value->integer >=0) && (value->integer <=255))
      {
        //Set object value
        entry->isense_cnt_timeout_freq_in= value->integer;
        g_sParameters.isense_cnt_timeout_freq_in = (uint16_t)value->integer;
        g_sWorkingDefaultParameters.isense_cnt_timeout_freq_in = g_sParameters.isense_cnt_timeout_freq_in;
        u8SaveConfigFlag |= 1;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //Successful processing
    return NO_ERROR;

}
/**
 * @brief Get ISENSEInfoEntry object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t privateMibGetISENSEInfoEntry(const MibObject *object, const uint8_t *oid,
                                  size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibISENSEInfoEntry *entry;

    //Point to the instance identifier
    n = object->oidLen;

    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index != 1)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the interface table entry
    entry = &privateMibBase.isenseGroup.isenseTable;
    //isenseID object?
    if(!strcmp(object->name, "isenseID"))
    {
        //Get object value
        value->integer = entry->isenseID;
    }
    //isenseStatus object?
    else if(!strcmp(object->name, "isenseStatus"))
    {
        //Get object value
        value->integer = entry->isenseStatus;
    }
    //isenseImportActiveEnergy object?
    else if(!strcmp(object->name, "isenseImportActiveEnergy"))
    {
        //Get object value
        value->integer = entry->isenseImportActiveEnergy;
    }
    //isenseExportActiveEnergy object?
    else if(!strcmp(object->name, "isenseExportActiveEnergy"))
    {
        //Get object value
        value->integer = entry->isenseExportActiveEnergy;
    }
    //isenseImportReactiveEnergy object?
    else if(!strcmp(object->name, "isenseImportReactiveEnergy"))
    {
        //Get object value
        value->integer = entry->isenseImportReactiveEnergy;
    }
    //isenseExportReactiveEnergy object?
    else if(!strcmp(object->name, "isenseExportReactiveEnergy"))
    {
        //Get object value
        value->integer = entry->isenseExportReactiveEnergy;
    }
    //isenseTotalActiveEnergy object?
    else if(!strcmp(object->name, "isenseTotalActiveEnergy"))
    {
        //Get object value
        value->integer = entry->isenseTotalActiveEnergy;
    }
    //isenseTotalReactiveEnergy object?
    else if(!strcmp(object->name, "isenseTotalReactiveEnergy"))
    {
        //Get object value
        value->integer = entry->isenseTotalReactiveEnergy;
    }
    //isenseActivePower object?
    else if(!strcmp(object->name, "isenseActivePower"))
    {
        //Get object value
        value->integer = entry->isenseActivePower;
    }
    //isenseReactivePower object?
    else if(!strcmp(object->name, "isenseReactivePower"))
    {
        //Get object value
        value->integer = entry->isenseReactivePower;
    }
    //isensePowerFactor object?
    else if(!strcmp(object->name, "isensePowerFactor"))
    {
        //Get object value
        value->integer = entry->isensePowerFactor;
    }
    //isenseFrequency object?
    else if(!strcmp(object->name, "isenseFrequency"))
    {
        //Get object value
        value->integer = entry->isenseFrequency;
    }
    //isenseCurrent object?
    else if(!strcmp(object->name, "isenseCurrent"))
    {
        //Get object value
        value->integer = entry->isenseCurrent;
    }
    //isenseVoltage object?
    else if(!strcmp(object->name, "isenseVoltage"))
    {
        //Get object value
        value->integer = entry->isenseVoltage;
    }
    //isenseApparentPower object?
    else if(!strcmp(object->name, "isenseApparentPower"))
    {
        //Get object value
        value->integer = entry->isenseApparentPower;
    }
    //isenseType object?
    else if(!strcmp(object->name, "isenseType"))
    {
        //Get object value
        value->integer = entry->isenseType;
    }
    //isenseSerial object?
    else if(!strcmp(object->name, "isenseSerial"))
    {
        //Make sure the buffer is large enough to hold the entire object
        if(*valueLen >= entry->isenseSerialLen)
        {
            //Copy object value
            memcpy(value->octetString, entry->isenseSerial, entry->isenseSerialLen);
            //Return object length
            *valueLen = entry->isenseSerialLen;
        }
        else
        {
            //Report an error
            error = ERROR_BUFFER_OVERFLOW;
        }
    }
    //isenseModbusID object?
    else if(!strcmp(object->name, "isenseModbusID"))
    {
        //Get object value
        value->integer = entry->isenseModbusID;
    }
    //isenseModel object?
    else if(!strcmp(object->name, "isenseModel"))
    {
        //Make sure the buffer is large enough to hold the entire object
        if(*valueLen >= entry->isenseModelLen)
        {
            //Copy object value
            memcpy(value->octetString, entry->isenseModel, entry->isenseModelLen);
            //Return object length
            *valueLen = entry->isenseModelLen;
        }
        else
        {
            //Report an error
            error = ERROR_BUFFER_OVERFLOW;
        }
    }
    //isenseOutOfRangeFreq object?
    else if(!strcmp(object->name, "isenseOutOfRangeFreq"))
    {
        //Get object value
        value->integer = entry->isenseOutOfRangeFreq;
    }
    //isenseDeltaFreqDisConnect object?
    else if(!strcmp(object->name, "isenseDeltaFreqDisConnect"))
    {
        //Get object value
        value->integer = entry->isenseDeltaFreqDisConnect;
    }
    //isenseDeltaFreqReConnect object?
    else if(!strcmp(object->name, "isenseDeltaFreqReConnect"))
    {
        //Get object value
        value->integer = entry->isenseDeltaFreqReConnect;
    }
    //isenseEnableFreqTrap object?
    else if(!strcmp(object->name, "isenseEnableFreqTrap"))
    {
        //Get object value
        value->integer = entry->isenseEnableFreqTrap;
    }
    //isense_delta_freq_v object?
    else if(!strcmp(object->name, "isense_delta_freq_v"))
    {
        //Get object value
        value->integer = entry->isense_delta_freq_v;
    }
    //isense_cnt_timeout_freq_out object?
    else if(!strcmp(object->name, "isense_cnt_timeout_freq_out"))
    {
        //Get object value
        value->integer = entry->isense_cnt_timeout_freq_out;
    }
    //isense_cnt_timeout_freq_in object?
    else if(!strcmp(object->name, "isense_cnt_timeout_freq_in"))
    {
        //Get object value
        value->integer = entry->isense_cnt_timeout_freq_in;
    }

    //Return status code
    return error;
}


/**
 * @brief Get next ISENSEInfoEntry object
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] nextOid OID of the next object in the MIB
 * @param[out] nextOidLen Length of the next object identifier, in bytes
 * @return Error code
 **/
error_t privateMibGetNextISENSEInfoEntry(const MibObject *object, const uint8_t *oid,
                                      size_t oidLen, uint8_t *nextOid, size_t *nextOidLen)
{
    error_t error;
    size_t n;
    uint_t index;

    //Make sure the buffer is large enough to hold the OID prefix
    if(*nextOidLen < object->oidLen)
        return ERROR_BUFFER_OVERFLOW;

    //Copy OID prefix
    memcpy(nextOid, object->oid, object->oidLen);

    //Loop through network interfaces
    for(index = 1; index <= privateMibBase.isenseGroup.isenseInstalledISENSE; index++)
    {
        //Append the instance identifier to the OID prefix
        n = object->oidLen;

        //The ifIndex is used as instance identifier
        error = mibEncodeIndex(nextOid, *nextOidLen, &n, index);
        //Any error to report?
        if(error) return error;

        //Check whether the resulting object identifier lexicographically
        //follows the specified OID
        if(oidComp(nextOid, n, oid, oidLen) > 0)
        {
            //Save the length of the resulting object identifier
            *nextOidLen = n;
            //Next object found
            return NO_ERROR;
        }
    }

    //The specified OID does not lexicographically precede the name
    //of some object
    return ERROR_OBJECT_NOT_FOUND;
}
//========================================== ISENSEInfo Function ==========================================//
//PM DC INFO FUNCTION ========================================================================================
//private mib set pm dc group---------------------------------------------------------------------------------
error_t privateMibSet_pm_dc_Group(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibPMDCGroup *entry;
    
    //	Point to the liBattGroup entry
    entry = &privateMibBase.pm_dc_group;
    
    if(!strcmp(object->name, "pm_dc_Installed_pm_dc"))
    {
      if((value->integer >= 0) && (value->integer <= 1))
      {
          //Get object value
          entry->pm_dc_installed_PM_DC = value->integer; 
          if (entry->pm_dc_installed_PM_DC != sModbusManager.u8_number_of_pm_dc)
          {
              sModbusManager.u8_number_of_pm_dc = entry->pm_dc_installed_PM_DC;
              g_sParameters.u8DevNum[TOTAL_PM_DC_INDEX] = sModbusManager.u8_number_of_pm_dc;
              g_sWorkingDefaultParameters.u8DevNum[TOTAL_PM_DC_INDEX] = g_sParameters.u8DevNum[TOTAL_PM_DC_INDEX];
              u8SaveConfigFlag |= 1;
          }
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    else
    {
        TRACE_ERROR("ERROR_OBJECT_NOT_FOUND!\r\n");
        //The specified object does not exist
        return ERROR_OBJECT_NOT_FOUND;
    }
    
    //Successful processing
    return NO_ERROR; 
}
// private mib set pm_dc info entry---------------------------------------------------------------
error_t privateMibSet_pm_dc_InfoEntry(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    uint_t j;
    tModbusParameters sModbusDev[3];
    PrivateMibPMDCInfoEntry *entry;

    //Point to the instance identifier
    n = object->oidLen;

    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index != 1)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the libTable entry
    entry = &privateMibBase.pm_dc_group.pm_dc_table;
    //isenseType object?
    if(!strcmp(object->name, "pm_dc_Type"))
    {
      if((value->integer >= 0) && (value->integer <= 15))
      {
        //Set object value
        entry->pm_dc_type = value->integer;
        j = index;
        sModbusManager.s_pm_dc_manager[j-1].u8_pm_dc_type = entry->pm_dc_type;
        
        switch(sModbusManager.s_pm_dc_manager[j-1].u8_pm_dc_type)
        {
        case 1:// YADA_DC
        {               
            sModbusManager.s_pm_dc_manager[j-1].u32_pm_dc_slave_offset    = 30;
            sModbusManager.s_pm_dc_manager[j-1].u32_pm_dc_abs_slaveID     = 30 +j;
            sModbusManager.s_pm_dc_manager[j-1].u32_pm_dc_base_addr[0]    = 0;       //0x03
            sModbusManager.s_pm_dc_manager[j-1].u8_pm_dc_num_of_reg[0]    = 2;
            sModbusManager.s_pm_dc_manager[j-1].u32_pm_dc_base_addr[1]    = 12;       //0x03
            sModbusManager.s_pm_dc_manager[j-1].u8_pm_dc_num_of_reg[1]    = 2;
            sModbusManager.s_pm_dc_manager[j-1].u32_pm_dc_base_addr[2]    = 26;       //0x03
            sModbusManager.s_pm_dc_manager[j-1].u8_pm_dc_num_of_reg[2]    = 2;
            sModbusManager.s_pm_dc_manager[j-1].u32_pm_dc_base_addr[3]    = 40;       //0x03
            sModbusManager.s_pm_dc_manager[j-1].u8_pm_dc_num_of_reg[3]    = 2;
        }
        break;
        };
        sModbusDev[j-1].u8DevVendor = sModbusManager.s_pm_dc_manager[j-1].u8_pm_dc_type;
        sModbusDev[j-1].u32SlaveID = sModbusManager.s_pm_dc_manager[j-1].u32_pm_dc_abs_slaveID;  
        //
        // Did parameters change?
        //
        if  (g_sParameters.sModbusParameters[j-1+ MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM + MAX_NUM_OF_VAC + MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL+MAX_NUM_OF_ISENSE].u8DevVendor != sModbusDev[j-1].u8DevVendor)
        {
          //
          // Update the current parameters with the new settings.
          //
          g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM + MAX_NUM_OF_VAC + MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL+MAX_NUM_OF_ISENSE].u8DevVendor = sModbusDev[j-1].u8DevVendor;
          g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM + MAX_NUM_OF_VAC + MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL+MAX_NUM_OF_ISENSE].u32SlaveID = sModbusDev[j-1].u32SlaveID;
          //
          // Yes - save these settings as the defaults.
          //
          g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM + MAX_NUM_OF_VAC + MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL+MAX_NUM_OF_ISENSE].u8DevVendor = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM + MAX_NUM_OF_VAC + MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL+MAX_NUM_OF_ISENSE].u8DevVendor;
          g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM + MAX_NUM_OF_VAC + MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL+MAX_NUM_OF_ISENSE].u32SlaveID = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM + MAX_NUM_OF_VAC + MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL+MAX_NUM_OF_ISENSE].u32SlaveID;
           
          u8SaveConfigFlag |= 1;
        }      
        
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //Successful processing
    return NO_ERROR;

}
// private mib get pm dc info entry---------------------------------------------------
error_t privateMibGet_pm_dc_InfoEntry(const MibObject *object, const uint8_t *oid,
                                  size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibPMDCInfoEntry *entry;

    //Point to the instance identifier
    n = object->oidLen;

    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index != 1)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the interface table entry
    entry = &privateMibBase.pm_dc_group.pm_dc_table;
    //pm_dc_ID object?
    if(!strcmp(object->name, "pm_dc_ID"))
    {
        //Get object value
        value->integer = entry->pm_dc_id;
    }
    //pm_dc_Status object?
    else if(!strcmp(object->name, "pm_dc_Status"))
    {
        //Get object value
        value->integer = entry->pm_dc_status;
    }
    //pm_dc_ActiveEnergy object?
    else if(!strcmp(object->name, "pm_dc_ActiveEnergy"))
    {
        //Get object value
        value->integer = entry->pm_dc_active_energy;
    }
    //pm_dc_ActivePower object?
    else if(!strcmp(object->name, "pm_dc_ActivePower"))
    {
        //Get object value
        value->integer = entry->pm_dc_active_power;
    }
    //pm_dc_Current object?
    else if(!strcmp(object->name, "pm_dc_Current"))
    {
        //Get object value
        value->integer = entry->pm_dc_current;
    }
    //pm_dc_Voltage object?
    else if(!strcmp(object->name, "pm_dc_Voltage"))
    {
        //Get object value
        value->integer = entry->pm_dc_voltage;
    }
    //pm_dc_Type object?
    else if(!strcmp(object->name, "pm_dc_Type"))
    {
        //Get object value
        value->integer = entry->pm_dc_type;
    }
    //pm_dc_ModbusID object?
    else if(!strcmp(object->name, "pm_dc_ModbusID"))
    {
        //Get object value
        value->integer = entry->pm_dc_modbus_ID;
    }
    //pm_dc_Model object?
    else if(!strcmp(object->name, "pm_dc_Model"))
    {
        //Make sure the buffer is large enough to hold the entire object
        if(*valueLen >= entry->pm_dc_model_len)
        {
            //Copy object value
            memcpy(value->octetString, entry->pm_dc_model_, entry->pm_dc_model_len);
            //Return object length
            *valueLen = entry->pm_dc_model_len;
        }
        else
        {
            //Report an error
            error = ERROR_BUFFER_OVERFLOW;
        }
    }
    //Return status code
    return error;
}
// private mib get next pm dc info entry------------------------------------------------------
error_t privateMibGetNext_pm_dc_InfoEntry(const MibObject *object, const uint8_t *oid,
                                      size_t oidLen, uint8_t *nextOid, size_t *nextOidLen)
{
    error_t error;
    size_t n;
    uint_t index;

    //Make sure the buffer is large enough to hold the OID prefix
    if(*nextOidLen < object->oidLen)
        return ERROR_BUFFER_OVERFLOW;

    //Copy OID prefix
    memcpy(nextOid, object->oid, object->oidLen);

    //Loop through network interfaces
    for(index = 1; index <= privateMibBase.pm_dc_group.pm_dc_installed_PM_DC; index++)
    {
        //Append the instance identifier to the OID prefix
        n = object->oidLen;

        //The ifIndex is used as instance identifier
        error = mibEncodeIndex(nextOid, *nextOidLen, &n, index);
        //Any error to report?
        if(error) return error;

        //Check whether the resulting object identifier lexicographically
        //follows the specified OID
        if(oidComp(nextOid, n, oid, oidLen) > 0)
        {
            //Save the length of the resulting object identifier
            *nextOidLen = n;
            //Next object found
            return NO_ERROR;
        }
    }

    //The specified OID does not lexicographically precede the name
    //of some object
    return ERROR_OBJECT_NOT_FOUND;
}
//PM DC INFO FUNCTION ========================================================================================

