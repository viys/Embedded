#ifndef _FATFSUSER_H
#define _FATFSUSER_H

#include "stm32f10x.h"
#include "ff.h"
#include "string.h"
#include "usart.h"

FRESULT miscellaneous(void);
FRESULT file_check(void);
FRESULT scan_files(char* path);

#endif
