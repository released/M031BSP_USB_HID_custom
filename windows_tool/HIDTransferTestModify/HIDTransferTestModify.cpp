/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright (c) 2010 Nuvoton Technology Corp. All rights reserved.                                        */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

#include "stdafx.h"
#include "HIDTransferTestModify.h"
#include "HID.hpp"

#include "hid_cmd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

unsigned char cmd_buffer[64] = {0};


void dump_buffer_hex(unsigned char *pucBuff, int nBytes)
{
    int     nIdx, i;

    nIdx = 0;
    while (nBytes > 0)
    {
        printf("0x%04X  ", nIdx);
        for (i = 0; i < 16; i++)
            printf("%02X ", pucBuff[nIdx + i]);
        printf("  ");
        for (i = 0; i < 16; i++)
        {
            if ((pucBuff[nIdx + i] >= 0x20) && (pucBuff[nIdx + i] < 127))
                printf("%c", pucBuff[nIdx + i]);
            else
                printf(".");
            nBytes--;
        }
        nIdx += 16;
        printf("\n");
    }
    printf("\n");
}


unsigned char cros_crc8_arg(const unsigned char *data, int len, unsigned char previous_crc)
{
	unsigned crc = previous_crc << 8;
	int i, j;

	for (j = len; j; j--, data++) {
		crc ^= (*data << 8);
		for (i = 8; i; i--) {
			if (crc & 0x8000)
				crc ^= (0x1070 << 3);
			crc <<= 1;
		}
	}

	return (unsigned char)(crc >> 8);
}

unsigned char cros_crc8(const unsigned char *data, int len)
{
	return cros_crc8_arg(data, len, 0);
}



int cmd_encode(unsigned char cmd_code , unsigned char *data_buffer , unsigned short data_length , unsigned char *format_buffer)
{
    unsigned short index = 0;
    unsigned char crc8 = 0;

    if (!data_buffer || !format_buffer )
    {
        return STATUS_EMTPY_BUFFER;
    }

    if (data_length == 0) 
    {
        return STATUS_EMTPY_LENGTH;
    }

    if (data_length > CMD_FORMAT_LEN) 
    {
        return STATUS_ERROR_LENGTH;
    }

    // HEAD
    format_buffer[index++] = CMD_HEAD;

    format_buffer[index++] = cmd_code;

    format_buffer[index++] = (data_length & 0xFF);           /* Low 8 bit */
    format_buffer[index++] = ((data_length >> 8) & 0xFF);    /* High 8 bit */

    // Data
    memcpy(&format_buffer[index], data_buffer, data_length);
    index += data_length;

    // CRC8
    crc8 = cros_crc8(format_buffer, index);
    format_buffer[index++] = crc8;

    // TAIL
    format_buffer[index++] = CMD_TAIL;

    return STATUS_OK;
}

int cmd_decode(unsigned char *format_buffer , unsigned short format_length , unsigned char *cmd_code ,unsigned char *data_buffer)
{
    unsigned short index = 0;
    // unsigned short len = 0;
    unsigned short data_length = 0;
    unsigned char *buffer;
    // unsigned short i = 0;
    unsigned char crc8 = 0;

    // check buffer
    if (!format_buffer)
    {
        return STATUS_EMTPY_BUFFER;
    }

    // check buffer length
    if (format_length == 0)
    {
        return STATUS_EMTPY_LENGTH;
    }

    // check buffer head
    if (format_buffer[index++] != CMD_HEAD)
    {
        return STATUS_ERROR_CMD_HEAD;
    }

    *cmd_code = format_buffer[index++];

    // decode data length
    data_length = ((unsigned short)((format_buffer[index] & 0x00FF) + ((format_buffer[index + 1] & 0x00FF) << 8)));

    if ((data_length == 0) || (data_length > format_length))
    {
        return STATUS_ERROR_CMD_LEN;
    }
    
    index += 2; // length take 2 bytes

    // decode data buffer
    buffer = &format_buffer[index];
    index += data_length;

    // check CRC8 checksum
    crc8 = cros_crc8(&format_buffer[0], index);

    if (crc8 != format_buffer[index])
    {
        return STATUS_ERROR_CHECKSUM;
    }

    index++;

    if (format_buffer[index] != CMD_TAIL)
    {
        return STATUS_ERROR_CMD_TAIL;
    }

    // prepare data output
    memcpy(data_buffer , buffer, data_length);    

    return STATUS_OK;
}

int data_decode(unsigned char * buffer)
{
    CMD_T cmd;
    int tmp = 0;
    unsigned char cmd_code = 0;
    unsigned char decode_data[CMD_DATA_LEN] = {0};

    tmp = cmd_decode((unsigned char *)buffer , sizeof(cmd) , &cmd_code ,decode_data);
    if(tmp != STATUS_OK)
    {
        printf("ERROR: cmd_decode fail! (%d)\r\n",tmp);

    } 
    printf("[from MCU] cmd_code = 0x%2X\r\n" , cmd_code );        
    dump_buffer_hex(decode_data,sizeof(decode_data));

    return OK;
}


