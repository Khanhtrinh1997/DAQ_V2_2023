#define TRACE_LEVEL 5

#include "AgissonDriver.h"
#include "config.h"
#include "flash_spi.h"
#include "debug.h"
#include "variables.h"


//void LoadHisLog(void);

//*****************************************************************************
//
//! This structure instance contains the factory-default set of configuration
//! parameters for S2E module.
//
//*****************************************************************************
static const tConfigParameters g_sParametersFactory =
{
    //
    // The sequence number (ucSequenceNum); this value is not important for
    // the copy in SRAM.
    //
    0,

    //
    // The CRC (ucCRC); this value is not important for the copy in SRAM.
    //
    0,

    //
    // The parameter block version number (ucVersion).
    //
    0,

    //
    // Flags (ucFlags)
    //
    0x80,

    //
    // Reserved (ucReserved1).
    //
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    },

    //
    // Port Parameter Array.
    //
    {
        //
        // Parameters for Port 0 (sPort[0]).
        //
        {
            //
            // The baud rate (ulBaudRate).
            //
            9600,

            //
            // The number of data bits.
            //
            8,

            //
            // The parity (NONE).
            //
            1,

            //
            // The number of stop bits.
            //
            1,

            //
            // The flow control (NONE).
            //
            1,

            //
            // The controller Type
            //
            0,

            //
            // Flags indicating the operating mode for this port.
            //
            1,

            //
            // Reserved (ucReserved0).
            //
            {
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
            },
        },

        //
        // Parameters for Port 1 (sPort[1]).
        //
        {
            //
            // The baud rate (ulBaudRate).
            //
            9600,

            //
            // The number of data bits.
            //
            8,

            //
            // The parity (NONE).
            //
            1,

            //
            // The number of stop bits.
            //
            1,

            //
            // The flow control (NONE).
            //
            1,

            //
            // The controller Type
            //
            0,

            //
            // Flags indicating the operating mode for this port.
            //
            1,

            //
            // Reserved (ucReserved0).
            //
            {
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
            },
        },
    },

    //
    // Module Name (ucModName).
    //
    {
        'V','i','e','t','t','e','l',' ',
		'P','o','w','e','r',' ',
		'S','y','s','t','e','m',' ',
		'M','o','n','i','t','o','r',
		'\0',0,0,0,0,0,0,0,0,0,0,0,
    },

    //
    // Static IP address (used only if indicated in ucFlags).
    //
    0xc0a8010a,
    //
    // Default gateway IP address (used only if static IP is in use).
    //
    0xc0a80101,
    //
    // Subnet mask (used only if static IP is in use).
    //
    0xFFFFFF00,
    //
    // SNMP server IP 
    //
    0xc0a8010c,
    //
    // Trap Period 
    //
    5,
    //
    // Trap Mode 
    //
    1,
    //
    // Firmware 
    //
    0xFFFFFFFF,

    //
    // FTP Server Name (ucModName).
    //
    {
        'V','i','e','t','t','e','l',' ',
		'P','o','w','e','r',' ',
		'S','y','s','t','e','m',' ',
		'M','o','n','i','t','o','r',
		'\0',0,0,0,0,0,0,0,0,0,0,0
    },
    //
    // u32Reserved2 (compiler will pad to the full length)
    //
    {
        0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 
	0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa,
        0xaaaaaaaa, 0xaaaaaaaa
    },
    //
    // Modbus Dev Number 
    //
    {
    0,0,0,0,0,0,0,0,0
    },
    //
    // sModbusDevice 
    //
    {
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00 //1
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00 //10
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00 //20
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00 //30
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00 //40
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      },
      {
        0x00, 0x00, 0x00, 0x00000000, 0x00
      }        
    },
    //
    // Freq Trap
    //
    {0x66},{5,5},{3,3},{1},
    //
    // ISENSE Trap
    //
    {0x66},{5},{3},{0},
    //thanhcm3 add config  for PM, ISENSE-------------------------------
    {0x66},{0x64},{0x64},
    //thanhcm3 add config  for PM, ISENSE 1-----------------------------
    {0x66},{0x01},{0x3C},{0x01},{0x0A},
    //thanhcm3 add config for check gen---------------------------------
    {0x66},{0x05},
    //
    {0xaa,0xaa,0xaa},
    //
    // u32Reserved3 (compiler will pad to the full length)
    //
    {
        0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 
	0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa,
        0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa
    }
    
};
//*****************************************************************************
static tConfigParameters2 g_sParametersFactory2 =
{
    //
    // Is Serial Number and MAC is written to flash?
    //
    0,

    //
    // The user MAC
    //
    {
      0x00, 0x1A, 0xB6, 0x20, 0x02, 0x90
    },
    //
    // The user Serial Number
    //
    {
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    },
    0,
    {
      0x20,0x20,0x20,0x20,
      0x20,0x20,0x20,0x20,
      0x20,0x20,0x20,0x20,
      0x20,0x20,0x20,0x20,
      0x20,0x20,0x20,0x20,
      0x20,0x20,0x20,0x20,
      0x20,0x20,0x20,0x20
    },
    //
    // FirmwareCRC_old
    //
    0xaaaaaaaa, 
    //
    // FirmwareCRC_new
    //
    0xaaaaaaaa,    
    //
    // u32Reserved2 (compiler will pad to the full length)
    //
    {
        0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 
        0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa,
        0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa,
        0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa,
        0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 
        0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa,
        0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa,
        0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa,      
        0xaaaaaaaa, 0xaaaaaaaa
    } 
    
};
//*****************************************************************************
//
//! This structure instance contains the run-time set of configuration
//! parameters for S2E module.  This is the active parameter set and may
//! contain changes that are not to be committed to flash.
//
//*****************************************************************************
tConfigParameters g_sParameters;
tConfigParameters2 g_sParameters2;
//tConfigParameters3 g_sParameters3;
//*****************************************************************************
//
//! This structure instance points to the most recently saved parameter block
//! in flash.  It can be considered the default set of parameters.
//
//*****************************************************************************
tConfigParameters *g_psDefaultParameters = &g_sWorkingDefaultParameters;
tConfigParameters2 *g_psDefaultParameters2 = &g_sParameters2;
//tConfigParameters *g_psDefaultParameters = &g_sWorkingDefaultParameters3;

