/*
 * SHT4X.h
 *
 *  Created on: 2022��12��11��
 *      Author: yiyaoyao
 */

#ifndef SRC_SHT4X_H_
#define SRC_SHT4X_H_

unsigned char SHT4XWriteByte(unsigned char Address,unsigned char dat);
unsigned char SHT4XReadSerial(unsigned char Address,unsigned char *dat,unsigned char len);
unsigned char SHT4XReadTemperatureAndHumidity(signed short *temperature,signed short *humidity);


#endif /* SRC_SHT4X_H_ */