int SendCmd_FUNC_05(void)
{
    CHidCmd io;
    CMD_T cmd;
    unsigned long length;
    BOOL bRet;
    bool isDeviceOpened;
    unsigned short packet_len = 0;
    unsigned short i = 0;

    isDeviceOpened = 0;
    if(!io.OpenDevice(USB_VID, USB_PID))
    {
        printf("Can't Open HID Device\r\n");
        return FAIL;
    }
    else
    {
        isDeviceOpened = TRUE;
        // printf("USB HID Device VID[%04x] PID[%04x] Open Success.\r\n", USB_VID, USB_PID);
        printf(">>> CMD_FUNC_05 command\r\n");

        cmd.code = CMD_FUNC_05;
        #if 1
        for(i = 0 ; i < CMD_DATA_LEN; i++)
        {
            cmd.data[i] = 0x50 + i;
        }
        #else
        memset(cmd.data , 0x00, CMD_DATA_LEN);
        #endif

        cmd_encode(cmd.code , &cmd.data[0] , CMD_DATA_LEN , (unsigned char *)&cmd);

        bRet = io.WriteFile((unsigned char *)&cmd, sizeof(cmd), &length, USB_TIME_OUT);
        // printf("sizeof(cmd) = %2d , length = %2d\r\n" , sizeof(cmd), length);
        if(!bRet)
        {
            printf("ERROR: Send SYNC command error!\n");
            if(isDeviceOpened)
                io.CloseDevice();

            return FAIL;
        }

        memset((unsigned char*) cmd_buffer, 0x00 , sizeof(cmd_buffer));
        bRet = io.ReadFile(cmd_buffer, sizeof(cmd_buffer) , &length, USB_TIME_OUT);
        data_decode(cmd_buffer);

        if(!bRet)
        {
            printf("ERROR: Send SYNC Read fail!\n");
            if(isDeviceOpened)
                io.CloseDevice();

            return FAIL;
        }
    }

    return OK;
}


int SendCmd_FUNC_04(void)
{
    CHidCmd io;
    CMD_T cmd;
    unsigned long length;
    BOOL bRet;
    bool isDeviceOpened;
    unsigned short packet_len = 0;
    unsigned short i = 0;

    isDeviceOpened = 0;
    if(!io.OpenDevice(USB_VID, USB_PID))
    {
        printf("Can't Open HID Device\r\n");
        return FAIL;
    }
    else
    {
        isDeviceOpened = TRUE;
        // printf("USB HID Device VID[%04x] PID[%04x] Open Success.\r\n", USB_VID, USB_PID);
        printf(">>> CMD_FUNC_04 command\r\n");

        cmd.code = CMD_FUNC_04;
        #if 1
        for(i = 0 ; i < CMD_DATA_LEN; i++)
        {
            cmd.data[i] = 0x40 + i;
        }
        #else
        memset(cmd.data , 0x00, CMD_DATA_LEN);
        #endif

        cmd_encode(cmd.code , &cmd.data[0] , CMD_DATA_LEN , (unsigned char *)&cmd);

        bRet = io.WriteFile((unsigned char *)&cmd, sizeof(cmd), &length, USB_TIME_OUT);
        // printf("sizeof(cmd) = %2d , length = %2d\r\n" , sizeof(cmd), length);
        if(!bRet)
        {
            printf("ERROR: Send SYNC command error!\n");
            if(isDeviceOpened)
                io.CloseDevice();

            return FAIL;
        }

        memset((unsigned char*) cmd_buffer, 0x00 , sizeof(cmd_buffer));
        bRet = io.ReadFile(cmd_buffer, sizeof(cmd_buffer) , &length, USB_TIME_OUT);
        data_decode(cmd_buffer);

        if(!bRet)
        {
            printf("ERROR: Send SYNC Read fail!\n");
            if(isDeviceOpened)
                io.CloseDevice();

            return FAIL;
        }
    }

    return OK;
}


