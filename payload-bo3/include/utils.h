#define BO3_100_VERSION_CHECK 0x48C9891974FFF983
#define BO3_126_VERSION_CHECK 0x2E6666666614EBC9
#define BO3_127_VERSION_CHECK 0xA5E8E7894C00001C

#define SYSCALL_BYTES 0x48, 0x89, 0xF8, 0x48, 0x89, 0xF7, 0x48, 0x89, 0xD6, 0x48, 0x89, 0xCA, 0x4D, 0x89, 0xC2, 0x4D, 0x89, 0xC8, 0x4C, 0x8B, 0x4C, 0x24, 0x08, 0x0F, 0x05, 0xC3

void initHooking(void);
void initUPnP(void);
int forwardUPnP(int port);
void newHeartbeat(void);
void getUsername(void);
void usernameKeyboard(void);
void openDialog(char* text);
void openWebDialog(char* url);
int registerPlayer(void);
uint32_t* makeMatchmaking(int partyID, int* clientNbr);
void changePartyID(void);
void newPartyID(void);
uint32_t getExternalIP(void);
void exitApp(void);
