/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义数据流数据处理函数
History:
	<author>	<time>		<desc>
************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ds_lib.h"
#include "../formula/formula_comply.h"
#include <assert.h>
#include "../command/command.h"
#include "../public/public.h"
#include "../public/protocol_config.h"
#include "../InitConfigFromXml/init_config_from_xml_lib.h"
#include "../formula_parse/interface.h"
#include "../protocol/iso_15765.h"
#include "../protocol/iso_14230.h"
#include "..\SpecialFunction\special_function.h"

//暂时保存数据流缓存及状态的数组，局部变量
STRUCT_DS_STORAGE_BUFFER * s_stDSBufferTempGroup = NULL;

STRUCT_SELECT_FUN stReadDataStreamFunGroup[] =
{
	{READ_GENERAL_DATA_STREAM, process_read_general_data_stream},
	{TOGETHER_READ_GENERAL_DATA_STREAM, process_together_read_general_data_stream},
};

/*************************************************
Description:	获取处理读取数据流函数
Input:
	cType		配置类型
Output:	保留
Return:	pf_general_function 函数指针
Others:
*************************************************/
pf_general_function get_read_data_stream_fun( byte cType )
{
	int i = 0;

	for( i = 0; i < sizeof( stReadDataStreamFunGroup ) / sizeof( stReadDataStreamFunGroup[0] ); i++ )
		if( cType == stReadDataStreamFunGroup[i].cType )
			return stReadDataStreamFunGroup[i].pFun;

	return 0;
}
/*************************************************
Description:	读数据流
Input:	pIn		输入参数
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
void process_read_data_stream( void* pIn, void* pOut )
{
	pf_general_function pFun = NULL;

	pFun = get_read_data_stream_fun( g_p_stProcessFunConfig->cDSFunOffset );

	assert( pFun );

	pFun( pIn, pOut );

}


/*************************************************
Description: 收取15765协议ECU回复的数据流命令
Input:	piCmdIndex 发送的命令号
Output:	int 	接收状态
Return:	保留
Others:
*************************************************/
int process_DS_receive_cmd_by_iso_15765(const int* piCmdIndex )
{
	int i =0;
	byte cBufferOffset = 0;
    byte cReceiveBuffer[20] = {0};
	int iReceiveResult = TIME_OUT;
	for (i = 0; i < piCmdIndex[0]; i++)
	{
		cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[piCmdIndex[i + 1]].cBufferOffset;
		//把当前数据流的缓存内容保存到临时的缓存中
        iReceiveResult = process_CAN_receive_Cmd( cBufferOffset, cReceiveBuffer );
		if( ( iReceiveResult != SUCCESS ) && ( iReceiveResult != NEGATIVE ) ) //如果状态既不是SUCCESS又不是NEGATIVE则认为出错
		{
			return  iReceiveResult;
		}
		if ( g_DSProrocess) // 数据流功能
		{
			s_stDSBufferTempGroup[i].iValidLen = g_stBufferGroup[cBufferOffset].iValidLen;

			memcpy( s_stDSBufferTempGroup[i].cBuffer, g_stBufferGroup[cBufferOffset].cBuffer,
				g_stBufferGroup[cBufferOffset].iValidLen );

			if( iReceiveResult == NEGATIVE )
			{
				s_stDSBufferTempGroup[i].bNegativeFlag = true;
			}
			else
			{
				s_stDSBufferTempGroup[i].bNegativeFlag = false;
			}
		}
	}

	return SUCCESS;

}


