/********************************** (C) COPYRIGHT *******************************
 * File Name          : broadcaster.c
 * Author             : ��һ��������
 * Version            : V1.0
 * Date               : 2022/12/15
 * Description        : �ɼ���ʪ��ͨ��BLE�㲥��ȥ�����߻��Ѻ�ɼ���ʪ�ȡ���ص�ѹ���ݺ�㲥��ȥ

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
unsigned char alarmFlag = 0;//������־λ
unsigned char normalFlag = 1;//������־λ
//�㲥�������    ��λ1ms
#define DEFAULT_ADVERTISING_INTERVAL    5000// @NOTE 
#define ALARM_ADVERTISING_INTERVAL    100

//��ȡ��ʪ�ȡ���ص�ѹ����ʼ�㲥    ʱ��������    ��λ1ms
#define measurementAndAdvertisingInterval    5000
//�㲥ʱ��
#define advertisingTime   500

// Company Identifier: WCH
#define WCH_COMPANY_ID                  0x07D7

// Length of bd addr as a string
#define B_ADDR_STR_LEN                  15

static uint8_t Broadcaster_TaskID; // Task ID for internal task/event processing

// ɨ��Ӧ��� 31�ֽڣ����������в�0
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

//�㲥���̶�����31���ֽڣ������Զ���0
static uint8_t advertData[] = {
    0x02, //AD Structure����  2
    GAP_ADTYPE_FLAGS,//�豸��ʶ
    GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

    //�㲥����
    0x0a,//AD Structure����  10
    GAP_ADTYPE_LOCAL_NAME_SHORT,//�������� ��д�豸��
    0xe6,0xb8,0xa9,0xe6,0xb9,0xbf,0xe8,0xae,0xa1,//��ʪ��  URL����

    0x02, //AD Structure����  2
    0x0a,//���书��
    0,//0dbm

    0x06,//AD Structure����  6
    GAP_ADTYPE_MANUFACTURER_SPECIFIC, //�Զ������� ���������ʪ�ȡ���ص�ѹ����
    0, 0, //�¶� 00.00��
    0, 0, //ʪ�� 00.00%
    0//��ص�ѹ  ��ѹ = ����/100
};

/* �������� */
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
        uint8_t initial_adv_event_type = GAP_ADTYPE_ADV_NONCONN_IND;//�������ӷǶ���
        // Set the GAP Role Parameters
        GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &initial_advertising_enable);//���ù㲥
        GAPRole_SetParameter(GAPROLE_ADV_EVENT_TYPE, sizeof(uint8_t), &initial_adv_event_type);//���ù㲥����   �������ӷǶ���
        //GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData), scanRspData);//����ɨ����Ӧ����// @NOTE 
        GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);//���ù㲥����
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
        tmos_set_event(Broadcaster_TaskID, SBP_Broadcaster_Measuremen_EVT);//ִ�в��������¼�

        return (events ^ SBP_START_DEVICE_EVT);
    }
    if(events & SBP_Broadcaster_ON_EVT)
    {
        uint8_t initial_advertising_enable = TRUE;
        uint8_t initial_adv_event_type = GAP_ADTYPE_ADV_NONCONN_IND;//�������ӷǶ���

        GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &initial_advertising_enable);//���ù㲥
        GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);//���ù㲥����
        //�����㲥
        GAPRole_BroadcasterStartDevice(&Broadcaster_BroadcasterCBs);//�����㲥

        return (events ^ SBP_Broadcaster_ON_EVT);
    }
    if(events & SBP_Broadcaster_OFF_EVT)
    {
        //�رչ㲥
        uint8 advertisingEnable = FALSE;
        GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &advertisingEnable );
        //tmos_start_task(Broadcaster_TaskID, SBP_Broadcaster_ON_EVT, MS1_TO_SYSTEM_TIME( measurementAndAdvertisingInterval ));//ִ�в������ݲ��㲥�¼�
#if PrintfEN
        PRINT("Broadcaster off.\r\n");
