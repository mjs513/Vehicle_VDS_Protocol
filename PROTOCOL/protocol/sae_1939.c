/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义根据ISO15765协议处理收发数据的相关函数
History:
	<author>	<time>		<desc>

************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sae_1939.h"
#include "../interface/protocol_interface.h"
#include "../public/protocol_config.h"
#include "../command/command.h"
#include "../InitConfigFromXml/init_config_from_xml_lib.h"
#include "../function/idle_link_lib.h"

/*************************************************
Description:	根据SAE1939协议收发函数
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
int send_and_receive_cmd_by_sae_1939( const int* piCmdIndex )
{
	int iStatus = TIME_OUT;
	int i = 0;
	int iCmdSum = piCmdIndex[0];
	byte cReceiveBuffer[50] = {0};
	STRUCT_CMD stSendCmd = {0};

	for( i = 0; i < iCmdSum; i++ )
	{
		iStatus = send_and_receive_single_cmd_by_sae_1939( ( STRUCT_CMD * )&stSendCmd, piCmdIndex[1 + i], cReceiveBuffer );

		if( NULL != stSendCmd.pcCmd )
			free( stSendCmd.pcCmd );

		if( ( iStatus != SUCCESS ) && ( iStatus != NEGATIVE ) ) //如果状态既不是SUCCESS又不是NEGATIVE则认为出错
		{
			break;
		}
        if (iCmdSum != 1)
        {
			time_delay_ms( g_p_stSAE1939Config->u16TimeBetweenFrames );
        }
		
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
Others:	按照SAE1939协议处理
*************************************************/
int send_and_receive_single_cmd_by_sae_1939( STRUCT_CMD *pstSendCmd, const int cCmdIndex, byte* pcReceiveBuffer )
{

	int		iReceiveStatus = TIME_OUT;
	byte	cRetransTime			= g_p_stSAE1939Config->cRetransTime;
	bool	bSendStatus = false;

	pstSendCmd->cBufferOffset	= g_stInitXmlGobalVariable.m_p_stCmdList[cCmdIndex].cBufferOffset;
	pstSendCmd->cReserved		= g_stInitXmlGobalVariable.m_p_stCmdList[cCmdIndex].cReserved;
	pstSendCmd->iCmdLen			= g_stInitXmlGobalVariable.m_p_stCmdList[cCmdIndex].iCmdLen;
	pstSendCmd->pcCmd			= ( byte* )malloc( sizeof( byte ) * pstSendCmd->iCmdLen );
	memcpy( pstSendCmd->pcCmd, g_stInitXmlGobalVariable.m_p_stCmdList[cCmdIndex].pcCmd, pstSendCmd->iCmdLen );



	
	while( 1 )
	{
		bSendStatus = SAE1939_package_and_send_frame( FRAME_HEAD_SAE1939_FRAME, pstSendCmd );

		if( !bSendStatus )
		{
			return FAIL;
		}

		iReceiveStatus = process_1939_receive_Cmd( pstSendCmd, pcReceiveBuffer );

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
Description:	处理1939的接收
Input:	cBufferOffset	存储缓存偏移
		pcSource		接收ECU回复数据的缓存

Output:	none
Return:	int	收发处理时的状态
Others:	按照SAE1939协议处理接收，这里
		cBufferOffset对应的是g_stBufferGroup
		而pcSource是调用receive_cmd函数时暂存
		数据的地址；
*************************************************/
int process_1939_receive_Cmd( STRUCT_CMD *pstSendCmd, byte* pcSource )
{
	int		i = 0;
	byte	cValidLen = 0;//
	bool	bReceiveStatus	= false;
	int     receive_times = g_p_stSAE1939Config->u16ECUResTimeout/100;
	int     idNumer = 0;
	uint16  u16receive_id = 0;
	uint16  u16Filter[10] = {0};
    byte frametiem =0;

// 03 (18 EA FF F1) (18 EC FF 00) (18 EB FF 00) (01) (07) (18 EA FF F1 D3 FE 00)
	idNumer = pstSendCmd->pcCmd[0];
	for (i = 0; i < idNumer; i++)
	{
		u16Filter[i] = (uint16)((pstSendCmd->pcCmd[i*4+2]<<8) | pstSendCmd->pcCmd[i*4+3]);
	}
	while(1)
	{
		do 
		{
			if (!(( bool )receive_cmd( &cValidLen, 1, 100 )))
			{
				continue;
			}
			else
			{
				break;
			}
		} while (receive_times--);

		if (receive_times <= 0)
		{
			return TIME_OUT;
		}             // 收到一个字节  

	
		do 
		{
			if (!(( bool )receive_cmd( pcSource, cValidLen+4, 100 )))
			{
				continue;
			}
			else
			{
				break;
			}
		} while (receive_times--);

		if (receive_times <= 0)
		{
			
			return TIME_OUT;
		}              // 收到全部数据

		u16receive_id = (uint16)((pcSource[1]<<8) | pcSource[2]);
      
		if (u16receive_id == u16Filter[0])  //回复单帧
		{
			save_SEA1939_single_data(pstSendCmd,pcSource+4);
			return SUCCESS;	
		} else if (u16receive_id == u16Filter[idNumer-3])  //4个滤波时第二个ID收单帧
		{
			save_SEA1939_single_data_Reply(pstSendCmd,pcSource+4);
			return SUCCESS;	
		} else if(u16receive_id == u16Filter[idNumer-2]) //回复多帧的第一帧 20帧
		{
			save_SEA1939_20_frame_data(pstSendCmd,pcSource+4);
			frametiem = 0;
		}else if(u16receive_id == u16Filter[idNumer-1]) //其余的所有多帧
		{
			frametiem = save_SEA1939_Multi_Frame_data(pstSendCmd,pcSource+4,frametiem);
		   if (g_stInitProtocolGobalVariable.bGlobal_SEA1939_Multi_Frame_status)
		   {
			   g_stInitProtocolGobalVariable.bGlobal_SEA1939_Multi_Frame_status = false;
			   return SUCCESS;
		   }
		} 
	}
	
    return SUCCESS;

}


void save_SEA1939_single_data(STRUCT_CMD *pstSendCmd,byte* pcSource)
{
	byte cBufferOffset = pstSendCmd->cBufferOffset;
	byte*	pcDestination		= g_stBufferGroup[cBufferOffset].cBuffer;
	g_stBufferGroup[cBufferOffset].iValidLen = 8; //都是满帧
	memcpy(pcDestination,pcSource,8);

}



void save_SEA1939_single_data_Reply(STRUCT_CMD *pstSendCmd,byte* pcSource)
{
	byte cBufferOffset = pstSendCmd->cBufferOffset;
	byte*	pcDestination		= g_stBufferGroup[cBufferOffset].cBuffer;
	uint16  u16SendID = 0;
	uint16  u16ReceiveID = 0;
	u16ReceiveID = (uint16)((pcSource[6]<<8)|pcSource[5]);//PGN
	u16SendID = (uint16)((pstSendCmd->pcCmd[2]<<8) | pstSendCmd->pcCmd[3]);
	if (u16SendID == u16ReceiveID)
	{
		g_stBufferGroup[cBufferOffset].iValidLen = 8; //都是满帧
		memcpy(pcDestination,pcSource,8);
	}
}

void save_SEA1939_20_frame_data(STRUCT_CMD *pstSendCmd,byte* pcSource)
{
	byte cBufferOffset = pstSendCmd->cBufferOffset;
	byte*	pcDestination		= g_stBufferGroup[cBufferOffset].cBuffer;
    uint16  u16SendID = 0;
	uint16  u16ReceiveID = 0;
	if (g_stInitProtocolGobalVariable.bGlobal_SEA1939_20Frame_status == false)
	{
		
        u16ReceiveID = (uint16)((pcSource[6]<<8)|pcSource[5]);//PGN
        u16SendID = (uint16)((pstSendCmd->pcCmd[2]<<8) | pstSendCmd->pcCmd[3]);
		if (u16SendID == u16ReceiveID)
        {
			g_stInitProtocolGobalVariable.bGlobal_SEA1939_Multi_Frame_Package_Number = pcSource[3]; //接收多帧的包号
			g_stInitProtocolGobalVariable.u16Global_SEA1939_Multi_Frame_ID = u16ReceiveID;
			g_stInitProtocolGobalVariable.bGlobal_SEA1939_20Frame_status = true;
			g_stBufferGroup[cBufferOffset].iValidLen = (int)((pcSource[2]<<8)|pcSource[1]);//
			memcpy(pcDestination+g_stBufferGroup[cBufferOffset].iValidLen,pcSource+1,7);
			
		}
	}

}

byte save_SEA1939_Multi_Frame_data(STRUCT_CMD *pstSendCmd,byte* pcSource,byte frametiem)
{
	byte cBufferOffset = pstSendCmd->cBufferOffset;
	byte*	pcDestination		= g_stBufferGroup[cBufferOffset].cBuffer;
	
	if (g_stInitProtocolGobalVariable.bGlobal_SEA1939_20Frame_status == true) //已经收到20帧
	{
		frametiem++;
		if (frametiem == pcSource[0])
		{
			
			if (frametiem == g_stInitProtocolGobalVariable.bGlobal_SEA1939_Multi_Frame_Package_Number)
			{
				g_stInitProtocolGobalVariable.bGlobal_SEA1939_20Frame_status = false;
				g_stInitProtocolGobalVariable.bGlobal_SEA1939_Multi_Frame_status = true; //收完
				memcpy(pcDestination+(frametiem-1)*7, pcSource+1, g_stBufferGroup[cBufferOffset].iValidLen-7*(frametiem-1));
			}
			else
			{
				memcpy(pcDestination+(frametiem-1)*7,pcSource+1,7);
				g_stInitProtocolGobalVariable.bGlobal_SEA1939_Multi_Frame_status = false;
			}
		}
		else
		{
			frametiem--;

		}
	}
	return frametiem;

}


/*************************************************
Description:	打包并发送命令帧
Input:
	cFrameHead		命令帧头
	pstFrameContent	具体命令结构体指针
	cReservedByte	命令中保留字节

Output:	none
Return:	bool	返回发送状态（成功、失败）
Others:	该函数会尝试发送三次，根据收发装置
		回复的内容判断发送状态；
*************************************************/

bool SAE1939_package_and_send_frame( const byte cFrameHead, STRUCT_CMD* pstFrameContent )
{
	UNN_2WORD_4BYTE uFrameLen;
	//bool bReturnStatus = false;
	byte *pcSendCache = NULL;
	byte cReceiveCache[20] = {0};//接收的缓存
	byte cCheckNum = 0;
	int i = 0;
    int receive_times = 0;
    byte cValidLen = 0;//

    receive_times = g_p_stSAE1939Config->u16VDI5ByteTimeout/100;

	uFrameLen.u32Bit = 1 + 2 + 1 + pstFrameContent->iCmdLen + 1;

	pcSendCache = ( byte * )malloc( ( uFrameLen.u32Bit ) * sizeof( byte ) ); //发送帧的缓存

	pcSendCache[0] = cFrameHead;
	pcSendCache[1] = uFrameLen.u8Bit[1];
	pcSendCache[2] = uFrameLen.u8Bit[0];
	pcSendCache[3] = pstFrameContent->cReserved;

	memcpy( &pcSendCache[4], pstFrameContent->pcCmd, pstFrameContent->iCmdLen );

	for( i = 0; ( i < ( int )uFrameLen.u32Bit - 1 ); i++ )
	{
		cCheckNum += pcSendCache[i];
	}

	pcSendCache[ uFrameLen.u32Bit - 1] = cCheckNum;

	send_cmd( pcSendCache, uFrameLen.u32Bit );
    
	while (1)
	{
		do 
		{
			if (!(( bool )receive_cmd( &cValidLen, 1, 100 )))
			{
				continue;
			}
			else
			{
				break;
			}
		} while (receive_times--);

		if (receive_times <= 0)
		{
			free( pcSendCache );
			return false;
		}             // 收到一个字节  
		
        if (cValidLen == 0)
        {
			break; //收到确认字节
        }

		do 
		{
			if (!(( bool )receive_cmd( cReceiveCache, cValidLen+4, 100 )))
			{
				continue;
			}
			else
			{
				break;
			}
		} while (receive_times--);

		if (receive_times <= 0)
		{
			free( pcSendCache );
			return false;
		}             // 收到全部数据  
	}

	do 
	{
		if (!(( bool )receive_cmd( cReceiveCache, 4, 100 )))
		{
			continue;
		}
		else
		{
			if( cReceiveCache[2] == 0x00 )
			{
				free( pcSendCache );
				return true;
			}
		}
	} while (receive_times--);

	if (receive_times <= 0)
	{
		free( pcSendCache );
		return false;
	} 
	return true;

}


