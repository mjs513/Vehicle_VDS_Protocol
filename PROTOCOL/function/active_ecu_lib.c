/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义激活处理函数
History:
	<author>	<time>		<desc>
************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include "active_ecu_lib.h"
#include "..\public\public.h"
#include "..\command\command.h"
#include "..\public\protocol_config.h"
#include "..\interface\protocol_interface.h"
#include "..\InitConfigFromXml\init_config_from_xml_lib.h"
#include <assert.h>
#include "..\SpecialFunction\special_function.h"
#include "idle_link_lib.h"
#include "quit_system_lib.h"
#include <string.h>


int g_iActiveECUStatus;

STRUCT_ACTIVE_FUN stActiveECUFunGroup[] =
{
// 	{ACTIVE_ECU_BY_GENERAL_CAN, NULL},
	{ACTIVE_ECU_BY_KWP, process_active_ECU_by_low_high_voltage},
	{ACTIVE_ECU_BY_5_BAUD_ADDRESS, process_active_ECU_by_5_baud_address},
};

/*************************************************
Description:	获取处理激活函数
Input:
	cType		配置类型
Output:	保留
Return:	pf_general_function 函数指针
Others:
*************************************************/
pf_active_function get_active_ECU_fun( byte cType )
{
	int i = 0;

	for( i = 0; i < sizeof( stActiveECUFunGroup ) / sizeof( stActiveECUFunGroup[0] ); i++ )
		if( cType == stActiveECUFunGroup[i].cType )
			return stActiveECUFunGroup[i].pFun;

	return 0;
}
/*************************************************
Description:	激活处理函数
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
void process_active_ECU( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;

	int * piActivateCmdIndex = NULL;
	int iActiveCmdSum = 0;
	int i = 0;
	byte cConfigType = 0;
	int iConfigOffset = 0;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	iConfigOffset = atoi( pstParam->pcData ); //获得处理函数配置ID

	pstParam = pstParam->pNextNode;
	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	cConfigType = ( byte )atoi( pstParam->pcData ); //获得处理函数配置模板号

	select_process_fun_config( iConfigOffset, cConfigType );

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	iConfigOffset = atoi( pstParam->pcData ); //获得安全进入配置ID

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	cConfigType = ( byte )atoi( pstParam->pcData ); //获得安全进入配置模板号

	init_security_access_config( iConfigOffset, cConfigType );

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	iConfigOffset = atoi( pstParam->pcData );//获得空闲链接配置ID

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	cConfigType = ( byte )atoi( pstParam->pcData ); //获得空闲链接配置模板号

	select_idle_link_config( iConfigOffset, cConfigType );

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );
	//获取空闲链接命令
	g_stInitXmlGobalVariable.m_iIdleLinkCmdGetFromXmlSum =
	    get_command_config_data( pstParam, &( g_stInitXmlGobalVariable.m_p_stIdleLinkCmdList ) );

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	iConfigOffset = atoi( pstParam->pcData ); //获得激活配置ID

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	cConfigType = ( byte )atoi( pstParam->pcData ); //获得激活配置模板号

	select_active_config( iConfigOffset, cConfigType );

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	//获取激活命令条数、命令内容
	iActiveCmdSum = get_command_config_data( pstParam, &( g_stInitXmlGobalVariable.m_p_stCmdList ) );

	if( 0 != iActiveCmdSum )
	{
		piActivateCmdIndex = ( int * )malloc( sizeof( int ) * ( iActiveCmdSum + 1 ) );
		//存放激活命令索引内容
		piActivateCmdIndex[0] = iActiveCmdSum;

		for( i = 1; i < iActiveCmdSum + 1; i++ )
			piActivateCmdIndex[i] = i - 1;

		if( !send_active_ECU_command( piActivateCmdIndex, pstParam, pOut ) )
		{
			g_iActiveECUStatus = ACTIVE_ECU_FAIL;
			free_xml_config_space(); //系统激活失败释放内存。
			if( piActivateCmdIndex != NULL )
			{
				free( piActivateCmdIndex );
			}
			special_return_status( PROCESS_FAIL | HAVE_JUMP | NO_TIP, "ACTIVE_FAIL", NULL, 0, pOut );
			( ( byte * )pOut )[0] = 2;  //激活失败，统一提示超时；（0x7F会提示fail，所以在此统一修改为timeout）
			return;
		}
	}
	g_iActiveECUStatus = ACTIVE_ECU_SUCCESS;

	//第一次按照xml来配置空闲状态
	if( !set_idle_link( g_p_stIdleLinkConfig->cIdleLinkStatus ) )
	{
		//提示设置空闲链接失败
		special_return_status( PROCESS_OK | NO_JUMP | HAVE_TIP, NULL, "ID_STR_SET_IDLE_LINK_FAIL", 0, pOut );
	}
	else
	{
		special_return_status( PROCESS_OK | NO_JUMP | NO_TIP, NULL, NULL, 0, pOut );
	}

	if( piActivateCmdIndex != NULL )
	{
		free( piActivateCmdIndex );
	}
}

/*************************************************
Description:	进入命令处理函数
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
bool send_active_ECU_command( int * piActiveCmdIndex, void* pIn, void* pOut )
{
	pf_active_function pFun = NULL;
	bool bResultStatus = false;
	byte *pcOutTemp = ( byte * )pOut;
	int iReceiveResult = TIME_OUT;
	int iReceiveValidLen = 0;//接收到的有效字节长度
	byte piSecuriteIndex[5][2] = { 0 };//最多支持激活发送5组加密
	int piSendCmd[2] = { 1, 0 };
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	int iSum = 0, i = 0, j = 0;
	byte cRetransTime = 0;
	byte cRetransTimeTemp = 1;
	bool bSecuritFlag = false;
	int iRequestSeedCmdIndexBak = g_iRequestSeedCmdIndex[1];
	int iSendKeyCmdIndexBak = g_iSendKeyCmdIndex[1];

	pFun = get_active_ECU_fun( g_p_stProcessFunConfig->cActiveECUFunOffset );

	pstParam = pstParam->pNextNode;
	if( pstParam )	//如果下个节点不为空，则判断是否为安全算法开关配置
	{
		if( !memcmp( pstParam->pcData, "true", 4 ) )//配置为true则为有安全算法
		{
			bSecuritFlag = true;
			pstParam = pstParam->pNextNode;
			while (pstParam)
			{	//获取安全算法命令的位置和安全算法等级，共两个参数，第一个参数为获取种子命令的偏移，第二个参数为安全算法等级
				iSum = get_string_type_data_to_byte( piSecuriteIndex[i++], pstParam->pcData, pstParam->iLen );
				if ( iSum != 2 )
				{
					return false;
				}
				pstParam = pstParam->pNextNode;
			}
		}
	}

	if ( g_p_stProcessFunConfig->cActiveECUFunOffset != 0)
	{
		cRetransTime		= g_p_stISO14230Config->cRetransTime;
		cRetransTimeTemp = cRetransTime;
		//暂时修改重发次数为1
		g_p_stISO14230Config->cRetransTime = 1;
	}

	while( cRetransTimeTemp-- )
	{
		if ( pFun != NULL )
		{
			if( !pFun( pIn, pOut ) )//激活不成功则不执行下面的流程，重新开始循环
				continue;
		}
		if( bSecuritFlag )	
		{
			for ( i = 0, j = 0; i < piActiveCmdIndex[0]; i++ )
			{
				if ( i == piSecuriteIndex[j][0] )//若是加密的命令则计算安全算法
				{
					g_iRequestSeedCmdIndex[1] = i++;
					g_iSendKeyCmdIndex[1] = i;
					iReceiveResult = process_security_access_algorithm( piSecuriteIndex[j][1], pOut );
					if (!iReceiveResult)
					{
						return false;
					}
					j++;
				}
				else//不是加密命令则正常发送
				{
					piSendCmd[1] = piActiveCmdIndex[i + 1];
					iReceiveResult = send_and_receive_cmd( piSendCmd );
					if( ( iReceiveResult != SUCCESS ) && ( iReceiveResult != NEGATIVE ) )//若命令发送失败则结束for循环
					{
						i = piActiveCmdIndex[0];
						bResultStatus = false;
					}
				}
			}
		}
		else//不发送安全算法则直接发命令
		{
			iReceiveResult = send_and_receive_cmd( piActiveCmdIndex );
		}

		if( !iReceiveResult )//若命令发送成功则退出while循环
		{
			bResultStatus = true;
			break;
		}
	}

	if ( g_p_stProcessFunConfig->cActiveECUFunOffset != 0)
	{
		g_p_stISO14230Config->cRetransTime = cRetransTime;
	}
	
	g_iRequestSeedCmdIndex[1] = iRequestSeedCmdIndexBak;
	g_iSendKeyCmdIndex[1] = iSendKeyCmdIndexBak;
	
	return bResultStatus;
}

/*************************************************
Description:	kwp协议激活处理函数
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:
激活方式:0x01 高低电平（25ms），0x04
*************************************************/
bool process_active_ECU_by_low_high_voltage( void* pIn, void* pOut )
{
	bool bSendStatus = true;

	if( g_p_stGeneralActiveEcuConfig->cActiveMode != 0x00 ) /* 不发送C2命令 */
	{
		//发送激活设置命令
		bSendStatus = package_and_send_active_config();
	}
	return bSendStatus;
}

