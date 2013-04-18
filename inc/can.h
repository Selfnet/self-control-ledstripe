
#ifndef __CAN_H__
#define __CAN_H__

// Can functions
void CAN_config(void);
void prozess_can_it(void);
void send_sync(char data);


#define getSender(ExtId)    ( ( (ExtId)     & 0xff ) )
#define getRecipient(ExtId) ( ( (ExtId>>8 ) & 0xff ) )
#define getTyp(ExtId)       ( ( (ExtId>>16) & 0xff ) )

#define setSender(X)        ( ( (X)         & 0xff ) )
#define setRecipient(X)     ( ( (X<<8)      & 0xff00 ) )
#define setType(X)          ( ( (X<<16)     & 0xff0000 ) )

//NODE_CAN_ID = 32 = 0x20 = 0b100000
#ifdef TEST_GATEWAY
    #define NODE_CAN_ID 0x21
#else
    #define NODE_CAN_ID 0x20
#endif
#define NODE_CAN_BROADCAST 0xFF

#define CAN_PROTO_SYNC 0x0A
#define CAN_PROTO_PING 0x08
#define CAN_PROTO_PONG 0x09
#define CAN_PROTO_LED  0xC0
#define CAN_PROTO_TEXT 0xD0

// ext id =  0b10 100  0*24
#define CAN_EXT_ID 0x14000000

#endif
