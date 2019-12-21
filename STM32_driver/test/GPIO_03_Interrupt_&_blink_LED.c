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

uint8_t flag = 0;

void delay();

int main(void)
{
	//Configure Led pin as output
	__GPIO_EnPCLK(GPIOA);
	__GPIO_init(GPIOA, 5, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_LOW, GPIO_NO_PUPD, GPIO_ALT_FNC_0);

	//Configure User Button as Interrupt Generator
	__GPIO_EnPCLK(GPIOC);
	__GPIO_init(GPIOC, 13, GPIO_MODE_INT_F, GPIO_OTYPE_PP, GPIO_SPEED_LOW, GPIO_NO_PUPD, GPIO_ALT_FNC_0);
	__GPIO_IRQconfig(13, EN, 0);

	while(1);
}


void EXTI15_10_IRQHandler(void){
	__GPIO_IRQhandling(13);
	delay();
	__GPIO_tooglePin(GPIOA, 5);

}

void delay(){
	uint16_t i;
	for(i=0; i<10000; i++);
}