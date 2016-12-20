/* bsp_eidetect.c */
#include "app_inc.h"
#include "bsp_rf.h"
#include "fsl_device_registers.h"
#include "fsl_lpspi.h"

/*******************************************************************************
* Definitions
******************************************************************************/
/*Master related*/
#define EXAMPLE_LPSPI_MASTER_IRQN LPSPI1_IRQn
#define EXAMPLE_LPSPI_MASTER_PCS_FOR_INIT kLPSPI_Pcs0
#define EXAMPLE_LPSPI_MASTER_PCS_FOR_TRANSFER kLPSPI_MasterPcs0
#define EXAMPLE_LPSPI_MASTER_CLOCK_FREQ 24000000U

#define TRANSFER_SIZE (256U)        /*! Transfer dataSize.*/
#define TRANSFER_BAUDRATE (500000U) /*! Transfer baudrate - 500k */

//******************NRF24L01*************************************
#define TX_ADR_WIDTH 5 // 5 uints TX address width
#define RX_ADR_WIDTH 5 // 5 uints RX address width
//#define TX_PLOAD_WIDTH 20 // 20 uints TX payload
//#define RX_PLOAD_WIDTH 20 // 20 uints TX payload
//#define TX_PLOAD_WIDTH 3 // 3 uints TX payload
//#define RX_PLOAD_WIDTH 3 // 3 uints TX payload
#define TX_PLOAD_WIDTH 2 // 3 uints TX payload
#define RX_PLOAD_WIDTH 2 // 3 uints TX payload
const uint8_t TX_ADDRESS[TX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01}; //????
const uint8_t RX_ADDRESS[RX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01}; //????

//**********************NRF24L01 ?????****************
#define READ_REG 0x00 // ??????
#define WRITE_REG 0x20 // ??????
#define RD_RX_PLOAD 0x61 // ????????
#define WR_TX_PLOAD 0xA0 // ???????
#define FLUSH_TX 0xE1 // ???? FIFO ??
#define FLUSH_RX 0xE2 // ???? FIFO ??
#define REUSE_TX_PL 0xE3 // ??????????
#define NOP 0xFF // ??

//*******************SPI(nRF24L01)?????**********************
#define CONFIG 0x00 // ??????, CRC ??????????????
#define EN_AA 0x01 // ????????
#define EN_RXADDR 0x02 // ??????
#define SETUP_AW 0x03 // ????????
#define SETUP_RETR 0x04 // ????????
#define RF_CH 0x05 // ??????
#define RF_SETUP 0x06 // ???????????
#define STATUS 0x07 // ?????
#define OBSERVE_TX 0x08 // ??????
#define CD 0x09 // ????
#define RX_ADDR_P0 0x0A // ?? 0 ??????
#define RX_ADDR_P1 0x0B // ?? 1 ??????
#define RX_ADDR_P2 0x0C // ?? 2 ??????
#define RX_ADDR_P3 0x0D // ?? 3 ??????
#define RX_ADDR_P4 0x0E // ?? 4 ??????
#define RX_ADDR_P5 0x0F // ?? 5 ??????
#define TX_ADDR 0x10 // ???????
#define RX_PW_P0 0x11 // ???? 0 ??????
#define RX_PW_P1 0x12 // ???? 0 ??????
#define RX_PW_P2 0x13 // ???? 0 ??????
#define RX_PW_P3 0x14 // ???? 0 ??????
#define RX_PW_P4 0x15 // ???? 0 ??????
#define RX_PW_P5 0x16 // ???? 0 ??????
#define FIFO_STATUS 0x17 // FIFO ???????????

/*******************************************************************************
* Prototypes
******************************************************************************/

/*******************************************************************************
* Variables
******************************************************************************/
uint8_t masterRxData[TRANSFER_SIZE] = {0};
uint8_t masterTxData[TRANSFER_SIZE] = {0};

lpspi_transfer_t masterXfer;
uint8_t sta;
#define RX_DR	(sta>>6)&0x01
#define TX_DS (sta>>5)&0x01
#define MAX_RT (sta>>4)&0x01

