#include "debug.h"

void debugLogInit(unsigned int ip, uint16_t port) {
    // Connect to server and send message
	char socketName[] = "debug_log";
	struct sockaddr_in server;

	server.sin_len = sizeof(server);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = ip;
	server.sin_port = sceNetHtons(port);
	memset(server.sin_zero, 0, sizeof(server.sin_zero));

	debugLogSocket = sceNetSocket(socketName, AF_INET, SOCK_STREAM, 0);
	sceNetConnect(debugLogSocket, (struct sockaddr *)&server, sizeof(server));
}

void debugLogTerm(void) {
    sceNetSocketClose(debugLogSocket);
}

void debugPrintHex(void* input, int length, int relative)
{
  uint8_t* buffer = (uint8_t*)input;
  int i;

  for (i = 0; i < length; i++)
  {
    if ((i % 16) == 0)
    {
      debugPrint("%llx :", !relative ? (uint64_t)&buffer[i] : (uint64_t)i);
    }

    debugPrint(" %02x", buffer[i]);

    if (i > 0 && (i % 16) == 15)
    {
      debugPrint("\n");
    }
  }

  if ((i % 16) != 0)
  {
    debugPrint("\n");
  }
}
