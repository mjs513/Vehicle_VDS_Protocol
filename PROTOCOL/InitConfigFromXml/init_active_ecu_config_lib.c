/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义获取激活配置处理函数
History:
	<author>	<time>		<desc>

***********************************************************************/
#include "..\interface\protocol_define.h"
#include "..\public\protocol_config.h"
#include "init_config_from_xml_lib.h"
#include <assert.h>
#include <stdlib.h>


STRUCT_INIT_CONFIG stInitActiveECUConfigGroup[] =
{
	{GENERAL_ACTIVE_ECU_CONFIG, get_general_active_ecu_config_data},
};


/*************************************************
Description:	获取初始化协议配置函数
Input:
	cConfigType		配置类型
Output:	保留
Return:	pf_parammeter_is_point_to_uint32 函数指针
Others:
*************************************************/
pf_parammeter_is_point_to_uint32 get_init_active_ECU_config_fun( byte cConfigType )
{
	int i = 0;

	for( i = 0; i < sizeof( stInitActiveECUConfigGroup ) / sizeof( stInitActiveECUConfigGroup[0] ); i++ )
		if( cConfigType == stInitActiveECUConfigGroup[i].cConfigType )
			return stInitActiveECUConfigGroup[i].pFun;

	return 0;//用于查错用
}

/*************************************************
Description:	获取激活配置
Input:
	PIn				具体配置内容
Output:	保留
Return:	无
Others: 根据不同的配置模版进相应的分支
*************************************************/
void get_active_config_data( void* pIn )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	uint32 u32ActiveConfigTemp[50] = {0};

	byte cActiveConfigType = 0;

	pf_parammeter_is_point_to_uint32 pFun = NULL;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	get_config_data( u32ActiveConfigTemp, pstParam->pcData, NO_LENGTH_LIMIT );

	cActiveConfigType	= ( byte )u32ActiveConfigTemp[1];

	pFun = get_init_active_ECU_config_fun( cActiveConfigType );

	assert( pFun );

	pFun( u32ActiveConfigTemp );

}
/*************************************************
Description:	获取通用激活配置数据
Input:
	pu32Source		具体配置内容
Output:	保留
Return:	无
Others: 函数会开辟空间存放配置数据

byte cTimeBetweenBytes;		//字节与字节间时间
byte cActiveMode;			//激活方式
byte cDetectBandRateFlag;	//自检标志 0x00:不需要 0x01:需要
uint16	u16ECUResByteTimeout;//等待ECU回复的字节超时时间
byte cBandRate[3];			//波特率
byte cActiveAddress[5];		//激活地址第一个字节表示地址长度
byte cReceiveKeyByte[10];	//接收ECU回复的关键字第一个字节表示地址长度
*************************************************/
void get_general_active_ecu_config_data( const uint32 * pu32Source )
{
	byte cConfigOffset = 0;
	int iSaveOffset = 0;
	int i = 0;
	UNN_2WORD_4BYTE unnTemp;
	unnTemp.u32Bit = 0;

	cConfigOffset = ( byte )pu32Source[0];

	if( g_p_stGeneralActiveEcuConfigGroup[cConfigOffset] == NULL )
		g_p_stGeneralActiveEcuConfigGroup[cConfigOffset] = ( STRUCT_ACTIVE_ECU_CONFIG * )malloc( sizeof( STRUCT_ACTIVE_ECU_CONFIG ) );

	g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->cTimeBetweenBytes		= ( byte )pu32Source[2];
	g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->cActiveMode			= ( byte )pu32Source[3];
	g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->cDetectBandRateFlag	= ( byte )pu32Source[4];
	g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->u16ECUResByteTimeout	= ( uint16 )pu32Source[5];
	unnTemp.u32Bit = pu32Source[6];
	g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->cBandRate[0]			= unnTemp.u8Bit[2];
	g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->cBandRate[1]			= unnTemp.u8Bit[1];
	g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->cBandRate[2]			= unnTemp.u8Bit[0];

	g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->cActiveAddress[0]		= ( byte )pu32Source[7];

	for( i = 0; i < ( byte )pu32Source[7]; i++ ) //保存激活地址
	{
		g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->cActiveAddress[1 + i] = ( byte )pu32Source[8 + i];
	}

	iSaveOffset = 6 + 1 + ( byte )pu32Source[7] + 1;

	g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->cReceiveKeyByte[0]	= ( byte )pu32Source[iSaveOffset];

	for( i = 0; i < g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->cReceiveKeyByte[0]; i++ ) //保存接收ECU回复的关键字
	{
		g_p_stGeneralActiveEcuConfigGroup[cConfigOffset]->cReceiveKeyByte[1 + i] = ( byte )pu32Source[iSaveOffset + 1 + i];
	}
}

/*************************************************
Description:	选择激活配置
Input:
	iConfigOffset		具体偏移
	cConfigType			配置类型
Output:	保留
Return:	无
Others: 根据激活配置类型和配置偏移选择相应的配置
*************************************************/
void select_active_config( int iConfigOffset, const byte cConfigType )
{
	switch( cConfigType )
	{
	case GENERAL_ACTIVE_ECU_CONFIG:
		g_p_stGeneralActiveEcuConfig = g_p_stGeneralActiveEcuConfigGroup[iConfigOffset];
		break;

	default:
		break;
	}
}

/*************************************************
Description:	释放存放激活配置的空间
Input:	无
Output:	保留
Return:	无
Others: 每添加一类配置就在该函数中添加相应的释放代码
并在quit_system_lib.c的free_xml_config_space
函数中调用该函数。
*************************************************/
void free_active_ecu_config_space( void )
{
	int i = 0;

	for( i = 0; i < sizeof( g_p_stGeneralActiveEcuConfigGroup ) / sizeof( g_p_stGeneralActiveEcuConfigGroup[0] ); i++ )
	{
		if( NULL != g_p_stGeneralActiveEcuConfigGroup[i] )
		{
			free( g_p_stGeneralActiveEcuConfigGroup[i] );
			g_p_stGeneralActiveEcuConfigGroup[i] = NULL;
		}
	}

}
