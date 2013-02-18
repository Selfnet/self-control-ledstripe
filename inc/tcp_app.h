

#ifndef __TCP_APP_H__
#define __TCP_APP_H__


#include "psock.h"

//die funktion die von uip aufgerufen wird um das eigneltiche zeug zu machen

#define UIP_APPCALL tcp_test_appcall
void tcp_test_appcall(void);


//das daten struct das im uip_conn struct mitgespeichert wird und somit daten fürs eigentliche zeug das man pro verbindung brauch zu halten

struct tcp_test_app_state {
  unsigned char timer;
  struct psock sin, sout;
  struct pt outputpt, scriptpt;
  char inputbuf[50];
  //char filename[20];
  char state;
  //struct httpd_fs_file file;
  int len;
  char *scriptptr;
  int scriptlen;

  unsigned short count;
};

typedef struct tcp_test_app_state uip_tcp_appstate_t;

#endif