/*************************************************
Description: 收取14230协议ECU回复的数据流命令
Input:	piCmdIndex 发送的命令号
Output:	int 	接收状态
Return:	保留
Others:
*************************************************/
int process_DS_receive_cmd_by_iso_14230(const int* piCmdIndex )
{
	int i =0;
	byte cBufferOffset = 0;
	byte cReceiveBuffer[512] = {0};
	int iReceiveResult = TIME_OUT;
	for (i = 0; i < piCmdIndex[0]; i++)
	{

		cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[piCmdIndex[i + 1]].cBufferOffset;
		//把当前数据流的缓存内容保存到临时的缓存中
		iReceiveResult = process_KWP_receive_Cmd( cBufferOffset, cReceiveBuffer );
		if( ( iReceiveResult != SUCCESS ) && ( iReceiveResult != NEGATIVE ) ) //如果状态既不是SUCCESS又不是NEGATIVE则认为出错
		{
			return  iReceiveResult;
		}

		if (g_DSProrocess)
		{
			s_stDSBufferTempGroup[i].iValidLen = g_stBufferGroup[cBufferOffset].iValidLen;
			if (s_stDSBufferTempGroup[i].iValidLen != 0)
			{
				memcpy( s_stDSBufferTempGroup[i].cBuffer, g_stBufferGroup[cBufferOffset].cBuffer,
					g_stBufferGroup[cBufferOffset].iValidLen );
			}

			if( iReceiveResult == NEGATIVE )
			{
				s_stDSBufferTempGroup[i].bNegativeFlag = true;
			}
			else
			{
				s_stDSBufferTempGroup[i].bNegativeFlag = false;
			}

		}
	}

	return SUCCESS;

}

/*************************************************
Description: 收取ECU回复的数据流命令
Input:	piCmdIndex 发送的命令号
Output:	int 	接收状态
Return:	保留
Others:   小心： 数据流收取与多帧的收放到了一起 用g_DSProrocess 判断是不是数据流功能
*************************************************/
int process_DS_receive_Cmd(const int *piCmdIndex)
{
	int iReturnStatus = TIME_OUT;

	switch( g_stInitXmlGobalVariable.m_cProtocolConfigType ) //这里根据协议配置模板类型进行区分
	{
	case ISO15765_CONFIG://CAN
		iReturnStatus = process_DS_receive_cmd_by_iso_15765( piCmdIndex );
		break;

	case ISO14230_CONFIG://K线
		iReturnStatus = process_DS_receive_cmd_by_iso_14230( piCmdIndex );
		break;

	default:
		iReturnStatus = PROTOCOL_ERROR;
		break;
	}

	return iReturnStatus;
}
/*************************************************
Description: 发送15765协议数据流命令
Input:	piCmdIndex 发送的命令号
Output:	int 	接收状态
Return:	保留
Others:
*************************************************/

bool process_DS_send_cmd_by_iso_15765(const int*  piCmdIndex )
{
	STRUCT_CMD stCopySendCmd = {0};
	int i = 0;
	int iNumber = 0;
	bool bSendStatus = false;

	stCopySendCmd.iCmdLen = 5;
	for (i = 0; i < piCmdIndex[0]; i++)
	{
		stCopySendCmd.iCmdLen += g_stInitXmlGobalVariable.m_p_stCmdList[piCmdIndex[i + 1]].iCmdLen + 1;
	}
	stCopySendCmd.pcCmd = ( byte* )malloc( sizeof( byte ) * (stCopySendCmd.iCmdLen+1) );


	stCopySendCmd.pcCmd[0] = 5;  //VDI 发送的帧间隔时间
	stCopySendCmd.pcCmd[1] = 6;  //帧属性  接收超时后不再发送下一帧
	stCopySendCmd.pcCmd[2] = 0x03;
	stCopySendCmd.pcCmd[3] = 0xe8;  //VDI 等待ECU回复超时时间  1S 

	stCopySendCmd.pcCmd[4] = piCmdIndex[0];
	iNumber = 5;
	for( i = 0; i < piCmdIndex[0]; i++ )   //组装命令
	{
		stCopySendCmd.pcCmd[iNumber] = g_stInitXmlGobalVariable.m_p_stCmdList[piCmdIndex[i + 1]].iCmdLen; //命令字节数
		iNumber++;
		memcpy(stCopySendCmd.pcCmd+iNumber,g_stInitXmlGobalVariable.m_p_stCmdList[piCmdIndex[i + 1]].pcCmd,stCopySendCmd.pcCmd[iNumber-1]);
		iNumber += stCopySendCmd.pcCmd[iNumber-1];
	}

	bSendStatus = package_and_send_frame( FRAME_HEAD_MULTI_FRAME, &stCopySendCmd, g_p_stISO15765Config->cReserved );
	if( NULL != stCopySendCmd.pcCmd )
	{
		free( stCopySendCmd.pcCmd );
		stCopySendCmd.pcCmd = NULL;
	}
	return bSendStatus;
}

/*************************************************
Description: 发送14230协议数据流命令
Input:	piCmdIndex 发送的命令号
Output:	int 	接收状态
Return:	保留
Others:
*************************************************/

