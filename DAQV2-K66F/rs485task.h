
#include "fsl_uart_freertos.h"
#include "fsl_uart.h"
#include "fsl_port.h"
#include "Header.h"


#if ((USERDEF_RS485_AGISSON == ENABLED)||(USERDEF_RS485_DKD51_BDP == ENABLED))
extern uint32_t iSet;
extern uint8_t Program_Status;
extern uint8_t Program_Status_old;
extern uint32_t RS485Timer;
extern uint32_t RS485Command;
extern uint32_t RecvCntEnd485;
extern uint32_t RecvOK;
extern uint32_t RecvCodeOK;      
extern uint8_t u8HisBuffer[256];
extern unsigned char g_UARTRxBuf485[300];
extern uint32_t RecvCntStart485;
extern uint32_t setuprepeat;
//-----------------------------------------------------------------------------
typedef struct{
  uint8_t    buff_485[500];
  uint16_t   cnt;
}RS_485_T;

typedef struct{
  RS_485_T uart_rx;
  RS_485_T rx;
  
  uint16_t check_sum_read;
  uint16_t check_sum_calculator;
  uint8_t  rect_number;
  uint8_t  var;
  uint32_t  fuse_flag;
}DPC_T;

extern DPC_T dpc_t;



void Agisson_Data_Process(void *pvParameters);
void DKD51_BDP_data_process(void *pvParameters);
void VERTIV_M830B_data_process(void *pvParameters);
#endif

