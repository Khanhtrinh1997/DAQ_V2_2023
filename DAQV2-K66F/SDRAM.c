#include"fsl_sdramc.h"

sdramc_refresh_config_t config_refresh;
sdramc_blockctl_config_t config_block;
sdramc_config_t config_sdramc;

extern uint16_t *Pointer;
uint16_t *bPointer;
void SDdelay(void)
{
  int i;
   for(i=0;i<8;i++)
     {
       __asm("nop");
     }
}
void sdramInit(void)
{
      
	SIM->SCGC7|=0x09; //enable SDRAM and FlexBus clock
	FB->CSPMCR=0x2222000;
	SIM->SOPT2|=0x300; //set FBSL bit so that the core can access external memory
	//64ms refresh 2048 row address(2K), each row address takes 64ms/2048=32.25uS,  20MHz SDRAM clock, RC=(31.25uS/0.05uS)/16-1=38=0x26, RTIM=0
	SIM->CLKDIV1&=~(0x00F00000); //clear OUTDIV3 bits so that CLKOUT is 20MHz
	SDRAM->CTRL=0x0039; //
	//The SDRAM memory space is 0x80000000, the CBM=3, SDRAM_A19 is A10, the A10 determine the precharge instruction
	SDRAM->BLOCK[0].AC=0x70003320; //CASL=010, PS=10 16 bits,
        SDRAM->BLOCK[1].AC=0x8C500000; //CASL=010, PS=10 16 bits,
	//the SDRAM is MT48LC2M32,
	//column address:A0~A7, 2**(8)=256
	//row address:A0~A11, 2**(12)=4096=4K
	//bank address:BA0/Ba1, 4 banks
	//total memory in bytes:256*4096*(4bank)*4*8bit=8M*8bits=64MBits
	//the address is from A0~A22, in other words, the address is from 0x8000_0000 to 0x807F_FFFF
	SDRAM->BLOCK[0].CM=0x7C0001; //bits A22/21/20/19/18 do not care
	//initialize the MSR register:
	//writting MRS is to write any value to the address which the SDRAM address match with

	//launch initialization sequence for SDRAM
	//issue precharge by setting the IP bit
	SDRAM->BLOCK[0].AC|=0x08;

	*(uint16_t *)Pointer=0x1234;
		SDdelay();

	//enable refresh by setting the RE bit
	SDRAM->BLOCK[0].AC|=0x8000;
	*(uint16_t *)Pointer=0x1234;  //write any valu
	SDdelay();

	//setting the IMRS bit
	//the low address is as following 0x020
	//WB bit 9: 0
	//OP mode bits 8:7: 00
	//CAS latency bits 6:5:4: 010
	//BT bit 3: 0
	//burst length bits 2:1:0: 000     /burst length 1
	SDRAM->BLOCK[0].AC|=0x40;
	bPointer=(uint16_t *)0x70000100;
	*(uint16_t *)bPointer=0x56; //write any value
	SDdelay();
}
void InitSDRAM(void)
{
  config_refresh.refreshTime = kSDRAMC_RefreshSixClocks;
  config_refresh.sdramRefreshRow = 0x39;
  config_refresh.busClock_Hz = 60000000;
  config_block.block = kSDRAMC_Block1;
  config_block.latency = kSDRAMC_LatencyThree;
  config_block.location = kSDRAMC_Commandbit19;
  config_block.portSize = kSDRAMC_PortSize16Bit;
  config_block.address  = 0x70000300;
  config_block.addressMask =  0x7c0000;
  config_sdramc.numBlockConfig = 2;
  config_sdramc.refreshConfig = &config_refresh;
  config_sdramc.blockConfig = &config_block;
  SDRAMC_Init(SDRAM, &config_sdramc);
  
  SDRAMC_SendCommand(SDRAM, kSDRAMC_Block0, kSDRAMC_PrechargeCommand);
  SDdelay();
  SDRAMC_SendCommand(SDRAM, kSDRAMC_Block0, kSDRAMC_AutoRefreshEnableCommand);
  SDdelay(); 
  SDRAMC_SendCommand(SDRAM, kSDRAMC_Block0, kSDRAMC_ImrsCommand); 
  SDdelay();
}  
//#define SDRAMADDR 0x70000000 
//#define NUMBER 2048 
//unsigned int array[NUMBER]; 
//static int index = 0;
//void sdramTest(void)
//{
//  unsigned int *Pointer;
//  Pointer =(unsigned int *)SDRAMADDR;
//  for(index=0; index<NUMBER; index++)
//  {
//  *Pointer++=index;
//  }
//  __asm("nop");
//  Pointer =(unsigned int *)SDRAMADDR;
//  for(index=0; index<NUMBER; index++)
//  {
//  array[index]=*(unsigned int *)Pointer++;
//  if(array[index]!=index)
//  __asm("nop"); //set a break point here to check there is error
//  }
//  __asm("nop"); //set a break here
//}