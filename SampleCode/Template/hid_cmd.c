/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright (c) 2010 Nuvoton Technology Corp. All rights reserved.                                        */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

#include "hid_cmd.h"


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

