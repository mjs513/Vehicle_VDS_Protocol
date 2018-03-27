#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include "Parser.h"

static char Operator[] = "#(|^&<>+-*/";

static int InPriority[] = {0, 1, 3, 7, 9, 11, 13, 13, 15, 15, 17, 17};
static int OutPriority[] = {0, 20, 2, 6, 8, 10, 12, 12, 14, 14, 16, 16};

int IsDigit( char ch, int bHex )
{
	if( bHex )
	{
		if( ( '0' <= ch && '9' >= ch ) || '.' == ch || ( 'A' <= ch && 'F' >= ch ) )
			return TRUE;
	}

	if( ( '0' <= ch && '9' >= ch ) || '.' == ch )
		return TRUE;

	return FALSE;
}

void PutDataToFormula( const char* pcDataSource, const int nDataSourceLen,
                       const char* pSrcFormula, int nFormulaLen, char* pDestFormula )
{
	const char* p = pSrcFormula;
	char* q = pDestFormula;
	char cBuffer[MAX_SIZE] = {'\0'};
	int i = 0;
	int j = 0;
	int n = 0;
	unsigned char nTmpByte = 0;

	for( i = 0; i < nFormulaLen; i++ )
	{

		if( *p >= 'A' && *p <= 'Z' && j < nDataSourceLen )
		{
			memset( cBuffer, '\0', MAX_SIZE );
			//j = *p - 0x30;
			nTmpByte = *( pcDataSource + ( *p - 0x41 ) );
			sprintf( cBuffer, "%d", nTmpByte );

			for( n = 0; cBuffer[n] != '\0'; n++ )
			{
				*q = cBuffer[n];

				if( cBuffer[n + 1] != '\0' )
					q++;
			}

			j++;
		}
		else if( *p == ' ' || *p == '\'' || *p == '\"' )
		{
			p++;
			continue;
		}
		else
		{
			*q = *p;
		}

		p++;
		q++;
	}
}

//处理负号
void HandleMinus( char* pcSrc )
{
	char Buffer[MAX_SIZE];
	char *p = pcSrc, *q = Buffer;

	*q++ = *p++;

	while( '\0' != *p )
	{
		if( '-' == *p && '(' == *( p - 1 ) )
		{
			*q++ = '0';
			*q++ = '-';
			p++;
		}
		else
			*q++ = *p++;
	}

	*q = '\0';

	strcpy( pcSrc, Buffer );
}

//用单个字母替换数学表达式
void FormulaProc( char* pSrc )
{
	char Buffer[MAX_SIZE];
	char *p = pSrc, *q = Buffer;

	while( *p != '\0' )
	{
		switch( *p )
		{
		case 's':	//  A for sin
			*q++ = 'A';
			p += 3;
			break;

		case 'c':	// B for cos
			*q++ = 'B';
			p += 3;
			break;

		case 'e':	//C for exp
			*q++ = 'C';
			p += 3;
			break;

		case 'l':
			if( 'n' == *( p + 1 ) )	//D for ln()
				*q++ = 'D';
			else
				*q++ = 'E';	//E for lg()

			p += 2;
			break;

		case 't':	//F for tan()
			*q++ = 'F';
			p += 3;
			break;

		case '<':	//F for <<
			*q++ = '<';
			p += 2;
			break;

		case '>':	//F for >>
			*q++ = '>';
			p += 2;
			break;

		default:
			*q++ = *p++;
			break;
		}
	}

	*q = '\0';
	strcpy( pSrc, Buffer );

}

//根据运算符的优先级格式化公式
void FormatFormula( char* pSrc, P_OPERATE pOperate )
{
	char *p = pSrc, y;
	P_OPERATE lpOptr = pOperate;
	char stack[MAX_SIZE];
	int top = -1;
	double Operand;
	int offset = 0;
	stack[++top] = '#';

	while( '\0' != *p )
	{
		if( IsDigit( *p, FALSE ) )
		{
			GetOperand( p, &offset, &Operand );
			p += offset;
			lpOptr->Operand = Operand;
			lpOptr->Operator = 0;
			lpOptr++;
		}
		else
		{
			if( 'x' == *p )
			{
				( lpOptr++ )->Operator = 'x';
				p++;
			}
			else
			{
				if( 'p' == *p )
				{
					lpOptr->Operand = 3.14159266;
					p += 2;
					lpOptr->Operator = 0;
					lpOptr++;
				}
				else
				{
					if( ')' == *p )
					{
						for( y = stack[top--]; y != '('; y = stack[top--] )
							( lpOptr++ )->Operator = y;

						p++;
					}
					else
					{
						for( y = stack[top--]; GetPushStackPriority( y ) > GetPopStackPriority( *p ); y = stack[top--] )
							( lpOptr++ )->Operator = y;

						stack[++top] = y;
						stack[++top] = *p++;
					}
				}
			}
		}
	}

	while( top != -1 )
		( lpOptr++ )->Operator = stack[top--];

}

