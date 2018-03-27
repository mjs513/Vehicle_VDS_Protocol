/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义根据ISO14230协议处理收发数据的相关函数
History:
	<author>	<time>		<desc>

************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "iso_14230.h"
#include "../interface/protocol_interface.h"
#include "../public/protocol_config.h"
#include "../command/command.h"
#include "../public/public.h" 
#include "../InitConfigFromXml/init_config_from_xml_lib.h"
#include "../function/idle_link_lib.h"

/*************************************************
Description:	根据ISO14230协议收发函数
Input:	piCmdIndex	命令索引地址
Output:	none
Return:	int	收发处理时的状态
Others:	根据命令索引可以发送和接收多条命令，
		把ECU积极相应的数据放到命令相应的缓存中
注意：这里存放ECU数据是从SID的回复开始的，如：
CMD:80 59 F1 01 81 4C RET:80 F1 59 03 C1 EA 8F 07
是从C1开始保存，若为消极相应则从7F开始保存。
*************************************************/
int send_and_receive_cmd_by_iso_14230( const int* piCmdIndex )
{
	int iStatus = TIME_OUT;
	int i = 0;
	int iCmdSum = piCmdIndex[0];
	byte cReceiveBuffer[256] = {0};
	STRUCT_CMD stSendCmd = {0};

	for( i = 0; i < iCmdSum; i++ )
	{
		iStatus = send_and_receive_single_cmd_by_iso_14230( ( STRUCT_CMD * )&stSendCmd, piCmdIndex[1 + i], cReceiveBuffer );

		if( NULL != stSendCmd.pcCmd )
			free( stSendCmd.pcCmd );

		if( ( iStatus != SUCCESS ) && ( iStatus != NEGATIVE ) ) //如果状态既不是SUCCESS又不是NEGATIVE则认为出错
		{
			break;
		}

		//time_delay_ms( g_p_stISO14230Config->u16TimeBetweenFrames );
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
Others:	按照ISO14230协议处理
*************************************************/
int send_and_receive_single_cmd_by_iso_14230( STRUCT_CMD *pstSendCmd, const int cCmdIndex, byte* pcReceiveBuffer )
{
	int		iReceiveStaus = TIME_OUT;
	byte	cTimeBetweenBytes		= g_p_stISO14230Config->cTimeBetweenBytes;
	byte	cRetransTime			= g_p_stISO14230Config->cRetransTime;
	bool	bSendStatus = false;

	byte	cBufferOffset = 0;


	pstSendCmd->cBufferOffset	= g_stInitXmlGobalVariable.m_p_stCmdList[cCmdIndex].cBufferOffset;
	pstSendCmd->cReserved		= g_stInitXmlGobalVariable.m_p_stCmdList[cCmdIndex].cReserved;
	pstSendCmd->iCmdLen			= g_stInitXmlGobalVariable.m_p_stCmdList[cCmdIndex].iCmdLen;
	pstSendCmd->pcCmd			= ( byte* )malloc( sizeof( byte ) * pstSendCmd->iCmdLen );
	memcpy( pstSendCmd->pcCmd, g_stInitXmlGobalVariable.m_p_stCmdList[cCmdIndex].pcCmd, pstSendCmd->iCmdLen );

	cBufferOffset = pstSendCmd->cBufferOffset;
	pre_process_cmd( pstSendCmd ); //计算校验和

	while( 1 )
	{
		switch( g_p_stVCI_params_config->cCommunicationType )
		{
		case 0x00:
			bSendStatus = package_and_send_frame( FRAME_HEAD_NORMAL_FRAME, pstSendCmd, cTimeBetweenBytes );
			break;

		case 0x01:
		case 0x50:
		case 0x51:/* 按时间上传 */
			bSendStatus = package_and_send_frame_time( FRAME_HEAD_NORMAL_FRAME, pstSendCmd, cTimeBetweenBytes );
			break;

		default:
			return PROTOCOL_ERROR;
			break;
		}


		if( !bSendStatus )
		{
			return FAIL;
		}

		iReceiveStaus = process_KWP_receive_Cmd( cBufferOffset, pcReceiveBuffer );

		switch( iReceiveStaus )
		{

		case TIME_OUT:
		{
			if( ( --cRetransTime ) == 0 )
				return iReceiveStaus;
		}
		break;

		case SUCCESS:
		case FRAME_TIME_OUT:
		case NEGATIVE:

		default:
			return iReceiveStaus;
		}
	}
}


/*************************************************
Description:	处理标准KWP的接收
Input:
	cBufferOffset	存储缓存偏移
	pcSource		接收ECU回复数据的缓存

Output:	none
Return:	int	收发处理时的状态
Others:	按照ISO14230协议处理接收，这里
		cBufferOffset对应的是g_stBufferGroup
		而pcSource是调用receive_cmd函数时暂存
		数据的地址；
*************************************************/
int process_KWP_receive_Cmd( const byte cBufferOffset, byte* pcSource )
{
	uint16	u16Code7F78Timeout	= g_p_stISO14230Config->u16Code7F78Timeout;
	uint16	u16ECUResTimeout	= g_p_stISO14230Config->u16ECUResTimeout;

	byte*	pcDestination		= g_stBufferGroup[cBufferOffset].cBuffer;

	int		iNegativeResponseCounter = 0;
	int		iValidLen = 0;
	int 	iReceiveStatus	= FAIL;

	iReceiveStatus = process_KWP_receive_single_Cmd( &pcSource, &iValidLen, u16ECUResTimeout );

	if( iReceiveStatus != SUCCESS )
	{
		return iReceiveStatus;
	}
	
	if (iValidLen == 0)
	{
		g_stBufferGroup[cBufferOffset].iValidLen = 0;
		return SUCCESS;
	}

	if( ( iValidLen == 0x03 ) && ( pcSource[0] == 0x7f ) ) //判断消极相应
	{
		if( pcSource[2] == 0x78 )
		{
			while( 1 ) //这个while是用来处理等待多个7F78，目前最多等200个
			{
				iReceiveStatus = process_KWP_receive_single_Cmd( &pcSource, &iValidLen, u16Code7F78Timeout );

				if( iReceiveStatus != SUCCESS )
				{
					return iReceiveStatus;
				}

				if( ( iValidLen == 0x03 ) && ( pcSource[0] == 0x7f ) && ( pcSource[2] == 0x78 ) )
					iNegativeResponseCounter++;
				else if( ( iValidLen == 0x03 ) && ( pcSource[0] == 0x7f ) )
				{
					memcpy( pcDestination, &pcSource[0], 3 ); //保存消极响应内容
					g_stBufferGroup[cBufferOffset].iValidLen = 3;
					return NEGATIVE;
				}
				else
					break;

				if( iNegativeResponseCounter == 200 )
					return TIME_OUT;

			}//end while

		}//end if
		else
		{
			memcpy( pcDestination, &pcSource[0], 3 ); //保存消极响应内容
			g_stBufferGroup[cBufferOffset].iValidLen = 3;
			return NEGATIVE;
		}

	}//end if

	//保存有效数据到指定的缓存中
	memcpy( pcDestination, pcSource, iValidLen );
	//修改缓存的有效字节
	g_stBufferGroup[cBufferOffset].iValidLen = iValidLen;

	return SUCCESS;
}
/************************************************************************
Description： 收取多帧数据                                                                      
Input      :  数据存储， 有效字节， 总长度
              利用递归算法进行运算
*************************************************************************/
static int process_receive_Mul_Cmd(byte **ppcSource, int* piValidLen, byte TotolLength)
{
	byte SingleFrameLength;
	bool bReceiveStatus = false;
	int  TempValidLen= 0;

	bReceiveStatus = ( bool )receive_cmd( *ppcSource, 1, 100 );

	switch( ( *ppcSource)[0] & 0xC0 )
	{
	case 0X00://没有地址信息型
		{
			TempValidLen = ( *ppcSource )[0];
			bReceiveStatus = ( bool )receive_cmd( *ppcSource, g_MulCmdOffset, 100);	
			if( !bReceiveStatus ) //
				return FRAME_TIME_OUT;		
			
			bReceiveStatus = ( bool )receive_cmd( *ppcSource, *piValidLen + 1 - g_MulCmdOffset, 100);	
			if( !bReceiveStatus ) //
				return FRAME_TIME_OUT;		

			SingleFrameLength = TempValidLen + 2;
		}
		break;

	case 0XC0:
	case 0X80:
		{
			if( ( ( *ppcSource )[0] != 0XC0 ) && ( ( *ppcSource )[0] != 0X80 ) ) //既不是0X80也不是0XC0
			{
				TempValidLen = (( *ppcSource )[0] & 0X3F );
				bReceiveStatus = ( bool )receive_cmd( *ppcSource, 2 + g_MulCmdOffset, 100 );	
				if( !bReceiveStatus ) //判断接收剩余部分时的状态
					return FRAME_TIME_OUT;			

				bReceiveStatus = ( bool )receive_cmd( *ppcSource, TempValidLen + 1 - g_MulCmdOffset, 100 );
				if( !bReceiveStatus ) //判断接收剩余部分时的状态
					return FRAME_TIME_OUT;			

				SingleFrameLength = TempValidLen + 4;				
			}
			else
			{
				bReceiveStatus = ( bool )receive_cmd( *ppcSource, 2 + 1, 100); //接收两个字节地址和一个字节长度
				if( !bReceiveStatus )
					return FRAME_TIME_OUT;

				TempValidLen = ( *ppcSource )[2];

				bReceiveStatus = ( bool )receive_cmd( *ppcSource, g_MulCmdOffset, 100 ); //
				if( !bReceiveStatus ) //判断接收剩余部分时的状态
					return FRAME_TIME_OUT;		

				bReceiveStatus = ( bool )receive_cmd( *ppcSource, TempValidLen + 1 - g_MulCmdOffset, 100 ); //根据长度字节接收
				if( !bReceiveStatus ) //判断接收剩余部分时的状态
					return FRAME_TIME_OUT;

				SingleFrameLength = TempValidLen + 5;
			}
		}
		break;	 

	default:
		return FORMAT_ERORR;
	}
	*piValidLen += (TempValidLen - g_MulCmdOffset);
	
	if (TotolLength - SingleFrameLength <= 0)
	{
		return SUCCESS;
	}
	else
	{
		*ppcSource += (TempValidLen - g_MulCmdOffset);
		return process_receive_Mul_Cmd( ppcSource, piValidLen, TotolLength - SingleFrameLength);
	}
}
/************************************************************************
Description： 9141收取多帧                                                                      
Input      :  
*************************************************************************/
static void process_9141_Mul_Cmd(byte** ppcSource, int *piValidLen, UNN_2WORD_4BYTE Key)
{
 	byte count = 0;
	byte *pSecond = NULL, *pHead = NULL;	
	byte *pTempSource = NULL;
	int Len = *piValidLen;
	bool bFindFlag = false;

	*piValidLen = 0;
	pSecond = *ppcSource;
	pTempSource = *ppcSource;
	for ( ;pSecond <= (*ppcSource + Len - 3); pSecond++ )
	{
		if (  *pSecond == Key.u8Bit[0] && 
			 *(pSecond + 1) == Key.u8Bit[1] && 
			 *(pSecond + 2) == Key.u8Bit[2] ) /* 找到相同的字节 */
		{
			bFindFlag = true;
			if (pHead == NULL)/* 第一次找到，将源地址移动到第一帧的数据域尾部 */
			{				 
				*piValidLen += (byte)(pSecond - pTempSource - 1);
				pHead = pSecond;
				pTempSource += (pHead - pTempSource - 1);				
			}
			else
			{				 
				count = (byte)(pSecond - pHead) - 4 - g_MulCmdOffset;/*实际有效数据*/
				*piValidLen += count;			
				memcpy(pTempSource, pHead + g_MulCmdOffset + 3, count);
				pTempSource += count;
				pHead = pSecond;
			}
		}

		if (bFindFlag)
		{
			if (pSecond == *ppcSource + Len - 3)
			{
				count = (byte)(pSecond + 3 - pHead) - 4 - g_MulCmdOffset;/*实际有效数据*/
				*piValidLen += count;			
				memcpy(pTempSource, pHead + g_MulCmdOffset + 3, count);
			}
		}		
	}
	if (*piValidLen == 0)
	{
		*piValidLen = Len;
	}
}

/*************************************************
Description:	处理以KWP协议方式接收单条回复函数
Input:	u16FrameTimeout			帧超时时间

Output:	ppcSource	存放接收数据地址的指针的地址
		piValidLen	接收数据有效字节

Return:	int	接收ECU回复的状态
Others: 按字节上传 ，上传的就是有效数据
        按时间上传， 上传的第一个字节是有效数据的长度，以后跟着的是有效数据
*************************************************/
int process_KWP_receive_single_Cmd( byte** ppcSource, int* piValidLen, const uint16 u16FrameTimeout )
{
	bool	bReceiveStatus	= false;
	uint16	u16FrameContentTimeout = g_p_stISO14230Config->u16ECUResTimeout;
	byte TimeTotalFrameLength = 0, SingleFrameLength = 0;
	byte *pTempSource = NULL;	     
	int status = TIME_OUT;
	UNN_2WORD_4BYTE ISO9141Head;
	ISO9141Head.u32Bit = 0;
 
	bReceiveStatus = ( bool )receive_cmd( *ppcSource, 1, u16FrameTimeout ); //接收第一个字节
	*piValidLen = **ppcSource;
	TimeTotalFrameLength = *piValidLen;

	if( !bReceiveStatus)
	{
		return TIME_OUT;
	}

	if (*piValidLen == 0)
	{
		return SUCCESS;
	}

	if( g_p_stGeneralActiveEcuConfig->cActiveMode == 0x06 ) /* 三菱1个字节 */
	{
		return SUCCESS;
	}
STATR_RECEIVE:
 	switch( g_p_stVCI_params_config->cCommunicationType ) //根据通用配置的模式
	{
	case 0x01:
	case 0x50:
	case 0x51:/* 按时间上传,先接收一个字节 */
	{
		bReceiveStatus = ( bool )receive_cmd( *ppcSource, 1, u16FrameTimeout ); //接收第一个字节
		if( !bReceiveStatus )
		{
			return TIME_OUT;
		}
	}
	break;

	default: /* 默认情况下是 单字节上传 */
		break;
	}

	switch( ( *ppcSource )[0] & 0xC0 )
	{
	case 0X00://没有地址信息型
	{
		*piValidLen = ( *ppcSource )[0];
		bReceiveStatus = ( bool )receive_cmd( *ppcSource, *piValidLen + 1, u16FrameContentTimeout );
		SingleFrameLength += *piValidLen + 2;
	}
	break;

	case 0XC0:
	case 0X80:
	{
		if( ( ( *ppcSource )[0] != 0XC0 ) && ( ( *ppcSource )[0] != 0X80 ) ) //既不是0X80也不是0XC0
		{
			*piValidLen = ( ( *ppcSource )[0] & 0X3F );
			bReceiveStatus = ( bool )receive_cmd( *ppcSource, *piValidLen + 3, u16FrameContentTimeout );
			if( !bReceiveStatus ) //判断接收剩余部分时的状态
				return FRAME_TIME_OUT;			
			
			*ppcSource += 2;//跳过两个字节的地址	
			SingleFrameLength += *piValidLen + 4;
		}
		else
		{
			bReceiveStatus = ( bool )receive_cmd( *ppcSource, 2 + 1, u16FrameContentTimeout ); //接收两个字节地址和一个字节长度

			if( !bReceiveStatus )
				return FRAME_TIME_OUT;

			*piValidLen = ( *ppcSource )[2];

			bReceiveStatus = ( bool )receive_cmd( *ppcSource, *piValidLen + 1, u16FrameContentTimeout ); //根据长度字节接收
			if( !bReceiveStatus ) //判断接收剩余部分时的状态
				return FRAME_TIME_OUT;

			SingleFrameLength += *piValidLen + 5;
		}
	}
	break;

	case 0X40://CARB模式
	{
		ISO9141Head.u8Bit[0] = *ppcSource[0];
		bReceiveStatus = ( bool )receive_cmd( *ppcSource, 2, u16FrameContentTimeout ); //6B + ECU地址
		if( !bReceiveStatus )
		{
			return FRAME_TIME_OUT;
		}
		if (*piValidLen <= 3)
		{
			return FRAME_TIME_OUT;
		}		
		ISO9141Head.u8Bit[1] = (*ppcSource)[0];
		ISO9141Head.u8Bit[2] = (*ppcSource)[1];
		bReceiveStatus = ( bool )receive_cmd( *ppcSource, *piValidLen - 3, u16FrameContentTimeout ); //接收剩余字节
		if( !bReceiveStatus )
		{
			return FRAME_TIME_OUT;
		}
		*piValidLen -= 3;
		process_9141_Mul_Cmd(ppcSource, piValidLen, ISO9141Head); /* 收取9141的数据 */	
		return SUCCESS;
	}
	default:
		return FORMAT_ERORR;
	}

	if ( (TimeTotalFrameLength > SingleFrameLength) && ((*piValidLen == 3) && ((*ppcSource)[0] ==0x7F) && ((*ppcSource)[2] == 0X78)) )
		goto STATR_RECEIVE;

	//处理按时间上传 速度过快，两帧合并为一帧上传的情况
	if(g_p_stVCI_params_config->cCommunicationType == 0x01  ||  
		 g_p_stVCI_params_config->cCommunicationType == 0x50 || 
		g_p_stVCI_params_config->cCommunicationType == 0x51)
	{
		if (TimeTotalFrameLength > SingleFrameLength)
		{
			pTempSource = *ppcSource;
			*ppcSource += *piValidLen;
			status = process_receive_Mul_Cmd(ppcSource, piValidLen, TimeTotalFrameLength - SingleFrameLength);  		      
			*ppcSource = pTempSource ;
			return status;
			 
 		}
	}

	return SUCCESS;
}
/*************************************************
Description:	打包并发送命令帧(时间可以控制)，
Input:
cFrameHead		命令帧头
pstFrameContent	具体命令结构体指针
cReservedByte	命令中保留字节

Output:	none
Return:	bool	返回发送状态（成功、失败）
Others:	该函数会尝试发送三次，根据收发装置
回复命令
*************************************************/
bool package_and_send_frame_time( const byte cFrameHead, STRUCT_CMD* pstFrameContent, const byte cReservedByte )
{
	UNN_2WORD_4BYTE uFrameLen;
	bool bReturnStatus = false;
	byte *pcSendCache = NULL;
	byte cCheckNum = 0;
	int i = 0;

	uFrameLen.u32Bit = 1 + 2 + 1 + pstFrameContent->iCmdLen + 1 + 2;

	pcSendCache = ( byte * )malloc( ( uFrameLen.u32Bit ) * sizeof( byte ) ); //发送帧的缓存

	pcSendCache[0] = cFrameHead;
	pcSendCache[1] = uFrameLen.u8Bit[1];
	pcSendCache[2] = uFrameLen.u8Bit[0];
	pcSendCache[3] = cReservedByte;

	pcSendCache[4] = 0x00;
	pcSendCache[5] = 0x37;

	memcpy( &pcSendCache[6], pstFrameContent->pcCmd, pstFrameContent->iCmdLen );

	for( i = 0; ( i < ( int )uFrameLen.u32Bit - 1 ); i++ )
	{
		cCheckNum += pcSendCache[i];
	}

	pcSendCache[ uFrameLen.u32Bit - 1] = cCheckNum;
	send_cmd( pcSendCache, uFrameLen.u32Bit );

	bReturnStatus = receive_confirm_byte( 3000 );

	free( pcSendCache );

	return bReturnStatus;
}
