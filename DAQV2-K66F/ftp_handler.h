#if (USERDEF_CLIENT_FTP == ENABLED)
#include "ftp/ftp_client.h"
#endif

#define FILE_NAME_LENGTH 30
#define USER_NAME_LENGTH 30
#define PASSWORD_LENGTH 30
#define ACCOUNT_LENGTH 30
#define VAC_CHECK_FIRM_VERSION 200

extern uint32_t pageToWrite;
extern uint32_t byteToWrite;
extern uint32_t totalByteToWrite;



typedef enum
{  
  _DAQ_FW_,
  _VAC_FW_,
  _VACV2_FW_,
  _NO_FW_UPDATE = 0xff
} eUpdateDevType;

typedef struct
{
  eUpdateDevType  devType;
  char fileName[FILE_NAME_LENGTH];
  char user[USER_NAME_LENGTH];
  char password[PASSWORD_LENGTH];
  char account[ACCOUNT_LENGTH];  
} tFTPFileObject;

typedef struct
{
  eUpdateDevType  devType;
  error_t ftpTransferErr1;
  error_t ftpTransferErr2;
  uint32_t pageToWrite;
  uint32_t byteToWrite;
  uint32_t totalByteToWrite;  
  uint8_t isFwUpdate;
  uint8_t fwUpdateComplete;
  uint8_t numOfBlockToErase;
  uint8_t isReboot;
  uint8_t isSaveConfig;
  uint8_t isVACUpdate;
  uint8_t vacSendUpdateCtrl;
  uint8_t isSyncTime;
  
  uint32_t   Check_vac_firm_version;
  uint8_t    is_vac_v2_update;
  uint8_t    is_check_state_vac_v2;
  uint32_t   CRC32_calculator_vac_v2;
  uint8_t    buff_end_CRC32[19];
  char       crc32_vac_v2[20];
 
} tConfigHandler;

#if (USERDEF_CLIENT_FTP == ENABLED)
error_t ftpClientTest(void);
#endif
