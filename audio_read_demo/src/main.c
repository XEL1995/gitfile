/*
 * main.c
 *
 *  Created on: 2016年2月19日
 *      Author: wangshi
 */
#include <stdlib.h>
#include <wmstdio.h>
#include <wm_os.h>
#include <string.h>
#include <stdint.h>
#include <wmerrno.h>
#include <wm_utils.h>
#include <ctype.h>
#include <wmlist.h>
#include <psm-v2.h>
#include "../header/audio_read_write.h"
#include "../header/save_file_addr.h"

AudioFileInfo file_info;

void file_type_storage(int num,int len,AudioFileInfo *file_info)
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
		file_info->poweron_addr = file_info->start_addr + 4;
		file_info->poweron_size = len;
		file_info->start_addr = file_info->poweron_addr + len;
		break;
	case POWER_OFF_FILE:
		wmprintf("[in file_type] start_addr:%p,length:%d\r\n",file_info->start_addr,len);
		file_info->poweroff_addr = file_info->start_addr + 4;
		file_info->poweroff_size = len;
		file_info->start_addr = file_info->poweroff_addr + len;
		break;
	default:
		wmprintf("file type error\r\n");
	}
}

int struct_build(AudioFileInfo *file_info)
{
	unsigned char file_info_head[12];
	int file_len;
	int i = 0,j;

	while(i++ < 39)
	{
		audio_read(file_info_head,AUDIO_INFO_LEN,file_info->start_addr);
		for(j = 0;j < 12;j++)
		{
			wmprintf("%02x ",file_info_head[j]);
		}
		wmprintf("\r\n");
		if((file_info_head[0] == 0x55)
			&&(file_info_head[1] == 0xA5)
			&&(file_info_head[2] == 0x5A))
		{

			file_len = (int) ((file_info_head[11] << 24) + (file_info_head[10] << 16)
							 + (file_info_head[9] << 8) + (file_info_head[8])) + 8;
			file_type_storage(file_info_head[3],file_len,file_info);
		}
		else
		{
			wmprintf("data error!\r\n");
			return -1;
		}
	}
	return WM_SUCCESS;
}

int main(char* argv[],int argc)
{
	os_thread_sleep(os_msec_to_ticks(5000));
	wmstdio_init(UART0_ID, 0);//初始化Debug串口

	if(init_struct_flash(&file_info) == WM_SUCCESS)
	{
		if(struct_build(&file_info) != WM_SUCCESS)
		{
			wmprintf("struct build failed\r\n");
			//return 0;
		}
		write_struct(&file_info);
        read_struct(&file_info);
	}
	file_read(POWER_OFF_FILE);  //测试是否写入成功
	return 0;
}

