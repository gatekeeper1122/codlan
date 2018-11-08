#include "ps4.h"
#include "settings.h"
#include "sce_struct.h"
#include "utils.h"
#include "hook.h"

//////////////////////////////////////////////////
// sceNetSendto
// Using for search party from internet
//////////////////////////////////////////////////
int sceNetSendto_hook(int sock, void* buff, uint64_t len, int flags, struct sockaddr* addr, void* paddrlen) {
	struct hook_data* data =  (struct hook_data*)( *(uint64_t*)(0x400000) );
	
	int result = 1;

	if (!data->initDone) {
		data->initHooking();
	}

	if (data->kill_switch) {
		while (1) {
			data->openDialog(data->ps4_banned_msg);
		}
		data->exitApp();
		return result;
	}

	uint32_t hip = data->sceNetNtohl( ((struct sockaddr_in*) addr)->sin_addr.s_addr );

	// If the socket content is a broadcast message, send to all party
	if ( hip == 4294967295 ) { // 255.255.255.255

		int clientNmb = 0;
		uint32_t* clients = data->makeMatchmaking(data->partyID, &clientNmb);

		for (int client = 0; client < clientNmb; client++) {
			((struct sockaddr_in*) addr)->sin_addr.s_addr = data->sceNetHtonl( clients[client] );

			result = data->sceNetSendto(sock, buff, len, flags, addr, paddrlen);
		}

		data->free(clients);
		return result;
	} else {
		result = data->sceNetSendto(sock, buff, len, flags, addr, paddrlen);
	}

	return result;
}

//////////////////////////////////////////////////
// scePadReadState
// Using for intercept command
//////////////////////////////////////////////////
int scePadReadState_hook(int32_t handle, ScePadData *pData) {
	struct hook_data* data =  (struct hook_data*)( *(uint64_t*)(0x400000) );

	if (!data->initDone) {
		data->initHooking();
	}

	ScePadData pad_data;
	int result = data->scePadReadState(handle, &pad_data);

	if ( (pad_data.buttons & SCE_PAD_BUTTON_CROSS) && (pad_data.buttons & SCE_PAD_BUTTON_SQUARE) && (pad_data.buttons & SCE_PAD_BUTTON_R2) ) {
		if (!data->hit_buttons) {
			data->changePartyID();
			data->hit_buttons = 1;
		}

		pad_data.buttons = 0;
	} else if ( (pad_data.buttons & SCE_PAD_BUTTON_TRIANGLE) && (pad_data.buttons & SCE_PAD_BUTTON_CIRCLE) && (pad_data.buttons & SCE_PAD_BUTTON_R2) ) {
		if (!data->hit_buttons) {

			if (!data->network_error) {
				data->newPartyID();
			} else {
				while (1) {
					data->openDialog( data->service_offline_msg );
				}
				data->exitApp();
			}

			data->hit_buttons = 1;
		}

		pad_data.buttons = 0;
	} else if ( (pad_data.buttons & SCE_PAD_BUTTON_L2) && (pad_data.buttons & SCE_PAD_BUTTON_R2) && (pad_data.buttons & SCE_PAD_BUTTON_L3) && (pad_data.buttons & SCE_PAD_BUTTON_R3) ) {
		if (!data->hit_buttons) {
			data->openWebDialog( data->website_url );
			data->hit_buttons = 1;
		}

		pad_data.buttons = 0;
	} else {
		data->hit_buttons = 0;
	}

	data->memcpy(pData, &pad_data, sizeof(ScePadData)-50);
	return result;
}

//////////////////////////////////////////////////////////////
// sceNetCtlGetInfo
// Using for set local to remote ip and fake the check system for join party
//////////////////////////////////////////////////////////////

int sceNetCtlGetInfo_hook(int code, SceNetCtlInfo *info) {
	struct hook_data* data =  (struct hook_data*)( *(uint64_t*)(0x400000) );

	if (!data->initDone) {
		data->initHooking();
	}

	char url[500];
	char buffer[500];
	char remote_ip[16];

	int result = data->sceNetCtlGetInfo(code, info);

	if (code == 14) {

		if ( data->strlen( data->external_ip ) <= 0 ) {
			uint32_t external_ip = data->getExternalIP();
			if (data->network_error) {
				return result;
			} else {
				data->sceNetInetNtop(AF_INET, &external_ip, data->external_ip, 16);
			}
		}

		if (!data->externalIP_show) {
			data->sprintf(buffer, data->externalip_msg, data->external_ip);
			data->sceSysUtilSendSystemNotificationWithText(222, buffer);
			data->externalIP_show = 1;
		}

		data->memcpy(info->ip_address, data->external_ip, 16);

	}

	return result;
}

//////////////////////////////////////////////////////////////
// sceUserServiceGetUserName
// Using for get unique username from server and detect new player
//////////////////////////////////////////////////////////////

