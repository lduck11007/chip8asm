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
	TOKEN_UNKNOWN
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
		char* label;
	};
} Token;

typedef struct InternStr{
	size_t len;
	const char* str;
} InternStr;

char* stream;
Token token;
Token* tokens;
Token emptyToken;
InternStr* interns;

char *str_intern_range(const char *start, const char *end){
	size_t len = end - start;
	for(size_t i = 0; i < buf_len(interns); i++){
		if(interns[i].len == len && strncmp(interns[i].str, start, len) == 0){
			return interns[i].str;
		}
	}
	char *str = malloc(len + 1);
	memcpy(str, start, len);
	str[len] = 0; //null byte
	buf_push(interns, ((InternStr){len, str}));
	return str;
}

char *str_intern(const char *str) {		//only works with null-byte terminated strings
	return str_intern_range(str, str + strlen(str));
}

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
			printf("Reg: %d\t0x%x\n", token.reg, token.reg);
			break;
		case TOKEN_UNKNOWN:
			printf("UNKNOWN TOKEN\n");
			printf("%s\n", token.label);
			break;
	}
	printf("\n");
}

void next_token(){
	char* start;
	char* end;
	char* buf;
	if(*stream == 0){
		token.tokenkind = TOKEN_EOF;
		return;
	}
	for(;*stream == 32; stream++);
	start = stream;
	for(;(*stream!=32)&&(*stream!=0); stream++);
	end = stream;
	buf = str_intern_range(start, end);
	if(buf[strlen(buf)-1] == ':'){
		buf[strlen(buf)-1] = 0;
		token.tokenkind = TOKEN_LABEL;
		token.label = str_intern(buf);
		return;
	} else if(buf == str_intern("dw") || buf == str_intern("DW")){
		token.tokenkind = TOKEN_DIRECTIVE;
		return;
	} else if((*buf == 'v' || *buf == 'V') && isxdigit(*(buf+1))){
		token.tokenkind = TOKEN_REGISTER;
		if (*(buf+1) >= '0' && *(buf+1) <= '9')
			token.reg = *(buf+1) - '0';
		if (*(buf+1) >= 'A' && *(buf+1) <= 'F')
			token.reg = *(buf+1) - 'A' + 10;
		if (*(buf+1) >= 'a' && *(buf+1) <= 'f')
			token.reg = *(buf+1) - 'a' + 10;
		return;
	} else if((str_intern_range(buf, buf+2) == str_intern("0x"))||(str_intern_range(buf, buf+2) == str_intern("0X"))){
		token.tokenkind = TOKEN_LITERAL;
		token.value = strtol(str_intern_range(buf+2, buf+6), NULL, 16);
	} else {
		token.tokenkind = TOKEN_UNKNOWN;
		token.label = buf;
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
	int len;
	for(int i = 0; *(s+i)!=0 ;i++){
		if(*(s+i)== 13 || *(s+i) == 10)
			*(s+i) = 32;
	}
	for(len = strlen(s)-1; *(s+len)!= 32; len--);
	*(s+len) = 0;
}

void fatal(const char *fmt, ...){
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	printf("\n");
	va_end(args);
	exit(1);
}

void parse(){
	do{
	token = emptyToken;
	next_token();
	print_token();
	} while(token.tokenkind != TOKEN_EOF);
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

	for(int i = 0; *(stream+i) != 0; i++){
		printf("%c:\t%d\n", *(stream+i), *(stream+i));
	}
	parse();
}