bool process_DS_send_cmd_by_iso_14230(const int*  piCmdIndex )
{
	STRUCT_CMD stCopySendCmd = {0};
	int i = 0;
	int iNumber = 0;
	bool bSendStatus = false;

	stCopySendCmd.iCmdLen = 5;
	for (i = 0; i < piCmdIndex[0]; i++)
	{
		stCopySendCmd.iCmdLen += g_stInitXmlGobalVariable.m_p_stCmdList[piCmdIndex[i + 1]].iCmdLen + 1;
	}
	stCopySendCmd.pcCmd = ( byte* )malloc( sizeof( byte ) * (stCopySendCmd.iCmdLen+1) );


	stCopySendCmd.pcCmd[0] = g_DsMulConfig.cDsFrameTime;  //VDI 发送的帧间隔时间
	stCopySendCmd.pcCmd[1] = g_DsMulConfig.cDsMode;  //帧属性  接收超时后不再发送下一帧
	stCopySendCmd.pcCmd[2] = 0;
	stCopySendCmd.pcCmd[3] = g_DsMulConfig.cDsTimeOut;  //VDI ECU回复完成的时间  比字节间隔时间大即可

	stCopySendCmd.pcCmd[4] = piCmdIndex[0];
	iNumber = 5;
	for( i = 0; i < piCmdIndex[0]; i++ )   //组装命令
	{
		stCopySendCmd.pcCmd[iNumber] = g_stInitXmlGobalVariable.m_p_stCmdList[piCmdIndex[i + 1]].iCmdLen; //命令字节数
		iNumber++;
		pre_process_cmd( &g_stInitXmlGobalVariable.m_p_stCmdList[piCmdIndex[i + 1]] ); //计算校验和
		memcpy(stCopySendCmd.pcCmd+iNumber,g_stInitXmlGobalVariable.m_p_stCmdList[piCmdIndex[i + 1]].pcCmd,stCopySendCmd.pcCmd[iNumber-1]);
		iNumber += stCopySendCmd.pcCmd[iNumber-1];
	}

	bSendStatus = package_and_send_frame( FRAME_HEAD_MULTI_FRAME, &stCopySendCmd, g_p_stISO14230Config->cTimeBetweenBytes );
	if( NULL != stCopySendCmd.pcCmd )
	{
		free( stCopySendCmd.pcCmd );
		stCopySendCmd.pcCmd = NULL;
	}
	return bSendStatus;
}

/*************************************************
Description: 发送数据流命令
Input:	piCmdIndex 发送的命令号
Output:	int 	接收状态
Return:	保留
Others:
*************************************************/
bool process_DS_send_Cmd(const int *piCmdIndex)
{
	bool bSendStatus = false;
	switch( g_stInitXmlGobalVariable.m_cProtocolConfigType ) //这里根据协议配置模板类型进行区分
	{
	case ISO15765_CONFIG://CAN
		bSendStatus = process_DS_send_cmd_by_iso_15765( piCmdIndex );
		break;

	case ISO14230_CONFIG://K线
		bSendStatus = process_DS_send_cmd_by_iso_14230( piCmdIndex );
		break;

	default:
		bSendStatus = false;
		break;
	}

	return bSendStatus;

}


/*************************************************
Description: 选择重发次数
Input:	void
Output:	byte 	重发次数
Return:	保留
Others:
*************************************************/
byte select_ResendTime_config()
{
	byte	cRetransTime = 0;
	switch( g_stInitXmlGobalVariable.m_cProtocolConfigType ) //这里根据协议配置模板类型进行区分
	{
	case ISO15765_CONFIG://CAN
		cRetransTime			= g_p_stISO15765Config->cRetransTime;
		break;

	case ISO14230_CONFIG://K线
		cRetransTime			= g_p_stISO14230Config->cRetransTime;
		break;

	default:
		cRetransTime = 0xff;
		break;
	}
	return cRetransTime;
}

