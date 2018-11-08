#include "ps4.h"
#include "settings.h"
#include "sce_struct.h"
#include "hook.h"
#include "freebsd.h"
#include "utils.h"

//////////////////////////////////////////////////
// initHooking
// Initialise all function and some settings
//////////////////////////////////////////////////

void initHooking(void) {
	struct hook_data* data = (struct hook_data*)( *(uint64_t*)(0x400000) );

	// Escape from sandbox
	data->remote_syscall(11, data->remote_escape);

	int libc_id, libscenet_id, libscepad_id, libsceime_id, libsceimedialog_id, libscehttp_id, libscessl_id, libkernel_id, libsysutil_id, libscenetctl_id, libsceuserservice_id, libscecommondialog_id, libscemsgdialog_id, libscewebbrowserdialog_id, libscefios2_id = 0;

	// (Re)Loading needed SPRX (Library)
	data->remote_syscall(594, data->library_name[0], 0, &libc_id, 0);
	data->remote_syscall(594, data->library_name[1], 0, &libscenet_id, 0);
	data->remote_syscall(594, data->library_name[2], 0, &libscepad_id, 0);
	data->remote_syscall(594, data->library_name[3], 0, &libsceime_id, 0);
	data->remote_syscall(594, data->library_name[4], 0, &libsceimedialog_id, 0);
	data->remote_syscall(594, data->library_name[5], 0, &libscehttp_id, 0);
	data->remote_syscall(594, data->library_name[6], 0, &libkernel_id, 0);
	data->remote_syscall(594, data->library_name[7], 0, &libscessl_id, 0);
	data->remote_syscall(594, data->library_name[8], 0, &libsysutil_id, 0);
	data->remote_syscall(594, data->library_name[9], 0, &libscenetctl_id, 0);
	data->remote_syscall(594, data->library_name[10], 0, &libsceuserservice_id, 0);
	data->remote_syscall(594, data->library_name[11], 0, &libscecommondialog_id, 0);
	data->remote_syscall(594, data->library_name[12], 0, &libscemsgdialog_id, 0);
	data->remote_syscall(594, data->library_name[13], 0, &libscewebbrowserdialog_id, 0);
	data->remote_syscall(594, data->library_name[14], 0, &libscefios2_id, 0);

	data->libscefios2_id = libscefios2_id;

	do {
		data->remote_syscall(591, libscenet_id, data->libscenet_name[0], &data->sceNetSocket);
		data->remote_syscall(591, libscenet_id, data->libscenet_name[1], &data->sceNetConnect);
		data->remote_syscall(591, libscenet_id, data->libscenet_name[2], &data->sceNetSend);
		data->remote_syscall(591, libscenet_id, data->libscenet_name[3], &data->sceNetSendto);
		data->remote_syscall(591, libscenet_id, data->libscenet_name[4], &data->sceNetInetNtop);
		data->remote_syscall(591, libscenet_id, data->libscenet_name[5], &data->sceNetNtohs);
		data->remote_syscall(591, libscenet_id, data->libscenet_name[6], &data->sceNetNtohl);
		data->remote_syscall(591, libscenet_id, data->libscenet_name[7], &data->sceNetHtonl);
		data->remote_syscall(591, libscenet_id, data->libscenet_name[8], &data->sceNetPoolCreate);
		data->remote_syscall(591, libscenet_id, data->libscenet_name[9], &data->sceNetRecvfrom);
		data->remote_syscall(591, libscenet_id, data->libscenet_name[10], &data->sceNetSetsockopt);

		data->remote_syscall(591, libscenetctl_id, data->libscenetctl_name[0], &data->sceNetCtlGetInfo);

		data->remote_syscall(591, libc_id, data->libc_name[0], &data->strlen);
		data->remote_syscall(591, libc_id, data->libc_name[1], &data->sprintf);
		data->remote_syscall(591, libc_id, data->libc_name[2], &data->memcpy);
		data->remote_syscall(591, libc_id, data->libc_name[3], &data->memset);
		data->remote_syscall(591, libc_id, data->libc_name[4], &data->mbstowcs);
		data->remote_syscall(591, libc_id, data->libc_name[5], &data->wcstombs);
		data->remote_syscall(591, libc_id, data->libc_name[6], &data->atoi);
		data->remote_syscall(591, libc_id, data->libc_name[7], &data->strncmp);
		data->remote_syscall(591, libc_id, data->libc_name[8], &data->strstr);
		data->remote_syscall(591, libc_id, data->libc_name[9], &data->malloc);
		data->remote_syscall(591, libc_id, data->libc_name[10], &data->realloc);
		data->remote_syscall(591, libc_id, data->libc_name[11], &data->free);
		data->remote_syscall(591, libc_id, data->libc_name[12], &data->exit);
		data->remote_syscall(591, libc_id, data->libc_name[13], &data->strcmp);
		data->remote_syscall(591, libc_id, data->libc_name[14], &data->fopen);
		data->remote_syscall(591, libc_id, data->libc_name[15], &data->opendir);
		data->remote_syscall(591, libc_id, data->libc_name[16], &data->readdir);
		data->remote_syscall(591, libc_id, data->libc_name[17], &data->closedir);

		data->remote_syscall(591, libscepad_id, data->libscepad_name[0], &data->scePadReadState);
	
		data->remote_syscall(591, libsceimedialog_id, data->libsceimedialog_name[0], &data->sceImeDialogInit);
		data->remote_syscall(591, libsceimedialog_id, data->libsceimedialog_name[1], &data->sceImeDialogGetStatus);
		data->remote_syscall(591, libsceimedialog_id, data->libsceimedialog_name[2], &data->sceImeDialogTerm);
		data->remote_syscall(591, libsceimedialog_id, data->libsceimedialog_name[3], &data->sceImeDialogGetResult);

		data->remote_syscall(591, libscehttp_id, data->libscehttp_name[0], &data->sceHttpInit);
		data->remote_syscall(591, libscehttp_id, data->libscehttp_name[1], &data->sceHttpCreateTemplate);
		data->remote_syscall(591, libscehttp_id, data->libscehttp_name[2], &data->sceHttpCreateConnectionWithURL);
		data->remote_syscall(591, libscehttp_id, data->libscehttp_name[3], &data->sceHttpCreateRequestWithURL);
		data->remote_syscall(591, libscehttp_id, data->libscehttp_name[4], &data->sceHttpSendRequest);
		data->remote_syscall(591, libscehttp_id, data->libscehttp_name[5], &data->sceHttpDeleteRequest);
		data->remote_syscall(591, libscehttp_id, data->libscehttp_name[6], &data->sceHttpDeleteTemplate);
		data->remote_syscall(591, libscehttp_id, data->libscehttp_name[7], &data->sceHttpDeleteConnection);
		data->remote_syscall(591, libscehttp_id, data->libscehttp_name[8], &data->sceHttpReadData);
		data->remote_syscall(591, libscehttp_id, data->libscehttp_name[9], &data->sceHttpAddRequestHeader);
		data->remote_syscall(591, libscehttp_id, data->libscehttp_name[10], &data->sceHttpSetRequestContentLength);

		data->remote_syscall(591, libscessl_id, data->libscessl_name[0], &data->sceSslInit);

		data->remote_syscall(591, libkernel_id, data->libkernel_name[0], &data->sceKernelGettimeofday);
		data->remote_syscall(591, libkernel_id, data->libkernel_name[1], &data->scePthreadCreate);
		data->remote_syscall(591, libkernel_id, data->libkernel_name[2], &data->sceKernelSleep);
		data->remote_syscall(591, libkernel_id, data->libkernel_name[3], &data->getpid);
		data->remote_syscall(591, libkernel_id, data->libkernel_name[4], &data->kill);
		data->remote_syscall(591, libkernel_id, data->libkernel_name[5], &data->sceKernelOpen);

		data->remote_syscall(591, libsysutil_id, data->libsysutil_name[0], &data->sceSysUtilSendSystemNotificationWithText);
	
		data->remote_syscall(591, libsceuserservice_id, data->libsceuserservice_name[0], &data->sceUserServiceGetInitialUser);
		data->remote_syscall(591, libsceuserservice_id, data->libsceuserservice_name[1], &data->sceUserServiceGetUserName);
			
		data->remote_syscall(591, libscecommondialog_id, data->libscecommondialog_name[0], &data->sceCommonDialogInitialize);

		data->remote_syscall(591, libscemsgdialog_id, data->libscemsgdialog_name[0], &data->sceMsgDialogInitialize);
		data->remote_syscall(591, libscemsgdialog_id, data->libscemsgdialog_name[1], &data->sceMsgDialogOpen);
		data->remote_syscall(591, libscemsgdialog_id, data->libscemsgdialog_name[2], &data->sceMsgDialogUpdateStatus);
		data->remote_syscall(591, libscemsgdialog_id, data->libscemsgdialog_name[3], &data->sceMsgDialogTerminate);

		data->remote_syscall(591, libscewebbrowserdialog_id, data->libscewebbrowserdialog_name[0], &data->sceWebBrowserDialogInitialize);
		data->remote_syscall(591, libscewebbrowserdialog_id, data->libscewebbrowserdialog_name[1], &data->sceWebBrowserDialogOpen);
		data->remote_syscall(591, libscewebbrowserdialog_id, data->libscewebbrowserdialog_name[2], &data->sceWebBrowserDialogTerminate);
		data->remote_syscall(591, libscewebbrowserdialog_id, data->libscewebbrowserdialog_name[3], &data->sceWebBrowserDialogUpdateStatus);
		data->remote_syscall(591, libscewebbrowserdialog_id, data->libscewebbrowserdialog_name[4], &data->sceWebBrowserDialogSetCookie);
	
		//data->remote_syscall(591, libscefios2_id, data->libscefios2_name[0], &data->sceFiosFHOpenSync);
	} while (data->sceNetSocket == NULL);

	data->poolID = data->sceNetPoolCreate(data->debug, 4096, 0);
	data->sslID = data->sceSslInit(SSL_POOLSIZE);
	data->httpID = data->sceHttpInit(data->poolID, data->sslID, LIBHTTP_POOLSIZE);

	data->sceCommonDialogInitialize();

	/*
	if (data->sock_log <= 0) { 
		data->sock_log = data->sceNetSocket(data->debug, AF_INET, SOCK_STREAM, 0);
		data->sceNetConnect(data->sock_log, (struct sockaddr *)&data->sock_obj_log, sizeof(data->sock_obj_log));
	}
	*/

	data->scePthreadCreate(&data->upnp_thread_obj, NULL, data->upnp_thread, NULL, data->debug);
	data->scePthreadCreate(&data->heartbeat_thread_obj, NULL, data->heartbeat_thread, NULL, data->debug);
	data->initDone = 1;
}

