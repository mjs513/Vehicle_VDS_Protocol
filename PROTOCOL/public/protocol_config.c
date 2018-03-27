/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义与协议有关的配置变量、全局的消息提示等
History:
	<author>	<time>		<desc>

************************************************************************/

#include "protocol_config.h"
#include <stdio.h>

/*
typedef struct _STRUCT_ISO15765_CONFIG
{
uint16	u16Code7F78Timeout;		//7F78消极代码的超时时间
uint16	u16ECUResTimeout;		//等待ECU回复的超时时间
uint16	u16TimeBetweenFrames;	//主动发送时的帧间隔
byte	cRetransTime;			//重发次数
byte	cReserved;				//保留字节，对于CAN协议没用
byte	cFCCmdOffset;			//FC帧命令偏移
} STRUCT_ISO15765_CONFIG;
*/

STRUCT_ISO15765_CONFIG *g_p_stISO15765ConfigGroup[5] = {0};
STRUCT_ISO15765_CONFIG *g_p_stISO15765Config = NULL;
/*
typedef struct _STRUCT_ISO14230_CONFIG
{
uint16	u16Code7F78Timeout;		//7F78消极代码的超时时间
uint16	u16ECUResTimeout;		//等待ECU回复的超时时间
uint16	u16TimeBetweenFrames;	//主动发送时的帧间隔
byte	cRetransTime;			//重发次数
byte	cTimeBetweenBytes;		//主动发送字节间时间
}STRUCT_ISO14230_CONFIG;
*/

STRUCT_ISO14230_CONFIG *g_p_stISO14230ConfigGroup[5] = {0};
STRUCT_ISO14230_CONFIG *g_p_stISO14230Config = NULL;

/*
typedef struct _STRUCT_SAE1939_CONFIG
{
uint16	u16Code7F78Timeout;		//7F78消极代码的超时时间
uint16	u16ECUResTimeout;		//等待ECU回复的超时时间
uint16	u16TimeBetweenFrames;	//主动发送时的帧间隔
byte	cRetransTime;			//重发次数
byte	cReserved;				//保留字节，对于CAN协议没用
byte	cFCCmdOffset;			//FC帧命令偏移
} STRUCT_ISO15765_CONFIG;
*/

STRUCT_SAE1939_CONFIG *g_p_stSAE1939ConfigGroup[5] = {0};
STRUCT_SAE1939_CONFIG *g_p_stSAE1939Config = NULL;

/*
typedef struct
{
byte cDtcStartOffset;	//有效回复中保存DTC起始偏移，从SID回复开始
byte cDtcBytesInCmd;	//命令中几个字节表示一个故障码
byte cStatusOffset;		//故障码状态在一个完整故障码中的偏移
byte cDtcBytesInDisplay;//一个故障码需要显示几个字节
byte cCurrentDtcMask;	//当前故障码mask
byte cHistoryDtcMask;	//历史故障码mask
}STRUCT_READ_DTC_CONFIG;
*/

STRUCT_READ_DTC_CONFIG *g_p_stGeneralReadDtcConfigGroup[5] = {0};
STRUCT_READ_DTC_CONFIG *g_p_stGeneralReadDtcConfig = NULL;

/*
typedef struct _STRUCT_UDS_FREEZE_DTC_CONFIG
{
byte cFreezeDtcStartOffset;		//有效回复中保存冻结帧DTC起始偏移，从SID回复开始
byte cDtcBytesInCmd;			//命令中几个字节表示一个故障码
byte cDtcBytesInDisplay;		//一个故障码需要显示几个字节
byte cModifyOffset;				//在发送读冻结帧数据流命令中修改字节的偏移
byte cModifyBytes;				//修改的字节个数
byte cRecordOffsetInDTC;		//存储号在一个完整冻结帧故障码中的偏移
} STRUCT_UDS_FREEZE_DTC_CONFIG;
*/

STRUCT_UDS_FREEZE_DTC_CONFIG *g_p_stUDSFreezeDtcConfigGroup[5] = {0};
STRUCT_UDS_FREEZE_DTC_CONFIG *g_p_stUDSFreezeDtcConfig = NULL;


/*
typedef struct
{
byte cCommunicationType;
byte cVoltage;
byte cLevel;
byte cLogic;
byte cReceivePin;
byte cSendPin;
byte cCANFrameMode;
byte cECUBaudRate[3];
byte cVCIBaudRate[3];
byte cCanSamplingConfig[4];
byte cCanFilterIDGroupNum;
byte cCanFilterID[16];
}STRUCT_VCI_PARAMS;
*/
STRUCT_VCI_PARAMS *g_p_stVciParamsGroup[5] = {0};
byte g_CANoffset = 0;
bool g_DSProrocess = false; //是否是数据流功能
byte g_MulCmdOffset = 2;

STRUCT_VCI_PARAMS *g_p_stVCI_params_config = NULL;
/*
typedef struct	_STRUCT_ACTIVE_ECU_CONFIG
{
	byte cTimeBetweenBytes;		//字节与字节间时间
	byte cActiveMode;			//激活方式
	byte cDetectBandRateFlag;	//自检标志 0x00:不需要 0x01:需要
	uint16	u16ECUResByteTimeout;//等待ECU回复的字节超时时间
	byte cBandRate[3];			//波特率
	byte cActiveAddress[5];		//激活地址第一个字节表示地址长度
	byte cReceiveKeyByte[10];	//接收ECU回复的关键字第一个字节表示地址长度
} STRUCT_ACTIVE_ECU_CONFIG;
*/

STRUCT_ACTIVE_ECU_CONFIG *g_p_stGeneralActiveEcuConfigGroup[5] = {0};
STRUCT_ACTIVE_ECU_CONFIG *g_p_stGeneralActiveEcuConfig = NULL;



