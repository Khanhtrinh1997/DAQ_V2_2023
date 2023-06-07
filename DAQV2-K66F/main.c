/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * This is template for main module created by New Kinetis SDK 2.x Project Wizard. Enjoy!
 **/

#include "Header.h"
#include "ftp_handler.h"
   
/* Task priorities. */
#define hello_task_PRIORITY (configMAX_PRIORITIES - 1)
#define TRANSFER_SIZE 256U        /*! Transfer dataSize */
#define BUS_CLK_FREQ CLOCK_GetFreq(kCLOCK_FlexBusClk)
/* Global variables */
#pragma default_variable_attributes = @ "MySDRAM"
uint32_t sysCountTest = 0;
uint8_t eepromTest=0;
uint8_t masterTxData[256] = {0U};
uint8_t masterRxData[1024] = {0U};
uint8_t flashWriteCheck;
uint8_t flashReadCheck;
uint8_t* g_pointer;




extern tConfigHandler configHandle;
extern error_t ftpTransferErr;
extern error_t ftpTransferErr2;
uint8_t u8FwUpdateCompleteFlag = 0;
uint8_t u8FwUpdateFlag=0;
uint8_t u8SaveConfigFlag=0;
uint8_t u8IsRebootFlag=0;
uint8_t u8IsRewriteSN=0;
uint8_t rw_flag = 0;
uint8_t buttonPush = 0;

uint32_t WD1_Cnt = 0;
uint32_t resetWD = 1;
unsigned char hexArray[8] = {0,0,0,0,0,0,0,0};

//Thanhcm3 TEST============================
uint32_t EMER_cnt_Tick=0;
uint32_t Save_EMER_cnt_Tick=0;
uint32_t flag_ReInitRs232=0;
//=========================================

#pragma default_variable_attributes =
uint32_t u32TestReadAddr = 0;
#if (USERDEF_SW_TIMER == ENABLED)
TimerHandle_t SwTimerHandle = NULL;
#endif
TaskHandle_t * pModbusRTUTask;
TaskHandle_t * pEmersonDataProcessTask;
TaskHandle_t * pDKD51DataProcessTask;
//uint8_t u8RowBuffer[45];
//uint32_t u32NumOfRow = 0;


#if (APP_USE_DHCP == ENABLED)
DhcpClientSettings dhcpClientSettings;
DhcpClientCtx dhcpClientContext;
#endif
#if (USERDEF_SW_TIMER == ENABLED)
/* The callback function. */
static void SwTimerCallback(TimerHandle_t xTimer);
#endif

/*!
 * @brief Init for CRC-16/MAXIM.
 * @details Init CRC peripheral module for CRC-16/MAXIM protocol.
 *          width=16 poly=0x8005 init=0x0000 refin=true refout=true xorout=0xffff check=0x44c2 name="CRC-16/MAXIM"
 *          http://reveng.sourceforge.net/crc-catalogue/
 */
static void InitCrc16(CRC_Type *base, uint32_t seed)
{
    crc_config_t config;

    config.polynomial = 0x8005;
    config.seed = seed;
    config.reflectIn = true;
    config.reflectOut = true;
    config.complementChecksum = false;
    config.crcBits = kCrcBits16;
    config.crcResult = kCrcFinalChecksum;

    CRC_Init(base, &config);
}

/*!
 * @brief Task responsible for printing of "Hello world." message.
 */