/*************************************************
Description:	protocol把一屏数据流全部发给VDI，VDI把放回的结果一起上传
Input:	pIn		输入参数
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
void process_together_read_general_data_stream( void* pIn, void* pOut )
{
	UNN_2WORD_4BYTE iDsId;
	int iReceiveResult = TIME_OUT;
	int piCmdIndex[255] = {0};
	int i = 0, j = 0, k = 0;
	byte cItemSum = 0, CmdSum = 0;
	bool bSendStatus = false, bNegativeFlag;
	byte cRetransTime = 0;
	byte ReceiveBuffer[10] = { 0 };
	int iDataLen, iValidByteLenSum, iStartOffset, iValidByteLen;
	byte cContentCache[128] = { 0 };
	
	cRetransTime = select_ResendTime_config();
	if (cRetransTime == 0xff)
	{
		general_load_tipID( "Multi_frame_Resend_Time_False", pOut );
		return;
	}
    g_DSProrocess = true;

	//获取当前屏的数据流配置
	cItemSum = get_current_screen_DS_config( pIn );

	for( i = 0; i < cItemSum; i++ )
	{
		memcpy( piCmdIndex + 1 + CmdSum, g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[i].piDSCmdID, g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[i].iDsCmdIDLen * sizeof(int) );
		CmdSum += g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[i].iDsCmdIDLen;
	}

	piCmdIndex[0] = remove_duplicate_int_element( piCmdIndex + 1, CmdSum );

	s_stDSBufferTempGroup = malloc( sizeof( STRUCT_DS_STORAGE_BUFFER ) * CmdSum ); 
	
	for( i = 0; i < CmdSum; i++ )
	{
		s_stDSBufferTempGroup[i].iCmdID = piCmdIndex[i + 1];
	}

	g_MulCmdOffset = g_KwpFunSaveOffset.cDsOffset; //K线数据流读取多帧保存偏移

	while(1)
	{
		bSendStatus = process_DS_send_Cmd(piCmdIndex);
		if( !bSendStatus )
		{
			g_DSProrocess = false;
			iReceiveResult =  FAIL;
			general_return_status( iReceiveResult, NULL, 0, pOut );
			free(s_stDSBufferTempGroup);
			s_stDSBufferTempGroup = NULL;
			return;
		}

		iReceiveResult = process_DS_receive_Cmd(piCmdIndex);
		if( ( iReceiveResult != SUCCESS ) && ( iReceiveResult != NEGATIVE ) ) //如果状态既不是SUCCESS又不是NEGATIVE则认为出错
		{
			if (( --cRetransTime ) == 0)
			{
				g_DSProrocess = false;
				general_return_status( iReceiveResult, NULL, 0, pOut );
				free(s_stDSBufferTempGroup);
				s_stDSBufferTempGroup = NULL;
				return;
			} 
			else
			{
				continue;
			}			
		}
		else
		{
			break;
		}
	}
	iDataLen = special_return_status( PROCESS_OK | NO_JUMP | NO_TIP, NULL, NULL, 0, pOut );

	for( i = 0; i < cItemSum; i++ )
	{
		iDsId.u32Bit = g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[i].iDSID;

		memset( cContentCache, 0, sizeof( cContentCache ) );//缓存区初始化
		
		iValidByteLenSum = 0;
		bNegativeFlag = true;

		for( k = 0; k < g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[i].iDsCmdIDLen; k++ )
		{
			for ( j = 0; j < CmdSum; j++ )
			{
				if (g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[i].piDSCmdID[k] == s_stDSBufferTempGroup[j].iCmdID)
				{
					if ( s_stDSBufferTempGroup[j].bNegativeFlag )
					{
						//若为消极响应则结束双重循环，不再查找此数据流的其余命令
						k = g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[i].iDsCmdIDLen;
						bNegativeFlag = true;
					}
					else
					{
						iStartOffset = g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[i].pcValidByteOffset[k];
						iValidByteLen = g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[i].pcValidByteNumber[k];
						memcpy( ReceiveBuffer + iValidByteLenSum, s_stDSBufferTempGroup[j].cBuffer + iStartOffset, iValidByteLen );
						iValidByteLenSum += iValidByteLen;
						bNegativeFlag = false;
					}
					break;
				}
				
			}
		}
		if ( bNegativeFlag )
		{
			strcpy(cContentCache,"ID_STR_NEG_DS");
		}
		else
		{
			if( PROTOCOL_PARSER == g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[i].cFormulaType )
			{
				//调用protocol的计算公式
				process_normal_ds_calculate( iDsId.u32Bit, ReceiveBuffer, iValidByteLenSum, cContentCache );
			}
			else
			{
				//调用公式解析器
				calculate( ReceiveBuffer, iValidByteLenSum,
					g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[i].pcFormula,
					g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[i].iFormulaLen,
					g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[i].pStrFormat,
					cContentCache
					);
			}
		}
		iDataLen = add_data_and_controlsID( iDataLen, iDsId.u8Bit, 4, DEC_LITTLE_ENDIAN, cContentCache, pOut );
	}

	g_DSProrocess = false;
	free(s_stDSBufferTempGroup);
	s_stDSBufferTempGroup = NULL;
}
/*************************************************
Description:	发送读取数据流命令
Input:	ReceiveBuffer 保存回复命令的缓存偏移
        iValidByteNum 保存的有效字节个数
		DsId 数据流ID
		CmdSum 命令总数 （去重）
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
int send_receive_ds_cmd( byte * ReceiveBuffer, int * iValidByteNum, int DsId, int CmdSum )
{
	int k = 0, j = 0; 
	int KeyByteNum = 0; 
	byte cBufferOffset = 0;
	int iValidByteOffset, iValidByteNumber;
	int piCmdIndex[2] = { 1, 0 };
	int iReceiveResult = TIME_OUT;

	*iValidByteNum = 0;//将有效字节个数初始化为0

	for ( k = 0; k < g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[DsId].iDsCmdIDLen; k++ )
	{
		iValidByteOffset = g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[DsId].pcValidByteOffset[k];
		iValidByteNumber = g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[DsId].pcValidByteNumber[k];
		*iValidByteNum += iValidByteNumber;

		for( j = 0; j < CmdSum; j++ )
		{
			if ( s_stDSBufferTempGroup[j].iCmdID != -1 )
			{
				if( g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[DsId].piDSCmdID[k] == s_stDSBufferTempGroup[j].iCmdID )
				{
					if( s_stDSBufferTempGroup[j].bNegativeFlag == true )
						iReceiveResult = NEGATIVE;
					else
					{
						memcpy( ReceiveBuffer + KeyByteNum, s_stDSBufferTempGroup[j].cBuffer + iValidByteOffset, iValidByteNumber );
						KeyByteNum += iValidByteNumber;
						iReceiveResult = SUCCESS;
					}
					break;
				}
			}
			else
			{
				piCmdIndex[1] = g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[DsId].piDSCmdID[k];
				s_stDSBufferTempGroup[j].iCmdID = piCmdIndex[1];
				cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[piCmdIndex[1]].cBufferOffset;
				iReceiveResult = send_and_receive_cmd( piCmdIndex );
				if ( iReceiveResult == SUCCESS)
				{
					s_stDSBufferTempGroup[j].bNegativeFlag = false;
					memcpy( ReceiveBuffer + KeyByteNum, g_stBufferGroup[cBufferOffset].cBuffer + iValidByteOffset, iValidByteNumber );
					memcpy( s_stDSBufferTempGroup[j].cBuffer, g_stBufferGroup[cBufferOffset].cBuffer, g_stBufferGroup[cBufferOffset].iValidLen );
					KeyByteNum += iValidByteNumber;
					break;
				}
				else if ( iReceiveResult == NEGATIVE )
				{
					s_stDSBufferTempGroup[j].bNegativeFlag = true;
					return iReceiveResult;
				}
				else
					return iReceiveResult;
			}
		}
	}
	return iReceiveResult;
}
/*************************************************
Description:	处理读取普通据流函数
Input:	pIn		输入参数
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
void process_read_general_data_stream( void* pIn, void* pOut )
{
	UNN_2WORD_4BYTE iDsId;
	int iReceiveResult = TIME_OUT;
	int i = 0;
	byte cItemSum = 0;
	byte CmdSum = 0;
	byte ReceiveBuffer[512] = { 0 };
	int iValidByteLen = 0;
	byte cContentCache[128] = {0};
	int iDataLen = 0;
    byte g_bStopCallProtocol = false;
	byte cNegtiveDisplay[] = {"ID_STR_NEG_DS"};

	//获取当前屏的数据流配置
	cItemSum = get_current_screen_DS_config( pIn );

	for( i = 0; i < cItemSum; i++ )
	{
		CmdSum += g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[i].iDsCmdIDLen;
	}

	s_stDSBufferTempGroup = malloc( sizeof( STRUCT_DS_STORAGE_BUFFER ) * CmdSum ); 
	for ( i = 0; i < CmdSum; i++ )
	{
		s_stDSBufferTempGroup[i].iCmdID = -1; //初始化为-1
	}

	g_MulCmdOffset = g_KwpFunSaveOffset.cDsOffset; //K线数据流读取多帧保存偏移

	iDataLen = special_return_status( PROCESS_OK | NO_JUMP | NO_TIP, NULL, NULL, 0, pOut );
	for( i = 0; i < cItemSum; i++ )
	{
		if (g_bStopCallProtocol) 
		{
			special_return_status( PROCESS_OK|NO_JUMP|NO_TIP, NULL, NULL, 0, pOut );
			return ;
		} 
		iDsId.u32Bit = g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[i].iDSID;

		iReceiveResult = send_receive_ds_cmd( ReceiveBuffer, &iValidByteLen, i, CmdSum );//发送读取数据流命令

		memset( cContentCache, 0, sizeof( cContentCache ) );

		if ( iReceiveResult == SUCCESS )//命令发送成功，则按照公式计算结果
		{
			if( PROTOCOL_PARSER == g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[i].cFormulaType )
			{
				//调用protocol的计算公式
				process_normal_ds_calculate( iDsId.u32Bit, ReceiveBuffer, iValidByteLen, cContentCache );
			}
			else
			{
				//调用公式解析器
				calculate( ReceiveBuffer, iValidByteLen,
					g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[i].pcFormula,
					g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[i].iFormulaLen,
					g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[i].pStrFormat,
					cContentCache
					);
			}
		}
		else if ( iReceiveResult == NEGATIVE )//回复消极响应，则提示车辆不支持
		{
			strcpy(cContentCache,"ID_STR_NEG_DS");
		}
		else//命令发送失败则提示
		{
			general_return_status( iReceiveResult, NULL, 0, pOut );
			free(s_stDSBufferTempGroup);
			s_stDSBufferTempGroup = NULL;
			return;
		}
		iDataLen = add_data_and_controlsID( iDataLen, iDsId.u8Bit, 4, DEC_LITTLE_ENDIAN, cContentCache, pOut );
	}
	free(s_stDSBufferTempGroup);
	s_stDSBufferTempGroup = NULL;
}

/*************************************************
Description:	获取数据流配置数据
Input:
	pstDSConfig		指向数据流配置的指针
	pIn				输入内容

Output:	无
Return:	int 当前配置所占空间大小
Others: 注意:在调用本函数时需使用calloc函数申请
		STRUCT_DS_FORMULA_CONFIG类型变量的内存，
		以使内容在申请时即被初始化。
*************************************************/
int get_ds_config( STRUCT_DS_FORMULA_CONFIG * pstDSConfig, const byte * pcSource )
{

	byte cTemp[15] = {0};
	const byte * pcTemp = NULL;
	const byte * pcHead = NULL;
	byte * pcFormulaType = NULL;
	int iCmdSum = 0;
	int iLen = 0, iDataNum;
	uint32 u32Temp = 0;
	uint32 pu32Temp[5] = { 0 }; 

	if( NULL != pstDSConfig->pcValidByteNumber )
	{
		free( pstDSConfig->pcValidByteNumber );
		pstDSConfig->pcValidByteNumber = NULL;
	}

	if( NULL != pstDSConfig->pcValidByteOffset )
	{
		free( pstDSConfig->pcValidByteOffset );
		pstDSConfig->pcValidByteOffset = NULL;
	}

	if( NULL != pstDSConfig->piDSCmdID )
	{
		free( pstDSConfig->piDSCmdID );
		pstDSConfig->piDSCmdID = NULL;
	}

	if( NULL != pstDSConfig->pcFormula )
	{
		free( pstDSConfig->pcFormula );
		pstDSConfig->pcFormula = NULL;
	}

	if( NULL != pstDSConfig->pStrFormat )
	{
		free( pstDSConfig->pStrFormat );
		pstDSConfig->pStrFormat = NULL;
	}

	pcTemp = pcSource;
	pcHead = pcTemp;

	memcpy( cTemp, pcTemp, 4 );
	cTemp[4] = '\0';

	iLen = 4;

	iLen += strtol( cTemp, NULL, 16 );

	get_config_data( &u32Temp, pcTemp, iLen );

	pstDSConfig->iDSID = ( int )u32Temp; //获得DS的ID

	pcTemp += iLen;

	memcpy( cTemp, pcTemp, 4 );
	cTemp[4] = '\0';

	iLen = 4;

	iLen += strtol( cTemp, NULL, 16 );


	iCmdSum = get_config_data( pu32Temp, pcTemp, iLen );

	pstDSConfig->iDsCmdIDLen = iCmdSum;

	pstDSConfig->piDSCmdID = ( int * )malloc( sizeof( int ) * ( iCmdSum ) );

	memcpy( pstDSConfig->piDSCmdID, pu32Temp, iCmdSum * sizeof(int) ); //获得命令

	pcTemp += iLen;

	iLen = get_command_one_block_config_data( cTemp, &iDataNum, pcTemp );

	pstDSConfig->iValidByteOffsetLen = (byte)iDataNum;

	pstDSConfig->pcValidByteOffset = ( byte * )malloc( iDataNum ); //申请内存空间

	memcpy( pstDSConfig->pcValidByteOffset, cTemp, iDataNum ); //获得有效字节起始偏移

	pcTemp += iLen;

	iLen = get_command_one_block_config_data( cTemp, &iDataNum, pcTemp );

	pstDSConfig->iValidByteNumberLen = (byte)iDataNum;

	pstDSConfig->pcValidByteNumber = ( byte * )malloc( iDataNum ); //申请内存空间

	memcpy( pstDSConfig->pcValidByteNumber, cTemp, iDataNum ); //获得有效字节个数

	pcTemp += iLen;

	memcpy( cTemp, pcTemp, 4 );
	cTemp[4] = '\0';

	iLen = 4;

	iLen += strtol( cTemp, NULL, 16 );

	pcFormulaType = ( byte * )malloc( sizeof( byte ) * ( iLen - 4 + 1 ) );

	memcpy( pcFormulaType, pcTemp + 4, iLen - 4 ); //获得公式类型

	pcFormulaType[iLen - 4] = '\0';

	pstDSConfig->cFormulaType = FORMULA_PARSER;//默认为解析器处理

	if( 0 == strcmp( pcFormulaType, "FORMULA_PARSER" ) )
		pstDSConfig->cFormulaType = FORMULA_PARSER;

	if( 0 == strcmp( pcFormulaType, "PROTOCOL_PARSER" ) )
		pstDSConfig->cFormulaType = PROTOCOL_PARSER;

	free( pcFormulaType );
	pcTemp += iLen;

	memcpy( cTemp, pcTemp, 4 );
	cTemp[4] = '\0';

	iLen = 4;

	iLen += strtol( cTemp, NULL, 16 );

	pstDSConfig->pcFormula = ( byte * )malloc( sizeof( byte ) * ( iLen - 4 + 1 ) );

	memcpy( pstDSConfig->pcFormula, pcTemp + 4, iLen - 4 ); //获得公式内容
	pstDSConfig->iFormulaLen = iLen - 4;//获得公式内容的长度

	pstDSConfig->pcFormula[iLen - 4] = '\0';

	pcTemp += iLen;

	memcpy( cTemp, pcTemp, 4 );
	cTemp[4] = '\0';

	iLen = 4;

	iLen += strtol( cTemp, NULL, 16 );

	pstDSConfig->pStrFormat = ( byte * )malloc( sizeof( byte ) * ( iLen - 4 + 1 ) );

	memcpy( pstDSConfig->pStrFormat, pcTemp + 4, iLen - 4 ); //获得显示控制

	pstDSConfig->pStrFormat[iLen - 4] = '\0';

	pcTemp += iLen;

	return ( int )( pcTemp - pcHead );

}

