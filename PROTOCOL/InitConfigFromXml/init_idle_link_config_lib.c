/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义从xml获取空闲链接配置处理函数
History:
	<author>	<time>		<desc>

***********************************************************************/
#include "..\interface\protocol_define.h"
#include "..\public\protocol_config.h"
#include "init_config_from_xml_lib.h"
#include <assert.h>
#include <stdlib.h>
#include <memory.h>

STRUCT_INIT_CONFIG stInitIdleLinkConfigGroup[] =
{
	{GENERAL_IDLE_LINK_CONFIG, get_general_idle_link_config_data},
};

/*************************************************
Description:	获取故障码配置函数
Input:
	cConfigType		配置类型
Output:	保留
Return:	pf_parammeter_is_point_to_uint32 函数指针
Others:
*************************************************/
pf_parammeter_is_point_to_uint32 get_init_idle_link_config_fun( byte cConfigType )
{
	int i = 0;

	for( i = 0; i < sizeof( stInitIdleLinkConfigGroup ) / sizeof( stInitIdleLinkConfigGroup[0] ); i++ )
		if( cConfigType == stInitIdleLinkConfigGroup[i].cConfigType )
			return stInitIdleLinkConfigGroup[i].pFun;

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
void get_idle_link_config_data( void* pIn )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	uint32 u32ConfigTemp[50] = {0};

	byte cConfigType = 0;
	pf_parammeter_is_point_to_uint32 pFun = NULL;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );
	
	get_config_data( u32ConfigTemp, pstParam->pcData, NO_LENGTH_LIMIT );

	cConfigType	= ( byte )u32ConfigTemp[1];

	pFun = get_init_idle_link_config_fun( cConfigType );

	assert( pFun );

	pFun( u32ConfigTemp );

}

/*************************************************
Description:	获取故障码配置
Input:
	PIn				具体配置内容
Output:	保留
Return:	无
Others:
*************************************************/
void get_general_idle_link_config_data( const uint32 * pu32Source )
{
	byte cConfigOffset = 0;
	UNN_2WORD_4BYTE unnTemp;
	unnTemp.u32Bit = 0;

	cConfigOffset = ( byte )pu32Source[0];

	if( g_p_stIdleLinkConfigGroup[cConfigOffset] == NULL )
		g_p_stIdleLinkConfigGroup[cConfigOffset] = ( STRUCT_IDLE_LINK_CONFIG * )malloc( sizeof( STRUCT_IDLE_LINK_CONFIG ) );

	g_p_stIdleLinkConfigGroup[cConfigOffset]->cTimeBetweenBytes		= ( byte )pu32Source[2];
	g_p_stIdleLinkConfigGroup[cConfigOffset]->cIdleLinkStatus		= ( byte )pu32Source[3];
	g_p_stIdleLinkConfigGroup[cConfigOffset]->u16TimeBetweenGroups	= ( uint16 )pu32Source[4];
	g_p_stIdleLinkConfigGroup[cConfigOffset]->u16TimeBetweenFrames	= ( uint16 )pu32Source[5];
	g_p_stIdleLinkConfigGroup[cConfigOffset]->cResponseFlag			= ( byte )pu32Source[6];
	g_p_stIdleLinkConfigGroup[cConfigOffset]->cCalculateChecksumFlag = ( byte )pu32Source[7];
	g_p_stIdleLinkConfigGroup[cConfigOffset]->cIdleLinkMode	= ( byte )pu32Source[8];
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
void select_idle_link_config( int iConfigOffset, const byte cConfigType )
{
	switch( cConfigType )
	{
	case GENERAL_IDLE_LINK_CONFIG:
		g_p_stIdleLinkConfig = g_p_stIdleLinkConfigGroup[iConfigOffset];
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
void free_idle_link_config_space( void )
{
	int i = 0;

	for( i = 0; i < sizeof( g_p_stIdleLinkConfigGroup ) / sizeof( g_p_stIdleLinkConfigGroup[0] ); i++ )
	{
		if( NULL != g_p_stIdleLinkConfigGroup[i] )
		{
			free( g_p_stIdleLinkConfigGroup[i] );
			g_p_stIdleLinkConfigGroup[i] = NULL;
		}
	}

}