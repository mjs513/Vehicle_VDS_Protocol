/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义根据ISO15765协议处理收发数据的相关函数
History:
	<author>	<time>		<desc>

************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "iso_15765.h"
#include "../interface/protocol_interface.h"
#include "../public/protocol_config.h"
#include "../command/command.h"
#include "../InitConfigFromXml/init_config_from_xml_lib.h"
#include "../function/idle_link_lib.h"
#include "../public/public.h" 

/*************************************************
Description:	根据ISO15765协议收发函数
Input:	piCmdIndex	命令索引地址

Output:	none
Return:	int	收发处理时的状态
Others:	根据命令索引可以发送和接收多条命令，
		把ECU积极相应的数据放到命令相应的缓存中
注意：这里存放ECU数据是从SID的回复开始的，如：
7E0  03 22 15 08 00 00 00 00
7E8  04 62 15 08 79 00 00 00
是从62开始保存，若为消极相应则从7F开始保存。
*************************************************/
int send_and_receive_cmd_by_iso_15765( const int* piCmdIndex )
{
	int iStatus = TIME_OUT;
	int i = 0;
	int iCmdSum = piCmdIndex[0];
	byte cReceiveBuffer[20] = {0};
	STRUCT_CMD stSendCmd = {0};

	for( i = 0; i < iCmdSum; i++ )
	{
		iStatus = send_and_receive_single_cmd_by_iso_15765( ( STRUCT_CMD * )&stSendCmd, piCmdIndex[1 + i], cReceiveBuffer );

		if( NULL != stSendCmd.pcCmd )
			free( stSendCmd.pcCmd );

		if( ( iStatus != SUCCESS ) && ( iStatus != NEGATIVE ) ) //如果状态既不是SUCCESS又不是NEGATIVE则认为出错
		{
			break;
		}

		time_delay_ms( g_p_stISO15765Config->u16TimeBetweenFrames );
	}

	return iStatus;
}
/*************************************************
Description:	收发单条命令函数
Input:
	cCmdIndex		命令索引
	pcReceiveBuffer	存放ECU回复数据的缓存

Output:	none
Return:	int	收发处理时的状态
Others:	按照ISO15765协议处理
*************************************************/
int send_and_receive_single_cmd_by_iso_15765( STRUCT_CMD *pstSendCmd, const int cCmdIndex, byte* pcReceiveBuffer )
{
	int		i = 0;
	int		j = 0;
	int		iReceiveStatus = TIME_OUT;
	byte	cIso15765ReservedByte	= g_p_stISO15765Config->cReserved;
	byte	cRetransTime			= g_p_stISO15765Config->cRetransTime;
	uint16	u16ECUResTimeout		= g_p_stISO15765Config->u16ECUResTimeout;
	bool	bSendStatus = false;
	int		iCmdLen = 0;
	bool	bMultiFrame = false;//多帧标志
	int 	iReceiveFCStatus = FAIL;//等待FC帧标志
	byte	cFrameSum = 0;
	STRUCT_CMD stCopySendCmd = {0};

	byte	cBufferOffset = 0;
	const byte ExtendCAN = 2;
	const byte StandardCAN = 0;

	//增加 扩展CAN支持 2015年10月20日16:46:37
	 
	if (g_CANoffset != 0 && g_CANoffset != 2)
	{
		return FORMAT_ERORR;
	}

	pstSendCmd->cBufferOffset	= g_stInitXmlGobalVariable.m_p_stCmdList[cCmdIndex].cBufferOffset;
	pstSendCmd->cReserved		= g_stInitXmlGobalVariable.m_p_stCmdList[cCmdIndex].cReserved;
	pstSendCmd->iCmdLen			= g_stInitXmlGobalVariable.m_p_stCmdList[cCmdIndex].iCmdLen;
	pstSendCmd->pcCmd			= ( byte* )malloc( sizeof( byte ) * pstSendCmd->iCmdLen );
	memcpy( pstSendCmd->pcCmd, g_stInitXmlGobalVariable.m_p_stCmdList[cCmdIndex].pcCmd, pstSendCmd->iCmdLen );

	cBufferOffset = pstSendCmd->cBufferOffset;

	if( pstSendCmd->iCmdLen > 2 && ( pstSendCmd->pcCmd[2 + g_CANoffset] & 0x10 ) == 0x10 )
	{
		bMultiFrame = true;
		iCmdLen = pstSendCmd->iCmdLen;

		cFrameSum = ( iCmdLen - 10 - g_CANoffset) / 7;
		cFrameSum += ( ( iCmdLen - 10 - g_CANoffset) % 7 == 0 ) ? 0 : 1;
	}

	//多帧发送，组装成一帧发送
	if( bMultiFrame )
	{
		i = j = cRetransTime;  //发送次数

		while( 1 )
		{
			stCopySendCmd.iCmdLen = pstSendCmd->iCmdLen + cFrameSum * (4 + g_CANoffset) + 4;   //命令长度
			stCopySendCmd.pcCmd = ( byte* )malloc( sizeof( byte ) * stCopySendCmd.iCmdLen );

			stCopySendCmd.pcCmd[0] = ( byte )g_p_stISO15765Config->cMultiframestime;	//多帧帧时间间隔

			stCopySendCmd.pcCmd[1] = 0x01; //帧属性  0x01: CAN多帧格式，第一帧有30帧回复； 0x02:发一帧等待回复一帧
			stCopySendCmd.pcCmd[2] = cFrameSum + 1; //帧个数

			stCopySendCmd.pcCmd[3] = 10 + g_CANoffset; //第一帧命令的字节个数

			memcpy( stCopySendCmd.pcCmd + 4, pstSendCmd->pcCmd, 10 + g_CANoffset );

			for( i = 0; i < cFrameSum; i++ )
			{
				if(g_CANoffset == StandardCAN)
				{
					stCopySendCmd.pcCmd[14 + i * 11 + 1] = stCopySendCmd.pcCmd[4]; //ID
					stCopySendCmd.pcCmd[14 + i * 11 + 2] = stCopySendCmd.pcCmd[5]; //ID
					stCopySendCmd.pcCmd[14 + i * 11 + 3] = 0x20 | ( ( i + 1 ) & 0x0F );
				}
				else if (g_CANoffset == ExtendCAN)
				{
					stCopySendCmd.pcCmd[16 + i * 13 + 1] = stCopySendCmd.pcCmd[4]; //ID
					stCopySendCmd.pcCmd[16 + i * 13 + 2] = stCopySendCmd.pcCmd[5]; //ID
					stCopySendCmd.pcCmd[16 + i * 13 + 3] = stCopySendCmd.pcCmd[6]; //ID
					stCopySendCmd.pcCmd[16 + i * 13 + 4] = stCopySendCmd.pcCmd[7]; //ID
					stCopySendCmd.pcCmd[16 + i * 13 + 5] = 0x20 | ( ( i + 1 ) & 0x0F );
				}

				if( i != cFrameSum - 1 ) //如果不是最后一帧
				{
					memcpy( stCopySendCmd.pcCmd + (14 + g_CANoffset) + i * (11 + g_CANoffset) + (4 + g_CANoffset), pstSendCmd->pcCmd + (10 + g_CANoffset) + i * 7, 7 );
					stCopySendCmd.pcCmd[14 + g_CANoffset + i * (11 + g_CANoffset)] = 2 + g_CANoffset + 1 + 7; //每帧命令的字节个数

				}
				else
				{
					memcpy( stCopySendCmd.pcCmd + (14 + g_CANoffset) + i * (11 + g_CANoffset) + (4 + g_CANoffset), pstSendCmd->pcCmd + (10 + g_CANoffset) + i * 7, iCmdLen - (10 + g_CANoffset) - i * 7 );
					stCopySendCmd.pcCmd[14 + g_CANoffset + i * (11 + g_CANoffset)] = 2 + g_CANoffset + 1 + iCmdLen - (10 + g_CANoffset) - i * 7; //每帧命令的字节个数
				}
			}

			bSendStatus = package_and_send_frame( FRAME_HEAD_MULTI_FRAME, &stCopySendCmd, cIso15765ReservedByte );

			//释放内存
			if( NULL != stCopySendCmd.pcCmd )
			{
				free( stCopySendCmd.pcCmd );
				stCopySendCmd.pcCmd = NULL;
			}

			if( !bSendStatus )
			{
				return FAIL;
			}

			iReceiveStatus = process_CAN_receive_Cmd( cBufferOffset, pcReceiveBuffer );

			if( TIME_OUT == iReceiveStatus )
			{
				if( ( --j ) == 0 )
					return iReceiveStatus;
			}
			else
				return iReceiveStatus;
		}//end while
	}

	while( 1 )
	{
		bSendStatus = package_and_send_frame( FRAME_HEAD_NORMAL_FRAME, pstSendCmd, cIso15765ReservedByte );

		if( !bSendStatus )
		{
			return FAIL;
		}

		iReceiveStatus = process_CAN_receive_Cmd( cBufferOffset, pcReceiveBuffer );

		switch( iReceiveStatus )
		{
		case TIME_OUT:
		{
			if( ( --cRetransTime ) == 0 )
				return iReceiveStatus;


		}
		break;

		case SUCCESS:
		case FRAME_TIME_OUT:
		case NEGATIVE:

		default:
			return iReceiveStatus;

		}
	}
}
/*************************************************
Description:	处理标准CAN的接收
Input:	cBufferOffset	存储缓存偏移
		pcSource		接收ECU回复数据的缓存

Output:	none
Return:	int	收发处理时的状态
Others:	按照ISO15765协议处理接收，这里
		cBufferOffset对应的是g_stBufferGroup
		而pcSource是调用receive_cmd函数时暂存
		数据的地址；
*************************************************/
int process_CAN_receive_Cmd( const byte cBufferOffset, byte* pcSource )
{
	int	iReceiveStatus	= FAIL;
	bool	bSendStatus		= false;

	uint16	u16Code7F78Timeout	= g_p_stISO15765Config->u16Code7F78Timeout;
	uint16	u16ECUResTimeout	= g_p_stISO15765Config->u16ECUResTimeout;
	byte	cReserved			= g_p_stISO15765Config->cReserved;

	byte*	pcDestination		= g_stBufferGroup[cBufferOffset].cBuffer;

	byte	cBSmax				= 0;

	int		i = 0;
	int		iNegativeResponseCounter = 0;
	int		iValidLen = 0;

	//接收ECU回复命令的第一帧
	iReceiveStatus = process_CAN_receive_single_Cmd( pcSource, u16ECUResTimeout );

	if( iReceiveStatus != SUCCESS )
	{
		return iReceiveStatus;
	}

	if( ( pcSource[2 + g_CANoffset] == 0x03 ) && ( pcSource[3 + g_CANoffset] == 0x7f ) ) //判断消极相应
	{
		if( pcSource[5 + g_CANoffset] == 0x78 )
		{
			while( 1 ) //这个while是用来处理等待多个7F78，目前最多等200个
			{
				iReceiveStatus = process_CAN_receive_single_Cmd( pcSource, u16Code7F78Timeout );

				if( iReceiveStatus != SUCCESS )
				{
					return iReceiveStatus;
				}

				if( ( pcSource[2 + g_CANoffset] == 0x03 ) && ( pcSource[3 + g_CANoffset] == 0x7f ) && ( pcSource[5 + g_CANoffset] == 0x78 ) )
					iNegativeResponseCounter++;
				else if( ( pcSource[2 + g_CANoffset] == 0x03 ) && ( pcSource[3 + g_CANoffset] == 0x7f ) )
				{
					memcpy( pcDestination, &pcSource[3 + g_CANoffset], 3 ); //保存消极响应内容
					g_stBufferGroup[cBufferOffset].iValidLen = 3;
					return NEGATIVE;
				}
				else
					break;

				if( iNegativeResponseCounter == 200 )
					return TIME_OUT;

			}

		}//end if
		else
		{
			memcpy( pcDestination, &pcSource[3 + g_CANoffset], 3 ); //保存消极响应内容
			g_stBufferGroup[cBufferOffset].iValidLen = 3;
			return NEGATIVE;
		}

	}//end if

	if( ( pcSource[2 + g_CANoffset] & 0x10 ) != 0x10 ) //单帧
	{
		iValidLen = pcSource[2 + g_CANoffset];

		g_stBufferGroup[cBufferOffset].iValidLen = iValidLen;

		memcpy( pcDestination, &pcSource[3 + g_CANoffset], iValidLen );

		return SUCCESS;
	}

	//到这儿就认为是多帧

	//cBSmax	= g_stInitXmlGobalVariable.m_p_stCmdList[g_p_stISO15765Config->cFCCmdOffset].pcCmd[3];
	iValidLen = ( pcSource[2 + g_CANoffset] & 0x0F );
	iValidLen <<= 8;
	iValidLen += pcSource[3 + g_CANoffset];

	g_stBufferGroup[cBufferOffset].iValidLen = iValidLen;

	memcpy( pcDestination, &pcSource[4 + g_CANoffset], 6 ); //先保存第一帧数据

	pcDestination += 6;
	/*

		//发送FC帧
		bSendStatus = package_and_send_frame( FRAME_HEAD_NORMAL_FRAME,
											( STRUCT_CMD * ) & (g_stInitXmlGobalVariable.m_p_stCmdList[g_p_stISO15765Config->cFCCmdOffset]), cReserved );

		if( !bSendStatus )
		{
			return FAILE;
		}*/

	for( ; i < ( ( iValidLen - 6 ) / 7 ); i++ )
	{
		iReceiveStatus = process_CAN_receive_single_Cmd( pcSource, u16ECUResTimeout );

		if( iReceiveStatus != SUCCESS )
		{
			return iReceiveStatus;
		}

		memcpy( &pcDestination[i * 7], &pcSource[3 + g_CANoffset], 7 ); //保存完整一帧

	}//end for

	if( ( iValidLen - 6 ) % 7  == 0 ) //如果都是整帧就返回成功
	{
		return SUCCESS;
	}

	//接收剩余字节，包括1个长度字节、2个字节ID、CF帧的首字节和剩余的有效字节
	iReceiveStatus = process_CAN_receive_single_Cmd( pcSource, u16ECUResTimeout );

	if( iReceiveStatus != SUCCESS )
	{
		return iReceiveStatus;
	}

	memcpy( &pcDestination[i * 7], &pcSource[3 + g_CANoffset], ( ( iValidLen - 6 ) % 7 ) ); //保存最后不满的帧

	return SUCCESS;
}


/*************************************************
Description:	处理标准CAN的接收

Input:
	pcSource		接收ECU回复数据的缓存
	u16Timeout		接收一帧命令的超时

Output:	pcSource	接收ECU回复数据的缓存
Return:	int	收发处理时的状态
Others:	VCI返回数据的格式是：1个字节有效字节长度
							+2个字节ID+有效字节
*************************************************/
int process_CAN_receive_single_Cmd( byte* pcSource, const uint16 u16Timeout )
{
	bool	bReceiveStatus	= false;
	byte	cValidLen = 0;//

	//先接收CAN帧内容的有效字节长度
	bReceiveStatus = ( bool )receive_cmd( &cValidLen, 1, u16Timeout );

	if( !bReceiveStatus )
		return TIME_OUT;

	//接收ID+有效数据
	bReceiveStatus = ( bool )receive_cmd( pcSource, cValidLen + 2 + g_CANoffset, u16Timeout );

	if( !bReceiveStatus )
		return TIME_OUT;

	return SUCCESS;

}

