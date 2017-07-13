#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <sys\types.h>
#include <sys\timeb.h>
#include "common.h"
#include "error.h"
#include "scanner.h"



#define EOF_CHAR		'\x7f'
#define TAB_SIZE		8
#define MAX_INTEGER		32767
#define MAX_DIGIT_COUNT 20
#define MAX_EXPONENT	37

#define MIN_RESERVED_WORD_LENGTH 2
#define MAX_RESERVED_WORD_LENGTH 9


/*------------------------------------------------------*/
/*   Character codes									*/
/*------------------------------------------------------*/

typedef enum {
	LETTER, DIGIT, QUOTE, SPECIAL, EOF_CODE,
}CHAR_CODE;


/*------------------------------------------------------*/
/*   Reserved word tables								*/
/*------------------------------------------------------*/


typedef struct
{
	char *string;
	TOKEN_CODE token_code;
}RW_STRUCT;

RW_STRUCT rw_2[] = { { "do",DO },{ "if",IF },{ "in",IN },{ "of",OF },{ "or",OR },{ "to",TO },{ NULL,0 } };
RW_STRUCT rw_3[] = { { "and",AND },{ "div",DIV },{ "end",END },{ "for",FOR },{ "mod",MOD },{ "nil",NIL },{ "not",NOT },{ "set",SET },{ "var",VAR },{ NULL,0 } };
RW_STRUCT rw_4[] = { { "case",CASE },{ "else",ELSE },{ "file",FFILE },{ "goto",GOTO },{ "then",THEN },{ "type",TYPE },{ "with",WITH },{ NULL,0 } };
RW_STRUCT rw_5[] = { { "array",ARRAY },{ "begin",BEGIN },{ "const",CONST },{ "label",LABEL },{ "until",UNTIL },{ "while",WHILE },{ NULL,0 } };
RW_STRUCT rw_6[] = { { "downto",DOWNTO },{ "packed",PACKED },{ "record",RECORD },{ "repeat",REPEAT },{ NULL,0 } };
RW_STRUCT rw_7[] = { { "program",PROGRAM } ,{ NULL,0 } };
RW_STRUCT rw_8[] = { { "function",FUNCTION } ,{ NULL,0 } };
RW_STRUCT rw_9[] = { { "procedure",PROCEDURE },{ NULL,0 } };
//baþtaki nullar 0. ve 1. olmadýðý için tarama esnasýnda dizi olarak tarayacaz.
RW_STRUCT *rw_table[] = { NULL,NULL,rw_2,rw_3,rw_4,rw_5,rw_6,rw_7,rw_8,rw_9, };


/*------------------------------------------------------*/
/*   Globals											*/
/*------------------------------------------------------*/
char ch;
TOKEN_CODE token;
LITERAL literal;
int buffer_offset;
int level = 0;
int line_number = 0;
BOOLEAN print_flag = TRUE;

char source_buffer[MAX_SOURCE_LINE_LENGTH];
char token_string[MAX_TOKEN_STRING_LENGTH];
char word_string[MAX_TOKEN_STRING_LENGTH];
char *bufferp = source_buffer;
char *tokenp = token_string;


int digit_count;
BOOLEAN count_error;

int page_number = 0;
int line_count = MAX_LINES_PER_PAGE;


char source_name[MAX_FILE_NAME_LENGTH];
char date[DATE_STRING_LENGTH];

FILE *source_file;
CHAR_CODE char_table[256];


/*------------------------------------------------------*/
/*   Char Code				Return the character of ch. */
/*------------------------------------------------------*/

#define char_code(ch)	char_table[ch]


/*------------------------------------------------------*/
/*  init_scanner										*/
/*  Initialize the scanner globals and open source file.*/
/*------------------------------------------------------*/


init_scanner(name)
char *name; /*name of source file*/
{

	int ch;

	//initialize character table
	for (ch = 0; ch < 256; ++ch)char_table[ch] = SPECIAL;
	for (ch = '0'; ch <= '9'; ++ch)char_table[ch] = DIGIT;
	for (ch = 'A'; ch <= 'Z'; ++ch)char_table[ch] = LETTER;
	for (ch = 'a'; ch <= 'z'; ++ch)char_table[ch] = LETTER;
	char_table['\''] = QUOTE;
	char_table[EOF_CHAR] = EOF_CODE;

	init_page_header(name);
	open_source_file(name);

}


