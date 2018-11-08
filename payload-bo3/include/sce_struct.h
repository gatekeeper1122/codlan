#define SCE_PAD_MAX_TOUCH_NUM	2
#define SCE_IME_MAX_PREEDIT_LENGTH 30
#define SCE_IME_MAX_EXPANDED_PREEDIT_LENGTH 120
#define SCE_IME_MAX_TEXT_LENGTH 2048
#define SCE_IME_MAX_TEXT_AREA 4
#define SCE_IME_MAX_CANDIDATE_WORD_LENGTH 55
#define SCE_IME_MAX_CANDIDATE_LIST_SIZE 100
#define SCE_IME_KEYBOARD_MAX_NUMBER 5

#define SCE_USER_SERVICE_USER_ID_INVALID -1
#define SCE_USER_SERVICE_USER_ID_SYSTEM 255
#define SCE_USER_SERVICE_USER_ID_EVERYONE 254

#define SSL_POOLSIZE 	(128 * 1024U)
#define LIBHTTP_POOLSIZE 	(64 * 1024)

#define SCE_NET_CTL_SSID_LEN                 (32 + 1)
#define SCE_NET_CTL_HOSTNAME_LEN            (255 + 1)
#define SCE_NET_CTL_AUTH_NAME_LEN           (127 + 1)
#define SCE_NET_CTL_IPV4_ADDR_STR_LEN      (16)

#define SCE_NET_ETHER_ADDR_LEN 6

typedef int32_t SceUserServiceUserId;

typedef enum{
	SCE_HTTP_METHOD_GET,
	SCE_HTTP_METHOD_POST,
	SCE_HTTP_METHOD_HEAD,
	SCE_HTTP_METHOD_OPTIONS,
	SCE_HTTP_METHOD_PUT,
	SCE_HTTP_METHOD_DELETE,
	SCE_HTTP_METHOD_TRACE,
	SCE_HTTP_METHOD_CONNECT
} SceHttpMethods;

typedef struct SceRtcTick {
    uint64_t tick;
} SceRtcTick;

typedef enum SceImeKeyboardType {
    SCE_IME_KEYBOARD_TYPE_NONE = 0,
    SCE_IME_KEYBOARD_TYPE_ENGLISH_US = 4,
    SCE_IME_KEYBOARD_TYPE_FRENCH = 9,
} SceImeKeyboardType;

typedef enum SceImeCaretMovementDirection {
	SCE_IME_CARET_MOVE_STILL = 0,
	SCE_IME_CARET_MOVE_LEFT = 1,
	SCE_IME_CARET_MOVE_RIGHT = 2,
	SCE_IME_CARET_MOVE_UP = 3,
	SCE_IME_CARET_MOVE_DOWN = 4,
	SCE_IME_CARET_MOVE_HOME = 5,
	SCE_IME_CARET_MOVE_END = 6,
	SCE_IME_CARET_MOVE_PAGE_UP = 7,
	SCE_IME_CARET_MOVE_PAGE_DOWN = 8,
	SCE_IME_CARET_MOVE_TOP = 9,
	SCE_IME_CARET_MOVE_BOTTOM = 10,
} SceImeCaretMovementDirection;

typedef enum SceImeEventId {
	SCE_IME_EVENT_OPEN = 0,
	SCE_IME_EVENT_UPDATE_TEXT = 1,
	SCE_IME_KEYBOARD_EVENT_OPEN = 256,
	SCE_IME_KEYBOARD_EVENT_KEYCODE_DOWN = 257,
	SCE_IME_KEYBOARD_EVENT_KEYCODE_UP = 258,
} SceImeEventId;

typedef struct SceImeKeycode {
	uint16_t keycode;
	wchar_t character;
	uint32_t status;
	SceImeKeyboardType type;
	SceUserServiceUserId userId;
	uint32_t resourceId;
	SceRtcTick timestamp;
} SceImeKeycode;

typedef struct SceImeRect {
	float x;
	float y;
	uint32_t width;
	uint32_t height;
} SceImeRect;