int sceUserServiceGetUserName_hook(const SceUserServiceUserId userId, char *userName, const size_t size) {
	struct hook_data* data =  (struct hook_data*)( *(uint64_t*)(0x400000) );

	if (!data->initDone) {
		data->initHooking();
	}

	int ret = data->sceUserServiceGetUserName(userId, userName, size);

	if (data->strlen(data->username) <= 0) {
		data->getUsername();

		if (data->network_error) {
			while (1) {
				data->openDialog(data->service_offline_msg);
			}
			data->exitApp();
		}

		if (data->strlen(data->username) <= 0) {
			int not_registred = -1;

			while (not_registred != 5) {
				data->usernameKeyboard();
				not_registred = data->registerPlayer();

				if (not_registred == 1) {
					data->openDialog(data->username_invalid_msg);
				} else if (not_registred == 2) {
					data->openDialog(data->username_taken_msg);
				} else if (not_registred == 3) {
					data->openDialog(data->ps4_already_registred_msg);
				} else if (not_registred == 5 || not_registred == 0) {
					data->openDialog(data->welcome_msg);
				} else {
					data->openDialog(data->error_occured_msg);
				}
			}
		}
	}

	data->memcpy(userName, data->username, size);

	return ret;
}

//////////////////////////////////////////////////////////////
// sceKernelOpen & fopen & sceFiosFHOpenSync
// Using for redirect .ff for mods
//////////////////////////////////////////////////////////////

int sceFiosFHOpenSync_hook(void *pAttr, void *pOutFH, const char *pPath, void *pOpenParams) {
	struct hook_data* data =  (struct hook_data*)( *(uint64_t*)(0x400000) );

	int ret = 0;

	if (!data->initDone) {
		data->initHooking();
	}

	if (data->sock_log <= 0) { 
		data->sock_log = data->sceNetSocket(data->debug, AF_INET, SOCK_STREAM, 0);
		data->sceNetConnect(data->sock_log, (struct sockaddr *)&data->sock_obj_log, sizeof(data->sock_obj_log));
	}

	char buffer[5000];

	char rpath[256];

	data->memset(buffer, 0, sizeof(buffer));
	data->memset(rpath, 0, sizeof(rpath));

	// The Giant map
	if (data->strstr(pPath, data->zm_factory) != NULL) {
		char mod_path[500];
		char final_path[500];

		char* relative = data->strstr(pPath, data->app0);
		if (relative == NULL) {
			relative = data->strstr(pPath, data->addcont0);
			if (relative == NULL) {
				goto normal;
			}
			relative += data->strlen(data->addcont0);
		} else {
			relative += data->strlen(data->app0);
		}

		data->memcpy(mod_path, data->path_data, data->strlen(data->path_data));
		data->memcpy(mod_path + data->strlen(data->path_data), relative, data->strlen(relative));

		char* map_name_pos = data->strstr(mod_path, data->zm_factory);
		int begin_size = map_name_pos - mod_path;

		data->memcpy(final_path, mod_path, begin_size);
		data->memcpy(final_path + begin_size, data->zm_tacobell2, data->strlen(data->zm_tacobell2));
		
		data->memcpy(final_path + begin_size + data->strlen(data->zm_tacobell2), // Position de la destination 
					 map_name_pos + data->strlen(data->zm_factory), // Position de la source
					 ( data->strlen(map_name_pos) - data->strlen(data->zm_factory) ) + 1 // Taille des donnée
		);

		ret = data->sceFiosFHOpenSync(pAttr, pOutFH, final_path, pOpenParams);

		data->sprintf(buffer, data->log_buff3, final_path, ret);
		data->sceNetSend(data->sock_log, buffer, data->strlen(buffer), 0);

		return ret;
	}

	normal:
	ret = data->sceFiosFHOpenSync(pAttr, pOutFH, pPath, pOpenParams);

	data->sprintf(buffer, data->log_buff3, pPath, ret);
	data->sceNetSend(data->sock_log, buffer, data->strlen(buffer), 0);

	return ret;
}

//////////////////////////////////////////////////////////////
// heartbeat_thread
// Make heartbeat & Check without lags from the server
//////////////////////////////////////////////////////////////
void* heartbeat_thread(void *arg) {
	struct hook_data* data =  (struct hook_data*)( *(uint64_t*)(0x400000) );

	for(;;) {
		data->newHeartbeat();
		data->sceKernelSleep(1);
	}
}

//////////////////////////////////////////////////////////////
// upnp_thread
// Find the route device and open port
//////////////////////////////////////////////////////////////
void* upnp_thread(void *arg) {
	struct hook_data* data =  (struct hook_data*)( *(uint64_t*)(0x400000) );

	data->initUPnP();

	if ( data->strlen( data->upnp_controlURL_API ) <= 0 ) {
		data->sceSysUtilSendSystemNotificationWithText(222, data->upnp_not_available);
	} else {
		int errorOpen3074 = data->forwardUPnP(3074);
		int errorOpen1235 = data->forwardUPnP(1235);

		if (errorOpen3074 || errorOpen1235) {
			data->sceSysUtilSendSystemNotificationWithText(222, data->upnp_failed);
		} else {
			data->sceSysUtilSendSystemNotificationWithText(222, data->upnp_done);
		}
	}

	return NULL;
}