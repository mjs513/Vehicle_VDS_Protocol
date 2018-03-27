/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	声明部分通用的函数
History:
	<author>	<time>		<desc>

************************************************************************/
#ifndef _PUBLIC_H
#define _PUBLIC_H

#include "../interface/protocol_define.h"

enum JUDGE_INPUT_VALUE_STATUS
{
    INPUT_FORMAT_ERROR = 0,	//输入格式错误
    INPUT_OUT_OF_RANGE,		//输入超出范围
    INPUT_WITHIN_THE_RANGE,	//输入在范围内
};

void general_return_status( const int iStatus, const byte* pcSource, const byte cAppendLen, void* pOut );
int general_load_tipID( const byte* pcTipID, void* pOut );
int send_and_receive_cmd( const int* piCmdIndex );

void process_general_response( const byte* pcSource, const int iValidLen, STRUCT_CHAIN_DATA_OUTPUT* pstOut );
bool process_single_cmd_without_subsequent_processing( const int iCmdOffset, void* pOut );

byte calculate_Checksum( const byte* pcSource, const int iValidLen );
bool package_and_send_vci_config( void );
void exit_1939_package_and_send_vci_config( void );
bool switch_channel( byte cReceivePin, byte cSendPin );
bool change_communicate_with_ECU_baudrate( uint32 uuBandRate );
bool package_and_send_active_config( void );

void change_cmd_space( const int iCmdOffset, const int iNewCmdLen );
bool reset_VCI( void );

int remove_duplicate_int_element( int * pi, byte num );

int judge_input_value( const int iRangeLen, const byte* pcRange, const int iValueLen, const byte* pcValue );

void process_SET_CONFIG_FC_CMD_CAN( void* pIn, void* pOut );
bool bSend_FC_Frame_Cmd( const STRUCT_CMD * FcCmdData, const uint32 FcFrameTime, const uint32 WhichByte, const uint32 MultiByte );
void process_SET_ECU_REPLY( void* pIn, void* pOut );
bool bSend_Ecu_Reply_Cmd( int ECUReplyFlag, int ECUReplyTimeOut );
void get_accord_ecu_cmdnum_send_cmdconfig_data( void* pIn, void* pOut );
bool bSend_Receive_config_Frame_Cmd( const STRUCT_CMD * SetCmdData, const int SetFrameNumber, const uint32 * u32Config_fuc );
void process_ds_mul_mode(void* pIn, void* pOut);
void set_mul_frame_offset(void* pIn, void* pOut);
bool package_and_send_frame( const byte cFrameHead, STRUCT_CMD* pstFrameContent, const byte cReservedByte );
void pre_process_cmd( STRUCT_CMD* pstSnedCmd );
bool receive_confirm_byte( int wait_time );

#endif