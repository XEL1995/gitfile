/*
 * save_file_addr.h
 *
 *  Created on: 2016��2��4��
 *      Author: Administrator
 */

#ifndef SAMPLE_APPS_IO_DEMO_FILE_SAVE_SRC_SAVE_FILE_ADDR_H_
#define SAMPLE_APPS_IO_DEMO_FILE_SAVE_SRC_SAVE_FILE_ADDR_H_
#include "audio_read_write.h"
//��ʼ���ṹ��
//int init_struct_flash(uint32_t address);
int init_struct_flash(AudioFileInfo *audio_file);

int file_write(char* file_buf,/*�洢�ļ�����*/
		        unsigned char num,/*�ļ����*/
				int length/*�ļ�����*/);

void file_read(int num);

void read_struct(AudioFileInfo *audio_file_info);

void write_struct(AudioFileInfo *audio_file_info);
//д��Ƶ�ļ������ݵ�flash��.
//int audio_write(const uint8_t *buf,uint32_t len, uint32_t address);
//��flash�й̶���ַ��ȡ��Ƶ�ļ�����.
//int audio_read(uint8_t* buf,int len, uint32_t address);
#endif /* SAMPLE_APPS_IO_DEMO_FILE_SAVE_SRC_SAVE_FILE_ADDR_H_ */
