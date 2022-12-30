# M031BSP_USB_HID_custom
 M031BSP_USB_HID_custom

 
update @ 2022/12/30

1. base on BSP USBD_HID_Transfer sample code ,  modify MCU and PC host tool

- PC host will issue CMD_SYNC to MCU , and start communication (INTERRUPT OUT)

- after MCU receive command , will reply modified data into commnad packet (INTERRUPT IN)

2. command packet : 

- [HEAD] + [CODE] + [LEN] + [DATA..] + [CRC8] + [TAIL]

3. Below is MCU terminal log message

![image](https://github.com/released/M031BSP_USB_HID_custom/blob/main/log.jpg)	

log message when execute windows tool 

![image](https://github.com/released/M031BSP_USB_HID_custom/blob/main/windows_tool_log.jpg)	

command handsaking 

PC to MCU : CMD_SYNC ( 0x7F) , after MCU recieve and decode data , will plus 1 into data array then send to PC

PC to MCU : CMD_FUNC_01 ( 0x80) , after MCU recieve and decode data , will plus 0x10 into data array then send to PC

PC to MCU : CMD_FUNC_02 ( 0x81) , after MCU recieve and decode data , will plus 0x10 into data array then send to PC

PC to MCU : CMD_FUNC_03 ( 0x82) , after MCU recieve and decode data , will plus 0x10 into data array then send to PC

PC to MCU : CMD_FUNC_04 ( 0x83) , after MCU recieve and decode data , will plus 0x10 into data array then send to PC

PC to MCU : CMD_FUNC_05 ( 0x84) , after MCU recieve and decode data , will plus 0x10 into data array then send to PC

![image](https://github.com/released/M031BSP_USB_HID_custom/blob/main/compare.jpg)	

4. below is wireshark log capture 

CMD_SYNC ( from PC to MCU ) 

![image](https://github.com/released/M031BSP_USB_HID_custom/blob/main/wireshark_USB_capture_00_SYNC.jpg)	

CMD_SYNC ( from MCU to PC ) 

![image](https://github.com/released/M031BSP_USB_HID_custom/blob/main/wireshark_USB_capture_01_SYNC.jpg)	

CMD_FUNC_02 ( from PC to MCU ) 

![image](https://github.com/released/M031BSP_USB_HID_custom/blob/main/wireshark_USB_capture_00_CMD_FUNC_02.jpg)	

CMD_FUNC_02 ( from MCU to PC ) 

![image](https://github.com/released/M031BSP_USB_HID_custom/blob/main/wireshark_USB_capture_01_CMD_FUNC_02.jpg)	



