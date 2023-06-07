#include"Header.h"
#if (USERDEF_RS485_AGISSON == ENABLED)

uint8_t Batt_Charge_Volt[15]={0x00,0x0C,0xC9,0x81,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t Batt_Info[21]={0x00,0x12,0xC9,0x82,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t LVD[26]={0x00,0x17,0xC9,0x83,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x55,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00};
uint8_t Batt_Temp[44]={0x00,0x29,0xC9,0x85,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t Volt_Limit[23]={0x00,0x14,0xC9,0x87,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t Env_Info[39]={0x00,0x24,0xC9,0x88,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t Batt_HTD[11]={0x00,0x08,0xC9,0x8A,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00};
uint8_t Batt_Test[65] = {0x00,0x3E,0xC9,0x8C,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
uint8_t Batt_Manual_Test_En[8] = {0x00,0x05,0xC9,0x9D,0xFF,0xFF,0x01,0x6A};
uint8_t Batt_Short_Test_En[8] = {0x00,0x05,0xC9,0x9D,0xFF,0xFF,0x02,0x6B};
uint8_t Batt_Test_Stop[8] = {0x00,0x05,0xC9,0x9D,0xFF,0xFF,0x55,0xBE};
uint16_t HisCount_old[5] ={0,0,0,0,0};
tHisFlashLog sHisFlashLog[5];
uint8_t ih=0;
uint8_t thao=0;
uint8_t thao1=0;


//uint8_t remainBat =0;
tHisFlashLog *pHisFlashLog = &sHisFlashLog[0];

struct uartinfoget Uart_array_data_get;
struct uartget Uart_data_get;
struct uartinfo Uart_data;
static unsigned char AcmdCodeTable[31][9] = {
{0x00,	0x04,	0xC9,	0x20,	0xFF,	0xFF,   0xEB,   0x00,   0x00},      // 0 Dien ap tai va dong nap acquy

{0x00,	0x04,	0xC9,	0x21,	0xFF,	0xFF,   0xEC,   0x00,   0x00},      // 1  Battery temperature

{0x00,	0x05,	0xC9,	0xA0,	0xFF,	0xFF,   0x01,   0x6D,   0x00},      // 2

{0x00,	0x05,	0xC9,	0xA0,	0xFF,	0xFF,   0x02,   0x6E,   0x00},       // 3

{0x00,	0x05,	0xC9,	0xA0,	0xFF,	0xFF,   0x03,   0x6F,   0x00},      //4

{0x00,	0x05,	0xC9,	0xA0,	0xFF,	0xFF,   0x04,   0x70,   0x00},       //5

{0x00,	0x05,	0xC9,	0xA0,	0xFF,	0xFF,   0x05,   0x71,   0x00},       //6

{0x00,	0x05,	0xC9,	0xA0,	0xFF,	0xFF,   0x06,   0x72,   0x00},       //7

{0x00,	0x05,	0xC9,	0xA0,	0xFF,	0xFF,   0x07,   0x73,   0x00},       //8

{0x00,	0x04,	0xC9,	0x02,	0xFF,	0xFF,   0xCD,   0x00,   0x00},       //9

{0x00,	0x04,	0xC9,	0x30,	0xFF,	0xFF,   0xFB,   0x00,   0x00},       //10  Cac canh bao, trang thai cac rect

{0x00,	0x04,	0xC9,	0x40,	0xFF,	0xFF,   0x0B,   0x00,   0x00},        //11

{0x00,	0x05,	0xC9,	0x41,	0xFF,	0xFF,   0x00,   0x0D,   0x00},         //12

{0x00,	0x05,	0xC9,	0x41,	0xFF,	0xFF,   0x01,   0x0E,   0x00},         //13

{0x00,	0x05,	0xC9,	0x41,	0xFF,	0xFF,   0x02,   0x0F,   0x00},          //14

{0x00,	0x04,	0xC9,	0x50,	0xFF,	0xFF,   0x1B,   0x00,   0x00},         //15

{0x00,	0x04,	0xC9,	0x51,	0xFF,	0xFF,   0x1C,   0x00,   0x00},         //16

{0x00,	0x04,	0xC9,	0x8B,	0xFF,	0xFF,   0x56,   0x00,   0x00},        //17

{0x00,	0x04,	0xC9,	0x8D,	0xFF,	0xFF,   0x58,   0x00,   0x00},         //18  Cac thong so cai dat test battery

{0x00,	0x06,	0xC9,	0x8E,	0xFF,	0xFF,   0x02,   0x00,   0x5D},         //19

{0x00,	0x04,	0xC9,	0x90,	0xFF,	0xFF,   0x5B,   0x00,   0x00},         //20

{0x00,	0x04,	0xC9,	0x91,	0xFF,	0xFF,   0x5C,   0x00,   0x00},          //21    Dien ap boost, dien ap float

{0x00,	0x04,	0xC9,	0x92,	0xFF,	0xFF,   0x5D,   0x00,   0x00},         //22    Dung luong acquy, he so dong nap gioi han, so to acquy

{0x00,	0x04,	0xC9,	0x93,	0xFF,	0xFF,   0x5E,   0x00,   0x00},         //23    LLVD, BLVD

{0x00,	0x04,	0xC9,	0x95,	0xFF,	0xFF,   0x60,   0x00,   0x00},         //       Battery high temp alarm

{0x00,	0x04,	0xC9,	0x98,	0xFF,	0xFF,   0x63,   0x00,   0x00},        //25      Ac low va dc low

{0x00,	0x04,	0xC9,	0x99,	0xFF,	0xFF,   0x64,   0x00,   0x00},         //26

{0x00,	0x04,	0xC9,	0xA9,	0xFF,	0xFF,   0x74,   0x00,   0x00},         //27

{0x00,	0x04,	0xC9,	0xB0,	0xFF,	0xFF,   0x7B,   0x00,   0x00},         //28

{0x00,	0x04,	0xC9,	0xF8,	0xFF,	0xFF,   0xC3,   0x00,   0x00},         //29        Dien ap va dong dien cac rect

{0x00,	0x04,	0xC9,	0xA4,	0xFF,	0xFF,   0x6F,   0x00,   0x00},         
};
uint8_t StartMesg01[7] = {0x00,0x04,0xC9,0x03,0xFF,0xFF,0xCE};
uint8_t StartMesg0[11]={0x00,0x08,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x05};
uint8_t StartMesg1[22]={0x23,0x7F,0xC3,0x00,0x04,0xC9,0x01,0xFF,0xFF,0xCC,0xC3,0x00,0x05,0xC9,0x60,0xFF,0xFF,0x07,0x33,0x23,0x23,0x23};
uint8_t StartMesg2[12] = {0xC3,0x00,0x05,0xC9,0x60,0xFF,0xFF,0x07,0x33,0x23,0x23,0x23};
uint8_t StartMesg3[12] = {0xC3,0x00,0x05,0xC9,0x60,0xFF,0xFF,0x07,0x33,0x23,0x23,0x23};
uint8_t StartMesg4[46] = {0xC3,0x00,0x04,0xC9,0xA4,0xFF,0xFF,0x6F,0x23,0x23,0x7F,0xC3,0x00,0x1C,0xC9,0xC9,0xFF,0xFF,0xC0,0xA8,0x00,0x0A,0xFF,0xFF,0x00,0x00,0xC0,0xA8,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x79,0xF4,0xF2,0x23,0x23,0x23};
uint8_t StartMesg5[35] = {0xC3,0x00,0x1C,0xC9,0xC9,0xFF,0xFF,0xC0,0xA8,0x00,0x0A,0xFF,0xFF,0x00,0x00,0xC0,0xA8,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x79,0xF4,0xF2,0x23,0x23,0x23};
uint8_t StartMesg6[35] = {0xC3,0x00,0x1C,0xC9,0xC9,0xFF,0xFF,0xC0,0xA8,0x00,0x0A,0xFF,0xFF,0x00,0x00,0xC0,0xA8,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x79,0xF4,0xF2,0x23,0x23,0x23};
uint8_t StartMesg7[35] = {0xC3,0x00,0x1C,0xC9,0xC9,0xFF,0xFF,0xC0,0xA8,0x00,0x0A,0xFF,0xFF,0x00,0x00,0xC0,0xA8,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x79,0xF4,0xF2,0x23,0x23,0x23};
uint8_t StartMesg8[35] = {0xC3,0x00,0x1C,0xC9,0xC9,0xFF,0xFF,0xC0,0xA8,0x00,0x0A,0xFF,0xFF,0x00,0x00,0xC0,0xA8,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x79,0xF4,0xF2,0x23,0x23,0x23};
uint8_t StartMesg9[35] = {0xC3,0x00,0x1C,0xC9,0xC9,0xFF,0xFF,0xC0,0xA8,0x00,0x0A,0xFF,0xFF,0x00,0x00,0xC0,0xA8,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x79,0xF4,0xF2,0x23,0x23,0x23};
uint8_t StartMesg10[22]={0xC3,0x00,0x0F,0xC9,0x86,0xFF,0xFF,0x00,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0x00,0x23,0x23,0x23};

uint32_t iCounter=0;
uint32_t RS485Period=0;
//uint32_t iSet =0;
void NextHis(void)
{
    uint16_t j;
    sHisFlashLog[ih].Count++;
    j =  sHisFlashLog[ih].Count-1;
    if(sHisFlashLog[ih].Count>39)
    {
      ih++;
      if(ih>4)
      {ih=0;
      sHisFlashLog[ih].Count = 0;}
      if(sHisFlashLog[ih].Count>39)    sHisFlashLog[ih].Count = 0;
    }
    if(TYPE ==2)
    {
      sHisFlashLog[ih].HisFlashEvent[j].Year  =   Uart_data_get.Year;
      sHisFlashLog[ih].HisFlashEvent[j].Month =   Uart_data_get.Month;
      sHisFlashLog[ih].HisFlashEvent[j].Day   =   Uart_data_get.Day;
      sHisFlashLog[ih].HisFlashEvent[j].Hour  =   Uart_data_get.Hour;
      sHisFlashLog[ih].HisFlashEvent[j].Minute=   Uart_data_get.Minute;
    }
    if(TYPE ==3)
    {
      sHisFlashLog[ih].HisFlashEvent[j].Year  =   EMERV21Time.Year;
      sHisFlashLog[ih].HisFlashEvent[j].Month =   EMERV21Time.Month;
      sHisFlashLog[ih].HisFlashEvent[j].Day   =   EMERV21Time.Date;
      sHisFlashLog[ih].HisFlashEvent[j].Hour  =   EMERV21Time.Hour;
      sHisFlashLog[ih].HisFlashEvent[j].Minute=   EMERV21Time.Minute;
    }

}
void delay(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < 13000; ++i)
    {
        __asm("NOP"); /* delay */
    }
}
uint8_t ChecksumH(uint8_t *Datachecksum,uint8_t size)
{
	uint8_t sum=0;
	uint8_t i;
	for(i=1;i<size;i++)
	{
		sum  = sum + Datachecksum[i];
	}
	return(sum);
}
void Send_StartMesg0(uint32_t RS485Timer)
{
  
  if((RS485Timer == 1)||(RS485Timer == 100)||(RS485Timer == 200)||(RS485Timer == 300))
  {
    RecvCntStart485 =0;
//    if(RS485Timer ==1)iCounter =0;
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        if(RecvOK==1)
        {
          UART_9Bit_Send(RS485T_UART,0x7F,0);
        }
        else
        {
        UART_9Bit_Send(RS485T_UART,0x23,1);
        }
        RecvOK=0;
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
//        iCounter++;
  }
  
  if( (RS485Timer >310) &&(RS485Timer <322))
  {
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        UART_9Bit_Send(RS485T_UART,StartMesg0[RS485Timer-311],0);
//        iCounter++;
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if((Recv03 ==0x01)&& (RS485Timer==332))
    {
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        UART_9Bit_Send(RS485T_UART,0xC3,1);
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    }
  
  if((RS485Timer >342) &&(RS485Timer <350))
  {
    if(Recv03 ==0x01)
    {
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        UART_9Bit_Send(RS485T_UART,StartMesg01[RS485Timer-343],0);
//        iCounter++;
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
       
    }
  }
    if((Recv03 ==0x01)&& (RS485Timer==362))
    {
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        UART_9Bit_Send(RS485T_UART,0x23,1);
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
        Recv03=0;
    }
}

void Send_StartMesg1(uint32_t RS485Timer)
{
  
  if((RS485Timer == 1)||(RS485Timer == 133)||(RS485Timer == 233)||(RS485Timer == 433))
  {
    RecvCntStart485 =0;
    if(RS485Timer ==1)iCounter =0;
        GPIO_SetPinsOutput(GPIOD,1u << 1u);

        UART_9Bit_Send(RS485T_UART,0x23,1);
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
        iCounter++;
  }
  if(RS485Timer == 333)
  {
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        
        UART_9Bit_Send(RS485T_UART,0x7F,0);
        UART_9Bit_Send(RS485T_UART,0x23,1);
        delay();
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
//        iCounter++;
//        iCounter++;
  }
  if(RS485Timer == 100)
  {
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        
        UART_9Bit_Send(RS485T_UART,0x7F,0);
        UART_9Bit_Send(RS485T_UART,0xC3,1);
        delay();
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
        iCounter++;
        iCounter++;
  }
    if(RS485Timer == 116)
  {
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        
        UART_9Bit_Send(RS485T_UART,0xC3,1);
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
        iCounter++;
  }
  
  if( (RS485Timer == 105)|| (RS485Timer == 106)|| (RS485Timer == 107)|| (RS485Timer == 108)|| (RS485Timer == 109)|| (RS485Timer == 110)              
               || (RS485Timer == 111)||  (RS485Timer == 121)|| (RS485Timer == 122)|| (RS485Timer == 123)|| (RS485Timer == 124)    
               || (RS485Timer == 125)|| (RS485Timer == 126)|| (RS485Timer == 127)|| (RS485Timer == 128) )
  {
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        UART_9Bit_Send(RS485T_UART,StartMesg1[iCounter],0);
        iCounter++;
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
}

void Send_StartMesg23(uint32_t RS485Timer)
{
  if(RS485Timer == 1)
  {
        iCounter=0;
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        
        UART_9Bit_Send(RS485T_UART,0xC3,1);
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
        iCounter++;
  }
  if( (RS485Timer >= 5)&&(RS485Timer <= 12) )
  {
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        UART_9Bit_Send(RS485T_UART,StartMesg2[iCounter],0);
        iCounter++;
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if((RS485Timer == 16)||(RS485Timer == 116)||(RS485Timer == 216))
  {
        RecvCntStart485 =0;
        GPIO_SetPinsOutput(GPIOD,1u << 1u);

        UART_9Bit_Send(RS485T_UART,0x23,1);
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
        iCounter++;
  }
}

void Send_StartMesg4(uint32_t RS485Timer)
{
  if(RS485Timer == 1)
  {
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        
        UART_9Bit_Send(RS485T_UART,0xC3,1);
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
        iCounter++;
  }
  if((RS485Timer>16)&&(RS485Timer <24))
  {
     GPIO_SetPinsOutput(GPIOD,1u << 1u);
        UART_9Bit_Send(RS485T_UART,StartMesg4[iCounter],0);
        iCounter++;
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  
  }
  if((RS485Timer == 31)||(RS485Timer == 131)||(RS485Timer == 310)||(RS485Timer == 426)) //||(RS485Timer == 526)
  {
        RecvCntStart485 =0;
        GPIO_SetPinsOutput(GPIOD,1u << 1u);

        UART_9Bit_Send(RS485T_UART,0x23,1);
        delay();
        RecvCntStart485=0;
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
        iCounter++;
  }
  if(RS485Timer ==1)iCounter =1;
  if( (RS485Timer == 231)|| ((RS485Timer > 250)&& (RS485Timer <282)) )
  {
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        UART_9Bit_Send(RS485T_UART,StartMesg4[iCounter],0);
        iCounter++;
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
    if(RS485Timer == 243)
  {
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        
        UART_9Bit_Send(RS485T_UART,0xC3,1);
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
        iCounter++;
  }
      if(RS485Timer == 526)
  {
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        
        UART_9Bit_Send(RS485T_UART,0x7F,0);
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
//        iCounter++;
  }
}

void Send_StartMesg56789(uint32_t RS485Timer)
{

  if(RS485Timer == 1)
  {
        iCounter=0;
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        
        UART_9Bit_Send(RS485T_UART,0xC3,1);
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
        iCounter++;
  }
  
  if(  (RS485Timer >5)&& (RS485Timer < 37) )
  {
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        UART_9Bit_Send(RS485T_UART,StartMesg5[iCounter],0);
        iCounter++;
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
    if((RS485Timer == 42)||(RS485Timer == 142))//||(RS485Timer == 242)
  {
        RecvCntStart485 =0;
        GPIO_SetPinsOutput(GPIOD,1u << 1u);

        UART_9Bit_Send(RS485T_UART,0x23,1);
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
        iCounter++;
  }
      if(RS485Timer == 242)
  {
        RecvCntStart485 =0;
        GPIO_SetPinsOutput(GPIOD,1u << 1u);

        UART_9Bit_Send(RS485T_UART,0x7F,0);
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
        iCounter++;
  }
}

void Send_StartMesg10(uint32_t RS485Timer)
{

  if(RS485Timer == 1)
  {
        iCounter=0;
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        
        UART_9Bit_Send(RS485T_UART,0xC3,1);
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
        iCounter++;
  }
  
  if( (RS485Timer > 6)&& (RS485Timer <25))
  {
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        UART_9Bit_Send(RS485T_UART,StartMesg10[iCounter],0);
        iCounter++;
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
    if((RS485Timer == 31)||(RS485Timer == 131)) //||(RS485Timer == 231)
  {
        RecvCntStart485 =0;
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        UART_9Bit_Send(RS485T_UART,0x23,1);
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
        iCounter++;
  }
    if(RS485Timer == 231)
  {
        RecvCntStart485 =0;
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        
        UART_9Bit_Send(RS485T_UART,0x7F,0);
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
//        iCounter++;
  }
  
}
void Send_RequestMesg(uint32_t RS485Timer,uint32_t RS485Command)
{
  if(RS485Timer == 1)
  {
    if( RS485Command!=0)
    {   
      if(RecvOK==1)
      {
        GPIO_SetPinsOutput(GPIOD,1u << 1u);       
        UART_9Bit_Send(RS485T_UART,0x7F,0);
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
      }

    }

  }
  if(RS485Timer == 11)
  {
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        UART_9Bit_Send(RS485T_UART,0xC3,1);
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if((RS485Command==0)||(RS485Command==1)||((RS485Command>8)&&(RS485Command<12))||((RS485Command>14)&&(RS485Command<31)))
    RS485Period = 17+7;
  
 if((RS485Command==12)||(RS485Command==13)||(RS485Command==14)||((RS485Command>1)&&(RS485Command<9)))
    RS485Period = 17+8;
// if((RS485Command==13)||(RS485Command==14))
//    RS485Period = 17+9;
 if(RS485Command==19)
    RS485Period = 17+10;
   if( (RS485Timer > 16)&&(RS485Timer < RS485Period))
  {
        if(RS485Timer == 17)iCounter=0;
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        UART_9Bit_Send(RS485T_UART,AcmdCodeTable[RS485Command][iCounter],0);
        iCounter++;
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if(RS485Timer == (RS485Period+8))
  {     
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        UART_9Bit_Send(RS485T_UART,0x23,1);
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);
        RX485Count =0;
  }  
  if(RS485Timer == 100)
  {
        RecvCntStart485 =0;
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        UART_9Bit_Send(RS485T_UART,0x23,1);  
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);   
  }  
   if(( RS485Command < 30)&&(RS485Timer==180))
  {
    if(RecvOK ==0)
    {
        RecvCntStart485 =0;
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        UART_9Bit_Send(RS485T_UART,0x23,1);  
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);  
    }
  }  
  if(( RS485Command == 30)&&(RS485Timer==200))
//  if(( RS485Command == 29)&&(RS485Timer==200))
  {
        RecvCntStart485 =0;
        GPIO_SetPinsOutput(GPIOD,1u << 1u);
        UART_9Bit_Send(RS485T_UART,0x7F,0);  
        delay();
        GPIO_ClearPinsOutput(GPIOD,1u << 1u);   
  }
//    if(( RS485Command == 30)&&(RS485Timer==300))
//  {
//        RecvCntStart485 =0;
//        GPIO_SetPinsOutput(GPIOD,1u << 1u);
//        UART_9Bit_Send(RS485T_UART,0x23,1);  
//        delay();
//        GPIO_ClearPinsOutput(GPIOD,1u << 1u);   
//  }
}
uint8_t CPMU01_Ver_IP =1;
 uint8_t cs,size;
 uint8_t inverse[4];
 uint32_t Cab_Connection=0;
void Recv_Mesg(void)
{
  uint32_t i,j;
  if(RecvCntStart485 ==0)Cab_Connection++;
  if(Cab_Connection>10)
  {
    Program_Status=0;
    RS485Timer =1;
    RS485Command =1;
  }
  
  
  if((RecvCntStart485>2)&&(RecvCntStart485>(g_UARTRxBuf485[2]+3))) 
  {
     size=(g_UARTRxBuf485[2]+3);
     cs = ChecksumH(&g_UARTRxBuf485[0],size);
     if(cs==g_UARTRxBuf485[(size)])
     {
             switch(g_UARTRxBuf485[4])
             {
             case 0x03:
               CPMU01_Ver_IP =1;
             case 0x01:
               Program_Status =1;
               break;
             case 0x20:
                     i=0;j=8;
                     for(i=0;i<4;i++)
                     {
                             Uart_array_data_get.V_load [i]=g_UARTRxBuf485[j];
                             inverse[3-i]=Uart_array_data_get.V_load [i];
                             j++;
                     }
                     memcpy(&sDcInfo.sINFODcInfo.fVoltage,&inverse,4);
                     //privateMibBase.configGroup.cfgFloatVolt = (int32_t) (sDcInfo.sINFODcInfo.fVoltage * 100);
                     privateMibBase.batteryGroup.battVolt  =  (int32_t) (sDcInfo.sINFODcInfo.fVoltage * 100);
                     for(i=0;i<4;i++)
                     {
                             Uart_array_data_get.I_load [i]=g_UARTRxBuf485[j];
                             inverse[3-i]=Uart_array_data_get.I_load [i];
                             j++;
                     }
                     memcpy(&sDcInfo.sINFODcInfo.fCurrent,&inverse,4);
                     privateMibBase.loadGroup.loadCurrent = (uint32_t)(sDcInfo.sINFODcInfo.fCurrent*100);

                     for(i=0;i<4;i++)
                     {
                             Uart_array_data_get.I_charge [i]=g_UARTRxBuf485[j];
                             inverse[3-i]=Uart_array_data_get.I_charge [i];
                             j++;
                     }
                     memcpy(&sDcInfo.sINFODcInfo.fBatt1Curr,&inverse,4);
                     privateMibBase.batteryGroup.battCurr =(int32_t) (sDcInfo.sINFODcInfo.fBatt1Curr*100);
                     j=j+4;
                     for(i=0;i<4;i++)
                     {
                             Uart_array_data_get.VAC [i]=g_UARTRxBuf485[j];
                             inverse[3-i]=Uart_array_data_get.VAC [i];
                             j++;
                     }
                     memcpy(&sAcInfo.facVolt[0],&inverse,4);
                     privateMibBase.acPhaseGroup.acPhaseTable[0].acPhaseVolt = (int32_t) (sAcInfo.facVolt[0] * 100);
                     break;
             case 0xF8:
                     i=0;j=8;
                     Uart_array_data_get.Rect1 = g_UARTRxBuf485[j];
                     j++;
                     for(i=0;i<4;i++)
                     {
                             Uart_array_data_get.Rect1_V [i]=g_UARTRxBuf485[j];
                             inverse[3-i]=Uart_array_data_get.Rect1_V [i];
                             j++;
                     }
                     memcpy(&Uart_data_get.Rect1_V,&inverse,4);
                     privateMibBase.rectGroup.rectTable[0].rectOutputVoltage = (uint32_t) (Uart_data_get.Rect1_V * 100);
                     for(i=0;i<4;i++)
                     {
                             Uart_array_data_get.Rect1_I [i]=g_UARTRxBuf485[j];
                             inverse[3-i]=Uart_array_data_get.Rect1_I [i];
                             j++;
                     }
                     memcpy(&Uart_data_get.Rect1_I,&inverse,4);
                     privateMibBase.rectGroup.rectTable[0].rectOutputCurrent = (uint32_t) (Uart_data_get.Rect1_I * 100);
                     Uart_array_data_get.Rect2 = g_UARTRxBuf485[j];
                     j++;
                     for(i=0;i<4;i++)
                     {
                             Uart_array_data_get.Rect2_V [i]=g_UARTRxBuf485[j];
                             inverse[3-i]=Uart_array_data_get.Rect2_V [i];
                             j++;
                     }
                     memcpy(&Uart_data_get.Rect2_V,&inverse,4);
                     privateMibBase.rectGroup.rectTable[1].rectOutputVoltage = (uint32_t) (Uart_data_get.Rect2_V * 100);
                     for(i=0;i<4;i++)
                     {
                             Uart_array_data_get.Rect2_I [i]=g_UARTRxBuf485[j];
                             inverse[3-i]=Uart_array_data_get.Rect2_I [i];
                             j++;
                     }
                     memcpy(&Uart_data_get.Rect2_I,&inverse,4);
                     privateMibBase.rectGroup.rectTable[1].rectOutputCurrent = (uint32_t) (Uart_data_get.Rect2_I * 100);
                     Uart_array_data_get.Rect3 = g_UARTRxBuf485[j];
                     j++;
                     for(i=0;i<4;i++)
                     {
                             Uart_array_data_get.Rect3_V [i]=g_UARTRxBuf485[j];
                             inverse[3-i]=Uart_array_data_get.Rect3_V [i];
                             j++;
                     }
                     memcpy(&Uart_data_get.Rect3_V,&inverse,4);
                     privateMibBase.rectGroup.rectTable[2].rectOutputVoltage = (uint32_t) (Uart_data_get.Rect3_V * 100);
                     for(i=0;i<4;i++)
                     {
                             Uart_array_data_get.Rect3_I [i]=g_UARTRxBuf485[j];
                             inverse[3-i]=Uart_array_data_get.Rect3_I [i];
                             j++;
                     }
                     memcpy(&Uart_data_get.Rect3_I,&inverse,4);
                     privateMibBase.rectGroup.rectTable[2].rectOutputCurrent =(uint32_t)(Uart_data_get.Rect3_I * 100);
                     Uart_array_data_get.Rect4 = g_UARTRxBuf485[j];
                     j++;
                     for(i=0;i<4;i++)
                     {
                             Uart_array_data_get.Rect4_V [i]=g_UARTRxBuf485[j];
                             inverse[3-i]=Uart_array_data_get.Rect4_V [i];
                             j++;
                     }
                     memcpy(&Uart_data_get.Rect4_V,&inverse,4);
                     privateMibBase.rectGroup.rectTable[3].rectOutputVoltage = (uint32_t) (Uart_data_get.Rect4_V * 100);
                     for(i=0;i<4;i++)
                     {
                             Uart_array_data_get.Rect4_I [i]=g_UARTRxBuf485[j];
                             inverse[3-i]=Uart_array_data_get.Rect4_I [i];
                             j++;
                     }
                     memcpy(&Uart_data_get.Rect4_I,&inverse,4);
                     privateMibBase.rectGroup.rectTable[3].rectOutputCurrent =(uint32_t) (Uart_data_get.Rect4_I * 100);
                     Uart_array_data_get.Rect5 = g_UARTRxBuf485[j];
                     j++;
                     for(i=0;i<4;i++)
                     {
                             Uart_array_data_get.Rect5_V [i]=g_UARTRxBuf485[j];
                             inverse[3-i]=Uart_array_data_get.Rect5_V [i];
                             j++;
                     }
                     memcpy(&Uart_data_get.Rect5_V,&inverse,4);
                     privateMibBase.rectGroup.rectTable[4].rectOutputVoltage = (uint32_t) (Uart_data_get.Rect5_V * 100);
                     for(i=0;i<4;i++)
                     {
                             Uart_array_data_get.Rect5_I [i]=g_UARTRxBuf485[j];
                             inverse[3-i]=Uart_array_data_get.Rect5_I [i];
                             j++;
                     }
                     memcpy(&Uart_data_get.Rect5_I,&inverse,4);
                     privateMibBase.rectGroup.rectTable[4].rectOutputCurrent =(uint32_t)(Uart_data_get.Rect5_I * 100);
                     Uart_array_data_get.Rect6 = g_UARTRxBuf485[j];
                     j++;
                     for(i=0;i<4;i++)
                     {
                             Uart_array_data_get.Rect6_V [i]=g_UARTRxBuf485[j];
                             inverse[3-i]=Uart_array_data_get.Rect6_V [i];
                             j++;
                     }
                     memcpy(&Uart_data_get.Rect6_V,&inverse,4);
                     privateMibBase.rectGroup.rectTable[5].rectOutputVoltage = (uint32_t) (Uart_data_get.Rect6_V * 100);
                     for(i=0;i<4;i++)
                     {
                             Uart_array_data_get.Rect6_I [i]=g_UARTRxBuf485[j];
                             inverse[3-i]=Uart_array_data_get.Rect6_I [i];
                             j++;
                     }
                     memcpy(&Uart_data_get.Rect6_I,&inverse,4);
                     privateMibBase.rectGroup.rectTable[5].rectOutputCurrent =(uint32_t) (Uart_data_get.Rect6_I * 100);
                     break;
             case 0xB0:
                     i=0;j=31;

                     for(i=0;i<4;i++)
                     {
                             Uart_array_data_get.AC_Volt_Over [i]=g_UARTRxBuf485[j];
                             inverse[3-i]=Uart_array_data_get.AC_Volt_Over [i];
                             j++;
                     }
                     memcpy(&sAcInfo.fAcHighThres,&inverse,4);

                     for(i=0;i<4;i++)
                     {
                             Uart_array_data_get.AC_Volt_Under [i]=g_UARTRxBuf485[j];
                             inverse[3-i]=Uart_array_data_get.AC_Volt_Under [i];
                             j++;
                     }
                     memcpy(&sAcInfo.fAcLowThres,&inverse,4);
                     privateMibBase.configGroup.cfgAcLowLevel = (uint32_t) sAcInfo.fAcLowThres;
                     for(i=0;i<4;i++)
                     {
                             Uart_array_data_get.DC_Volt_Over [i]=g_UARTRxBuf485[j];
                             inverse[3-i]=Uart_array_data_get.DC_Volt_Over [i];
                             j++;
                     }
                     memcpy(&Uart_data_get.DC_Volt_Over,&inverse,4);
                     for(i=0;i<4;i++)
                     {
                             Uart_array_data_get.DC_Volt_Under [i]=g_UARTRxBuf485[j];
                             inverse[3-i]=Uart_array_data_get.DC_Volt_Under [i];
                             j++;
                     }
                     memcpy(&sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg,&inverse,4);
                     privateMibBase.configGroup.cfgDCLowVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg*100);
                     break;
             case 0x9D:
                     i=0;j=10;

                      privateMibBase.cfgBTGroup.cfgAgissonBTRespond= g_UARTRxBuf485[7];

                     break;

             case 0x95:
                     i=0;j=16;
                     for(i=0;i<4;i++)
                     {
                             Uart_array_data_get.Batt_Temp_High_Alarm [i]=g_UARTRxBuf485[j];
                             inverse[3-i]=Uart_array_data_get.Batt_Temp_High_Alarm [i];
                             j++;
                     }
                     memcpy(&sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel,&inverse,4);
                     privateMibBase.configGroup.cfgHighMajorTempLevel = (int32_t) (sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel * 100);
                     for(i=0;i<4;i++)
                             {
                                     Uart_array_data_get.Batt_Temp_Low_Alarm [i]=g_UARTRxBuf485[j];
                                     inverse[3-i]=Uart_array_data_get.Batt_Temp_Low_Alarm [i];
                                     j++;
                             }
                     memcpy(&Uart_data_get.Batt_Temp_Low_Alarm,&inverse,4);
                     privateMibBase.configGroup.cfgLowTempLevel = (int32_t) (Uart_data_get.Batt_Temp_Low_Alarm*100);
                     for(i=0;i<4;i++)
                             {
                                     Uart_array_data_get.Batt_Temp_Meas_Upper_Limit [i]=g_UARTRxBuf485[j];
                                     inverse[3-i]=Uart_array_data_get.Batt_Temp_Meas_Upper_Limit [i];
                                     j++;
                             }
                     memcpy(&Uart_data_get.Batt_Temp_Meas_Upper_Limit,&inverse,4);
                     privateMibBase.configGroup.cfgOverMajorTempLevel = (uint32_t) (Uart_data_get.Batt_Temp_Meas_Upper_Limit * 100);
                     for(i=0;i<4;i++)
                             {
                                     Uart_array_data_get.Batt_Temp_Meas_Lower_Limit [i]=g_UARTRxBuf485[j];
                                     inverse[3-i]=Uart_array_data_get.Batt_Temp_Meas_Lower_Limit [i];
                                     j++;
                             }
                     memcpy(&Uart_data_get.Batt_Temp_Meas_Lower_Limit,&inverse,4);

                     j=j+8;
                     for(i=0;i<4;i++)
                             {
                                     Uart_array_data_get.Batt_Temp_Coff [i]=g_UARTRxBuf485[j];
                                     inverse[3-i]=Uart_array_data_get.Batt_Temp_Coff [i];
                                     j++;
                             }
                     memcpy(&sBattInfo.sRAWBattInfo.fTempCompVal,&inverse,4);
                     privateMibBase.configGroup.cfgTempCompValue = (uint32_t) (sBattInfo.sRAWBattInfo.fTempCompVal);
                     Uart_data.Foundation_of_Temp_Comp=g_UARTRxBuf485[j];
                     break;
             case 0x98:
                     i=0;j=10;



                     break;
             case 0x8E:
                     i=0;j=10;
//                      remainBat = g_UARTRxBuf485[10];
                      privateMibBase.batteryGroup.battCapLeft1 = (uint32_t)g_UARTRxBuf485[10] * 100;

                     break;
             case 0x93:
                     i =0; j=10;
                     Uart_array_data_get.LLVD_Allow = g_UARTRxBuf485[8];
                     Uart_array_data_get.BLVD_Allow = g_UARTRxBuf485[9];
                     //sBattInfo.sRAWBattInfo.u8LLVDEnable
                     if( Uart_array_data_get.LLVD_Allow==0xAA)      privateMibBase.configGroup.cfgLLVDEn=1;
                     else    privateMibBase.configGroup.cfgLLVDEn=0;
                     if( Uart_array_data_get.BLVD_Allow==0xAA)      privateMibBase.configGroup.cfgBLVDEn=1;
                     else    privateMibBase.configGroup.cfgBLVDEn=0;
//                     privateMibBase.configGroup.cfgLLVDEn =  g_UARTRxBuf485[8];
                    // sBattInfo.sRAWBattInfo.u8BLVDEnable   
//                     privateMibBase.configGroup.cfgBLVDEn = g_UARTRxBuf485[9];   
                     for(i=0;i<4;i++)
                             {
                                     Uart_array_data_get.LLVD_Volt [i]=g_UARTRxBuf485[j];
                                     inverse[3-i]=Uart_array_data_get.LLVD_Volt [i];
                                     j++;
                             }
                     memcpy(&sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg,&inverse,4);
                     privateMibBase.configGroup.cfgLLVDVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg * 100);
                     for(i=0;i<4;i++)
                             {
                                     Uart_array_data_get.BLVD_Volt [i]=g_UARTRxBuf485[j];
                                     inverse[3-i]=Uart_array_data_get.BLVD_Volt [i];
                                     j++;
                             }
                     memcpy(&sBattInfo.sRAWBattInfo.fLVDDV,&inverse,4);
                     privateMibBase.configGroup.cfgBLVDVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fLVDDV * 100);
                     break;
             case 0x92:
                     i =0; j=8;
                     for(i=0;i<4;i++)
                             {
                                     Uart_array_data_get.Batt_I_Charge_coff [i]=g_UARTRxBuf485[j];
                                     inverse[3-i]=Uart_array_data_get.Batt_I_Charge_coff [i];
                                     j++;
                             }
                     memcpy(&sBattInfo.sRAWBattInfo.fCCLVal,&inverse,4);
                     privateMibBase.configGroup.cfgCurrentLimit = (uint32_t) (sBattInfo.sRAWBattInfo.fCCLVal * 1000);
                     for(i=0;i<4;i++)
                             {
                                     Uart_array_data_get.Batt_1_Cap [i]=g_UARTRxBuf485[j];
                                     inverse[3-i]=Uart_array_data_get.Batt_1_Cap [i];
                                     j++;
                             }
                     memcpy(&sBattInfo.sRAWBattInfo.fCapTotal,&inverse,4);
                     privateMibBase.configGroup.cfgBattCapacityTotal = (uint32_t) (sBattInfo.sRAWBattInfo.fCapTotal);
                     for(i=0;i<4;i++)
                             {
                                     Uart_array_data_get.Batt_2_Cap [i]=g_UARTRxBuf485[j];
                                     inverse[3-i]=Uart_array_data_get.Batt_2_Cap [i];
                                     j++;
                             }
                     memcpy(&sBattInfo.sRAWBattInfo.fCapTotal2,&inverse,4);
                     privateMibBase.configGroup.cfgBattCapacityTotal2 = (uint32_t) (sBattInfo.sRAWBattInfo.fCapTotal2);
                     privateMibBase.batteryGroup.battBanksNumofBanks = g_UARTRxBuf485[20];
                     Uart_array_data_get.Batt_Cycled_BC = g_UARTRxBuf485[21];
                     break;
             case 0x91:
                     i =0; j=8;
                     for(i=0;i<4;i++)
                             {
                                     Uart_array_data_get.Batt_Boost_Volt [i]=g_UARTRxBuf485[j];
                                     inverse[3-i]=Uart_array_data_get.Batt_Boost_Volt [i];
                                     j++;
                             }
                     memcpy(&sBattInfo.sRAWBattInfo.fBotVoltCfg,&inverse,4);
                     privateMibBase.configGroup.cfgBoostVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fBotVoltCfg * 100);
                     for(i=0;i<4;i++)
                             {
                                     Uart_array_data_get.Batt_Float_Volt [i]=g_UARTRxBuf485[j];
                                     inverse[3-i]=Uart_array_data_get.Batt_Float_Volt [i];
                                     j++;
                             }
                     memcpy(&sBattInfo.sRAWBattInfo.fFltVoltCfg,&inverse,4);
                     privateMibBase.configGroup.cfgFloatVolt = (uint32_t) (sBattInfo.sRAWBattInfo.fFltVoltCfg * 100);
                     break;
             case 0x40:
                     sRectInfo.u8Rect_Num = g_UARTRxBuf485[15];
                     privateMibBase.rectGroup.rectInstalledRect = 6;
                     privateMibBase.rectGroup.rectActiveRect = sRectInfo.u8Rect_Num;
                     break;
             case 0x30:
                     Uart_array_data_get.AC_Status=g_UARTRxBuf485[8];
                     if(Uart_array_data_get.AC_Status==0xAA)privateMibBase.mainAlarmGroup.alarmACmains = 0x01; //sAlarmInfo.u32ACmains=0x01;
                     else
                       privateMibBase.mainAlarmGroup.alarmACmains = 0x00; //sAlarmInfo.u32ACmains=0x00;
                     Uart_array_data_get.AC_Low =g_UARTRxBuf485[9];
                     if(Uart_array_data_get.AC_Low==0x55)privateMibBase.mainAlarmGroup.alarmACLow = 0x01;  //sAlarmInfo.u32ACThres
                     else privateMibBase.mainAlarmGroup.alarmACLow = 0x00;  //sAlarmInfo.u32ACThres
                     Uart_array_data_get.DC_Status=g_UARTRxBuf485[10];
                     if(Uart_array_data_get.DC_Status==0x55)privateMibBase.mainAlarmGroup.alarmDCLow =0x01;    //sAlarmInfo.u32DCopen
                     else privateMibBase.mainAlarmGroup.alarmDCLow =0x00;        //sAlarmInfo.u32DCopen

                     Uart_array_data_get.LLVD_Status=g_UARTRxBuf485[11];
                     if(Uart_array_data_get.LLVD_Status == 0x01)privateMibBase.mainAlarmGroup.alarmLLVD  = 0x01;     //sAlarmInfo.u32LLVDopen
                     else privateMibBase.mainAlarmGroup.alarmLLVD = 0x00;   //sAlarmInfo.u32LLVDopen 
                     Uart_array_data_get.BLVD_Status=g_UARTRxBuf485[12];
                     if(Uart_array_data_get.BLVD_Status == 0x01)privateMibBase.mainAlarmGroup.alarmBLVD  = 0x01;       //sAlarmInfo.u32BLVDopen
                     else privateMibBase.mainAlarmGroup.alarmBLVD = 0x00;   //sAlarmInfo.u32BLVDopen 

                     Uart_array_data_get.Batt_Temp_Status=g_UARTRxBuf485[16];
//                     sAlarmInfo.u32MjBattHiTemp = 0;
                     privateMibBase.mainAlarmGroup.alarmBattHighTemp =0;
                     if(Uart_array_data_get.Batt_Temp_Status==0xAA)privateMibBase.mainAlarmGroup.alarmBattHighTemp = 0x02; //sAlarmInfo.u32MjBattHiTemp

//                     sAlarmInfo.u32RectFail = 0;
//                     sAlarmInfo.u32MnRect =0;
                     privateMibBase.mainAlarmGroup.alarmRectACFault=0;
                     privateMibBase.mainAlarmGroup.alarmRectNoResp=0;

                     Uart_array_data_get.Rect1_Status=g_UARTRxBuf485[20];
                     privateMibBase.rectGroup.rectTable[0].rectStatus = Uart_array_data_get.Rect1_Status;
                     Uart_array_data_get.Rect2_Status=g_UARTRxBuf485[21];
                     privateMibBase.rectGroup.rectTable[1].rectStatus = Uart_array_data_get.Rect2_Status;
                     Uart_array_data_get.Rect3_Status=g_UARTRxBuf485[22];
                     privateMibBase.rectGroup.rectTable[2].rectStatus = Uart_array_data_get.Rect3_Status;
                     Uart_array_data_get.Rect4_Status=g_UARTRxBuf485[23];
                     privateMibBase.rectGroup.rectTable[3].rectStatus = Uart_array_data_get.Rect4_Status;
                     Uart_array_data_get.Rect5_Status=g_UARTRxBuf485[24];
                     privateMibBase.rectGroup.rectTable[4].rectStatus = Uart_array_data_get.Rect5_Status;
                     Uart_array_data_get.Rect6_Status=g_UARTRxBuf485[25];
                     privateMibBase.rectGroup.rectTable[5].rectStatus = Uart_array_data_get.Rect6_Status;
                     for(i=0;i<6;i++)
                     {
                             switch(privateMibBase.rectGroup.rectTable[i].rectStatus)
                             {
                             case 0xA5:
                                     privateMibBase.mainAlarmGroup.alarmRectACFault |= 0x01;  //sAlarmInfo.u32RectFail
                                     break;
                             case 0xAA:
                                     privateMibBase.mainAlarmGroup.alarmRectNoResp  |= 0x02;   //sAlarmInfo.u32MnRect
                                     break;
                             case 0xFF:
                                     break;
                             case 0x00:
                                     break;
                             default:
                                     privateMibBase.rectGroup.rectTable[i].rectStatus = 0x22; // other failure;
                                     break;
                             }
                     }
                     if(privateMibBase.mainAlarmGroup.alarmRectACFault !=0)privateMibBase.mainAlarmGroup.alarmRectACFault  =1;   //sAlarmInfo.u32MjRect
                     else privateMibBase.mainAlarmGroup.alarmRectACFault  =0;       //sAlarmInfo.u32MjRect
                     if(privateMibBase.mainAlarmGroup.alarmRectNoResp !=0)privateMibBase.mainAlarmGroup.alarmRectNoResp =1;
                     Uart_array_data_get.Load_Status=g_UARTRxBuf485[35];
//                     if(Uart_array_data_get.Load_Status !=0)privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen =1;
//                     else privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen =0;
                     if(Uart_array_data_get.Load_Status & 0x0F)privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen =1;
                     else privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen =0;
                     if(Uart_array_data_get.Load_Status & 0x30)privateMibBase.mainAlarmGroup.alarmBattBreakerOpen =1;
                     else privateMibBase.mainAlarmGroup.alarmBattBreakerOpen =0;
                     Uart_array_data_get.DI_Alarm=g_UARTRxBuf485[34];
                     for(i=0;i<8;i++)
                     {
//                             sAlarmInfo.u32DIFuse[i] = (Uart_array_data_get.DI_Alarm >>i)&0x01;
                       privateMibBase.diAlarmGroup.alarmDigitalInput[i]= (Uart_array_data_get.DI_Alarm >>i)&0x01;
                     }
                       
                     
//                    HisCount_old[ih] = sHisFlashLog[ih].Count; 
                    if((privateMibBase.mainAlarmGroup.alarmACmains ==1)&&(privateMibBase.mainAlarmGroup.alarmACmains_old==0))
                     {
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =1;
                          NextHis();
     
                     }
                    if((privateMibBase.mainAlarmGroup.alarmACmains ==0)&&(privateMibBase.mainAlarmGroup.alarmACmains_old==1))
                    {     
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =2;
                          NextHis();
                    }
                     if((privateMibBase.mainAlarmGroup.alarmACLow ==1)&&(privateMibBase.mainAlarmGroup.alarmACLow_old==0))
                     {
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =3;
                          NextHis();

                     }
                     if((privateMibBase.mainAlarmGroup.alarmACLow ==0)&&(privateMibBase.mainAlarmGroup.alarmACLow_old==1))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =4;
                          NextHis();

                     }
                     
                     if((privateMibBase.diAlarmGroup.alarmDigitalInput[2] ==1)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[2]==0))
                     {    
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =5;
                          NextHis();

                     }  
                     if((privateMibBase.diAlarmGroup.alarmDigitalInput[2]==0)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[2]==1))
                     {
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =6;
                          NextHis();

                     }
                     if((privateMibBase.mainAlarmGroup.alarmDCLow ==1)&&(privateMibBase.mainAlarmGroup.alarmDCLow_old==0))
                     {    
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =7;
                          NextHis();

                     }  
                     if((privateMibBase.mainAlarmGroup.alarmDCLow==0)&&(privateMibBase.mainAlarmGroup.alarmDCLow_old==1))
                     {
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =8;
                          NextHis();

                     }
                     
                     if((privateMibBase.diAlarmGroup.alarmDigitalInput[3] ==1)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[3]==0))
                     {    
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =9;
                          NextHis();
 
                     } 
                     if((privateMibBase.diAlarmGroup.alarmDigitalInput[3]==0)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[3]==1))
                     {
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =10;
                          NextHis();

                     }
                     
                     if((privateMibBase.mainAlarmGroup.alarmLLVD ==1)&&(privateMibBase.mainAlarmGroup.alarmLLVD_old==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =11;
                          NextHis();

                     }
                    if((privateMibBase.mainAlarmGroup.alarmLLVD==0)&&(privateMibBase.mainAlarmGroup.alarmLLVD_old==1))
                    {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =12;
                          NextHis();

                    }
                    
                     if((privateMibBase.mainAlarmGroup.alarmBLVD ==1)&&(privateMibBase.mainAlarmGroup.alarmBLVD_old==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =13;
                          NextHis();

                     }  
                     if((privateMibBase.mainAlarmGroup.alarmBLVD==0)&&(privateMibBase.mainAlarmGroup.alarmBLVD_old==1))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =14;
                          NextHis();

                     }
                     if((privateMibBase.mainAlarmGroup.alarmBattBreakerOpen ==1)&&(privateMibBase.mainAlarmGroup.alarmBattBreakerOpen_old==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =15;
                          NextHis();

                     }  
                     if((privateMibBase.mainAlarmGroup.alarmBattBreakerOpen==0)&&(privateMibBase.mainAlarmGroup.alarmBattBreakerOpen_old==1))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =16;
                          NextHis();

                     }
                     if((privateMibBase.mainAlarmGroup.alarmBattHighTemp ==2)&&(privateMibBase.mainAlarmGroup.alarmBattHighTemp_old==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =17;
                          NextHis();

                     }  
                     if((privateMibBase.mainAlarmGroup.alarmBattHighTemp==0)&&(privateMibBase.mainAlarmGroup.alarmBattHighTemp_old==2))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =18;
                          NextHis();

                     }
                     
                     if((privateMibBase.mainAlarmGroup.alarmRectACFault ==1)&&(privateMibBase.mainAlarmGroup.alarmRectACFault_old==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =19;
                          NextHis();

                     }  
                     if((privateMibBase.mainAlarmGroup.alarmRectACFault==0)&&(privateMibBase.mainAlarmGroup.alarmRectACFault_old==1))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =20;
                          NextHis();

                     }
                     
                     if((privateMibBase.mainAlarmGroup.alarmRectNoResp ==1)&&(privateMibBase.mainAlarmGroup.alarmRectNoResp_old==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =21;
                          NextHis();

                     }  
                     if((privateMibBase.mainAlarmGroup.alarmRectNoResp==0)&&(privateMibBase.mainAlarmGroup.alarmRectNoResp_old==1))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =22;
                          NextHis();

                     }
                     
                     if((privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen ==1)&&(privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen_old==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =23;
                          NextHis();

                     }  
                     if((privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen==0)&&(privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen_old==1))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =24;
                          NextHis();

                     }
                     
                     if((privateMibBase.diAlarmGroup.alarmDigitalInput[0]==1)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[0]==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =25;
                          NextHis();

                     }  
                     if((privateMibBase.diAlarmGroup.alarmDigitalInput[0]==0)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[0]==1))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =26;
                          NextHis();

                     }
                     
                     if((privateMibBase.diAlarmGroup.alarmDigitalInput[1]==1)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[1]==0))
                     {   
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =27;
                          NextHis();

                     }  
                     if((privateMibBase.diAlarmGroup.alarmDigitalInput[1]==0)&&(privateMibBase.diAlarmGroup.alarmDigitalInput_old[1]==1))
                     { 
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =28;
                          NextHis();

                     }
                     if((Program_Status ==0)&& (Program_Status_old !=0))
                     {
                          sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =29;
                          NextHis();

                     }
                     if((Program_Status !=0)&& (Program_Status_old ==0))
                     {
                       sHisFlashLog[ih].HisFlashEvent[sHisFlashLog[ih].Count].Hiscode =30;
                       NextHis();
                     }
          
                     
                     privateMibBase.mainAlarmGroup.alarmACmains_old  =  privateMibBase.mainAlarmGroup.alarmACmains;
                     privateMibBase.mainAlarmGroup.alarmACLow_old  =  privateMibBase.mainAlarmGroup.alarmACLow;
                     privateMibBase.mainAlarmGroup.alarmDCLow_old  =  privateMibBase.mainAlarmGroup.alarmDCLow;
                     privateMibBase.mainAlarmGroup.alarmLLVD_old  =  privateMibBase.mainAlarmGroup.alarmLLVD;
                     privateMibBase.mainAlarmGroup.alarmBLVD_old  =  privateMibBase.mainAlarmGroup.alarmBLVD;
                     privateMibBase.mainAlarmGroup.alarmBattBreakerOpen_old =  privateMibBase.mainAlarmGroup.alarmBattBreakerOpen;
                     privateMibBase.mainAlarmGroup.alarmBattHighTemp_old  =  privateMibBase.mainAlarmGroup.alarmBattHighTemp;
                     privateMibBase.mainAlarmGroup.alarmRectACFault_old  =  privateMibBase.mainAlarmGroup.alarmRectACFault;
                     privateMibBase.mainAlarmGroup.alarmRectNoResp_old  =  privateMibBase.mainAlarmGroup.alarmRectNoResp;
                     privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen_old  =  privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen;
                     for(i=0;i<4;i++)
                     {
                       privateMibBase.diAlarmGroup.alarmDigitalInput_old[i]=  privateMibBase.diAlarmGroup.alarmDigitalInput[i];
                     }
                      
                     thao1++;
                     break;
               case 0x21:
                     i =0; j=8;
                     for(i=0;i<4;i++)
                             {
                                     Uart_array_data_get.Batt_Temp1 [i]=g_UARTRxBuf485[j];
                                     inverse[3-i]=Uart_array_data_get.Batt_Temp1 [i];
                                     j++;
                             }
                     memcpy(&Uart_data_get.Batt_Temp1,&inverse,4);
                     privateMibBase.batteryGroup.battTemp = (uint32_t)(Uart_data_get.Batt_Temp1*10);
                     j=j+8;
                     for(i=0;i<4;i++)
                             {
                                     Uart_array_data_get.Env_Temp2 [i]=g_UARTRxBuf485[j];
                                     inverse[3-i]=Uart_array_data_get.Env_Temp2 [i];
                                     j++;
                             }
                     memcpy(&Uart_data_get.Env_Temp2,&inverse,4);
                     privateMibBase.siteGroup.siteAmbientTemp = (uint32_t) (Uart_data_get.Env_Temp2*10);
                     for(i=0;i<4;i++)
                             {
                                     Uart_array_data_get.Batt_Temp2 [i]=g_UARTRxBuf485[j];
                                     inverse[3-i]=Uart_array_data_get.Batt_Temp2 [i];
                                     j++;
                             }
                     memcpy(&Uart_data_get.Batt_Temp2,&inverse,4);
                     break;                               
             case 0xA4:
//                thao ++;
                     Uart_data_get.Year = g_UARTRxBuf485[8];
                     Uart_data_get.Month = g_UARTRxBuf485[9];
                     Uart_data_get.Day = g_UARTRxBuf485[10];
                     Uart_data_get.Hour = g_UARTRxBuf485[11];
                     Uart_data_get.Minute = g_UARTRxBuf485[12];
                     for(i=0;i<(ih+1);i++)
                     {
                        if(HisCount_old[i] <sHisFlashLog[i].Count)
                        {
                          for(j=HisCount_old[i];j<(sHisFlashLog[i].Count);j++)
                            {
                             
                                sHisFlashLog[i].HisFlashEvent[j].Year  =   Uart_data_get.Year;
                                sHisFlashLog[i].HisFlashEvent[j].Month =   Uart_data_get.Month;
                                sHisFlashLog[i].HisFlashEvent[j].Day   =   Uart_data_get.Day;
                                sHisFlashLog[i].HisFlashEvent[j].Hour  =   Uart_data_get.Hour;
                                sHisFlashLog[i].HisFlashEvent[j].Minute=   Uart_data_get.Minute;
                            }
                          HisCount_old[i] = sHisFlashLog[i].Count;
                        }
                     }
                    
                     break;
             case 0x8D:
                     i =0; j=52;
                     for(i=0;i<4;i++)
                       {
                         Uart_array_data_get.StdBattTestEndVolt[i]=g_UARTRxBuf485[j];
                         inverse[3-i]=Uart_array_data_get.StdBattTestEndVolt [i];
                         j++;
                       }
                     memcpy(&sBattTestInfo.fBattTestVolt,&inverse,4);
                     privateMibBase.cfgBTGroup.cfgBTEndVolt = (uint32_t)(sBattTestInfo.fBattTestVolt*100);
                     sBattTestInfo.u8StdCellNo =g_UARTRxBuf485[j];
                     j= 57;
                      for(i=0;i<4;i++)
                       {
                         Uart_array_data_get.StdBattTestTime[i]=g_UARTRxBuf485[j];
                         inverse[3-i]=Uart_array_data_get.StdBattTestTime [i];
                         j++;
                       }
                     memcpy(&sBattTestInfo.fBattTestDur,&inverse,4);
                     privateMibBase.cfgBTGroup.cfgBTEndTime = (uint32_t)(sBattTestInfo.fBattTestDur);
                      j= 61;
                      for(i=0;i<4;i++)
                       {
                         Uart_array_data_get.ShortBattTestEndVolt[i]=g_UARTRxBuf485[j];
                         inverse[3-i]=Uart_array_data_get.ShortBattTestEndVolt [i];
                         j++;
                       }
                     memcpy(&sBattTestInfo.fShortTestEndVolt,&inverse,4);
                     sBattTestInfo.u8ShortTestTime =g_UARTRxBuf485[j];
                     break;
             }
     }

  }
  Program_Status_old = Program_Status;
  //===reset thong so khi mat ket noi
  if(privateMibBase.connAlarmGroup.alarmPMUConnect == 1){
    privateMibBase.rectGroup.rectActiveRect = 0;
    privateMibBase.rectGroup.rectTable[0].rectOutputCurrent = 0;
    privateMibBase.rectGroup.rectTable[1].rectOutputCurrent = 0;
    privateMibBase.rectGroup.rectTable[2].rectOutputCurrent = 0;
    privateMibBase.rectGroup.rectTable[3].rectOutputCurrent = 0;
    privateMibBase.rectGroup.rectTable[4].rectOutputCurrent = 0;
    privateMibBase.rectGroup.rectTable[5].rectOutputCurrent = 0;
    privateMibBase.rectGroup.rectTable[0].rectOutputVoltage = 0;
    privateMibBase.rectGroup.rectTable[1].rectOutputVoltage = 0;
    privateMibBase.rectGroup.rectTable[2].rectOutputVoltage = 0;
    privateMibBase.rectGroup.rectTable[3].rectOutputVoltage = 0;
    privateMibBase.rectGroup.rectTable[4].rectOutputVoltage = 0;
    privateMibBase.rectGroup.rectTable[5].rectOutputVoltage = 0;
    privateMibBase.acPhaseGroup.acPhaseTable[0].acPhaseVolt = 0;
    privateMibBase.batteryGroup.battVolt = 0;
    privateMibBase.batteryGroup.battCurr = 0;
    privateMibBase.batteryGroup.battTemp = 0;
    privateMibBase.batteryGroup.battBanksNumofBanks = 0;
    privateMibBase.batteryGroup.battCapLeft1 = 0;
    privateMibBase.loadGroup.loadCurrent = 0;
    privateMibBase.cfgBTGroup.cfgBTEndTime = 0;
    privateMibBase.configGroup.cfgCurrentLimit = 0;
    privateMibBase.configGroup.cfgFloatVolt = 0;
    privateMibBase.configGroup.cfgBoostVolt = 0;
    privateMibBase.configGroup.cfgLLVDVolt = 0;
    privateMibBase.configGroup.cfgDCLowVolt = 0;
    privateMibBase.configGroup.cfgTempCompValue = 0;
    privateMibBase.configGroup.cfgBLVDVolt = 0;
    privateMibBase.configGroup.cfgBattCapacityTotal = 0;
    privateMibBase.configGroup.cfgHighMajorTempLevel = 0;
    privateMibBase.configGroup.cfgAcLowLevel = 0;
    privateMibBase.configGroup.cfgBattCapacityTotal2 = 0;
    privateMibBase.configGroup.cfgLLVDEn = 0;
    privateMibBase.configGroup.cfgBLVDEn = 0;
  }
  //===reset thong so khi mat ket noi
}
void DataSetup(uint8_t Setupcode)
{
	uint8_t i;
	uint32_t tempdata;
	uint8_t hex[4];
        float testtemp;
        float testendtime;
	switch(Setupcode)
	{
	case 1:
		sBattInfo.sRAWBattInfo.fBotVoltCfg = (float)privateMibBase.configGroup.cfgBoostVolt*0.01;
//		sBattInfo.sRAWBattInfo.fBotVoltCfg  = 54;
                memcpy(&tempdata,&sBattInfo.sRAWBattInfo.fBotVoltCfg,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Batt_Charge_Volt[(i+6)]=hex[i];
		}
		sBattInfo.sRAWBattInfo.fFltVoltCfg = (float)privateMibBase.configGroup.cfgFloatVolt*0.01;
		memcpy(&tempdata,&sBattInfo.sRAWBattInfo.fFltVoltCfg,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Batt_Charge_Volt[(i+10)]=hex[i];
		}

		Batt_Charge_Volt[14]=ChecksumH(&Batt_Charge_Volt[0],14);
		break;
	case 2:
		sBattInfo.sRAWBattInfo.fCCLVal = (float)privateMibBase.configGroup.cfgCurrentLimit*0.001;
//		sBattInfo.sRAWBattInfo.fCCLVal =0.12;
                memcpy(&tempdata,&sBattInfo.sRAWBattInfo.fCCLVal,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Batt_Info[(i+6)]=hex[i];
		}
		sBattInfo.sRAWBattInfo.fCapTotal = (float)privateMibBase.configGroup.cfgBattCapacityTotal;
		memcpy(&tempdata,&sBattInfo.sRAWBattInfo.fCapTotal,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Batt_Info[(i+10)]=hex[i];
		}
		sBattInfo.sRAWBattInfo.fCapTotal2 = (float)privateMibBase.configGroup.cfgBattCapacityTotal2;
		memcpy(&tempdata,&sBattInfo.sRAWBattInfo.fCapTotal2,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Batt_Info[(i+14)]=hex[i];
		}
		Batt_Info[18] = (uint8_t)(privateMibBase.batteryGroup.battBanksNumofBanks & 0xFF);
		Batt_Info[19]=Uart_array_data_get.Batt_Cycled_BC;
		Batt_Info[20]=ChecksumH(&Batt_Info[0],20);
		break;
	case 3:
          if( privateMibBase.configGroup.cfgLLVDEn!=0){ sBattInfo.sRAWBattInfo.u8LLVDE =0xAA;}
          else {sBattInfo.sRAWBattInfo.u8LLVDE =0x55;}
          if( privateMibBase.configGroup.cfgBLVDEn!=0){  sBattInfo.sRAWBattInfo.u8BLVDE=0xAA;}
          else  {  sBattInfo.sRAWBattInfo.u8BLVDE=0x55;}
          
//                sBattInfo.sRAWBattInfo.u8LLVDE = privateMibBase.configGroup.cfgLLVDEn;
//                sBattInfo.sRAWBattInfo.u8BLVDE = privateMibBase.configGroup.cfgBLVDEn;   
		LVD[6]= sBattInfo.sRAWBattInfo.u8LLVDE; //Uart_data.LLVD_Enable;
		LVD[7]= sBattInfo.sRAWBattInfo.u8BLVDE; //Uart_data.BLVD_Enable;
		sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg = (float)privateMibBase.configGroup.cfgLLVDVolt*0.01;
//		sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg = 50;
                memcpy(&tempdata,&sBattInfo.sRAWBattInfo.fLoMjAlrmVoltCfg ,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			LVD[(i+8)]=hex[i];
		}
		sBattInfo.sRAWBattInfo.fLVDDV = (float)privateMibBase.configGroup.cfgBLVDVolt*0.01;
//		 sBattInfo.sRAWBattInfo.fLVDDV =48;
                memcpy(&tempdata,&sBattInfo.sRAWBattInfo.fLVDDV,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			LVD[(i+12)]=hex[i];
		}
		LVD[25]=ChecksumH(&LVD[0],25);
		break;
	case 4:
		memcpy(&tempdata,&Uart_data_get.Batt_Temp_Meas_Upper_Limit,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Batt_Temp[(i+6)]=hex[i];
		}

		memcpy(&tempdata,&Uart_data_get.Batt_Temp_Meas_Lower_Limit,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Batt_Temp[(i+10)]=hex[i];
		}
		sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel = (float)privateMibBase.configGroup.cfgHighMajorTempLevel*0.01;
		memcpy(&tempdata,&sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Batt_Temp[(i+14)]=hex[i];
		}

		memcpy(&tempdata,&Uart_data_get.Batt_Temp_Low_Alarm,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Batt_Temp[(i+18)]=hex[i];
		}

		memcpy(&tempdata,&Uart_data_get.Batt_Temp_Meas_Upper_Limit,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Batt_Temp[(i+22)]=hex[i];
		}
		memcpy(&tempdata,&Uart_data_get.Batt_Temp_Meas_Lower_Limit,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Batt_Temp[(i+26)]=hex[i];
		}
		memcpy(&tempdata,&sBattInfo.sRAWBattInfo.fHiMjTempAlrmLevel,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Batt_Temp[(i+30)]=hex[i];
		}

		memcpy(&tempdata,&Uart_data_get.Batt_Temp_Low_Alarm,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Batt_Temp[(i+34)]=hex[i];
		}
		sBattInfo.sRAWBattInfo.fTempCompVal = (float)privateMibBase.configGroup.cfgTempCompValue;
//		sBattInfo.sRAWBattInfo.fTempCompVal =72;
                memcpy(&tempdata,&sBattInfo.sRAWBattInfo.fTempCompVal,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Batt_Temp[(i+38)]=hex[i];
		}
		Batt_Temp[42]=Uart_data.Foundation_of_Temp_Comp;

		Batt_Temp[43]=ChecksumH(&Batt_Temp[0],43);
		break;
	case 5:
		sAcInfo.fAcHighThres = (float)privateMibBase.configGroup.cfgAcHighLevel;
		sAcInfo.fAcHighThres = 300;
		memcpy(&tempdata,&sAcInfo.fAcHighThres,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Volt_Limit[(i+6)]=hex[i];
		}
		sAcInfo.fAcLowThres = (float)privateMibBase.configGroup.cfgAcLowLevel;
//		sAcInfo.fAcLowThres =180;
                memcpy(&tempdata,&sAcInfo.fAcLowThres,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Volt_Limit[(i+10)]=hex[i];
		}
		memcpy(&tempdata,&Uart_data_get.DC_Volt_Over,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Volt_Limit[(i+14)]=hex[i];
		}
		sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg = (float)privateMibBase.configGroup.cfgDCLowVolt*0.01;
		memcpy(&tempdata,&sBattInfo.sRAWBattInfo.fLoMnAlrmVoltCfg,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Volt_Limit[(i+18)]=hex[i];
		}
		Volt_Limit[22]=ChecksumH(&Volt_Limit[0],22);
		break;
	case 6:
		memcpy(&tempdata,&Uart_data.Env_Temp_Meas_Upper_Limit,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Env_Info[(i+6)]=hex[i];
		}

		memcpy(&tempdata,&Uart_data.Env_Temp_Meas_Lower_Limit,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Env_Info[(i+10)]=hex[i];
		}

		memcpy(&tempdata,&Uart_data.Env_Temp_Upper_Limit,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Env_Info[(i+14)]=hex[i];
		}

		memcpy(&tempdata,&Uart_data.Env_Temp_Lower_Limit,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Env_Info[(i+18)]=hex[i];
		}

		memcpy(&tempdata,&Uart_data.Env_Hum_Meas_Upper_Limit,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Env_Info[(i+22)]=hex[i];
		}

		memcpy(&tempdata,&Uart_data.Env_Hum_Meas_Lower_Limit,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Env_Info[(i+26)]=hex[i];
		}

		memcpy(&tempdata,&Uart_data.Env_Hum_Upper_Limit,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Env_Info[(i+30)]=hex[i];
		}

		memcpy(&tempdata,&Uart_data.Env_Hum_Lower_Limit,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Env_Info[(i+34)]=hex[i];
		}
		Env_Info[38]=ChecksumH(&Env_Info[0],38);

		break;
	case 7:
		Batt_HTD[8]=Uart_data.HTD_Enable;
		Batt_HTD[9]=Uart_data.Batt_HTD_Temp;
		Batt_HTD[10] = ChecksumH(&Batt_HTD[0],10);
		break;
        case 8:   // batt test end volt
                testtemp =  (((float)(privateMibBase.cfgBTGroup.cfgBTEndVolt))/100);      //(float) 
                sBattTestInfo.fBattTestVolt = (((float)(privateMibBase.cfgBTGroup.cfgBTEndVolt))/100);      //(float) 
                memcpy(&tempdata,&testtemp,4);
//                memcpy(&tempdata,&sSNMPConfigInfo.battTestEndVolt,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Batt_Test[(i+50)]=hex[i];
		}
                Batt_Test[64]=ChecksumH(&Batt_Test[0],64);
          
          break;
        case 9:   // batt test std cell number

                Batt_Test[54]= privateMibBase.cfgBTGroup.cfgBTCellNo;
                Batt_Test[64]=ChecksumH(&Batt_Test[0],64);
          
          break;
        case 10:   // batt test set time test
                testendtime =   (float) (privateMibBase.cfgBTGroup.cfgBTEndTime);
                sBattTestInfo.fBattTestDur =  (float) (privateMibBase.cfgBTGroup.cfgBTEndTime);
                memcpy(&tempdata,&testendtime,4);
		for(i=0; i<4; i++)
		{
			hex[i]=(tempdata>>((3-i)*8))&0xFF;
		}
		for(i=0; i<4; i++)
		{
			Batt_Test[(i+55)]=hex[i];
		}
                Batt_Test[64]=ChecksumH(&Batt_Test[0],64);
          break;  
//        case 11:   // batt test short test end volt
//                testtemp = sSNMPConfigInfo.battShortTestEndVolt/10; 
////                memcpy(&tempdata,&sSNMPConfigInfo.battShortTestEndVolt,4);
//                memcpy(&tempdata,&testtemp,4);
//		for(i=0; i<4; i++)
//		{
//			hex[i]=(tempdata>>((3-i)*8))&0xFF;
//		}
//		for(i=0; i<4; i++)
//		{
//			Batt_Test[(i+59)]=hex[i];
//		}
//                Batt_Test[64]=ChecksumH(&Batt_Test[0],64);
//          break;  
//        case 12:   // batt test short test end volt
//                Batt_Test[63]=(uint8_t)sSNMPConfigInfo.battShortTestTime;
//                Batt_Test[64]=ChecksumH(&Batt_Test[0],64);          
//          break;   
//          
	default:
		break;
	}
}
void SetFloatVolt(void)
{
   if(iSet ==0)
  {
    DataSetup(1);
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x7F,0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);  
    
  }
  if(iSet == 9)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0xC3,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  
  if((iSet<30)&&(iSet>14))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,Batt_Charge_Volt[iSet-15],0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  iSet++;
  if((iSet == 100)||(iSet == 40))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x23,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if(iSet>200)
  {
    iSet =0;
 
    settingCommandmask =settingCommandmask & (~SET_FLTVOL);
    if(settingCommandmask ==0)    setCmd_flag =0;  
    settingCommand =0;
//                setCmd_mask = SET_CCL;
  }
              
}

