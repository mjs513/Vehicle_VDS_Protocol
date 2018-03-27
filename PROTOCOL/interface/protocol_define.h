/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义通用的数据类型、部分宏定义和消息类型等
History:
	<author>	<time>		<desc>

************************************************************************/


#ifndef _PROTOCOL_DEFINE_H
#define _PROTOCOL_DEFINE_H

typedef enum {false = 0, true} bool;

typedef unsigned char byte;  //无符号8位整型

typedef unsigned char	 uint8;
typedef unsigned short   uint16; //无符号16位整型
typedef unsigned int     uint32; //无符号32位整型

typedef union
{
	uint32 u32Bit;
	uint16 u16Bit[2];
	byte  u8Bit[4];
} UNN_2WORD_4BYTE;

#define FRAME_HEAD_SET_ALL	0X80
#define FRAME_HEAD_GET_ALL	0XA0
#define FRAME_HEAD_TEST_CONNECTION	0XC0
#define FRAME_HEAD_NORMAL_FRAME	0XE0
#define FRAME_HEAD_SAE1939_FRAME	0XE1
#define FRAME_HEAD_MULTI_FRAME	0XF0
#define FRAME_HEAD_ACTIVE_ECU	0XC2
#define FRAME_HEAD_SET_IDLE_LINK 0X8F
#define FRAME_HEAD_RESET_VCI 0XC1
#define FRAME_HEAD_RESET_CAN_FC_FRAME 0X91
#define FRAME_HEAD_ECU_REPLAY 0X92
#define FRAME_HEAD_SEND_RECEIVE 0X90




enum SET_CONFIG_FUN
{
    SET_CONFIG_FC_CAN = 0,
    SET_ECU_REPLAY,
    SET_SEND_AND_RECEIVE,
	SET_MUL_DS_MODE,
	SET_MUL_FRAME_OFFSET,
};

enum RETURN_TYPE
{
    TIME_OUT = -1,
    SUCCESS,
    FRAME_TIME_OUT,
    PROTOCOL_ERROR,
    FAIL,
    FORMAT_ERORR,
    NULL_POINT,
    RECEIVE_FRAME_TYPE_ERROR,
    MESSAGE_TYPE_ERROR,
    NEGATIVE,
    MATCH_ERROR,
    NO_FREEZE_DS,
    NO_FREEZE_DTC,
    NO_CURRENT_DTC,
    NO_HISTORY_DTC,
    INVALID_INPUT,
};

enum CAN_MESSAGE_TYPE
{
    SINGLE_FRAME = 0,
    FIRST_FRAME,
    CONSECUTIVE_FRAME,
    FLOW_CONTROL_FRAME,
};

typedef struct _STRUCT_STORAGE_BUFFER
{
	int iValidLen;
	byte cBuffer[512];
} STRUCT_STORAGE_BUFFER;

//预定义15组缓存
extern STRUCT_STORAGE_BUFFER	g_stBufferGroup[15];

typedef struct _STRUCT_CMD
{
	byte cBufferOffset;
	byte cReserved;
	int iCmdLen;
	byte* pcCmd;
} STRUCT_CMD;


typedef struct _STRUCT_DS
{
	byte cStartOffset;
	byte cValidByteLen;
	int	 iDSCmdID;
} STRUCT_DS;

typedef struct _STRUCT_INFORM
{
	byte cDisplayID;
	byte cStartOffset;
	byte cValidByteLen;
	int	 iInformCmdOffset;
	byte cDisplayStyle;
} STRUCT_INFORM;

typedef struct _STRUCT_INFORM_CONFIG
{
	byte cDisplayItemSum;
	STRUCT_INFORM* pstInforGroup;
} STRUCT_INFORM_CONFIG;

typedef struct _STRUCT_FREEZE_CONFIG
{
	byte cDIDHighByte;
	byte cDIDLowByte;
	byte cNeedByteSum;
	byte cDSItemSum;
	byte* pcSpecificDIDRule;
} STRUCT_FREEZE_CONFIG;

typedef struct _STRUCT_DATA
{
	byte cLenHighByte;
	byte cLenLowByte;
	byte* pcData;
} STRUCT_DATA;

typedef struct _STRUCT_SEND_CMD
{
	byte cFrameHead;
	byte cFrameLen[2];
	byte cReserved;
	STRUCT_CMD* pstFrameContent;
	byte cParityByte;
} STRUCT_SEND_CMD;

typedef void ( *pf_general_function )( void *, void * );

typedef struct _STRUCT_SELECT_FUN
{
	byte cType;
	pf_general_function pFun;
} STRUCT_SELECT_FUN;

typedef bool ( *pf_active_function )( void *, void * );

typedef struct _STRUCT_ACTIVE_FUN
{
	byte cType;
	pf_active_function pFun;
} STRUCT_ACTIVE_FUN;


// 输入参数 ***********************************************************************************************
typedef struct _STRUCT_CHAIN_DATA_INPUT
{
	int iLen;
	byte* pcData;
	struct _STRUCT_CHAIN_DATA_INPUT* pNextNode;
} STRUCT_CHAIN_DATA_INPUT;



// 输出参数 *************************************************************************************************
typedef struct _STRUCT_CHAIN_DATA_NODE
{
	byte cLenHighByte;
	byte cLenLowByte;
	byte* pcData;
	struct _STRUCT_CHAIN_DATA_NODE* pNextNode;
} STRUCT_CHAIN_DATA_NODE;

typedef struct _STRUCT_CHAIN_DATA_OUTPUT
{
	byte cKeyByte[3];
	STRUCT_DATA stJump;
	STRUCT_DATA stTip;
	STRUCT_CHAIN_DATA_NODE *pstData;
} STRUCT_CHAIN_DATA_OUTPUT;


#endif