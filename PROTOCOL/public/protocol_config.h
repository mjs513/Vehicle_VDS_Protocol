/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	声明与协议有关的配置变量、全局的消息提示等
History:
	<author>	<time>		<desc>

************************************************************************/
#ifndef _PROTOCOL_CONFIG_H
#define _PROTOCOL_CONFIG_H

#include "../interface/protocol_define.h"
// CAN协议设置 **********************************************************************************************
typedef struct _STRUCT_ISO15765_CONFIG
{
	uint16	u16Code7F78Timeout;		//7F78消极代码的超时时间
	uint16	u16ECUResTimeout;		//等待ECU回复的超时时间
	uint16	u16TimeBetweenFrames;	//主动发送时的帧间隔
	byte	cRetransTime;			//重发次数
	byte	cReserved;				//保留字节，对于CAN协议没用
	byte	cMultiframestime;       //多帧帧间隔时间
} STRUCT_ISO15765_CONFIG;

// KWP协议设置 **********************************************************************************************
typedef struct _STRUCT_ISO14230_CONFIG
{
	uint16	u16Code7F78Timeout;		//7F78消极代码的超时时间
	uint16	u16ECUResTimeout;		//等待ECU回复的超时时间
	uint16	u16TimeBetweenFrames;	//主动发送时的帧间隔
	byte	cRetransTime;			//重发次数
	byte	cTimeBetweenBytes;		//主动发送字节间时间
} STRUCT_ISO14230_CONFIG;

// 1939协议设置 **********************************************************************************************
typedef struct _STRUCT_SAE1939_CONFIG
{
	uint16  u16TimeBetweenFrames;	//主动发送时的帧间隔
	uint16	u16ECUResTimeout;		//等待ECU回复的超时时间
	uint16  u16VDI5ByteTimeout;     //VDI应答5个确认字节回复的时间
	byte	cRetransTime;			//重发次数
} STRUCT_SAE1939_CONFIG;


// 读故障码设置 *********************************************************************************************
typedef struct _STRUCT_READ_DTC_CONFIG
{
	byte cDtcStartOffset;	//有效回复中保存DTC起始偏移，从SID回复开始
	byte cDtcBytesInCmd;	//命令中几个字节表示一个故障码
	byte cStatusOffset;		//故障码状态在一个完整故障码中的偏移
	byte cDtcBytesInDisplay;//一个故障码需要显示几个字节
	byte cCurrentDtcMask;	//当前故障码mask
	byte cHistoryDtcMask;	//历史故障码mask
// 	bool bDTCMaskDisplay;   //故障码状态是否显示
} STRUCT_READ_DTC_CONFIG;

// UDS冻结帧故障码设置 ***************************************************************************************
typedef struct _STRUCT_UDS_FREEZE_DTC_CONFIG
{
	byte cFreezeDtcStartOffset;		//有效回复中保存冻结帧DTC起始偏移，从SID回复开始
	byte cDtcBytesInCmd;			//命令中几个字节表示一个故障码
	byte cDtcBytesInDisplay;		//一个故障码需要显示几个字节
	byte cModifyOffset;				//在发送读冻结帧数据流命令中修改字节的偏移
	byte cModifyBytes;				//修改的字节个数
	byte cRecordOffsetInDTC;		//存储号在一个完整冻结帧故障码中的偏移
} STRUCT_UDS_FREEZE_DTC_CONFIG;

// VCI设置 **************************************************************************************************
typedef struct	_STRUCT_VCI_PARAMS
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
	byte cCanFilterID[40];
} STRUCT_VCI_PARAMS;
// 激活系统设置 *********************************************************************************************
typedef struct	_STRUCT_ACTIVE_ECU_CONFIG
{
	byte cTimeBetweenBytes;		//字节与字节间时间
	byte cActiveMode;			//激活方式
	byte cDetectBandRateFlag;	//自检标志 0x00:不需要 0x01:需要
	uint16	u16ECUResByteTimeout;//等待ECU回复的字节超时时间
	byte cBandRate[3];			//波特率
	byte cActiveAddress[10];	//激活地址第一个字节表示地址长度
	byte cReceiveKeyByte[5];	//接收ECU回复的关键字第一个字节表示地址长度
} STRUCT_ACTIVE_ECU_CONFIG;



// 处理函数配置 *********************************************************************************************
typedef struct _STRUCT_PROCESS_FUN_CONFIG
{
	byte cActiveECUFunOffset;
	byte cActuatorTestFunOffset;
	byte cClearDTCFunOffset;
	byte cDSFunOffset;
	byte cCurrentDTCFunOffset;
	byte cHistoryDTCFunOffset;
	byte cFreezeDTCFunOffset;
	byte cFreezeDSFunOffset;
	byte cInforFunOffset;
	byte cQuitSystemFunOffset;

} STRUCT_PROCESS_FUN_CONFIG;

// 安全进入配置 *********************************************************************************************
typedef struct _STRUCT_SECURITY_ACCSEE_CONFIG
{
	int iDefaultSessionCmdID;
	int iExtendSessionCmdID;
	int iRequestSeedCmdID;
	int iSendKeyCmdID;
} STRUCT_SECURITY_ACCSEE_CONFIG;
// 数据流计算公式配置 ***************************************************************************************
typedef struct _STRUCT_DS_FORMULA_CONFIG
{
	int  iDSID;
	int	 * piDSCmdID;
	int iDsCmdIDLen;
	byte * pcValidByteOffset;
	int iValidByteOffsetLen;
	byte * pcValidByteNumber;
	byte iValidByteNumberLen;
	byte cFormulaType;
	byte * pcFormula;
	int  iFormulaLen;
	byte * pStrFormat;
} STRUCT_DS_FORMULA_CONFIG;
// 数据流计算公式组的配置 *****************************************************************************************
typedef struct _STRUCT_DS_FORMULA_GROUP_CONFIG
{
	byte cItemSum;
	STRUCT_DS_FORMULA_CONFIG * pstDSFormulaConfig;
} STRUCT_DS_FORMULA_GROUP_CONFIG;

