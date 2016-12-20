/* main.c */
#include "app_inc.h"

/*
* 调试日志：
* 4/30, 2016 接收可以，发送不行
*/

NRF24L01_Config_T gNrf24l01ConfigStruct =
{
    {0x34, 0x43, 0x10, 0x10, 0x01},
    {0x34, 0x43, 0x10, 0x10, 0x01}
};

uint8_t gRfBuffer[NRF24L01_XFER_PACKAGE_LENGTH];

void EnterRxWork(void);
void EnterTxWork(void);

int main(void)
{
    uint8_t ch;

    BSP_InitSystem();
    BSP_InitStdioLPUART(115200UL);
    
    printf("\r\n\r\nNRF24L01 Example\r\n");
    printf("Compiled at %s on %s\r\n", __TIME__, __DATE__);
    
    if (NRF24L01_Init(&gNrf24l01ConfigStruct))
    {
        printf("NRF24L01 is ready.\r\n");
    }
    else
    {
        printf("NRF24L01 is not detected.\r\n");
    }
    
    while (1)
    {
        printf("\r\nRx mode or Tx mode? R/T: ");
        ch = getchar();
        if ( (ch == 'R') || (ch == 'T') )
        {
            break;
        }
    }
    
    if (ch == 'R')
    {
        EnterRxWork();
    }
    else
    {
        EnterTxWork();
    }
}

void EnterRxWork(void)
{
    uint32_t i;

    printf("\r\nEnterRxWork()\r\n");
    
    
    printf("Waiting for the data ...\r\n");
    while (1)
    {
        if (NRF24L01_GetRxPackageReadyFlag())
        {
            NRF24L01_GetRxPackage(gRfBuffer);
            NRF24L01_ClearRxPackageReadyFlag();
            
            printf("Rx Done\r\n");
            for (i = 0U; i < NRF24L01_XFER_PACKAGE_LENGTH; i++)
            {
                printf("%2d: 0x%X\r\n", i, gRfBuffer[i]);
            }
            printf("\r\n");
        }
    }

}

void EnterTxWork(void)
{
    uint32_t i;
    uint8_t ch;

    printf("EnterTxWork()\r\n");
    printf("Press any key to tansfer the data ...\r\n");
    while (1)
    {
        ch = getchar();
        for (i = 0U; i < NRF24L01_XFER_PACKAGE_LENGTH; i++)
        {
            gRfBuffer[i] = ch;
        }
				
				/* For test. */
        //gRfBuffer[0] = 31;
				
        if (NRF24L01_TxPackage(gRfBuffer))
        {
            printf("Tx Done.\r\n");
        }
        else
        {
            printf("Tx Failed.\r\n");
        }
    }
}

/* EOF. */
