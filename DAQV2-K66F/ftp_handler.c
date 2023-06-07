#include "ftp_handler.h"
#include "fsl_crc.h"
#include "Header.h"

extern error_t ftpTransferErr2;

extern TaskHandle_t * pModbusRTUTask;
extern TaskHandle_t * pEmersonDataProcessTask;
extern TaskHandle_t * pDKD51DataProcessTask;
extern tConfigHandler configHandle;

uint32_t pageToWrite = 0;
uint32_t byteToWrite = 0;
uint32_t totalByteToWrite = 0;

tFTPFileObject ftpFileObject[3] = 
{//====eUpdateDevType========fileName======================user==========password=====account
  {  _DAQ_FW_,          "DAQV2-K66F.hex",              "qltunguon",    "vtnet@2019",  ""},
  {  _VAC_FW_,          "MKE06_VAC03_6000_srec.hex",   "qltunguon",    "vtnet@2019",  ""},
  {  _VACV2_FW_,        "VAC_V2_MKE06_SREC.hex",       "qltunguon",    "vtnet@2019",  ""}
};

#if (USERDEF_CLIENT_FTP == ENABLED)

/**
 * @brief FTP client test routine
 * @return Error code
 **/
error_t ftpClientTest(void)
{
   error_t error;
   size_t length;
   IpAddr ipAddr;
   FtpClientContext ftpContext;
   
   memset(&ftpContext.buffer[0],0,sizeof(ftpContext.buffer));
   
   static char_t buffer[1024];
   configHandle.pageToWrite = 0;
   configHandle.byteToWrite = 0;
   configHandle.totalByteToWrite = 0;
   CRC_Type *base = CRC0;
   //Debug message
   TRACE_INFO("\r\n\r\nResolving server name...\r\n");
   //Resolve FTP server name
   error = getHostByName(NULL,privateMibBase.cfgNetworkGroup.cFTPServer, &ipAddr, 0);//ftp.gnu.org, privateMibBase.cfgNetworkGroup.cFTPServer
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_INFO("Failed to resolve server name!\r\n");
      //Exit immediately
      return error;
   }

   //Debug message
   TRACE_INFO("Connecting to FTP server %s\r\n", ipAddrToString(&ipAddr, NULL));
   //Connect to the FTP server
   error = ftpConnect(&ftpContext, NULL, &ipAddr, 8384, FTP_NO_SECURITY | FTP_PASSIVE_MODE);

   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_INFO("Failed to connect to FTP server!\r\n");
      //Exit immediately
      return error;
   }

   //Debug message
   TRACE_INFO("Successful connection\r\n");

   //Start of exception handling block
   do
   {
      //Login to the FTP server using the provided username and password
      error = ftpLogin(&ftpContext, ftpFileObject[configHandle.devType].user, ftpFileObject[configHandle.devType].password, ftpFileObject[configHandle.devType].account);
      //Any error to report?
      if(error) break;

      //Open the specified file for reading
      error = ftpOpenFile(&ftpContext, ftpFileObject[configHandle.devType].fileName, FTP_FOR_READING | FTP_BINARY_TYPE);
      //Any error to report?
      if(error) break;
      //Dump the contents of the file
      /* ***************
      * CRC-32 *
      *************** */
      InitCrc32(base, 0xFFFFFFFFU); 
      while(1)
      {
         //Read data
         error = ftpReadFile(&ftpContext, buffer, sizeof(buffer) - 1, &configHandle.byteToWrite, 0);
//         ftpTransferErr2 = error;
         //End of file?
         if(error) break;

         //Properly terminate the string with a NULL character
         buffer[configHandle.byteToWrite] = '\0';
         
         sFLASH_WriteBuffer((uint8_t *)&buffer[0], configHandle.totalByteToWrite + FLASH_FW_START, configHandle.byteToWrite);
         CRC_WriteData(base, (uint8_t *)&buffer[0], configHandle.byteToWrite);
         configHandle.totalByteToWrite += configHandle.byteToWrite;
         //Dump current data
//         TRACE_INFO("%s", buffer);       
      }  
      privateMibBase.siteGroup.siteFirmwareCRC = CRC_Get32bitResult(base);      

      //End the string with a line feed
//      TRACE_INFO("\r\n");
      //Close the file
      error = ftpCloseFile(&ftpContext);
      if (error) break;
      //End of exception handling block
   } while(0);
   ftpTransferErr2 = error;

   //Close the connection
   error = ftpClose(&ftpContext);
   
   //Debug message
   TRACE_INFO("Connection closed...\r\n");

   //Return status code
   return error;
}
//
///**
// * @brief FTP client test routine
// * @return Error code
// **/
//
//error_t ftpClientTest(void)
//{
//   error_t error;
//   size_t length;
//   IpAddr ipAddr;
//   FtpClientContext ftpContext;
//   static char_t buffer[1024];//256
////   pageToWrite = 0;
////   byteToWrite = 0;
////   totalByteToWrite = 0;
//   
//   configHandle.pageToWrite = 0;
//   configHandle.byteToWrite = 0;
//   configHandle.totalByteToWrite = 0;
//   CRC_Type *base = CRC0;
//   //Debug message
//   TRACE_INFO("\r\n\r\nResolving server name...\r\n");
//   //Resolve FTP server name
//   error = getHostByName(NULL,privateMibBase.cfgNetworkGroup.cFTPServer, &ipAddr, 0);//ftp.gnu.org
//
//   //Any error to report?
//   if(error)
//   {
//      //Debug message
//      TRACE_INFO("Failed to resolve server name!\r\n");
//      //Exit immediately
//      return error;
//   }
//
//   //Debug message
//   TRACE_INFO("Connecting to FTP server %s\r\n", ipAddrToString(&ipAddr, NULL));
//   //Connect to the FTP server
//   error = ftpConnect(&ftpContext, NULL, &ipAddr, 8384, FTP_NO_SECURITY | FTP_PASSIVE_MODE);
//
//   //Any error to report?
//   if(error)
//   {
//      //Debug message
//      TRACE_INFO("Failed to connect to FTP server!\r\n");
//      //Exit immediately
//      return error;
//   }
//
//   //Debug message
//   TRACE_INFO("Successful connection\r\n");
//
//   //Start of exception handling block
//   do
//   {
//      //Login to the FTP server using the provided username and password
//      error = ftpLogin(&ftpContext, ftpFileObject[configHandle.devType].user, ftpFileObject[configHandle.devType].password, ftpFileObject[configHandle.devType].account);
//      //Any error to report?
//      if(error) break;
//
//      //Open the specified file for reading
//      error = ftpOpenFile(&ftpContext, ftpFileObject[configHandle.devType].fileName, FTP_FOR_READING | FTP_BINARY_TYPE);
//      //Any error to report?
//      if(error) break;
//      //Dump the contents of the file
//      /* ***************
//      * CRC-32 *
//      *************** */
//      InitCrc32(base, 0xFFFFFFFFU); 
////      UART_Deinit(RS2321_UART);
////      UART_Deinit(RS4851R_UART);
////      vTaskDelete(pEmersonDataProcessTask);    
////      vTaskDelete(pModbusRTUTask);
//      while(1)
//      {
//         //Read data
//         error = ftpReadFile(&ftpContext, buffer, sizeof(buffer) - 1, &byteToWrite, 0);
////         ftpTransferErr2 = error;
//         //End of file?
//         if(error) break;
//
//         //Properly terminate the string with a NULL character
//         buffer[byteToWrite] = '\0';
//         
//         sFLASH_WriteBuffer((uint8_t *)&buffer[0], totalByteToWrite + FLASH_FW_START, byteToWrite);
//         CRC_WriteData(base, (uint8_t *)&buffer[0], byteToWrite);
//         totalByteToWrite += byteToWrite;
//         //Dump current data
////         TRACE_INFO("%s", buffer);       
//      }  
//      privateMibBase.siteGroup.siteFirmwareCRC = CRC_Get32bitResult(base);      
//
//      //End the string with a line feed
////      TRACE_INFO("\r\n");
//      //Close the file
//      error = ftpCloseFile(&ftpContext);
//      if (error) break;
//      //End of exception handling block
//   } while(0);
//   ftpTransferErr2 = error;
//
//   //Close the connection
//   error = ftpClose(&ftpContext);
//   
//   //Debug message
//   TRACE_INFO("Connection closed...\r\n");
//
//   //Return status code
//   return error;
//}

#endif