/** Anvd33
** 12/4/2022
** Driver Vertiv M830B
**/

//include=================================
#include <stdint.h>
#include <stdbool.h>
#include <string.h> 

//Define==================================
#define  VERTIV_SLAVE_ID                 1
#define  Send_rs485_dc_cab               GPIO_SetPinsOutput(GPIOD,1u << 1u);
#define  Recive_rs485_dc_cab             GPIO_ClearPinsOutput(GPIOD,1u << 1u);
#define  FUNC_04                         04
#define  FUNC_03                         03
typedef struct{
 uint8_t        u8_buff[500];
 uint16_t       u16cnt;
}REV_485;
typedef struct{
  REV_485 rx_vertiv;
  uint8_t ID;
}VERTIV_T;
void VERTIV_init_rs485_uart();
extern VERTIV_T vertiv_t;

void Write_Multi_Reg_Vertiv(uint8_t slaveAddr, uint16_t regAddr, uint8_t *writeBuff, uint8_t size);
void Read_Regs_Query_Vertiv(uint8_t slaveAddr, uint16_t startingAddr, uint16_t numPoint, uint8_t fc);
int8_t Vertiv_Check_Respond_Data();
void Extract_Holding_Regs_Data_Vertiv();
void ModbusCRC1(unsigned char *str, unsigned char len);
void SetVar_i16_vertiv(uint8_t slaveAddr, uint16_t regAddr, uint16_t val);
int8_t Vertiv_Check_Respond_Setting_Data (void);
void Vertiv_updateSnmp(void);