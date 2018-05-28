/*
 * rotary_encoder.c
 *
 *  Created on: 11-Apr-2018
 *      Author: medprime
 */

#include "rotary_encoder.h"
#include "main.h"

// multi encoder code with speed up values adapted for STM32-arduino by Matthias Diro
// no caps for encoder Pins needed: gives back wrong acc values
// using gpio_read_bit instead of digitalRead gives about 4x more speed http://forums.leaflabs.com/topic.php?id=1107
#define MAX_ENCODERS 3

typedef struct Encoder_Struct_t
{
	uint16_t Encoder_Pin_0;
	uint16_t Encoder_Pin_1;
	GPIO_TypeDef* Encoder_Pin_0_Port;
	GPIO_TypeDef* Encoder_Pin_1_Port;
	uint8_t Encoder_Pin_0__State;
	uint8_t Encoder_Pin_1__State;
	uint32_t Encoder_Time_Stamp;
	int16_t Encoder_Count;

} Encoder_Struct_t;

static Encoder_Struct_t Encoder_Array[MAX_ENCODERS];

static uint8_t Attached_Encoders = 0;


uint8_t Encoder_Attach(uint16_t _Encoder_Pin_0, GPIO_TypeDef* _Encoder_Pin_0_Port,
		uint16_t _Encoder_Pin_1, GPIO_TypeDef* _Encoder_Pin_1_Port)
{
	//init GPIOs as input

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE()
	;
	__HAL_RCC_GPIOD_CLK_ENABLE()
	;
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;
	__HAL_RCC_GPIOB_CLK_ENABLE()
	;

	GPIO_InitStruct.Pin = _Encoder_Pin_0;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(_Encoder_Pin_0_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = _Encoder_Pin_1;
	HAL_GPIO_Init(_Encoder_Pin_1_Port, &GPIO_InitStruct);

	Encoder_Array[Attached_Encoders].Encoder_Pin_0 = _Encoder_Pin_0;
	Encoder_Array[Attached_Encoders].Encoder_Pin_1 = _Encoder_Pin_1;
	Encoder_Array[Attached_Encoders].Encoder_Pin_0_Port = _Encoder_Pin_0_Port;
	Encoder_Array[Attached_Encoders].Encoder_Pin_1_Port = _Encoder_Pin_1_Port;
	Encoder_Array[Attached_Encoders].Encoder_Time_Stamp = 0;

	Encoder_Array[Attached_Encoders].Encoder_Pin_0__State = HAL_GPIO_ReadPin(
			Encoder_Array[Attached_Encoders].Encoder_Pin_0_Port,
			Encoder_Array[Attached_Encoders].Encoder_Pin_0);

	Encoder_Array[Attached_Encoders].Encoder_Pin_1__State = HAL_GPIO_ReadPin(
			Encoder_Array[Attached_Encoders].Encoder_Pin_1_Port,
			Encoder_Array[Attached_Encoders].Encoder_Pin_1);


	Attached_Encoders++;

	if (Attached_Encoders > MAX_ENCODERS)
	{
		_Error_Handler(__FILE__, __LINE__);
		//Error
	}
	return (Attached_Encoders - 1);//return encoder ID
}


void Encoder_Scan()
{

	for (uint8_t Index = 0; Index < Attached_Encoders; Index++)
	{
		if (HAL_GPIO_ReadPin(Encoder_Array[Index].Encoder_Pin_0_Port,
				Encoder_Array[Index].Encoder_Pin_0)
				!= Encoder_Array[Index].Encoder_Pin_0__State)
		{
			Encoder_Array[Index].Encoder_Pin_0__State =
					!Encoder_Array[Index].Encoder_Pin_0__State;
			if (Encoder_Array[Index].Encoder_Pin_0__State
					&& !Encoder_Array[Index].Encoder_Pin_1__State)
			{
				if (HAL_GetTick() - Encoder_Array[Index].Encoder_Time_Stamp
						> 3)
				{
					Encoder_Array[Index].Encoder_Count += 1;
				}
				else
				{
					Encoder_Array[Index].Encoder_Count += 5;
				}

				Encoder_Array[Index].Encoder_Time_Stamp = HAL_GetTick();
			}
		}
		if (HAL_GPIO_ReadPin(Encoder_Array[Index].Encoder_Pin_1_Port,
				Encoder_Array[Index].Encoder_Pin_1)
				!= Encoder_Array[Index].Encoder_Pin_1__State)
		{
			Encoder_Array[Index].Encoder_Pin_1__State =
					!Encoder_Array[Index].Encoder_Pin_1__State;
			if (Encoder_Array[Index].Encoder_Pin_1__State
					&& !Encoder_Array[Index].Encoder_Pin_0__State)
			{
				if (HAL_GetTick() - Encoder_Array[Index].Encoder_Time_Stamp
						> 3)
				{
					Encoder_Array[Index].Encoder_Count -= 1;
				}
				else
				{
					Encoder_Array[Index].Encoder_Count -= 5;
				}
				Encoder_Array[Index].Encoder_Time_Stamp = HAL_GetTick();
			}
		}
	}

}


int16_t Encoder_Get_Count(Encoder_t Encoder)
{
	if (Encoder > Attached_Encoders)
	{
		_Error_Handler(__FILE__, __LINE__);
		//Error
	}
	return Encoder_Array[Encoder].Encoder_Count;
}
void Encoder_Reset_Count(Encoder_t Encoder)
{
	if (Encoder > Attached_Encoders)
	{
		_Error_Handler(__FILE__, __LINE__);
		//Error
	}
	else
	{
		Encoder_Array[Encoder].Encoder_Count = 0;
	}
}

