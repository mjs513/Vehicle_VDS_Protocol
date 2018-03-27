/************************************************************************
* Copyright (c) 2014,北京一雄信息科技有限公司
*
* 文件名称：clear_dtc_lib.h
* 文件标识：
* 摘    要：
* 当前版本：1.0
* 作    者：
* 完成日期：
*
* 取代版本：
* 原作者  ：
* 完成日期：
**************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include "clear_dtc_lib.h"
#include "..\public\public.h"
#include "..\command\command.h"
#include "..\interface\protocol_define.h"
#include "..\public\protocol_config.h"
#include <assert.h>
#include "..\InitConfigFromXml\init_config_from_xml_lib.h"
#include "..\SpecialFunction\special_function.h"

STRUCT_SELECT_FUN stClearDTCFunGroup[] =
{
	{GENERAL_CLEAR_DTC, process_general_clear_Dtc},
	{GENERAL_CLEAR_1939_DTC,process_general_clear_1939_Dtc},
};

/*************************************************
Description:	获取处理清码函数
Input      :	cType		配置类型
Output     :	保留
Return     :	pf_general_function 函数指针
Time       :
*************************************************/
pf_general_function get_clear_DTC_fun( byte cType )
{
	int i = 0;

	for( i = 0; i < sizeof( stClearDTCFunGroup ) / sizeof( stClearDTCFunGroup[0] ); i++ )
		if( cType == stClearDTCFunGroup[i].cType )
			return stClearDTCFunGroup[i].pFun;

	return 0;
}

/*************************************************
Description:	清除故障码
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
void process_clear_Dtc( void* pIn, void* pOut )
{
	pf_general_function pFun = NULL;

	pFun = get_clear_DTC_fun( g_p_stProcessFunConfig->cClearDTCFunOffset );

	assert( pFun );

	pFun( pIn, pOut );
}

/*************************************************
Description:	清除1939故障码函数
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
void process_general_clear_1939_Dtc( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	int iReceiveResult = TIME_OUT;
	int iReceiveValidLen = 0;//接收到的有效字节长度

	uint32 u32CmdIndex[50] = {0};//命令数据

	byte cBufferOffset  = 0;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	u32CmdIndex[0] = get_string_type_data_to_uint32( u32CmdIndex + 1, pstParam->pcData, pstParam->iLen );

	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ u32CmdIndex[1] ].cBufferOffset;

	iReceiveResult = send_and_receive_cmd( u32CmdIndex );

	iReceiveValidLen = g_stBufferGroup[cBufferOffset].iValidLen;

	if (iReceiveResult == SUCCESS)
    {
		switch (g_stBufferGroup[cBufferOffset].cBuffer[0]&0x03)
		{
			case 1:
				special_return_status( PROCESS_FAIL|NO_JUMP|HAVE_TIP, NULL, "CLEAR_Negation", 0, pOut );
				break;
			case 2:
				special_return_status( PROCESS_FAIL|NO_JUMP|HAVE_TIP, NULL, "CLEAR_Refuse", 0, pOut );
				break;
			case 3:
				special_return_status( PROCESS_FAIL|NO_JUMP|HAVE_TIP, NULL, "CLEAR_Unrespond", 0, pOut );
				break;
			default:
				special_return_status( PROCESS_OK | NO_JUMP | NO_TIP, NULL, NULL, 0, pOut );
				break;
		}
    }
	else
		general_return_status( iReceiveResult, g_stBufferGroup[cBufferOffset].cBuffer, 3, pOut );

}
/*************************************************
Description:	普通清除故障码函数
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
void process_general_clear_Dtc( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	int iReceiveResult = TIME_OUT;
	int iReceiveValidLen = 0;//接收到的有效字节长度

	uint32 u32CmdIndex[5] = {0};//命令数据

	byte cBufferOffset  = 0;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	u32CmdIndex[0] = get_string_type_data_to_uint32( u32CmdIndex + 1, pstParam->pcData, pstParam->iLen );

	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ u32CmdIndex[1] ].cBufferOffset;

	iReceiveResult = send_and_receive_cmd( u32CmdIndex );

	iReceiveValidLen = g_stBufferGroup[cBufferOffset].iValidLen;

	switch( iReceiveResult )
	{
		case SUCCESS:
			special_return_status( PROCESS_OK | NO_JUMP | NO_TIP, NULL, NULL, 0, pOut );
			break;

		case NEGATIVE:
		case FRAME_TIME_OUT:
		case TIME_OUT:
		default:
			general_return_status( iReceiveResult, g_stBufferGroup[cBufferOffset].cBuffer, 3, pOut );
			break;
	}
}