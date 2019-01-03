#ifndef __TRANSPORT_H
#define __TRANSPORT_H

#include <stdint.h>
#include <windows.h>
#include "base/types_def.h"

int transport_sendPacketBuffer(unsigned char *buf, int buflen);
int transport_getdata(unsigned char *buf, int count);
SOCKET transport_open(unsigned char* servip, int port);
int8_t transport_close(void);

#endif
