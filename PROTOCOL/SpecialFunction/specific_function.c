/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	特殊功能的函数处理文件，将所有的处理函数写在 process_special_function 上边。这样不用写函数声明。
                所有特殊功能的操作都在一个文件内进行。
History:
	<author>    张学岭
	<time>		2015年5月18日14:08:25
	<desc>      文件最后有 函数模板，可以作参考
************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "special_function.h"
#include "..\InitConfigFromXml\init_config_from_xml_lib.h"
#include <assert.h>
#include <time.h>



/*************************************************
Description:	写入处理函数
Input:
pIn		输入与特殊功能有关的命令数据
和从UI输入的内容
Output:	pOut	输出数据地址
Return:	void
Others:	根据第一个命令数据执行不同的功能函数
*************************************************/
bool write_Function_S( byte * cSpecialCmdData, const STRUCT_CHAIN_DATA_INPUT* pstParam, void * pOut )
{
	int i = 0;
	int iCmdSum = 0;
	uint32 u8CmdIndex[30];//特殊功能命令数据
	int iReceiveResult = TIME_OUT;
	byte cBufferOffset = 0;//缓存偏移
	int ipCmdIndex[2] = {1, 0};
	int WriteCmd = 0, ReadCmd = 0;
	byte InputData[20] = {0};
	uint32 user_permission = 0;  //高级权限

	iCmdSum  = get_string_type_data_to_uint32( u8CmdIndex, pstParam->pcData, pstParam->iLen );

	WriteCmd = u8CmdIndex[0];
	ReadCmd  = u8CmdIndex[1];

	//指向下一个结点
	pstParam = pstParam->pNextNode;

	if( pstParam->iLen != cSpecialCmdData[2] )
	{
		general_return_status( INVALID_INPUT, NULL, 0, pOut );
		return false;
	}

	for( i = 0; i < pstParam->iLen; i++ )
	{
		if( !(( pstParam->pcData[i] >= '0' && pstParam->pcData[i] <= '9' ) || ( pstParam->pcData[i] >= 'A' && pstParam->pcData[i] <= 'Z' )))
		{
			general_return_status( INVALID_INPUT, NULL, 0, pOut );
			return false;
		}
		InputData[i] = pstParam->pcData[i];
	}

	//特殊条件判定
	switch (cSpecialCmdData[3])
	{
	case 1:  //限定输入起始三个字符为“LNB”
		if ( pstParam->pcData[0] != 'L' || pstParam->pcData[1] != 'N' || pstParam->pcData[2] != 'B' )
		{
			general_return_status( INVALID_INPUT, NULL, 0, pOut );
			return false;
		}

		pstParam = pstParam->pNextNode;

		for ( i = 0; i < pstParam->iLen; i ++ )
		{
			user_permission = (pstParam->pcData[i]-0x30) + user_permission * 10;
		}

		if ( 1 != ( user_permission >> 31 ) )  //不为高级权限时
		{
			cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ReadCmd].cBufferOffset;
			ipCmdIndex[1] = ReadCmd;
			iReceiveResult = send_and_receive_cmd( ipCmdIndex );
			if( iReceiveResult != SUCCESS )
			{
				general_return_status( iReceiveResult, g_stBufferGroup[cBufferOffset].cBuffer, 3, pOut );
				return false;
			}
			if ( 0 == memcmp(g_stBufferGroup[cBufferOffset].cBuffer + 3,"LNB",3) )
			{
				special_return_status( PROCESS_OK | HAVE_JUMP | HAVE_TIP, "ID_LABLE_VIN_ALREAD_WRITE", NULL,0, pOut );
				return false;
			}
		}
		break;
	default:
		break;
	}

	iReceiveResult = process_single_cmd_without_subsequent_processing( g_iExtendSessionCmdIndex[1], pOut );
	if( !iReceiveResult )
	{
		return false;
	}
	iReceiveResult = process_security_access_algorithm( 0, pOut );
	if( !iReceiveResult )
	{
		return false;
	}

	memcpy( g_stInitXmlGobalVariable.m_p_stCmdList[WriteCmd].pcCmd + cSpecialCmdData[1], InputData, cSpecialCmdData[2] );

	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[WriteCmd].cBufferOffset;
	ipCmdIndex[1] = WriteCmd;
	iReceiveResult = send_and_receive_cmd( ipCmdIndex );
	if( iReceiveResult != SUCCESS )
	{
		general_return_status( iReceiveResult, g_stBufferGroup[cBufferOffset].cBuffer, 3, pOut );
		return false;
	}

	//提示成功
	special_return_status( PROCESS_OK | HAVE_JUMP | NO_TIP, "ACTUATOR_TEST_SUCCESS", NULL, 0, pOut );

	return true;
}