// 空闲链接的配置 *******************************************************************************************
typedef struct _STRUCT_IDLE_LINK_CONFIG
{
	byte cTimeBetweenBytes;			//字节间时间
	byte cIdleLinkStatus;			//空闲链接状态
	uint16	u16TimeBetweenGroups;	//主动发送时组与组的间隔
	uint16	u16TimeBetweenFrames;	//主动发送时帧与帧的间隔
	byte cResponseFlag;				//需不需要接收空闲链接回复的标志 0：不需要 1：需要
	byte cCalculateChecksumFlag;	//计算校验和标志 0：不需要计算校验和 非0：需要计算校验和
	byte cIdleLinkMode;		//空闲链接命令处理 0x81:按照玉柴协议方式处理
} STRUCT_IDLE_LINK_CONFIG;

//数据流新模式设置 *******************************************************************************************
typedef struct _STRUCT_DS_MUL_CONFIG
{
	byte cDsMode;
	byte cDsTimeOut;
	byte cDsFrameTime;
}STRUCT_DS_MUL_CONFIG;
//各功能多帧收取配置 *******************************************************************************************
typedef struct _STRUCT_FUN_MUL_CONFIG
{
	byte cDsOffset;
	byte cDtcOffset;
	byte cFreezeOffset;
	byte cInforOffset;
}STRUCT_FUN_MUL_CONFIG;
// 全局变量声明**********************************************************************************************
/*
存储功能配置,数据大小不定
*/

extern uint32 u32Config_fuc_Group[8][16];
extern uint32 *u32Config_fuc;


extern STRUCT_VCI_PARAMS *g_p_stVciParamsGroup[5];
extern STRUCT_VCI_PARAMS *g_p_stVCI_params_config;

extern STRUCT_ACTIVE_ECU_CONFIG *g_p_stGeneralActiveEcuConfigGroup[5];
extern STRUCT_ACTIVE_ECU_CONFIG *g_p_stGeneralActiveEcuConfig;

extern STRUCT_READ_DTC_CONFIG *g_p_stGeneralReadDtcConfigGroup[5];
extern STRUCT_READ_DTC_CONFIG *g_p_stGeneralReadDtcConfig;

extern STRUCT_ISO15765_CONFIG *g_p_stISO15765ConfigGroup[5];
extern STRUCT_ISO15765_CONFIG *g_p_stISO15765Config;

extern STRUCT_ISO14230_CONFIG *g_p_stISO14230ConfigGroup[5];
extern STRUCT_ISO14230_CONFIG *g_p_stISO14230Config;

extern STRUCT_SAE1939_CONFIG *g_p_stSAE1939ConfigGroup[5];
extern STRUCT_SAE1939_CONFIG *g_p_stSAE1939Config;

extern STRUCT_UDS_FREEZE_DTC_CONFIG *g_p_stUDSFreezeDtcConfigGroup[5];
extern STRUCT_UDS_FREEZE_DTC_CONFIG *g_p_stUDSFreezeDtcConfig;

extern STRUCT_PROCESS_FUN_CONFIG *g_p_stProcessFunConfigGroup[5];
extern STRUCT_PROCESS_FUN_CONFIG *g_p_stProcessFunConfig;

extern STRUCT_SECURITY_ACCSEE_CONFIG *g_p_stSecurityAccessConfigGroup[5];
extern STRUCT_SECURITY_ACCSEE_CONFIG *g_p_stSecurityAccessConfig;

extern STRUCT_DS_FORMULA_GROUP_CONFIG *g_p_stInformationGroupConfigGroup[10];
extern STRUCT_DS_FORMULA_GROUP_CONFIG *g_p_stInformationGroupConfig;

extern STRUCT_DS_FORMULA_GROUP_CONFIG *g_p_stFreezeDSFormulaConfig;

extern STRUCT_DS_FORMULA_GROUP_CONFIG g_stGeneralDSFormulaGroupConfig;
extern STRUCT_DS_MUL_CONFIG g_DsMulConfig;//数据流多帧配置
extern STRUCT_FUN_MUL_CONFIG g_KwpFunSaveOffset;


extern STRUCT_IDLE_LINK_CONFIG *g_p_stIdleLinkConfigGroup[5];
extern STRUCT_IDLE_LINK_CONFIG *g_p_stIdleLinkConfig;

extern const byte g_cTime_out[];
extern const byte g_cSuccess[];
extern const byte g_cFrame_time_out[];
extern const byte g_cProtocol_error[];
extern const byte g_cFaile[];
extern const byte g_cFormat_error[];
extern const byte g_cNull_point[];
extern const byte g_cNegative[];
extern const byte g_cMatch_error[];
extern const byte g_cNo_freeze_DS[];
extern const byte g_cNo_freeze_DTC[];
extern const byte g_cNo_current_dtc[];
extern const byte g_cNo_history_dtc[];
extern const byte g_cInvalid_input[];
extern const byte g_cNegative11[];
extern const byte g_cNegative12[];
extern const byte g_cNegative13[];
extern const byte g_cNegative22[];
extern const byte g_cNegative24[];
extern const byte g_cNegative31[];
extern const byte g_cNegative33[];
extern const byte g_cNegative35[];
extern const byte g_cNegative36[];
extern const byte g_cNegative37[];
extern const byte g_cNegative7F[];
extern const byte g_cNegativeOther[];
extern byte g_CANoffset;
extern bool g_DSProrocess;
extern byte g_MulCmdOffset;

#endif

