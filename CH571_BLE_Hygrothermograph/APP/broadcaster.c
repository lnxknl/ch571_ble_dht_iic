/********************************** (C) COPYRIGHT *******************************
 * File Name          : broadcaster.c
 * Author             : 风一样的流逝
 * Version            : V1.0
 * Date               : 2022/12/15
 * Description        : 采集温湿度通过BLE广播出去，休眠唤醒后采集温湿度、电池电压数据后广播出去

 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "CONFIG.h"
#include "devinfoservice.h"
#include "broadcaster.h"
#include "SHT4X.h"
unsigned char alarmFlag = 0;//警报标志位
unsigned char normalFlag = 1;//警报标志位
//广播间隔设置    单位1ms
#define DEFAULT_ADVERTISING_INTERVAL    5000// @NOTE 
#define ALARM_ADVERTISING_INTERVAL    100

//读取温湿度、电池电压并开始广播    时间间隔设置    单位1ms
#define measurementAndAdvertisingInterval    5000
//广播时长
#define advertisingTime   500

// Company Identifier: WCH
#define WCH_COMPANY_ID                  0x07D7

// Length of bd addr as a string
#define B_ADDR_STR_LEN                  15

static uint8_t Broadcaster_TaskID; // Task ID for internal task/event processing

// 扫描应答包 31字节，不够会自行补0
static uint8_t scanRspData[] = {// @NOTE 
    // complete name
    0x0c,                                 // length of this data
    GAP_ADTYPE_LOCAL_NAME_COMPLETE, 0x42, // 'B'
    0x72,                                 // 'r'
    0x6f,                                 // 'o'
    0x61,                                 // 'a'
    0x64,                                 // 'd'
    0x63,                                 // 'c'
    0x61,                                 // 'a'
    0x73,                                 // 's'
    0x74,                                 // 't'
    0x65,                                 // 'e'
    0x72,                                 // 'r'

    // Tx power level
    0x02,                     // length of this data
    GAP_ADTYPE_POWER_LEVEL, 0 // 0dBm
};

//广播包固定长度31个字节，不够自动补0
static uint8_t advertData[] = {
    0x02, //AD Structure长度  2
    GAP_ADTYPE_FLAGS,//设备标识
    GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

    //广播数据
    0x0a,//AD Structure长度  10
    GAP_ADTYPE_LOCAL_NAME_SHORT,//数据类型 缩写设备名
    0xe6,0xb8,0xa9,0xe6,0xb9,0xbf,0xe8,0xae,0xa1,//温湿计  URL编码

    0x02, //AD Structure长度  2
    0x0a,//发射功率
    0,//0dbm

    0x06,//AD Structure长度  6
    GAP_ADTYPE_MANUFACTURER_SPECIFIC, //自定义数据 用来填充温湿度、电池电压数据
    0, 0, //温度 00.00℃
    0, 0, //湿度 00.00%
    0//电池电压  电压 = 读数/100
};

/* 函数声明 */
static void Broadcaster_ProcessTMOSMsg(tmos_event_hdr_t *pMsg);
static void Broadcaster_StateNotificationCB(gapRole_States_t newState);
uint8_t BatteryMeasuremen();

/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks
static gapRolesBroadcasterCBs_t Broadcaster_BroadcasterCBs = {Broadcaster_StateNotificationCB, // Profile State Change Callbacks
                                                              NULL};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      Broadcaster_Init
 *
 * @brief   Initialization function for the Broadcaster App
 *          Task. This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by TMOS.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void Broadcaster_Init()
{
    Broadcaster_TaskID = TMOS_ProcessEventRegister(Broadcaster_ProcessEvent);

    // Setup the GAP Broadcaster Role Profile
    {
        // Device starts advertising upon initialization
        uint8_t initial_advertising_enable = TRUE;
        uint8_t initial_adv_event_type = GAP_ADTYPE_ADV_NONCONN_IND;//不可连接非定向
        // Set the GAP Role Parameters
        GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &initial_advertising_enable);//启用广播
        GAPRole_SetParameter(GAPROLE_ADV_EVENT_TYPE, sizeof(uint8_t), &initial_adv_event_type);//设置广播类型   不可连接非定向
        //GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData), scanRspData);//设置扫描响应数据// @NOTE 
        GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);//设置广播数据
    }

    // Set advertising interval
    {
        uint16_t advInt = MS1_TO_SYSTEM_TIME( DEFAULT_ADVERTISING_INTERVAL );

        GAP_SetParamValue(TGAP_DISC_ADV_INT_MIN, advInt);
        GAP_SetParamValue(TGAP_DISC_ADV_INT_MAX, advInt);
    }

    // Setup a delayed profile startup
    tmos_set_event(Broadcaster_TaskID, SBP_START_DEVICE_EVT);
}