int GetOperand( char* pSrc, int * nLen, double *pOperand )
{
	char* p = pSrc, ch = *pSrc++;
	double z = 0, x = 0;
	int bits = 0;
	int point = FALSE;
	char cHex[20] = {'\0'};
	int bHex = FALSE;
	int i = 2;

	while( IsDigit( ch, bHex ) == TRUE )
	{
		if( ch == '.' )
		{
			if( point == TRUE )
				return FALSE;

			point = TRUE;
		}
		else if( ch == '0' && ( *pSrc == 'X' || *pSrc == 'x' ) )
		{
			cHex[0] = '0';
			cHex[1] = 'X';
			pSrc++;
			bHex = TRUE;
		}
		else
		{
			if( point == TRUE )
			{
				x *= 10;
				x += ch - '0';
				bits++;
			}
			else if( bHex == TRUE )
			{
				cHex[i] = ch;
				i++;
			}
			else
			{
				z *= 10;
				z += ch - '0';
			}
		}

		ch = *pSrc++;
	}

	while( bits-- > 0 )
	{
		x /= 10;
	}

	if( bHex )
	{
		z = ( int )strtol( cHex, NULL, 16 );
	}
	else
	{
		z += x;
	}

	*pOperand = z;
	*nLen = ( int )( pSrc - p - 1 );

	return TRUE;
}


int Locate( char ch )
{
	int i = 0;

	for( i = 0; Operator[i] != '\0'; i++ )
	{
		if( Operator[i] == ch )
			return i;
	}

	return -1;
}

int GetPushStackPriority( char ch )
{
	if( 'A' <=  ch && 'Z' >= ch )
		return 19;
	else
		return InPriority[Locate( ch )];
}

int GetPopStackPriority( char ch )
{
	if( 'A' <=  ch && 'Z' >= ch )
		return 18;
	else
		return OutPriority[Locate( ch )];
}

double CalculateValue( P_OPERATE pOperator, double x )
{
	double stack[MAX_SIZE], y1, y2;
	int top = -1;
	P_OPERATE lpOptr = pOperator;
	stack[++top] = 0;

	while( lpOptr->Operator != '#' )
	{
		if( !lpOptr->Operator )
		{
			stack[++top] = ( lpOptr++ )->Operand;
		}
		else
		{
			if( 'x' == lpOptr->Operator )
			{
				stack[++top] = x;
				lpOptr++;
			}
			else
			{
				switch( ( lpOptr++ )->Operator )
				{
				case '+':
					y1 = stack[top--];
					y2 = stack[top--];
					stack[++top] = y1 + y2;
					break;

				case '-':
					y1 = stack[top--];
					y2 = stack[top--];
					stack[++top] = y2 - y1;
					break;

				case '*':
					y1 = stack[top--];
					y2 = stack[top--];
					stack[++top] = y2 * y1;
					break;

				case '/':
					y1 = stack[top--];
					y2 = stack[top--];
					stack[++top] = y2 / y1;
					break;

				case '<':
					y1 = stack[top--];
					y2 = stack[top--];
					stack[++top] = ( int )y2 << ( int )y1;
					break;

				case '>':
					y1 = stack[top--];
					y2 = stack[top--];
					stack[++top] = ( int )y2 >> ( int )y1;
					break;

				case '&':
					y1 = stack[top--];
					y2 = stack[top--];
					stack[++top] = ( int )y2 & ( int )y1;
					break;

				case '|':
					y1 = stack[top--];
					y2 = stack[top--];
					stack[++top] = ( int )y2 | ( int )y1;
					break;

				case '^':
					y1 = stack[top--];
					y2 = stack[top--];
					stack[++top] = ( int )y2 ^ ( int )y1;
					break;

				case 'A':
					y1 = stack[top--];
					stack[++top] = sin( y1 );
					break;

				case 'B':
					y1 = stack[top--];
					stack[++top] = cos( y1 );
					break;

				case 'C':
					y1 = stack[top--];
					stack[++top] = exp( y1 );
					break;

				case 'D':
					y1 = stack[top--];
					stack[++top] = log( y1 );
					break;

				case 'E':
					y1 = stack[top--];
					stack[++top] = log10( y1 );
					break;

				case 'F':
					y1 = stack[top--];
					stack[++top] = tan( y1 );
					break;

				default:
					break;
				}
			}
		}
	}

	return stack[top];
}

void ParseFormulaAndSwitch( const char* pstrFormula, int nLen, char* pOutStrFormula, char* pOutSwitch )
{
	int i = 0;
	int nFlag = 0;

	for( i = 0; i < nLen; i++ )
	{
		if( *( pstrFormula + i ) == ':' )
		{
			nFlag = 1;
		}
		else if( nFlag == 0 )
		{
			*( pOutStrFormula + i ) = *( pstrFormula + i );
		}
		else if( nFlag == 1 )
		{
			*( pOutSwitch ) = *( pstrFormula + i );
			pOutSwitch++;
		}
	}
}

