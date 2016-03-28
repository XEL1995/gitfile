/*
 * electric_measure.c
 *
 *  Created on: 2016��3��21��
 *      Author: wangshi
 */
#include <stdlib.h>
#include <string.h>
#include <wmstdio.h>
#include <wm_os.h>
#include <wmerrno.h>
#include <wm_utils.h>
#include <ctype.h>
#include <wmlist.h>
#include <psm-v2.h>
#include <mdev_gpt.h>
#include <mdev_gpio.h>

#include "../header/electric_measure.h"

//static os_thread_t PulseThread;
//static os_thread_stack_define(Pulse_Thread_Stack, 2048);

//int createPulseThread(void){

//	CH_P("createPulseThread...\r\n");

//	if (WM_SUCCESS != os_thread_create(&PulseThread,/* thread handle */"UartThread",	/* thread name */
//			Pulse_Thread,	/* entry function */0,	/* argument */&Pulse_Thread_Stack,/* stack */OS_PRIO_3/* priority - medium low */)) {
//		CH_P("Can not create UartThread.\r\n");
//		return -1;
//	}
//	return WM_SUCCESS;
//}

static u8 data_exist_flashflag;
//static u8 data_exist_flag;

mdev_t *gpio_dev;
u8 gpio_P;
u8 gpio_IV;
u8 gpio_OUT;

extern u8 U8_CURR_WorkMode;

extern u1    	B_VI_Test_Mode;				//1:��ѹ����ģʽ;0:��������ģʽ
extern u16   	U16_VI_Test_Times;
extern u32     U32_Cal_Times;                 		//У��ʱ��

extern u8 Measure_P_Flag;
extern u8 Measure_V_Flag;
extern u8 Measure_I_Flag;
extern u8 Measure_COS_Flag;

extern u16   	U16_AC_P;				//����ֵ 1000.0W
extern u16   	U16_AC_V;				//��ѹֵ 220.0V
extern u16   	U16_AC_I;				//����ֵ 4.545A
extern u32   	U32_AC_E;				//�õ���   0.01��
extern u32   	U32_AC_BACKUP_E;		//�õ�������
extern u32     U32_AC_SAVE_E;          //�洢���õ���
extern u8    	U8_AC_COS;				//�������� 0.00

extern u16   	U16_REF_001_E_Pluse_CNT;        	//0.01�ȵ����������ο�ֵ

extern u32  U32_SaveTimes;              //��ʱ�洢��ʱ
extern u16	U16_P_TotalTimes;			//��ǰ���� ���ʲ�����ʱ��
extern u16	U16_V_TotalTimes;			//��ǰ���� ��ѹ������ʱ��
extern u16	U16_I_TotalTimes;			//��ǰ���� ����������ʱ��

extern u16	U16_P_OneCycleTime;			//���ʲ���ʱ�����
extern u16	U16_V_OneCycleTime;			//��ѹ����ʱ�����
extern u16	U16_I_OneCycleTime;			//��������ʱ�����

extern u16	U16_P_CNT;				//���ʲ�����������
extern u16	U16_V_CNT;				//��ѹ������������
extern u16	U16_I_CNT;				//����������������

extern u32   	U32_P_REF_PLUSEWIDTH_TIME;      	//�ο����� ��������
extern u32   	U32_V_REF_PLUSEWIDTH_TIME;      	//�ο���ѹ ��������
extern u32   	U32_I_REF_PLUSEWIDTH_TIME;      	//�ο����� ��������

extern u16   	U16_P_REF_Data;				//�ο�����ֵ,����1000WУ����1000.0W
extern u16   	U16_V_REF_Data;				//�ο���ѹ  220.0V
extern u16   	U16_I_REF_Data;				//�ο�����  1000W,220V��������4.545A

void InitGPIO(void)
{

	gpio_drv_init();
	gpio_dev = gpio_drv_open("GPIO_MDEV");

	gpio_drv_setdir(gpio_dev,gpio_P,GPIO_INPUT);

	gpio_drv_setdir(gpio_dev,gpio_IV,GPIO_INPUT);

	gpio_drv_setdir(gpio_dev,gpio_OUT,GPIO_OUTPUT);

	gpio_drv_write(gpio_dev,gpio_IV,GPIO_IO_LOW);

}

/*int SetFlashFlag(void)
{
	mdev_t * dev;  //flash�豸
	wmprintf("In audio_write the address is : %08x the len is: %d\r\n", address, len);

	flash_drv_init();

	dev = flash_drv_open(0);
	if (dev == NULL)
	{
		wmprintf("Flash driver init is required before open");
		return -WM_FAIL;
	}

	if (flash_drv_erase(dev, address, len) < 0) {
		flash_drv_close(dev);
		wmprintf("Failed to erase partition\r\n");
		return -WM_FAIL;
	}

	if (flash_drv_write(dev, data, len, address) != 0)
	{
		wmprintf("Failed to write update data\r\n");
		return -WM_FAIL;
	}
	flash_drv_close(dev);
	return WM_SUCCESS;
}*/

