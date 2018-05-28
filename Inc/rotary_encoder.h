/*
 * rotary_encoder.h
 *
 *  Created on: 11-Apr-2018
 *      Author: medprime
 */

#ifndef ROTARY_ENCODER_H_
#define ROTARY_ENCODER_H_

#include"stm32f1xx_hal.h"

#define HIGH 1
#define LOW  0

typedef uint8_t Encoder_t;

typedef enum Enoder_State_t
{
	Enoder_State_00=0,
	Enoder_State_01,
	Enoder_State_10,
	Enoder_State_11
}Enoder_State_t;

void Encoder_Init();
int16_t Encoder_Get_Count(uint8_t Encoder);
void Encoder_Scan();
uint8_t Encoder_Attach(uint16_t Button_Pin_0, GPIO_TypeDef* Button_Pin_0_Port,
		uint16_t Button_Pin_1, GPIO_TypeDef* Button_Pin_1_Port);



#endif /* ROTARY_ENCODER_H_ */