static void hello_task(void *pvParameters)
{
  uint32_t i, j;
  
  //Endless loop
  while(1)
  {
    //ReInit RS232
    if(privateMibBase.cntGroup.RS232ReInitEnable==1){
      UART_Deinit(RS2321_UART);
      vTaskDelay(3);
      Init_RS232_UART();
      privateMibBase.cntGroup.RS232ReInitEnable=2;
      //flag_ReInitRs232=0;
    }
    //SW2 button pressed?
    if(!BUTTON_RESET_READ())
    {
      buttonPush = 1;

      //Wait for the SW2 button to be released
      while(!BUTTON_RESET_READ());
    }   
    
    if (u8FwUpdateFlag == 1)
    {
      privateMibBase.vacGroup.vacTable[0].vacUpdateStep = 1;
      u8FwUpdateCompleteFlag = 1;
      sFLASH_EraseChip();
      
      ConfigSave(); 
      sFLASH_WritePage((uint8_t *)&g_sWorkingDefaultParameters2, FLASH_CFG_START_SN, FLASH_PB_SIZE);
      //FTP client test routine
      ftpTransferErr = ftpClientTest(); 
      if ((ftpTransferErr == NO_ERROR) && (ftpTransferErr2 == NO_ERROR))
      {
        switch(configHandle.devType)
        {
        case _DAQ_FW_:
          {
            sFLASH_EraseSector(FLASH_CFG_START_FW);
            masterTxData[0]=0xaa;
            sFLASH_WriteBuffer((uint8_t *)&masterTxData[0], FLASH_CFG_START_FW, FLASH_PB_SIZE);
            
            g_sParameters2.u8IsWritten = 0x53;
            g_sParameters2.u32FirmwareCRC = privateMibBase.siteGroup.siteFirmwareCRC;
            g_sWorkingDefaultParameters2 = g_sParameters2;  
            u8IsRewriteSN = 1;             
          }
          break;
        case _VAC_FW_:
          {
            if((configHandle.Check_vac_firm_version==0)||(configHandle.Check_vac_firm_version>=VAC_CHECK_FIRM_VERSION))
            {
              configHandle.Check_vac_firm_version=0;
            }else{
              configHandle.isVACUpdate =1;
            }
          }
          break;
        case _VACV2_FW_:
          {
            if((configHandle.Check_vac_firm_version==0)||(configHandle.Check_vac_firm_version<VAC_CHECK_FIRM_VERSION))
            {
              configHandle.Check_vac_firm_version=0;
            }else{
              configHandle.is_vac_v2_update =1;
              configHandle.CRC32_calculator_vac_v2 =privateMibBase.siteGroup.siteFirmwareCRC;
              sprintf(configHandle.crc32_vac_v2, "%08X",configHandle.CRC32_calculator_vac_v2);
              // line end
              configHandle.buff_end_CRC32[0]=':';
              configHandle.buff_end_CRC32[1]='c';
              configHandle.buff_end_CRC32[2]='r';
              configHandle.buff_end_CRC32[3]='c';
              configHandle.buff_end_CRC32[4]='_';
              configHandle.buff_end_CRC32[5]=configHandle.crc32_vac_v2[0];
              configHandle.buff_end_CRC32[6]=configHandle.crc32_vac_v2[1];
              configHandle.buff_end_CRC32[7]=configHandle.crc32_vac_v2[2];
              configHandle.buff_end_CRC32[8]=configHandle.crc32_vac_v2[3];
              configHandle.buff_end_CRC32[9]=configHandle.crc32_vac_v2[4];
              configHandle.buff_end_CRC32[10]=configHandle.crc32_vac_v2[5];
              configHandle.buff_end_CRC32[11]=configHandle.crc32_vac_v2[6];
              configHandle.buff_end_CRC32[12]=configHandle.crc32_vac_v2[7];            
              configHandle.buff_end_CRC32[13]='_';
              configHandle.buff_end_CRC32[14]='e';
              configHandle.buff_end_CRC32[15]='n';
              configHandle.buff_end_CRC32[16]='d';
              configHandle.buff_end_CRC32[17]='\r';
              configHandle.buff_end_CRC32[18]='\n';
              
            }
          }
          break;
        };
      }
      u8FwUpdateFlag = 2;
      switch(configHandle.devType)
      {
      case _DAQ_FW_:
        {
          u8IsRebootFlag = 1; 
        }
        break;
      case _VAC_FW_:
        {
          if((configHandle.Check_vac_firm_version==0)||(configHandle.Check_vac_firm_version>=VAC_CHECK_FIRM_VERSION)){
            configHandle.Check_vac_firm_version=0;
          }else{
          sVACInfo[0].u16FirmVersion = 0;
          privateMibBase.vacGroup.vacTable[0].vacFirmVersion = 0;
          privateMibBase.vacGroup.vacTable[0].vacUpdateStep = 2; 
          }
        }
        break;
      case _VACV2_FW_:
        {
          if((configHandle.Check_vac_firm_version==0)||(configHandle.Check_vac_firm_version<VAC_CHECK_FIRM_VERSION))
            {
              configHandle.Check_vac_firm_version=0;
            }else{
              sVACInfo[0].u16FirmVersion = 0;
              privateMibBase.vacGroup.vacTable[0].vacFirmVersion = 0;
              privateMibBase.vacGroup.vacTable[0].vacUpdateStep = 2;
              
            }
          
        }
        break;
      };     
           

    }
    
    if (buttonPush == 1)
    {
      switch (rw_flag)
      {
      case 0:
        {
          ConfigLoadFactory();
          u8SaveConfigFlag |= 1; 
          u8IsRebootFlag = 1; 
          u8IsRewriteSN = 1;
        }break;
      case 1:
        {
#if (USERDEF_CLIENT_FTP == ENABLED)
        //FTP client test routine
          ftpClientTest();
#endif
        }break;
      case 2:
        {
          sFLASH_EraseChip();
        }break;
      case 3:
        {          
            sFirmwarePackage.u8SectorIdx = (sFirmwarePackage.u32RowIdx-1)/4097;
            sFirmwarePackage.u32OffsetAddr = (sFirmwarePackage.u32RowIdx-1)%4097;
            sFirmwarePackage.u32ReadingAddr = sFirmwarePackage.u8SectorIdx * (4096 * 45 + 17) + (sFirmwarePackage.u32OffsetAddr) * 45;
            sFLASH_ReadBuffer((uint8_t *)&sFirmwarePackage.u8RowBuffer[0],sFirmwarePackage.u32ReadingAddr, 45);
            //********************* Convert Ascii To Hex Byte **********************//
            sFirmwarePackage.u8Ascii2Hex[0] = sFirmwarePackage.u8RowBuffer[0];            
            for (i = 0; i < 21; i++)
            {
              sFirmwarePackage.u8Ascii2Hex[i+1] = hex2byte(&sFirmwarePackage.u8RowBuffer[0],1 + i*2);
            }
            if (sFirmwarePackage.u8Ascii2Hex[0] == 0x3A)
            {
              sFirmwarePackage.bIsValidStartCode = 1;
              sFirmwarePackage.u8StartCode = sFirmwarePackage.u8Ascii2Hex[0];
              sFirmwarePackage.u8ByteCount = sFirmwarePackage.u8Ascii2Hex[1];
              sFirmwarePackage.u16FlashAddress = (sFirmwarePackage.u8Ascii2Hex[2] << 8) | sFirmwarePackage.u8Ascii2Hex[3];
              sFirmwarePackage.u8RecordType = sFirmwarePackage.u8Ascii2Hex[4];
              //********************* Compare Checksum     Byte **********************//
              sFirmwarePackage.u8Checksum = sFirmwarePackage.u8Ascii2Hex[5+sFirmwarePackage.u8ByteCount];
              sFirmwarePackage.u8ChecksumCal = Checksum8(&sFirmwarePackage.u8Ascii2Hex[0], 4+sFirmwarePackage.u8ByteCount);
              //********************* Compare Checksum     Byte **********************// 
              sFirmwarePackage.u8Ascii2Hex[6+sFirmwarePackage.u8ByteCount] = sFirmwarePackage.u8RowBuffer[11+2*sFirmwarePackage.u8ByteCount];
              sFirmwarePackage.u8Ascii2Hex[7+sFirmwarePackage.u8ByteCount] = sFirmwarePackage.u8RowBuffer[12+2*sFirmwarePackage.u8ByteCount]; 
            }
            else
            {
              sFirmwarePackage.bIsValidStartCode = 0;
              sFirmwarePackage.u8StartCode = 0;
              sFirmwarePackage.u8ByteCount = 0;
              sFirmwarePackage.u16FlashAddress = 0;
              sFirmwarePackage.u8RecordType = 0;
              sFirmwarePackage.u8Checksum = 0xFF;
              sFirmwarePackage.u8ChecksumCal = 0;
            }            
            //********************* Convert Ascii To Hex Byte **********************//    
            if (sFirmwarePackage.u8Checksum == sFirmwarePackage.u8ChecksumCal)
            {             
              for (i = 0; i < sFirmwarePackage.u8ByteCount; i++)
              {
                sFirmwarePackage.u8Data[i] = sFirmwarePackage.u8Ascii2Hex[5 + i];            
              }            
              for (i = 0; i < (24 - sFirmwarePackage.u8ByteCount); i++)
              {
                sFirmwarePackage.u8Ascii2Hex[24-i] = 0;
              }
              sFirmwarePackage.bIsValidRow = 1;
            } 
            else
            {
              sFirmwarePackage.bIsValidRow = 0;
            }
          
        }break;
      case 4:
        {
          sFLASH_ReadBuffer((uint8_t *)&masterRxData[0], u32TestReadAddr, 256);
        }break;
        
      case 5:
        {
        }break;
        
      case 6:
        {
          g_sWorkingDefaultParameters.u32TrapPeriod = 5;
          g_sWorkingDefaultParameters.sPort[0].u8ControllerType = 1;
          ConfigLoad();
        }break;
      default:
        break;
      };
      
      buttonPush = 0;      

    }    
    
    if ((u8SaveConfigFlag & 0x01) == 0x01)
    {
      ConfigSave();      
      u8SaveConfigFlag &= ~(0x01);   
    }
    
    if (u8IsRewriteSN == 1)
    {
      sFLASH_EraseSector(FLASH_CFG_START_SN);
      sFLASH_WritePage((uint8_t *)&g_sWorkingDefaultParameters2, FLASH_CFG_START_SN, sizeof(tConfigParameters2));
      u8IsRewriteSN = 0;
    }
    if (u8IsRebootFlag == 1)
    {
      privateMibBase.siteGroup.siteSystemReset = 0x53;
      u8IsRebootFlag = 0;
    }

    //      Loop delay
    osDelayTask(100);
  }
}

