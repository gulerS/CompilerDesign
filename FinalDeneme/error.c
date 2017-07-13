/***********************************
*
* error.c
*
**********************************/
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include "common.h"
#include "error.h"
#include "scanner.h"


extern char     *tokenp;
extern BOOLEAN  print_flag;
extern char     source_buffer[];
extern char     *bufferp;

int error_count = 0;



char *error_messages[] = {
	"NO ERROR",                             // 1
	"SYNTAX ERROR",                    // 2
	"TOO MANY SYNTAX ERRORS",                   // 3
	"FAIED SOURCE FILE OPEN",
	"UNEXPECTED END OF FILE",
	"INVALID NUMBER",
	"INVALID FRACTION",
	"INVALID EXPONENT",
	"TOO MANY DIGITS",
	"REAL LITERAL OUT OF RANGE",
	"INTEGER LITERAL OUT OF RANGE",
	"FALTA PARENTESIS IZQUIERDO",
	"FALTA PARENTESIS DERECHO",
	"EXPRESION INVALIDA",
	"ASIGNACION INVALIDA",
	"FALTA IDENTIFICADOR",
	"WEND INVALIDO",
	"FALTA WEND",
	"IDENTIFICADOR NO DEFINIDO",
	"OVERFLOW DE STACK",
	"SENTENCIA INVALIDA",
	"ELEMENTO INESPERADO",
	"FALTA PUNTO Y COMA",
	"GOSUB AL PGM. PRINCIPAL",
	"IFEND NO VALIDO",
	"FALTA IFEND",
	"ELSE NO VALIDO",
	"THEN NO VALIDO",
	"FOR-CONTROL INVALIDO",
	"FALTA CONSTANTE",
	"FALTA DOSPUNTOS",
	"FALTA END",
	"FALTA TO",
	"IDENTIFICADOR REDEFINIDO",
	"FALTA IGUAL",
	"VARIABLE DE NEXT INCORRECTA",
	"NEXT SIN FOR",
	"TIPO INVALIDO DE SUBRANGO",
	"FALTA CASE",
	"FALTA CEND",
	"CASE FUERA DE SITIO",
	"CEND FUERA DE SITIO",
	"OTHERWISE FUERA DE SITIO",
	"FALTA COMA",
	"MATRIZ DE TIPO DESCONOCIDO",
	"DESTINOS INVALIDOS",
	"VARIABLE INVALIDA",
	"ASIGNACION INCOMPATIBLE",
	"MINIMO MAYOR QUE MAXIMO",
	"MATRIZ SIN INDICE",
	"FALTA ]",
	"TIPO INVALIDO DE INDICE",
	"HAY SENTENCIAS QUE NO SE EJECUTAN A PARTIR DE AQUI",
	"FALTA PUNTO",
	"DEMASIADOS SUBSCRIPTS",
	"COMPARACION INVALIDA",
	"ANIDAMIENTO DEMASIADO PROFUNDO",
	"PROGRAMA INEXISTENTE",
	"EXP. STRING NO VALIDA",
	"EXP. DE COMA FLOTANTE NO VALIDA",
	"EXP. ENTERA NO VALIDA",
	"EXP. NUMERICA NO VALIDA",
	"NUMERO ERRONEO DE PARAMETROS",
	"VAR_PARM NO VALIDO",
	"TIPO DE ARG. CASE DISTINTO DE TIPO ARG. SELECT",
	"ESPERADO STRING",
	"ETIQUETA NO VALIDA, PROBAR CON OPCION '-l'",
	"FEATURE NO IMPLEMENTADA",
	"WHILE SIN CERRAR",
	"FOR SIN CERRAR",
	"IF-THEN-ELSE SIN CERRAR",
	"SELECT-CASE SIN CERRAR",
	"ESTRUCTURA DESCONOCIDA SIN CERRAR",
	"VALOR ERRONEO PARA OPCION CASE",
	"SOLO VALIDO AL COMIENZO DEL PROGRAMA",

};


void error(code)

ERROR_CODE code;
{
	extern int buffer_offset;
	char message_buffer[MAX_PRINT_LINE_LENGTH];
	char *message = error_messages[code];
	int offset = buffer_offset - 2;

	if (print_flag)offset + 8;
	sprintf(message_buffer, "%*s^\n", offset, " ");

	if (print_flag)
		print_line(message_buffer);
	else
		printf(message_buffer);

	sprintf(message_buffer, "*** HATA: %s.\n", message);
	if (print_flag) print_line(message_buffer);
	else printf(message_buffer);


	*tokenp = '\0';
	++error_count;

	if (error_count > MAX_SYNTAX_ERRORS) {
		sprintf(message_buffer, "Too many syntax errors. Aborted.\n");
		if (print_flag)
			print_line(message_buffer);
		else
			printf(message_buffer);

		exit(-TOO_MANY_SYNTAX_ERRORS);
	}


}
