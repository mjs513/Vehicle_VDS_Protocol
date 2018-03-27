/**********************************************************************
Copyright:		YiXiong AUTO S&T Co., Ltd.
Description:
	1.定义获取故障码状态函数；
	2.定义处理版本信息显示方式函数；
	3.定义处理普通数据流计算函数；
	4.定义处理冻结帧数据流计算函数；
	5.定义处理安全进入算法函数；

History:
	<author>	<time>		<desc>
************************************************************************/

#include <string.h>
#include "formula_comply.h"
#include "formula.h"
#include "../command/command.h"
#include "../public/public.h"
#include "../InitConfigFromXml/init_config_from_xml_lib.h"

int	g_iDefaultSessionCmdIndex[]		= {1, 0};
int	g_iExtendSessionCmdIndex[]		= {1, 0};
int	g_iRequestSeedCmdIndex[]		= {1, 0};
int	g_iSendKeyCmdIndex[]			= {1, 0};


//故障码各状态位为1的显示
byte *DTCStatusBitTrueArry[] =
{
	"DTC_BIT0_TRUE",
	"DTC_BIT1_TRUE",
	"DTC_BIT2_TRUE",
	"DTC_BIT3_TRUE",
	"DTC_BIT4_TRUE",
	"DTC_BIT5_TRUE",
	"DTC_BIT6_TRUE",
	"DTC_BIT7_TRUE"

};
//故障码各状态位为0的显示
byte *DTCStatusBitFalseArry[] =
{
	"DTC_BIT0_FALSE",
	"DTC_BIT1_FALSE",
	"DTC_BIT2_FALSE",
	"DTC_BIT3_FALSE",
	"DTC_BIT4_FALSE",
	"DTC_BIT5_FALSE",
	"DTC_BIT6_FALSE",
	"DTC_BIT7_FALSE"
};
//J1939 FMI
byte *DTCDescriptionArry[] = 
{
	"ID_STR_J1939_FMI_00",
	"ID_STR_J1939_FMI_01",
	"ID_STR_J1939_FMI_02",
	"ID_STR_J1939_FMI_03",
	"ID_STR_J1939_FMI_04",
	"ID_STR_J1939_FMI_05",
	"ID_STR_J1939_FMI_06",
	"ID_STR_J1939_FMI_07",
	"ID_STR_J1939_FMI_08",
	"ID_STR_J1939_FMI_09",
	"ID_STR_J1939_FMI_0A",
	"ID_STR_J1939_FMI_0B",
	"ID_STR_J1939_FMI_0C",
	"ID_STR_J1939_FMI_0D",
	"ID_STR_J1939_FMI_0E",
	"ID_STR_J1939_FMI_0F",
	"ID_STR_J1939_FMI_10",
	"ID_STR_J1939_FMI_11",
	"ID_STR_J1939_FMI_12",
	"ID_STR_J1939_FMI_13",
	"ID_STR_J1939_FMI_14",
	"ID_STR_J1939_FMI_15",
	"ID_STR_J1939_FMI_16",
	"ID_STR_J1939_FMI_17",
	"ID_STR_J1939_FMI_18",
	"ID_STR_J1939_FMI_19",
	"ID_STR_J1939_FMI_1A",
	"ID_STR_J1939_FMI_1B",
	"ID_STR_J1939_FMI_1C",
	"ID_STR_J1939_FMI_1D",
	"ID_STR_J1939_FMI_1E",
	"ID_STR_J1939_FMI_1F",
};


/*************************************************
Description:	获得故障码状态
Input:
	cDctStatusData	故障码状态字节
	cDtcMask		故障码mask值

Output:	pcOut	结果输出地址
Return:	int		该故障码支持的状态个数
Others:
*************************************************/

