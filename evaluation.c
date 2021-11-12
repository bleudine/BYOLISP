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

long eval_op(long x, char* op, long y) {
	if (strcmp(op, "+") == 0) { return x + y; }
	if (strcmp(op, "-") == 0) { return x - y; }
	if (strcmp(op, "*") == 0) { return x * y; }
	if (strcmp(op, "/") == 0) { return x / y; }
	if (strcmp(op, "%") == 0) { return x % y; }
	if (strcmp(op, "^") == 0) {
		long elevated = x;
		for (int i = 1; i < y; i++) {
			elevated *= x;
		}
		return elevated;
	}
	if (strcmp(op, "min") == 0) {
		if (x > y) {
			return y;
		}
		return x;
	}
	if (strcmp(op, "max") == 0) {
		if (x < y) {
			return y;
		}
		return x;
	}
	return 0;
}

long eval(mpc_ast_t* t) {
	if (strstr(t->tag, "number")) {
		return atoi(t->contents);
	}
	
	char* op = t->children[1]->contents;
	
	long x = eval(t->children[2]);
	
	int i = 3;
	while(strstr(t->children[i]->tag, "expr")) {
		x = eval_op(x, op, eval(t->children[i]));
		i++;
	}
	
	return x;
}

long count_branches(mpc_ast_t* t) {
	if (strstr(t->tag, "number")) {
		return 0;
	}
	long branches = 1;
	branches = branches + count_branches(t->children[2]);
	
	long i = 3;
	while(strstr(t->children[i]->tag, "expr")) {
		branches = branches + count_branches(t->children[i]);
		i++;
	}
	
	return branches;
}

long count_leafs(mpc_ast_t* t) {
	if (strstr(t->tag, "number")) {
		return 1;
	}
	long leafs = 1;
	leafs = leafs + count_leafs(t->children[2]);
	long i = 3;
	while(strstr(t->children[i]->tag, "expr")) {
		leafs = leafs + count_leafs(t->children[i]);
		i++;
	}
	
	return leafs;
}

long max_children(mpc_ast_t* t) {
	if (strstr(t->tag, "number") || strstr(t->tag, "operator")) {
		return 1;
	}
	
	long children = 3;
	children = max_children(t->children[2]);
	
	long i = 3;
	while(strstr(t->children[i]->tag, "expr")) {
		if (children < max_children(t->children[i])) {
			children = max_children(t->children[i]);
		}
		i++;
	}
	
	i--;
	
	if (children > i) {
		return children;
	} else {
		return i;
	}
}

int main(int argc, char** argv) {
	mpc_parser_t* Number		= mpc_new("number");
	mpc_parser_t* Operator 		= mpc_new("operator");
	mpc_parser_t* Expr			= mpc_new("expr");
	mpc_parser_t* Lispy			= mpc_new("lispy");
	
	mpca_lang(MPCA_LANG_DEFAULT,
		"																							\
			number			: /-?[0-9]+/ ;															\
			operator		: '+' | '-' | '*' | '/' | '%' | '^' | /min/ | /max/;					\
			expr			: <number> | '(' <operator> <expr>+ ')' ;								\
			lispy			: /^/ <operator> <expr>+ /$/ ;											\
		",
		Number, Operator, Expr, Lispy);
	
	puts("Lispy Version 0.0.0.0.2");
	puts("Press Ctrl+c to Exit\n");
	
	while(1) {
		char* input = readline("lispy> ");
		add_history(input);
		
		mpc_result_t r;
		if(mpc_parse("<stdin>", input, Lispy, &r)) {
			long result = eval(r.output);
			printf("result:%li\n", result);
			long branches = count_branches(r.output);
			printf("branches:%li\n", branches);
			long leafs = count_leafs(r.output);
			printf("leafs:%li\n", leafs);
			long children = max_children(r.output);
			printf("children;%li\n", children);
			
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