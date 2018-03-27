/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	声明激活处理函数
History:
	<author>	<time>		<desc>
************************************************************************/
#ifndef _ACTIVE_ECU_LIB
#define _ACTIVE_ECU_LIB
#include "../interface/protocol_define.h"

enum ACTIVE_ECU_FUN
{
    ACTIVE_ECU_BY_GENERAL_CAN = 0,
    ACTIVE_ECU_BY_KWP,
    ACTIVE_ECU_BY_5_BAUD_ADDRESS,
};

enum ACTIVE_ECU_STATUS
{
    ACTIVE_ECU_SUCCESS = 0,
    ACTIVE_ECU_FAIL,
};

extern int g_iActiveECUStatus;

bool process_active_ECU_by_low_high_voltage( void* pIn, void* pOut );
// void process_active_ECU_by_general_CAN( void* pIn, void* pOut );
bool process_active_ECU_by_5_baud_address( void* pIn, void* pOut );
void process_active_ECU( void* pIn, void* pOut );
bool send_active_ECU_command( int * piActiveCmdIndex, void* pIn, void* pOut );

#endif