typedef enum SceImeTextAreaMode {
	SCE_IME_TEXT_AREA_MODE_DISABLE = 0,
	SCE_IME_TEXT_AREA_MODE_EDIT = 1,
	SCE_IME_TEXT_AREA_MODE_PREEDIT = 2,
	SCE_IME_TEXT_AREA_MODE_SELECT = 3,
} SceImeTextAreaMode;

typedef struct SceImeTextAreaProperty {
	SceImeTextAreaMode mode;
	uint32_t index;
	int32_t length;
} SceImeTextAreaProperty;

typedef struct SceImeEditText {
	wchar_t *str;
	uint32_t caretIndex;
	uint32_t areaNum;
	SceImeTextAreaProperty textArea[SCE_IME_MAX_TEXT_AREA];
} SceImeEditText;

typedef struct SceImeKeyboardResourceIdArray {
	SceUserServiceUserId userId;
	uint32_t resourceId[SCE_IME_KEYBOARD_MAX_NUMBER];
} SceImeKeyboardResourceIdArray;

typedef enum SceImeDeviceType {
    SCE_IME_DEVICE_TYPE_NONE = 0,
    SCE_IME_DEVICE_TYPE_CONTROLLER = 1,
    SCE_IME_DEVICE_TYPE_EXT_KEYBOARD = 2,
    SCE_IME_DEVICE_TYPE_REMOTE_OSK = 3,
} SceImeDeviceType;

typedef union {
	SceImeRect rect;
	SceImeEditText text;
	SceImeCaretMovementDirection caretMove;
	SceImeKeycode keycode;
	SceImeKeyboardResourceIdArray resourceIdArray;
	wchar_t *candidateWord;
	int32_t candidateIndex;
	SceImeDeviceType deviceType;
	uint32_t inputMethodState;
	int8_t reserved[64];
} SceImeEventParam;

typedef struct SceImeEvent {
	SceImeEventId id;
	SceImeEventParam param;
} SceImeEvent;

typedef void (*SceImeEventHandler)(
	void *arg,
	const SceImeEvent *e
);

typedef struct SceImeKeyboardParam {
	uint32_t option;
	int8_t reserved1[4];
	void *arg;
	SceImeEventHandler handler;
	int8_t reserved2[8];
} SceImeKeyboardParam;

typedef struct ScePadTouch {
	uint16_t x;
	uint16_t y;
	uint8_t id;
	uint8_t reserve[3];
} ScePadTouch;

typedef struct ScePadTouchData {
	uint8_t touchNum;
	uint8_t reserve[3];
	uint8_t reserve1;
	ScePadTouch touch[SCE_PAD_MAX_TOUCH_NUM];
} ScePadTouchData;

typedef struct ScePadAnalogButtons {
	uint8_t l2;
	uint8_t r2;
	uint8_t padding[2];
} ScePadAnalogButtons;

typedef struct ScePadAnalogStick {
	uint8_t x;
	uint8_t y;
} ScePadAnalogStick;

typedef struct SceFQuaternion {
	float x, y, z, w;
} SceFQuaternion;

typedef struct SceFVector2 {
	float x, y;
} SceFVector2;

typedef struct SceFVector3 {
	float x, y, z;
} SceFVector3;

typedef struct SceFVector4 {
	float x, y, z, w;
} SceFVector4;

typedef struct ScePadExtensionUnitData {
	uint32_t extensionUnitId;
	uint8_t reserve[1];
	uint8_t dataLength;
	uint8_t data[10];
} ScePadExtensionUnitData;

typedef enum {
	SCE_IME_DIALOG_END_STATUS_OK = 0,
	SCE_IME_DIALOG_END_STATUS_USER_CANCELED = 1,
	SCE_IME_DIALOG_END_STATUS_ABORTED = 2,
} SceImeDialogEndStatus;

typedef enum SceImeVerticalAlignment {
	SCE_IME_VALIGN_TOP = 0,
	SCE_IME_VALIGN_CENTER = 1,
	SCE_IME_VALIGN_BOTTOM = 2,
} SceImeVerticalAlignment;

