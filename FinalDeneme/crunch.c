#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include "common.h"
#include "scanner.h"
#include "symtab.h"



/*------------------------------------------------------*/
/*   Externals											*/
/*------------------------------------------------------*/

extern TOKEN_CODE token;
extern char token_string[];
extern char word_string[];
extern BOOLEAN print_flag;

extern SYMTAB_NODE_PTR symtab_root;

/*------------------------------------------------------*/
/*   Globals											*/
/*------------------------------------------------------*/

short index = 0;
FILE *crunch_file;

/*------------------------------------------------------*/
/*   main												*/
/*------------------------------------------------------*/

//main(argc, argv)
//int argc;
//char *argv[]; {
//	print_flag = FALSE;
//
//	init_scanner(argv[1]);
//
//	do_pass_1();
//	close_source_file();
//	crunch_file = fopen(argv[2], "wb");
//	if (crunch_file == NULL)
//	{
//		fprintf(stderr, "*** ERROR : Failed to open crunch file.\n");
//		exit(-2);
//	}
//	fwrite(&index, sizeof(short), 1, crunch_file);
//	output_crunched_symtab(symtab_root);
//
//	open_source_file(argv[1]);
//	do_pass_2();
//
//	fclose(crunch_file);
//	quit_scanner();
//}

/*------------------------------------------------------*/
/*   do_pass_1											*/
/*------------------------------------------------------*/

do_pass_1() {

	SYMTAB_NODE_PTR np;

	do
	{
		get_token();
		if (token == END_OF_FILE) break;
		if (token == IDENTIFIER)
		{
			if ((np = search_symtab(word_string, symtab_root)) == NULL) {

				np = enter_symtab(word_string, &symtab_root);
				np->info = (char *)index++;
			}
		}

		if (token == NUMBER || token == STRING)
		{
			if ((np = search_symtab(token_string, symtab_root)) == NULL)
			{
				np = enter_symtab(token_string, &symtab_root);
				np->info = (char *)index++;
			}
		}


		/*
				switch (token){

					CASE IDENTIFIER:
					if ((np = search_symtab(word_string, symtab_root)) == NULL) {

						np = enter_symtab(word_string, &symtab_root);
						np->info = (char *)index++;
					}
					break;
					CASE NUMBER:
					CASE STRING:
					if ((np = search_symtab(token_string, symtab_root)) == NULL)
					{
						np = enter_symtab(token_string, &symtab_root);
						np->info = (char *)index++;
					}
					break;

				default:
					break;

				}*/


	} while (token != PERIOD);


}


/*------------------------------------------------------*/
/*   do_pass_2											*/
/*------------------------------------------------------*/


do_pass_2() {
	SYMTAB_NODE_PTR np;
	do
	{
		get_token();
		if (token == END_OF_FILE) break;

		output_crunched_token();


	} while (token != PERIOD);
}

/*------------------------------------------------------*/
/*   output_crunched_symtab								*/
/*------------------------------------------------------*/

output_crunched_symtab(np)
SYMTAB_NODE_PTR np;
{
	char lenght;
	if (np == NULL)return;

	output_crunched_symtab(np->left);
	lenght = strlen(np->name) + 1;
	index = (short)np->info;
	fwrite(&index, sizeof(short), 1, crunch_file);
	fwrite(&lenght, 1, 1, crunch_file);
	fwrite(np->name, lenght, 1, crunch_file);
	output_crunched_symtab(np->right);
}

/*------------------------------------------------------*/
/*   output_crunched_token								*/
/*------------------------------------------------------*/


output_crunched_token() {

	SYMTAB_NODE_PTR np;
	char  token_code = token;
	fwrite(&token_code, 1, 1, crunch_file);
	if (token == IDENTIFIER)
	{
		np = search_symtab(word_string, symtab_root);
		index = (short)np->info;
		fwrite(&index, sizeof(short), 1, crunch_file);
	}
	if (token == NUMBER || token == STRING)
	{
		np = search_symtab(token_string, symtab_root);
		index = (short)np->info;
		fwrite(&index, sizeof(short), 1, crunch_file);
	}

	/*switch (token)
	{
		CASE IDENTIFIER :
		np = search_symtab(word_string, symtab_root);
		index = (short)np->info;
		fwrite(&index, sizeof(short), 1, crunch_file);
		break;
		CASE NUMBER :
		CASE STRING :
		np = search_symtab(token_string, symtab_root);
		index = (short)np->info;
		fwrite(&index, sizeof(short), 1, crunch_file);
		break;
	default:
		break;
	}*/

}