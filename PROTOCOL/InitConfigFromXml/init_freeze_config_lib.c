/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义从xml获取冻结帧配置处理函数
History:
	<author>	<time>		<desc>

***********************************************************************/
#include "..\interface\protocol_define.h"
#include "..\public\protocol_config.h"
#include "init_config_from_xml_lib.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/*************************************************
Description:	获取冻结帧故障码配置
Input:
	PIn				具体配置内容
Output:	保留
Return:	无
Others: 根据不同的配置模版进相应的分支
*************************************************/
void get_freeze_dtc_config_data( void* pIn )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte cConfigTemp[50] = {0};

	byte cDtcConfigType = 0;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	get_cmd_config_content_data( cConfigTemp, pstParam->pcData, NO_LENGTH_LIMIT );

	cDtcConfigType	= cConfigTemp[1];

	switch( cDtcConfigType )
	{
	case UDS_FREEZE_DTC_CONFIG:
	{
		get_UDS_freeze_dtc_config_data( cConfigTemp );
	}
	break;

	default:
		break;
	}

}

/*************************************************
Description:	获取UDS冻结帧故障码配置
Input:
	PIn				具体配置内容
Output:	保留
Return:	无
Others:
*************************************************/
void get_UDS_freeze_dtc_config_data( const byte * pcSource )
{
	byte cConfigOffset = 0;

	cConfigOffset = pcSource[0];

	if( g_p_stUDSFreezeDtcConfigGroup[cConfigOffset] == NULL )
		g_p_stUDSFreezeDtcConfigGroup[cConfigOffset] = ( STRUCT_UDS_FREEZE_DTC_CONFIG * )malloc( sizeof( STRUCT_UDS_FREEZE_DTC_CONFIG ) );

	g_p_stUDSFreezeDtcConfigGroup[cConfigOffset]->cFreezeDtcStartOffset	= pcSource[2];
	g_p_stUDSFreezeDtcConfigGroup[cConfigOffset]->cDtcBytesInCmd		= pcSource[3];
	g_p_stUDSFreezeDtcConfigGroup[cConfigOffset]->cDtcBytesInDisplay	= pcSource[4];
	g_p_stUDSFreezeDtcConfigGroup[cConfigOffset]->cModifyOffset			= pcSource[5];
	g_p_stUDSFreezeDtcConfigGroup[cConfigOffset]->cModifyBytes			= pcSource[6];
	g_p_stUDSFreezeDtcConfigGroup[cConfigOffset]->cRecordOffsetInDTC	= pcSource[7];

}

/*************************************************
Description:	选择故障码配置
Input:
	iConfigOffset		具体偏移
	g_cConfigType			故障码配置类型
Output:	保留
Return:	无
Others: 根据激活配置类型和配置偏移选择相应的配置
*************************************************/
void select_freeze_dtc_config( int iConfigOffset, const byte cConfigType )
{
	switch( cConfigType )
	{
	case UDS_FREEZE_DTC_CONFIG:
		g_p_stUDSFreezeDtcConfig = g_p_stUDSFreezeDtcConfigGroup[iConfigOffset];
		break;

	default:
		break;
	}
}

/*************************************************
Description:	释放存放冻结帧故障码配置的空间
Input:	无
Output:	保留
Return:	无
Others: 每添加一类配置就在该函数中添加相应的释放代码
并在quit_system_lib.c的free_xml_config_space
函数中调用该函数。
*************************************************/
void free_freeze_dtc_config_space( void )
{
	int i = 0;

	for( i = 0; i < sizeof( g_p_stUDSFreezeDtcConfigGroup ) / sizeof( g_p_stUDSFreezeDtcConfigGroup[0] ); i++ )
	{
		if( NULL != g_p_stUDSFreezeDtcConfigGroup[i] )
		{
			free( g_p_stUDSFreezeDtcConfigGroup[i] );
			g_p_stUDSFreezeDtcConfigGroup[i] = NULL;
		}
	}

}

/*************************************************
Description:	获取冻结帧故障码配置
Input:
	PIn				具体配置内容
Output:	保留
Return:	无
Others: 根据不同的配置模版进相应的分支
*************************************************/
void get_freeze_ds_config_data( void* pIn )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte * pcTemp = NULL;
	byte cConfigTemp[15] = {0};
	byte cDSConfigType = 0;
	int iLen = 0;
	int temp = 0;
	byte cConfigID = 0;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	pcTemp = pstParam->pcData;

	iLen = get_command_one_block_config_data( cConfigTemp, &temp, pcTemp ); //获得ID
	pcTemp += iLen;

	cConfigID = cConfigTemp[0];

	iLen = get_command_one_block_config_data( cConfigTemp, &temp, pcTemp ); //获得模板号即配置类型
	pcTemp += iLen;

	cDSConfigType	= cConfigTemp[0];

// 	switch( cDSConfigType )
// 	{
// 	case UDS_FREEZE_DS_CONFIG:
// 	{
// 		get_UDS_freeze_ds_config_data( cConfigID, pcTemp );
// 	}
// 	break;
// 
// 	default:
// 		break;
// 	}

}

