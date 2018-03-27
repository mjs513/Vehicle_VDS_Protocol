/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	声明空闲链接数据处理函数
History:
	<author>	<time>		<desc>

***********************************************************************/
#ifndef _IDLE_LINK_LIB
#define _IDLE_LINK_LIB
#include "../interface/protocol_define.h"

enum IDLE_LINK_STATUS
{
    IDLE_LINK_STOP = 0,
    IDLE_LINK_START,
};

enum IDLE_LINK_TYPE
{
    GENERAL_IDLE_LINK = 0,
    AFTER_CONFIG_IDLE_LINK,
};

void process_idle_link( void* pIn, void* pOut );

void process_general_idle_link( void* pIn, void* pOut );
int get_idle_link_command_config_data( void* pIn );

bool set_idle_link( byte cIdleLinkStatus );

void free_idle_link_command_config_space( void );

#endif