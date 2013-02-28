/**
  ******************************************************************************
  * @file    usbd_cdc_vcp.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   Generic media access Layer.
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
  
/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_vcp.h"
#include "usb_conf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/*
 *  constants and macros
 */

/* size of RX/TX buffers */
#define COM_RX_BUFFER_MASK ( COM_RX_BUFFER_SIZE - 1)

#if ( COM_RX_BUFFER_SIZE & COM_RX_BUFFER_MASK )
#error RX buffer size is not a power of 2
#endif

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
LINE_CODING linecoding =
  {
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* nb. of bits 8*/
  };

/* These are external variables imported from CDC core to be used for IN 
   transfer management. */
extern uint8_t  APP_Rx_Buffer []; /* Write CDC received data in this buffer.
                                     These data will be sent over USB IN endpoint
                                     in the CDC core functions. */
extern uint32_t APP_Rx_ptr_in;    /* Increment this pointer or roll it back to
                                     start address when writing received data
                                     in the buffer APP_Rx_Buffer. */           
                                     
/*
 *  module global variables
 */
static volatile uint8_t COM_RxBuf[COM_RX_BUFFER_SIZE];
static volatile uint8_t COM_RxHead;
static volatile uint8_t COM_RxMsgBegin;
static volatile uint8_t COM_RxMsgEnd;


static volatile uint8_t COM_escaped = 0;
static volatile uint8_t COM_msgReady = 0;
static volatile uint8_t COM_msgProcessed = 0;
static volatile uint8_t COM_msgStarted = 0;
static volatile uint8_t COM_overflow = 0;

 uint8_t rs232_debug_msg[2];                                           

/* Private function prototypes -----------------------------------------------*/
static uint16_t VCP_Init     (void);
static uint16_t VCP_DeInit   (void);
static uint16_t VCP_Ctrl     (uint32_t Cmd, uint8_t* Buf, uint32_t Len);
//static uint16_t VCP_DataTx   (uint8_t* Buf, uint32_t Len); //needed as exported function in .h
static uint16_t VCP_DataRx   (uint8_t* Buf, uint32_t Len);

CDC_IF_Prop_TypeDef VCP_fops = 
{
  VCP_Init,
  VCP_DeInit,
  VCP_Ctrl,
  VCP_DataTx,
  VCP_DataRx
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  VCP_Init
  *         Initializes the Media on the STM32
  * @param  None
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t VCP_Init(void)
{
  return USBD_OK;
}

/**
  * @brief  VCP_DeInit
  *         DeInitializes the Media on the STM32
  * @param  None
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t VCP_DeInit(void)
{
  return USBD_OK;
}


/**
  * @brief  VCP_Ctrl
  *         Manage the CDC class requests
  * @param  Cmd: Command code            
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t VCP_Ctrl (uint32_t Cmd, uint8_t* Buf, uint32_t Len)
{ 
  switch (Cmd)
  {
  case SEND_ENCAPSULATED_COMMAND:
    /* Not  needed for this driver */
    break;

  case GET_ENCAPSULATED_RESPONSE:
    /* Not  needed for this driver */
    break;

  case SET_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case GET_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case CLEAR_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case SET_LINE_CODING:
    /* Not  needed for this driver */
    break;

  case GET_LINE_CODING:
    Buf[0] = (uint8_t)(linecoding.bitrate);
    Buf[1] = (uint8_t)(linecoding.bitrate >> 8);
    Buf[2] = (uint8_t)(linecoding.bitrate >> 16);
    Buf[3] = (uint8_t)(linecoding.bitrate >> 24);
    Buf[4] = linecoding.format;
    Buf[5] = linecoding.paritytype;
    Buf[6] = linecoding.datatype; 
    break;
    


  case SET_CONTROL_LINE_STATE:
    /* Not  needed for this driver */
    break;

  case SEND_BREAK:
    /* Not  needed for this driver */
    break;    
    
  default:
    break;
  }

  return USBD_OK;
}

/**
  * @brief  VCP_DataTx
  *         CDC received data to be send over USB IN endpoint are managed in 
  *         this function.
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else VCP_FAIL
  */
uint16_t VCP_DataTx (uint8_t* Buf, uint32_t Len)
{
    uint32_t i = 0;
    uint8_t esccounter = 0;
    uint8_t byte = 0;
    uint8_t buf_free = 0;
    uint8_t retval = 0;
    
  //APP_Rx_Buffer[APP_Rx_ptr_in] = RS232_PRE;
  //APP_Rx_ptr_in++;
    
  for(i=0; i<Len;i++){
  
	    byte = *(Buf+i);
		
		 // standard case: byte is sent normally
		  if( (byte != RS232_PRE) && (byte != RS232_POST) && (byte != RS232_ESC)	){
			    APP_Rx_Buffer[APP_Rx_ptr_in] = byte;
			    APP_Rx_ptr_in++;
		    }
		    // byte is control code: escape and xor
		    else{
			    APP_Rx_Buffer[APP_Rx_ptr_in] = RS232_ESC;
			    APP_Rx_ptr_in++;
			    byte += 0x01;
			    APP_Rx_Buffer[APP_Rx_ptr_in] = byte;
			    APP_Rx_ptr_in++;
		    }
		  }
		   // send stop byte
	       //APP_Rx_Buffer[APP_Rx_ptr_in] = RS232_POST;
	       //APP_Rx_ptr_in++;
		        
  return USBD_OK;
}