/*************************************************
Description:	写入Programming Date处理函数
Input:
pIn		输入与特殊功能有关的命令数据
和从UI输入的内容
Output:	pOut	输出数据地址
Return:	void
Others:	根据第一个命令数据执行不同的功能函数
*************************************************/
bool write_DATE_S( byte * cSpecialCmdData, const STRUCT_CHAIN_DATA_INPUT* pstParam, void * pOut )
{
	int i = 0;
	int iCmdSum = 0;
	uint32 u8CmdIndex[30];//特殊功能命令数据
	int iReceiveResult = TIME_OUT;
	byte cBufferOffset = 0;//缓存偏移
	int ipCmdIndex[2] = {1, 0};
	byte InputDATE[6];

	time_t nowTime;
	struct tm *sysTime;
	int year;
	int mon;
	int day;                   

	iCmdSum  = get_string_type_data_to_uint32( u8CmdIndex, pstParam->pcData, pstParam->iLen );

	//获得系统日期
	time(&nowTime);                 
	sysTime = localtime(&nowTime);
	year = sysTime->tm_year+1900; //年-2016
	mon = sysTime->tm_mon+1; //月-12    
	day = sysTime->tm_mday;  //日-13

	InputDATE[0] =0X30+(year%100)/10;
	InputDATE[1] =0X30+(year%100)%10;
	InputDATE[2] =0X30+mon/10;
	InputDATE[3] =0X30+mon%10;
	InputDATE[4] =0X30+day/10;
	InputDATE[5] =0X30+day%10;

	//进入扩展层，并发送安全算法
	iReceiveResult = process_single_cmd_without_subsequent_processing( g_iExtendSessionCmdIndex[1], pOut );
	if( !iReceiveResult )
	{
		return false;
	}
	iReceiveResult = process_security_access_algorithm( 0, pOut );
	if( !iReceiveResult )
	{
		return false;
	}

	//写入编程日期
	ipCmdIndex[1] = u8CmdIndex[0];
	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ipCmdIndex[1]].cBufferOffset;
	memcpy( g_stInitXmlGobalVariable.m_p_stCmdList[ipCmdIndex[1]].pcCmd + cSpecialCmdData[1], InputDATE, 6 );
	iReceiveResult = send_and_receive_cmd( ipCmdIndex );
	if( iReceiveResult != SUCCESS )
	{
		general_return_status( iReceiveResult, g_stBufferGroup[cBufferOffset].cBuffer, 3, pOut );

		return false;
	}

	//提示成功
	special_return_status( PROCESS_OK | HAVE_JUMP | NO_TIP, "ACTUATOR_TEST_SUCCESS", NULL, 0, pOut );
	return true;
}

/*************************************************
Description:	特殊功能处理函数
Input:
pIn		输入与特殊功能有关的命令数据
和从UI输入的内容
Output:	pOut	输出数据地址
Return:	void
Others:	根据第一个命令数据执行不同的功能函数
*************************************************/
void process_special_function( void* pIn, void* pOut )
{

	byte cSpecialCmdData[40] = {0};//存放特殊功能命令数据
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte FunctionSlect = 0;
	bool bStatus = false;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );
	FunctionSlect = atoi( pstParam->pcData );

	get_string_type_data_to_byte( cSpecialCmdData, pstParam->pcData, pstParam->iLen );
	//指向下一个结点
	pstParam = pstParam->pNextNode;

	/* 特殊功能的函数入口 */
	switch( FunctionSlect )
	{
		/* e.g.   process_Key_Match( cSpecialCmdData, pstParam, pOut );   */
	case 0:		//读取功能
		bStatus = read_function( cSpecialCmdData, pstParam, pOut );
		break;

	case 1:		//写入功能
		bStatus = write_function( cSpecialCmdData, pstParam, pOut );
		break;

	case 2:		//写入VIN
		bStatus = write_Function_S( cSpecialCmdData, pstParam, pOut );  //cSpecialCmdData：1：要修改命令的偏移位置，2：输入字符长度，3：限定输入范围的特殊情况
		break;

	case 3:		//写入编程日期，使用系统时间
		bStatus = write_DATE_S( cSpecialCmdData, pstParam, pOut );
		break;


	default:
		break;
	}

}