typedef enum SceImeHorizontalAlignment {
	SCE_IME_HALIGN_LEFT = 0,
	SCE_IME_HALIGN_CENTER = 1,
	SCE_IME_HALIGN_RIGHT = 2,
} SceImeHorizontalAlignment;

typedef enum SceImeInputMethod {
	SCE_IME_INPUT_METHOD_DEFAULT = 0,
} SceImeInputMethod;

typedef enum SceImeType {
    SCE_IME_TYPE_DEFAULT = 0,
    SCE_IME_TYPE_BASIC_LATIN = 1,
    SCE_IME_TYPE_URL = 2,
    SCE_IME_TYPE_MAIL = 3,
    SCE_IME_TYPE_NUMBER = 4,
} SceImeType;

typedef enum SceImeEnterLabel {
	SCE_IME_ENTER_LABEL_DEFAULT = 0,
	SCE_IME_ENTER_LABEL_SEND = 1,
	SCE_IME_ENTER_LABEL_SEARCH = 2,
	SCE_IME_ENTER_LABEL_GO = 3,
} SceImeEnterLabel;

typedef enum {
	SCE_IME_DIALOG_STATUS_NONE = 0,
	SCE_IME_DIALOG_STATUS_RUNNING = 1,
	SCE_IME_DIALOG_STATUS_FINISHED = 2,
} SceImeDialogStatus;

typedef int (*SceImeTextFilter)(
	wchar_t *outText,
	uint32_t *outTextLength,
	const wchar_t *srcText,
	uint32_t srcTextLength
);

typedef struct SceImeDialogResult {
	SceImeDialogEndStatus endstatus;
	int8_t reserved[12];
} SceImeDialogResult;

typedef struct SceImeParam {
	SceUserServiceUserId userId;
	SceImeType type;
	uint64_t supportedLanguages;
	SceImeEnterLabel enterLabel;
	SceImeInputMethod inputMethod;
	SceImeTextFilter filter;
	uint32_t option;
	uint32_t maxTextLength;
	wchar_t *inputTextBuffer;
	float posx;
	float posy;
	SceImeHorizontalAlignment horizontalAlignment;
	SceImeVerticalAlignment verticalAlignment;
	void *work;
	void *arg;
	SceImeEventHandler handler;
	int8_t reserved[8];
} SceImeParam;

typedef struct SceImeDialogParam {
	SceUserServiceUserId userId;
	SceImeType type;
	uint64_t supportedLanguages;
	SceImeEnterLabel enterLabel;
	SceImeInputMethod inputMethod;
	SceImeTextFilter filter;
	uint32_t option;
	uint32_t maxTextLength;
	wchar_t *inputTextBuffer;
	float posx;
	float posy;
	SceImeHorizontalAlignment horizontalAlignment;
	SceImeVerticalAlignment verticalAlignment;
	const wchar_t *placeholder;
	const wchar_t *title;
	int8_t reserved[16];
} SceImeDialogParam;

typedef enum ScePadButtonDataOffset {
	SCE_PAD_BUTTON_L3    	= 0x00000002,
	SCE_PAD_BUTTON_R3    	= 0x00000004,
	SCE_PAD_BUTTON_OPTIONS	= 0x00000008,
	SCE_PAD_BUTTON_UP    	= 0x00000010,
	SCE_PAD_BUTTON_RIGHT 	= 0x00000020,
	SCE_PAD_BUTTON_DOWN  	= 0x00000040,
	SCE_PAD_BUTTON_LEFT  	= 0x00000080,
	SCE_PAD_BUTTON_L2    	= 0x00000100,
	SCE_PAD_BUTTON_R2    	= 0x00000200,
	SCE_PAD_BUTTON_L1    	= 0x00000400,
	SCE_PAD_BUTTON_R1    	= 0x00000800,
	SCE_PAD_BUTTON_TRIANGLE	= 0x00001000,
	SCE_PAD_BUTTON_CIRCLE	= 0x00002000,
	SCE_PAD_BUTTON_CROSS 	= 0x00004000,
	SCE_PAD_BUTTON_SQUARE	= 0x00008000,
	SCE_PAD_BUTTON_TOUCH_PAD	= 0x00100000,
	SCE_PAD_BUTTON_INTERCEPTED	= 0x80000000,
} ScePadButtonDataOffset;

