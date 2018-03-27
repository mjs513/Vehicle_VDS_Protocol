/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义获取处理函数配置处理函数
History:
	<author>	<time>		<desc>

***********************************************************************/
#include "..\interface\protocol_define.h"
#include "..\public\protocol_config.h"
#include "init_config_from_xml_lib.h"
#include <assert.h>
#include <stdlib.h>
#include "..\command\command.h"
#include "..\formula\formula_comply.h"

STRUCT_INIT_CONFIG stInitSecurityAccessConfigGroup[] =
{
	{GENERAL_SECURITY_ACCESS_CONFIG, get_general_security_access_config_data},
};


/*************************************************
Description:	获取初始化安全进入配置函数
Input:
	cConfigType		配置类型
Output:	保留
Return:	pf_parammeter_is_point_to_uint32 函数指针
Others:
*************************************************/
pf_parammeter_is_point_to_uint32 get_init_security_access_config_fun( byte cConfigType )
{
	int i = 0;

	for( i = 0; i < sizeof( stInitSecurityAccessConfigGroup ) / sizeof( stInitSecurityAccessConfigGroup[0] ); i++ )
		if( cConfigType == stInitSecurityAccessConfigGroup[i].cConfigType )
			return stInitSecurityAccessConfigGroup[i].pFun;

	return 0;//用于查错用
}

/*************************************************
Description:	获取安全进入配置
Input:
	PIn				具体配置内容
Output:	保留
Return:	无
Others: 根据不同的配置模版进相应的分支
*************************************************/
void get_security_access_config( void* pIn )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	uint32 u32ConfigTemp[50] = {0};

	byte cConfigType = 0;

	pf_parammeter_is_point_to_uint32 pFun = NULL;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	get_config_data( u32ConfigTemp, pstParam->pcData, NO_LENGTH_LIMIT );

	cConfigType	= ( byte )u32ConfigTemp[1];

	pFun = get_init_security_access_config_fun( cConfigType );

	assert( pFun );

	pFun( u32ConfigTemp );

}
/*************************************************
Description:	获取通用安全进入配置数据
Input:
	pu32Source		具体配置内容
Output:	保留
Return:	无
Others: 函数会开辟空间存放配置数据
*************************************************/
void get_general_security_access_config_data( const uint32 * pu32Source )
{
	byte cConfigOffset = 0;

	cConfigOffset = ( byte )pu32Source[0];

	if( g_p_stSecurityAccessConfigGroup[cConfigOffset] == NULL )
		g_p_stSecurityAccessConfigGroup[cConfigOffset] = ( STRUCT_SECURITY_ACCSEE_CONFIG * )malloc( sizeof( STRUCT_SECURITY_ACCSEE_CONFIG ) );

	g_p_stSecurityAccessConfigGroup[cConfigOffset]->iDefaultSessionCmdID	= ( int )pu32Source[2];
	g_p_stSecurityAccessConfigGroup[cConfigOffset]->iExtendSessionCmdID		= ( int )pu32Source[3];
	g_p_stSecurityAccessConfigGroup[cConfigOffset]->iRequestSeedCmdID		= ( int )pu32Source[4];
	g_p_stSecurityAccessConfigGroup[cConfigOffset]->iSendKeyCmdID			= ( int )pu32Source[5];

}

/*************************************************
Description:	选择安全进入配置
Input:
	iConfigOffset		具体偏移
	cConfigType			配置类型
Output:	保留
Return:	无
Others: 根据配置类型和配置偏移选择相应的配置并进行
		初始化
*************************************************/
void init_security_access_config( int iConfigOffset, const byte cConfigType )
{
	switch( cConfigType )
	{
	case GENERAL_SECURITY_ACCESS_CONFIG:
	{
		g_p_stSecurityAccessConfig = g_p_stSecurityAccessConfigGroup[iConfigOffset];

		g_iDefaultSessionCmdIndex[1]	= g_p_stSecurityAccessConfig->iDefaultSessionCmdID;
		g_iExtendSessionCmdIndex[1]		= g_p_stSecurityAccessConfig->iExtendSessionCmdID;
		g_iRequestSeedCmdIndex[1]		= g_p_stSecurityAccessConfig->iRequestSeedCmdID;
		g_iSendKeyCmdIndex[1]			= g_p_stSecurityAccessConfig->iSendKeyCmdID;
	}
	break;

	default:
		break;
	}
}

/*************************************************
Description:	释放存放安全进入配置的空间
Input:	无
Output:	保留
Return:	无
Others: 每添加一类配置就在该函数中添加相应的释放代码
并在quit_system_lib.c的free_xml_config_space
函数中调用该函数。
*************************************************/
void free_security_access_config_space( void )
{
	int i = 0;

	for( i = 0; i < sizeof( g_p_stSecurityAccessConfigGroup ) / sizeof( g_p_stSecurityAccessConfigGroup[0] ); i++ )
	{
		if( NULL != g_p_stSecurityAccessConfigGroup[i] )
		{
			free( g_p_stSecurityAccessConfigGroup[i] );
			g_p_stSecurityAccessConfigGroup[i] = NULL;
		}
	}

}