/*************************************************
Description:	以5波特率地址码激活处理函数
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
bool process_active_ECU_by_5_baud_address( void* pIn, void* pOut )
{
	bool bStatus	= false;

	byte cReceiveKeyByte[3] = {0};
	int i = 0;

	uint16	u16ECUResByteTimeout = g_p_stGeneralActiveEcuConfig->u16ECUResByteTimeout;

	STRUCT_CMD	stActiveCmdTemp = {0};

	//发送激活设置命令
	bStatus = package_and_send_active_config();

	if( !bStatus )
	{
		return bStatus;
	}

	if( g_p_stGeneralActiveEcuConfig->cActiveMode == 0x02 ) /* 普通的5bps激活，protocol取反 */
	{
		//接收关键字节，当激自检模式为[不自检]时VCI上传0x55 0xMM 0xNN，否则上传0xMM 0xNN
		bStatus = ( bool )receive_cmd( cReceiveKeyByte, g_p_stGeneralActiveEcuConfig->cReceiveKeyByte[0]
		                                      , u16ECUResByteTimeout * 3 );

		if( !bStatus )
		{
			return bStatus;
		}

		//判断接收到的关键字节跟设定的是否相同
		for( ; i < g_p_stGeneralActiveEcuConfig->cReceiveKeyByte[0]; i++ )
		{
			if( cReceiveKeyByte[i] != g_p_stGeneralActiveEcuConfig->cReceiveKeyByte[i + 1] )
			{
				return false;
			}
			//也可以进行其他操作
		}

		stActiveCmdTemp.cBufferOffset = 0;
		stActiveCmdTemp.cReserved = 0;
		stActiveCmdTemp.iCmdLen = 1;
		stActiveCmdTemp.pcCmd = ( byte* )malloc( sizeof( byte ) );
		stActiveCmdTemp.pcCmd[0] = ~cReceiveKeyByte[1];//修改激活命令，该命令仅一个字节

		//发送取反字节,对于03取反的可把取反的字节当做激活命令使用放到command.xml中
		if( !package_and_send_frame( FRAME_HEAD_NORMAL_FRAME, ( STRUCT_CMD* )&stActiveCmdTemp, 0xaa ) )
		{
			free( stActiveCmdTemp.pcCmd );
			return false;
		}
	}
	else if( g_p_stGeneralActiveEcuConfig->cActiveMode == 0x05 ||  \
	         g_p_stGeneralActiveEcuConfig->cActiveMode == 0x06 ) /* 0x05 VDI取反 激活系统, 0x06 VDI不取反 */
	{
		bStatus = ( bool )receive_cmd( cReceiveKeyByte, g_p_stGeneralActiveEcuConfig->cReceiveKeyByte[0]
		                                      , u16ECUResByteTimeout * 3 );
	}

	free( stActiveCmdTemp.pcCmd );
	return true;
}