//////////////////////////////////////////////////
// initUPnP
// Initialise UPnP and find the ControlURL
//////////////////////////////////////////////////
void initUPnP(void) {
	struct hook_data* data =  (struct hook_data*)( *(uint64_t*)(0x400000) );

	char buffer[2000];

	// Create the UPnP socket and broadcast the message
	int upnp_sock = data->sceNetSocket(data->debug, AF_INET, SOCK_DGRAM, 0);
	data->sceNetSendto(upnp_sock, data->upnp_search, data->strlen(data->upnp_search)+1, 0, (struct sockaddr*)&data->sock_obj_upnp, sizeof(data->sock_obj_upnp));
	
	// Set the timeout
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 10000000; // 10s

	data->sceNetSetsockopt(upnp_sock, SOL_SOCKET, 0x1006, &tv, sizeof(tv));

	// Find any device interesting in the network
	char devices_url[UPNP_MAX_DEVICE][500];
	data->memset(devices_url, 0, 500*UPNP_MAX_DEVICE);

	for (int i = 0; i < UPNP_MAX_DEVICE; i++) {
		char resp[1024];
		data->memset(resp, 0, sizeof(resp));

		data->sceNetRecvfrom(upnp_sock, resp, sizeof(resp), 0, NULL, NULL);

		char* pos1 = data->strstr(resp, data->upnp_http);
		char* pos2 = data->strstr(pos1, data->upnp_rn);
		uint64_t size = pos2 - pos1;

		if (pos1 != NULL && pos2 != NULL && size > 0 && size < 500) {
			// Check if the url already exist
			int already_exist = 0;
			for (int k = 0; k < UPNP_MAX_DEVICE; k++) {
				if (data->strncmp(devices_url[k], pos1, size) == 0) {
					already_exist = 1;
				}
			}

			if (!already_exist) {
				data->memcpy(devices_url[i], pos1, size);
			}
		}
	}

	int done = 0;
	for (int i = 0; i < UPNP_MAX_DEVICE; i++) {
		if (data->strlen(devices_url[i]) > 15 && data->strstr(devices_url[i], data->upnp_invalid_ip) == NULL) {
			// Create request to device
			int tmplId = data->sceHttpCreateTemplate(data->httpID, data->debug, 1, 0);
			int connId = data->sceHttpCreateConnectionWithURL(tmplId, devices_url[i], 1);

			int reqId = data->sceHttpCreateRequestWithURL(connId, SCE_HTTP_METHOD_GET, devices_url[i], 0);

			int ret = data->sceHttpSendRequest(reqId, NULL, 0);

			if (ret == 0) {
				char* xml = data->malloc(50000);
				data->memset(xml, 0, 50000);

				int sizeData = data->sceHttpReadData(reqId, xml, 50000);

				char* WANIP = data->strstr(xml, data->upnp_WANIPConnection);

				if (WANIP != NULL) {
					char* controlURL_begin = data->strstr(WANIP, data->upnp_controlURL);
					controlURL_begin += data->strlen(data->upnp_controlURL);

					char* controlURL_end = data->strstr(controlURL_begin, data->upnp_controlURL_end);
					uint64_t size = controlURL_end - controlURL_begin;

					if (controlURL_begin && controlURL_end && size > 0) {
						char controlURL[500];
						char base_url[100];

						data->memset(controlURL, 0, sizeof(controlURL));
						data->memset(base_url, 0, sizeof(base_url));

						data->memcpy(controlURL, controlURL_begin, size);

						char* base_url_end = data->strstr(devices_url[i] + 0x7, data->upnp_slash);
						uint64_t base_url_size = base_url_end - devices_url[i];
						data->memcpy(base_url, devices_url[i], base_url_size);

						data->sprintf(data->upnp_controlURL_API, data->upnp_split, base_url, controlURL);
						done = 1;
					}

				}

				data->free(xml);
			}

			data->sceHttpDeleteRequest(reqId);
			data->sceHttpDeleteConnection(connId);
			data->sceHttpDeleteTemplate(tmplId);

			if (done) {
				break;
			}
		}
	}
}

