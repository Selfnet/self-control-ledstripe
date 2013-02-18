#ifndef __SYSCALLS_H__
#define __SYSCALLS_H__

#include <reent.h>
#include <errno.h>
#include <stdlib.h> /* abort */
#include <sys/types.h>
#include <sys/stat.h>

#include "term_io.h"
#include "stm32f10x.h" /* for _get_PSP() from core_cm3.h*/

int _write(int file, char *ptr, int len);

#endif /*__SYSCALLS_H__ */
