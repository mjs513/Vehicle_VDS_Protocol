/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	系统进入时扫描ECU版本号的函数处理文件，
History:
<author>    敦莹
<time>		2016年10月26日
<desc>      
************************************************************************/
#include <ctype.h>
#include "scan_ecu_version.h"

/*************************************************
Description:	读取ECU版本，根据ECU版本跳到响应的激活函数处
Input:
pIn		输入与读取ECU版本有关的命令数据和从UI输入的内容
Output:	pOut	输出数据地址
Return:	void
Others:	根据第一个命令数据执行不同的功能函数
*************************************************/
void Read_ECU_Version( STRUCT_CHAIN_DATA_INPUT* pstParam, void* pOut )
{
	int piActiveCmdIndex[2] = {1,0};
	int iReceiveResult = TIME_OUT;
	byte cBufferOffset = 0;
	int  SoftwareVersionOffset = 0;
	int  SoftwareVersionLength = 0;
	byte SoftwareVersionComper[255] = {0};//要比较的版本字符长度最多255个，若更长则需更改此数组长度
	int ipOutLen = 0;
	int i = 0, j = 0;
	byte* pcECUversion = NULL;
	bool bMatch = false;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );
	get_command_config_data( pstParam, &( g_stInitXmlGobalVariable.m_p_stCmdList ) ); //获取命令内容

	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[piActiveCmdIndex[1]].cBufferOffset;

	iReceiveResult = send_and_receive_cmd( piActiveCmdIndex );

	pstParam = pstParam->pNextNode; 
	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );
	SoftwareVersionOffset =  atoi( pstParam->pcData );

	pstParam = pstParam->pNextNode; 

	if( iReceiveResult == SUCCESS )
	{
		while (pstParam != NULL)
		{
			memcpy(SoftwareVersionComper,pstParam->pcData,pstParam->iLen);
			SoftwareVersionLength = pstParam->iLen;

			pstParam = pstParam->pNextNode;
			
			pcECUversion = g_stBufferGroup[cBufferOffset].cBuffer + SoftwareVersionOffset;

			if ( g_stBufferGroup[cBufferOffset].iValidLen - SoftwareVersionOffset < SoftwareVersionLength ) 
			{//若回复命令比版本号长度短，则匹配失败
				bMatch = false;
			}
			else
			{
				i = 0;
				j = SoftwareVersionOffset;
				while ( 1 )
				{
					if ( isdigit( *pcECUversion ) )//判断是否为数字，若为数字则进行比较，否则不比对
					{
						if ( SoftwareVersionComper[i++] == *pcECUversion )
						{
							if ( i >=  SoftwareVersionLength )
							{
								bMatch = true;
								break;
							}
						}
						else
						{
							bMatch = false;
							break;
						}
					}
					if( ++j >= g_stBufferGroup[cBufferOffset].iValidLen )//若回复命令已比对完，则退出循环
					{
						bMatch = false;
						break;
					}
					pcECUversion++;
				}
			}
			if ( bMatch )
			{
				ipOutLen = special_return_status( PROCESS_OK | HAVE_JUMP | HAVE_TIP_DATA, pstParam->pcData, "SoftwareVersion", 0, pOut);
				add_data_and_controlsID(ipOutLen,g_stBufferGroup[cBufferOffset].cBuffer + SoftwareVersionOffset, j,ORIGINAL,NULL,pOut);
				return; //若找到了匹配的版本，则中止while循环并返回
			}
			else//版本不匹配，则进行下一个的比较
			{
				pstParam = pstParam->pNextNode;
			}
		}
		//运行到此处说明未匹配到对应的版本，则跳到默认版本执行
		ipOutLen = special_return_status( PROCESS_OK | HAVE_JUMP | HAVE_TIP_DATA, "DEFAULT", "ECU_version_not_support", 0, pOut );
		add_data_and_controlsID( ipOutLen, g_stBufferGroup[cBufferOffset].cBuffer+SoftwareVersionOffset, g_stBufferGroup[cBufferOffset].iValidLen - SoftwareVersionOffset, ORIGINAL, NULL, pOut);
	}
	else if (  iReceiveResult == NEGATIVE )
	{
		//回复消极响应则跳到默认版本
		ipOutLen = special_return_status( PROCESS_OK | HAVE_JUMP | HAVE_TIP_DATA, "DEFAULT", "ECU_version_not_support", 0, pOut );
		add_data_and_controlsID( ipOutLen, g_stBufferGroup[cBufferOffset].cBuffer, 3, ORIGINAL, NULL, pOut );
		return;
	}
	else
	{
		//失败则跳到ACTIVE_FAIL处理
		special_return_status( PROCESS_FAIL | HAVE_JUMP | NO_TIP, "ACTIVE_FAIL", NULL, 0, pOut );
		return;
	}

}


