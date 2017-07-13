#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include "common.h"
#include "scanner.h"
#include <string.h>
#define MAX_OUTPUT_RECORD_LENGHT 80


/*------------------------------------------------------*/
/*   Token Classes										*/
/*------------------------------------------------------*/
typedef enum {
	DELIMITER, NONDELIMITER,
}TOKEN_CLASS;


/*------------------------------------------------------*/
/*   Externals											*/
/*------------------------------------------------------*/
extern TOKEN_CODE token;
extern char token_string[];
extern BOOLEAN print_flag;

/*------------------------------------------------------*/
/*   Globals											*/
/*------------------------------------------------------*/
int record_lenght;
char *recp;

char output_record[MAX_OUTPUT_RECORD_LENGHT];


/*------------------------------------------------------*/
/*   Main												*/
/*------------------------------------------------------*/


main(argc, argv)
int argc;
char *argv[]; {
	TOKEN_CLASS class_;
	TOKEN_CLASS prev_class;
	TOKEN_CLASS token_class();

	print_flag = FALSE;

	init_scanner(argv[1]);


	prev_class = DELIMITER;
	recp = output_record;
	*recp = '\0';
	record_lenght = 0;
	do
	{
		get_token();
		if (token == END_OF_FILE) break;
		class_ = token_class();


		if ((prev_class == NONDELIMITER) && (class_ == NONDELIMITER))
			append_blank();
		append_token();
		prev_class = class_;


	} while (token != PERIOD);

	if (record_lenght > 0) flush_output_record();

	quit_scanner();
}



/*------------------------------------------------------*/
/*   token_class										*/
/*------------------------------------------------------*/


TOKEN_CLASS token_class() {

	switch (token)
	{

	case IDENTIFIER:
	case NUMBER:
		return (NONDELIMITER);

	default:
		return (token < AND ? DELIMITER : NONDELIMITER);
		//return (token < AND!DELIMITER:NONDELIMITER);
	}

}

/*------------------------------------------------------*/
/*   append_blank										*/
/*------------------------------------------------------*/
append_blank() {

	if (++record_lenght == MAX_OUTPUT_RECORD_LENGHT - 1)
		flush_output_record();
	else strcat(output_record, " ");


}


/*------------------------------------------------------*/
/*   append_token										*/
/*------------------------------------------------------*/

append_token() {


	int token_lenght;
	token_lenght = strlen(token_string);


	if (record_lenght + token_lenght >= MAX_OUTPUT_RECORD_LENGHT - 1)
		flush_output_record();

	strcat(output_record, token_string);
	record_lenght += token_lenght;

}


/*------------------------------------------------------*/
/*   flush_output_record								*/
/*------------------------------------------------------*/

flush_output_record() {

	printf("%s\n", output_record);
	recp = output_record;
	*recp = '\0';
	record_lenght = 0;

}