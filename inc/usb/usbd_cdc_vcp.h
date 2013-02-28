/**
  ******************************************************************************
  * @file    usbd_cdc_vcp.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   Header for usbd_cdc_vcp.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CDC_VCP_H
#define __USBD_CDC_VCP_H

/* Includes ------------------------------------------------------------------*/
 #include "stm32f10x.h"

#include "usbd_cdc_core.h"
#include "usbd_conf.h"


/* Exported typef ------------------------------------------------------------*/
/* The following structures groups all needed parameters to be configured for the 
   ComPort. These parameters can modified on the fly by the host through CDC class
   command class requests. */
typedef struct
{
  uint32_t bitrate;
  uint8_t  format;
  uint8_t  paritytype;
  uint8_t  datatype;
}LINE_CODING;

/* Exported constants --------------------------------------------------------*/

#define DEFAULT_CONFIG                  0
#define OTHER_CONFIG                    1

/** Size of the circular receive buffer, must be power of 2 */
#define COM_RX_BUFFER_SIZE 64

/** Size of the circular transmit buffer, must be power of 2 */
#define COM_TX_BUFFER_SIZE 64

/* 
** high byte error return code of uart_getc()
*/
#define COM_FRAME_ERROR      0x0800              /* Framing Error by UART       */
#define COM_OVERRUN_ERROR    0x0400              /* Overrun condition by UART   */
#define COM_BUFFER_OVERFLOW  0x0200              /* receive ringbuffer overflow */
#define COM_NO_DATA          0x0100              /* no receive data available   */

 // definition of limiter bytes for frame based RS232 communication with computers
#define RS232_PRE 		0xB0
#define RS232_POST 	0xB2
#define RS232_ESC 		0xB4





/* Exported Variables -------------------------------------------------------*/
//extern volatile uint8_t COM_RxBuf[COM_RX_BUFFER_SIZE];
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint16_t VCP_DataTx (uint8_t* Buf, uint32_t Len);
uint8_t com_getmsg(uint8_t RS232_msg[],uint8_t length);

#endif /* __USBD_CDC_VCP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