typedef struct ScePadData {
	uint32_t buttons;
	ScePadAnalogStick leftStick;
	ScePadAnalogStick rightStick;
	ScePadAnalogButtons analogButtons;
	SceFQuaternion orientation;
	SceFVector3 acceleration;
	SceFVector3 angularVelocity;
	ScePadTouchData touchData;
	bool connected;
	uint64_t timestamp;
	ScePadExtensionUnitData extensionUnitData;
	uint8_t connectedCount;
	uint8_t reserve[2];
	uint8_t deviceUniqueDataLen;
	uint8_t deviceUniqueData[50];
} ScePadData;

typedef enum SceHttpAddHeaderMode {
	SCE_HTTP_HEADER_OVERWRITE,
	SCE_HTTP_HEADER_ADD
} SceHttpAddHeaderMode;


////// MSG DIALOG

#define SCE_COMMON_DIALOG_MAGIC_NUMBER 0xC0D1A109

typedef enum SceCommonDialogStatus {
	SCE_COMMON_DIALOG_STATUS_NONE				= 0,
	SCE_COMMON_DIALOG_STATUS_INITIALIZED		= 1,
	SCE_COMMON_DIALOG_STATUS_RUNNING			= 2,
	SCE_COMMON_DIALOG_STATUS_FINISHED			= 3
} SceCommonDialogStatus;

typedef enum SceCommonDialogResult {
	SCE_COMMON_DIALOG_RESULT_OK					= 0,
	SCE_COMMON_DIALOG_RESULT_USER_CANCELED		= 1,
} SceCommonDialogResult;

typedef struct SceCommonDialogBaseParam {
	size_t size;
	uint8_t reserved[36];
	uint32_t magic;
} SceCommonDialogBaseParam __attribute__ ((__aligned__(8)));

typedef enum SceMsgDialogMode {
	SCE_MSG_DIALOG_MODE_USER_MSG = 1,
	SCE_MSG_DIALOG_MODE_PROGRESS_BAR = 2,
	SCE_MSG_DIALOG_MODE_SYSTEM_MSG = 3,
} SceMsgDialogMode;

typedef enum SceMsgDialogButtonType {
	SCE_MSG_DIALOG_BUTTON_TYPE_OK = 0,
	SCE_MSG_DIALOG_BUTTON_TYPE_YESNO = 1,
	SCE_MSG_DIALOG_BUTTON_TYPE_NONE = 2,
	SCE_MSG_DIALOG_BUTTON_TYPE_OK_CANCEL = 3,
	SCE_MSG_DIALOG_BUTTON_TYPE_WAIT = 5,
	SCE_MSG_DIALOG_BUTTON_TYPE_WAIT_CANCEL = 6,
	SCE_MSG_DIALOG_BUTTON_TYPE_YESNO_FOCUS_NO = 7,
	SCE_MSG_DIALOG_BUTTON_TYPE_OK_CANCEL_FOCUS_CANCEL = 8,
	SCE_MSG_DIALOG_BUTTON_TYPE_2BUTTONS = 9,
} SceMsgDialogButtonType;

typedef enum SceMsgDialogProgressBarType {
	SCE_MSG_DIALOG_PROGRESSBAR_TYPE_PERCENTAGE = 0,
	SCE_MSG_DIALOG_PROGRESSBAR_TYPE_PERCENTAGE_CANCEL = 1,
} SceMsgDialogProgressBarType;