/*!
 * @brief Idle hook.
 */
void vApplicationIdleHook(void)
{
    static uint32_t ulCount = 0;
    ulCount++;
}

/*!
 * @brief tick hook is executed every tick.
 */
void vApplicationTickHook(void)
{
    
    
    
    sysCountTest++;
    if(sysCountTest >= 300)
    {
        LED_RUN_TOGGLE();
        sysCountTest = 0;
    }
    if(netInterface[0].linkState == 1)
    {
      GPIO_WritePinOutput(GPIOE,11U,0); /*!< Turn on target LED_1 */
    }
    else
    {
      GPIO_WritePinOutput(GPIOE,11U,1); /*!< Turn off target LED_1 */
    }
    
    WD1_Cnt++;
    if (privateMibBase.siteGroup.siteSystemReset == 0x53)
    {
       resetWD = 6000;
    }
    else resetWD = 1;
    if(WD1_Cnt >= (resetWD * 2))
    {
      watchdog_Refresh();
      WD1_Cnt = 0;
    }     
    
    trapStatus_TimePeriod++;
    
}

#pragma section = "MySDRAM"
#pragma section = "MySDRAM_init"

void DoInit()
{
    char * from2 = __section_begin("MySDRAM_init");
    char * to2 = __section_begin("MySDRAM");
    memcpy(to2, from2, __section_size("MySDRAM"));
}
extern uint32_t SDRAMClock;
/*!
 * @brief Application entry point.
 */

