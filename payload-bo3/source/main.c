#include "ps4.h"
#include "settings.h"
#include "debug.h"
#include "sparse.h"
#include "kernel_func.h"
#include "sce_struct.h"
#include "utils.h"
#include "hook.h"

int(*sceSysUtilSendSystemNotificationWithText)(int messageType, char* message);

void notify(char* msg) {
	sceSysUtilSendSystemNotificationWithText(222, msg);
}

void waitRemoteLib(int pid, char* name) {
	int vm_nbr = 0;
	Map* vm_list = NULL;
	uint64_t start = 0;
	
	while (start == 0) {
		vm_list = process_vm_list(pid, &vm_nbr);

		for (int i = 0; i < 200; i++) {
			if ( strcmp(vm_list[i].name, name) == 0 ) {
				start = vm_list[i].start;
			}
		}
		free(vm_list);
	}
}

/*
void alway_mount_data(void) {
	Process shellcore = process_by_name("SceShellCore");

	if (shellcore.pid != -1) {
		int map_nbr = 0;
		Map* vm_map = process_vm_list(shellcore.pid, &map_nbr);

		uint64_t nop_slide = 0x9090909090909090;
		process_write_mem(shellcore.pid, vm_map[1].start + 0x31CA2A, &nop_slide, 6);

		free(vm_map);

		debugPrint("Now /data is alway mounted on all process !");
	} else {
		debugPrint("SceShellCore not found !");
	}
}
*/

