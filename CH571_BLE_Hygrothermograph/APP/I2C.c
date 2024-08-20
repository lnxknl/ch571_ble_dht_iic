/*
 * I2C.c
 *
 *  Created on: 2022年12月11日
 *      Author: yiyaoyao
 */
#include "I2C.h"


//模拟IIC初始化
void I2CInit()
{
    SCL_out;
    SDA_out;
}


void I2C_delay()
{
   mDelayuS(100);
}

void I2CStart()
{
    SDA_out;
    mDelayuS(1);
    SCL_H;
    SDA_H;
    mDelayuS(5);
    SDA_L;
    mDelayuS(5);
    SCL_L;
}

void I2CStop()
{
    SDA_out;
    mDelayuS(1);
    SCL_L;
    SDA_L;
    mDelayuS(5);
    SCL_H;
    mDelayuS(5);
    SDA_H;
    mDelayuS(5);
}

unsigned char I2CWrite(unsigned char I2Cdata)
{
     unsigned char mask;

     SDA_out;
     mDelayuS(1);
     for (mask = 0x80; mask != 0; mask >>= 1)
     {
         if (mask & I2Cdata)
             SDA_H;
         else
             SDA_L;
         mDelayuS(5);
         SCL_H;
         mDelayuS(5);
         SCL_L;
     }
     SDA_H;
     SDA_in;
     mDelayuS(5);
     SCL_H;
     mDelayuS(5);
     if(SDA_read)
     {
         SCL_L;
         return 0;
     }
     else
     {
         SCL_L;
         return 1;
     }
}
unsigned char I2CAddress(unsigned char address)
{
    unsigned char ack = 0;

     I2CStart();
     ack = I2CWrite(address);
     I2CStop();

     return ack;
}
unsigned char I2CReadACK()
{
     unsigned char mask;
     unsigned char I2Cdata;

     SDA_H;
     SDA_in;

     for(mask = 0x80;mask != 0; mask >>= 1)
     {
         mDelayuS(5);
         SCL_H;
          if(SDA_read == 0)
              I2Cdata &= ~mask;
          else
              I2Cdata |= mask;
          mDelayuS(5);
          SCL_L;
     }
     SDA_out;
     mDelayuS(1);
     SDA_L;
     mDelayuS(5);
     SCL_H;
     mDelayuS(5);
     SCL_L;

     return I2Cdata;
}
unsigned char I2CReadNAK()
{
     unsigned char mask;
     unsigned char I2Cdata;

     SDA_H;
     SDA_in;

     for(mask = 0x80;mask != 0; mask >>= 1)
     {
          mDelayuS(5);
          SCL_H;
          if(SDA_read == 0)
              I2Cdata &= ~mask;
          else
              I2Cdata |= mask;
          mDelayuS(5);
          SCL_L;
     }
     SDA_out;
     mDelayuS(1);
     SDA_H;
     mDelayuS(5);
     SCL_H;
     mDelayuS(5);
     SCL_L;

     return I2Cdata;
}