int main(void)
{
 
    uint32_t soptReg;
    uint32_t fbReg;
    error_t error;
    NetInterface *interface;
    OsTask *task;
    MacAddr macAddr;
    uint32_t i=0;
    char testData[] = "123456789";
    CRC_Type *base = CRC0;
#if (APP_USE_DHCP == DISABLED)
    Ipv4Addr ipv4Addr;
#endif
    DoInit();

    /* Define the init structure */
    gpio_pin_config_t out_config =
    {
        kGPIO_DigitalOutput, 0,
    };
    gpio_pin_config_t in_config =
    {
        kGPIO_DigitalInput, 0,
    };
    /* Init board hardware. */
    BOARD_InitLEDs();
    BOARD_InitENET();
    BOARD_InitUARTs();
    BOARD_InitI2C();
    BOARD_InitBUTTONs();
    BOARD_InitSPIs();
    BOARD_BootClockHSRUN();
    
    sFLASH_Init();
    LED_RUN_INIT(1);
    GPIO_PinInit(GPIOE,11U,&out_config);  
    watchdog_Init();    

    ConfigInit();
    Init_RS485_MODBUS_UART();
   //g_sParameters.sPort[0].u8ControllerType=5;
    TYPE = g_sParameters.sPort[0].u8ControllerType;
    //TYPE = VERTIV_M830B;
    if ((TYPE == EMER) || (TYPE == ZTE) || (TYPE == EMERV21))
    {
      Init_RS232_UART(); 
    } else if ((TYPE == HW)||(TYPE==DKD51_BDP))
    {
#if ((USERDEF_RS485_AGISSON == ENABLED)||(USERDEF_RS485_DKD51_BDP == ENABLED))
      if(TYPE == HW){
       Init_RS485_UART();
        
      }else if(TYPE==DKD51_BDP){
        DKD51_init_rs485_uart(); 
      }
#endif    
    }    
    if (TYPE == VERTIV_M830B)
    {
     VERTIV_init_rs485_uart();   
    }
    
    
    
    gpio_pin_config_t RS485Dir_config = {
      kGPIO_DigitalOutput, 0,
      };
    GPIO_PinInit(GPIOD,1U,&RS485Dir_config);    
    GPIO_PinInit(GPIOD,0U,&RS485Dir_config); 
    GPIO_SetPinsOutput(GPIOD,1u << 1u);
    //GPIO_SetPinsOutput(GPIOD,1u << 0u);
    GPIO_PinInit(GPIOE,2U,&RS485Dir_config);    
    GPIO_PinInit(GPIOE,3U,&RS485Dir_config); 
    
    //Initialize kernel
    osInitKernel();

    //Start-up message
    TRACE_INFO("\r\n");
    TRACE_INFO("**********************************\r\n");
    TRACE_INFO("*** DCU Monitoring Device ***\r\n");
    TRACE_INFO("**********************************\r\n");
    TRACE_INFO("\r\n");
    
#if (USERDEF_CLIENT_SNMP == ENABLED)
    //Standard MIB-II initialization
    error = mib2Init();
    //Any error to report?
    if(error)
    {
        //Debug message
        TRACE_ERROR("Failed to initialize MIB!\r\n");
    }
    //Private MIB initialization
    error = privateMibInit();
    //Any error to report?
    if(error)
    {
        //Debug message
        TRACE_ERROR("Failed to initialize MIB!\r\n");
    }
    
#endif
    hex2char(&hexArray[0],0xF3,2);
    
  
    //=================================================== TCP/IP stack initialization ===========================================//   
    //TCP/IP stack initialization
    error = netInit();
    //Any error to report?
    if(error)
    {
        //Debug message
        TRACE_ERROR("Failed to initialize TCP/IP stack!\r\n");
    }

    //Configure the first Ethernet interface
    interface = &netInterface[0];

    //Set interface name
    netSetInterfaceName(interface, "eth0");
    //Set host name
    netSetHostname(interface, "SiteMonitor");
    //Select the relevant network adapter
    netSetDriver(interface, &mk6xEthDriver);
    netSetPhyDriver(interface, &ksz8081PhyDriver);
    //Set host MAC address
    macStringToAddr(&privateMibBase.siteGroup.siteMACInfo[0], &macAddr);
    netSetMacAddr(interface, &macAddr);

    //Initialize network interface
    error = netConfigInterface(interface);
    //Any error to report?
    if(error)
    {
        //Debug message
        TRACE_ERROR("Failed to configure interface %s!\r\n", interface->name);
    }

#if (IPV4_SUPPORT == ENABLED)
#if (APP_USE_DHCP == ENABLED)
    //Get default settings
    dhcpClientGetDefaultSettings(&dhcpClientSettings);
    //Set the network interface to be configured by DHCP
    dhcpClientSettings.interface = interface;
    //Disable rapid commit option
    dhcpClientSettings.rapidCommit = FALSE;

    //DHCP client initialization
    error = dhcpClientInit(&dhcpClientContext, &dhcpClientSettings);
    //Failed to initialize DHCP client?
    if(error)
    {
        //Debug message
        TRACE_ERROR("Failed to initialize DHCP client!\r\n");
    }

    //Start DHCP client
    error = dhcpClientStart(&dhcpClientContext);
    //Failed to start DHCP client?
    if(error)
    {
        //Debug message
        TRACE_ERROR("Failed to start DHCP client!\r\n");
    }
#else
    //Set IPv4 host address
    ipv4SetHostAddr(interface, (Ipv4Addr) privateMibBase.cfgNetworkGroup.siteNetworkInfo.u32IP);

    //Set subnet mask
    ipv4SetSubnetMask(interface, (Ipv4Addr) privateMibBase.cfgNetworkGroup.siteNetworkInfo.u32SN);

    //Set default gateway
    ipv4SetDefaultGateway(interface, (Ipv4Addr) privateMibBase.cfgNetworkGroup.siteNetworkInfo.u32GW);

    //Set primary and secondary DNS servers
    ipv4StringToAddr(APP_IPV4_PRIMARY_DNS, &ipv4Addr);
    ipv4SetDnsServer(interface, 0, ipv4Addr);
    ipv4StringToAddr(APP_IPV4_SECONDARY_DNS, &ipv4Addr);
    ipv4SetDnsServer(interface, 1, ipv4Addr);
#endif
#endif
    
    TRACE_INFO("\r\n");
    TRACE_INFO("IP         : %s\r\n",privateMibBase.cfgNetworkGroup.siteNetworkInfo.ucIP);
    TRACE_INFO("Gateway    : %s\r\n",privateMibBase.cfgNetworkGroup.siteNetworkInfo.ucGW);
    TRACE_INFO("Subnet Mask: %s\r\n",privateMibBase.cfgNetworkGroup.siteNetworkInfo.ucSN);
    TRACE_INFO("Server IP  : %s\r\n",privateMibBase.cfgNetworkGroup.siteNetworkInfo.ucSIP);
    TRACE_INFO("MAC        : %s\r\n",privateMibBase.siteGroup.siteMACInfo);
    
    //=================================================== TCP/IP stack initialization ===========================================//
    //==================================================HTTP SERVER CONFIG===========================================//
#if (USERDEF_SERVER_HTTP == ENABLED)
    http_handler_init();
#endif
    //==================================================HTTP SERVER CONFIG===========================================//

    //==================================================SNMP CLIENT CONFIG===========================================//
#if (USERDEF_CLIENT_SNMP == ENABLED)
    snmp_handler_init();
#endif

    //==================================================SNMP CLIENT CONFIG===========================================//
#if (USERDEF_SW_TIMER == ENABLED)

    /* Create the software timer. */
    SwTimerHandle = xTimerCreate("SwTimer",          /* Text name. */
                                    SW_TIMER_PERIOD_MS, /* Timer period. */
                                    pdTRUE,             /* Enable auto reload. */
                                    0,                  /* ID is not used. */
                                    SwTimerCallback);   /* The callback function. */
    /* Start timer. */
    xTimerStart(SwTimerHandle, 0);
#endif      
    /* Create RTOS task */
    xTaskCreate(hello_task, "Hello_task", 800, NULL, hello_task_PRIORITY, NULL);//configMINIMAL_STACK_SIZE
    if (TYPE == EMER)
    {
      xTaskCreate(Emerson_Data_Process, "Emerson_Data_Process", configMINIMAL_STACK_SIZE+256, NULL, RS2321_Send_Priority , pEmersonDataProcessTask);
    } 
        else if (TYPE == EMERV21)
    {
      xTaskCreate(EMER_V21_Data_Process, "EMER_V21_Data_Process", configMINIMAL_STACK_SIZE+200, NULL, RS2321_Send_Priority , NULL);
    }
    else if (TYPE == ZTE)
    {
      xTaskCreate(ZTE_Data_Process, "ZTE_Data_Process", configMINIMAL_STACK_SIZE+256, NULL, RS2321_Send_Priority , NULL);
    }
    else if (TYPE == HW)
    {
#if (USERDEF_RS485_AGISSON == ENABLED)
      xTaskCreate(Agisson_Data_Process, "Agisson_Data_Process", configMINIMAL_STACK_SIZE+256, NULL, RS2321_Send_Priority , NULL);
#endif
    }
    else if (TYPE == DKD51_BDP)
    {
#if (USERDEF_RS485_DKD51_BDP == ENABLED)
      xTaskCreate(DKD51_BDP_data_process, "DKD51_BDP_data_process", configMINIMAL_STACK_SIZE+256, NULL, RS2321_Send_Priority , pDKD51DataProcessTask);
#endif
    }
    else if (TYPE == VERTIV_M830B)
    {
#if (USERDEF_RS485_VERTIV_M830B == ENABLED)
      xTaskCreate(VERTIV_M830B_data_process, "VERTIV_M830B_data_process", configMINIMAL_STACK_SIZE+256, NULL, RS2321_Send_Priority , NULL);
#endif
    }    
    //Create a task to active RS485 communication
    xTaskCreate(modbusRTU_task, "modbusRTU_task", configMINIMAL_STACK_SIZE+512, NULL, RS2321_Send_Priority , pModbusRTUTask);
    
    vTaskStartScheduler();  


    /* Add your code here */

    for(;;)   /* Infinite loop to avoid leaving the main function */
    {
        __asm("NOP"); /* something to use as a breakpoint stop while looping */
    }
}


#if (USERDEF_SW_TIMER == ENABLED)
/*!
 * @brief Software timer callback.
 */
static void SwTimerCallback(TimerHandle_t xTimer)
{
//    PRINTF("Tick.\r\n");
  if (privateMibBase.cfgNetworkGroup.u32TrapTick == 8000)
  {
    trapStatus_TimePeriod++;
  }
}
#endif
