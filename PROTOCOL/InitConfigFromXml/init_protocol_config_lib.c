/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义获取协议配置处理函数
History:
	<author>	<time>		<desc>

***********************************************************************/

#include "init_config_from_xml_lib.h"
#include "..\public\protocol_config.h"
#include "..\interface\protocol_define.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>



STRUCT_INIT_CONFIG stInitProtocolConfigGroup[] =
{
	{ISO15765_CONFIG, get_ISO15765_config_data },
	{ISO14230_CONFIG, get_ISO14230_config_data },
	{SEA1939_CONFIG,  get_SAE1939_config_data }, //目前SEA1939 没有用到协议配置，暂和ISO15765一样
	
};

/*************************************************
Description:	获取初始化协议配置函数
Input:
	cConfigType		配置类型
Output:	保留
Return:	pf_parammeter_is_point_to_uint32 函数指针
Others:
*************************************************/
pf_parammeter_is_point_to_uint32 get_init_protocol_config_fun( byte cConfigType )
{
	int i = 0;

	for( i = 0; i < sizeof( stInitProtocolConfigGroup ) / sizeof( stInitProtocolConfigGroup[0] ); i++ )
		if( cConfigType == stInitProtocolConfigGroup[i].cConfigType )
			return stInitProtocolConfigGroup[i].pFun;

	return 0;//用于查错用
}
/*************************************************
Description:	获取协议配置
Input:
	PIn				具体配置内容
Output:	保留
Return:	无
Others: 根据不同的配置模版进相应的分支
*************************************************/

void get_protocol_config_data( void* pIn )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	uint32 u32ConfigTemp[50] = {0};

	byte cProtocolConfigType = 0;
	pf_parammeter_is_point_to_uint32 pFun = NULL;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	get_config_data( u32ConfigTemp, pstParam->pcData, NO_LENGTH_LIMIT );

	cProtocolConfigType	= ( byte )u32ConfigTemp[1];

	pFun = get_init_protocol_config_fun( cProtocolConfigType );

	assert( pFun );

	pFun( u32ConfigTemp );

}

/*************************************************
Description:	获取ISO15765配置数据
Input:
	pu32Source		具体配置内容
Output:	保留
Return:	无
Others: 函数会开辟空间存放配置数据
*************************************************/
void get_ISO15765_config_data( const uint32 * pu32Source )
{
	byte cConfigOffset = 0;
	UNN_2WORD_4BYTE unnTemp;
	unnTemp.u32Bit = 0;

	cConfigOffset = ( byte )pu32Source[0];

	if( g_p_stISO15765ConfigGroup[cConfigOffset] == NULL )
		g_p_stISO15765ConfigGroup[cConfigOffset] = ( STRUCT_ISO15765_CONFIG * )malloc( sizeof( STRUCT_ISO15765_CONFIG ) );

	g_p_stISO15765ConfigGroup[cConfigOffset]->u16Code7F78Timeout	= ( uint16 )pu32Source[2];
	g_p_stISO15765ConfigGroup[cConfigOffset]->u16ECUResTimeout		= ( uint16 )pu32Source[3];
	g_p_stISO15765ConfigGroup[cConfigOffset]->u16TimeBetweenFrames	= ( uint16 )pu32Source[4];
	g_p_stISO15765ConfigGroup[cConfigOffset]->cRetransTime			= ( byte )pu32Source[5];
	g_p_stISO15765ConfigGroup[cConfigOffset]->cReserved				= ( byte )pu32Source[6];
	g_p_stISO15765ConfigGroup[cConfigOffset]->cMultiframestime	    = ( byte )pu32Source[7];

}

/*************************************************
Description:	获取ISO14230配置数据
Input:
	pu32Source		具体配置内容
Output:	保留
Return:	无
Others: 函数会开辟空间存放配置数据
*************************************************/
void get_ISO14230_config_data( const uint32 * pu32Source )
{
	byte cConfigOffset = 0;
	UNN_2WORD_4BYTE unnTemp;
	unnTemp.u32Bit = 0;

	cConfigOffset = ( byte )pu32Source[0];

	if( g_p_stISO14230ConfigGroup[cConfigOffset] == NULL )
		g_p_stISO14230ConfigGroup[cConfigOffset] = ( STRUCT_ISO14230_CONFIG * )malloc( sizeof( STRUCT_ISO14230_CONFIG ) );

	g_p_stISO14230ConfigGroup[cConfigOffset]->u16Code7F78Timeout	= ( uint16 )pu32Source[2];
	g_p_stISO14230ConfigGroup[cConfigOffset]->u16ECUResTimeout		= ( uint16 )pu32Source[3];
	g_p_stISO14230ConfigGroup[cConfigOffset]->u16TimeBetweenFrames	= ( uint16 )pu32Source[4];
	g_p_stISO14230ConfigGroup[cConfigOffset]->cRetransTime			= ( byte )pu32Source[5];
	g_p_stISO14230ConfigGroup[cConfigOffset]->cTimeBetweenBytes		= ( byte )pu32Source[6];
}