/*------------------------------------------------------*/
/*  quit_scanner										*/
/*  Terminate the scanner								*/
/*------------------------------------------------------*/

quit_scanner() {
	close_source_file();
}


/*------------------------------------------------------*/
/*  get_char											*/
/*  Set ch to the next character from the source buffer */
/*------------------------------------------------------*/


get_char() {

	BOOLEAN get_source_line();

	if (*bufferp == '\0')
	{
		if (!get_source_line())
		{
			ch = EOF_CHAR;
			return;
		}
		bufferp = source_buffer;
		buffer_offset = 0;
	}
	ch = *bufferp++; // next character in the buffer

	switch (ch)
	{
	case '\t': buffer_offset += TAB_SIZE - buffer_offset%TAB_SIZE;
		ch = ' ';
		break;
	case '\n': ++buffer_offset;
		ch = ' ';
		break;
	case '{':++buffer_offset;
		skip_comment();
		ch = ' ';
		break;
	default: ++buffer_offset; break;

	}
}



/*------------------------------------------------------*/
/*   skip_comment   Skip over a comment. Set ch to '}'  */
/*------------------------------------------------------*/

skip_comment() {

	do
	{
		get_char();
	} while ((ch != '}') && (ch != EOF_CHAR));


}

/*------------------------------------------------------*/
/*  skip_blanks (Set ch to the next nonblank character.)*/
/*------------------------------------------------------*/

skip_blanks() {
	while (ch == ' ') get_char();
}


/*------------------------------------------------------*/
/* get_token											*/
/* Extract the next token from the source buffer		*/
/*------------------------------------------------------*/

get_token() {
	skip_blanks();
	tokenp = token_string;



	switch (char_code(ch))
	{
	case LETTER: get_word();			break;
	case DIGIT:  get_number();			break;
	case QUOTE: get_string();			break;
	case EOF_CODE: token = END_OF_FILE; break;
	default: get_special();				break;

	}

}



/*------------------------------------------------------*/
/* get_word Extract a word token and downshift its		*/
/*			characters. Check if it's a reserved		*/
/*			word. Set token to IDENTIFIER  ift it's not.*/
/*------------------------------------------------------*/

get_word() {

	BOOLEAN is_reserved_word();

	/* Extract the word. */
	while ((char_code(ch) == LETTER) || (char_code(ch) == DIGIT))
	{
		*tokenp++ = ch;
		get_char();

	}
	*tokenp = '\0';
	downshift_word();


	if (!is_reserved_word())token = IDENTIFIER;

}


/*------------------------------------------------------*/
/* get_number	Extract a number token and set literal  */
/*				to its value. Set token to NUMBER.		*/
/*------------------------------------------------------*/

get_number() {

	int whole_count = 0; /*no. digits in whole part */
	int decimal_offset = 0;/*no. digits to move decimal */
	char exponent_sign = '+';
	int exponent = 0;
	float nvalue = 0.0;
	float evalue = 0.0;
	BOOLEAN saw_dotdot = FALSE;

	digit_count = 0;
	count_error = FALSE;
	token = NO_TOKEN;


	literal.type = INTEGER_LIT;

	accumulate_value(&nvalue, INVALID_NUMBER);
	if (token == ERROR)return;
	whole_count = digit_count;


	if (ch == '.')
	{
		get_char();
		if (ch == '.')
		{
			saw_dotdot = TRUE;
			--bufferp;

		}
		else
		{
			literal.type = REAL_LIT;
			*tokenp++ = '.';

			accumulate_value(&nvalue, INVALID_FRACTION);
			if (token == ERROR) return;
			decimal_offset = whole_count - digit_count;

		}
	}

	if (!saw_dotdot && ((ch == 'E') || (ch == 'e')))
	{
		literal.type = REAL_LIT;
		*tokenp++ = ch;
		get_char();


		if ((ch == '+') || (ch == '-'))
		{
			*tokenp++ = exponent_sign = ch;
			get_char();
		}

		accumulate_value(&evalue, INVALID_EXPONENT);
		if (token == ERROR)return;
		if (exponent_sign == '-')evalue = -evalue;



	}

	if (count_error)
	{

		error(TOO_MANY_DIGITS);
		token = ERROR;
		return;

	}

	exponent = evalue + decimal_offset;
	if ((exponent + whole_count < -MAX_EXPONENT) || (exponent + whole_count > MAX_EXPONENT))
	{
		error(REAL_OUT_OF_RANGE);
		token = ERROR;
		return;
	}

	if (exponent != 0)nvalue *= pow(10, exponent);

	if (literal.type == INTEGER_LIT)
	{
		if ((nvalue < -MAX_INTEGER) || (nvalue > MAX_INTEGER))
		{
			error(INTEGER_OUT_OF_RANGE);
			token = ERROR;
			return;
		}
		literal.value.integer = nvalue;
	}
	else literal.value.real = nvalue;

	*tokenp = '\0';
	token = NUMBER;


}




