/*
 * save_file_addr.c
 *
 *  Created on: 2016年2月2日
 *      Author: Administrator
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <wmstdio.h>
#include <wm_os.h>

#include <wmerrno.h>
#include <wm_utils.h>
#include <ctype.h>
#include <wmlist.h>

#include "../header/audio_read_write.h"
//#include "../header/save_file_addr.h"

#define POWER_ON_MARK fffffffffffffffe
#define AUDIO_INFO_LEN 12
//#define AUDIO_ADDR 0x1cc000  //结构体写入地址

extern AudioFileInfo file_info;


//根据文件类型将文件地址存入结构体中对应的成员
void file_type(int num,int len,AudioFileInfo *file_info,uint64_t flag)
{

	switch(num)
	{
	case POWER_ON_FILE:
		if(file_info->poweron_addr != 0)
		{
			wmprintf("poweron_addr is exited\r\n");
			break;
		}
		wmprintf("[in file_type] start_addr:%p,length:%d\r\n",file_info->start_addr,len);
		file_info->poweron_addr = file_info->start_addr;
		file_info->poweron_size = len;
		flag = flag&0xfffffffe|0x00000001;
		break;
	case POWER_OFF_FILE:
		wmprintf("[in file_type] start_addr:%p,length:%d\r\n",file_info->start_addr,len);
		file_info->poweroff_addr = file_info->start_addr;
		file_info->poweroff_size = len;
		break;
	case WARM_MODE_FILE:
		wmprintf("[in file_type] start_addr:%p,length:%d\r\n",file_info->start_addr,len);
		file_info->poweroff_addr = file_info->start_addr;
		file_info->poweroff_size = len;
		break;
#if 0
	case COLD_MODE_FILE:
		break;
	case DRY_MODE_FILE:
	case AUTO_MODE_FILE:
	case WIND_MODE_FILE:
	case LIGHT_WIND_FILE:
	case LOW_WIND_FILE:
	case MID_WIND_FILE:
	case HIGH_WIND_FILE:
	case STRONG_WIND_FILE:
	case AUTO_WIND_FILE:
	case AIR_FRESH_FILE:
	case HORIZON_ON_FILE:
	case HORIZON_OFF_FILE:
	case VERTICAL_ON_FILE:
	case VERTICAL_OFF_FILE:
	case ELEHEAT_ON_FILE:
	case ELEHEAT_OFF_FILE:
	case SLEEP_ON_FILE:
	case SLEEP_OFF_FILE:
	case TEM_SIXTEEN_FILE:
	case TEM_SEVENTEEN_FILE:
	case TEM_EIGHTEEN_FILE:
	case TEM_NINETEEN_FILE:
	case TEM_TWENTY_FILE:
	case TEM_TWENTY_ONE_FILE:
	case TEM_TWENTY_TWO_FILE:
	case TEM_TWENTY_THREE_FILE:
	case TEM_TWENTY_FOUR_FILE:
	case TEM_TWENTY_FIVE_FILE:
	case TEM_TWENTY_SIX_FILE:
	case TEM_TWENTY_SEVEN_FILE:
	case TEM_TWENTY_EIGHT_FILE:
	case TEM_TWENTY_NINE_FILE:
	case TEM_THIRTY_FILE:
	case TEM_THIRTY_ONE_FILE:
	case TEM_THIRTY_TWO_FILE:
#endif
	default:
		wmprintf("file type error\r\n");
	}

}

//根据文件编号从flash中读取出对应文件
void file_read(int num)
{

	char * file_buf = NULL;
	uint32_t address;
	uint16_t length = 0;
	int i;

	switch(num)
	{
	case 1:
		address = file_info.poweron_addr;  //根据文件编号获取对应的文件地址和长度
		length = file_info.poweron_size;
		wmprintf("[in file_read] start_addr:%p,length:%d\r\n",address,length);
		break;

	case 2:
		address = file_info.poweroff_addr;
		length = file_info.poweroff_size;
		wmprintf("[in file_read] start_addr:%p,length:%d\r\n",address,length);
		break;

	case 3:
		break;
	default:
		wmprintf("file type is not exist\r\n");
	}
	if(length)
	{
		file_buf = os_mem_alloc(length);
		audio_read(file_buf,length,address);
		wmprintf("file_buffer:\r\n");
		for(i = 0;i < 50;i++)
		{
			wmprintf("%02x ",file_buf[i]);
		}
		wmprintf("\r\n");
		for(i = length - 50;i < length;i++)
		{
			wmprintf("%02x ",file_buf[i]);
		}
		wmprintf("\r\n");
		//play_wav(file_buf,length);
		os_mem_free(file_buf);
	}
	else
		wmprintf("no data\r\n");

}
#if 0
//用于收到文件的校验
void file_miss(int num)
{
	switch(num)
	case 0:
		wmprintf("poweron audio file missing\r\n");
		break;
	case 1:
		wmprintf("poweroff audio file missing\r\n");
		break;
	default:
}

void file_verify(uint64_t flag)
{
	int i;
	if(flag == ffffffff)
		//设置结构体中的标志位
	else
	{
		flag ^= ffffffff;
		for(i=0;i<38;i++)
		{
			if((flag>>i)&00000001)
				file_miss(i);
		}
	}

}
#endif

//将完整文件写入flash
int file_write(char* file_buf,/*存储文件内容*/
		        unsigned char num,/*文件编号*/
				int length/*文件长度*/)
{

	uint64_t flag = 0;
	int ret;

    wmprintf("start_addr:%p\r\n",file_info.start_addr);

	ret = audio_write(file_buf,length,file_info.start_addr); //将音频文件写入flash

	file_type(num,length,&file_info,flag); //根据文件类型保存地址信息

	if(length%4096 != 0)
	{
		file_info.start_addr += 4096*(length/4096+1);
		wmprintf("ret = %d,mod = %d\r\n",length/4096,length%4096);
	}
	else
	{
		file_info.start_addr += length; //将写文件的起始地址向后偏移
	}

	return ret;
}

//读出文件信息结构体
void read_struct(AudioFileInfo *audio_file_info)
{
	audio_read(audio_file_info,sizeof(AudioFileInfo),INFO_STRUCT_ADDR);
	show_audio_file_info(audio_file_info);
}

//将文件信息结构体写入flash
void write_struct(AudioFileInfo *audio_file_info)
{
	audio_write(audio_file_info,sizeof(AudioFileInfo),INFO_STRUCT_ADDR);
	show_audio_file_info(audio_file_info);
}

//第一次往flash分区中存放结构体时调用这个初始化函数。
int init_struct_flash(AudioFileInfo *audio_file)
{
	wmprintf("In function init_struct_flash\r\n");

	read_struct(audio_file);

	if(audio_file->flash_init_flag != 1)
	{
		memset(audio_file,0,sizeof(AudioFileInfo));
		audio_file->flash_init_flag = 0x1;
		audio_file->start_addr = AUDIO_ADDR;

	show_audio_file_info(audio_file);
	return WM_SUCCESS;
	}

	return -1;
}