//*****************************************************************************
//
//! This structure contains the latest set of parameter committed to flash
//! and is used by the configuration pages to store changes that are to be
//! written back to flash.  Note that g_sParameters may contain other changes
//! which are not to be written so we can't merely save the contents of the
//! active parameter block if the user requests some change to the defaults.
//
//*****************************************************************************
//static tConfigParameters g_sWorkingDefaultParameters;
tConfigParameters g_sWorkingDefaultParameters;
tConfigParameters2 g_sWorkingDefaultParameters2;
//tConfigParameters3 g_sWorkingDefaultParameters3;

//*****************************************************************************
//
//! This structure instance points to the factory default set of parameters in
//! flash memory.
//
//*****************************************************************************
const tConfigParameters *const g_psFactoryParameters = &g_sParametersFactory;
//*****************************************************************************
//
// The address of the most recent parameter block in flash.
//
//*****************************************************************************
 uint8_t *g_pui8FlashPBCurrent;

  uint8_t u8Buffer[1000];//512
//  uint8_t *pui8Buffer = &u8Buffer[0];

void ConfigLoadFactory(void)
{
    //
    // Copy the factory default parameter set to the active and working
    // parameter blocks.
    //
    g_sParameters = g_sParametersFactory;
    g_sWorkingDefaultParameters = g_sParametersFactory;
}

//*****************************************************************************
//
//! Loads the S2E parameter block from flash.
//!
//! This function is called to load the most recently saved parameter block
//! from flash.
//!
//! \return None.
//
//*****************************************************************************

