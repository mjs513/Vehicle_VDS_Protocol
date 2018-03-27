/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义冻结帧故障码和数据流数据处理函数
History:
	<author>	<time>		<desc>

************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freeze_lib.h"
#include "../formula/formula_comply.h"
#include "../public/public.h"
#include <assert.h>
#include "../InitConfigFromXml/init_config_from_xml_lib.h"
#include "../formula_parse/interface.h"
#include "ds_lib.h"
#include "../SpecialFunction/special_function.h"
#include "../function/dtc_lib.h"

int s_iUDSReadFreezeDTCCmdIndex = 0;//UDS读取冻结帧故障码命令索引，局部变量，本文件有效，为使得xml更简洁

STRUCT_SELECT_FUN stReadFreezeDTCFunGroup[] =
{
	{READ_FREEZE_DTC_BY_ISO14229, process_read_freeze_frame_DTC_by_ISO14229},
	{READ_FREEZE_DTC_BY_SAE_1939, process_read_freeze_frame_DTC_by_SAE1939},
};

/*************************************************
Description:	获取处理冻结帧读码函数
Input:
	cType		配置类型
Output:	保留
Return:	pf_general_function 函数指针
Others:
*************************************************/
pf_general_function get_read_freeze_DTC_fun( byte cType )
{
	int i = 0;

	for( i = 0; i < sizeof( stReadFreezeDTCFunGroup ) / sizeof( stReadFreezeDTCFunGroup[0] ); i++ )
		if( cType == stReadFreezeDTCFunGroup[i].cType )
			return stReadFreezeDTCFunGroup[i].pFun;

	return 0;
}

/*************************************************
Description:	冻结帧读码
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
void process_read_freeze_frame_DTC( void* pIn, void* pOut )
{
	pf_general_function pFun = NULL;

	pFun = get_read_freeze_DTC_fun( g_p_stProcessFunConfig->cFreezeDTCFunOffset );

	assert( pFun );

	pFun( pIn, pOut );
}

STRUCT_SELECT_FUN stReadFreezeDSFunGroup[] =
{
	{READ_FREEZE_DS_BY_ID,					process_read_freeze_frame_DS_by_ID},
	{READ_FIXED_FREEZE_DS,					process_read_freeze_frame_DS_by_fixed},
	{READ_FREEZE_DS_SAE1939,				process_read_freeze_frame_DS_SAE1939},

};
/*************************************************
Description:	获取处理冻结帧数据流函数
Input:
	cType		配置类型
Output:	保留
Return:	pf_general_function 函数指针
Others:
*************************************************/
pf_general_function get_read_freeze_DS_fun( byte cType )
{
	int i = 0;

	for( i = 0; i < sizeof( stReadFreezeDSFunGroup ) / sizeof( stReadFreezeDSFunGroup[0] ); i++ )
		if( cType == stReadFreezeDSFunGroup[i].cType )
			return stReadFreezeDSFunGroup[i].pFun;

	return 0;
}