/*------------------------------------------------------*/
/* get_string	Extract a string token. Set token to	*/
/*				STRING. Note that the quotes are		*/
/*				stored as part of token string but not	*/
/*				literal.value.string.					*/
/*------------------------------------------------------*/
get_string() {
	char *sp = literal.value.string;

	*tokenp++ = '\'';
	get_char();

	//Extract the string.

	while (ch != EOF_CHAR)
	{
		if (ch == '\'')
		{
			*tokenp++ = ch;
			get_char();
			if (ch != '\'') break;


		}

		*tokenp++ = ch;
		*sp++ = ch;
		get_char();

	}

	*tokenp = '\0';
	*sp = '\0';
	token = STRING;
	literal.type = STRING_LIT;


}


/*------------------------------------------------------*/
/* get_special	Extract a special token. Most are		*/
/*				single-character. Some are double-		*/
/*				character. Set token appropriately.		*/
/*------------------------------------------------------*/
get_special() {

	*tokenp++ = ch;
	switch (ch) {
	case '^': token = UPARROW;		get_char(); break;
	case '*': token = STAR;			get_char(); break;
	case '(': token = LPAREN;		get_char(); break;
	case ')': token = RPAREN;		get_char(); break;
	case '-': token = MINUS;		get_char(); break;
	case '+': token = PLUS;			get_char(); break;
	case '=': token = EQUAL;		get_char(); break;
	case '[': token = LBRACKED;		get_char(); break;
	case ']': token = RBRACKED;		get_char(); break;
	case ';': token = SEMICOLON;	get_char(); break;
	case ',': token = COMMA;		get_char(); break;
	case '/': token = SLASH;		get_char(); break;
	case ':': get_char();			/* : or := */
		if (ch == '=')
		{
			*tokenp++ = '=';
			token = COLONEQUAL;
			get_char();
		}
		else token = COLON;
		break;
	case '<': get_char();
		if (ch == '=')
		{
			*tokenp++ = '=';
			token = LE;
			get_char();
		}
		else  if (ch == '>') {
			*tokenp++ = '>';
			token = NE;
			get_char();
		}
		else token = LT;
		break;

	case '>': get_char();
		if (ch == '=')
		{
			*tokenp++ = '=';
			token = GE;
			get_char();

		}
		else token = GT;
		break;

	case '.': get_char();		/* . or .. */
		if (ch == '.')
		{
			*tokenp++ = '.';
			token = DOTDOT;
			get_char();
		}
		else token = PERIOD;
		break;

	default:token = ERROR;
		get_char();
		break;


	}
	*tokenp = '\0';
}


/*------------------------------------------------------*/
/*downshift_word	Copy a word token into word_string  */
/*					with all letters downshifted.		*/
/*------------------------------------------------------*/

downshift_word() {
	int offset = 'a' - 'A';
	char *wp = word_string;
	char *tp = token_string;


	//Copy word into word_string
	do
	{
		*wp++ = (*tp >= 'A') && (*tp <= 'Z') ? *tp + offset : *tp;  // büyükse küçült deðilse sadece kopyala
		++tp;


	} while (*tp != '\0');

	*wp = '\0';

}

/*------------------------------------------------------*/
/*accumulate_value Extract a number part and accumulate	*/
/*				  its value. Flag the error if the first*/
/*				  character is not a digit.				*/
/*------------------------------------------------------*/