//////////////////////////////////////////////////
// forwardUPnP
// Enable Port Forward via UPnP
//////////////////////////////////////////////////
int forwardUPnP(int port) {
	struct hook_data* data =  (struct hook_data*)( *(uint64_t*)(0x400000) );

	if ( data->strlen( data->upnp_controlURL_API ) <= 0 ) {
		return 1;
	}

	int error = 0;

	SceNetCtlInfo info;
	data->sceNetCtlGetInfo(14, &info);

	char* body = data->malloc(1000);
	char* resp_body = data->malloc(1000);

	data->sprintf(body, data->upnp_soap_body_pf, port, port, info.ip_address, data->upnp_desc);

	// Create request to router
	int tmplId = data->sceHttpCreateTemplate(data->httpID, data->debug, 1, 0);
	int connId = data->sceHttpCreateConnectionWithURL(tmplId, data->upnp_controlURL_API, 1);
	int reqId = data->sceHttpCreateRequestWithURL(connId, SCE_HTTP_METHOD_POST, data->upnp_controlURL_API, data->strlen(body) );

	data->sceHttpAddRequestHeader(reqId, data->SOAPAction, data->SOAPAction_data, SCE_HTTP_HEADER_OVERWRITE);
	data->sceHttpAddRequestHeader(reqId, data->ContentType, data->xml_mime, SCE_HTTP_HEADER_OVERWRITE);

	data->sceHttpSetRequestContentLength(reqId, data->strlen(body) );

	int ret = data->sceHttpSendRequest(reqId, body, data->strlen(body) );
	int sizeData = data->sceHttpReadData(reqId, resp_body, 1000);

	if ( ret < 0 || sizeData <= 0 || data->strstr(resp_body, data->upnp_error) != NULL) {
		error = 1;
	}

	data->free(resp_body);
	data->free(body);

	data->sceHttpDeleteRequest(reqId);
	data->sceHttpDeleteConnection(connId);
	data->sceHttpDeleteTemplate(tmplId);

	return error;
}

