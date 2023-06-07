#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_dspi.h"

/* Exported types ------------------------------------------------------------*/
typedef struct
{
    uint8_t ManufacturerID;
    uint8_t DeviceID[2];
}flashInfoTypedef;
extern flashInfoTypedef flashInfo;

/* Exported constants --------------------------------------------------------*/

/* M25P SPI Flash supported commands */
#define sFLASH_CMD_WREN           0x06  /* Write enable instruction */
#define sFLASH_CMD_RDSR           0x05  /* Read Status Register instruction  */
#define sFLASH_CMD_WRSR           0x01  /* Write Status Register instruction */
#define sFLASH_CMD_READ           0x03  /* Read from Memory instruction */

#define sFLASH_CMD_WRITE          0x02  /* Write to Memory instruction */
#define sFLASH_CMD_SE             0x20  /* Sector Erase instruction */
#define sFLASH_CMD_BE             0xD8//0x52  /* Bulk Erase instruction */
#define sFLASH_CMD_CE             0xC7  /* Chip Erase instruction */
#define sFLASH_CMD_PP             0x02  /* Page Program instruction */
#define sFLASH_CMD_RDID           0x9F  /* Read identification */

#define sFLASH_WIP_FLAG           0x01  /* Write In Progress (WIP) flag */

#define sFLASH_DUMMY_BYTE         0xA5
#define sFLASH_SPI_PAGESIZE       0x100

#define sFLASH_M25P128_ID         0x202018
#define sFLASH_M25P64_ID          0x202017
#define sFLASH_W25Q80_ID          0xef4014
#define sFLASH_W25Q16_ID          0xef4015
#define sFLASH_W25Q32_ID          0xef4016
//#define Check_Flash_W25Q32        sFLASH_ReadID
#define LOW    	 			0x00  /* Chip Select line low */
#define HIGH    			   0x01  /* Chip Select line high */
#define SET    			   0x01  /*  */
/* M25P FLASH SPI Interface pins  */
//#define sFLASH_SPI                           SPI1
#define SEC_SIZE    0x1000

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/* High layer functions  */
void sFLASH_Init(void);
void sFLASH_EraseSector(uint32_t SectorAddr);
void sFLASH_EraseBulk(uint32_t BlockAddr);
void sFLASH_EraseChip(void);
void sFLASH_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void sFLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void sFLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void sFLASH_ReadID(flashInfoTypedef * pFlashInfo);
void sFLASH_StartReadSequence(uint32_t ReadAddr);

/* Low layer functions */

void sFLASH_ChipSelect(uint8_t State);
uint8_t sFLASH_WriteByte(uint8_t byte);
uint8_t sFLASH_ReadByte(void);
void sFLASH_WriteEnable(void);
void sFLASH_WaitForWriteEnd(void);
uint16_t sFLASH_SendHalfWord(uint16_t HalfWord);

#ifdef __cplusplus
}
#endif

#endif /* __SPI_FLASH_H */