void SetBoostVolt(void)
{
   if(iSet ==0)
  {
    DataSetup(1);
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x7F,0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if(iSet == 9)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0xC3,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if((iSet<30)&&(iSet>14))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,Batt_Charge_Volt[iSet-15],0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if((iSet == 100)||(iSet == 40))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x23,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  iSet++; 
  if(iSet>200)
  {
    
    iSet =0;
    if(setuprepeat==0)
      {
        setuprepeat =1;
      }
    else 
      {
        settingCommandmask =settingCommandmask & (~SET_BSTVOL);
        if(settingCommandmask ==0)    setCmd_flag =0;  
        settingCommand =0;
        setuprepeat =0;
      }
  }
}

void SetCCL(void)
{
     if(iSet ==0)
    {
      DataSetup(2);
      GPIO_SetPinsOutput(GPIOD,1u << 1u);
      UART_9Bit_Send(RS485T_UART,0x7F,0);
      delay();
      GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    }
    if(iSet == 9)
    {
      GPIO_SetPinsOutput(GPIOD,1u << 1u);

      UART_9Bit_Send(RS485T_UART,0xC3,1);
      delay();
      GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    }
    if((iSet<36)&&(iSet>14))
    {
      GPIO_SetPinsOutput(GPIOD,1u << 1u);

      UART_9Bit_Send(RS485T_UART,Batt_Info[iSet-15],0);
      delay();
      GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    }
    if((iSet == 100)||(iSet == 45))
    {
      GPIO_SetPinsOutput(GPIOD,1u << 1u);

      UART_9Bit_Send(RS485T_UART,0x23,1);
      delay();
      GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    }
    iSet++;
    if(iSet>200)
    {
      iSet =0;
       if(setuprepeat==0)
          {
            setuprepeat =1;
          }
        else 
          {
            settingCommandmask =settingCommandmask & (~SET_CCL);
            if(settingCommandmask ==0)    setCmd_flag =0;  
            settingCommand =0;
            setuprepeat =0;
          } 
//    settingCommandmask =settingCommandmask & (~SET_CCL);
//    if(settingCommandmask ==0)    setCmd_flag =0; 
//      settingCommand =0;
//                  setCmd_mask = SET_LLVD;   
    }
}
              
void SetBattCap1(void)
{
    if(iSet ==0)
  {
    DataSetup(2);
    GPIO_SetPinsOutput(GPIOD,1u << 1u);
    UART_9Bit_Send(RS485T_UART,0x7F,0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if(iSet == 9)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0xC3,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if((iSet<36)&&(iSet>14))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,Batt_Info[iSet-15],0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if((iSet == 100)||(iSet == 45))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x23,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  iSet++;
  if(iSet>200)
  {
    iSet =0;
     if(setuprepeat==0)
        {
          setuprepeat =1;
        }
      else 
        {
          settingCommandmask =settingCommandmask & (~SET_BATTSTDCAP);
          if(settingCommandmask ==0)    setCmd_flag =0;  
          settingCommand =0;
          setuprepeat =0;
        } 
//    settingCommandmask =settingCommandmask & (~SET_BATTSTDCAP);
//    if(settingCommandmask ==0)    setCmd_flag =0; 
//    settingCommand =0;
//                  setCmd_mask = SET_LLVD;
  }
  
}

void SetBattCap2(void)
{
  if(iSet ==0)
  {
    DataSetup(2);
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x7F,0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if(iSet == 9)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0xC3,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if((iSet<36)&&(iSet>14))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,Batt_Info[iSet-15],0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if((iSet == 100)||(iSet == 45))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x23,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  iSet++;
  if(iSet>200)
  {
    iSet =0;
    if(setuprepeat==0)
      {
        setuprepeat =1;
      }
    else 
      {
        settingCommandmask =settingCommandmask & (~SET_BATTSTDCAP2);
        if(settingCommandmask ==0)    setCmd_flag =0;  
        settingCommand =0;
        setuprepeat =0;
      } 
//    settingCommandmask =settingCommandmask & (~SET_BATTSTDCAP2);
//    if(settingCommandmask ==0)    setCmd_flag =0; 
//    settingCommand =0;
//                  setCmd_mask = SET_LLVD;
  }
}

void SetLLVD_EN(void)
{
  if(iSet ==0)
  {
    DataSetup(3);
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x7F,0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if(iSet == 9)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0xC3,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    
  }
  if((iSet<41)&&(iSet>14))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,LVD[iSet-15],0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if((iSet == 100)||(iSet == 51))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x23,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  iSet++;
  if(iSet>200)
  {
    iSet =0;
     if(setuprepeat==0)
      {
        setuprepeat =1;
      }
    else 
      {
        settingCommandmask =settingCommandmask & (~SET_LLVD_EN);
        if(settingCommandmask ==0)    setCmd_flag =0;  
        settingCommand =0;
        setuprepeat =0;
      } 
//    settingCommandmask =settingCommandmask & (~0x00000001);
//    if(settingCommandmask ==0)    setCmd_flag =0; 
//    settingCommand =0;
//                 setCmd_mask =  SET_TEMPCOMP_VAL ;
  }
}
void SetBLVD_EN(void)
{
  if(iSet ==0)
  {
    DataSetup(3);
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x7F,0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if(iSet == 9)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0xC3,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    
  }
  if((iSet<41)&&(iSet>14))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,LVD[iSet-15],0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if((iSet == 100)||(iSet == 51))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x23,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  iSet++;
  if(iSet>200)
  {
    iSet =0;
     if(setuprepeat==0)
      {
        setuprepeat =1;
      }
    else 
      {
        settingCommandmask =settingCommandmask & (~SET_BLVD_EN);
        if(settingCommandmask ==0)    setCmd_flag =0;  
        settingCommand =0;
        setuprepeat =0;
      } 
//    settingCommandmask =settingCommandmask & (~0x00000001);
//    if(settingCommandmask ==0)    setCmd_flag =0; 
//    settingCommand =0;
//                 setCmd_mask =  SET_TEMPCOMP_VAL ;
  }
}



void SetLLVD(void)
{
  if(iSet ==0)
  {
    DataSetup(3);
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x7F,0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if(iSet == 9)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0xC3,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    
  }
  if((iSet<41)&&(iSet>14))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,LVD[iSet-15],0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if((iSet == 100)||(iSet == 51))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x23,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  iSet++;
  if(iSet>200)
  {
    iSet =0;
     if(setuprepeat==0)
      {
        setuprepeat =1;
      }
    else 
      {
        settingCommandmask =settingCommandmask & (~0x00000001);
        if(settingCommandmask ==0)    setCmd_flag =0;  
        settingCommand =0;
        setuprepeat =0;
      } 
//    settingCommandmask =settingCommandmask & (~0x00000001);
//    if(settingCommandmask ==0)    setCmd_flag =0; 
//    settingCommand =0;
//                 setCmd_mask =  SET_TEMPCOMP_VAL ;
  }
}

void SetBLVD(void)
{
    if(iSet ==0)
    {
      DataSetup(3);
      GPIO_SetPinsOutput(GPIOD,1u << 1u);

      UART_9Bit_Send(RS485T_UART,0x7F,0);
      delay();
      GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    }
    if(iSet == 9)
    {
      GPIO_SetPinsOutput(GPIOD,1u << 1u);

      UART_9Bit_Send(RS485T_UART,0xC3,1);
      delay();
      GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    }
    if((iSet<41)&&(iSet>14))
    {
      GPIO_SetPinsOutput(GPIOD,1u << 1u);

      UART_9Bit_Send(RS485T_UART,LVD[iSet-15],0);
      delay();
      GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    }
    if((iSet == 100)||(iSet == 51))
    {
      GPIO_SetPinsOutput(GPIOD,1u << 1u);

      UART_9Bit_Send(RS485T_UART,0x23,1);
      delay();
      GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    }
    iSet++;
    if(iSet>200)
    {
      iSet =0;
      if(setuprepeat==0)
        {
          setuprepeat =1;
        }
      else 
        {
          settingCommandmask =settingCommandmask & (~SET_BLVD);
          if(settingCommandmask ==0)    setCmd_flag =0;  
          settingCommand =0;
          setuprepeat =0;
        } 
//      settingCommandmask =settingCommandmask & (~SET_BLVD);
//      if(settingCommandmask ==0)    setCmd_flag =0; 
//      settingCommand =0;
//                 setCmd_mask =  SET_TEMPCOMP_VAL ;
    }
              
}

void SetTempComVal(void)
{
  if(iSet ==0)
  {
    DataSetup(4);
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x7F,0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    
  }
  if(iSet == 9)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0xC3,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if((iSet<59)&&(iSet>14))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,Batt_Temp[iSet-15],0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if((iSet == 120)||(iSet == 69))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x23,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  iSet++;
  if(iSet>220)
  {
    iSet =0;
      if(setuprepeat==0)
        {
          setuprepeat =1;
        }
      else 
        {
          settingCommandmask =settingCommandmask & (~SET_TEMPCOMP_VAL);
          if(settingCommandmask ==0)    setCmd_flag =0;  
          settingCommand =0;
          setuprepeat =0;
        } 
//    settingCommandmask =settingCommandmask & (~SET_TEMPCOMP_VAL);
//    if(settingCommandmask ==0)    setCmd_flag =0; 
//    settingCommand =0;
//                 setCmd_mask =   SET_DCLOW;
  }
               
}

void SetOverTemp(void)
{
    if(iSet ==0)
    {
      DataSetup(4);
      GPIO_SetPinsOutput(GPIOD,1u << 1u);

      UART_9Bit_Send(RS485T_UART,0x7F,0);
      delay();
      GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    }
    if(iSet == 9)
    {
      GPIO_SetPinsOutput(GPIOD,1u << 1u);

      UART_9Bit_Send(RS485T_UART,0xC3,1);
      delay();
      GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    }
    if((iSet<59)&&(iSet>14))
    {
      GPIO_SetPinsOutput(GPIOD,1u << 1u);

      UART_9Bit_Send(RS485T_UART,Batt_Temp[iSet-15],0);
      delay();
      GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    }
    if((iSet == 120)||(iSet == 69))
    {
      GPIO_SetPinsOutput(GPIOD,1u << 1u);

      UART_9Bit_Send(RS485T_UART,0x23,1);
      delay();
      GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    }
    iSet++;
    if(iSet>220)
    {
      iSet =0;
      if(setuprepeat==0)
        {
          setuprepeat =1;
        }
      else 
        {
          settingCommandmask =settingCommandmask & (~SET_OVERTEMP);
          if(settingCommandmask ==0)    setCmd_flag =0;  
          settingCommand =0;
          setuprepeat =0;
        } 
//      settingCommandmask =settingCommandmask & (~SET_OVERTEMP);
//      if(settingCommandmask ==0)    setCmd_flag =0; 
//      settingCommand =0;
//                 setCmd_mask =   SET_DCLOW;
   } 
}

void SetDClow(void)
{
  if(iSet ==0)
  {
    DataSetup(5);
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x7F,0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if(iSet == 9)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0xC3,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
    if((iSet<38)&&(iSet>14))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,Volt_Limit[iSet-15],0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    
  }
  if((iSet == 100)||(iSet == 48))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x23,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  iSet++;
  if(iSet>200)
  {
    iSet =0;
      if(setuprepeat==0)
        {
          setuprepeat =1;
        }
      else 
        {
          settingCommandmask =settingCommandmask & (~SET_DCLOW);
          if(settingCommandmask ==0)    setCmd_flag =0;  
          settingCommand =0;
          setuprepeat =0;
        } 
//    settingCommandmask =settingCommandmask & (~SET_DCLOW);
//    if(settingCommandmask ==0)    setCmd_flag =0; 
////    setCmd_mask =0;
//    settingCommand =0;
//                setCmd_mask = SET_FLTVOL;
  }
  
}

void SetACLow(void)
{
  if(iSet ==0)
  {
    DataSetup(5);
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x7F,0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if(iSet == 9)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0xC3,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
 
  }
    if((iSet<38)&&(iSet>14))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,Volt_Limit[iSet-15],0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    
  }
  if((iSet == 100)||(iSet == 48))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x23,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  iSet++;
  if(iSet>200)
  {
    iSet =0;
    if(setuprepeat==0)
      {
        setuprepeat =1;
      }
    else 
      {
        settingCommandmask =settingCommandmask & (~SET_AC_LOW_THRES);
        if(settingCommandmask ==0)    setCmd_flag =0;  
        settingCommand =0;
        setuprepeat =0;
      } 
//    settingCommandmask =settingCommandmask & (~SET_AC_LOW_THRES);
//    if(settingCommandmask ==0)    setCmd_flag =0; 
//    settingCommand =0;
//                setCmd_mask = SET_FLTVOL;
  }
  
}
   uint8_t athao[100];
void SetBattTestEndVolt(void)
{
  if(iSet ==0)
  {
    DataSetup(8);
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x7F,0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if(iSet == 9)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0xC3,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
    if((iSet<80)&&(iSet>14))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,Batt_Test[iSet-15],0);
    athao[iSet-15] =  Batt_Test[iSet-15];
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  if((iSet == 150)||(iSet == 90))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x23,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  iSet++;
  if(iSet>250)
  {
    iSet =0;
    if(setuprepeat==0)
      {
        setuprepeat =1;
      }
    else 
      {
        settingCommandmask =settingCommandmask & (~SET_BT_ENDVOLT);
        if(settingCommandmask ==0)    setCmd_flag =0;  
        settingCommand =0;
        Batt_Test[50] =0xFF;  Batt_Test[51] =0xFF; Batt_Test[52] =0xFF; Batt_Test[53] =0xFF;
        setuprepeat =0;
      } 
//    settingCommandmask =settingCommandmask & (~SET_BT_ENDVOLT);
//    if(settingCommandmask ==0)    setCmd_flag =0; 
//    settingCommand =0;
//    Batt_Test[50] =0xFF;  Batt_Test[51] =0xFF; Batt_Test[52] =0xFF; Batt_Test[53] =0xFF;
  }
  
}

void SetBattStdCellNo(void)
{
  if(iSet ==0)
  {
    DataSetup(9);
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x7F,0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  
  }
  if(iSet == 9)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0xC3,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    
  
  }
    if((iSet<80)&&(iSet>14))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,Batt_Test[iSet-15],0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);

  }
  if((iSet == 150)||(iSet == 90))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x23,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);

  }
  iSet++;
  if(iSet>250)
  {
    iSet =0;
   if(setuprepeat==0)
      {
        setuprepeat =1;
      }
    else 
      {
        settingCommandmask =settingCommandmask & (~SET_BT_CELLNO);
        if(settingCommandmask ==0)    setCmd_flag =0;  
        settingCommand =0;
         Batt_Test[54] =0xFF;  
        setuprepeat =0;
      } 
//    settingCommandmask =settingCommandmask & (~SET_BT_CELLNO);
//    if(settingCommandmask ==0)    setCmd_flag =0; 
//    settingCommand =0;
//    Batt_Test[54] =0xFF;  
  }
  
}
void SetBattStdTestTime(void)
{
  if(iSet ==0)
  {
    DataSetup(10);
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x7F,0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    
  }
  if(iSet == 9)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0xC3,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
   
  }
    if((iSet<80)&&(iSet>14))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,Batt_Test[iSet-15],0);
     athao[iSet-15] =  Batt_Test[iSet-15];
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);

  }
  if((iSet == 150)||(iSet == 90))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x23,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);

  }
  iSet++;
  if(iSet>250)
  {
    iSet =0;
    if(setuprepeat==0)
      {
        setuprepeat =1;
      }
    else 
      {
        settingCommandmask =settingCommandmask & (~SET_BT_ENDTIME);
        if(settingCommandmask ==0)    setCmd_flag =0;  
        settingCommand =0;
        Batt_Test[55] =0xFF;  Batt_Test[56] =0xFF; Batt_Test[57] =0xFF; Batt_Test[58] =0xFF; 
        setuprepeat =0;
      } 
//    settingCommandmask =settingCommandmask & (~SET_BT_ENDTIME);
//    if(settingCommandmask ==0)    setCmd_flag =0; 
//    settingCommand =0;
//    Batt_Test[55] =0xFF;  Batt_Test[56] =0xFF; Batt_Test[57] =0xFF; Batt_Test[58] =0xFF;
  }
  
}
void SetBattShortTestEndVolt(void)
{
  if(iSet ==0)
  {
    DataSetup(11);
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x7F,0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  
  }
  if(iSet == 9)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0xC3,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    
  }
  if((iSet<80)&&(iSet>14))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,Batt_Test[iSet-15],0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);

  }
  if((iSet == 150)||(iSet == 90))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x23,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);

  }
  iSet++;
  if(iSet>250)
  {
    iSet =0;
//     setCmd_mask =setCmd_mask & (~SET_BATT_SHORT_TEST_END_VOLT);
//    if(setCmd_mask ==0)    setCmd_flag =0; 
    settingCommand =0;
    Batt_Test[59] =0xFF;  Batt_Test[60] =0xFF; Batt_Test[61] =0xFF; Batt_Test[62] =0xFF;
  }
  
}
void SetBattShortTestTime(void)
{
  if(iSet ==0)
  {
    DataSetup(12);
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x7F,0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
   
    
  }
  if(iSet == 9)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0xC3,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    
  }
    if((iSet<80)&&(iSet>14))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,Batt_Test[iSet-15],0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);

  }
  if((iSet == 150)||(iSet == 90))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x23,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);

  }
  iSet++;
  if(iSet>250)
  {
    iSet =0;
//    settingCommandmask =settingCommandmask & (~SET_BATT_SHORT_TEST_TIME);
    if(settingCommandmask ==0)    setCmd_flag =0; 
    settingCommand =0;
    Batt_Test[63] =0xFF; 
  }
  
}
uint8_t iBT=0;
void Start_Batt_Manual_Test(void)
{
  if(iSet ==0)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x7F,0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
 
    
  }
  if(iSet == 10)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0xC3,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    
  }
  if((iSet>19)&&(iSet<60)&&((iSet%5)==0))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,Batt_Manual_Test_En[iBT],0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    iBT++;
  
   
  }
  if(iSet == 70)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x23,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);

  }
  iSet++;
  if(iSet>200)
  {
    iSet =0;
    iBT =0;
     settingCommandmask =settingCommandmask & (~START_BT_TEST);
    if(settingCommandmask ==0)    setCmd_flag =0; 
//    setCmd_mask = START_BATT_SHORT_TEST;
    settingCommand =0;
  }
  
}