//////////////////////////////////////////////////
// newHeartbeat
// Make a heartbeat every X seconds
//////////////////////////////////////////////////
void newHeartbeat(void) {
	struct hook_data* data =  (struct hook_data*)( *(uint64_t*)(0x400000) );

	char url[500];

	SceKernelTimeval tv;
	data->sceKernelGettimeofday( &tv );

	// Heartbeat system
	if (data->heartbeat_time+HEARTBEAT_TIMEOUT < tv.tv_sec) {
		// Create request
		data->sprintf(url, data->hb_url, data->base_url, data->partyID);

		int tmplId = data->sceHttpCreateTemplate(data->httpID, data->debug, 1, 0);
		int connId = data->sceHttpCreateConnectionWithURL(tmplId, url, 1);
		int reqId = data->sceHttpCreateRequestWithURL(connId, SCE_HTTP_METHOD_POST, url, 0);
		
		data->sceHttpSetRequestContentLength(reqId, SCE_NET_ETHER_ADDR_LEN);
		int ret = data->sceHttpSendRequest(reqId, data->ether_mac, SCE_NET_ETHER_ADDR_LEN);

		if (ret < 0) {
			data->network_error = 1;
		} else {
			char status[50];
			data->memset(status, 0, sizeof(status));
			data->sceHttpReadData(reqId, status, 49);

			if ( data->strcmp(status, data->ban_status) == 0 ) {
				data->kill_switch = 1;
			}
		}

		data->sceHttpDeleteRequest(reqId);
		data->sceHttpDeleteConnection(connId);
		data->sceHttpDeleteTemplate(tmplId);

		data->heartbeat_time = tv.tv_sec;
	}
}