int Load_CalData_Flash(void)
{
	mdev_t * dev;

    //����Ĭ��ֵ
    U16_P_REF_Data = 10000;		// 1000.0W
    U16_V_REF_Data = 2200;		// 220.0V
    U16_I_REF_Data = 4545;		// 4.545A

    //Ĭ������
    U32_P_REF_PLUSEWIDTH_TIME = 4975;     // 4975us
    U32_V_REF_PLUSEWIDTH_TIME = 1666;      // 1666us
    U32_I_REF_PLUSEWIDTH_TIME = 1666;      // 1666us

    //��������ʼֵ
    U32_AC_BACKUP_E = 0;
    U32_AC_E = 0;

	flash_drv_init();
	dev = flash_drv_open(0);
	if (dev == NULL) {
		wmprintf("Flash driver init is required before open");
		return -WM_FAIL;
	}

	int rv;
	rv = flash_drv_read(dev, &data_exist_flashflag, 1, DATA_FLAG_ADDR);
	if (rv != WM_SUCCESS) {
		wmprintf("Unable to read from flash: %x\r\n", DATA_FLAG_ADDR);
		return rv;
	}
	wmprintf("read from flash: %x\r\n", DATA_FLAG_ADDR);

    if (data_exist_flashflag != 0x55)	//�ж�EEPROM���Ƿ����У��ֵ,��û��,�򽫲�������ΪĬ��ֵ
    {
      //data_exist_flag = 1;          //�����Ƿ�д���flash,��д���������flash��־λ

      data_exist_flashflag = 0x55;
      U32_AC_SAVE_E = 0;
    }
    else
    {
    	rv = flash_drv_read(dev, (u8*)(&U32_AC_SAVE_E), 4, E_SAVE_ADDR);
    	if (rv != WM_SUCCESS) {
    		wmprintf("Unable to read from flash: %x\r\n", E_SAVE_ADDR);
    		return rv;
    	}

    	U8_CURR_WorkMode = D_CAL_END_MODE;
    	rv = flash_drv_read(dev, (u8*)(&U16_REF_001_E_Pluse_CNT), 2, E_001_Pluse_CNT_ADDR);
    	if (rv != WM_SUCCESS) {
    		wmprintf("Unable to read from flash: %x\r\n", E_SAVE_ADDR);
    		return rv;
    	}
    }
    flash_drv_close(dev);
	return  WM_SUCCESS;
}

/*
int Save_CalData_Flash(u8 *data,u32 len,u32 address)
{
	mdev_t * dev;  //flash�豸

	flash_drv_init();

	dev = flash_drv_open(0);
	if (dev == NULL)
	{
		wmprintf("Flash driver init is required before open");
		return -WM_FAIL;
	}

	if (flash_drv_erase(dev, DATA_ADDR, PAGESIZE) < 0) {
		flash_drv_close(dev);
		wmprintf("Failed to erase partition\r\n");
		return -WM_FAIL;
	}

	if (flash_drv_write(dev, data, len, address) != 0)
	{
		wmprintf("Failed to write update data\r\n");
		return -WM_FAIL;
	}
	wmprintf("The write address is : %08x\r\n", address);

	flash_drv_close(dev);
	return WM_SUCCESS;
}*/

int Save_AllData_Flash(void)
{
	mdev_t * dev;  //flash�豸

	flash_drv_init();

	dev = flash_drv_open(0);
	if (dev == NULL)
	{
		wmprintf("Flash driver init is required before open");
		return -WM_FAIL;
	}

	if (flash_drv_erase(dev, DATA_ADDR, PAGESIZE) < 0) {
		flash_drv_close(dev);
		wmprintf("Failed to erase partition\r\n");
		return -WM_FAIL;
	}

	if (flash_drv_write(dev, &data_exist_flashflag, 1, DATA_FLAG_ADDR) != 0)
	{
		wmprintf("Failed to write update data\r\n");
		return -WM_FAIL;
	}
	wmprintf("The write address is : %08x\r\n", DATA_FLAG_ADDR);

	U32_AC_SAVE_E += U32_AC_BACKUP_E;
	U32_AC_BACKUP_E = 0;
	U32_AC_E = 0;
	if (flash_drv_write(dev, (u8*)(&U32_AC_SAVE_E), 4, E_SAVE_ADDR) != 0)
	{
		wmprintf("Failed to write update data\r\n");
		return -WM_FAIL;
	}

	if (flash_drv_write(dev, (u8*)(&U16_AC_P), 2, P_SAVE_ADDR) != 0)
	{
		wmprintf("Failed to write update data\r\n");
		return -WM_FAIL;
	}

	if (flash_drv_write(dev, (u8*)(&U16_AC_V), 2, V_SAVE_ADDR) != 0)
	{
		wmprintf("Failed to write update data\r\n");
		return -WM_FAIL;
	}

	if (flash_drv_write(dev, (u8*)(&U16_AC_I), 2, I_SAVE_ADDR) != 0)
	{
		wmprintf("Failed to write update data\r\n");
		return -WM_FAIL;
	}

	if (flash_drv_write(dev, (u8*)(&U16_REF_001_E_Pluse_CNT), 2, E_001_Pluse_CNT_ADDR) != 0)
	{
		wmprintf("Failed to write update data\r\n");
		return -WM_FAIL;
	}
	flash_drv_close(dev);
	return WM_SUCCESS;
}