void ConfigLoad(void)
{
//  uint8_t u8Buffer[512];
  uint8_t *pui8Buffer=&u8Buffer[0];
    //
    // Copy the factory default parameter set to the active and working
    // parameter blocks.
    //
    sFLASH_ReadBuffer((uint8_t *)&u8Buffer[0],FLASH_CFG_START_PARAM1, FLASH_PB_SIZE * 2);
    //
    // A parameter block was found so copy the contents to both our
    // active parameter set and the working default set.
    //
    g_sParameters = *(tConfigParameters *)pui8Buffer;
    g_sWorkingDefaultParameters = g_sParameters;
}
//void
//ConfigLoad(void)
//{
//    unsigned char *pucBuffer;
//
//    //
//    // Get a pointer to the latest parameter block in flash.
//    //
//    pucBuffer = FlashPBGet();
//
//    //
//    // See if a parameter block was found in flash.
//    //
//    if(pucBuffer)
//    {
//        //
//        // A parameter block was found so copy the contents to both our
//        // active parameter set and the working default set.
//        //
//        g_sParameters = *(tConfigParameters *)pucBuffer;
//        g_sWorkingDefaultParameters = g_sParameters;
//    }
//}
//
//*****************************************************************************
//
//! Saves the S2E parameter block to flash.
//!
//! This function is called to save the current S2E configuration parameter
//! block to flash memory.
//!
//! \return None.
//
//*****************************************************************************

void ConfigSave(void)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, i = 0;
  
  NumOfPage =  sizeof(tConfigParameters) / sFLASH_SPI_PAGESIZE;
  NumOfSingle = sizeof(tConfigParameters) % sFLASH_SPI_PAGESIZE;
  for(i = 0; i <= NumOfPage; i++)
  {
     sFLASH_EraseSector(FLASH_CFG_START_PARAM1 + sFLASH_SPI_PAGESIZE *i);
  }
    //
    // Save the working defaults parameter block to flash.
    //
    sFLASH_WriteBuffer((uint8_t *)g_psDefaultParameters, FLASH_CFG_START_PARAM1, sizeof(tConfigParameters));//FLASH_PB_SIZE * 2);
    TRACE_INFO("DONE Save!!!%d",g_sWorkingDefaultParameters.ucSequenceNum);
}

void ConfigSave2(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, i = 0;
  
  NumOfPage =  NumByteToWrite / sFLASH_SPI_PAGESIZE;
  NumOfSingle = NumByteToWrite % sFLASH_SPI_PAGESIZE;
  for(i = 0; i <= NumOfPage; i++)
  {
     sFLASH_EraseSector(WriteAddr + sFLASH_SPI_PAGESIZE *i);
  }
  //
  // Save the working defaults parameter block to flash.
  //
  sFLASH_WriteBuffer(pBuffer, WriteAddr, NumByteToWrite);
}

//void
//ConfigSave(void)
//{
//    unsigned char *pucBuffer;
//
//    //
//    // Save the working defaults parameter block to flash.
//    //
//    FlashPBSave((unsigned char *)&g_sWorkingDefaultParameters);
//
//    //
//    // Get the pointer to the most recenly saved buffer.
//    // (should be the one we just saved).
//    //
//    pucBuffer = FlashPBGet();
//
//    //
//    // Update the default parameter pointer.
//    //
//    if(pucBuffer)
//    {
//        g_psDefaultParameters = (tConfigParameters *)pucBuffer;
//    }
//    else
//    {
//        g_psDefaultParameters = (tConfigParameters *)g_psFactoryParameters;
//    }
//}
//
////*****************************************************************************
////
////! Initializes the configuration parameter block.
////!
////! This function initializes the configuration parameter block.  If the
////! version number of the parameter block stored in flash is older than
////! the current revision, new parameters will be set to default values as
////! needed.
////!
////! \return None.
////
////***************** ************************************************************

extern tHisFlashLog sHisFlashLog[5];
//extern tHisFlashLog sHisFlashLog1;
//extern tHisFlashLog sHisFlashLog2;
//extern tHisFlashLog sHisFlashLog3;
extern tHisFlashLog *pHisFlashLog;

