#include <stdint.h>

#define MAX_S2E_PORTS           2
#define MOD_NAME_LEN            40

////*****************************************************************************
////
////! The address of the first block of flash to be used for storing parameters.
////
////*****************************************************************************
//#define FLASH_CFG_START_SN          0x000100//0x40FF00//0x000F0400//0x00FF00// External Flash Block 0 - Sector 0 - Line 1
//#define FLASH_CFG_START_FW          0x000200//0x40FF00//0x000F0400//0x00FF00// External Flash Block 0 - Sector 0 - Line 2
//#define FLASH_CFG_START_PARAM1          0x000300//0x40FF00//0x000F0400//0x00FF00// External Flash Block 0 - Sector 0 - Line 3
//#define FLASH_PB_LOG_START 0x001100// External Flash Block 0 - Sector 1 - Line 1
//#define FLASH_PB1_LOG_START 0x001200// External Flash Block 0 - Sector 1 - Line 2
//#define FLASH_PB2_LOG_START 0x001300// External Flash Block 0 - Sector 1 - Line 3
//#define FLASH_PB3_LOG_START 0x001400// External Flash Block 0 - Sector 1 - Line 4 
//
//#define FLASH_FW_START 0x010000// External Flash Block 1 - Sector 0 - Line 0 

//*****************************************************************************
//
//! The address of the last block of flash to be used for storing parameters.
//! Since the end of flash is used for parameters, this is actually the first
//! address past the end of flash.
//
//*****************************************************************************
#define FLASH_PB_END            0x410000//0x00070C00 //0x000F0C00 //0x0007FF00

//*****************************************************************************
//
//! The size of the parameter block to save.  This must be a power of 2,
//! and should be large enough to contain the tConfigParameters structure.
//
//*****************************************************************************
#define FLASH_PB_SIZE           256

//*****************************************************************************
//
//! This structure defines the port parameters used to configure the UART and
//! telnet session associated with a single instance of a port on the S2E
//! module.
//
//*****************************************************************************
typedef struct
{
    //
    //! The baud rate to be used for the UART, specified in bits-per-second
    //! (bps).
    //
    uint32_t u32BaudRate;

    //
    //! The data size to be use for the serial port, specified in bits.  Valid
    //! values are 5, 6, 7, and 8.
    //
    unsigned char ucDataSize;

    //
    //! The parity to be use for the serial port, specified as an enumeration.
    //! Valid values are 1 for no parity, 2 for odd parity, 3 for even parity,
    //! 4 for mark parity, and 5 for space parity.
    //
    unsigned char ucParity;

    //
    //! The number of stop bits to be use for the serial port, specified as
    //! a number.  Valid values are 1 and 2.
    //
    unsigned char ucStopBits;

    //
    //! The flow control to be use for the serial port, specified as an
    //! enumeration.  Valid values are 1 for no flow control and 3 for HW
    //! (RTS/CTS) flow control.
    //
    unsigned char ucFlowControl;

    //
    //! The controller Type
    //
    uint8_t u8ControllerType;

    //
    //! Miscellaneous flags associated with this connection.
    //
    unsigned char ucFlags;

    //! Padding to ensure consistent parameter block alignment, and
    //! to allow for future expansion of port parameters.
    //
    unsigned char ucReserved0[32];
}
tPortParameters;
//*****************************************************************************
//
//! This structure defines the port parameters used to configure the UART and
//! telnet session associated with a single instance of a port on the S2E
//! module.
//
//*****************************************************************************
typedef struct
{
    //
    //! The Type of Modbus device
    //! 
    //
    uint8_t u8DevType;
    //
    //! The Index of Modbus device
    //! 
    //
    uint8_t u8DevIndex;
    //
    //! The SubType of Modbus device
    //! 
    //
    uint8_t u8DevVendor;
    //
    //! The Modbus SlaveID
    //! 
    //
    uint32_t u32SlaveID;
    //
    //! The SubType of Modbus device
    //! 
    //
    uint8_t u8Reserved0;
}
tModbusParameters;
//*****************************************************************************
//
//! This structure contains the S2E module parameters that are saved to flash.
//! A copy exists in RAM for use during the execution of the application, which
//! is loaded from flash at startup.  The modified parameter block can also be
//! written back to flash for use on the next power cycle.
//
//*****************************************************************************
typedef struct
{
    //
    //! The sequence number of this parameter block.  When in RAM, this value
    //! is not used.  When in flash, this value is used to determine the
    //! parameter block with the most recent information.
    //
    unsigned char ucSequenceNum;

    //
    //! The CRC of the parameter block.  When in RAM, this value is not used.
    //! When in flash, this value is used to validate the contents of the
    //! parameter block (to avoid using a partially written parameter block).
    //
    unsigned char ucCRC;

    //
    //! The version of this parameter block.  This can be used to distinguish
    //! saved parameters that correspond to an old version of the parameter
    //! block.
    //
    unsigned char ucVersion;

    //
    //! Character field used to store various bit flags.
    //
    unsigned char ucFlags;

    //
    //! Padding to ensure consistent parameter block alignment.
    //
    unsigned char ucReserved1[17];

    //
    //! The configuration parameters for each port available on the S2E
    //! module.
    //
    tPortParameters sPort[2];

    //
    //! An ASCII string used to identify the module to users via UPnP and
    //! web configuration.
    //
    unsigned char siteName[40];

    //
    //! The static IP address to use if DHCP is not in use.
    //
    uint32_t u32StaticIP;

    //
    //! The default gateway IP address to use if DHCP is not in use.
    //
    uint32_t u32GatewayIP;

    //
    //! The subnet mask to use if DHCP is not in use.
    //
    uint32_t u32SubnetMask;	
	
    //
    //! The SNMP Server IP address to use if DHCP is not in use.
    //
    uint32_t u32SnmpIP;
	
    //
    //! The period of SNMP trap.
    //
    uint32_t u32TrapPeriod;
	
    //
    //! The SNMP trap mode
    //
    uint8_t u8TrapMode;

    //
    //! The Firmware version
    //
    uint32_t u32FirmwareCRC;    
    //
    //! Padding to ensure the whole structure is 256 bytes long.
    //
    unsigned char ucFTPServer[40];
    //
    //! Padding to ensure the whole structure is 256 bytes long.
    //
    uint32_t u32Reserved2[14];
//======================================= New Config =============================================//
    //
    //! Device number.
    //
    uint8_t u8DevNum[9];

    //
    //! Device Info.
    //! 0 - 15: Lithium
    //! 16 - 17: Gen
    //! 18 - 33: Lead-Acid BM
    //! 34 - 35: PM
    //! 36 - 37: VAC
    //! 38 - 42: SMCB
    //! 43 - 44: Fuel_Sensor
    //! 45: I_Sense
    //
    tModbusParameters sModbusParameters[50];
    
    //
    //! Freq Trap.
    //  
    uint8_t u8FlagFreqTrap;
    uint8_t u8DeltaFreqDisConnect[2];
    uint8_t u8DeltaFreqReConnect[2];
    uint8_t u8EnableFreqTrap;
    //
    //! ISENSE Freq Trap.
    //  
    uint8_t u8FlagFreqTrap2;
    uint8_t u8DeltaFreqDisConnect2;
    uint8_t u8DeltaFreqReConnect2;
    uint8_t u8EnableFreqTrap2;
    //
    //! Reserved3.
    //
    //thanhcm3 add config  for PM, ISENSE-------------------------------
    uint8_t     flag_config;
    uint16_t    pm_delta_freq_v;
    uint16_t    isense_delta_freq_v;
    //thanhcm3 add config for PM, ISENSE 1------------------------------
    uint8_t     flag_config_1;
    uint8_t     pm_cnt_timeout_freq_out;
    uint8_t     pm_cnt_timeout_freq_in;
    uint8_t     isense_cnt_timeout_freq_out;
    uint8_t     isense_cnt_timeout_freq_in;
    //thanhcm3 add config for check gen---------------------------------
    uint8_t     flag_config_2;
    uint16_t    pm_set_cnt_remaining_gen;
    uint8_t     reserved[3];
    uint32_t    u32Reserved3[15];
    
}
tConfigParameters;