mdev_t* gpt_dev;

void Timing_cb(void)
{
    U32_Cal_Times++;//�õ�����ʱ����У��ʱ��36S��1000W����36Sʱ������0.01�ȵ�
    //���ʼ�ʱ��
    if (U16_P_CNT != 0)
    {
        U16_P_OneCycleTime++;  //���β���ѭ������
        U16_P_TotalTimes++;    //��ѭ������,�����������12s
    }
    //��ѹ��ʱ��
    if(B_VI_Test_Mode == 1)
    {
		if (U16_V_CNT != 0)
		{
			U16_V_OneCycleTime++;
			U16_V_TotalTimes++;
		}
    }
    //������ʱ��
    else
    {
		if (U16_I_CNT != 0)
		{
			U16_I_OneCycleTime++;
			U16_I_TotalTimes++;
		}
    }
    //��ѹ����ģʽ�л�
    U16_VI_Test_Times--;
    //��ʱ�洢��ʱ��
    //U32_SaveTimes++;
}

void MsTimer(void)
{
	gpt_drv_init(0);

	gpt_dev = gpt_drv_open(0);
	if(gpt_dev == NULL)
	{
		wmprintf("open GPT device error\r\n");
	}
	gpt_drv_set(gpt_dev,D_TIME1_1000US);
	gpt_drv_setcb(gpt_dev,Timing_cb);
	gpt_drv_start(gpt_dev);

}

int main(int argc,char* argv[])
//void Pulse_Thread(os_thread_arg_t data)
{
	Init_Rom();
	Load_CalData_Flash();
	InitGPIO();
	MsTimer();

	while(1)
    {
    	if(U8_CURR_WorkMode == D_CAL_END_MODE)
    	{
    		//u8 *E_PLUSE_CNT = (u8*)(&U16_REF_001_E_Pluse_CNT);//0.01�ȵ�ο�����

    		U8_CURR_WorkMode = D_NORMAL_MODE;  //�������ģʽ
    	}

		TIM1_UPD_Interrupt();

		//���������źţ������ж�,�⹦�ʼƴ�
		gpio_drv_set_cb(gpio_dev,gpio_P,
				        GPIO_INT_FALLING_EDGE,
				        NULL,PC5_EXTI_Interrupt);

		//���ѹ�����ƴ�
		gpio_drv_set_cb(gpio_dev,gpio_IV,
				        GPIO_INT_FALLING_EDGE,
				        NULL,PD2_EXTI_Interrupt);

		//HLW8012_Measure()
		//�ﵽ��ʱʱ����ߵ����ڼ�ʱ����
		if(Measure_P_Flag)
		{
			//u8 *P_PLUSE_TIME;  //һ���ڵ�΢����

			HLW8012_Measure_P();  //���в���

			//P_PLUSE_TIME = (u8*)(&U32_P_CURRENT_PLUSEWIDTH_TIME);
			//Save_CalData_Flash(P_PLUSE_TIME,4,E_SAVE_ADDR);
			Measure_P_Flag = 0;
			Measure_COS_Flag = 1;
		}
		if(Measure_I_Flag)
		{
			//u8 *I_PLUSE_TIME = (u8*)(&U32_I_CURRENT_PLUSEWIDTH_TIME);

			HLW8012_Measure_I();
			//Save_CalData_Flash(I_PLUSE_TIME,4,I_SAVE_ADDR);
			Measure_I_Flag = 0;
			Measure_COS_Flag = 1;
		}
		if(Measure_V_Flag)
		{
			//u8 *V_PLUSE_TIME = (u8*)(&U32_V_CURRENT_PLUSEWIDTH_TIME);

			HLW8012_Measure_V();
			//Save_CalData_Flash(V_PLUSE_TIME,4,V_SAVE_ADDR);
			Measure_V_Flag = 0;
			Measure_COS_Flag = 1;
		}

		HLW8012_Measure_E();

		if(Measure_COS_Flag == 1)  //ÿ����һ��P/V/I������һ��cos
		{
			HLW8012_Measure_COS();
			Measure_COS_Flag = 0;
		}

		/*ÿ15min��ʱ�洢
		if(U32_SaveTimes%900000 == 0)
		{
		    U32_SaveTimes = 0;
			Save_AllData_Flash();
		}
		*/

		//os_thread_sleep(os_msec_to_ticks(1));
    }
	gpio_drv_close(gpio_dev);
}

