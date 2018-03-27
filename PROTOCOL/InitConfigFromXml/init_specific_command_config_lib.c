/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义获取具体命令配置处理函数
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
Description:	获取具体命令配置数据
Input:	PIn		具体配置内容
Output:	保留
Return:	int		命令条数
Others:
*************************************************/
int get_specific_command_config_data( void* pIn )
{
	free_specific_command_config_space();

	g_stInitXmlGobalVariable.m_iSpecificCmdGetFromXmlSum =
	    get_command_config_data( pIn, &( g_stInitXmlGobalVariable.m_p_stCmdList ) );

	return( g_stInitXmlGobalVariable.m_iSpecificCmdGetFromXmlSum );
}
/*************************************************
Description:	获取命令配置数据
Input:	PIn		具体配置内容
Output:	ppstCmd	命令结构体指针的指针
Return:	int		命令条数
Others:
*************************************************/
int get_command_config_data( void* pIn, STRUCT_CMD ** ppstCmd )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte cCmdTemp[256] = {0};
	byte cTemp[15] = {0};
	byte * pcTemp = NULL;
	int iCmdSum = 0;
	int i = 0;
	int iLen = 0;
	uint32 u32CmdLen = 0;
	int temp = 0;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	pcTemp = pstParam->pcData;

	memcpy( cTemp, pcTemp, 4 );
	cTemp[4] = '\0';

	pcTemp += 4;

	iLen = strtol( cTemp, NULL, 16 );

	memcpy( cTemp, pcTemp, iLen );
	cTemp[iLen] = '\0';

	iCmdSum = strtol( cTemp, NULL, 16 ); //获取命令总条数

	if( 0 == iCmdSum )
	{
		return iCmdSum;
	}

	//申请内存
	*ppstCmd = ( STRUCT_CMD * )malloc( sizeof( STRUCT_CMD ) * iCmdSum );

	pcTemp += iLen;

	for( i = 0; i < iCmdSum; i++ )
	{
		iLen = get_command_one_block_config_data( cCmdTemp, &temp, pcTemp );
		( *ppstCmd )[i].cBufferOffset = cCmdTemp[0]; //缓存偏移

		pcTemp += iLen;

		iLen = get_command_one_block_config_data( cCmdTemp, &temp, pcTemp );
		( *ppstCmd )[i].cReserved = cCmdTemp[0]; //保留字节

		pcTemp += iLen;

		memcpy( cTemp, pcTemp, 4 );
		cTemp[4] = '\0';

		iLen = 4;

		iLen += strtol( cTemp, NULL, 16 );

		get_config_data( &u32CmdLen, pcTemp, iLen );

		( *ppstCmd )[i].iCmdLen = ( int )u32CmdLen; //获取命令的长度

		pcTemp += iLen;

		iLen = get_command_one_block_config_data( cCmdTemp, &temp, pcTemp );

		( *ppstCmd )[i].pcCmd = ( byte* )malloc( sizeof( byte ) * u32CmdLen  + 1 );

		memcpy( ( *ppstCmd )[i].pcCmd, cCmdTemp, u32CmdLen ); //保存命令内容

		pcTemp += iLen;
	}

	return iCmdSum;
}



/*************************************************
Description:	释放存放具体命令配置的空间
Input:	无
Output:	保留
Return:	无
Others: 每添加一类配置就在该函数中添加相应的释放代码
并在quit_system_lib.c的free_xml_config_space
函数中调用该函数。
*************************************************/
void free_specific_command_config_space( void )
{
	int i = 0;

	if( NULL != g_stInitXmlGobalVariable.m_p_stCmdList )
	{
		for( i = 0; i < g_stInitXmlGobalVariable.m_iSpecificCmdGetFromXmlSum; i++ )
		{
			if( NULL != g_stInitXmlGobalVariable.m_p_stCmdList[i].pcCmd )
			{
				free( g_stInitXmlGobalVariable.m_p_stCmdList[i].pcCmd );
				g_stInitXmlGobalVariable.m_p_stCmdList[i].pcCmd = NULL;
			}
		}

		free( g_stInitXmlGobalVariable.m_p_stCmdList );

		g_stInitXmlGobalVariable.m_p_stCmdList = NULL;
	}

	g_stInitXmlGobalVariable.m_iSpecificCmdGetFromXmlSum = 0;
}