/*************************************************
Description:	获取当前屏数据流配置数据
Input:
	pIn				输入内容

Output:	无
Return:	byte		当前屏数据流个数
Others: 输入内容为数据流总数+具体数据流配置。
注意这里g_stGeneralDSFormulaGroupConfig.cItemSum
保存的是最大值情况，而非每次的总数。
*************************************************/
byte get_current_screen_DS_config( void* pIn )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte cTemp	= 0;
	byte * pcTemp = NULL;
	byte j = 0;
	byte cItemSum = 0;
	int iLen = 0;
	int temp = 0;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	pcTemp = pstParam->pcData;

	iLen = get_command_one_block_config_data( &cTemp, &temp, pcTemp );
	cItemSum = cTemp;//获得当前屏数据流总数
	pcTemp += iLen;

	if( g_stGeneralDSFormulaGroupConfig.cItemSum < cItemSum ) //如果当前数据流条数小于需要的则重新分配
	{
		free_general_DS_formula_config_space();//释放已分配的

		g_stGeneralDSFormulaGroupConfig.cItemSum = cItemSum;

		g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig =
		    ( STRUCT_DS_FORMULA_CONFIG * )calloc( cItemSum, sizeof( STRUCT_DS_FORMULA_CONFIG ) );
	}

	for( j = 0; j < cItemSum; j++ )
	{
		iLen = get_ds_config( ( STRUCT_DS_FORMULA_CONFIG * ) & ( g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[j] ), pcTemp );
		pcTemp += iLen;
	}

	return cItemSum;
}

