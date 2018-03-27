/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:	声明退出系统处理函数
History:
	<author>	<time>		<desc>

***********************************************************************/
#ifndef _QUIT_SYSTEM_LIB
#define _QUIT_SYSTEM_LIB
#include "../interface/protocol_define.h"

enum QUIT_SYSTEM_TYPE
{
    GENERAL_QUIT_SYSTEM = 0,
};

void process_quit_system( void* pIn, void* pOut );
void process_general_quit_system( void* pIn, void* pOut );
void free_xml_config_space( void );
#endif