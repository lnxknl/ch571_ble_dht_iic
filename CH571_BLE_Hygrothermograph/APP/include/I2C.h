/*
 * I2C.h
 *
 *  Created on: 2022Äê12ÔÂ11ÈÕ
 *      Author: yiyaoyao
 */

#ifndef SRC_I2C_H_
#define SRC_I2C_H_
#include "CH57x_common.h"

#ifndef UINT8
typedef unsigned char           u8;
#endif
#define I2C_Pin_SCL        GPIO_Pin_5
#define I2C_Pin_SDA        GPIO_Pin_15

#define SCL_H         GPIOA_SetBits( I2C_Pin_SCL )
#define SCL_L         GPIOA_ResetBits( I2C_Pin_SCL )
#define SDA_H         GPIOA_SetBits( I2C_Pin_SDA )
#define SDA_L         GPIOA_ResetBits( I2C_Pin_SDA )
#define SCL_read      GPIOA_ReadPortPin( I2C_Pin_SCL )
#define SDA_read      GPIOA_ReadPortPin( I2C_Pin_SDA )
#define SCL_out          GPIOA_ModeCfg(I2C_Pin_SCL, GPIO_ModeOut_PP_5mA)
#define SCL_in          GPIOA_ModeCfg(I2C_Pin_SCL, GPIO_ModeIN_PU)
#define SDA_out          GPIOA_ModeCfg(I2C_Pin_SDA, GPIO_ModeOut_PP_5mA)
#define SDA_in          GPIOA_ModeCfg(I2C_Pin_SDA, GPIO_ModeIN_PU)


void I2CInit();
void I2C_delay();
void I2CStart();
void I2CStop();
unsigned char I2CWrite(unsigned char I2Cdata);
unsigned char I2CAddress(unsigned char address);
unsigned char I2CReadACK();
unsigned char I2CReadNAK();



#endif /* SRC_I2C_H_ */
