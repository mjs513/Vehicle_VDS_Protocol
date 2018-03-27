/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	定义各接口函数
History:
	<author>	<time>		<desc>

************************************************************************/

#define PROTOCOL_DLL_EXPORTS

#include <string.h>
#include <stdlib.h>

/*#define NDEBUG*/
#include <assert.h>

#include "protocol_interface.h"

#include "../function/dtc_lib.h"
#include "../function/clear_dtc_lib.h"
#include "../function/ds_lib.h"
#include "../function/infor_lib.h"
#include "../function/active_ecu_lib.h"
#include "../function/idle_link_lib.h"
#include "../function/freeze_lib.h"
#include "../function/actuator_test_lib.h"
#include "../function/scan_ecu_version.h"
#include "../SpecialFunction/special_function.h"
#include "../function/quit_system_lib.h"
#include "../InitConfigFromXml/init_config_from_xml_lib.h"
#include "../public/public.h"



//全局变量，用于退出死循环
bool g_bCancelWaitDlg = true;
//全部变量，用于停止跳出当前的protocol
bool g_bStopCallProtocol = false; 
/*************************************************
Description:	注册发送命令回调函数
Input:
	receive_cmd_callback	发送命令函数指针，由应用程序定义
Return:	void
Others:
*************************************************/
void regist_send_cmd_callback( SEND_CMD_CALLBACK send_cmd_callback )
{
	send_cmd = send_cmd_callback;
}

/*************************************************
Description:	注册接收命令回调函数
Input:
	receive_cmd_callback	接收命令函数，由应用程序定义
Return:	void
Others:
*************************************************/
void regist_receive_cmd_callback( RECEIVE_CMD_CALLBACK receive_cmd_callback )
{
	receive_cmd = receive_cmd_callback;
}

/*************************************************
Description:	注册接收所有带帧头命令回调函数
Input:
receive_cmd_callback	接收命令函数，由应用程序定义
Return:	void
Others:
*************************************************/
void regist_receive_all_cmd_callback( RECEIVE_ALL_CMD_CALLBACK receive_all_cmd_callback )
{
	receive_all_cmd = receive_all_cmd_callback;
}

/*************************************************
Description:	注册延时回调函数
Input:
	time_delay_callback		接收命令函数，由应用程序定义
Return:	void
Others:
*************************************************/
void regist_time_delay( TIME_DELAY time_delay_callback )
{
	time_delay_ms = time_delay_callback;
}