/*************************************************
Description:	冻结帧读数据流
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
void process_read_freeze_frame_DS( void* pIn, void* pOut )
{
	pf_general_function pFun = NULL;

	pFun = get_read_freeze_DS_fun( g_p_stProcessFunConfig->cFreezeDSFunOffset );

	assert( pFun );

	pFun( pIn, pOut );
}
/*************************************************
Description:	读冻结帧故障码
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
void process_read_freeze_frame_DTC_by_ISO14229( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	int iReceiveResult = TIME_OUT;
	int iReceiveValidLen = 0;//接收到的有效字节长度
	int iDtcNum = 0;//处理状态

	byte cBufferOffset = 0;
	byte cConfigType = 0;
	int iConfigOffset = 0;
	int iReadFreezeFrameDtcCmdIndex[2] = {1, 0};

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	iConfigOffset = atoi( pstParam->pcData ); //获得读冻结帧故障码配置ID

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	cConfigType = ( byte )atoi( pstParam->pcData ); //获得读冻结帧故障码配置模板号

	select_freeze_dtc_config( iConfigOffset, cConfigType );

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	iReadFreezeFrameDtcCmdIndex[1] = atoi( pstParam->pcData ); //获得读冻结帧故障码命令索引
	s_iUDSReadFreezeDTCCmdIndex = iReadFreezeFrameDtcCmdIndex[1];

	iReceiveResult = send_and_receive_cmd( iReadFreezeFrameDtcCmdIndex );

	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ iReadFreezeFrameDtcCmdIndex[1] ].cBufferOffset;

	iReceiveValidLen = g_stBufferGroup[cBufferOffset].iValidLen;

	switch( iReceiveResult )
	{
	case SUCCESS:
	{
		iDtcNum = process_Dtc_data( g_stBufferGroup[cBufferOffset].cBuffer, iReceiveValidLen, pOut, 0, 0 );

		if( !iDtcNum )
		{
			special_return_status( PROCESS_OK | HAVE_JUMP | HAVE_TIP, "return", "NO_FREEZE_DTC", 0,pOut );
		}
	}
	break;

	case NEGATIVE:
	case FRAME_TIME_OUT:
	case TIME_OUT:
	default:
		general_return_status( iReceiveResult, g_stBufferGroup[cBufferOffset].cBuffer, 3, pOut );
		break;
	}
}

/*************************************************
Description:	SAE1939读冻结帧故障码
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
void process_read_freeze_frame_DTC_by_SAE1939( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	int iReceiveResult = TIME_OUT;
	int iReceiveValidLen = 0;//接收到的有效字节长度
	int iDtcNum = 0;//处理状态

	byte cBufferOffset = 0;
	byte cConfigType = 0;
	int iConfigOffset = 0;
	int iReadFreezeFrameDtcCmdIndex[2] = {1, 0};

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	iConfigOffset = atoi( pstParam->pcData ); //获得读冻结帧故障码配置ID

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	cConfigType = ( byte )atoi( pstParam->pcData ); //获得读冻结帧故障码配置模板号

	select_freeze_dtc_config( iConfigOffset, cConfigType );

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	iReadFreezeFrameDtcCmdIndex[1] = atoi( pstParam->pcData ); //获得读冻结帧故障码命令索引
	s_iUDSReadFreezeDTCCmdIndex = iReadFreezeFrameDtcCmdIndex[1];

	iReceiveResult = send_and_receive_cmd( iReadFreezeFrameDtcCmdIndex );

	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ s_iUDSReadFreezeDTCCmdIndex ].cBufferOffset;

	iReceiveValidLen = g_stBufferGroup[cBufferOffset].iValidLen;
    
    if ((g_stBufferGroup[cBufferOffset].cBuffer[1] ==0x00)&&(g_stBufferGroup[cBufferOffset].cBuffer[2] ==0x00)&&(g_stBufferGroup[cBufferOffset].cBuffer[3] ==0x00)&&(g_stBufferGroup[cBufferOffset].cBuffer[4] ==0x00))
    {
		special_return_status( PROCESS_FAIL| NO_JUMP | HAVE_TIP, NULL, "ID_WITHOUT_FREZZE", 0, pOut);
		return ;
    }
	switch( iReceiveResult )
	{
	case SUCCESS:
		{
			iDtcNum = process_freeze_DTC_data_by_SAE1939( g_stBufferGroup[cBufferOffset].cBuffer,
				iReceiveValidLen, pOut);

			if( !iDtcNum )
			{
				special_return_status( PROCESS_OK | HAVE_JUMP | HAVE_TIP, "return", "NO_FREEZE_DTC", 0,pOut );
			}
		}
		break;

	case NEGATIVE:
	case FRAME_TIME_OUT:
	case TIME_OUT:
	default:
		general_return_status( iReceiveResult, g_stBufferGroup[cBufferOffset].cBuffer, 3, pOut );
		break;
	}
}

/*************************************************
Description:	修改读取冻结帧数据流命令中的关键字节
Input:	pstParam	列表中DTC的ID
Return:	返回读冻结帧数据流的命令号
Others:	
*************************************************/
uint32 modify_freeze_ds_command( STRUCT_CHAIN_DATA_INPUT** pstParam )
{
	byte cBufferOffset			= 0;
	byte cReadFreezeDsMode		= 0;//读冻结帧数据流模式

	byte cFreezeDtcStartOffset	= 0;//故障码在回复命令中的起始偏移
	byte cDtcBytesInCmd			= 0;//故障码或记录号

	byte cModifyOffset			= 0;//冻结帧数据流修改命令的偏移
	byte cModifyBytes			= 0;//冻结帧数据流命令修改的字节数
	byte cRecordOffsetInDTC		= 0;//若按照记录号读冻结帧数据流，则存放记录号在故障码中的偏移
	uint32 uDtcID, uFreezeDsCmd;
	byte* cTemp = NULL;

	assert( (*pstParam)->pcData );
	assert( (*pstParam)->iLen != 0 );

	uDtcID = atoi( (*pstParam)->pcData ); //获得列表中DTC的ID

	*pstParam = (*pstParam)->pNextNode;

	assert( (*pstParam)->pcData );
	assert( (*pstParam)->iLen != 0 );

	uFreezeDsCmd = atoi( (*pstParam)->pcData ); //获得读冻结帧数据流命令索引
	*pstParam = (*pstParam)->pNextNode;

	cFreezeDtcStartOffset	= g_p_stUDSFreezeDtcConfig->cFreezeDtcStartOffset;	//回复中DTC起始偏移，从SID回复开始
	cDtcBytesInCmd			= g_p_stUDSFreezeDtcConfig->cDtcBytesInCmd;			//命令中几个字节表示一个故障码
	cModifyOffset			= g_p_stUDSFreezeDtcConfig->cModifyOffset;
	cModifyBytes			= g_p_stUDSFreezeDtcConfig->cModifyBytes;
	cRecordOffsetInDTC		= g_p_stUDSFreezeDtcConfig->cRecordOffsetInDTC;

	//获得存储冻结帧故障码或存储号的缓存偏移
	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ s_iUDSReadFreezeDTCCmdIndex ].cBufferOffset;

	cTemp = g_stBufferGroup[cBufferOffset].cBuffer + cFreezeDtcStartOffset;
	memcpy( ( g_stInitXmlGobalVariable.m_p_stCmdList[ uFreezeDsCmd ].pcCmd + cModifyOffset ),
		(  cTemp + uDtcID * cDtcBytesInCmd + cRecordOffsetInDTC),  cModifyBytes );
	return uFreezeDsCmd;

}
/*************************************************
Description:	读冻结帧数据流
Input:	pIn		列表中DTC的ID
Output:	pOut	输出数据地址
Return:	保留
Others:	根据ISO14229协议
*************************************************/
void process_read_freeze_frame_DS_by_ID( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	int iReceiveResult = TIME_OUT;
	int iReceiveValidLen = 0;//接收到的有效字节长度
	int iDtcID = 0;

	byte cBufferOffset = 0;
	int iProcessStatus;//冻结帧数据流处理状态
	byte cErrorDID[] = {0, 0};
	byte cConfigType = 0;
	int iConfigOffset = 0;
	byte cKeyOffset = 0;
	uint32 iFreezeDSCmdIndex[2] = {1, 0};

	iFreezeDSCmdIndex[1] = modify_freeze_ds_command( &pstParam );//修改冻结帧命令，并返回命令行号

	//获得存储冻结帧数据流的缓存偏移
	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ iFreezeDSCmdIndex[1] ].cBufferOffset;

	iReceiveResult = send_and_receive_cmd( iFreezeDSCmdIndex );

	iReceiveValidLen = g_stBufferGroup[cBufferOffset].iValidLen;

	switch( iReceiveResult )
	{
	case SUCCESS:
	{
		if (pstParam)
		{
			cKeyOffset = atoi(pstParam->pcData);
			if ( iReceiveValidLen < cKeyOffset )
			{
				special_return_status( PROCESS_OK | HAVE_JUMP | HAVE_TIP, "return", "NO_FREEZE_DS", 0, pOut);
				return;
			}
		}
		iProcessStatus = process_freeze_data_stream_by_ISO14229( g_stBufferGroup[cBufferOffset].cBuffer + cKeyOffset,
		                 iReceiveValidLen - cKeyOffset, pOut );

		if( iProcessStatus == 0 )
		{
			special_return_status( PROCESS_OK | HAVE_JUMP | HAVE_TIP, "return", "NO_FREEZE_DS", 0, pOut);
		}
	}
	break;

	case NEGATIVE:
	case FRAME_TIME_OUT:
	case TIME_OUT:
	default:
		general_return_status( iReceiveResult, g_stBufferGroup[cBufferOffset].cBuffer, 3, pOut );
		break;
	}
}
/*************************************************
Description:	请求支持冻结帧的故障码或故障码存储号
Input:
	pcDctData	故障码回复命令存储地址
	iValidLen	有效长度

Output:	pstDtc	输出链表指针
Return:	int 处理状态
Others:	本函数按照SAE1939协议处理，读取命令
		为：
*************************************************/
int process_freeze_DTC_data_by_SAE1939( byte* pcDctData, int iValidLen, void* pOut )
{
	int i = 0;
	int iDtcNum = 0, iDataLen;
	byte *pcDtcStart = NULL;//故障码起始存放地址
	UNN_2WORD_4BYTE DTCByte;
	byte DTC_SPN[50] = {0};
	byte cDtcStatusCache[256] = {0};

	byte cFreezeDtcStartOffset	= g_p_stUDSFreezeDtcConfig->cFreezeDtcStartOffset;	//有效回复中保存快照DTC起始偏移，从SID回复开始
	byte cDtcBytesInCmd			= g_p_stUDSFreezeDtcConfig->cDtcBytesInCmd;	//命令中几个字节表示一个故障码
	byte cDtcBytesInDisplay		= g_p_stUDSFreezeDtcConfig->cDtcBytesInDisplay;//一个故障码需要显示几个字节

    if (pcDctData[0] == 0) //没有冻结帧故障码
    {
		return 0;
    }
	else
	{
		iDtcNum = 1;
	}
	pcDtcStart = pcDctData + cFreezeDtcStartOffset;

	iDataLen = special_return_status( PROCESS_OK | NO_JUMP | NO_TIP, NULL, NULL, 0, pOut );
	
	for( i = 0; i < iDtcNum; i++ )
	{
		DTCByte.u32Bit = 0;
		DTCByte.u8Bit[2] = (pcDtcStart[i * cDtcBytesInCmd+2]>>5)&0x07;
		DTCByte.u8Bit[1] = pcDtcStart[i * cDtcBytesInCmd+1];
		DTCByte.u8Bit[0] = pcDtcStart[i * cDtcBytesInCmd+0];

		get_J1939Dtc_status( pcDtcStart[i * cDtcBytesInCmd+2], cDtcStatusCache, 0x1f );
		iDataLen = add_data_and_controlsID( iDataLen, DTCByte.u8Bit, 3, J1939_SPN, cDtcStatusCache, pOut );
	}
	return iDtcNum;
}