/**
  * @brief  VCP_DataRx
  *         Data received over USB OUT endpoint are sent over CDC interface 
  *         through this function.
  *           
  *         @note
  *         This function will block any OUT packet reception on USB endpoint 
  *         untill exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result 
  *         in receiving more data while previous ones are still not sent.
  *                 
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else VCP_FAIL
  */
  //static removed...
static uint16_t VCP_DataRx (uint8_t* Buf, uint32_t Len)
{
  static uint32_t i;
  static uint8_t data;
  uint8_t send_string[] = {RS232_PRE,0x02,0x01,RS232_POST};
  
  for (i = 0; i < Len; i++)
  {
    data = *(Buf+i);
    
    //send_string[2] = *(Buf+i);
    //VCP_DataTx(&send_string[0],4);
    
	// frames aus dem byte wust fuddeln
	// das startbyte setzt den empfangspuffer zurück auf das Ende der zuletzt
	// komplett empfangenen Nachricht
	
    if(data==RS232_PRE){
	     COM_RxHead = (COM_RxMsgEnd);
	     COM_msgStarted = 1;
	     COM_escaped = 0;
	     
	     send_string[2] = 1;
	     VCP_DataTx(&send_string[0],4);
     }
     // eof byte 
     // discard if no msg is started
     
     else if(COM_msgStarted){
             if(data==RS232_POST){
             
                send_string[2] = 4;
        	    VCP_DataTx(&send_string[0],4);
        	    
                COM_msgStarted = 0;
                if(COM_msgReady == 0){
                    COM_RxMsgBegin = COM_RxMsgEnd;
           	        COM_RxMsgEnd = COM_RxHead;
           	        COM_msgReady = 1;
           	        // block further communication from PC
           	        //PORTD &= ~(1<<PD2);
           	    }   
	            // clear msgStarted flag to supress double reception of messages
	            // double post byte is sent to make flow control more robust
	         }
             // resolve escaped command bytes and store data
             else if(data==RS232_ESC){
	            COM_escaped=1;
	            
	            send_string[2] = 0;
	            VCP_DataTx(&send_string[0],4);
	            
             }
             else{
                // if buffer is full, abort message collection
                if(((COM_RxHead + 1) & COM_RX_BUFFER_MASK) == COM_RxMsgBegin){
                    COM_msgStarted = 0;     
                }
                // otherwise collect content to buffer
                else{
                    COM_RxHead = (COM_RxHead + 1) & COM_RX_BUFFER_MASK;
	                if(!COM_escaped){
		                COM_RxBuf[COM_RxHead] = data;
	                }
	                else{
		                COM_RxBuf[COM_RxHead] = data - 0x01;
		                COM_escaped = 0;
	                }
	                
	                send_string[2] = 2;
	                VCP_DataTx(&send_string[0],4);
	                
	            }
	        }
	    }
    }
    return USBD_OK;
}


uint8_t com_getmsg(uint8_t RS232_msg[],uint8_t length){
	uint8_t msgNum, index, msg_size, lowIndex, highIndex, temphead, retval;
	
	retval = 0;
	
	if(COM_msgReady){
	    
	    if(COM_RxMsgEnd >= COM_RxMsgBegin)
	        msg_size = COM_RxMsgEnd-COM_RxMsgBegin;
	    else if(COM_RxMsgEnd < COM_RxMsgBegin)
	        msg_size = COM_RX_BUFFER_SIZE - (COM_RxMsgBegin - COM_RxMsgEnd);
	    
	    if(msg_size <= length){
	        temphead = (COM_RxMsgBegin + 1) & COM_RX_BUFFER_MASK;
	        for(index=0;index<msg_size;index++){
	            RS232_msg[index] = COM_RxBuf[(temphead + index) & COM_RX_BUFFER_MASK];
	        }
	        
		    if(RS232_msg[0]==0x01){
/*			    can_msg->id = *((uint32_t*) &RS232_msg[1]);*/

/*			    can_msg->flags.rtr = RS232_msg[5]>>1;*/
/*			    can_msg->flags.extended = RS232_msg[5] & 0x01;*/
/*		*/
/*			    can_msg->length = RS232_msg[6];*/
/*		*/
/*			    for(index=0; index<=can_msg->length; index++)*/
/*				    can_msg->data[index] = RS232_msg[index+7];*/

			    retval = 1;
			
		    }
		    else if(RS232_msg[0]==0x02){
		        //uint8_t send_string[] = {RS232_PRE,0x02,0x0A,RS232_POST};
		        //VCP_DataTx(&send_string[0], 4);
			    retval = 2;
		    }
	    }
	    COM_msgReady = 0;
	    retval = msg_size;
	}
	//PORTD |= (1<<PD2);
	return retval;
}













/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