typedef enum SceMsgDialogSystemMessageType {
	SCE_MSG_DIALOG_SYSMSG_TYPE_TRC_EMPTY_STORE = 0,
	SCE_MSG_DIALOG_SYSMSG_TYPE_TRC_PSN_CHAT_RESTRICTION = 1,
	SCE_MSG_DIALOG_SYSMSG_TYPE_TRC_PSN_UGC_RESTRICTION = 2,
	SCE_MSG_DIALOG_SYSMSG_TYPE_CAMERA_NOT_CONNECTED = 4,
	SCE_MSG_DIALOG_SYSMSG_TYPE_WARNING_PROFILE_PICTURE_AND_NAME_NOT_SHARED = 5,
} SceMsgDialogSystemMessageType;

typedef struct SceMsgDialogButtonsParam {
	const char *msg1;
	const char *msg2;
	char reserved[32];
} SceMsgDialogButtonsParam;

typedef struct SceMsgDialogUserMessageParam {
	SceMsgDialogButtonType buttonType;
	int :32;
	const char *msg;
	SceMsgDialogButtonsParam *buttonsParam;
	char reserved[24];
} SceMsgDialogUserMessageParam;

typedef struct SceMsgDialogProgressBarParam {
	SceMsgDialogProgressBarType barType;
	int :32;
	const char *msg;
	char reserved[64];
} SceMsgDialogProgressBarParam;

typedef struct SceMsgDialogSystemMessageParam {
	SceMsgDialogSystemMessageType sysMsgType;
	char reserved[32];
} SceMsgDialogSystemMessageParam;

typedef struct SceMsgDialogParam {
	SceCommonDialogBaseParam baseParam;
	size_t size;
	SceMsgDialogMode mode;
	int :32;
	SceMsgDialogUserMessageParam *userMsgParam;
	SceMsgDialogProgressBarParam *progBarParam;
	SceMsgDialogSystemMessageParam *sysMsgParam;
	SceUserServiceUserId userId;
	char reserved[40];
	int :32;
} SceMsgDialogParam;

// WebDialog

typedef enum SceWebBrowserDialogMode {
	SCE_WEB_BROWSER_DIALOG_MODE_DEFAULT = 1,
	SCE_WEB_BROWSER_DIALOG_MODE_CUSTOM = 2,
} SceWebBrowserDialogMode;

typedef int32_t SceWebBrowserCallbackParamType;
typedef struct SceWebBrowserDialogCallbackInitParam {
	size_t size;
	SceWebBrowserCallbackParamType type;
	int :32;
	const char *data;
	char reserved[32];
} SceWebBrowserDialogCallbackInitParam;

typedef struct SceWebBrowserDialogImeParam {
	size_t size;
	uint32_t option;
	char reserved[256];
	int :32;
} SceWebBrowserDialogImeParam;

typedef struct SceWebBrowserDialogWebViewParam {
	size_t size;
	uint32_t option;
	char reserved[256];
	int :32;
} SceWebBrowserDialogWebViewParam;

typedef struct SceWebBrowserDialogParam {
	SceCommonDialogBaseParam baseParam;
	size_t size;
	SceWebBrowserDialogMode mode;
	SceUserServiceUserId userId;
	const char *url;
	SceWebBrowserDialogCallbackInitParam *callbackInitParam;
	uint16_t width;
	uint16_t height;
	uint16_t positionX;
	uint16_t positionY;
	uint32_t parts;
	uint16_t headerWidth;
	uint16_t headerPositionX;
	uint16_t headerPositionY;
	short :16;
	uint32_t control;
	SceWebBrowserDialogImeParam *imeParam;
	SceWebBrowserDialogWebViewParam *webviewParam;
	uint32_t animation;
	char reserved[202];
	short :16;
} SceWebBrowserDialogParam;

#define SCE_WEB_BROWSER_DIALOG_COOKIE_URL_SIZE (2048)
#define SCE_WEB_BROWSER_DIALOG_COOKIE_DATA_SIZE (4096)

typedef struct SceWebBrowserDialogSetCookieParam {
	size_t size;
	const char* url;
	const char* cookie;
	char reserved[256];
} SceWebBrowserDialogSetCookieParam;
