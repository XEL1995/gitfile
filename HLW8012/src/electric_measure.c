/*
 * electric_measure.c
 *
 *  Created on: 2016年3月21日
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

extern u1    	B_VI_Test_Mode;				//1:电压测量模式;0:电流测量模式
extern u16   	U16_VI_Test_Times;
extern u32     U32_Cal_Times;                 		//校正时间

extern u8 Measure_P_Flag;
extern u8 Measure_V_Flag;
extern u8 Measure_I_Flag;
extern u8 Measure_COS_Flag;

extern u16   	U16_AC_P;				//功率值 1000.0W
extern u16   	U16_AC_V;				//电压值 220.0V
extern u16   	U16_AC_I;				//电流值 4.545A
extern u32   	U32_AC_E;				//用电量   0.01度
extern u32   	U32_AC_BACKUP_E;		//用电量备份
extern u32     U32_AC_SAVE_E;          //存储的用电量
extern u8    	U8_AC_COS;				//功率因素 0.00

extern u16   	U16_REF_001_E_Pluse_CNT;        	//0.01度电脉冲总数参考值

extern u32  U32_SaveTimes;              //定时存储计时
extern u16	U16_P_TotalTimes;			//当前脉冲 功率测量总时间
extern u16	U16_V_TotalTimes;			//当前脉冲 电压测量总时间
extern u16	U16_I_TotalTimes;			//当前脉冲 电流测量总时间

extern u16	U16_P_OneCycleTime;			//功率测量时间参数
extern u16	U16_V_OneCycleTime;			//电压测量时间参数
extern u16	U16_I_OneCycleTime;			//电流测量时间参数

extern u16	U16_P_CNT;				//功率测量脉冲数量
extern u16	U16_V_CNT;				//电压测量脉冲数量
extern u16	U16_I_CNT;				//电流测量脉冲数量

extern u32   	U32_P_REF_PLUSEWIDTH_TIME;      	//参考功率 脉冲周期
extern u32   	U32_V_REF_PLUSEWIDTH_TIME;      	//参考电压 脉冲周期
extern u32   	U32_I_REF_PLUSEWIDTH_TIME;      	//参考电流 脉冲周期

extern u16   	U16_P_REF_Data;				//参考功率值,如以1000W校正。1000.0W
extern u16   	U16_V_REF_Data;				//参考电压  220.0V
extern u16   	U16_I_REF_Data;				//参考电流  1000W,220V条件下是4.545A

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
	mdev_t * dev;  //flash设备
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

    //设置默认值
    U16_P_REF_Data = 10000;		// 1000.0W
    U16_V_REF_Data = 2200;		// 220.0V
    U16_I_REF_Data = 4545;		// 4.545A

    //默认周期
    U32_P_REF_PLUSEWIDTH_TIME = 4975;     // 4975us
    U32_V_REF_PLUSEWIDTH_TIME = 1666;      // 1666us
    U32_I_REF_PLUSEWIDTH_TIME = 1666;      // 1666us

    //电量检测初始值
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

    if (data_exist_flashflag != 0x55)	//判断EEPROM内是否存有校正值,若没有,则将参数设置为默认值
    {
      //data_exist_flag = 1;          //数据是否写入过flash,若写入过则不再置flash标志位

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
	mdev_t * dev;  //flash设备

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
	mdev_t * dev;  //flash设备

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
    U32_Cal_Times++;//用电量计时器，校正时间36S，1000W负载36S时间消耗0.01度电
    //功率计时器
    if (U16_P_CNT != 0)
    {
        U16_P_OneCycleTime++;  //单次测量循环次数
        U16_P_TotalTimes++;    //总循环次数,控制最大周期12s
    }
    //电压计时器
    if(B_VI_Test_Mode == 1)
    {
		if (U16_V_CNT != 0)
		{
			U16_V_OneCycleTime++;
			U16_V_TotalTimes++;
		}
    }
    //电流计时器
    else
    {
		if (U16_I_CNT != 0)
		{
			U16_I_OneCycleTime++;
			U16_I_TotalTimes++;
		}
    }
    //电压电流模式切换
    U16_VI_Test_Times--;
    //定时存储计时器
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
    		//u8 *E_PLUSE_CNT = (u8*)(&U16_REF_001_E_Pluse_CNT);//0.01度电参考次数

    		U8_CURR_WorkMode = D_NORMAL_MODE;  //进入测量模式
    	}

		TIM1_UPD_Interrupt();

		//接受脉冲信号，进入中断,测功率计次
		gpio_drv_set_cb(gpio_dev,gpio_P,
				        GPIO_INT_FALLING_EDGE,
				        NULL,PC5_EXTI_Interrupt);

		//测电压电流计次
		gpio_drv_set_cb(gpio_dev,gpio_IV,
				        GPIO_INT_FALLING_EDGE,
				        NULL,PD2_EXTI_Interrupt);

		//HLW8012_Measure()
		//达到计时时间或者单周期计时结束
		if(Measure_P_Flag)
		{
			//u8 *P_PLUSE_TIME;  //一周期的微秒数

			HLW8012_Measure_P();  //进行测量

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

		if(Measure_COS_Flag == 1)  //每更新一次P/V/I，计算一次cos
		{
			HLW8012_Measure_COS();
			Measure_COS_Flag = 0;
		}

		/*每15min定时存储
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