#endif
        return (events ^ SBP_Broadcaster_OFF_EVT);
    }

    if(events & SBP_Broadcaster_Measuremen_EVT)
    {
        unsigned char rc;
        static signed short temperatureBuf,humidityBuf;

        //������ʪ��
        rc = SHT4XReadTemperatureAndHumidity(&temperatureBuf, &humidityBuf);// @NOTE 
         if(rc == 1)
         {
             advertData[19] = temperatureBuf >> 8;//�¶ȸ�8λ// @NOTE 
             advertData[20] = (uint8_t)temperatureBuf;//�¶ȵ�8λ
             advertData[21] = humidityBuf >> 8;//ʪ�ȸ�8λ
             advertData[22] = (uint8_t)humidityBuf;//ʪ�ȵ�8λ

             //�¶ȳ�60�� ʱ��Ϊ100����㲥һ��
             if (temperatureBuf > 6000)
             {
                 //δ��ʼ����ʱ�޸�Ϊ100����㲥һ��
                 if (alarmFlag == 0)
                 {
                     alarmFlag = 1;
                 }
             }
             //ʪ�ȴ���95%ʱ��Ϊ100����㲥һ��
             else if (humidityBuf > 9500)
             {
                  //δ��ʼ����ʱ�޸�Ϊ100����㲥һ��
                  if (alarmFlag == 0)
                  {
                      alarmFlag = 1;
                  }
             }
             else
             {
                 if(alarmFlag == 1) alarmFlag = 0;//������� �㲥����Ļ�5��һ��
             }
         }
         else {
             advertData[19]=0;//�¶ȸ�8λ
             advertData[20]=0;//�¶ȵ�8λ
             advertData[21]=0;//ʪ�ȸ�8λ
             advertData[22]=0;//ʪ�ȵ�8λ
         }

         //������ص�ѹ
         advertData[23] = BatteryMeasuremen();
         GAP_UpdateAdvertisingData( 0,TRUE ,sizeof( advertData ),advertData );// @NOTE , wirte send data to stack and update advertising data

         if(alarmFlag == 0)//δ��������5�����һ����ʪ��
         {
             tmos_start_task(Broadcaster_TaskID, SBP_Broadcaster_Measuremen_EVT, MS1_TO_SYSTEM_TIME(5000));//ִ�в��������¼�
             if (normalFlag == 0) {
                 //�ӷ�����״̬��ת������ʱ �رչ㲥���ڻص��������޸Ĺ㲥���Ϊ5����ٿ����㲥
                 tmos_start_task(Broadcaster_TaskID, SBP_Broadcaster_OFF_EVT, MS1_TO_SYSTEM_TIME(10));//
            }
         }
         else//��������200���������һ����ʪ��
         {
             tmos_start_task(Broadcaster_TaskID, SBP_Broadcaster_Measuremen_EVT, MS1_TO_SYSTEM_TIME(200));//ִ�в��������¼�
             if (normalFlag == 1) {
                 //�رչ㲥���ڻص��������޸Ĺ㲥���Ϊ100�����   �ٴο����㲥
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
                uint8_t initial_adv_event_type = GAP_ADTYPE_ADV_NONCONN_IND;//�������ӷǶ���
                uint16_t advInt = MS1_TO_SYSTEM_TIME( ALARM_ADVERTISING_INTERVAL );

                GAP_SetParamValue(TGAP_DISC_ADV_INT_MIN, advInt);
                GAP_SetParamValue(TGAP_DISC_ADV_INT_MAX, advInt);
                GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &initial_advertising_enable);//���ù㲥
                GAPRole_BroadcasterStartDevice(&Broadcaster_BroadcasterCBs);//�����㲥
                normalFlag = 0;
            }
            else {
                if (normalFlag == 0) {
                    normalFlag = 1;
                    int8_t initial_advertising_enable = TRUE;
                    uint8_t initial_adv_event_type = GAP_ADTYPE_ADV_NONCONN_IND;//�������ӷǶ���
                    uint16_t advInt = MS1_TO_SYSTEM_TIME( DEFAULT_ADVERTISING_INTERVAL );

                    GAP_SetParamValue(TGAP_DISC_ADV_INT_MIN, advInt);
                    GAP_SetParamValue(TGAP_DISC_ADV_INT_MAX, advInt);
                    GAP_SetParamValue(TGAP_DISC_ADV_INT_MIN, advInt);
                    GAP_SetParamValue(TGAP_DISC_ADV_INT_MAX, advInt);
                    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &initial_advertising_enable);//���ù㲥
                    GAPRole_BroadcasterStartDevice(&Broadcaster_BroadcasterCBs);//�����㲥
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
/* ������ص�ѹ */
uint8_t BatteryMeasuremen()
{
    float batVol;
    uint16_t batBuf;
    uint16_t RoughCalib_Value = 0; // ADC�ֵ�ƫ��ֵ;

    GPIOA_ModeCfg(GPIO_Pin_14, GPIO_ModeOut_PP_5mA);//PA14��ص�ѹ�����������
    GPIOA_ModeCfg(GPIO_Pin_13, GPIO_ModeIN_Floating);//PA13  ADC����

    //�򿪵�ص�ѹ����
    GPIOA_SetBits(GPIO_Pin_14);
    DelayMs(1);

    ADC_ExtSingleChSampInit(SampleFreq_3_2, ADC_PGA_0);//3.2M ADCʱ��    3.2M/18����Ƶ��     ADC����1������

    RoughCalib_Value = ADC_DataCalib_Rough(); // ���ڼ���ADC�ڲ�ƫ���¼��ȫ�ֱ��� RoughCalib_Value��
    ADC_ChannelCfg(3);//ADCͨ��3

    //ADC����  ͨ��3
    batBuf = ADC_ExcutSingleConver() + RoughCalib_Value;

    //�رյ�ص�ѹ���벢������ PA13  PA14
    GPIOA_ResetBits(GPIO_Pin_14);
    GPIOA_ModeCfg(GPIO_Pin_13 | GPIO_Pin_14, GPIO_ModeIN_PD);

    //�ر�ADC
    R8_TKEY_CFG &= 0xfe;//�ر�TouchKeyģ���Դ
    R8_ADC_CFG &= 0xfc; //�ر�ADCģ���Դ �ر�ADC����

    //�����ص�ѹ   ʹ�õ͹��ķ�����+7Ϊ����ֵ����5�ż��Ե�ط��ֲ���Ҫ����
    batVol = batBuf * 1.05;
    batVol = batVol / 2048;
    //batBuf = batVol * 200;//ADC����10K��ѹ���账��*200��������ѹ = batBuf/100
    batBuf = batVol * 100;//��ѹ = batBuf/100


    return (uint8_t)batBuf;
}