/*************************************************
Description:	处理冻结帧的数据流
Input:
	pcDctData	故障码回复命令存储地址
	iValidLen	有效长度

Output:	pOut	输出数据地址（出现匹配错误时
				存放出错时的DID）
Return:	void
Others:	本函数按照IS014229协议处理，读取命令
		为：19 04/05
*************************************************/
int process_freeze_data_stream_by_ISO14229( byte* pcDsData, int iValidLen,  void* pOut )
{
	byte i = 0, m = 0, cFreezeDsSum = 0;
	byte cDataValueCache[128] = {0};
	byte *pcFreezeDsData = NULL;
	int iDataLen = 0;
	byte cSumOffset = 0;

	UNN_2WORD_4BYTE iDsID;

	pcFreezeDsData = pcDsData;//指向第一个DID

	iDataLen = special_return_status( PROCESS_OK | NO_JUMP | NO_TIP, NULL, NULL, 0, pOut );
	for( i = 0; i < iValidLen - 2; )   //数据流总数
	{
		for ( m = 0; m < g_p_stFreezeDSFormulaConfig->cItemSum; m++ )
		{
			iDsID.u32Bit = g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].iDSID;
			if( (pcFreezeDsData[i] *256 + pcFreezeDsData[i + 1]) == iDsID.u32Bit / 0x100 )
			{
				cFreezeDsSum++;
				memset( cDataValueCache, 0, sizeof( cDataValueCache ) );//将缓存清空
				if( FORMULA_PARSER == g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].cFormulaType )
				{
					//调用公式解析器
					calculate(	pcFreezeDsData + i + 2 + g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].pcValidByteOffset[0],
						g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].pcValidByteNumber[0],
						g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].pcFormula,
						g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].iFormulaLen,
						g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].pStrFormat,
						cDataValueCache
						);
				}
				else
				{
					process_freeze_ds_calculate( iDsID.u32Bit, pcFreezeDsData + i + 2 + g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].pcValidByteOffset[0],
						cDataValueCache );
				}
				iDataLen = add_data_and_controlsID( iDataLen, iDsID.u8Bit, 4, DEC_LITTLE_ENDIAN, cDataValueCache, pOut );
				cSumOffset = g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].pcValidByteOffset[0] + g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[m].pcValidByteNumber[0];
				//break;//结束循环
			}
		}
		i += 2 + cSumOffset;
		cSumOffset = 0;
	}
	return cFreezeDsSum;
}

