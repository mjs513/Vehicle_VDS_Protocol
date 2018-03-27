/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	声明根据ISO14230协议处理收发数据的相关函数
History:
	<author>	<time>		<desc>

************************************************************************/
#ifndef _ISO_14230_H
#define _ISO_14230_H
#include "../interface/protocol_define.h"

int send_and_receive_cmd_by_iso_14230( const int* piCmdIndex );
int send_and_receive_single_cmd_by_iso_14230( STRUCT_CMD *pstSendCmd, const int cCmdIndex, byte* pcReceiveBuffer );
int process_KWP_receive_Cmd( const byte cBufferOffset, byte* pcSource );
int process_KWP_receive_single_Cmd( byte** ppcSource, int* piValidLen, const uint16 u16FrameTimeout );
bool package_and_send_frame_time( const byte cFrameHead, STRUCT_CMD* pstFrameContent, const byte cReservedByte );

#endif


