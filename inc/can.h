
#ifndef __CAN_H__
#define __CAN_H__

// Can functions
void CAN_config(void);
void prozess_can_it(void);

int getSender(uint32_t ExtId);
int getRecipient(uint32_t ExtId);
int getTyp(uint32_t ExtId);
void setSender(uint32_t *ExtId , int recipient);
void setRecipient(uint32_t *ExtId , int recipient);
void setTyp(uint32_t *ExtId , int recipient);

//NODE_CAN_ID = 32 = 0x20 = 0b100000
#define NODE_CAN_ID 0x20

#endif