//////////////////////////////////////////////////
// changePartyID
// Open the Keyboard for change the PartyID
//////////////////////////////////////////////////
void changePartyID(void) {
	struct hook_data* data =  (struct hook_data*)( *(uint64_t*)(0x400000) );

	if (data->network_error == 1) {
		while (1) {
			data->openDialog(data->service_offline_msg);
		}
		data->exitApp();
		return;
	}

	char buffer[1000];
	wchar_t title[100];
	wchar_t inputTextBuffer[100];

	char partyID_orig[PARTY_ID_LEN+1];
	data->sprintf(partyID_orig, data->debug_int, data->partyID);

	data->mbstowcs(inputTextBuffer, partyID_orig, data->strlen(partyID_orig)+1);
	data->mbstowcs(title, data->titleKeyboard, data->strlen(data->titleKeyboard)+1);

	SceImeDialogParam param;
	data->memset(&param, 0, sizeof(SceImeDialogParam));

	param.maxTextLength = PARTY_ID_LEN;
	param.inputTextBuffer = inputTextBuffer;
	param.title = title;
	param.userId = SCE_USER_SERVICE_USER_ID_EVERYONE;
	param.type = SCE_IME_TYPE_NUMBER;
	param.enterLabel = SCE_IME_ENTER_LABEL_GO;

	int init_log = data->sceImeDialogInit( &param, NULL );

	int keyboardRunning = 1;
	while (keyboardRunning) {
		int status = data->sceImeDialogGetStatus();

		if (status == SCE_IME_DIALOG_STATUS_FINISHED) {
			SceImeDialogResult result;
			data->memset(&result, 0, sizeof(SceImeDialogResult));

			data->sceImeDialogGetResult(&result);

			if (result.endstatus == SCE_IME_DIALOG_END_STATUS_OK) {
				char partyID[PARTY_ID_LEN+1];
				data->wcstombs(partyID, inputTextBuffer, PARTY_ID_LEN);
				data->partyID = data->atoi(partyID);
				data->heartbeat_time = 0;

				data->sprintf(buffer, data->party_changed, data->partyID);
				data->sceSysUtilSendSystemNotificationWithText(222, buffer);
			}

			keyboardRunning = 0;
			data->sceImeDialogTerm();
		} else if (status == SCE_IME_DIALOG_STATUS_NONE) {
			keyboardRunning = 0;
		}
	}
}

