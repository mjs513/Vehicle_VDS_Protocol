/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义空闲链接数据处理函数
History:
	<author>	<time>		<desc>

***********************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include "idle_link_lib.h"
#include "..\public\public.h"
#include "..\public\protocol_config.h"
#include <assert.h>
#include "..\InitConfigFromXml\init_config_from_xml_lib.h"
#include <string.h>


/*************************************************
Description:	设置空闲链接
Input:	none
Output:	none
Return:	bool	设置的状态
Others:	如果命令总数为0则返回true
*************************************************/
bool set_idle_link( byte cIdleLinkStatus )
{
	bool bSetStatus = false;
	STRUCT_CMD stSendCmd = {0};
	STRUCT_CMD stCmdTemp = {0};
	int iIdleLinkCmdLen = 0;
	int i = 0;
	int iSaveOffset = 0;
	UNN_2WORD_4BYTE unnTemp;
	unnTemp.u32Bit = 0;

	//如果命令总数为0直接返回
	if( 0 == g_stInitXmlGobalVariable.m_iIdleLinkCmdGetFromXmlSum )
		return true;

	for( i = 0; i < g_stInitXmlGobalVariable.m_iIdleLinkCmdGetFromXmlSum; i++ )
	{
		iIdleLinkCmdLen += g_stInitXmlGobalVariable.m_p_stIdleLinkCmdList[i].iCmdLen;//获取空闲命令长度
	}

	stSendCmd.iCmdLen	= 1 + 2 + 2 + 1 + 1 + 1 + iIdleLinkCmdLen +
	                      g_stInitXmlGobalVariable.m_iIdleLinkCmdGetFromXmlSum/*有几条命令就对应有几个表示命令长度的字节*/;

	stSendCmd.pcCmd		= ( byte* )malloc( sizeof( byte ) * stSendCmd.iCmdLen );

	stSendCmd.pcCmd[0]	= cIdleLinkStatus;
	unnTemp.u32Bit		= g_p_stIdleLinkConfig->u16TimeBetweenGroups;
	stSendCmd.pcCmd[1]	= unnTemp.u8Bit[1];
	stSendCmd.pcCmd[2]	= unnTemp.u8Bit[0];
	unnTemp.u32Bit		= g_p_stIdleLinkConfig->u16TimeBetweenFrames;
	stSendCmd.pcCmd[3]	= unnTemp.u8Bit[1];
	stSendCmd.pcCmd[4]	= unnTemp.u8Bit[0];

	stSendCmd.pcCmd[5]	= g_p_stIdleLinkConfig->cIdleLinkMode;//
	stSendCmd.pcCmd[6]	= g_p_stIdleLinkConfig->cResponseFlag;
	stSendCmd.pcCmd[7]	= g_stInitXmlGobalVariable.m_iIdleLinkCmdGetFromXmlSum;

	iSaveOffset = 8;

	for( i = 0; i < g_stInitXmlGobalVariable.m_iIdleLinkCmdGetFromXmlSum; i++ )
	{
		stCmdTemp.iCmdLen = g_stInitXmlGobalVariable.m_p_stIdleLinkCmdList[i].iCmdLen;
		stCmdTemp.pcCmd = ( byte* )malloc( sizeof( byte ) * stCmdTemp.iCmdLen ); //为的是不直接修改原来的命令

		memcpy( stCmdTemp.pcCmd, g_stInitXmlGobalVariable.m_p_stIdleLinkCmdList[i].pcCmd, stCmdTemp.iCmdLen );

		if( 0 != g_p_stIdleLinkConfig->cCalculateChecksumFlag ) //判断是否需要计算校验和
		{
			stCmdTemp.pcCmd[stCmdTemp.iCmdLen - 1] = calculate_Checksum( stCmdTemp.pcCmd, stCmdTemp.iCmdLen - 1 );
		}

		stSendCmd.pcCmd[iSaveOffset] = stCmdTemp.iCmdLen;

		memcpy( stSendCmd.pcCmd + iSaveOffset + 1, stCmdTemp.pcCmd, stCmdTemp.iCmdLen );

		iSaveOffset  += stCmdTemp.iCmdLen + 1;

		free( stCmdTemp.pcCmd );
	}

	bSetStatus = package_and_send_frame( FRAME_HEAD_SET_IDLE_LINK, ( STRUCT_CMD * )&stSendCmd,
	                                     g_p_stIdleLinkConfig->cTimeBetweenBytes );

	free( stSendCmd.pcCmd );

	return bSetStatus;
}

/*************************************************
Description:	释放存放空闲链接命令的空间
Input:	无
Output:	保留
Return:	无
Others: 需在quit_system_lib.c的free_xml_config_space
函数中调用该函数。
*************************************************/
void free_idle_link_command_config_space( void ) //释放空闲链接空间
{
	int i = 0;

	if( NULL != g_stInitXmlGobalVariable.m_p_stIdleLinkCmdList )
	{
		for( i = 0; i < g_stInitXmlGobalVariable.m_iIdleLinkCmdGetFromXmlSum; i++ )
		{
			if( NULL != g_stInitXmlGobalVariable.m_p_stIdleLinkCmdList[i].pcCmd )
			{
				free( g_stInitXmlGobalVariable.m_p_stIdleLinkCmdList[i].pcCmd );
				g_stInitXmlGobalVariable.m_p_stIdleLinkCmdList[i].pcCmd = NULL;
			}
		}

		free( g_stInitXmlGobalVariable.m_p_stIdleLinkCmdList );
		g_stInitXmlGobalVariable.m_p_stIdleLinkCmdList = NULL;
	}

	g_stInitXmlGobalVariable.m_iIdleLinkCmdGetFromXmlSum = 0;
}