void SwitchFunction( int nValue, char* pSwitchFunction, int nLen, char* pOutValue )
{
	int i = 0;
	char szKey[256] = {'\0'};
	char szValue[256] = {'\0'};
	char* p = szKey, *q = szValue;
	int nFlag = 0;
	long nTemp = 0;
	char cCompareFlag = 0;

	for( i = 0; i < nLen; i++ )
	{
		if( *( pSwitchFunction + i ) == '<' )
		{
			if (*( pSwitchFunction + i + 1 ) == '<')
			{
				cCompareFlag = 1;
				i++;
			}
			else if(*( pSwitchFunction + i + 1 ) == '>')
			{
				cCompareFlag = 2;
				i++;
			}
			memset( szKey, '\0', 256 );
			memset( szValue, '\0', 256 );
			p = szKey;
			q = szValue;
			nFlag = 1;
		}
		else if( *( pSwitchFunction + i ) == '>' )
		{
			//判断是否为默认值
			if( strlen( szValue ) == 0 )
			{
				memcpy( pOutValue, szKey, strlen( szKey ) );

				return;
			}

			//一个表达式结束
			if( szKey[0] == '0' && ( szKey[1] == 'x' || szKey[1] == 'X' ) )
			{
				nTemp = strtol( szKey, NULL, 16 );
			}
			else
				nTemp = strtol( szKey, NULL, 10 );

			switch ( cCompareFlag )
			{
				case 0:
					if(nTemp == nValue)
					{
						memcpy( pOutValue, szValue, strlen( szValue ) );
						return;
					}
					break;
				case 1:
					if(nValue < nTemp)
					{
						memcpy( pOutValue, szValue, strlen( szValue ) );
						return;
					}
					break;
				case 2:
					if(nValue > nTemp)
					{
						memcpy( pOutValue, szValue, strlen( szValue ) );
						return;
					}
					break;
			}
			cCompareFlag = 0;
			nFlag = 0;
		}
		else if( *( pSwitchFunction + i ) == ',' )
		{
			nFlag = 2;
		}
		else if( *( pSwitchFunction + i ) == ' ' )
		{
		}
		else if( nFlag == 1 )
		{
			*p++ = *( pSwitchFunction + i );
			//*(szKey + i) == *(pSwitchFunction + i);
		}
		else if( nFlag == 2 )
		{
			*q++ = *( pSwitchFunction + i );
			//*(szValue + i) = *(pSwitchFunction + i);
		}
	}
}

void PieceSwitchFunction(unsigned int nValue, char* pSwitchFunction, int nLen, char* pOutValue )
{
	int i = 0;
	char szKey[20] = {'\0'};
	char szValue[256] = {'\0'};
	char* p = szKey, *q = szValue;
	int nFlag = 0;
	unsigned int nTemp = 0;

	for( i = 0; i < nLen; i++ )
	{
		if( *( pSwitchFunction + i ) == '<' )
		{
			memset( szKey, '\0', 20 );
			memset( szValue, '\0', 256 );
			p = szKey;
			q = szValue;
			nFlag = 1;
		}
		else if( *( pSwitchFunction + i ) == '>' )
		{
			//判断是否为默认值
			if( strlen( szValue ) == 0 )
			{
				memcpy( pOutValue, szKey, strlen( szKey ) );

				return;
			}

			//一个表达式结束
			if( szKey[0] == '0' && ( szKey[1] == 'x' || szKey[1] == 'X' ) )
			{
				nTemp = strtoul( szKey, NULL, 16 );
			}
			else
				nTemp = strtoul( szKey, NULL, 10 );

			if(nValue <= nTemp)
			{
				memcpy( pOutValue, szValue, strlen( szValue ) );
				return;
			}
			nFlag = 0;
		}
		else if( *( pSwitchFunction + i ) == ',' )
		{
			nFlag = 2;
		}
		else if( *( pSwitchFunction + i ) == ' ' )
		{
		}
		else if( nFlag == 1 )
		{
			*p++ = *( pSwitchFunction + i );
			//*(szKey + i) == *(pSwitchFunction + i);
		}
		else if( nFlag == 2 )
		{
			*q++ = *( pSwitchFunction + i );
			//*(szValue + i) = *(pSwitchFunction + i);
		}
	}
}

int GetFormatType( char *pStrFormat )
{
	int nLen = 0;

	if( pStrFormat == NULL )
		return -1;

	nLen = ( int )strlen( pStrFormat );	

	switch( nLen )
	{
	case 3:
		if( strcmp( pStrFormat, "HEX" ) == 0 )
		{
			return 1;
		}

	case 5:
		if( strcmp( pStrFormat, "ASCII" ) == 0 )
		{
			return 0;
		}

	case 6:
		if( strcmp( pStrFormat, "STRING" ) == 0 )
		{
			return 5;
		}

	case 7:
		if( strcmp( pStrFormat, "DECIMAL" ) == 0 )
		{
			return 2;
		}

	default:
		if( *( pStrFormat + ( nLen - 1 ) ) == 'f' || *( pStrFormat + ( nLen - 1 ) ) == 'F' )
		{
			return 4;
		}
		else if (*( pStrFormat + ( nLen - 1 ) ) == 'u' || *( pStrFormat + ( nLen - 1 ) ) == 'U' )
		{
			return 6;
		}
		else
		{
			return 3;
		}	
	}

}