int _main(struct thread *td) {
	// Init and resolve needed libraries
	initKernel();
	initLibc();
	initNetwork();

	syscall(11, jailbreak);

	int sysUtil = sceKernelLoadStartModule("/system/common/lib/libSceSysUtil.sprx", 0, NULL, 0, 0, 0);
	RESOLVE(sysUtil, sceSysUtilSendSystemNotificationWithText);

	////////////////////////////
	// FIND PROCESS & LIBRARY
	////////////////////////////

	// Init debug log
	#ifdef DEBUG
		debugLogInit(IP(192, 168, 1, 27), 9025);
		debugPrint("BO3 LAN Redirector\n");
	#endif

	Process process = process_by_name("eboot.bin");
	
	if (process.pid != -1) {
		process_kill(process.pid);
	}

	process.pid = -1;

	// Patch SceShellCore for mount /data inside all new app opened
	//alway_mount_data();
	
	notify("Please launch Black Ops 3");

	int game_type = 0;
	while (process.pid == -1 && strcmp(process.app_id, "CUSA02290") != 0 && strcmp(process.app_id, "CUSA02624") != 0 && strcmp(process.app_id, "CUSA02626") != 0) {
		process = process_by_name("eboot.bin");
	}

	#ifdef DEBUG
		debugPrint("eboot.bin found at pid %i\n", process.pid);
	#endif

	// Find the BO3 Version
	int version = 0;
	uint64_t version_buff = 0;

	process_read_mem(process.pid, 0x400000 + 0xD00000, &version_buff, sizeof(uint64_t));

	if (version_buff == BO3_100_VERSION_CHECK) {
		version = BO3_VER_100;
		notify("Black Ops 3 Version 1.00 | Found !");
	} else if (version_buff == BO3_126_VERSION_CHECK) {
		version = BO3_VER_126;
		notify("Black Ops 3 Version 1.26 | Found !");
	} else if (version_buff == BO3_127_VERSION_CHECK) {
		version = BO3_VER_127;
		notify("Black Ops 3 Version 1.27 | Found !");
	} else {
		notify("Version not found !");
	}

	// Allocate memory (RWX) in remote process
	void* allocated_hook = process_allocate(process.pid, 0x200000);

	#ifdef DEBUG
		debugPrint("Allocated memory: %p\n", allocated_hook);
	#endif

	////////////////////////////
	// Create all data needed
	////////////////////////////
	struct hook_data data;
	memset(&data, 0, sizeof(struct hook_data));

	// String
	strcpy(data.debug, "debug");
	strcpy(data.log_buff, "sceKernelOpen: %s\n");
	strcpy(data.log_buff2, "nsize: %i\n");
	strcpy(data.log_buff3, "sceFiosFHOpenSync: %s (%i)\n");
	strcpy(data.log_buff4, "%02x");
	strcpy(data.path1, "/mods/zm_tacobell2/zone/zm_tacobell2.ff");
	strcpy(data.path2, "./mods/zm_tacobell2/zone/zm_tacobell2.ff");
	strcpy(data.path_data, "/user/data/CUSA02624/zm_tacobell2");
	strcpy(data.path_root, "/");
	strcpy(data.rb, "rb");
	strcpy(data.party_changed, "PartyID changed to %lu");
	strcpy(data.client_connected, "%i clients connected in this party !");
	strcpy(data.upnp_not_available, "UPnP Error: UPnP Service not found.\nYou need to open you port manually for host a game.");
	strcpy(data.upnp_failed, "UPnP Error: UPnP Service detected but cannot open port.\nYou need to open you port manually for host a game.");
	strcpy(data.upnp_done, "Port forwarding : Success !");
	strcpy(data.externalip_msg, "External IP: %s");
	strcpy(data.internalip_msg, "Internal IP: %s");
	strcpy(data.beta_warning, "CODLAN is a Beta (PB1.0) !");
	strcpy(data.username_taken_msg, "Username already taken\nPlease choose another username.");
	strcpy(data.username_invalid_msg, "Invalid username\nBetween 4 and 16 characters. Only Alphanumerics.");
	strcpy(data.ps4_already_registred_msg, "Your PS4 is already registred\nPlease contact an Administrator.");
	strcpy(data.ps4_banned_msg, "Your PS4 have been banned from CODLAN.\nPlease close the game.");
	strcpy(data.service_offline_msg, "CODLAN is currenly offline\nPlease close the game.");
	strcpy(data.error_occured_msg, "An error occured in CODLAN.\nPlease contact an Administrator");
	strcpy(data.welcome_msg, "Welcome in CODLAN !");
	strcpy(data.zm_factory, "zm_factory");
	strcpy(data.zm_tacobell2, "zm_tacobell2");
	strcpy(data.app0, "/app0");
	strcpy(data.addcont0, "/addcont0");
	strcpy(data.mods, "/mods/zm_tacobell2");
	strcpy(data.debug_int, "%i");
	strcpy(data.debug_hex, "%x");
	strcpy(data.debug_ptr, "%p");
	strcpy(data.website_url, WEBSITE_URL);
	strcpy(data.base_url, BASE_URL);
	strcpy(data.hb_url, "%s/heartbeat/%i");
	strcpy(data.gc_url, "%s/get/%i");
	strcpy(data.gei_url, "%s/netip");
	strcpy(data.gun_url, "%s/username");
	strcpy(data.np_url, "%s/new_player/%s");
	strcpy(data.new_url, "%s/new_party");
	strcpy(data.titleKeyboard, "Enter the PartyID");
	strcpy(data.usernameTitleKB, "Enter your username (Max. 16 characters)");
	strcpy(data.ServerlistInfo, "ServerlistInfo");
	strcpy(data.ok_status, "OK");
	strcpy(data.ban_status, "BAN");
	strcpy(data.is_ps4_cookie, "is_ps4=1");

	strcpy(data.upnp_search, "M-SEARCH * HTTP/1.1\r\n"
    "Host: 239.255.255.250:1900\r\n"
    "Man: \"ssdp:discover\"\r\n"
    "ST:upnp:rootdevice\r\n"
    "MX:3\r\n"
    "\r\n");

    strcpy(data.upnp_slash, "/");
	strcpy(data.upnp_http, "http://");
	strcpy(data.upnp_rn, "\r\n");
	strcpy(data.upnp_WANIPConnection, "WANIPConnection");
	strcpy(data.upnp_controlURL, "<controlURL>");
	strcpy(data.upnp_controlURL_end, "</controlURL>");
	strcpy(data.upnp_invalid_ip, "0.0.0.0");
	strcpy(data.upnp_split, "%s%s");
	strcpy(data.upnp_soap_body_pf, "<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:AddPortMapping xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:1\"><NewRemoteHost></NewRemoteHost><NewExternalPort>%i</NewExternalPort><NewProtocol>UDP</NewProtocol><NewInternalPort>%i</NewInternalPort><NewInternalClient>%s</NewInternalClient><NewEnabled>1</NewEnabled><NewPortMappingDescription>%s</NewPortMappingDescription><NewLeaseDuration>0</NewLeaseDuration></u:AddPortMapping></s:Body></s:Envelope>");
	strcpy(data.SOAPAction, "SOAPAction");
	strcpy(data.SOAPAction_data, "\"urn:schemas-upnp-org:service:WANIPConnection:1#AddPortMapping\"");
	strcpy(data.ContentType, "Content-Type");
	strcpy(data.ContentLength, "Content-Length");
	strcpy(data.xml_mime, "text/xml");
	strcpy(data.upnp_desc, "BO3LanUDP");
	strcpy(data.upnp_error, "error");

	strcpy(data.log[0], "External IP: %s");

	strcpy(data.library_name[0], "libc.prx");
	strcpy(data.library_name[1], "libSceNet.sprx");
	strcpy(data.library_name[2], "libScePad.sprx");
	strcpy(data.library_name[3], "libSceIme.sprx");
	strcpy(data.library_name[4], "libSceImeDialog.sprx");
	strcpy(data.library_name[5], "libSceHttp.sprx");
	strcpy(data.library_name[6], "libkernel.sprx");
	strcpy(data.library_name[7], "libSceSsl.sprx");
	strcpy(data.library_name[8], "libSceSysUtil.sprx");
	strcpy(data.library_name[9], "libSceNetCtl.sprx");
	strcpy(data.library_name[10], "libSceUserService.sprx");
	strcpy(data.library_name[11], "libSceCommonDialog.sprx");
	strcpy(data.library_name[12], "libSceMsgDialog.sprx");
	strcpy(data.library_name[13], "libSceWebBrowserDialog.sprx");
	strcpy(data.library_name[14], "libSceFios2.sprx");

	strcpy(data.libscenet_name[0], "sceNetSocket");
	strcpy(data.libscenet_name[1], "sceNetConnect");
	strcpy(data.libscenet_name[2], "sceNetSend");
	strcpy(data.libscenet_name[3], "sceNetSendto");
	strcpy(data.libscenet_name[4], "sceNetInetNtop");
	strcpy(data.libscenet_name[5], "sceNetNtohs");
	strcpy(data.libscenet_name[6], "sceNetNtohl");
	strcpy(data.libscenet_name[7], "sceNetHtonl");
	strcpy(data.libscenet_name[8], "sceNetPoolCreate");
	strcpy(data.libscenet_name[9], "sceNetRecvfrom");
	strcpy(data.libscenet_name[10], "sceNetSetsockopt");
	
	strcpy(data.libscenetctl_name[0], "sceNetCtlGetInfo");

	strcpy(data.libscehttp_name[0], "sceHttpInit");
	strcpy(data.libscehttp_name[1], "sceHttpCreateTemplate");
	strcpy(data.libscehttp_name[2], "sceHttpCreateConnectionWithURL");
	strcpy(data.libscehttp_name[3], "sceHttpCreateRequestWithURL");
	strcpy(data.libscehttp_name[4], "sceHttpSendRequest");
	strcpy(data.libscehttp_name[5], "sceHttpDeleteRequest");
	strcpy(data.libscehttp_name[6], "sceHttpDeleteTemplate");
	strcpy(data.libscehttp_name[7], "sceHttpDeleteConnection");
	strcpy(data.libscehttp_name[8], "sceHttpReadData");
	strcpy(data.libscehttp_name[9], "sceHttpAddRequestHeader");
	strcpy(data.libscehttp_name[10], "sceHttpSetRequestContentLength");

	strcpy(data.libscessl_name[0], "sceSslInit");

	strcpy(data.libc_name[0], "strlen");
	strcpy(data.libc_name[1], "sprintf");
	strcpy(data.libc_name[2], "memcpy");
	strcpy(data.libc_name[3], "memset");
	strcpy(data.libc_name[4], "mbstowcs");
	strcpy(data.libc_name[5], "wcstombs");
	strcpy(data.libc_name[6], "atoi");
	strcpy(data.libc_name[7], "strncmp");
	strcpy(data.libc_name[8], "strstr");
	strcpy(data.libc_name[9], "malloc");
	strcpy(data.libc_name[10], "realloc");
	strcpy(data.libc_name[11], "free");
	strcpy(data.libc_name[12], "exit");
	strcpy(data.libc_name[13], "strcmp");
	strcpy(data.libc_name[14], "fopen");
	strcpy(data.libc_name[15], "opendir");
	strcpy(data.libc_name[16], "readdir");
	strcpy(data.libc_name[17], "closedir");

	strcpy(data.libscepad_name[0], "scePadReadState");

	strcpy(data.libsceimedialog_name[0], "sceImeDialogInit");
	strcpy(data.libsceimedialog_name[1], "sceImeDialogGetStatus");
	strcpy(data.libsceimedialog_name[2], "sceImeDialogTerm");
	strcpy(data.libsceimedialog_name[3], "sceImeDialogGetResult");

	strcpy(data.libkernel_name[0], "sceKernelGettimeofday");
	strcpy(data.libkernel_name[1], "scePthreadCreate");
	strcpy(data.libkernel_name[2], "sceKernelSleep");
	strcpy(data.libkernel_name[3], "getpid");
	strcpy(data.libkernel_name[4], "kill");
	strcpy(data.libkernel_name[5], "sceKernelOpen");

	strcpy(data.libsysutil_name[0], "sceSysUtilSendSystemNotificationWithText");

	strcpy(data.libsceuserservice_name[0], "sceUserServiceGetInitialUser");
	strcpy(data.libsceuserservice_name[1], "sceUserServiceGetUserName");

	strcpy(data.libscecommondialog_name[0], "sceCommonDialogInitialize");

	strcpy(data.libscemsgdialog_name[0], "sceMsgDialogInitialize");
	strcpy(data.libscemsgdialog_name[1], "sceMsgDialogOpen");
	strcpy(data.libscemsgdialog_name[2], "sceMsgDialogUpdateStatus");
	strcpy(data.libscemsgdialog_name[3], "sceMsgDialogTerminate");

	strcpy(data.libscewebbrowserdialog_name[0], "sceWebBrowserDialogInitialize");
	strcpy(data.libscewebbrowserdialog_name[1], "sceWebBrowserDialogOpen");
	strcpy(data.libscewebbrowserdialog_name[2], "sceWebBrowserDialogTerminate");
	strcpy(data.libscewebbrowserdialog_name[3], "sceWebBrowserDialogUpdateStatus");
	strcpy(data.libscewebbrowserdialog_name[4], "sceWebBrowserDialogSetCookie");

	strcpy(data.libscefios2_name[0], "sceFiosFHOpenSync");

	// Get Ether Addr
	SceNetCtlInfo ether_info;
	sceNetCtlGetInfo(SCE_NET_CTL_INFO_ETHER_ADDR, &ether_info);
	memcpy(data.ether_mac, ether_info.ether_addr.data, SCE_NET_ETHER_ADDR_LEN);

	// State
	data.hit_buttons = 0;
	data.initDone = 0;
	data.partyID = 0;
	data.poolID = 0;
	data.sslID = 0;
	data.httpID = 0;
	data.heartbeat_time = 0;
	data.externalIP_show = 0;

	// Socket object
	// Create socket structure

	data.sock_obj_upnp.sin_len = sizeof(data.sock_obj_upnp);
	data.sock_obj_upnp.sin_family = AF_INET;
	data.sock_obj_upnp.sin_addr.s_addr = IP(239, 255, 255, 250);
	data.sock_obj_upnp.sin_port = sceNetHtons(1900);
	memset(data.sock_obj_upnp.sin_zero, 0, sizeof(data.sock_obj_upnp.sin_zero));

	data.sock_obj_log.sin_len = sizeof(data.sock_obj_log);
	data.sock_obj_log.sin_family = AF_INET;
	data.sock_obj_log.sin_addr.s_addr = IP(192, 168, 1, 27);
	data.sock_obj_log.sin_port = sceNetHtons(9026);
	memset(data.sock_obj_log.sin_zero, 0, sizeof(data.sock_obj_log.sin_zero));

	/////////////////////////
	// 	WRITE UTILS
	/////////////////////////

	uint64_t hook_addr = (uint64_t)allocated_hook;

	// Writting initHooking function
	process_write_mem(process.pid, hook_addr, initHooking, 0x2000);
	data.initHooking = (void*)hook_addr;
	hook_addr += 0x2000;

	// Writting initUPnP function
	process_write_mem(process.pid, hook_addr, initUPnP, 0x1000);
	data.initUPnP = (void*)hook_addr;
	hook_addr += 0x1000;

	// Writting newHeartbeat function
	process_write_mem(process.pid, hook_addr, newHeartbeat, 0x500);
	data.newHeartbeat = (void*)hook_addr;
	hook_addr += 0x500;

	// Writting getUsername function
	process_write_mem(process.pid, hook_addr, getUsername, 0x500);
	data.getUsername = (void*)hook_addr;
	hook_addr += 0x500;

	// Writting usernameKeyboard function
	process_write_mem(process.pid, hook_addr, usernameKeyboard, 0x500);
	data.usernameKeyboard = (void*)hook_addr;
	hook_addr += 0x500;

	// Writting openDialog function
	process_write_mem(process.pid, hook_addr, openDialog, 0x500);
	data.openDialog = (void*)hook_addr;
	hook_addr += 0x500;

	// Writting openWebDialog function
	process_write_mem(process.pid, hook_addr, openWebDialog, 0x500);
	data.openWebDialog = (void*)hook_addr;
	hook_addr += 0x500;

	// Writting registerPlayer function
	process_write_mem(process.pid, hook_addr, registerPlayer, 0x500);
	data.registerPlayer = (void*)hook_addr;
	hook_addr += 0x500;

	// Writting makeMatchmaking function
	process_write_mem(process.pid, hook_addr, makeMatchmaking, 0x500);
	data.makeMatchmaking = (void*)hook_addr;
	hook_addr += 0x500;

	// Writting changePartyID function
	process_write_mem(process.pid, hook_addr, changePartyID, 0x500);
	data.changePartyID = (void*)hook_addr;
	hook_addr += 0x500;

	// Writting newPartyID function
	process_write_mem(process.pid, hook_addr, newPartyID, 0x500);
	data.newPartyID = (void*)hook_addr;
	hook_addr += 0x500;

	// Writting getExternalIP function
	process_write_mem(process.pid, hook_addr, getExternalIP, 0x500);
	data.getExternalIP = (void*)hook_addr;
	hook_addr += 0x500;

	// Writting forwardUPnP function
	process_write_mem(process.pid, hook_addr, forwardUPnP, 0x500);
	data.forwardUPnP = (void*)hook_addr;
	hook_addr += 0x500;

	// Writting remote_escape function
	process_write_mem(process.pid, hook_addr, remote_escape, 0x500);
	data.remote_escape = (void*)hook_addr;
	hook_addr += 0x500;

	// Writting upnp_thread function
	process_write_mem(process.pid, hook_addr, upnp_thread, 0x1000);
	data.upnp_thread = (void*)hook_addr;
	hook_addr += 0x1000;
	
	// Writting heartbeat_thread function
	process_write_mem(process.pid, hook_addr, heartbeat_thread, 0x1000);
	data.heartbeat_thread = (void*)hook_addr;
	hook_addr += 0x1000;

	// Writting syscall function
	uint8_t sycall_asm[] = { SYSCALL_BYTES };
	process_write_mem(process.pid, hook_addr, &sycall_asm, sizeof(sycall_asm));
	data.remote_syscall = (void*)hook_addr;
	hook_addr += sizeof(sycall_asm);

	/////////////////////////
	// 	WRITE DATA
	/////////////////////////

	// Writting data at the top
	process_write_mem(process.pid, hook_addr, &data, sizeof(struct hook_data));
	process_write_mem(process.pid, 0x400000, &hook_addr, sizeof(uint64_t));
	struct hook_data* remote_data = (struct hook_data*)hook_addr;
	hook_addr += sizeof(struct hook_data) + 0x500;

	/////////////////////////
	// 	WRITE HOOKS
	/////////////////////////

	uint64_t sceNetCtlGetInfo_addr,  scePadReadState_addr, sceNetSendto_addr, sceUserServiceGetUserName_addr, sceKernelOpen_addr, fopen_addr, sceFiosFHOpenSync_addr = 0;

	// Writting sceNetCtlGetInfo hook
	process_write_mem(process.pid, hook_addr, sceNetCtlGetInfo_hook, 0x500);
	sceNetCtlGetInfo_addr = hook_addr;
	hook_addr += 0x500;

	// Writting scePadReadState hook
	process_write_mem(process.pid, hook_addr, scePadReadState_hook, 0x1000);
	scePadReadState_addr = hook_addr;
	hook_addr += 0x1000;

	// Writting sceNetSendto hook
	process_write_mem(process.pid, hook_addr, sceNetSendto_hook, 0x500);
	sceNetSendto_addr = hook_addr;
	hook_addr += 0x500;

	// Writting sceUserServiceGetUserName hook
	process_write_mem(process.pid, hook_addr, sceUserServiceGetUserName_hook, 0x500);
	sceUserServiceGetUserName_addr = hook_addr;
	hook_addr += 0x500;

	// Writting sceFiosFHOpenSync hook
	process_write_mem(process.pid, hook_addr, sceFiosFHOpenSync_hook, 0x500);
	sceFiosFHOpenSync_addr = hook_addr;
	hook_addr += 0x500;

	waitRemoteLib(process.pid, "libSceNetCtl.sprx");

	// Writting sceNetCtlGetInfo addr
	if (version == BO3_VER_127) {
		process_write_mem(process.pid, 0x400000 + 0x1527548, &sceNetCtlGetInfo_addr, sizeof(uint64_t));
	} else if (version == BO3_VER_126) {
		process_write_mem(process.pid, 0x400000 + 0x1527568, &sceNetCtlGetInfo_addr, sizeof(uint64_t));
	} else if (version == BO3_VER_100) {
		process_write_mem(process.pid, 0x400000 + 0x15E6770, &sceNetCtlGetInfo_addr, sizeof(uint64_t));
	}

	waitRemoteLib(process.pid, "libSceUserService.sprx");

	// Writting sceUserServiceGetUserName addr
	if (version == BO3_VER_127) {
		process_write_mem(process.pid, 0x400000 + 0x1527978, &sceUserServiceGetUserName_addr, sizeof(uint64_t));
	} else if (version == BO3_VER_126) {
		process_write_mem(process.pid, 0x400000 + 0x1527970, &sceUserServiceGetUserName_addr, sizeof(uint64_t));
	} else if (version == BO3_VER_100) {
		process_write_mem(process.pid, 0x400000 + 0x15E7378, &sceUserServiceGetUserName_addr, sizeof(uint64_t));
	}

	waitRemoteLib(process.pid, "libSceNet.sprx");

	// Writting sceNetSendto addr
	if (version == BO3_VER_127) {
		process_write_mem(process.pid, 0x400000 + 0x15274C0, &sceNetSendto_addr, sizeof(uint64_t));
	} else if (version == BO3_VER_126) {
		process_write_mem(process.pid, 0x400000 + 0x1527540, &sceNetSendto_addr, sizeof(uint64_t));
	} else if (version == BO3_VER_100) {
		process_write_mem(process.pid, 0x400000 + 0x15E6E20, &sceNetSendto_addr, sizeof(uint64_t));
	}

	waitRemoteLib(process.pid, "libScePad.sprx");

	// Writting scePadReadState addr
	if (version == BO3_VER_127) {
		process_write_mem(process.pid, 0x400000 + 0x1527DB0, &scePadReadState_addr, sizeof(uint64_t));
	} else if (version == BO3_VER_126) {
		process_write_mem(process.pid, 0x400000 + 0x1527DD8, &scePadReadState_addr, sizeof(uint64_t));
	} else if (version == BO3_VER_100) {
		process_write_mem(process.pid, 0x400000 + 0x15E7438, &scePadReadState_addr, sizeof(uint64_t));
	}

	/*
	// sceFiosFHOpenSync hook for map mods
	uint64_t addr = 0;
	process_read_mem(process.pid, 0x400000 + 0x15280D0, &addr, sizeof(uint64_t));
	process_write_mem(process.pid, &remote_data->sceFiosFHOpenSync, &addr, sizeof(uint64_t));
	process_write_mem(process.pid, 0x400000 + 0x15280D0, &sceFiosFHOpenSync_addr, sizeof(uint64_t));

	////////////////////////
	// FASTFILE LOADER PATCH
	////////////////////////

	char short_jmp = 0xEB;
	uint64_t nop_slide = 0x9090909090909090;

	process_write_mem(process.pid, 0x400000 + 0x7F9D3B, &short_jmp, sizeof(char)); // FastFile magic check
	process_write_mem(process.pid, 0x400000 + 0x7F9D79, &short_jmp, sizeof(char)); // FastFile Lower bypass
	process_write_mem(process.pid, 0x400000 + 0x7F9D8A, &short_jmp, sizeof(char)); // FastFile Higher bypass
	process_write_mem(process.pid, 0x400000 + 0x7F9DAE, &short_jmp, sizeof(char)); // FastFile Allow server on client
	process_write_mem(process.pid, 0x400000 + 0x7F9DC7, &nop_slide, 2); // FastFile Allow any platform
	*/

	////////////////////////
	// END
	////////////////////////

	#ifdef DEBUG
		debugPrint("Done.\n");
	#endif

	end:
	#ifdef DEBUG
		debugLogTerm();
	#endif

	return 0;
}