//*****************************************************************************
//
//! This structure contains the S2E module parameters that are saved to flash.
//! A copy exists in RAM for use during the execution of the application, which
//! is loaded from flash at startup.  The modified parameter block can also be
//! written back to flash for use on the next power cycle.
//
//*****************************************************************************
typedef struct
{
    uint8_t u8IsWritten;
    uint8_t u8UserMAC[6];
    uint8_t u8UserSerialNo[20];   
    uint8_t u8Reserved1;
    uint8_t u8CompileInfo[28];    
    uint32_t u32FirmwareCRC_old;    
    uint32_t u32FirmwareCRC;
    uint32_t u32Reserved2[48];
}
tConfigParameters2;


////*****************************************************************************
////
////! This structure contains the S2E module parameters that are saved to flash.
////! A copy exists in RAM for use during the execution of the application, which
////! is loaded from flash at startup.  The modified parameter block can also be
////! written back to flash for use on the next power cycle.
////
////*****************************************************************************
//typedef struct
//{
//    //
//    //! The sequence number of this parameter block.  When in RAM, this value
//    //! is not used.  When in flash, this value is used to determine the
//    //! parameter block with the most recent information.
//    //
//    unsigned char ucSequenceNum;
//
//    //
//    //! The CRC of the parameter block.  When in RAM, this value is not used.
//    //! When in flash, this value is used to validate the contents of the
//    //! parameter block (to avoid using a partially written parameter block).
//    //
//    unsigned char ucCRC;
//
//    //
//    //! Reserved1.
//    //
//    uint32_t u32Reserved1[20];
//
//    //
//    //! Device number.
//    //
//    uint8_t u8DevNum;
//
//    //
//    //! Device Info.
//    //
//    tModbusParameters sModbusParameters[20];
//
//    //
//    //! Reserved2.
//    //
//    uint8_t u8Reserved2[13];
//
//    //
//    //! Reserved3.
//    //
//    uint32_t u32Reserved3[20];
//    
//}
//tConfigParameters3;

extern tConfigParameters2 g_sParameters2;
extern tConfigParameters2 g_sWorkingDefaultParameters2;

extern tConfigParameters g_sWorkingDefaultParameters;
extern tConfigParameters g_sParameters;
extern tConfigParameters *g_psDefaultParameters;
extern tConfigParameters2 *g_psDefaultParameters2;
extern const tConfigParameters *const g_psFactoryParameters;

extern void ConfigLoadFactory(void);
extern void ConfigLoad(void);
extern void ConfigSave(void);
extern void ConfigSave2(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite);
extern void ConfigInit(void);
extern void LoadHisLog(void);