/*************************************************
Description:	读取函数（ASCII码显示）
Input:
pIn		输入与特殊功能有关的命令数据和从UI输入的内容
Output:	pOut	输出数据地址
Return:	bool
Others:
*************************************************/
bool read_function( byte* cSpecialCmdData, const STRUCT_CHAIN_DATA_INPUT* pstParam, void* pOut )
{
	byte i = 0;
	byte iCmdSum = 0;
	uint32 u8CmdIndex[30];//特殊功能命令数据
	bool iReceiveResult;
	byte cBufferOffset = 0;//缓存偏移
	byte ReadCmd;
	byte* pReadData = NULL;
	int iDataLen;

	iCmdSum  = get_string_type_data_to_uint32( u8CmdIndex, pstParam->pcData, pstParam->iLen );

	ReadCmd  = u8CmdIndex[iCmdSum - 1];

	for ( i = 0; i < iCmdSum; i++ )
	{
		iReceiveResult = process_single_cmd_without_subsequent_processing( u8CmdIndex[i], pOut);
		if( !iReceiveResult )
		{
			return false;
		}
	}
	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ReadCmd].cBufferOffset;
	pReadData = g_stBufferGroup[cBufferOffset].cBuffer + cSpecialCmdData[1];
	iDataLen = special_return_status( PROCESS_OK | NO_JUMP | NO_TIP, NULL, NULL, 0, pOut );
	add_data_and_controlsID( iDataLen,pReadData, cSpecialCmdData[2], ORIGINAL, "ID_TEXT_CODE", pOut );
	return true;
}
/*************************************************
Description:	写入函数（ASCII码写入）
Input:
pIn		输入与特殊功能有关的命令数据和从UI输入的内容
Output:	pOut	输出数据地址
Return:	bool
Others:	
*************************************************/
bool write_function( byte* cSpecialCmdData, const STRUCT_CHAIN_DATA_INPUT* pstParam, void* pOut )
{
	byte i = 0;
	byte iCmdSum = 0;
	uint32 u8CmdIndex[30];//特殊功能命令数据
	int iReceiveResult = TIME_OUT;
	byte cBufferOffset = 0;//缓存偏移
	byte WriteCmd;

	iCmdSum  = get_string_type_data_to_uint32( u8CmdIndex, pstParam->pcData, pstParam->iLen );

	WriteCmd  = u8CmdIndex[iCmdSum - 1];

	//指向下一个结点
	pstParam = pstParam->pNextNode;

	if( pstParam->iLen != cSpecialCmdData[2] )
	{
		general_return_status( INVALID_INPUT, NULL, 0, pOut );
		return false;
	}
	//判断输入字符是否是0-9或A-Z之间的数
	for( i = 0; i < pstParam->iLen; ++i )
	{
		if( !(( pstParam->pcData[i] >= '0' && pstParam->pcData[i] <= '9' ) || ( pstParam->pcData[i] >= 'A' && pstParam->pcData[i] <= 'Z' )))
		{
			general_return_status( INVALID_INPUT, NULL, 0, pOut );
			return false;
		}
	}

	memcpy( g_stInitXmlGobalVariable.m_p_stCmdList[WriteCmd].pcCmd + cSpecialCmdData[1], pstParam->pcData, cSpecialCmdData[2] );

	switch( cSpecialCmdData[3] ) /* 进入扩展层，带有安全访问 */
	{
		case 1://发送安全进入命令后发送安全算法
			iReceiveResult = process_single_cmd_without_subsequent_processing( g_iExtendSessionCmdIndex[1], pOut );
			if (!iReceiveResult)
			{
				return false;
			}
			//break;
		case 2://只发送安全算法
			iReceiveResult = process_security_access_algorithm( 0, pOut );
			if (!iReceiveResult)
			{
				return false;
			}
			break;
		default:
			break;
	}

	for ( i = 0; i < iCmdSum; i++ )
	{
		iReceiveResult = process_single_cmd_without_subsequent_processing( u8CmdIndex[i], pOut);
		if( !iReceiveResult )
		{
			return false;
		}
	}

	//提示成功
	special_return_status( PROCESS_OK | NO_JUMP | HAVE_TIP, NULL, "SUCCESS", 0, pOut );

	return true;
}
