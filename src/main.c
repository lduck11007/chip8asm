#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
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
	TOKEN_UNKNOWN,
	TOKEN_LOCATION,
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

void print_token(Token a){
	switch(a.tokenkind){
		case TOKEN_EOF:
			printf("TOKEN EOF\n");
			break;
		case TOKEN_LABEL:
			printf("TOKEN LABEL: %s\n", token.label);
			break;
		case TOKEN_DIRECTIVE:
			printf("TOKEN DIRECTIVE: %s\n", "dw"); // no other directives so far
			break;
		case TOKEN_LITERAL:
			printf("TOKEN LITERAL: %d\t0x%x\n", token.value, token.value);
			break;
		case TOKEN_MNEMONIC:
			printf ("TOKEN MNEMONIC: %d\n", token.mnemonic);
			break;
		case TOKEN_REGISTER:
			printf("TOKEN REGISTER: %d\t0x%x\n", token.reg, token.reg);
			break;
		case TOKEN_LOCATION:
			printf("TOKEN LOCATION: %s\n", token.label);
			break;
		case TOKEN_UNKNOWN:
			printf("UNKNOWN TOKEN: %s\n", token.label);
			break;
	}
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
	if((buf[strlen(buf)-1] == ':') && isValidLabel(buf)){
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
		token.value = strtol(str_intern_range(buf+2, end), NULL, 16);
		return;
	} else if((str_intern_range(buf, buf+2) == str_intern("0d"))||(str_intern_range(buf, buf+2) == str_intern("0D"))){
		token.tokenkind = TOKEN_LITERAL;
		token.value = strtol(str_intern_range(buf+2, end), NULL, 10);
		return;
	} else if((str_intern_range(buf, buf+2) == str_intern("0b"))||(str_intern_range(buf, buf+2) == str_intern("0B"))){
		token.tokenkind = TOKEN_LITERAL;
		token.value = strtol(str_intern_range(buf+2, end), NULL, 2);
		return;
	}
	if(buf == str_intern("CLS")){
		token.tokenkind = TOKEN_MNEMONIC;
		token.mnemonic = CLS;
		return;
	} else if(buf == str_intern("RET")){
		token.tokenkind = TOKEN_MNEMONIC;
   		token.mnemonic = RET;
   		return;
 	}
	else if(buf == str_intern("SYS")){
		token.tokenkind = TOKEN_MNEMONIC;
		token.mnemonic = SYS;
	return;
	}
	else if(buf == str_intern("JP")){
		token.tokenkind = TOKEN_MNEMONIC;
		token.mnemonic = JP;
	return;
	}
	else if(buf == str_intern("CALL")){
		token.tokenkind = TOKEN_MNEMONIC;
		token.mnemonic = CALL;
	return;
	}
	else if(buf == str_intern("SE")){
		token.tokenkind = TOKEN_MNEMONIC;
		token.mnemonic = SE;
	return;
	}
	else if(buf == str_intern("SNE")){
		token.tokenkind = TOKEN_MNEMONIC;
		token.mnemonic = SNE;
	return;
	}
	else if(buf == str_intern("LD")){
		token.tokenkind = TOKEN_MNEMONIC;
		token.mnemonic = LD;
	return;
	}
	else if(buf == str_intern("ADD")){
		token.tokenkind = TOKEN_MNEMONIC;
		token.mnemonic = ADD;
	return;
	}
	else if(buf == str_intern("OR")){
		token.tokenkind = TOKEN_MNEMONIC;
		token.mnemonic = OR;
	return;
	}
	else if(buf == str_intern("AND")){
		token.tokenkind = TOKEN_MNEMONIC;
		token.mnemonic = AND;
	return;
	}
	else if(buf == str_intern("XOR")){
		token.tokenkind = TOKEN_MNEMONIC;
		token.mnemonic = XOR;
	return;
	}
	else if(buf == str_intern("SUB")){
		token.tokenkind = TOKEN_MNEMONIC;
		token.mnemonic = SUB;
	return;
	}
	else if(buf == str_intern("SHR")){
		token.tokenkind = TOKEN_MNEMONIC;
		token.mnemonic = SHR;
	return;
	}
	else if(buf == str_intern("SUBN")){
		token.tokenkind = TOKEN_MNEMONIC;
		token.mnemonic = SUBN;
	return;
	}
	else if(buf == str_intern("SHL")){
		token.tokenkind = TOKEN_MNEMONIC;
		token.mnemonic = SHL;
	return;
	}
	else if(buf == str_intern("RND")){
		token.tokenkind = TOKEN_MNEMONIC;
		token.mnemonic = RND;
	return;
	}
	else if(buf == str_intern("DRW")){
		token.tokenkind = TOKEN_MNEMONIC;
		token.mnemonic = DRW;
	return;
	}
	else if(buf == str_intern("SKP")){
		token.tokenkind = TOKEN_MNEMONIC;
		token.mnemonic = SKP;
	return;
	}
	else if(buf == str_intern("SKNP")){
		token.tokenkind = TOKEN_MNEMONIC;
		token.mnemonic = SKNP;
	return;
	} else if(isValidLabel(buf)){
		token.tokenkind = TOKEN_LOCATION;
		token.label = buf;
	} else {
		token.tokenkind = TOKEN_UNKNOWN;
		token.label = buf;
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

void parse_test(){
	do{
	//token = emptyToken;
	next_token();
	print_token(token);
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
	trimTrailing(stream);
	parse_test();
	printf("%d\n", buf_len(tokens));
	for(int i = 0; i < buf_len(tokens); i++)
		print_token(tokens[i]);
}