//////////////////////////////////////////////////
// getUsername
// Get the username from the server
//////////////////////////////////////////////////
void getUsername(void) {
	struct hook_data* data =  (struct hook_data*)( *(uint64_t*)(0x400000) );

	// Create request
	char url[500];
	data->sprintf(url, data->gun_url, data->base_url);

	int tmplId = data->sceHttpCreateTemplate(data->httpID, data->debug, 1, 0);
	int connId = data->sceHttpCreateConnectionWithURL(tmplId, url, 1);
	int reqId = data->sceHttpCreateRequestWithURL(connId, SCE_HTTP_METHOD_POST, url, 0);

	data->sceHttpSetRequestContentLength(reqId, SCE_NET_ETHER_ADDR_LEN);
	int ret = data->sceHttpSendRequest(reqId, data->ether_mac, SCE_NET_ETHER_ADDR_LEN);
	if (ret < 0) {
		data->network_error = 1;
	}

	data->sceHttpReadData(reqId, data->username, PLAYER_USERNAME_MAX_LEN+2);

	data->sceHttpDeleteRequest(reqId);
	data->sceHttpDeleteConnection(connId);
	data->sceHttpDeleteTemplate(tmplId);
}

//////////////////////////////////////////////////
// usernameKeyboard
// Edit the username via keyboard
//////////////////////////////////////////////////
void usernameKeyboard(void) {
	struct hook_data* data =  (struct hook_data*)( *(uint64_t*)(0x400000) );

	char username[PLAYER_USERNAME_MAX_LEN+1];

	char buffer[1000];
	wchar_t title[100];
	wchar_t inputTextBuffer[100];

	data->mbstowcs(inputTextBuffer, data->username, data->strlen(data->username)+1);
	data->mbstowcs(title, data->usernameTitleKB, data->strlen(data->usernameTitleKB)+1);

	SceImeDialogParam param;
	data->memset(&param, 0, sizeof(SceImeDialogParam));

	param.maxTextLength = PLAYER_USERNAME_MAX_LEN;
	param.inputTextBuffer = inputTextBuffer;
	param.title = title;
	param.userId = SCE_USER_SERVICE_USER_ID_EVERYONE;
	param.type = SCE_IME_TYPE_BASIC_LATIN;
	param.enterLabel = SCE_IME_ENTER_LABEL_GO;

	int init_log = data->sceImeDialogInit( &param, NULL );

	int keyboardRunning = 1;
	while (keyboardRunning) {
		int status = data->sceImeDialogGetStatus();

		if (status == SCE_IME_DIALOG_STATUS_FINISHED) {
			SceImeDialogResult result;
			data->memset(&result, 0, sizeof(SceImeDialogResult));

			data->sceImeDialogGetResult(&result);

			if (result.endstatus == SCE_IME_DIALOG_END_STATUS_OK) {
				data->wcstombs(username, inputTextBuffer, PLAYER_USERNAME_MAX_LEN);
				data->memcpy(data->username, username, PLAYER_USERNAME_MAX_LEN);
			}

			keyboardRunning = 0;
			data->sceImeDialogTerm();
		} else if (status == SCE_IME_DIALOG_STATUS_NONE) {
			keyboardRunning = 0;
		}
	}
}

