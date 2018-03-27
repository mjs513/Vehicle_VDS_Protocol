/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	声明各接口函数
History:
	<author>	<time>		<desc>
DEMO Version: V2.1 2017.3.30
************************************************************************/


#ifndef _INTERFACE_H
#define _INTERFACE_H


#include "protocol_define.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifdef WIN32
#ifdef PROTOCOL_DLL_EXPORTS
#	define PROTOCOL_API __declspec(dllexport)
#else
#	define PROTOCOL_API __declspec(dllimport)
#endif
#else
#	define PROTOCOL_API
#endif


	// ****************************************************************************
	typedef int ( *SEND_CMD_CALLBACK )( const byte*, int ); // 发送命令回调函数指针
	SEND_CMD_CALLBACK send_cmd;
	PROTOCOL_API void regist_send_cmd_callback( SEND_CMD_CALLBACK send_cmd_callback ); // 注册发送命令回调函数

	typedef int ( *RECEIVE_CMD_CALLBACK )( byte*, int nLen, int nTimeOut ); // 发送命令回调函数指针
	RECEIVE_CMD_CALLBACK receive_cmd;
	PROTOCOL_API void regist_receive_cmd_callback( RECEIVE_CMD_CALLBACK receive_cmd_callback ); // 注册接收命令回调函数

	typedef int ( *RECEIVE_ALL_CMD_CALLBACK )( byte*, int nLen, int nTimeOut ); // 接收带帧头命令回调函数指针
	RECEIVE_ALL_CMD_CALLBACK receive_all_cmd;
	PROTOCOL_API void regist_receive_all_cmd_callback( RECEIVE_ALL_CMD_CALLBACK receive_all_cmd_callback ); // 注册接收命令回调函数

	typedef void ( *TIME_DELAY )( int ); //延时回调函数指针
	TIME_DELAY time_delay_ms;
	PROTOCOL_API void regist_time_delay( TIME_DELAY time_delay_callback ); // 注册延时回调函数

	// 设置VCI
	PROTOCOL_API int setting_vci( void* pIn, void* pOut );

	// 激活ECU
	PROTOCOL_API int active_ECU( void* pIn, void* pOut );

	// 故障码功能
	PROTOCOL_API int read_current_Dtc( void* pIn, void* pOut );
	PROTOCOL_API int read_history_Dtc( void* pIn, void* pOut );
	PROTOCOL_API int clear_Dtc( void* pIn, void* pOut );

	// 冻结帧功能
	PROTOCOL_API int read_freeze_frame_DTC( void* pIn, void* pOut );
	PROTOCOL_API int read_freeze_frame_DS( void* pIn, void* pOut );

	// 读数据流
	PROTOCOL_API int read_data_stream( void* pIn, void* pOut );
	PROTOCOL_API int enter_read_data_stream( void* pIn, void* pOut );
	PROTOCOL_API int quit_read_data_stream( void* pIn, void* pOut );

	// 读版本信息
	PROTOCOL_API int read_Ecu_information( void* pIn, void* pOut );

	// 执行器测试
	PROTOCOL_API int actuator_test( void* pIn, void* pOut );

	// 特殊功能
	PROTOCOL_API int special_function( void* pIn, void* pOut );

	// 退出系统
	PROTOCOL_API int quit_system( void* pIn, void* pOut );

	//获取功能配置
	PROTOCOL_API int init_config_from_xml( int iConfigType, void* pIn );

	//获取特殊功能配置
	PROTOCOL_API int special_config_function( void* pIn, void* pOut );

	//等待框中按钮取消
	PROTOCOL_API void cancelWaitDlg(void* pIn, void* pOut);

	//扫描ECU版本
	PROTOCOL_API void process_ECU_version(void* pIn, void* pOut);
	
	//设置VDI自定义的命令
	PROTOCOL_API int XML_to_VDI(void* pIn, void* pOut);

	//自动扫描时读取系统的故障码个数
	PROTOCOL_API int read_dtc( void* pIn, void* pOut );

	//停止调用 此函数上位执行XML中CALLPROTOCOL之前调用此函数（此函数用于protocol函数的停止）
	PROTOCOL_API   void stopCallProtocol(void* pIn, void* pOut); 

#undef PROTOCOL_API

#ifdef __cplusplus
}
#endif

#endif/* _INTERFACE_H */