/*************************************************
Description:	读冻结帧数据流（数据流是固定的，不通过ID查找，直接显示XML中的配置项）
Input:	pIn		列表中DTC的ID
Output:	pOut	输出数据地址
Return:	保留
Others:	根据ISO14229协议
*************************************************/
void process_read_freeze_frame_DS_by_fixed( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	int iReceiveResult = TIME_OUT;
	int iReceiveValidLen = 0;//接收到的有效字节长度
	int iDtcID = 0;

	byte cBufferOffset = 0;
	int iProcessStatus;//冻结帧数据流处理状态
	byte cErrorDID[] = {0, 0};
	byte cConfigType = 0;
	int iConfigOffset = 0;
	byte cKeyOffset = 0;
	uint32 iFreezeDSCmdIndex[2] = {1, 0};

	iFreezeDSCmdIndex[1] = modify_freeze_ds_command( &pstParam );//修改冻结帧命令，并返回命令行号

	//获得存储冻结帧数据流的缓存偏移
	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ iFreezeDSCmdIndex[1] ].cBufferOffset;

	iReceiveResult = send_and_receive_cmd( iFreezeDSCmdIndex );

	iReceiveValidLen = g_stBufferGroup[cBufferOffset].iValidLen;

	switch( iReceiveResult )
	{
	case SUCCESS:
		{
			pstParam = pstParam->pNextNode;
			if (pstParam)
			{
				cKeyOffset = atoi(pstParam->pcData);
				if ( iReceiveValidLen < cKeyOffset )
				{
					special_return_status( PROCESS_OK | HAVE_JUMP | HAVE_TIP, "return", "NO_FREEZE_DS", 0, pOut);
					return;
				}
			}
			iProcessStatus = process_freeze_data_stream_by_xml( g_stBufferGroup[cBufferOffset].cBuffer + cKeyOffset,
				iReceiveValidLen - cKeyOffset, pOut );
		}
		break;

	case NEGATIVE:
	case FRAME_TIME_OUT:
	case TIME_OUT:
	default:
		general_return_status( iReceiveResult, g_stBufferGroup[cBufferOffset].cBuffer, 3, pOut );
		break;
	}
}
/*************************************************
Description:	处理冻结帧的数据流
Input:
pcDctData	故障码回复命令存储地址
iValidLen	有效长度

Output:	pOut	输出数据地址（出现匹配错误时
存放出错时的DID）
Return:	void
Others:	本函数按照IS014229协议处理，读取命令
为：19 04/05
*************************************************/
int process_freeze_data_stream_by_xml( byte* pcDsData, int iValidLen,  void* pOut )
{
	byte i = 0;
	byte cDataValueCache[128] = {0};
	byte *pcFreezeDsData = NULL;
	int iDataLen = 0;
	UNN_2WORD_4BYTE iDsID;
	byte cNegtiveDisplay[] = {"ID_STR_NEG_DS"};

	pcFreezeDsData = pcDsData;//指向第一个DID

	iDataLen = special_return_status( PROCESS_OK | NO_JUMP | NO_TIP, NULL, NULL, 0, pOut );

	for( i = 0; i < g_p_stFreezeDSFormulaConfig->cItemSum; i++)   //数据流总数
	{
		iDsID.u32Bit = g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[i].iDSID;

		memset( cDataValueCache, 0, sizeof( cDataValueCache ) );//将缓存清空
		if ( g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[i].pcValidByteOffset[0] > iValidLen )
		{
			strcpy( cDataValueCache, cNegtiveDisplay);
		}
		else if( FORMULA_PARSER == g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[i].cFormulaType )
		{
			//调用公式解析器
			calculate(	pcFreezeDsData + g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[i].pcValidByteOffset[0],
				g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[i].pcValidByteNumber[0],
				g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[i].pcFormula,
				g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[i].iFormulaLen,
				g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[i].pStrFormat,
				cDataValueCache
				);
		}
		else
		{
			process_freeze_ds_calculate( iDsID.u32Bit, pcFreezeDsData + g_p_stFreezeDSFormulaConfig->pstDSFormulaConfig[i].pcValidByteOffset[0],
				cDataValueCache );
		}
		iDataLen = add_data_and_controlsID( iDataLen, iDsID.u8Bit, 4, DEC_LITTLE_ENDIAN, cDataValueCache, pOut );
	}
	return i;
}
/*************************************************
Description:	读冻结帧数据流
Input:	pIn		列表中DTC的ID
Output:	pOut	输出数据地址
Return:	保留
Others:	根据SAE1939协议
*************************************************/
void process_read_freeze_frame_DS_SAE1939( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	int iReceiveValidLen = 0;//接收到的有效字节长度
	int iDtcID = 0;

	byte cBufferOffset = 0;
	int iProcessStatus,iReceiveResult;//冻结帧数据流处理状态
	byte cErrorDID[] = {0, 0};
	byte cConfigType = 0;
	int iConfigOffset = 0;
	int iReadFreezeFrameDSCmdIndex[2] = {1, 0};

	iReadFreezeFrameDSCmdIndex[1] = atoi( pstParam->pcData ); //获得读冻结帧故障码命令索引

	iReceiveResult = send_and_receive_cmd( iReadFreezeFrameDSCmdIndex );

	switch( iReceiveResult )
	{
	case SUCCESS:
		cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ iReadFreezeFrameDSCmdIndex[1] ].cBufferOffset;

		iReceiveValidLen = g_stBufferGroup[cBufferOffset].iValidLen;

		if ((g_stBufferGroup[cBufferOffset].cBuffer[1] ==0x00)&&(g_stBufferGroup[cBufferOffset].cBuffer[2] ==0x00)&&(g_stBufferGroup[cBufferOffset].cBuffer[3] ==0x00)&&(g_stBufferGroup[cBufferOffset].cBuffer[4] ==0x00))
		{
			special_return_status( PROCESS_FAIL| NO_JUMP | HAVE_TIP, NULL, "ID_WITHOUT_FREZZE", 0, pOut);
			return ;
		}
		if ( iReceiveValidLen <= 13 )
		{
			special_return_status( PROCESS_OK | HAVE_JUMP | HAVE_TIP, "return", "NO_FREEZE_DS", 0, pOut);
			return;
		}
		iProcessStatus = process_freeze_data_stream_by_xml( g_stBufferGroup[cBufferOffset].cBuffer, iReceiveValidLen, pOut );
		break;

	case NEGATIVE:
	case FRAME_TIME_OUT:
	case TIME_OUT:
	default:
		general_return_status( iReceiveResult, g_stBufferGroup[cBufferOffset].cBuffer, 3, pOut );
		break;
	}
}
