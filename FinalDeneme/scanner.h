#ifndef scanner_h
#define scanner_h
#include "common.h"




/*------------------------------------------------------*/
/*   Token codes										*/
/*------------------------------------------------------*/

typedef enum
{
	NO_TOKEN, IDENTIFIER, NUMBER, STRING, UPARROW, STAR, LPAREN, RPAREN, MINUS, PLUS, EQUAL, LBRACKED, RBRACKED, COLON, SEMICOLON, LT, GT, COMMA,
	PERIOD, SLASH, COLONEQUAL, LE, GE, NE, DOTDOT, END_OF_FILE, ERROR, AND, ARRAY, BEGIN, CASE, CONST, DIV, DO, DOWNTO, ELSE, END, FFILE, FOR, FUNCTION,
	GOTO, IF, IN, LABEL, MOD, NIL, NOT, OF, OR, PACKED, PROCEDURE, PROGRAM, RECORD, REPEAT, SET, THEN, TO, TYPE, UNTIL, VAR, WHILE, WITH,
} TOKEN_CODE;




/*------------------------------------------------------*/
/*   Literal structure									*/
/*------------------------------------------------------*/
typedef enum
{
	INTEGER_LIT, REAL_LIT, STRING_LIT,
}LITERAL_TYPE;


typedef struct
{
	LITERAL_TYPE type;
	union
	{
		int integer;
		float real;
		char string[MAX_SOURCE_LINE_LENGTH];
	}value;
}LITERAL;


#endif // scanner_h

