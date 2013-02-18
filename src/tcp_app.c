
#include "tcp_app.h"
#include "io-helper.h"
#include "uip.h"

#include <string.h>


#define STATE_WAITING 0
#define STATE_OUTPUT  1

#define ISO_nl      0x0a


/*---------------------------------------------------------------------------*/
static  //TODO make it simple and add to tcp_app
PT_THREAD(handle_output(struct tcp_test_app_state *s))
{
  PSOCK_BEGIN(&s->sout);
  PSOCK_SEND_STR(&s->sout,"TEST");
  PSOCK_END(&s->sout);
}

/*---------------------------------------------------------------------------*/

static  //TODO make it simple
PT_THREAD(handle_input(struct tcp_test_app_state *s))
{
  PSOCK_BEGIN(&s->sin);

  PSOCK_READTO(&s->sin, ISO_nl);
  
  s->state = STATE_WAITING;
  
  if(strncmp(s->inputbuf, "ON", 2) == 0) {
    LED_On(1);
//    PSOCK_CLOSE_EXIT(&s->sin);
  }
  else if(strncmp(s->inputbuf, "OFF", 3) == 0) {
    LED_Off(1);
  }
  else
  {
    s->state = STATE_OUTPUT;
  }

  PSOCK_END(&s->sin);
}

static void
handle_connection(struct tcp_test_app_state *s)
{
  handle_input(s); //TODO niy
  if(s->state == STATE_OUTPUT) {
    handle_output(s); //TODO niy
  }
}

void tcp_test_appcall(void)
{
  struct tcp_test_app_state  *s = (struct tcp_test_app_state  *)&(uip_conn->appstate);

  if(uip_closed() || uip_aborted() || uip_timedout()) {
  } else if(uip_connected()) {
    PSOCK_INIT(&s->sin, s->inputbuf, sizeof(s->inputbuf) - 1);
    PSOCK_INIT(&s->sout, s->inputbuf, sizeof(s->inputbuf) - 1);
    PT_INIT(&s->outputpt);
    s->state = STATE_WAITING;
    /*    timer_set(&s->timer, CLOCK_SECOND * 100);*/
    s->timer = 0;
    handle_connection(s); //TODO
  } else if(s != 0) {
    if(uip_poll()) {
      ++s->timer;
      if(s->timer >= 20) {
	uip_abort();
      }
    } else {
      s->timer = 0;
    }
    handle_connection(s); //TODO
  } else {
    uip_abort();
  }
}

