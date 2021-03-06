/**
  ******************************************************************************
  * @file    main.c
  * @author  Auto-generated by STM32CubeIDE
  * @version V1.0
  * @brief   Default main function.
  ******************************************************************************
*/
#include <STM32F303RE.h>
#include <GPIO.h>
#include <I2C.h>

/*
 * SCL  --> PB8	(D15)
 * SDA  --> PB9 (D14)
 * SCL debug  --> PB6
 * SDA debug  --> PB7
 * IMPORTANT: arduino Mega PULLUP resistor is already present on SDA and SCL
 * IMPORTANT: arduino Wire Lib. use a 32 byte rx\tx buffer, if more byte are
 * transmitted\received in the same I2C communication it will be discarded
 */

#define OWN_SLAVEADDR	0x69

void delay();
void PushButton(void);

int main(void)
{
	uint8_t pTxBuf[32] = {0};
	uint8_t pRxBuf[32] = {0};
	uint8_t string[] = "Ciao sono STM32F303RE!\n";
	uint8_t ssize = sizeof(string);

	I2C_handle I2C1_handle;

	I2C1_handle.pI2Cx = I2C1;
	I2C1_handle.pI2Cx_conf.Speed = 100000; //100kHz
	I2C1_handle.pI2Cx_conf.Addr  = OWN_SLAVEADDR;
	I2C1_handle.pTxBuf = NULL;
	I2C1_handle.pRxBuf = NULL;
	I2C1_handle.TxLen  = 0;
	I2C1_handle.RxLen  = 0;

	//Initialize SPI1
	__I2C_EnPCLK(I2C1);
	__I2C_init(&I2C1_handle);

	//Enable GPIOx peripheral clock
	__GPIO_EnPCLK(GPIOA);
	__GPIO_EnPCLK(GPIOB);

	//Configure Led pin as output
	__GPIO_init(GPIOA, 5, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_LOW, GPIO_NO_PUPD, GPIO_ALT_FNC_0);

	//Configure SCL
	__GPIO_init(GPIOB, 8, GPIO_MODE_AF, GPIO_OTYPE_OD, GPIO_SPEED_HIGH, GPIO_NO_PUPD, GPIO_ALT_FNC_4);

	//Configure SDA
	__GPIO_init(GPIOB, 9, GPIO_MODE_AF, GPIO_OTYPE_OD, GPIO_SPEED_HIGH, GPIO_NO_PUPD, GPIO_ALT_FNC_4);

	//Configure SCL for Logic Analyzer
	__GPIO_init(GPIOB, 6, GPIO_MODE_AF, GPIO_OTYPE_OD, GPIO_SPEED_HIGH, GPIO_NO_PUPD, GPIO_ALT_FNC_4);

	//Configure SDA for Logic Analyzer
	__GPIO_init(GPIOB, 7, GPIO_MODE_AF, GPIO_OTYPE_OD, GPIO_SPEED_HIGH, GPIO_NO_PUPD, GPIO_ALT_FNC_4);



	while(1){

		uint8_t command = 0;

		//Enable I2C1
		__I2C_enable(&I2C1_handle);

		//Receive Command
		__I2C_SlaveReceive(&I2C1_handle, pRxBuf, 1);
		command = pRxBuf[0];

		//Decode command
		if(command == 0x51)	//Send string size
		{
			pTxBuf[0] = ssize;
			__I2C_SlaveSend(&I2C1_handle, pTxBuf, 1);
		}
		else if(command == 0x52) //Send string
		{
			__I2C_SlaveSend(&I2C1_handle, string, ssize);
		}
		else if(command == 0x53) //toogle LED
		{
			__GPIO_tooglePin(GPIOA, 5);
		}


		//Disable I2C1
		__I2C_disable(&I2C1_handle);
	}

}


void PushButton(void){

	//Wait until button is pressed
	while(__GPIO_readPin(GPIOC, 13) == TRUE);
	delay();

}

void delay(){
	uint32_t i;
	for(i=0; i<100000; i++);
}