//////////////////////////////////////////////////
// openDialog
// Open a Dialog Box with text inside
//////////////////////////////////////////////////
void openDialog(char* text) {
	struct hook_data* data =  (struct hook_data*)( *(uint64_t*)(0x400000) );

	data->sceMsgDialogInitialize();

	SceMsgDialogParam param;

	// Setting some shitty params
	data->memset( &param, 0x0, sizeof(SceMsgDialogParam) );
	data->memset( &param.baseParam, 0x0, sizeof(SceCommonDialogBaseParam) );
	param.baseParam.magic = (uint32_t)( SCE_COMMON_DIALOG_MAGIC_NUMBER + (uint64_t)&param.baseParam );
	param.baseParam.size = (uint32_t)sizeof(SceCommonDialogBaseParam);
	param.size = sizeof(SceMsgDialogParam);

	param.mode = SCE_MSG_DIALOG_MODE_USER_MSG;

	SceMsgDialogUserMessageParam userMsgParam;
	data->memset( &userMsgParam, 0, sizeof(userMsgParam) );
	userMsgParam.msg = text;
	userMsgParam.buttonType = SCE_MSG_DIALOG_BUTTON_TYPE_OK;
	param.userMsgParam = &userMsgParam;

	data->sceMsgDialogOpen( &param );

	SceCommonDialogStatus stat;
	while(1) {
		stat = data->sceMsgDialogUpdateStatus();
		if( stat == SCE_COMMON_DIALOG_STATUS_FINISHED ) {
			data->sceMsgDialogTerminate();
			break;
		}
	}
}

//////////////////////////////////////////////////
// openWebDialog
// Open website on a dialog box
//////////////////////////////////////////////////
void openWebDialog(char* url) {
	struct hook_data* data = (struct hook_data*)( *(uint64_t*)(0x400000) );
	data->sceWebBrowserDialogInitialize();


	SceWebBrowserDialogSetCookieParam cookie_param;
	data->memset( &cookie_param, 0, sizeof(cookie_param) );
	cookie_param.size = sizeof(cookie_param);
	cookie_param.url = url;
	cookie_param.cookie = data->is_ps4_cookie;

	data->sceWebBrowserDialogSetCookie( &cookie_param );

	SceWebBrowserDialogParam param;
	
	// Setting some shitty params
	data->memset( &param, 0x0, sizeof(SceWebBrowserDialogParam) );
	data->memset( &param.baseParam, 0x0, sizeof(SceCommonDialogBaseParam) );
	param.baseParam.magic = (uint32_t)( SCE_COMMON_DIALOG_MAGIC_NUMBER + (uint64_t)&param.baseParam );
	param.baseParam.size = (uint32_t)sizeof(SceCommonDialogBaseParam);
	param.size = sizeof(SceWebBrowserDialogParam);

	param.mode = SCE_WEB_BROWSER_DIALOG_MODE_DEFAULT;
	param.url = url;
	data->sceUserServiceGetInitialUser( &param.userId );

	data->sceWebBrowserDialogOpen( &param );

	SceCommonDialogStatus stat;
	while(1) {
		stat = data->sceWebBrowserDialogUpdateStatus();
		if (stat == SCE_COMMON_DIALOG_STATUS_FINISHED ) {
			data->sceWebBrowserDialogTerminate();
			break;
		}
	}
}

//////////////////////////////////////////////////
// registerPlayer
// Register the player inside the server
//////////////////////////////////////////////////

int registerPlayer(void) {
	struct hook_data* data =  (struct hook_data*)( *(uint64_t*)(0x400000) );

	// Create request
	char url[500];
	data->sprintf(url, data->np_url, data->base_url, data->username);

	int tmplId = data->sceHttpCreateTemplate(data->httpID, data->debug, 1, 0);
	int connId = data->sceHttpCreateConnectionWithURL(tmplId, url, 1);
	int reqId = data->sceHttpCreateRequestWithURL(connId, SCE_HTTP_METHOD_POST, url, 0);

	data->sceHttpSetRequestContentLength(reqId, SCE_NET_ETHER_ADDR_LEN);
	int ret = data->sceHttpSendRequest(reqId, data->ether_mac, SCE_NET_ETHER_ADDR_LEN);

	int error_msg = 0;
	data->sceHttpReadData(reqId, &error_msg, sizeof(int));

	data->sceHttpDeleteRequest(reqId);
	data->sceHttpDeleteConnection(connId);
	data->sceHttpDeleteTemplate(tmplId);

	return error_msg;
}