void Check_ECU_Version(STRUCT_CHAIN_DATA_INPUT* pstParam, void* pOut)
{
	int piActiveCmdIndex[2] = {1,0};
	int iReceiveResult = TIME_OUT;
	int iReceiveValidLen = 0;//接收到的有效字节长度
	int ComperSize = 0;
	byte cBufferOffset = 0;
	int  SoftwareVersionOffset = 0;
	int  SoftwareVersionLength = 0;
	byte SoftwareVersionComper[50] = {0};
	int iAppendDataOffset = 0;
	byte idleLinks_Station = 0;


	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );
	get_command_config_data( pstParam, &( g_stInitXmlGobalVariable.m_p_stCmdList ) ); //获取命令内容

	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[piActiveCmdIndex[1]].cBufferOffset;

	iReceiveResult = send_and_receive_cmd( piActiveCmdIndex );

	iReceiveValidLen = g_stBufferGroup[cBufferOffset].iValidLen;

	pstParam = pstParam->pNextNode; 
	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );
	SoftwareVersionOffset =  atoi( pstParam->pcData );

	pstParam = pstParam->pNextNode; 
	switch( iReceiveResult )
	{
	case SUCCESS:
		{
			assert( pstParam->pcData );
			assert( pstParam->iLen != 0 );
			memcpy(SoftwareVersionComper,pstParam->pcData,pstParam->iLen);
			SoftwareVersionLength = pstParam->iLen;
			if (!memcmp(SoftwareVersionComper,g_stBufferGroup[cBufferOffset].cBuffer+SoftwareVersionOffset,SoftwareVersionLength))
			{
				iAppendDataOffset = special_return_status( PROCESS_OK | HAVE_JUMP | HAVE_TIP_DATA, pstParam->pcData, "SoftwareVersion", 0, pOut);
				add_data_and_controlsID(iAppendDataOffset,SoftwareVersionComper,SoftwareVersionLength,ORIGINAL,NULL,pOut);
				pstParam = pstParam->pNextNode; 
				assert( pstParam->pcData );
				assert( pstParam->iLen != 0 );
				idleLinks_Station = atoi( pstParam->pcData );
				set_idle_link( idleLinks_Station );
				if (1 == idleLinks_Station)
				{
					g_iActiveECUStatus = ACTIVE_ECU_SUCCESS;
				}
				special_return_status( PROCESS_OK | NO_JUMP | NO_TIP, NULL, NULL, 0, pOut );
				break;
			}
			else
			{
				iAppendDataOffset = special_return_status( PROCESS_FAIL | HAVE_JUMP | HAVE_TIP_DATA, "ACTIVE_FAIL", "ECU_version_not_support", 0, pOut );
				add_data_and_controlsID(iAppendDataOffset,g_stBufferGroup[cBufferOffset].cBuffer+SoftwareVersionOffset,SoftwareVersionLength,ORIGINAL,NULL,pOut);
			}

		}
		break;

	case NEGATIVE:
	case FRAME_TIME_OUT:
	case TIME_OUT:
	default:
		special_return_status( PROCESS_FAIL | HAVE_JUMP | NO_TIP, "ACTIVE_FAIL", NULL, 0, pOut );
		break;
	}
}

/*************************************************
Description:	扫描ECU版本处理函数
Input:
pIn		输入与扫描版本有关的命令数据和从UI输入的内容
Output:	pOut	输出数据地址
Return:	void
Others:	根据第一个命令数据执行不同的功能函数
*************************************************/
void process_ECU_version_function( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte FunctionSlect = 0;
	int ReadECUVersion = 0;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );
	FunctionSlect = atoi( pstParam->pcData );

	pstParam = pstParam->pNextNode;
	switch( FunctionSlect )
	{
	case 0:
		{
			Read_ECU_Version(pstParam,pOut);
		}
		break;

	case 1:
		{
			Check_ECU_Version(pstParam,pOut);
		}
		break;

	default:
		break;
	}
}
