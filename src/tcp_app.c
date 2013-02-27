
#include "tcp_app.h"
#include "io-helper.h"
#include "uip.h"

#include <string.h>


#define STATE_WAITING 0
#define STATE_OUTPUT  1

#define ISO_nl      0x0a


/*---------------------------------------------------------------------------*/

static handle_input(struct tcp_test_app_state *s)
{
    char * tmp = (char *)uip_appdata;
    //s->inputbuf = uip_appdata;

    if(strncmp(tmp, "ON", 2) == 0) {
        LED_On(1);
        


        
    }
    else if(strncmp(tmp, "OFF", 3) == 0) {
        LED_Off(1);
    }
    else if(strncmp(tmp, "TEST", 4) == 0) {
        if( strlen(s->outputbuf) )
            LED_Toggle(2);
    }
    else
    {
    }
}

static void
handle_connection(struct tcp_test_app_state *s)
{
  if( uip_newdata() )
    handle_input(s);
  if( strlen(s->outputbuf) > 0 )
  {
    uip_send(s->outputbuf , strlen(s->outputbuf) );
    memset(s->outputbuf, 0, 50);
  }
    
}

void tcp_test_appcall(void)
{
  struct tcp_test_app_state  *s = (struct tcp_test_app_state  *)&(uip_conn->appstate);

  if(uip_closed() || uip_aborted() || uip_timedout()) {
  } else if(uip_connected()) {
    //PSOCK_INIT(&s->sin, s->inputbuf, sizeof(s->inputbuf) - 1);
    //PSOCK_INIT(&s->sout, s->outputbuf, sizeof(s->outputbuf) - 1);
    s->timer = 0;
    strcpy(s->outputbuf , "hallo");
    
    handle_connection(s); //TODO
  } else if(s != 0) {
    if(uip_poll()) {
      ++s->timer;
      strcpy(s->outputbuf+strlen(s->outputbuf) , "Timer");
      if(s->timer >= 20) {
	        uip_abort(); //TimeOut
      }
    } else {
      s->timer = 0;
    }
    handle_connection(s); //TODO
  } else {
    uip_abort();
  }
}

