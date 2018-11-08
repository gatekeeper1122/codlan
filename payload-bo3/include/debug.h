#include "ps4.h"

int debugLogSocket;

void debugLogInit(unsigned int ip, uint16_t port);
void debugPrintHex(void* input, int length, int relative);
void debugLogTerm(void);

#define debugPrint(format, ...)\
	do {\
		char buffer[512];\
		int size = sprintf(buffer, format, ##__VA_ARGS__);\
		sceNetSend(debugLogSocket, buffer, size, 0);\
	} while(0)

#define debugHex(buffer, size)\
	do {\
		sceNetSend(debugLogSocket, buffer, size, 0);\
	} while(0)