/*************************************************
Description:	释放存放普通数据流公式配置的空间
Input:	无
Output:	保留
Return:	无
Others: 每添加一类配置就在该函数中添加相应的释放代码
		并在quit_system_lib.c的free_xml_config_space
		函数中调用该函数。
*************************************************/
void free_general_DS_formula_config_space( void )
{
	byte j = 0;

	if( NULL != g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig )
	{
		for( j = 0; j < g_stGeneralDSFormulaGroupConfig.cItemSum; j++ )
		{
			if( NULL != g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[j].pcValidByteNumber )
			{
				free( g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[j].pcValidByteNumber );
				g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[j].pcValidByteNumber = NULL;
			}

			if( NULL != g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[j].pcValidByteOffset )
			{
				free( g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[j].pcValidByteOffset );
				g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[j].pcValidByteOffset = NULL;
			}

			if( NULL != g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[j].piDSCmdID )
			{
				free( g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[j].piDSCmdID );
				g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[j].piDSCmdID = NULL;
			}

			if( NULL != g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[j].pcFormula )
			{
				free( g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[j].pcFormula );
				g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[j].pcFormula = NULL;
			}

			if( NULL != g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[j].pStrFormat )
			{
				free( g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[j].pStrFormat );
				g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig[j].pStrFormat = NULL;
			}
		}

		free( g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig );
		g_stGeneralDSFormulaGroupConfig.pstDSFormulaConfig = NULL;
	}

}