void RF24L01_InitDevice(void)
{
    lpspi_master_config_t masterConfig;

		/* Initialize CE pin */
    GPIO_SetPinLogic(BSP_GPIO_RF_CE_PORT, BSP_GPIO_RF_CE_PIN, true);
		GPIO_SetPinDir(BSP_GPIO_RF_CE_PORT, BSP_GPIO_RF_CE_PIN, true); /* CE. */
	
    /*Master config*/
    masterConfig.baudRate = TRANSFER_BAUDRATE;
    //masterConfig.bitsPerFrame = 8 * TRANSFER_SIZE;
    masterConfig.bitsPerFrame = 8;
    masterConfig.cpol = kLPSPI_ClockPolarityActiveHigh;
    masterConfig.cpha = kLPSPI_ClockPhaseFirstEdge;
    masterConfig.direction = kLPSPI_MsbFirst;

    masterConfig.pcsToSckDelayInNanoSec = 1000000000 / masterConfig.baudRate;
    masterConfig.lastSckToPcsDelayInNanoSec = 1000000000 / masterConfig.baudRate;
    masterConfig.betweenTransferDelayInNanoSec = 1000000000 / masterConfig.baudRate;

    masterConfig.whichPcs = EXAMPLE_LPSPI_MASTER_PCS_FOR_INIT;
    masterConfig.pcsActiveHighOrLow = kLPSPI_PcsActiveLow;

    masterConfig.pinCfg = kLPSPI_SdiInSdoOut;
    masterConfig.dataOutConfig = kLpspiDataOutRetained;

    LPSPI_MasterInit(BSP_RF_LPSPI_MODULE_NUM, &masterConfig, EXAMPLE_LPSPI_MASTER_CLOCK_FREQ);	
}

void RF24L01_Enable(void)
{
    GPIO_SetPinLogic(BSP_GPIO_RF_CE_PORT, BSP_GPIO_RF_CE_PIN, true);	
		delay_us(150);
}

void RF24L01_Disable(void)
{
    GPIO_SetPinLogic(BSP_GPIO_RF_CE_PORT, BSP_GPIO_RF_CE_PIN, false);	
		delay_us(150);
}

void SPI_Write_Buf(uint8_t reg, uint8_t *buf, uint8_t bufsize)
{
		masterTxData[0] = reg;
		memcpy(&masterTxData[1], buf, bufsize);
	
    /*Start master transfer*/
    masterXfer.txData = masterTxData;
    masterXfer.rxData = masterRxData;
    masterXfer.dataSize = bufsize + 1;
    masterXfer.configFlags = EXAMPLE_LPSPI_MASTER_PCS_FOR_TRANSFER | kLPSPI_MasterPcsContinuous | kLPSPI_SlaveByteSwap;

    LPSPI_MasterTransferBlocking(BSP_RF_LPSPI_MODULE_NUM, &masterXfer);		
		delay_us(100);
}

void SPI_Read_Buf(uint8_t reg, uint8_t *buf, uint8_t bufsize)
{
		uint8_t i;
	
		masterTxData[0] = reg;
		for(i=0; i<bufsize; i++) {
			masterTxData[i+1] = 0;
		}
			
    /*Start master transfer*/
    masterXfer.txData = masterTxData;
    masterXfer.rxData = masterRxData;
    masterXfer.dataSize = bufsize + 1;
    masterXfer.configFlags = EXAMPLE_LPSPI_MASTER_PCS_FOR_TRANSFER | kLPSPI_MasterPcsContinuous | kLPSPI_SlaveByteSwap;

    LPSPI_MasterTransferBlocking(BSP_RF_LPSPI_MODULE_NUM, &masterXfer);		
		
		memcpy(buf, &masterRxData[1], bufsize);
		delay_us(100);
}

void SPI_RW_Reg(uint8_t reg, uint8_t value)
{
		masterTxData[0] = reg;
		masterTxData[1] = value;
	
    /*Start master transfer*/
    masterXfer.txData = masterTxData;
    masterXfer.rxData = masterRxData;
    masterXfer.dataSize = 2;
    masterXfer.configFlags = EXAMPLE_LPSPI_MASTER_PCS_FOR_TRANSFER | kLPSPI_MasterPcsContinuous | kLPSPI_SlaveByteSwap;

    LPSPI_MasterTransferBlocking(BSP_RF_LPSPI_MODULE_NUM, &masterXfer);			
		delay_us(100);
}