/*************************************************
Description:	获取SAE1939配置数据
Input:
	pu32Source		具体配置内容
Output:	保留
Return:	无
Others: 函数会开辟空间存放配置数据
*************************************************/
void get_SAE1939_config_data( const uint32 * pu32Source )
{
	byte cConfigOffset = 0;
	UNN_2WORD_4BYTE unnTemp;
	unnTemp.u32Bit = 0;

	cConfigOffset = ( byte )pu32Source[0];


	if( g_p_stSAE1939ConfigGroup[cConfigOffset] == NULL )
		g_p_stSAE1939ConfigGroup[cConfigOffset] = ( STRUCT_SAE1939_CONFIG * )malloc( sizeof( STRUCT_SAE1939_CONFIG ) );

	g_p_stSAE1939ConfigGroup[cConfigOffset]->u16TimeBetweenFrames	= ( uint16 )pu32Source[2];
	g_p_stSAE1939ConfigGroup[cConfigOffset]->u16ECUResTimeout    	= ( uint16 )pu32Source[3];
	g_p_stSAE1939ConfigGroup[cConfigOffset]->u16VDI5ByteTimeout	    = ( uint16 )pu32Source[4];
	g_p_stSAE1939ConfigGroup[cConfigOffset]->cRetransTime 			= ( byte )pu32Source[5];

	/*g_p_stSAE1939ConfigGroup[cConfigOffset]->u16Code7F78Timeout	= ( uint16 )pu32Source[2];
	g_p_stSAE1939ConfigGroup[cConfigOffset]->u16ECUResTimeout		= ( uint16 )pu32Source[3];
	g_p_stSAE1939ConfigGroup[cConfigOffset]->u16TimeBetweenFrames	= ( uint16 )pu32Source[4];
	g_p_stSAE1939ConfigGroup[cConfigOffset]->cRetransTime			= ( byte )pu32Source[5];
	g_p_stSAE1939ConfigGroup[cConfigOffset]->cReserved				= ( byte )pu32Source[6];
	g_p_stSAE1939ConfigGroup[cConfigOffset]->cMultiframestime	    = ( byte )pu32Source[7];*/

}

/*************************************************
Description:	选择协议配置
Input:
	iProtocolConfigOffset	具体偏移
	cProtocolConfigType		协议配置类型
Output:	保留
Return:	无
Others: 根据协议配置类型和配置偏移选择相应的配置
*************************************************/
void select_protocol_config( int iProtocolConfigOffset, const byte cProtocolConfigType )
{
	switch( cProtocolConfigType )
	{
	case ISO15765_CONFIG:
		g_p_stISO15765Config = g_p_stISO15765ConfigGroup[iProtocolConfigOffset];
		break;

	case ISO14230_CONFIG:
		g_p_stISO14230Config = g_p_stISO14230ConfigGroup[iProtocolConfigOffset];
		break;
	case SEA1939_CONFIG:
		g_p_stSAE1939Config = g_p_stSAE1939ConfigGroup[iProtocolConfigOffset];
		break;
	default:
		break;
	}
}

/*************************************************
Description:	释放存放协议配置的空间
Input:	无
Output:	保留
Return:	无
Others: 每添加一类配置就在该函数中添加相应的释放代码
并在quit_system_lib.c的free_xml_config_space
函数中调用该函数。
*************************************************/
void free_protocol_config_space( void )
{
	int i = 0;

	for( i = 0; i < sizeof( g_p_stISO15765ConfigGroup ) / sizeof( g_p_stISO15765ConfigGroup[0] ); i++ )
	{
		if( NULL != g_p_stISO15765ConfigGroup[i] )
		{
			free( g_p_stISO15765ConfigGroup[i] );
			g_p_stISO15765ConfigGroup[i] = NULL;
		}

	}

	for( i = 0; i < sizeof( g_p_stISO14230ConfigGroup ) / sizeof( g_p_stISO14230ConfigGroup[0] ); i++ )
	{
		if( NULL != g_p_stISO14230ConfigGroup[i] )
		{
			free( g_p_stISO14230ConfigGroup[i] );
			g_p_stISO14230ConfigGroup[i] = NULL;
		}

	}


	for( i = 0; i < sizeof( g_p_stSAE1939ConfigGroup ) / sizeof( g_p_stSAE1939ConfigGroup[0] ); i++ )
	{
		if( NULL != g_p_stSAE1939ConfigGroup[i] )
		{
			free( g_p_stSAE1939ConfigGroup[i] );
			g_p_stSAE1939ConfigGroup[i] = NULL;
		}

	}
}