/*************************************************
Description:	发送多帧数据发送接收函数
Input:  piCmdIndex:  命令号1 + 命令号2 + ......
        CmdSum:  命令总个数
Output:	
Return:	bool
Others:
*************************************************/
bool process_Multi_frame_cmd_To_VDI_processing(int CmdSum,byte* piCmd,void* pOut)
{
	int iReceiveResult = TIME_OUT;
	bool bSendStatus = false;
	int piCmdIndex[300] = {0};
	int i = 0;
	byte	cRetransTime			= 0;

	cRetransTime = select_ResendTime_config();
	if (cRetransTime == 0xff)
	{
		//special_return_status(PROCESS_FAIL|NO_JUMP|HAVE_TIP,NULL,"Multi_frame_Resend_Time_False",NULL,0,ORIGINAL,NULL,pOut);
		general_load_tipID( "Multi_frame_Resend_Time_False", pOut );
		return false;
	}

	piCmdIndex[0] = CmdSum;
	for (i = 0 ; i < CmdSum; i++)
	{
		piCmdIndex[i+1] = piCmd[i];
	}
	while(1)
	{
		bSendStatus = process_DS_send_Cmd(piCmdIndex);
		if( !bSendStatus )
		{
			iReceiveResult =  FAIL;			
			general_return_status( iReceiveResult, NULL, 0, pOut );
			return false;
		}

		iReceiveResult = process_DS_receive_Cmd(piCmdIndex);
		if( ( iReceiveResult != SUCCESS ) && ( iReceiveResult != NEGATIVE ) ) //如果状态既不是SUCCESS又不是NEGATIVE则认为出错
		{
			if (( --cRetransTime ) == 0)
			{			
				general_return_status( iReceiveResult, NULL, 0, pOut );
				return false;
			} 
			else
			{
				continue;
			}			
		}
		else
		{
			break;
		}
	}
	return true;
}