/*********************************************************************
 * @fn      Broadcaster_ProcessEvent
 *
 * @brief   Broadcaster Application Task event processor. This
 *          function is called to process all events for the task. Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The TMOS assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
uint16_t Broadcaster_ProcessEvent(uint8_t task_id, uint16_t events)
{
    if(events & SYS_EVENT_MSG)
    {
        uint8_t *pMsg;

        if((pMsg = tmos_msg_receive(Broadcaster_TaskID)) != NULL)
        {
            Broadcaster_ProcessTMOSMsg((tmos_event_hdr_t *)pMsg);

            // Release the TMOS message
            tmos_msg_deallocate(pMsg);
        }

        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }

    if(events & SBP_START_DEVICE_EVT)
    {
        // Start the Device
        GAPRole_BroadcasterStartDevice(&Broadcaster_BroadcasterCBs);
        tmos_start_task(Broadcaster_TaskID, SBP_Broadcaster_ON_EVT, MS1_TO_SYSTEM_TIME( 1000 ));
        tmos_set_event(Broadcaster_TaskID, SBP_Broadcaster_Measuremen_EVT);//执行测量任务事件

        return (events ^ SBP_START_DEVICE_EVT);
    }
    if(events & SBP_Broadcaster_ON_EVT)
    {
        uint8_t initial_advertising_enable = TRUE;
        uint8_t initial_adv_event_type = GAP_ADTYPE_ADV_NONCONN_IND;//不可连接非定向

        GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &initial_advertising_enable);//启用广播
        GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);//设置广播数据
        //开启广播
        GAPRole_BroadcasterStartDevice(&Broadcaster_BroadcasterCBs);//开启广播

        return (events ^ SBP_Broadcaster_ON_EVT);
    }
    if(events & SBP_Broadcaster_OFF_EVT)
    {
        //关闭广播
        uint8 advertisingEnable = FALSE;
        GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &advertisingEnable );
        //tmos_start_task(Broadcaster_TaskID, SBP_Broadcaster_ON_EVT, MS1_TO_SYSTEM_TIME( measurementAndAdvertisingInterval ));//执行测量数据并广播事件
#if PrintfEN
        PRINT("Broadcaster off.\r\n");
#endif
        return (events ^ SBP_Broadcaster_OFF_EVT);
    }

    if(events & SBP_Broadcaster_Measuremen_EVT)
    {
        unsigned char rc;
        static signed short temperatureBuf,humidityBuf;

        //测量温湿度
        rc = SHT4XReadTemperatureAndHumidity(&temperatureBuf, &humidityBuf);// @NOTE 
         if(rc == 1)
         {
             advertData[19] = temperatureBuf >> 8;//温度高8位// @NOTE 
             advertData[20] = (uint8_t)temperatureBuf;//温度低8位
             advertData[21] = humidityBuf >> 8;//湿度高8位
             advertData[22] = (uint8_t)humidityBuf;//湿度低8位

             //温度超60℃ 时改为100毫秒广播一次
             if (temperatureBuf > 6000)
             {
                 //未开始警报时修改为100毫秒广播一次
                 if (alarmFlag == 0)
                 {
                     alarmFlag = 1;
                 }
             }
             //湿度大于95%时改为100毫秒广播一次
             else if (humidityBuf > 9500)
             {
                  //未开始警报时修改为100毫秒广播一次
                  if (alarmFlag == 0)
                  {
                      alarmFlag = 1;
                  }
             }
             else
             {
                 if(alarmFlag == 1) alarmFlag = 0;//解除警报 广播间隔改回5秒一次
             }
         }
         else {
             advertData[19]=0;//温度高8位
             advertData[20]=0;//温度低8位
             advertData[21]=0;//湿度高8位
             advertData[22]=0;//湿度低8位
         }

         //测量电池电压
         advertData[23] = BatteryMeasuremen();
         GAP_UpdateAdvertisingData( 0,TRUE ,sizeof( advertData ),advertData );// @NOTE , wirte send data to stack and update advertising data

         if(alarmFlag == 0)//未开启警报5秒测量一次温湿度
         {
             tmos_start_task(Broadcaster_TaskID, SBP_Broadcaster_Measuremen_EVT, MS1_TO_SYSTEM_TIME(5000));//执行测量任务事件
             if (normalFlag == 0) {
                 //从非正常状态下转回正常时 关闭广播后在回调函数里修改广播间隔为5秒后再开启广播
                 tmos_start_task(Broadcaster_TaskID, SBP_Broadcaster_OFF_EVT, MS1_TO_SYSTEM_TIME(10));//
            }
         }
         else//开启警报200毫秒秒测量一次温湿度
         {
             tmos_start_task(Broadcaster_TaskID, SBP_Broadcaster_Measuremen_EVT, MS1_TO_SYSTEM_TIME(200));//执行测量任务事件
             if (normalFlag == 1) {
                 //关闭广播后在回调函数里修改广播间隔为100毫秒后   再次开启广播
                 tmos_start_task(Broadcaster_TaskID, SBP_Broadcaster_OFF_EVT, MS1_TO_SYSTEM_TIME(10));
            }
         }

         return (events ^ SBP_Broadcaster_Measuremen_EVT);
    }
    // Discard unknown events
    return 0;
}

/*********************************************************************
 * @fn      Broadcaster_ProcessTMOSMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void Broadcaster_ProcessTMOSMsg(tmos_event_hdr_t *pMsg)
{
    switch(pMsg->event)
    {
        default:
            break;
    }
}

/*********************************************************************
 * @fn      Broadcaster_StateNotificationCB
 *
 * @brief   Notification from the profile of a state change.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void Broadcaster_StateNotificationCB(gapRole_States_t newState)
{
    switch(newState)
    {
        case GAPROLE_STARTED:
#if PrintfEN
            PRINT("Initialized..\n");
#endif
            break;

        case GAPROLE_ADVERTISING:
#if PrintfEN
            PRINT("Advertising..\n");
#endif
            break;

        case GAPROLE_WAITING:
            if(alarmFlag == 1)
            {
                int8_t initial_advertising_enable = TRUE;
                uint8_t initial_adv_event_type = GAP_ADTYPE_ADV_NONCONN_IND;//不可连接非定向
                uint16_t advInt = MS1_TO_SYSTEM_TIME( ALARM_ADVERTISING_INTERVAL );

                GAP_SetParamValue(TGAP_DISC_ADV_INT_MIN, advInt);
                GAP_SetParamValue(TGAP_DISC_ADV_INT_MAX, advInt);
                GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &initial_advertising_enable);//启用广播
                GAPRole_BroadcasterStartDevice(&Broadcaster_BroadcasterCBs);//开启广播
                normalFlag = 0;
            }
            else {
                if (normalFlag == 0) {
                    normalFlag = 1;
                    int8_t initial_advertising_enable = TRUE;
                    uint8_t initial_adv_event_type = GAP_ADTYPE_ADV_NONCONN_IND;//不可连接非定向
                    uint16_t advInt = MS1_TO_SYSTEM_TIME( DEFAULT_ADVERTISING_INTERVAL );

                    GAP_SetParamValue(TGAP_DISC_ADV_INT_MIN, advInt);
                    GAP_SetParamValue(TGAP_DISC_ADV_INT_MAX, advInt);
                    GAP_SetParamValue(TGAP_DISC_ADV_INT_MIN, advInt);
                    GAP_SetParamValue(TGAP_DISC_ADV_INT_MAX, advInt);
                    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &initial_advertising_enable);//启用广播
                    GAPRole_BroadcasterStartDevice(&Broadcaster_BroadcasterCBs);//开启广播
                }
            }
#if PrintfEN
            PRINT("Waiting for advertising..\n");
#endif
            break;

        case GAPROLE_ERROR:
#if PrintfEN
            PRINT("Error..\n");
#endif
            break;

        default:
            break;
    }
}
/* 测量电池电压 */
uint8_t BatteryMeasuremen()
{
    float batVol;
    uint16_t batBuf;
    uint16_t RoughCalib_Value = 0; // ADC粗调偏差值;

    GPIOA_ModeCfg(GPIO_Pin_14, GPIO_ModeOut_PP_5mA);//PA14电池电压输入控制引脚
    GPIOA_ModeCfg(GPIO_Pin_13, GPIO_ModeIN_Floating);//PA13  ADC引脚

    //打开电池电压输入
    GPIOA_SetBits(GPIO_Pin_14);
    DelayMs(1);

    ADC_ExtSingleChSampInit(SampleFreq_3_2, ADC_PGA_0);//3.2M ADC时钟    3.2M/18采样频率     ADC开启1倍增益

    RoughCalib_Value = ADC_DataCalib_Rough(); // 用于计算ADC内部偏差，记录到全局变量 RoughCalib_Value中
    ADC_ChannelCfg(3);//ADC通道3

    //ADC采样  通道3
    batBuf = ADC_ExcutSingleConver() + RoughCalib_Value;

    //关闭电池电压输入并且下拉 PA13  PA14
    GPIOA_ResetBits(GPIO_Pin_14);
    GPIOA_ModeCfg(GPIO_Pin_13 | GPIO_Pin_14, GPIO_ModeIN_PD);

    //关闭ADC
    R8_TKEY_CFG &= 0xfe;//关闭TouchKey模块电源
    R8_ADC_CFG &= 0xfc; //关闭ADC模块电源 关闭ADC缓存

    //计算电池电压   使用低功耗分析仪+7为补偿值，用5号碱性电池发现不需要补偿
    batVol = batBuf * 1.05;
    batVol = batVol / 2048;
    //batBuf = batVol * 200;//ADC测量10K分压电阻处，*200补正，电压 = batBuf/100
    batBuf = batVol * 100;//电压 = batBuf/100


    return (uint8_t)batBuf;
}
