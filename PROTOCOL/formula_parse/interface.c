#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include "Parser.h"
#include "interface.h"

typedef enum {false = 0, true} bool;

int calculate( const char* pcDataSource, const int nDataSourceLen,
               const char* pcFormula, const int nFormulaLen,
               char *pStrFormat, char* pOutStringData )
{
	char szBuffer[MAX_SIZE] = {'\0'};
	char szFormula[MAX_SIZE] = {'\0'};
	//char szSwitch[MAX_SIZE] = {'\0'};
	int nFormulaRealLen = 0;
	int nSwitchLen = 0;
	bool PiecewizeFlag = false;
	
	double dResult = 0;
	OPERATE operate[MAX_SIZE];
	int nResult = 0;
	int i = 0;
	int n = 0;
	int nType = 0;

	//初始化缓冲区
	memset( szBuffer, '\0', MAX_SIZE );
	memset( operate, '\0', sizeof( OPERATE ) * MAX_SIZE );

	//判断数据源是否为空
	if( NULL == pcDataSource )
	{
		return -1;//表示数据源为空
	}

	//判断输出字符串指针是否为空
	if( NULL == pOutStringData )
	{
		return -2;//表示输出字符串指针为空
	}

	//判断格式化字符串是否为空
	if( NULL == pStrFormat )
	{
		return -3;//判断格式化字符串是否为空
	}

	nType = GetFormatType( pStrFormat );

	if( nType == 0 )
	{
		//ASCII显示
		for( i = 0; i < nDataSourceLen; i++ )
		{
			sprintf( ( pOutStringData + i ), "%c", *( pcDataSource + i ) );
		}

		return 1;
	}
	else if( nType == 1 )
	{
		//Hex显示
		for( i = 0; i < nDataSourceLen; i++ )
		{
			sprintf( pOutStringData, "%02X", ( unsigned char )( *( pcDataSource + i ) ) );
			pOutStringData += 2;
		}

		return 1;
	}
	else if( nType == 2 )
	{
		//Decimal十进制显示
		for( i = 0; i < nDataSourceLen; i++ )
		{
			n = sprintf( pOutStringData, "%d", *( pcDataSource + i ) );
			pOutStringData += n;
		}

		return 1;
	}
	else if( 0 != nFormulaLen )//判断公式是否为空
	{
		char* szSwitch = malloc( sizeof( char ) * nFormulaLen );
		memset( szSwitch, '\0', sizeof( char ) * nFormulaLen );

		//拆分公式和Switch
		ParseFormulaAndSwitch( pcFormula, nFormulaLen, szFormula, szSwitch );

		nFormulaRealLen = ( int )strlen( szFormula );
		nSwitchLen = ( int )strlen( szSwitch );

		if( nFormulaRealLen > 0 )
		{
			//置换公式中的占位符A-Z
			PutDataToFormula( pcDataSource, nDataSourceLen, szFormula, nFormulaRealLen, szBuffer );

			//处理公式中的负号
			HandleMinus( szBuffer );

			//用单个字母替换数学表达式
			FormulaProc( szBuffer );

			//根据运算符的优先级格式化公式
			FormatFormula( szBuffer, operate );

			//计算数值
			dResult = CalculateValue( operate, 3.14159266 );
			if (nSwitchLen > 0)
			{
				//根据数值找到对应的字符串ID
				nResult = ( unsigned int )dResult;
				SwitchFunction( nResult, szSwitch, nSwitchLen, pOutStringData );
				if ( *pOutStringData =='%' )
				{
					memset( pStrFormat, '\0', strlen( pStrFormat ) );
					for ( i = 0; i < (int)strlen(pOutStringData); i++ )
					{
						if ( i != 0 && *(pOutStringData + i) =='%' )
						{
							i++;
							break;
						}
						*(pStrFormat + i) = *(pOutStringData + i);
					}
					nType = GetFormatType( pStrFormat );
					nFormulaRealLen = (int)strlen(pOutStringData + i);
					//置换公式中的占位符A-Z
					PutDataToFormula( pcDataSource, nDataSourceLen, pOutStringData + i, nFormulaRealLen, szBuffer );

					//处理公式中的负号
					HandleMinus( szBuffer );

					//用单个字母替换数学表达式
					FormulaProc( szBuffer );

					//根据运算符的优先级格式化公式
					FormatFormula( szBuffer, operate );

					//计算数值
					dResult = CalculateValue( operate, 3.14159266 );
				}
			}

			if( nType == 3 )
			{
				//整数数据的格式化
				nResult = ( int )dResult;
				sprintf( pOutStringData, pStrFormat, nResult );
			}
			else if( nType == 4 )
			{
				//浮点型的数据格式化
				sprintf( pOutStringData, pStrFormat, dResult );
			}
			else if (nType == 6)
			{
				//整数数据格式化，无符号数据输出
				nResult = (unsigned int)dResult;
				sprintf(pOutStringData, pStrFormat, nResult );
			}
		}
		free( szSwitch );
	}
	return 1;
}