uint8_t SPI_Read(uint8_t reg)
{
		masterTxData[0] = reg;
		masterTxData[1] = 0;
	
    /*Start master transfer*/
    masterXfer.txData = masterTxData;
    masterXfer.rxData = masterRxData;
    masterXfer.dataSize = 2;
    masterXfer.configFlags = EXAMPLE_LPSPI_MASTER_PCS_FOR_TRANSFER | kLPSPI_MasterPcsContinuous | kLPSPI_SlaveByteSwap;

    LPSPI_MasterTransferBlocking(BSP_RF_LPSPI_MODULE_NUM, &masterXfer);	

		delay_us(100);
		
		return (masterRxData[1]);
}


/***************************************************************/
/*NRF24L01 ???                                                 */
/***************************************************************/
void RF24L01_Init(void)
{
		delay_us(100);
		RF24L01_Disable(); // chip disable
		SPI_Write_Buf(WRITE_REG + TX_ADDR, (uint8_t *)TX_ADDRESS, TX_ADR_WIDTH); //		?????
		SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, (uint8_t *)RX_ADDRESS, RX_ADR_WIDTH); //		??????
		SPI_RW_Reg(WRITE_REG + EN_AA, 0x01); // ?? 0 ?? ACK ? ? ? ?
//		SPI_RW_Reg(WRITE_REG + EN_AA, 0x00); // ?? 0 ?? ACK ? ? ? ?
		SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01); //?????????? 0,??		????????? Page21
		SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x1a); //关闭自动重发10次，等待500us
//		SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x00); //关闭自动重发
		SPI_RW_Reg(WRITE_REG + RF_CH, 32); // ??????? 2.4GHZ, ??????
		SPI_RW_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //????????,		????? 32 ??
		SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07); //??????? 1MHZ,??		?????? 0dB
}

/***************************************************************/
/*??: void SetRX_Mode(void)                                    */
/*??:??????                                                    */
/***************************************************************/
void SetRX_Mode(void)
{
		RF24L01_Disable(); // chip disable
		SPI_RW_Reg(WRITE_REG+STATUS,0xff);
		SPI_RW_Reg(FLUSH_RX,0x00);
		SPI_RW_Reg(WRITE_REG + CONFIG, 0x7f); // IRQ ????????, 16? CRC ,???
		RF24L01_Enable(); // chip enable
		delay_us(130);
}

/***************************************************************/
/*??: unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)   */
/*??:??????? rx_buf ??????                                     */
/***************************************************************/
unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
{
		unsigned char revale=0;
		sta=SPI_Read(STATUS); // ????????????????
		if(RX_DR) // ?????????
		{
			RF24L01_Disable(); // chip disable
			SPI_Read_Buf(RD_RX_PLOAD,rx_buf,RX_PLOAD_WIDTH); // read receive Payload from	RX_FIFO buffer
			SPI_RW_Reg(FLUSH_RX,0x00);
			revale =1; //????????
		}
		SPI_RW_Reg(WRITE_REG+STATUS,sta); //?????? RX_DR,TX_DS,MAX_PT		???? 1,??? 1 ???????
		return revale;
}

/***************************************************************/
/*??: void nRF24L01_TxPacket(unsigned char * tx_buf)           */
/*??:?? tx_buf ???                                             */
/***************************************************************/
void nRF24L01_TxPacket(unsigned char * tx_buf)
{
		RF24L01_Disable(); // chip disable
		SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, (uint8_t *)TX_ADDRESS, TX_ADR_WIDTH); //		???????
		SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH); // ????
		RF24L01_Enable(); //?? CE,??????
		delay_us(150);
		sta = SPI_Read(STATUS);
		SPI_RW_Reg(WRITE_REG+STATUS,sta);
		if(TX_DS == 1) {
			RF24L01_Disable();
			SPI_RW_Reg(FLUSH_TX,0x00);
			RF24L01_Enable();			
		}		
}

void SetTX_Mode(void)
{
		RF24L01_Disable(); // chip disable
		SPI_RW_Reg(WRITE_REG+STATUS,0xff);
		SPI_RW_Reg(FLUSH_TX,0x00);
		SPI_RW_Reg(WRITE_REG+CONFIG,0x7e);
		RF24L01_Enable(); //?? CE,??????
		delay_us(150);			
}


/* EOF. */
