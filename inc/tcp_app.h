

#ifndef __TCP_APP_H__
#define __TCP_APP_H__

#include <stdint.h>

//#include "psock.h"

//die funktion die von uip aufgerufen wird um das eigneltiche zeug zu machen

#define UIP_APPCALL tcp_test_appcall
void tcp_test_appcall(void);


#define TCP_APP_MAX_QUEUE_LEN 10
#define TCP_APP_BUFFER_SIZE 256

//das daten struct das im uip_conn struct mitgespeichert wird und somit daten fürs eigentliche zeug das man pro verbindung brauch zu halten

struct tcp_test_app_state {
    unsigned char timer;
    //char inputbuf[128];
    
    char output_buf[TCP_APP_BUFFER_SIZE];
    char *output_buf_pt;
    
    char * output_queue_pt[TCP_APP_MAX_QUEUE_LEN];
    uint8_t output_queue_pos;
};

typedef struct tcp_test_app_state uip_tcp_appstate_t;


void send_ascii(uint32_t len, char *txt);
int send_tcp(uip_tcp_appstate_t *s, char *txt, uint16_t len);
int append_to_cur_tcp(uip_tcp_appstate_t *s, char c);


#endif
