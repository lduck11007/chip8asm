#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include "main.h"

enum TokenKind {
	TOKEN_EOF,
	TOKEN_LABEL,
	TOKEN_DIRECTIVE,
	TOKEN_LITERAL,
	TOKEN_MNEMONIC,
	TOKEN_REGISTER,
};

enum Mnemonic{
	CLS = 1,
	RET,
	SYS,
	JP,
	CALL,
	SE,
	SNE,
	LD,
	ADD,
	OR,
	AND,
	XOR,
	SUB,
	SHR,
	SUBN,
	SHL,
	RND,
	DRW,
	SKP,
	SKNP,
};

typedef struct Token{
	enum TokenKind tokenkind;
	union{
		uint8_t reg;
		uint16_t value;
		enum Mnemonic mnemonic;
		char label[17];
	};
} Token;

char* stream;
Token token;
Token* tokens;

void print_token(){
	printf("Token: ");
	switch(token.tokenkind){
		case TOKEN_EOF:
			printf("EOF\n");
			break;
		case TOKEN_LABEL:
			printf("LABEL\n");
			printf("label: %s\n", token.label);
			break;
		case TOKEN_DIRECTIVE:
			printf("DIRECTIVE\n");
			printf("directive: %s\n", "dw"); // no other directives so far
			break;
		case TOKEN_LITERAL:
			printf("LITERAL\n");
			printf("value: %x\n", token.value);
			break;
		case TOKEN_MNEMONIC:
			printf("MNEMONIC\n");
			printf ("mnemonic: %d\n", token.mnemonic);
			break;
		case TOKEN_REGISTER:
			printf("REGISTER\n");
			printf("Reg: %d\n", token.reg);
			break;
	}
}

void next_token(){
	char* start;
	char* end;
	char buf[17];
	if(*stream == 0){
		token.tokenkind = TOKEN_EOF;
		return;
	}
	for(;*stream == 32; stream++);
	start = stream;
	for(;!(*stream==32); stream++);
	end = stream;
	sprintf(buf, "%.*s", end-start, start);
	putchar(buf[end-start]);
	if(buf[(end-start)-1] == ':') {
		buf[(end-start)-1] = 0;
		token.tokenkind = TOKEN_LABEL;
		strcpy(token.label, buf);
	}
}

void stripcomments(char *s) {
    char *a, *b;
    int len = strlen(s) + 1;
 
    while ((a = strstr(s, "/*")) != NULL) {
	b = strstr(a+2, "*/");
	if (b == NULL)
	    break;
	b += 2;
	memmove(a, b, len-(b-a));
    }
}

void stripnewlines(char* s){
	for(int i = 0; !(*(s+i)==0) ;i++){
		if(*(s+i)== 13 || *(s+i) == 10)
			*(s+i) = 32;
	}
}

void fatal(const char *fmt, ...){
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	printf("\n");
	va_end(args);
	exit(1);
}

int main(int argc, char* argv[]){
    if(argc < 2)
        fatal("Usage: ./chip8asm <file.ch8>");
    FILE* f = fopen(argv[1], "rb");
    if(f == NULL){
        fatal("Cannot open file '%s'", argv[1]);}
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	stream = calloc(fsize + 1, 1);		
	fread(stream, fsize, 1, f);
	fclose(f);
	*(stream+fsize) = 0;
	tokens = NULL; 
	stripcomments(stream);
	stripnewlines(stream);
	next_token();
	print_token();
	print_token();

}