#define MAX(x, y) ((x) >= (y) ? (x) : (y))

// All the preprocessery-hacky stuff I don't want to look at
#define buf__hdr(b) ((BufHdr *)((char *)b - offsetof(BufHdr, buf)))
#define buf__fits(b, n) (buf_len(b) + (n) <= buf_cap(b))
#define buf__fit(b, n) (buf__fits(b, n) ? 0 : ((b) = buf__grow((b), buf_len(b) + (n), sizeof(*(b)))))

#define buf_len(b) ((b) ? buf__hdr(b)->len : 0)
#define buf_cap(b) ((b) ? buf__hdr(b)->cap : 0)
#define buf_push(b, x) (buf__fit((b), 1), (b)[buf__hdr(b)->len++] = (x))
#define buf_free(b) ((b) ? (free(buf__hdr(b)), (b) = NULL) : 0)

typedef struct BufHdr {
	size_t len;
	size_t cap;
	char buf[];
} BufHdr;

void *buf__grow(const void *buf, size_t new_len, size_t elem_size) {
	size_t new_cap = MAX(1 + 2 * buf_cap(buf), new_len);
	size_t new_size = offsetof(BufHdr, buf) + new_cap*elem_size;
	BufHdr *new_hdr;
	if (buf) {
		new_hdr = realloc(buf__hdr(buf), new_size);
	}
	else {
		new_hdr = malloc(new_size);
		new_hdr->len = 0;
	}
	new_hdr->cap = new_cap;
	return new_hdr->buf;
}

void stripcomments(char *s) {
    return;			//TODO
}

void trimTrailing(char* str){
    int index, i;
    index = -1;
    i = 0;
    while(str[i] != '\0'){
        if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n'){
            index= i;
        }
	i++;
    }
	str[index + 1] = '\0';
}

void stripnewlines(char* s){
	for(int i = 0; *(s+i)!=0 ;i++){
		if(*(s+i)== 13 || *(s+i) == 10)
			*(s+i) = 32;
	}
}

int isValidLabel(char* s){
	for(int i = 0; i < strlen(s); i++)
		if(!(isalnum(*(s+i)) || *(s+i) == '-'))
			return 0;
	return 1;
}