/*************************************************
Description:	设置VCI
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:	尝试发送三次若失败输出FAIL
*************************************************/
int setting_vci( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte *pcOutTemp = ( byte * )pOut;

	int iVciConfigOffset = 0;
	int iProtocolConfigOffset = 0;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	iVciConfigOffset = atoi( pstParam->pcData );//获取VCI配置偏移
	g_p_stVCI_params_config = g_p_stVciParamsGroup[iVciConfigOffset];

	g_CANoffset = g_p_stVCI_params_config->cCANFrameMode * 2;

	pstParam = pstParam->pNextNode;//获取VCI配置模板号

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	pstParam = pstParam->pNextNode;//获取协议配置偏移

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	iProtocolConfigOffset = atoi( pstParam->pcData );

	pstParam = pstParam->pNextNode;//获取协议配置偏移

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	g_stInitXmlGobalVariable.m_cProtocolConfigType = ( byte )atoi( pstParam->pcData ); //获取协议配置模板号(类型)

	//选择协议配置，根据相应协议选择相应配置
	select_protocol_config( iProtocolConfigOffset, g_stInitXmlGobalVariable.m_cProtocolConfigType );

	if( package_and_send_vci_config() ) //如果接收成功且设置成功则返回SUCCESS
	{
		general_return_status( SUCCESS, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//不提示
	}
	else
	{
		special_return_status( PROCESS_FAIL | HAVE_JUMP | NO_TIP, "ACTIVE_FAIL", NULL, 0, pcOutTemp );
	}

	return 0;
}
/*************************************************
Description:	从XML获取数据，直接发送给VDI
Input:	pIn		输入参数
Output:	pOut	
Return:	保留
Others:
*************************************************/
int XML_to_VDI( void* pIn, void* pOut )
{
	byte FunctionSlect = 0;
	bool bReturnStatus = false;
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte pcCmd[255] = { 0 };
	byte cLen = 0;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );
	
	cLen = get_string_type_data_to_byte( pcCmd, pstParam->pcData, pstParam->iLen );
	
	pcCmd[cLen - 1] = calculate_Checksum( pcCmd, cLen - 1 );
	pcCmd[2] = cLen;

	send_cmd( pcCmd, cLen );
	
	bReturnStatus = receive_confirm_byte( 3000 );
	if ( !bReturnStatus )
	{
		special_return_status( PROCESS_FAIL | HAVE_JUMP | NO_TIP, "ACTIVE_FAIL", NULL, 0, pOut );
	}
	else
	{
		general_return_status( SUCCESS, NULL, 0, pOut );
		((byte *)pOut)[2] = 0;//不提示
	}

	return bReturnStatus;
}
/*************************************************
Description:	激活ECU
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
int active_ECU( void* pIn, void* pOut )
{
	process_active_ECU( pIn, pOut );

	return 0;
}

//扫描ECU版本
void process_ECU_version(void* pIn, void* pOut)
{
	process_ECU_version_function( pIn, pOut );
}

/*************************************************
Description:	读取当前故障码
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
int read_current_Dtc( void* pIn, void* pOut )
{
	process_read_current_Dtc( pIn, pOut );

	return 0;
}

/*************************************************
Description:	读取历史故障码
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
int read_history_Dtc( void* pIn, void* pOut )
{
	process_read_history_Dtc( pIn, pOut );

	return 0;
}


/*************************************************
Description:	清除故障码
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
int clear_Dtc( void* pIn, void* pOut )
{

	process_clear_Dtc( pIn, pOut );

	return 0;
}


/*************************************************
Description:	读冻结帧故障码
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
int read_freeze_frame_DTC( void* pIn, void* pOut )
{
	process_read_freeze_frame_DTC( pIn, pOut );

	return 0;
}


/*************************************************
Description:	读冻结帧数据流
Input:	pIn		列表中DTC的ID
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
int read_freeze_frame_DS( void* pIn, void* pOut )
{
	process_read_freeze_frame_DS( pIn, pOut );

	return 0;
}

/*************************************************
Description:	读数据流
Input:	pIn		DS的ID
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
int read_data_stream( void* pIn, void* pOut )
{

	process_read_data_stream( pIn, pOut );

	return 0;
}

/*************************************************
Description:	进入读数据流
Input:	pIn		DS的ID
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
int enter_read_data_stream( void* pIn, void* pOut )
{


	return 0;
}
/*************************************************
Description:	退出读数据流
Input:	pIn		DS的ID
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
int quit_read_data_stream( void* pIn, void* pOut )
{


	return 0;
}

/*************************************************
Description:	读版本信息
Input:	pIn		版本信息组的ID
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
int read_Ecu_information( void* pIn, void* pOut )
{
	process_read_ECU_information( pIn, pOut );

	return 0;
}

/*************************************************
Description:	执行器测试
Input:	pIn		测试项的ID（带输入型的附加输入值）
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
int actuator_test( void* pIn, void* pOut )
{
	process_actuator_test( pIn, pOut );

	return 0;
}


/*************************************************
Description:	特殊功能
Input:	pIn		与执行功能有关的命令数据
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
int special_function( void* pIn, void* pOut )
{

	process_special_function( pIn, pOut );

	return 0;
}

/*************************************************
Description:	退出系统
Input:	pIn		保留
Output:	pOut	输出数据地址
Return:	保留
Others:
*************************************************/
int quit_system( void* pIn, void* pOut )
{

	process_quit_system( pIn, pOut );

	return 0;
}
/*************************************************
Description:	从xml获取系统配置
Input:
		iConfigType		配置类型
		PIn				具体配置内容
Output:	保留
Return:	保留
Others:
*************************************************/

int init_config_from_xml( int iConfigType, void* pIn )
{
	process_init_config_from_xml( iConfigType, pIn );

	return 0;
}



STRUCT_SELECT_FUN stSetConfigFunGroup[] =
{
	{SET_CONFIG_FC_CAN, process_SET_CONFIG_FC_CMD_CAN},
	{SET_ECU_REPLAY, process_SET_ECU_REPLY},
	{SET_SEND_AND_RECEIVE, get_accord_ecu_cmdnum_send_cmdconfig_data },
	{SET_MUL_DS_MODE, process_ds_mul_mode},
	{SET_MUL_FRAME_OFFSET, set_mul_frame_offset},

};


/*************************************************
Description:	获取处理激活函数
Input:
	cType		配置类型
Output:	保留
Return:	pf_general_function 函数指针
Others:
*************************************************/
pf_general_function get_set_config_fun( byte cType )
{
	int i = 0;

	for( i = 0; i < sizeof( stSetConfigFunGroup ) / sizeof( stSetConfigFunGroup[0] ); i++ )
		if( cType == stSetConfigFunGroup[i].cType )
			return stSetConfigFunGroup[i].pFun;

	return 0;
}