int get_Dtc_status( byte cDctStatusData, byte *pcOut, byte cDtcMask )
{
	int i = 0;
	int iSupportStatusCounter = 0;//支持的状态计数
	byte temp_Status = 0;
	byte temp_SupportStatus = 0;
  
	while( i < 8 )
	{
		temp_SupportStatus = ( ( cDtcMask >> i ) & 0x01 );
		temp_Status = ( ( cDctStatusData >> i ) & 0x01 );

		if( 0x01 == temp_SupportStatus )//位为1
		{

			if( iSupportStatusCounter != 0 )
			{
				*pcOut = ',';
				pcOut++;
			}

			if( 0x01 == temp_Status )
			{
				memcpy( pcOut, DTCStatusBitTrueArry[i], strlen( DTCStatusBitTrueArry[i] ) );
				pcOut += strlen( DTCStatusBitTrueArry[i] );
			}
			//else//位为0
			//{
			//	memcpy( pcOut, DTCStatusBitFalseArry[i], strlen( DTCStatusBitFalseArry[i] ) );
			//	pcOut += strlen( DTCStatusBitFalseArry[i] );
			//}

			iSupportStatusCounter++;
		}

		i++;

	}

	*pcOut = '\0';

	return iSupportStatusCounter;
}


/*************************************************
Description:	获得故障码状态
Input:
cDctStatusData	故障码状态字节
cDtcMask		故障码mask值

Output:	pcOut	结果输出地址
Return:	int		该故障码支持的状态个数
Others:
*************************************************/

int get_J1939Dtc_status( byte cDctStatusData, byte *pcOut, byte cDtcMask )
{
	memcpy( pcOut, DTCDescriptionArry[cDctStatusData&cDtcMask], strlen( DTCDescriptionArry[cDctStatusData&cDtcMask] ) );
	pcOut += strlen( DTCDescriptionArry[cDctStatusData&cDtcMask] );
	*pcOut = '\0';
	return 1;
}



/*************************************************
Description:	处理版本信息显示格式
Input:
	pcSource	取值地址
	cIvalidLen	有效长度
	cStyle		显示方式

Output:	pcOut	结果输出地址
Return:	void
Others:
*************************************************/

void process_inform_format( const byte* pcSource, byte cIvalidLen, byte cStyle, byte* pcOut )
{
	switch( cStyle )
	{
	case 'A'://ASCII码方式处理
		DisASCII( pcSource, cIvalidLen, pcOut );
		break;

	case 'H':
	case 'B':
		DisHex( pcSource, cIvalidLen, pcOut );
		break;

	case 'D':
		break;

	default:
		break;
	}
}
/************************************************************************/
/* 下面是数据流字符串公式用到的                                         */
/************************************************************************/
/*************************************************
typedef struct _STRUCT_STRING_UNIT
{
	byte cCompareData;		//要比较的数据
	byte* pcOutputString;	//要输出的字符串
} STRUCT_STRING_UNIT;
*************************************************/

STRUCT_STRING_UNIT stStringUintArray000[] =
{
	0x00, "ID_STR_DS_CSTRING_000",//关	Off
	0x01, "ID_STR_DS_CSTRING_001",//开	On
};

/*************************************************
typedef struct _STRUCT_DIS_STRING
{
byte cCompareDataSum;					//需要比较数据个数
byte* pcDefaultOutputString;			//不满足要求时输出的字符串
STRUCT_STRING_UNIT *pstStringUnitGroup;	//定义在上面
} STRUCT_DIS_STRING;
*************************************************/
STRUCT_DIS_STRING stDisStringArray[] =
{
	{0x02, "----", stStringUintArray000}, //00  bit.0=1:开 bit.0=0:关
};
/*************************************************
Description:	根据数据流ID处理数据流计算公式
Input:
	iDsId		数据流项ID
	pcDsSource	取值地址

Output:	pcDsValueOut	结果输出地址
Return:	void
Others:
DisplayString(pcDsSource,stDisStringArraypcDsSource,stDisStringArray,0,0xff,0,pcDsValueOut);
*************************************************/