/*
typedef struct _STRUCT_PROCESS_FUN_CONFIG
{
byte cActiveECUFunOffset;
byte cActuatorTestFunOffset;
byte cClearDTCFunOffset;
byte cDSFunOffset;
byte cDTCFunOffset;
byte cFreezeDTCFunOffset;
byte cFreezeDSFunOffset;
byte cInforFunOffset;
byte cQuitSystemFunOffset;

}STRUCT_PROCESS_FUN_CONFIG;
*/
STRUCT_PROCESS_FUN_CONFIG *g_p_stProcessFunConfigGroup[5] = {0};
STRUCT_PROCESS_FUN_CONFIG *g_p_stProcessFunConfig = NULL;

/*
typedef struct _STRUCT_SECURITY_ACCSEE_CONFIG
{
byte cDefaultSessionCmdID;
byte cExtendSessionCmdID;
byte cRequestSeedCmdID;
byte cSendKeyCmdID;
}STRUCT_SECURITY_ACCSEE_CONFIG;
*/

STRUCT_SECURITY_ACCSEE_CONFIG *g_p_stSecurityAccessConfigGroup[5] = {0};
STRUCT_SECURITY_ACCSEE_CONFIG *g_p_stSecurityAccessConfig = NULL;

/*
typedef struct _STRUCT_DS_FORMULA_CONFIG
{
int  iDSID;
int	 iDSCmdID;
byte cValidByteOffset;
byte cValidByteNumber;
byte cFormulaType;
byte * pcFormula;
int  iFormulaLen;
byte * pStrFormat;
} STRUCT_DS_FORMULA_CONFIG;
*/

/*
typedef struct _STRUCT_INFORMATION_GROUP_CONFIG
{
byte cDisplayItemSum;
STRUCT_DS_FORMULA_CONFIG * pstDSFormulaConfig;
}STRUCT_DS_FORMULA_GROUP_CONFIG;
*/
STRUCT_DS_MUL_CONFIG g_DsMulConfig;//数据流多帧配置
STRUCT_FUN_MUL_CONFIG g_KwpFunSaveOffset; //K线功能函数收取多帧的字节偏移
STRUCT_DS_FORMULA_GROUP_CONFIG *g_p_stInformationGroupConfigGroup[10] = {0};
STRUCT_DS_FORMULA_GROUP_CONFIG	*g_p_stInformationGroupConfig = NULL;

STRUCT_DS_FORMULA_GROUP_CONFIG *g_p_stFreezeDSFormulaConfig = NULL;

STRUCT_DS_FORMULA_GROUP_CONFIG g_stGeneralDSFormulaGroupConfig = {0};

/*
typedef struct _STRUCT_IDLE_LINK_CONFIG
{
byte cTimeBetweenBytes;			//字节间时间
byte cIdleLinkStatus;			//空闲链接状态
uint16	u16TimeBetweenGroups;	//主动发送时组与组的间隔
uint16	u16TimeBetweenFrames;	//主动发送时帧与帧的间隔
byte cResponseFlag;				//需不需要接收空闲链接回复的标志 0：不需要 1：需要
byte cCalculateChecksumFlag;	//计算校验和标志 0：不需要计算校验和 非0：需要计算校验和
}STRUCT_IDLE_LINK_CONFIG;
*/

STRUCT_IDLE_LINK_CONFIG *g_p_stIdleLinkConfigGroup[5] = {0};
STRUCT_IDLE_LINK_CONFIG *g_p_stIdleLinkConfig = NULL;

/*
typedef struct _STRUCT_STORAGE_BUFFER
{
int iValidLen;
byte cBuffer[512];
}STRUCT_STORAGE_BUFFER;
*/
STRUCT_STORAGE_BUFFER	g_stBufferGroup[15] = //
{
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
};

uint32 u32Config_fuc_Group[8][16] = {0};
uint32 *u32Config_fuc = NULL;


//返回的提示内容
const byte g_cTime_out[]		= "TIME_OUT";
const byte g_cSuccess[]			= "SUCCESS";
const byte g_cFrame_time_out[]	= "FRAME_TIME_OUT";
const byte g_cProtocol_error[]	= "PROTOCOL_ERROR";
const byte g_cFaile[]			= "FAILE";
const byte g_cFormat_error[]	= "FORMAT_ERORR";
const byte g_cNull_point[]		= "NULL_POINT";
const byte g_cNegative[]		= "NEGATIVE";
const byte g_cMatch_error[]		= "MATCH_ERROE";
const byte g_cNo_freeze_DS[]	= "NO_FREEZE_DS";
const byte g_cNo_freeze_DTC[]	= "NO_FREEZE_DTC";
const byte g_cNo_current_dtc[]	= "NO_CURRENT_DTC";
const byte g_cNo_history_dtc[]	= "NO_HISTORY_DTC";
const byte g_cInvalid_input[]	= "INVALID_INPUT";
const byte g_cNegative11[] = "ID_STR_NEG_11";
const byte g_cNegative12[] = "ID_STR_NEG_12";
const byte g_cNegative13[] = "ID_STR_NEG_13";
const byte g_cNegative22[] = "ID_STR_NEG_22";
const byte g_cNegative24[] = "ID_STR_NEG_24";
const byte g_cNegative31[] = "ID_STR_NEG_31";
const byte g_cNegative33[] = "ID_STR_NEG_33";
const byte g_cNegative35[] = "ID_STR_NEG_35";
const byte g_cNegative36[] = "ID_STR_NEG_36";
const byte g_cNegative37[] = "ID_STR_NEG_37";
const byte g_cNegative7F[] = "ID_STR_NEG_7F";
const byte g_cNegativeOther[] = "ID_SRT_NEG_OTHTER";