/*************************************************
Description:	 设置配置命令
Input:	PIn		保留
Output:	保留
Return:	保留
Others:
*************************************************/
int special_config_function( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	pf_general_function pFun = NULL;
	byte cConfigType = 0;
	int iConfigOffset = 0;

	do
	{
		assert( pstParam->pcData );
		assert( pstParam->iLen != 0 );
		iConfigOffset = atoi( pstParam->pcData ); //获得处理函数配置ID

		pstParam = pstParam->pNextNode;
		if (pstParam == NULL)
		{
			break;
		}
		assert( pstParam->pcData );
		assert( pstParam->iLen != 0 );
		cConfigType = ( byte )atoi( pstParam->pcData ); //获得处理函数配置模板号

		u32Config_fuc = u32Config_fuc_Group[iConfigOffset];


		pFun = get_set_config_fun( cConfigType );

		assert( pFun );

		pFun( pstParam, pOut );

		if( 1 != *( byte * )pOut )
		{
			return 0;
		}
		if (pstParam != NULL)
		{
			pstParam = pstParam->pNextNode;
		}
	}
	while( pstParam != NULL );

	return 0;
}



/*************************************************
Description:	 等待框中加按钮，此函数用于退出死循环
Input:	PIn		保留
Output:	保留
Return:	保留
Others:
*************************************************/
void cancelWaitDlg(void* pIn, void* pOut)
{
	g_bCancelWaitDlg = false;
}
/*************************************************
Description:在执行XML正CALLPROTOCOL之前调此函数
Input:	PIn		保留
Output:	保留
Return:	保留
Others:
*************************************************/
void stopCallProtocol(void* pIn, void* pOut)
{
	g_bStopCallProtocol = true;
}
/*************************************************
Description:	读取故障码（两种方式：通过命令1901或命令1902）
Input:	pIn		输入参数（保留）
Output:	pOut	输出数据地址
Return:	保留
Others:	尝试发送三次若失败输出FAIL
*************************************************/
int read_dtc( void* pIn, void* pOut )
{
	STRUCT_CHAIN_DATA_INPUT* pstParam = ( STRUCT_CHAIN_DATA_INPUT* )pIn;
	byte *pcOutTemp = ( byte * )pOut;
	int iActiveCmdSum = 0;
	byte cBufferOffset = 0;//缓存偏移
	byte DTCNumbers[4]= {0};

	int DTCType = 0;
	int DTCNumberStartOffset = 0;
	int DTCNumberBytes = 0;
	int DTCStartOffset = 0;
	int DTCBytesInCmd = 0;
	int iDataLen = 0;
	bool bProcessSingleCmdStatus = false;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	DTCType = atoi( pstParam->pcData );//获取故障码类型

	pstParam = pstParam->pNextNode;
	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	DTCNumberStartOffset = atoi( pstParam->pcData );//故障码个数的偏移

	pstParam = pstParam->pNextNode;
	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	DTCNumberBytes = atoi( pstParam->pcData );//故障码号个数由几个字节组成

	pstParam = pstParam->pNextNode;
	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	DTCStartOffset = atoi( pstParam->pcData );//从SID开始，故障码的起始偏移

	pstParam = pstParam->pNextNode;
	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	DTCBytesInCmd = atoi( pstParam->pcData );//几个字节表示一个故障码

	pstParam = pstParam->pNextNode;

	assert( pstParam->pcData );
	assert( pstParam->iLen != 0 );

	//获取激活命令条数、命令内容
	iActiveCmdSum = get_command_config_data( pstParam, &( g_stInitXmlGobalVariable.m_p_stCmdList ) );
	g_stInitXmlGobalVariable.m_iSpecificCmdGetFromXmlSum = iActiveCmdSum;

	if( 0 == iActiveCmdSum )
	{
		general_return_status( SUCCESS, NULL, 0, pcOutTemp );
		pcOutTemp[2] = 0;//不提示

		free_specific_command_config_space();

		return 0;
	}


	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ 0 ].cBufferOffset;

	bProcessSingleCmdStatus = process_single_cmd_without_subsequent_processing( 0, pOut );

	if( !bProcessSingleCmdStatus )
	{
		(( byte* )pOut)[0] = 2;  //失败,统一提示超时；（0x7F会提示fail，所以在此统一修改为timeout）
		return false;
	}

	switch (DTCType)
	{
	case 0:
		memcpy( DTCNumbers, g_stBufferGroup[cBufferOffset].cBuffer + DTCNumberStartOffset, DTCNumberBytes );
		iDataLen = special_return_status( PROCESS_OK | NO_JUMP | NO_TIP, NULL, NULL, 0, pOut);
		add_data_and_controlsID( iDataLen, DTCNumbers, DTCNumberBytes,ORIGINAL, NULL, pOut );
		break;
	case 1:
		DTCNumbers[0] = (g_stBufferGroup[cBufferOffset].iValidLen - DTCStartOffset)/DTCBytesInCmd;
		iDataLen = special_return_status( PROCESS_OK | NO_JUMP | NO_TIP, NULL, NULL, 0, pOut);
		add_data_and_controlsID( iDataLen, DTCNumbers, 1,ORIGINAL, NULL, pOut );
		break;
	default:
		break;
	}

	//停止发送空闲
	set_idle_link(0);	

	return 0;
}