accumulate_value(valuep, error_code)
float *valuep;
ERROR_CODE error_code;
{
	float value = *valuep;

	//if first character is not a digit
	if (char_code(ch) != DIGIT) {
		error(error_code);
		token = ERROR;
		return;

	}

	do
	{
		*tokenp++ = ch;

		if (++digit_count <= MAX_DIGIT_COUNT)
		{
			value = 10 * value + (ch - '0');
		}
		else count_error = TRUE;

		get_char();

	} while (char_code(ch) == DIGIT);

	*valuep = value;
}


/*------------------------------------------------------*/
/*	is_reserved_word Check to see if a word token is	*/
/*					 a reserved word. If so, set token	*/
/*					 appropriately and return TRUE.Else,*/
/*					 return FALSE.						*/
/*------------------------------------------------------*/
BOOLEAN is_reserved_word() {
	int word_lenght = strlen(word_string);

	RW_STRUCT *rwp;

	//is it the right length?

	if ((word_lenght >= MIN_RESERVED_WORD_LENGTH) && (word_lenght <= MAX_RESERVED_WORD_LENGTH)) {

		for (rwp = rw_table[word_lenght]; rwp->string != NULL; ++rwp) // rwp lerin sonunda tanýmladýðýmýz NULL'lar sayesinde döngüyü durduruyoruz
		{
			if (strcmp(word_string, rwp->string) == 0)
			{
				token = rwp->token_code;
				return(TRUE); //yes , a reserved word
			}
		}


	}

	return(FALSE);
}


/*------------------------------------------------------*/
/* open_source_file	  Open the source file ad fetch its */
/*					  first character.					*/
/*------------------------------------------------------*/
open_source_file(name)
char *name; //filename
{
	if ((name == NULL) || ((source_file = fopen(name, "r")) == NULL))
	{
		error(FAILED_SOURCE_FILE_OPEN);
		exit(-FAILED_SOURCE_FILE_OPEN);
	}

	//Fetch the first character

	bufferp = "";
	get_char();
}

/*------------------------------------------------------*/
/*close_source_file		Close the source file			*/
/*------------------------------------------------------*/

close_source_file() {
	fclose(source_file);
}


/*------------------------------------------------------*/
/*get_source_line	Read the next line from the source	*/
/*					file. If there is one, print it out	*/
/*					and return TRUE. ELSE return FALSE	*/
/*					for the end of file.				*/
/*------------------------------------------------------*/

BOOLEAN get_source_line() {
	char print_buffer[MAX_SOURCE_LINE_LENGTH + 9];

	if ((fgets(source_buffer, MAX_SOURCE_LINE_LENGTH, source_file)) != NULL)
	{
		++line_number;
		if (print_flag)
		{
			sprintf(print_buffer, "%4d %d: %s", line_number, level, source_buffer);
			print_line(print_buffer);
		}
		return(TRUE);
	}
	else return(FALSE);

}

/*------------------------------------------------------*/
/*	print_line	  Print out a line. Start a new page if */
/*			      the current page is full				*/
/*------------------------------------------------------*/

print_line(line)
char line[]; //line to be printed
{
	char save_ch;
	char *save_chp = NULL;

	if (++line_count > MAX_LINES_PER_PAGE)
	{
		print_page_header();
		line_count = 1;

	}


	if (strlen(line) > MAX_PRINT_LINE_LENGTH)
	{
		save_chp = &line[MAX_PRINT_LINE_LENGTH];
		save_ch = *save_chp;
		*save_chp = '\0';
	}
	printf(line);
	if (save_chp) *save_chp = save_ch;

}

/*------------------------------------------------------*/
/* init_page_header	Initialize the fields of thr page	*/
/*					header.								*/
/*------------------------------------------------------*/
init_page_header(name)
char *name; // name of source file
{
	time_t timer;
	strncpy(source_name, name, MAX_FILE_NAME_LENGTH - 1);
	//set current date and time in the date string.
	time(&timer);
	strcpy(date, asctime(localtime(&timer)));

}

/*------------------------------------------------------*/
/* print_page_header	Print the page header at the top*/
/*						of the next page.				*/
/*------------------------------------------------------*/

print_page_header() {

	putchar(FORM_FEED_CHAR);
	printf("Page %d		%s		%s\n\n", ++page_number, source_name, date);

}