int SendCmd_FUNC_03(void)
{
    CHidCmd io;
    CMD_T cmd;
    unsigned long length;
    BOOL bRet;
    bool isDeviceOpened;
    unsigned short packet_len = 0;
    unsigned short i = 0;

    isDeviceOpened = 0;
    if(!io.OpenDevice(USB_VID, USB_PID))
    {
        printf("Can't Open HID Device\r\n");
        return FAIL;
    }
    else
    {
        isDeviceOpened = TRUE;
        // printf("USB HID Device VID[%04x] PID[%04x] Open Success.\r\n", USB_VID, USB_PID);
        printf(">>> CMD_FUNC_03 command\r\n");

        cmd.code = CMD_FUNC_03;
        #if 1
        for(i = 0 ; i < CMD_DATA_LEN; i++)
        {
            cmd.data[i] = 0x30 + i;
        }
        #else
        memset(cmd.data , 0x00, CMD_DATA_LEN);
        #endif

        cmd_encode(cmd.code , &cmd.data[0] , CMD_DATA_LEN , (unsigned char *)&cmd);

        bRet = io.WriteFile((unsigned char *)&cmd, sizeof(cmd), &length, USB_TIME_OUT);
        // printf("sizeof(cmd) = %2d , length = %2d\r\n" , sizeof(cmd), length);
        if(!bRet)
        {
            printf("ERROR: Send SYNC command error!\n");
            if(isDeviceOpened)
                io.CloseDevice();

            return FAIL;
        }

        memset((unsigned char*) cmd_buffer, 0x00 , sizeof(cmd_buffer));
        bRet = io.ReadFile(cmd_buffer, sizeof(cmd_buffer) , &length, USB_TIME_OUT);
        data_decode(cmd_buffer);

        if(!bRet)
        {
            printf("ERROR: Send SYNC Read fail!\n");
            if(isDeviceOpened)
                io.CloseDevice();

            return FAIL;
        }
    }

    return OK;
}

int SendCmd_FUNC_02(void)
{
    CHidCmd io;
    CMD_T cmd;
    unsigned long length;
    BOOL bRet;
    bool isDeviceOpened;
    unsigned short packet_len = 0;
    unsigned short i = 0;

    isDeviceOpened = 0;
    if(!io.OpenDevice(USB_VID, USB_PID))
    {
        printf("Can't Open HID Device\r\n");
        return FAIL;
    }
    else
    {
        isDeviceOpened = TRUE;
        // printf("USB HID Device VID[%04x] PID[%04x] Open Success.\r\n", USB_VID, USB_PID);
        printf(">>> CMD_FUNC_02 command\r\n");

        cmd.code = CMD_FUNC_02;
        #if 1
        for(i = 0 ; i < CMD_DATA_LEN; i++)
        {
            cmd.data[i] = 0x20 + i;
        }
        #else
        memset(cmd.data , 0x00, CMD_DATA_LEN);
        #endif

        cmd_encode(cmd.code , &cmd.data[0] , CMD_DATA_LEN , (unsigned char *)&cmd);

        bRet = io.WriteFile((unsigned char *)&cmd, sizeof(cmd), &length, USB_TIME_OUT);
        // printf("sizeof(cmd) = %2d , length = %2d\r\n" , sizeof(cmd), length);
        if(!bRet)
        {
            printf("ERROR: Send SYNC command error!\n");
            if(isDeviceOpened)
                io.CloseDevice();

            return FAIL;
        }

        memset((unsigned char*) cmd_buffer, 0x00 , sizeof(cmd_buffer));
        bRet = io.ReadFile(cmd_buffer, sizeof(cmd_buffer) , &length, USB_TIME_OUT);
        data_decode(cmd_buffer);

        if(!bRet)
        {
            printf("ERROR: Send SYNC Read fail!\n");
            if(isDeviceOpened)
                io.CloseDevice();

            return FAIL;
        }
    }

    return OK;
}

int SendCmd_FUNC_01(void)
{
    CHidCmd io;
    CMD_T cmd;
    unsigned long length;
    BOOL bRet;
    bool isDeviceOpened;
    unsigned short packet_len = 0;
    unsigned short i = 0;

    isDeviceOpened = 0;
    if(!io.OpenDevice(USB_VID, USB_PID))
    {
        printf("Can't Open HID Device\r\n");
        return FAIL;
    }
    else
    {
        isDeviceOpened = TRUE;
        // printf("USB HID Device VID[%04x] PID[%04x] Open Success.\r\n", USB_VID, USB_PID);
        printf(">>> CMD_FUNC_01 command\r\n");

        cmd.code = CMD_FUNC_01;
        #if 1
        for(i = 0 ; i < CMD_DATA_LEN; i++)
        {
            cmd.data[i] = 0x7F;
        }
        #else
        memset(cmd.data , 0x00, CMD_DATA_LEN);
        #endif

        cmd_encode(cmd.code , &cmd.data[0] , CMD_DATA_LEN , (unsigned char *)&cmd);

        bRet = io.WriteFile((unsigned char *)&cmd, sizeof(cmd), &length, USB_TIME_OUT);
        // printf("sizeof(cmd) = %2d , length = %2d\r\n" , sizeof(cmd), length);
        if(!bRet)
        {
            printf("ERROR: Send SYNC command error!\n");
            if(isDeviceOpened)
                io.CloseDevice();

            return FAIL;
        }

        memset((unsigned char*) cmd_buffer, 0x00 , sizeof(cmd_buffer));
        bRet = io.ReadFile(cmd_buffer, sizeof(cmd_buffer) , &length, USB_TIME_OUT);
        data_decode(cmd_buffer);

        if(!bRet)
        {
            printf("ERROR: Send SYNC Read fail!\n");
            if(isDeviceOpened)
                io.CloseDevice();

            return FAIL;
        }
    }

    return OK;
}