void process_normal_ds_calculate( int iDsId, const byte* pcDsSource, int iValidByteNum, byte* pcDsValueOut )
{
	switch( iDsId )
	{
	case   0:
		OneByteOperation( pcDsSource, 0, 0, 1, 10, "%4.1f", pcDsValueOut ); //x/10
		break;
	default:
		break;
	}
}
/*************************************************
Description:	版本信息程序处理
Input:
iDsId
pcDsSource	取值地址

Output:	pcDsValueOut	结果输出地址
Return:	void
Others:
DisplayString(pcDsSource,stDisStringArraypcDsSource,stDisStringArray,0,0xff,0,pcDsValueOut);
*************************************************/
void process_normal_infor_calculate( int iDsId, const byte* pcDsSource, const int iByteLen, byte* pcDsValueOut )
{
	switch( iDsId )
	{
	case   0:
		OneByteOperation( pcDsSource, 0, 0, 1, 10, "%4.1f", pcDsValueOut ); //x/10
		break;
	default:
		break;
	}
}
/*************************************************
Description:	根据冻结帧数据流ID处理数据流计算公式
Input:
	iDsId		数据流项ID
	pcDsSource	取值地址

Output:	pcDsValueOut	结果输出地址
Return:	void
Others:
*************************************************/

void process_freeze_ds_calculate( int iDsId, const byte* pcDsSource, byte* pcDsValueOut )
{
	switch( iDsId )
	{
	case   0:
// 		DisplayString( pcDsSource, stDisStringArray, 1, 0X01, 0, pcDsValueOut ); //ACC.0=1~Present   ACC.0=0~Not Present
		break;
	default:
		break;
	}
}

/************************************************************************/
/* 下面是安全进入计算公式                                               */
/************************************************************************/

/*************************************************
  Description:	计算安全算法
  Calls:	seedToKey;
  Called By:	process_security_access_algorithm;

  Input:	Group	保存ECU回复seed的首地址

  Output:	Group	结果输出地址
  Return:	byte	key占用字节数
  Others:	函数定义会随系统而异
*************************************************/
byte security_calculate( byte *Group )
{
	//此处写安全算法公式


	return 2;//Key需要几个字节
}
/*************************************************
Description:	根据安全等级处理安全算法
Input:	cAccessLevel	安全等级

Output:	pOut	结果输出地址
Return:	bool	算法执行状态（成功、失败）
Others:	函数具体实现会因系统而异
*************************************************/

bool process_security_access_algorithm( byte cAccessLevel, void* pOut )
{
	bool bProcessSingleCmdStatus = false;
	byte cBufferOffset = 0;//缓存偏移
	byte cRequestSeedCmdOffset = 0;
	byte cSendKeyCmdOffset = 0;

	byte cDataArray[10] = {0};
	byte cNeedBytes = 0;

	int i = 0;

	//根据安全等级确定命令偏移
	switch( cAccessLevel )
	{
		case 0:
			cRequestSeedCmdOffset	= g_iRequestSeedCmdIndex[1];
			cSendKeyCmdOffset		= g_iSendKeyCmdIndex[1];
			break;

		default:
			return false;
	}

	cBufferOffset = g_stInitXmlGobalVariable.m_p_stCmdList[ cRequestSeedCmdOffset ].cBufferOffset;
	bProcessSingleCmdStatus = process_single_cmd_without_subsequent_processing( cRequestSeedCmdOffset, pOut );
	if( !bProcessSingleCmdStatus )
	{
		return false;
	}


	memcpy( cDataArray, ( g_stBufferGroup[cBufferOffset].cBuffer + 2 ), 4 );

	//判断seed是否全为0，若全为0则不发送key
	for ( i = 0; i < ( g_stBufferGroup[cBufferOffset].iValidLen - 2) ; i++ )
	{
		if ( cDataArray[i] != 0x00 )
		{
			break;
		}
		if ( i == ( g_stBufferGroup[cBufferOffset].iValidLen - 3))
		{
			return true;
		}
	}
	//根据安全等级确定计算公式
	switch( cAccessLevel )
	{
	case 0://安全等级为0
	{

		cNeedBytes = security_calculate( cDataArray );
	}
	break;

	default:
		break;
	}

	memcpy( ( g_stInitXmlGobalVariable.m_p_stCmdList[cSendKeyCmdOffset].pcCmd + 5), cDataArray, cNeedBytes );

	bProcessSingleCmdStatus = process_single_cmd_without_subsequent_processing( cSendKeyCmdOffset, pOut );

	if( !bProcessSingleCmdStatus )
	{
		return false;
	}

	return true;

}