void ConfigInit(void)
{
//  uint8_t u8Buffer[512];
  uint8_t *pui8Buffer = &u8Buffer[0];
//  tHisFlashLog sHisFlashLogBuf;
//  tHisFlashLog *pHisFlashLogBuf=&sHisFlashLogBuf;
  sFLASH_ReadBuffer((uint8_t *)&u8Buffer[0],FLASH_CFG_START_PARAM1, sizeof(tConfigParameters));//FLASH_PB_SIZE * 2);
  
  if (u8Buffer[3] != 0x80)
  {
    //
    // Copy the factory default parameter set to the active and working
    // parameter blocks.
    //
    g_sParameters = g_sParametersFactory;
    g_sWorkingDefaultParameters = g_sParametersFactory;
         
    sFLASH_WriteBuffer((uint8_t *)g_psDefaultParameters, FLASH_CFG_START_PARAM1, sizeof(tConfigParameters));//FLASH_PB_SIZE * 2);
    TRACE_INFO("Flash Not Write yet!...:%d\r\n",u8Buffer[0]);
//    ConfigLoad();    
  }
  else
  {
    TRACE_INFO("Read from Flash...:%d",u8Buffer[0]);
    //
    // A parameter block was found so copy the contents to both our
    // active parameter set and the working default set.
    //
    g_sParameters = *(tConfigParameters *)pui8Buffer;//pui8Buffer;
    g_sWorkingDefaultParameters = g_sParameters;
  }    

  if ((u8Buffer[888] != 0x66)&&(u8Buffer[888] != 0x55))
  {
    g_sParameters.u8FlagFreqTrap = 0x66;
    g_sParameters.u8DeltaFreqDisConnect[0] = 5;
    g_sParameters.u8DeltaFreqReConnect[0] = 3;
    g_sParameters.u8DeltaFreqDisConnect[1] = 5;
    g_sParameters.u8DeltaFreqReConnect[1] = 3;
    g_sParameters.u8EnableFreqTrap = 1;           //0 edit =1
    g_sParameters.u8DeltaFreqDisConnect2 = 5;
    g_sParameters.u8DeltaFreqReConnect2 = 3;
    g_sParameters.u8EnableFreqTrap2 = 0;
    g_sWorkingDefaultParameters = g_sParameters;
    ConfigSave();
    u8SaveConfigFlag &= ~(0x01);  
  } else if (u8Buffer[888] == 0x55)
  {
    g_sParameters.u8FlagFreqTrap = 0x66;
    g_sParameters.u8DeltaFreqDisConnect2 = 5;
    g_sParameters.u8DeltaFreqReConnect2 = 3;
    g_sParameters.u8EnableFreqTrap2 = 0;
    g_sWorkingDefaultParameters = g_sParameters;
    ConfigSave();
    u8SaveConfigFlag &= ~(0x01);  
  }
  //thanhcm3 fix ---------------------------------------------------------------
  if(u8Buffer[898]!= 0x66){
    g_sParameters.flag_config         = 0x66;
    g_sParameters.pm_delta_freq_v     = 0x64;
    g_sParameters.isense_delta_freq_v = 0x64;
    g_sWorkingDefaultParameters = g_sParameters;
    ConfigSave();
    u8SaveConfigFlag &= ~(0x01); 
  }
  if(u8Buffer[904]!= 0x66){
    g_sParameters.flag_config_1               = 0x66;
    g_sParameters.pm_cnt_timeout_freq_out     = 0x01;
    g_sParameters.pm_cnt_timeout_freq_in      = 0x3C; //0x0A edit = 0x3C
    g_sParameters.isense_cnt_timeout_freq_out = 0x01;
    g_sParameters.isense_cnt_timeout_freq_in  = 0x0A;
    
    g_sWorkingDefaultParameters = g_sParameters;
    ConfigSave();
    u8SaveConfigFlag &= ~(0x01); 
    
    
  }
  if(u8Buffer[909]!= 0x66)
  {
    g_sParameters.flag_config_2               = 0x66;
    g_sParameters.pm_set_cnt_remaining_gen    = 0x05; //0x3C edit = 0x05
      
    g_sWorkingDefaultParameters = g_sParameters;
    ConfigSave();
    u8SaveConfigFlag &= ~(0x01); 
  }
  
  //thanhcm3 fix ---------------------------------------------------------------  
  sFLASH_ReadBuffer((uint8_t *)&u8Buffer[0],FLASH_CFG_START_SN, FLASH_PB_SIZE);
  if (u8Buffer[0] == 0x53)
  {
    g_sParameters2 = *(tConfigParameters2 *)pui8Buffer;  
    g_sWorkingDefaultParameters2 = g_sParameters2;
  }
  else
  {
    g_sParameters2 = g_sParametersFactory2;
    g_sWorkingDefaultParameters2 = g_sParameters2;
  }
  sprintf((char*)g_sWorkingDefaultParameters2.u8CompileInfo,"%s %s", __DATE__, __TIME__);
  LoadHisLog();
  

  
}
//void
//ConfigInit(void)
//{
//    unsigned char *pucBuffer;
//
//    //
//    // Verify that the parameter block structure matches the FLASH parameter
//    // block size.
//    //
//    ASSERT(sizeof(tConfigParameters) == FLASH_PB_SIZE);
//
//    //
//    // Initialize the flash parameter block driver.
//    //
//    FlashPBInit(FLASH_CFG_START_PARAM1, FLASH_PB_END, FLASH_PB_SIZE);
//
//    //
//    // First, load the factory default values.
//    //
//    ConfigLoadFactory();
//
//    //
//    // Then, if available, load the latest non-volatile set of values.
//    //
//    ConfigLoad();
//
//    //
//    // Get the pointer to the most recently saved buffer.
//    //
//    pucBuffer = FlashPBGet();
//
//    //
//    // Update the default parameter pointer.
//    //
//    if(pucBuffer)
//    {
//        g_psDefaultParameters = (tConfigParameters *)pucBuffer;
//    }
//    else
//    {
//        g_psDefaultParameters = (tConfigParameters *)g_psFactoryParameters;
//    }
//}
void LoadHisLog(void)
{
  int i =0, j = 0; 
  for(i=0;i<5;i++)
  {
    pHisFlashLog = &sHisFlashLog[i];
    switch(i)
    {
    case 0:
           sFLASH_ReadBuffer((uint8_t *)pHisFlashLog,FLASH_PB_LOG_START, FLASH_PB_SIZE);
      break;
    case 1:
           sFLASH_ReadBuffer((uint8_t *)pHisFlashLog,FLASH_PB1_LOG_START, FLASH_PB_SIZE);
      break;
    case 2:
           sFLASH_ReadBuffer((uint8_t *)pHisFlashLog,FLASH_PB2_LOG_START, FLASH_PB_SIZE);
      break;
    case 3:
            sFLASH_ReadBuffer((uint8_t *)pHisFlashLog,FLASH_PB3_LOG_START, FLASH_PB_SIZE);
      break;
    case 4:
            sFLASH_ReadBuffer((uint8_t *)pHisFlashLog,FLASH_PB4_LOG_START, FLASH_PB_SIZE);
      break;
    default:
      break;
    }
   
    if(( sHisFlashLog[i].CODE[0]==0x09)&&(sHisFlashLog[i].CODE[1]==0x03))
      {
        //Do nothing
        if(j==0){
          if(sHisFlashLog[i].Count ==40){
            ih++;
            if(ih>4){
            sHisFlashLog[0].Count_old=0;
            sHisFlashLog[0].Count =0;
            ih=0;
            }
            }
          else j = 1;
        }        
      }
    else
      {
          sHisFlashLog[i].Count_old=0;
          sHisFlashLog[i].Count =0;
          sHisFlashLog[i].CODE[0]=0x09;
          sHisFlashLog[i].CODE[1]=0x03;
      }   
      HisCount_old[i] = sHisFlashLog[i].Count;
  }
  
}