int SendSyncCmd(void)
{
    CHidCmd io;
    CMD_T cmd;
    unsigned long length;
    BOOL bRet;
    bool isDeviceOpened;
    unsigned short packet_len = 0;
    unsigned short i = 0;

    isDeviceOpened = 0;
    if(!io.OpenDevice(USB_VID, USB_PID))
    {
        printf("Can't Open HID Device\r\n");
        return FAIL;
    }
    else
    {
        isDeviceOpened = TRUE;
        printf("USB HID Device VID[%04x] PID[%04x] Open Success.\r\n", USB_VID, USB_PID);
        printf(">>> SYNC command\r\n");

        cmd.code = CMD_SYNC;
        #if 0
        for(i = 0 ; i < CMD_DATA_LEN; i++)
        {
            cmd.data[i] = i;
        }
        #else
        memset(cmd.data , 0x00, CMD_DATA_LEN);
        #endif

        cmd_encode(cmd.code , &cmd.data[0] , CMD_DATA_LEN , (unsigned char *)&cmd);

        bRet = io.WriteFile((unsigned char *)&cmd, sizeof(cmd), &length, USB_TIME_OUT);
        // printf("sizeof(cmd) = %2d , length = %2d\r\n" , sizeof(cmd), length);
        if(!bRet)
        {
            printf("ERROR: Send SYNC command error!\n");
            if(isDeviceOpened)
                io.CloseDevice();

            return FAIL;
        }

        memset((unsigned char*) cmd_buffer, 0x00 , sizeof(cmd_buffer));
        bRet = io.ReadFile(cmd_buffer, sizeof(cmd_buffer) , &length, USB_TIME_OUT);
        data_decode(cmd_buffer);

        if(!bRet)
        {
            printf("ERROR: Send SYNC Read fail!\n");
            if(isDeviceOpened)
                io.CloseDevice();

            return FAIL;
        }
    }

    return OK;
}

int hid_cmd_process(void)
{
    unsigned short time = 250;
    int isErr = 0;
    
    Sleep(time);
    isErr = SendSyncCmd();
    if(isErr)
    {
        printf("ERROR: SendSyncCmd fail!\n");
        return -1;
    }
    else
    {

        Sleep(time);
        isErr = SendCmd_FUNC_01();
        if(isErr)
        {
            printf("ERROR: SendCmd_FUNC_01 fail!\n");
            return -1;
        }    

        Sleep(time);
        isErr = SendCmd_FUNC_02();
        if(isErr)
        {
            printf("ERROR: SendCmd_FUNC_02 fail!\n");
            return -1;
        }   

        Sleep(time);
        isErr = SendCmd_FUNC_03();
        if(isErr)
        {
            printf("ERROR: SendCmd_FUNC_03 fail!\n");
            return -1;
        }   

        Sleep(time);
        isErr = SendCmd_FUNC_04();
        if(isErr)
        {
            printf("ERROR: SendCmd_FUNC_04 fail!\n");
            return -1;
        }   

        Sleep(time);
        isErr = SendCmd_FUNC_05();
        if(isErr)
        {
            printf("ERROR: SendCmd_FUNC_05 fail!\n");
            return -1;
        }   
    }


    return 0;

}

int main(void)
{
    
    #if 1   // period
    while(1)
    {
        hid_cmd_process();
    }
    #else   // one time
    Sleep(500);
    isErr = SendSyncCmd();
    if(isErr)
    {
        printf("ERROR: SendSyncCmd fail!\n");
        return -1;
    }

    isErr = SendCmd_FUNC_01();
    if(isErr)
    {
        printf("ERROR: SendCmd_FUNC_01 fail!\n");
        return -1;
    }    

    isErr = SendCmd_FUNC_02();
    if(isErr)
    {
        printf("ERROR: SendCmd_FUNC_02 fail!\n");
        return -1;
    }   

    isErr = SendCmd_FUNC_03();
    if(isErr)
    {
        printf("ERROR: SendCmd_FUNC_03 fail!\n");
        return -1;
    }   

    isErr = SendCmd_FUNC_04();
    if(isErr)
    {
        printf("ERROR: SendCmd_FUNC_04 fail!\n");
        return -1;
    }   

    isErr = SendCmd_FUNC_05();
    if(isErr)
    {
        printf("ERROR: SendCmd_FUNC_05 fail!\n");
        return -1;
    }   
 
    printf("HID Transfer test ok!\n");

    return 0;

    #endif
}