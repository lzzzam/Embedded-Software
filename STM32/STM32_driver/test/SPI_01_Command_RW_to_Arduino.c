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
#include <SPI.h>

/*
 * NSS  --> PA4	(A2)
 * SCLK --> PB3 (D3)
 * MISO --> PB4 (D5)
 * MOSI --> PB5 (D4)
 */


void delay();
void PushButton(void);

int main(void)
{

	SPI_Handle SPI1_handle;
	SPI_Config SPI1_config;

	SPI1_config.Mode	  = SPI_MODE_MASTER;
	SPI1_config.BusConfig = SPI_BUS_FULLDUPLEX;
	SPI1_config.BaudRate  = SPI_BR_PCLK_DIV16;	//SCLK at 500kHz
	SPI1_config.CPha 	  = SPI_CPHA_FIRST;
	SPI1_config.CPol 	  = SPI_CPOL_LOW;
	SPI1_config.DataSize  = SPI_DS_8BIT;
	SPI1_config.SSMgm     = SPI_SS_HW;


	SPI1_handle.pSPIx 	   = SPI1;
	SPI1_handle.pSPIx_conf = SPI1_config;

	//Initialize SPI1
	__SPI_EnPCLK(SPI1);
	__SPI_init(&SPI1_handle);

	//Enable GPIOx peripheral clock
	__GPIO_EnPCLK(GPIOA);
	__GPIO_EnPCLK(GPIOB);
	__GPIO_EnPCLK(GPIOC);

	//Configure User Button as Input
	__GPIO_init(GPIOC, 13, GPIO_MODE_IN, GPIO_OTYPE_PP, GPIO_SPEED_LOW, GPIO_NO_PUPD, GPIO_ALT_FNC_0);

	//Configure NSS
	__GPIO_init(GPIOA, 4, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PULL_UP, GPIO_ALT_FNC_5);

	//Configure SCLK
	__GPIO_init(GPIOB, 3, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PULL_UP, GPIO_ALT_FNC_5);

	//Configure MISO
	__GPIO_init(GPIOB, 4, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_NO_PUPD, GPIO_ALT_FNC_5);

	//Configure MOSI
	__GPIO_init(GPIOB, 5, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PULL_UP, GPIO_ALT_FNC_5);


	uint8_t command1 = 0x1C;	//Send string to Arduino
	uint8_t command2 = 0x1D;	//Send uint32_t value to Arduino
	uint8_t command3 = 0x1E;	//Receive string from Arduino
	uint32_t dummy = 0;			//Used to fetch data from Slave

	const char String[] = "Ciao sono STM32F303RE!!\n";
	const uint8_t ssize = sizeof(String);
	uint8_t str_size = 0;
	char str[100] = {0};
	uint8_t i;

	uint32_t intVal = 491238472;

	while(1){

		PushButton();

		//Enable SPI1
		__SPI_enable(&SPI1_handle);

		//Send command1
		__SPI_sendData(&SPI1_handle,(uint8_t *)&command1, 1);
		__SPI_sendData(&SPI1_handle,(uint8_t *)&ssize, 1);
		__SPI_sendData(&SPI1_handle,(uint8_t *)String, ssize);

		PushButton();
		//Send command2
		__SPI_sendData(&SPI1_handle,(uint8_t *)&command2, 1);
		__SPI_sendData(&SPI1_handle,(uint8_t *)&intVal , 4);

		PushButton();

		//clear RXFIFO
		__SPI_receiveData(&SPI1_handle, (uint8_t *)&dummy, 4);
		//Send command3
		__SPI_sendData(&SPI1_handle,(uint8_t *)&command3, 1);
		__SPI_receiveData(&SPI1_handle, (uint8_t *)&dummy, 1);

		//fetch size
		__SPI_sendData(&SPI1_handle,(uint8_t *)&dummy, 1);
		__SPI_receiveData(&SPI1_handle, (uint8_t *)&str_size, 1);

		//read string
		for(i=0; i<str_size; i++){
			__SPI_sendData(&SPI1_handle,(uint8_t *)&dummy, 1);
			__SPI_receiveData(&SPI1_handle, (uint8_t *)&str[i], 1);
		}

		while(__SPI_get_SRflag(&SPI1_handle, SPI_SR_BSY));
		__SPI_disable(&SPI1_handle);


		//Set AF0 for PB3 (SWO)
		__GPIO_init(GPIOB, 3, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PULL_UP, GPIO_ALT_FNC_0);

		//Transmit received string through SWO into SerialPort 0
		printf(str);
		delay();

		//Set AF5 for PB3 (SPI1_SCLK)
		__GPIO_init(GPIOB, 3, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PULL_UP, GPIO_ALT_FNC_5);


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