//////////////////////////////////////////////////
// makeMatchmaking
// Get all clients in a party
//////////////////////////////////////////////////
uint32_t* makeMatchmaking(int partyID, int* clientNbr) {
	struct hook_data* data =  (struct hook_data*)( *(uint64_t*)(0x400000) );

	if (data->network_error) {
		while (1) {
			data->openDialog(data->service_offline_msg);
		}

		data->exitApp();
		return NULL;
	}

	char url[500];
	data->sprintf(url, data->gc_url, data->base_url, partyID);

	uint32_t* clients_list = data->malloc(MAX_CLIENT_PARTY * sizeof(uint32_t));

	int tmplId = data->sceHttpCreateTemplate(data->httpID, data->debug, 1, 0);
	int connId = data->sceHttpCreateConnectionWithURL(tmplId, url, 1);
	int reqId = data->sceHttpCreateRequestWithURL(connId, SCE_HTTP_METHOD_POST, url, 0);

	data->sceHttpSetRequestContentLength(reqId, SCE_NET_ETHER_ADDR_LEN);
	int ret = data->sceHttpSendRequest(reqId, data->ether_mac, SCE_NET_ETHER_ADDR_LEN);
	if (ret < 0) {
		data->network_error = 1;
		*clientNbr = 0;
		return clients_list;
	}

	int sizeData = 0;

	sizeData = data->sceHttpReadData(reqId, clients_list, MAX_CLIENT_PARTY * sizeof(uint32_t));
	*clientNbr = sizeData / sizeof(uint32_t);

	data->sceHttpDeleteRequest(reqId);
	data->sceHttpDeleteConnection(connId);
	data->sceHttpDeleteTemplate(tmplId);

	return clients_list;
}

//////////////////////////////////////////////////
// newPartyID
// Get a new PartyID with nobody in this
//////////////////////////////////////////////////
void newPartyID(void) {
	struct hook_data* data =  (struct hook_data*)( *(uint64_t*)(0x400000) );

	if (data->network_error == 1) {
		while (1) {
			data->openDialog(data->service_offline_msg);
		}
		data->exitApp();
		return;
	}

	char url[500];
	char buffer[2000];

	data->sprintf(url, data->new_url, data->base_url);

	int tmplId = data->sceHttpCreateTemplate(data->httpID, data->debug, 1, 0);
	int connId = data->sceHttpCreateConnectionWithURL(tmplId, url, 1);
	int reqId = data->sceHttpCreateRequestWithURL(connId, SCE_HTTP_METHOD_GET, url, 0);

	int ret = data->sceHttpSendRequest(reqId, NULL, 0);
	if (ret < 0) {
		data->network_error = 1;
	}

	uint32_t new_party = 0;
	int sizeData = data->sceHttpReadData(reqId, &new_party, sizeof(uint32_t));

	data->partyID = new_party;

	data->sceHttpDeleteRequest(reqId);
	data->sceHttpDeleteConnection(connId);
	data->sceHttpDeleteTemplate(tmplId);

	data->sprintf(buffer, data->party_changed, data->partyID);
	data->sceSysUtilSendSystemNotificationWithText(222, buffer);
}

//////////////////////////////////////////////////
// getExternalIP
// Get the External IP of the Client
//////////////////////////////////////////////////

uint32_t getExternalIP(void) {
	struct hook_data* data =  (struct hook_data*)( *(uint64_t*)(0x400000) );

	char url[500];

	// Create request
	data->sprintf(url, data->gei_url, data->base_url);

	int tmplId = data->sceHttpCreateTemplate(data->httpID, data->debug, 1, 0);
	int connId = data->sceHttpCreateConnectionWithURL(tmplId, url, 1);
	int reqId = data->sceHttpCreateRequestWithURL(connId, SCE_HTTP_METHOD_GET, url, 0);

	int ret = data->sceHttpSendRequest(reqId, NULL, 0);

	uint32_t ext_ip = 0;

	if (ret < 0) {
		data->network_error = 1;
	} else {
		int sizeData = data->sceHttpReadData(reqId, &ext_ip, sizeof(uint32_t));
	}

	data->sceHttpDeleteRequest(reqId);
	data->sceHttpDeleteConnection(connId);
	data->sceHttpDeleteTemplate(tmplId);

	return ext_ip;
}

//////////////////////////////////////////////////
// exitApp
// Close the Application
//////////////////////////////////////////////////

void exitApp(void) {
	struct hook_data* data =  (struct hook_data*)( *(uint64_t*)(0x400000) );

	data->exit(0);
	//int pid = data->getpid();
	//data->kill(pid, 1);
}
