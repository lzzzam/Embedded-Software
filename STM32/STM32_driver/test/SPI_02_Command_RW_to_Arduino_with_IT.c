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

/************** PERIPHERAL VARIABLES ******************/
SPI_Handle SPI1_handle;
SPI_Config SPI1_config;

/************** GLOBAL VARIABLES **********************/

#define HELLO_STRING	"CIAO SONO STMF303RE!\n"
#define SSIZE(x)	sizeof(x)
const uint8_t command1 = 0x1C;	//Send string to Arduino
const uint8_t command2 = 0x1D;	//Send uint32_t value to Arduino
const uint8_t command3 = 0x1E;	//Receive string from Arduino
uint32_t dummy = 0;			    //Used to fetch data from Slave
char dummyStr[100] = {0};		//Used to fetch string from slave
uint32_t intVal = 491238472;
char str[100] = {0};
uint8_t str_size = 0;

uint8_t cmdCounter = 0;

struct Command{
	uint8_t opcode;
	uint8_t arg0;
	uint8_t arg1[100];
};

/********************* FUNCTIONS **********************/

void initSPI(){

	SPI1_config.Mode	  = SPI_MODE_MASTER;
	SPI1_config.BusConfig = SPI_BUS_FULLDUPLEX;
	SPI1_config.BaudRate  = SPI_BR_PCLK_DIV2;	//SCLK at 4MHz
	SPI1_config.CPha 	  = SPI_CPHA_FIRST;
	SPI1_config.CPol 	  = SPI_CPOL_LOW;
	SPI1_config.DataSize  = SPI_DS_8BIT;
	SPI1_config.SSMgm     = SPI_SS_HW;


	SPI1_handle.pSPIx 	   = SPI1;
	SPI1_handle.pSPIx_conf = SPI1_config;

	//Initialize SPI1
	__SPI_EnPCLK(SPI1);
	__SPI_init(&SPI1_handle);
}

void initGPIO(){

	//Enable GPIOx peripheral clock
	__GPIO_EnPCLK(GPIOA);
	__GPIO_EnPCLK(GPIOB);
	__GPIO_EnPCLK(GPIOC);

	//Configure User Button as Interrupt Generator
	__GPIO_init(GPIOC, 13, GPIO_MODE_INT_F, GPIO_OTYPE_PP, GPIO_SPEED_LOW, GPIO_NO_PUPD, GPIO_ALT_FNC_0);

	//Configure PA5 as output to drive LED
	__GPIO_init(GPIOA, 5, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_LOW, GPIO_NO_PUPD, GPIO_ALT_FNC_0);

	//Configure NSS
	__GPIO_init(GPIOA, 4, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PULL_UP, GPIO_ALT_FNC_5);

	//Configure SCLK
	__GPIO_init(GPIOB, 3, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PULL_UP, GPIO_ALT_FNC_5);

	//Configure MISO
	__GPIO_init(GPIOB, 4, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_NO_PUPD, GPIO_ALT_FNC_5);

	//Configure MOSI
	__GPIO_init(GPIOB, 5, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PULL_UP, GPIO_ALT_FNC_5);

}

void delay(){
	uint32_t i;
	for(i=0; i<100; i++);
}


void SPI1_IRQHandler(void){
	__SPI_IRQhandle(&SPI1_handle);
}


void EXTI15_10_IRQHandler(void){

	__GPIO_IRQhandling(13);

	//Blink LED
	__GPIO_tooglePin(GPIOA, 5);
	delay();
	__GPIO_tooglePin(GPIOA, 5);

	//Increment Command counter
	if(cmdCounter < 3) cmdCounter++;
	else cmdCounter = 1;

	//Execute command
	if(cmdCounter == 1){

		struct Command SendString = {command1,
									 SSIZE(HELLO_STRING),
									 HELLO_STRING
									 };

		//Enable SPI1
		__SPI_enable(&SPI1_handle);

		//Send command1
		__SPI_sendData_IT(&SPI1_handle,(uint8_t *)&SendString, 2 + SSIZE(HELLO_STRING));

	}
	else if(cmdCounter == 2){

		struct Command SendInt = {command2,
								  4,
								  {(uint8_t)intVal, (uint8_t)(intVal >> 8), (uint8_t)(intVal >> 16), (uint8_t)(intVal >> 24)}
								  };

		//Send command2
		__SPI_sendData_IT(&SPI1_handle,(uint8_t *)&SendInt, 6);

	}
	else if(cmdCounter == 3){

		//Send command3
		__SPI_sendData(&SPI1_handle,(uint8_t *)&command3, 1);

		//clear RXFIFO
		__SPI_receiveData(&SPI1_handle, (uint8_t *)&dummy, 4);

		//Read string size
		__SPI_sendData(&SPI1_handle,(uint8_t *)&dummy, 1);
		__SPI_receiveData(&SPI1_handle, (uint8_t *)&str_size, 1);

		//Read string
		__SPI_sendData_IT(&SPI1_handle,(uint8_t *)dummyStr, str_size);
		__SPI_receiveData_IT(&SPI1_handle, (uint8_t *)str, str_size);

	}


}


int main(void)
{

	//initialize SPI
	initSPI();

	//initialize GPIO pins
	initGPIO();

	//initialize IRQ for SPI and GPIO
	__SPI_IRQconfig(&SPI1_handle, EN, 1);
	__GPIO_IRQconfig(13, EN, 0);

	while(1){
		/*
		 * do stuff here
		 */
	}

}


void __SPI_AppEventCallback(SPI_Handle *pSPIx_h, uint8_t AppEv){

	//If last command is complete close communication
	//and print received string
	if((cmdCounter == 3) && (AppEv == SPI_EVENT_RX_COMPLETE)){

		//Disable SPI
		while(__SPI_get_SRflag(&SPI1_handle, SPI_SR_BSY));
		__SPI_disable(&SPI1_handle);


		//Set AF0 for PB3 (SWO)
		__GPIO_init(GPIOB, 3, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PULL_UP, GPIO_ALT_FNC_0);

		//Transmit received string through SWO
		printf("%s",str);
		delay();

		//Set AF5 for PB3 (SPI1_SCLK)
		__GPIO_init(GPIOB, 3, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PULL_UP, GPIO_ALT_FNC_5);
	}

}