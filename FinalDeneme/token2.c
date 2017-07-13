#define _CRT_SECURE_NO_DEPRECATE


#include <stdio.h>
#include "common.h"
#include "error.h"
#include "scanner.h"

char *symbol_strings[] = {
	"<no_token>","<IDENTIFIER>","<NUMBER>","<STRING>"," ","*","(",")","-","+","=","{","}",":",";","<",">",",",".","/",":=","<=",">=","<>","..","<END OF FILE>",
	"<ERROR>","AND","ARRAY","BEGIN","CASE","CONST","DIV","DO","DOWNTO","ELSE","END","FILE","FOR","FUNCTION","GOTO","IF","IN","LABEL","MOD","NIL","NOT","OF","OR",
	"PACKED","PROCEDURE","PROGRAM","RECORD","REPEAT","SET","THEN","TO","TYPE","UNTIL","VAR","WHILE","WITH",

};

/*Externals*/

extern TOKEN_CODE token;
extern char token_string[];
extern LITERAL literal;

/*main*/

//main(argc, argv)
//int argc;
//char *argv[];
//{
//	init_scanner(argv[1]);
//	do
//	{
//		get_token();
//
//		if (token == END_OF_FILE)
//		{
//			error(UNEXPECTED_END_OF_FILE);
//			break;
//		}
//		print_token();
//	} while (token != PERIOD);
//	quit_scanner();
//
//}


/*Print Token*/

print_token() {

	char line[MAX_SOURCE_LINE_LENGTH + 32];
	char *symbol_string = symbol_strings[token];

	switch (token)
	{
	case NUMBER:
		if (literal.type == INTEGER_LIT)
		{
			sprintf(line, "		>> %-16s %d (integer)\n", symbol_string, literal.value.integer);
		}
		else sprintf(line, "		>> %-16s %g (real)\n", symbol_string, literal.value.real);
		break;
	case STRING:
		sprintf(line, "		>> %-16s %-s\n", symbol_string, literal.value.string);
		break;
	default:
		sprintf(line, "		>> %-16s %-s\n", symbol_string, token_string);
		break;
	}
	print_line(line);
}