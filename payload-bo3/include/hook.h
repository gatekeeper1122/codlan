struct hook_data {
	///////////////////////////////
	// TOOLS
	///////////////////////////////

	uint64_t (*remote_syscall)(int num, ...);
	void (*initHooking)(void);
	void (*initUPnP)(void);
	int (*forwardUPnP)(int port);
	void (*newHeartbeat)(void);
	void (*getUsername)(void);
	void (*usernameKeyboard)(void);
	void (*openDialog)(char* text);
	void (*openWebDialog)(char* url);
	int (*registerPlayer)(void);
	uint32_t* (*makeMatchmaking)(int partyID, int* clientNbr);
	void (*changePartyID)(void);
	void (*newPartyID)(void);
	uint32_t (*getExternalIP)(void);
	void (*exitApp)(void);

	void (*remote_escape)(struct thread* td);

	///////////////////////////////
	// Main Thread
	///////////////////////////////

	ScePthread upnp_thread_obj;
	ScePthread heartbeat_thread_obj;
	void* (*heartbeat_thread)(void *arg);
	void* (*upnp_thread)(void *arg);

	///////////////////////////////
	// SECURITY
	///////////////////////////////
	uint8_t ether_mac[SCE_NET_ETHER_ADDR_LEN];

	///////////////////////////////
	// LIBRARY & FUNCTION RELATED
	///////////////////////////////

	char library_name[20][50];
	char libscenet_name[15][50];
	char libscenetctl_name[15][50];
	char libscessl_name[15][50];
	char libscehttp_name[15][50];
	char libc_name[20][50];
	char libscepad_name[15][50];
	char libsceimedialog_name[15][50];
	char libkernel_name[15][50];
	char libsysutil_name[15][50];
	char libsceuserservice_name[15][50];
	char libscecommondialog_name[15][50];
	char libscemsgdialog_name[15][50];
	char libscewebbrowserdialog_name[15][50];
	char libscefios2_name[15][50];

	// libSceNet function
	int (*sceNetSocket)(const char *, int, int, int);
	int (*sceNetConnect)(int, struct sockaddr *, int);
	int (*sceNetSend)(int, const void *, size_t, int);
	int (*sceNetSendto)(int sock, void* buff, uint64_t len, int flags, void* addr, void* paddrlen);
	int (*sceNetInetNtop)(int af, const void *src, char *dst, int size);
	uint16_t (*sceNetNtohs)(uint16_t net16);
	uint32_t (*sceNetNtohl)(uint32_t net32);
	uint32_t (*sceNetHtonl)(uint32_t host32);
	int (*sceNetPoolCreate)(const char *name, int size, int flags);
	int (*sceNetRecvfrom)(int sock, void* buff, uint64_t len, int flags, void* addr, void* paddrlen);
	int (*sceNetSetsockopt)(int s, int level, int optname, const void *optval, uint32_t optlen);
	
	// libSceNetCtl function
	int (*sceNetCtlGetInfo)(int code, SceNetCtlInfo *info);

	// libSceHttp function
	int (*sceHttpInit)(int libnetMemId, int libsslCtxId, size_t poolSize);
	int (*sceHttpCreateTemplate)(int libhttpCtxId, const char*userAgent, int httpVer, int autoProxyConf);
	int (*sceHttpCreateConnectionWithURL)(int tmplId, const char *url, bool enableKeepalive);
	int (*sceHttpCreateRequestWithURL)(int connId, int32_t method, const char *url, uint64_t contentLength);
	int (*sceHttpSendRequest)(int reqId, const void *postData, size_t size);
	int (*sceHttpDeleteRequest)(int reqId);
	int (*sceHttpDeleteTemplate)(int templateId);
	int (*sceHttpDeleteConnection)(int connId);
	int (*sceHttpReadData)(int reqId, void *data, unsigned int size);
	int (*sceHttpAddRequestHeader)(int id, const char *name, const char *value, int32_t mode);
	int (*sceHttpSetRequestContentLength)(int id, uint64_t contentLength);
	int (*sceHttpSetConnectTimeOut)(int id, uint32_t usec);

	// libSceSsl function
	int (*sceSslInit)(size_t poolSize);

	// libc function
	uint64_t (*strlen)(const char * str);
	int (*sprintf)(char* str, const char * format, ...);
	void* (*memcpy)(void* destination, const void * source, size_t num);
	void* (*memset)(void *b, int c, size_t len);
	size_t (*mbstowcs)(wchar_t* dest, const char* src, size_t max);
	size_t (*wcstombs)(char* dest, const wchar_t* src, size_t max);
	int (*atoi)(const char *nptr);
	int (*strncmp)( const char * str1, const char * str2, size_t num );
	char* (*strstr)(const char *haystack, const char *needle);
	void* (*malloc)(size_t size);
	void (*free)(void *ptr);
	void* (*realloc)(void *ptr, uint64_t size);
	void (*exit)(int status);
	int (*strcmp)(const char *s1, const char *s2);
	void* (*fopen)(const char *pathname, const char *mode);
	void* (*opendir)(const char *filename);
	struct dirent * (*readdir)(void *dirp);
    int (*closedir)(void *dirp);

	// libScePad function
	int (*scePadReadState)(int32_t handle, ScePadData *pData);

	// libSceImeDialog function
	int (*sceImeDialogGetResult)(SceImeDialogResult *result);
	int (*sceImeDialogInit)(const SceImeDialogParam *param, void* extended);
	SceImeDialogStatus (*sceImeDialogGetStatus)(void);
	int (*sceImeDialogTerm)(void);

	// dialog function
	int32_t (*sceCommonDialogInitialize)(void);
	int32_t (*sceMsgDialogInitialize)(void);
	int32_t (*sceMsgDialogOpen)(const SceMsgDialogParam *param);
	SceCommonDialogStatus (*sceMsgDialogUpdateStatus)(void);
	int32_t (*sceMsgDialogTerminate)(void);

	int32_t (*sceWebBrowserDialogInitialize)(void);
	int32_t (*sceWebBrowserDialogTerminate)(void);
	int32_t (*sceWebBrowserDialogOpen)(const SceWebBrowserDialogParam *param);
	SceCommonDialogStatus (*sceWebBrowserDialogUpdateStatus)(void);
	int32_t (*sceWebBrowserDialogSetCookie)(const SceWebBrowserDialogSetCookieParam *param);

	// libkernel function
	int (*sceKernelGettimeofday)(SceKernelTimeval *tp);
	int (*scePthreadCreate)(ScePthread *thread, const ScePthreadAttr *attr, void *(*entry) (void *), void *arg, const char *name);
	unsigned int (*sceKernelSleep)(unsigned int seconds);
	int (*getpid)(void);
	int (*kill)(int pid, int sig);
	int (*sceKernelOpen)(const char *path, int flags, int mode);

	// libsysutil function
	int (*sceSysUtilSendSystemNotificationWithText)(int messageType, char* message);
	
	// libsceuserservice function
	int32_t (*sceUserServiceGetInitialUser)(SceUserServiceUserId *userId);
	int32_t (*sceUserServiceGetUserName)(const SceUserServiceUserId userId, char *userName, const size_t size);

	// libscefios2
	int (*sceFiosFHOpenSync)(void *pAttr, void *pOutFH, const char *pPath, void *pOpenParams);

	///////////////////////////////
	// Network related
	///////////////////////////////

	// Http
	int poolID;
	int sslID;
	int httpID;
	char external_ip[16];

	// Matchmaking API
	char website_url[500];
	char base_url[500];
	char hb_url[500];
	char gc_url[500];
	char gei_url[500];
	char gun_url[500];
	char np_url[500];
	char new_url[500];

	char ok_status[10];
	char ban_status[10];
	char is_ps4_cookie[50];

	// UPnP Related
	char upnp_search[1000];
	char upnp_http[10];
	char upnp_rn[5];
	char upnp_WANIPConnection[20];
	char upnp_controlURL[20];
	char upnp_controlURL_end[20];
	char upnp_slash[10];
	char upnp_invalid_ip[10]; 
	char upnp_split[10];
	char upnp_controlURL_API[1000];
	char upnp_soap_body_pf[5000];
	char upnp_error[10];
	char SOAPAction[50];
	char SOAPAction_data[200];
	char ContentType[50];
	char ContentLength[50];
	char xml_mime[50];
	char upnp_desc[10];

	struct sockaddr_in sock_obj_upnp;
	struct sockaddr_in sock_obj_log;

	int sock_log;
	int libscefios2_id;

	///////////////////////////////
	// OTHERS
	///////////////////////////////

	char titleKeyboard[100];
	char usernameTitleKB[100];
	char ServerlistInfo[20];

	// State probe
	int hit_buttons;
	int initDone;
	int externalIP_show;
	int partyID;
	uint64_t heartbeat_time;
	int network_error;
	int kill_switch;

	char username[PLAYER_USERNAME_MAX_LEN+3];
	char log[10][500];
	char debug[10];
	char debug_int[10];
	char debug_hex[10];
	char debug_ptr[10];
	char party_changed[1000];
	char client_connected[1000];
	char upnp_not_available[1000];
	char upnp_failed[1000];
	char upnp_done[1000];
	char externalip_msg[1000];
	char internalip_msg[1000];
	char username_taken_msg[1000];
	char username_invalid_msg[1000];
	char ps4_already_registred_msg[1000];
	char ps4_banned_msg[1000];
	char service_offline_msg[1000];
	char error_occured_msg[1000];
	char welcome_msg[1000];
	char beta_warning[1000];
	char zm_factory[50];
	char zm_tacobell2[50];
	char app0[50];
	char addcont0[50];
	char mods[50];
	char log_buff[1000];
	char log_buff2[1000];
	char log_buff3[1000];
	char log_buff4[1000];
	char path1[200];
	char path2[200];
	char path_root[100];
	char path_data[100];
	char rb[10];
};

int sceNetSendto_hook(int sock, void* buff, uint64_t len, int flags, struct sockaddr* addr, void* paddrlen);
int scePadReadState_hook(int32_t handle, ScePadData *pData);
int sceNetCtlGetInfo_hook(int code, SceNetCtlInfo *info);
int sceUserServiceGetUserName_hook(const SceUserServiceUserId userId, char *userName, const size_t size);
int sceFiosFHOpenSync_hook(void *pAttr, void *pOutFH, const char *pPath, void *pOpenParams);
void* upnp_thread(void *arg);
void* heartbeat_thread(void *arg);
