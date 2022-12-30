/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*_____ D E C L A R A T I O N S ____________________________________________*/

/*_____ D E F I N I T I O N S ______________________________________________*/

// #define USB_VID                         0x0416  /* Vendor ID */
// #define USB_PID                         0x5020  /* Product ID */

// #define USB_TIME_OUT                    100

/*
    FORMAT : 
    [HEAD] + [CODE] + [LEN] + [DATA..] + [CRC8] + [TAIL]

*/

#define CMD_DATA_LEN                    (16) 

#define CMD_HEAD                        (0x5A)
#define CMD_TAIL                        (0xA5)

#define CMD_FORMAT_LEN                  (sizeof (CMD_T)) 

#define CMD_SYNC                        (0x7F)
#define CMD_FUNC_01                     (0x80)
#define CMD_FUNC_02                     (0x81)
#define CMD_FUNC_03                     (0x82)
#define CMD_FUNC_04                     (0x83)
#define CMD_FUNC_05                     (0x84)


enum
{
    FAIL = -1,    
    OK = 0,
};

 typedef enum
{
    STATUS_OK                   = 0,
    STATUS_EMTPY_BUFFER         = -1,
    STATUS_EMTPY_LENGTH         = -2,
    STATUS_ERROR_LENGTH         = -3,
    STATUS_ERROR_CMD_HEAD       = -4,
    STATUS_ERROR_CMD_LEN        = -5,
    STATUS_ERROR_CHECKSUM       = -6,
    STATUS_ERROR_CMD_TAIL       = -7,

}cmd_status;

typedef struct
{
    unsigned char head;                 // 1 byte
    unsigned char code;                 // 1 byte
    unsigned char len_lsb;              // 1 byte
    unsigned char len_msb;              // 1 byte
    unsigned char data[CMD_DATA_LEN];   // 16 byte
    unsigned char checksum;             // 1 byte
    unsigned char tail;                 // 1 byte

} CMD_T;



/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

int cmd_encode(unsigned char cmd_code , unsigned char *data_buffer , unsigned short data_length , unsigned char *format_buffer);
int cmd_decode(unsigned char *format_buffer , unsigned short format_length , unsigned char *cmd_code ,unsigned char *data_buffer);



