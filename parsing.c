#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

#ifdef _WIN32
#include <string.h>

static char buffer [2048];

char* readline(char* prompt) {
	fputs(prompt, stdout);
	fgets(buffer, 2048, stdin);
	char* cpy = malloc(strlen(buffer)+1);
	strcpy(cpy, buffer);
	cpy[strlen(cpy)-1] = '\0';
	return cpy;
}

void add_history(char* unused) {}

#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

int main(int argc, char** argv) {
	mpc_parser_t* Number		= mpc_new("number");
	mpc_parser_t* Addition		= mpc_new("addition");
	mpc_parser_t* Substraction	= mpc_new("substraction");
	mpc_parser_t* Multiply		= mpc_new("multiply");
	mpc_parser_t* Division		= mpc_new("division");
	mpc_parser_t* Modulo		= mpc_new("modulo");
	mpc_parser_t* Operator 		= mpc_new("operator");
	mpc_parser_t* Expr			= mpc_new("expr");
	mpc_parser_t* Lispy			= mpc_new("lispy");
	
	
	/*
		allaorb		: /(a|b)+/ ;															\
		consecutive	: /b?(ab)+a?/ ;															\
		pitpot		: /^([^s])?p([i|o])t([a-zA-Z]+)?/										\
	*/
	
	mpca_lang(MPCA_LANG_DEFAULT,
		"																							\
			number			: /-?[0-9]+(.[0-9]+)?/ ;												\
			addition		: '+' | /add/ ;															\
			substraction	: '-' | /sub/ ;															\
			multiply		: '*' | /mul/ ;															\
			division		: '/' | /div/ ;															\
			modulo			: '%' | /mod/ ;															\
			operator		: <addition> | <substraction> | <multiply> | <division> | <modulo> ;	\
			expr			: <number> | '(' <expr> <operator> <expr> ')' ;								\
			lispy			: /^/ <expr> <operator> <expr> (<operator> <expr>+)? /$/ ;											\
		",
		Number, Addition, Substraction, Multiply, Division, Modulo, Operator, Expr, Lispy);
	
	puts("Lispy Version 0.0.0.0.2");
	puts("Press Ctrl+c to Exit\n");
	
	while(1) {
		char* input = readline("lispy> ");
		add_history(input);
		
		mpc_result_t r;
		if(mpc_parse("<stdin>", input, Lispy, &r)) {
			mpc_ast_print(r.output);
			mpc_ast_delete(r.output);
		} else {
			mpc_err_print(r.error);
			mpc_err_delete(r.error);
		}
		free(input);
	}
	
	mpc_cleanup(4, Number, Operator, Expr, Lispy);
	return 0;
}