void Start_Batt_Short_Test(void)
{
  if(iSet ==0)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x7F,0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  
  }
  if(iSet == 10)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0xC3,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
    
  }
  if((iSet>19)&&(iSet<60)&&((iSet%5)==0))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,Batt_Short_Test_En[iBT],0);
    iBT++;
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
   
   
  }
  if(iSet == 70)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x23,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);

  }
  iSet++;
  if(iSet>200)
  {
    iSet =0;
    iBT =0;
    setCmd_flag =0;
    setCmd_mask =0;
//    setCmd_mask = BATT_TEST_STOP;
    settingCommand =0;
  }
  
}
void Set_Batt_Test_Stop(void)
{
  if(iSet ==0)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x7F,0);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  
    
  }
  if(iSet == 10)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0xC3,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
   
  }
  if((iSet>19)&&(iSet<60)&&((iSet%5)==0))
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,Batt_Test_Stop[iBT],0);
    iBT++;
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  
   
  }
  if(iSet == 70)
  {
    GPIO_SetPinsOutput(GPIOD,1u << 1u);

    UART_9Bit_Send(RS485T_UART,0x23,1);
    delay();
    GPIO_ClearPinsOutput(GPIOD,1u << 1u);
  }
  iSet++;
  if(iSet>200)
  {
    iSet =0;
    iBT =0;
    settingCommandmask =settingCommandmask & (~STOP_BT_TEST);
    if(settingCommandmask ==0)    setCmd_flag =0; 
    settingCommand =0;
  }
  
}

#endif
