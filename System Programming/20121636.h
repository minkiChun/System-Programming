#include<string.h>
#include<stdlib.h>
#include<sys/stat.h>

typedef struct _node {
	int num;
	char histry[100];
	struct _node *link;
}NODE;

typedef struct _opcd {
	int num;
	char mnemonic[10];
	char data[10];
	struct _opcd *link;
}OPCD;

NODE *head;
OPCD *hash_head[20];

int history_count = 0;
int last_address = 0;
char memory[65536][16];

int commacheck(char *, int);
void printhelp(void);
int resetmem(char(*mem)[], int, int);
void makehistory(int, char*);
void printhistory(void);
void printerror(void);
int print_dump(int, int, int);
int fill_dump(int, int, int, int);
int edit_dump(int, int, int);
int hashing(char *);
int print_hash(void);
int search_mnemonic(char *);
void make_hash(void);
void push_hash(int, char*, char*);