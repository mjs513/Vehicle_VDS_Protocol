/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	声明根据ISO15765协议处理收发数据的相关函数
History:
	<author>	<time>		<desc>

************************************************************************/
#ifndef _SAE_1939_H
#define _SAE_1939_H
#include "../interface/protocol_define.h"

int send_and_receive_cmd_by_sae_1939( const int* piCmdIndex );
int send_and_receive_single_cmd_by_sae_1939( STRUCT_CMD *pstSendCmd, const int cCmdIndex, byte* pcReceiveBuffer );
int process_1939_receive_Cmd( STRUCT_CMD *pstSendCmd, byte* pcSource );
bool SAE1939_package_and_send_frame( const byte cFrameHead, STRUCT_CMD* pstFrameContent);
//bool package_and_send_frame_time( const byte cFrameHead, STRUCT_CMD* pstFrameContent, const byte cReservedByte );
int process_1939_receive_single_Cmd( byte* pcSource, const uint16 u16Timeout );
void save_SEA1939_single_data(STRUCT_CMD *pstSendCmd,byte* pcSource);
void save_SEA1939_20_frame_data(STRUCT_CMD *pstSendCmd,byte* pcSource);
byte save_SEA1939_Multi_Frame_data(STRUCT_CMD *pstSendCmd,byte* pcSource,byte frametiem);
void save_SEA1939_single_data_Reply(STRUCT_CMD *pstSendCmd,byte* pcSource);
#endif

