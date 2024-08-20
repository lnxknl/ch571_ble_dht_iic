/*
 * SHT4X.c
 *
 *  Created on: 2022年12月11日
 *      Author: yiyaoyao
 */
#include "SHT4X.h"
#include "I2C.h"

/* SHT4X写一个字节 */
unsigned char SHT4XWriteByte(unsigned char Address,unsigned char dat)
{
    I2CStart();
    if (I2CWrite(Address) == 0)
    {
        I2CStop();
        return 2;
    }
    //发送数据
    if (I2CWrite(dat) == 0)
    {
        I2CStop();
        return 3;
    }
    I2CStop();

    return 1;
}
/* SHT4X读多字节  */
unsigned char SHT4XReadSerial(unsigned char Address,unsigned char *dat,unsigned char len)
{
    unsigned char ACK;

    I2CStart();
    ACK = I2CWrite(Address);
    if (ACK == 0)
    {
        I2CStop();
        return 0;
    }
    while(len > 1)
    {
        *dat++ = I2CReadACK();
        len--;
    }
    *dat = I2CReadNAK();//最后一个字节为读取操作+非应答
    I2CStop();

    return 1;
}
/* 读取温湿度 */
unsigned char SHT4XReadTemperatureAndHumidity(signed short *temperature,signed short *humidity)// @NOTE 
{
    unsigned char RxBuff[6];
    signed short HdataBuf, TdataBuf;
    float Hdata,Tdata;

    I2CInit();
    DelayUs(2);

    RxBuff[0] = SHT4XWriteByte(0x88,0xfd);
    if(RxBuff[0] == 0)
    {
        //SCL SDA 设置上拉输入
        GPIOA_SetBits(GPIO_Pin_5 | GPIO_Pin_15);
        GPIOA_ModeCfg(GPIO_Pin_5 | GPIO_Pin_15, GPIO_ModeIN_PU);

        return 0;//没收到SHT4X的ACK
    }
    DelayMs(10);//延时等待测量
    SHT4XReadSerial(0x89,RxBuff,6);

    //SCL SDA 设置上拉输入
    GPIOA_SetBits(GPIO_Pin_5 | GPIO_Pin_15);
    GPIOA_ModeCfg(GPIO_Pin_5 | GPIO_Pin_15, GPIO_ModeIN_PU);

    Tdata = RxBuff[0]*256 + RxBuff[1];
    Hdata = RxBuff[3]*256 + RxBuff[4];
    TdataBuf = (Tdata/65535) * 100;
    HdataBuf = (Hdata/65535) * 100;
    TdataBuf = 175*TdataBuf - 4500;
    HdataBuf = 125*HdataBuf - 600;

    //*temperature = (signed short)Tdata;
    *temperature = TdataBuf;
    *humidity = HdataBuf;

    return 1;
}
