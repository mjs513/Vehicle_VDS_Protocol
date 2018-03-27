/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义从xml获取故障码配置处理函数
History:
	<author>	<time>		<desc>

***********************************************************************/
#include "..\interface\protocol_define.h"
#include "..\public\protocol_config.h"
#include "init_config_from_xml_lib.h"
#include <assert.h>
#include <stdlib.h>
#include <memory.h>

STRUCT_INIT_DTC_CONFIG stInitDTCConfigGroup[] =
{
	{GENERAL_DTC_CONFIG, get_general_dtc_config_data},
};

/*************************************************
Description:	获取故障码配置函数
Input:
	cConfigType		配置类型
Output:	保留
Return:	pf_parammeter_is_point_to_byte 函数指针
Others:
*************************************************/
pf_parammeter_is_point_to_byte get_init_DTC_config_fun( byte cConfigType )
{
	int i = 0;

	for( i = 0; i < sizeof( stInitDTCConfigGroup ) / sizeof( stInitDTCConfigGroup[0] ); i++ )
		if( cConfigType == stInitDTCConfigGroup[i].cDTCConfigType )
			return stInitDTCConfigGroup[i].pFun;

	return 0;//用于查错用
}

/*************************************************
Description:	获取故障码配置
Input:
	PIn				具体配置内容
Output:	保留
Return:	无
Others: 根据不同的配置模版进相应的分支
*************************************************/
void get_dtc_config_data( void* pIn )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte cConfigTemp[50] = {0};

	byte cDtcConfigType = 0;
	pf_parammeter_is_point_to_byte pFun = NULL;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	memset(cConfigTemp, 0, 50);
	get_cmd_config_content_data( cConfigTemp, pstParam->pcData, NO_LENGTH_LIMIT );

	cDtcConfigType	= cConfigTemp[1];

	pFun = get_init_DTC_config_fun( cDtcConfigType );

	assert( pFun );

	pFun( cConfigTemp );

}

/*************************************************
Description:	获取故障码配置
Input:
	PIn				具体配置内容
Output:	保留
Return:	无
Others:
*************************************************/
void get_general_dtc_config_data( const byte * pcSource )
{
	byte cConfigOffset = 0;

	cConfigOffset = pcSource[0];

	if( g_p_stGeneralReadDtcConfigGroup[cConfigOffset] == NULL )
		g_p_stGeneralReadDtcConfigGroup[cConfigOffset] = ( STRUCT_READ_DTC_CONFIG * )malloc( sizeof( STRUCT_READ_DTC_CONFIG ) );

	g_p_stGeneralReadDtcConfigGroup[cConfigOffset]->cDtcStartOffset		= pcSource[2];
	g_p_stGeneralReadDtcConfigGroup[cConfigOffset]->cDtcBytesInCmd		= pcSource[3];
	g_p_stGeneralReadDtcConfigGroup[cConfigOffset]->cStatusOffset		= pcSource[4];
	g_p_stGeneralReadDtcConfigGroup[cConfigOffset]->cDtcBytesInDisplay	= pcSource[5];
	g_p_stGeneralReadDtcConfigGroup[cConfigOffset]->cCurrentDtcMask		= pcSource[6];
	g_p_stGeneralReadDtcConfigGroup[cConfigOffset]->cHistoryDtcMask		= pcSource[7];
// 	g_p_stGeneralReadDtcConfigGroup[cConfigOffset]->bDTCMaskDisplay     = pcSource[8];
}

/*************************************************
Description:	选择故障码配置
Input:
	iConfigOffset		具体偏移
	cConfigType			故障码配置类型
Output:	保留
Return:	无
Others: 根据激活配置类型和配置偏移选择相应的配置
*************************************************/
void select_dtc_config( int iConfigOffset, const byte cConfigType )
{
	switch( cConfigType )
	{
	case GENERAL_DTC_CONFIG:
		g_p_stGeneralReadDtcConfig = g_p_stGeneralReadDtcConfigGroup[iConfigOffset];
		break;

	default:
		break;
	}
}

/*************************************************
Description:	释放存放读码配置的空间
Input:	无
Output:	保留
Return:	无
Others: 每添加一类配置就在该函数中添加相应的释放代码
并在quit_system_lib.c的free_xml_config_space
函数中调用该函数。
*************************************************/
void free_read_dtc_config_space( void )
{
	int i = 0;

	for( i = 0; i < sizeof( g_p_stGeneralReadDtcConfigGroup ) / sizeof( g_p_stGeneralReadDtcConfigGroup[0] ); i++ )
	{
		if( NULL != g_p_stGeneralReadDtcConfigGroup[i] )
		{
			free( g_p_stGeneralReadDtcConfigGroup[i] );
			g_p_stGeneralReadDtcConfigGroup[i] = NULL;
		}
	}

}