#include<stdio.h>
#include"20121636.h"

int main(void)
{
	int i, j, commacount, argcount, arg[3] = { 0 };
	char input[4][25] = { 0 }, str[100], tstr[100], *tok;
	OPCD **hash_temp;
	head = (NODE*)malloc(sizeof(struct _node));
	head->link = NULL;
	hash_temp = (OPCD**)malloc(sizeof(struct _opcd*) * 20);
	for (i = 0; i<20; i++)
	{
		hash_temp[i] = (struct _opcd*)malloc(sizeof(struct _opcd));
		hash_temp[i]->link = NULL;
		hash_head[i] = hash_temp[i];
	}
	make_hash();

	while (1) {
		printf("sicsim> ");
		fgets(str, sizeof(str), stdin);
		str[strlen(str) - 1] = '\0';
		strcpy(tstr, str);
		tok = strtok(tstr, " \t\n");
		argcount = 1;
		while (1)
		{
			tok = strtok(NULL, " ,\t");
			if (!tok)break;
			argcount++;
		}
		strcpy(tstr, str);
		commacount = commacheck(tstr, argcount);
		if (commacount != 1)
		{
			printerror();
			continue;
		}
		sscanf(str, "%[^ ] %[^,],%[^,],%[^\n]", input[0], input[1], input[2], input[3]);
		for (i = 1; i<4; i++)
			sscanf(input[i], "%x", &arg[i - 1]);
		if (!strcmp("quit", input[0]) || !strcmp("q", input[0]))
			break;
		else if (!strcmp("help", input[0]) || !strcmp("h", input[0])) {
			printhelp();
			makehistory(history_count++, str);
		}
		else if (!strcmp("history", input[0]) || !strcmp("hi", input[0])) {
			makehistory(history_count++, str);
			printhistory();
		}
		else if (!strcmp("dump", input[0]) || !strcmp("du", input[0])) {
			if (argcount == 1) {
				arg[0] = last_address;
				arg[1] = arg[0] + 160;
			}
			else if (argcount == 2)
			{
				arg[1] = arg[0] + 160;
			}
			if (print_dump(arg[0], arg[1], argcount) == 1)
				makehistory(history_count++, str);
			else
				printerror();
		}
		else if (!strcmp("edit", input[0]) || !strcmp("e", input[0])) {
			if (edit_dump(arg[0], arg[1], argcount) == 1)
				makehistory(history_count++, str);
			else
				printerror();
		}
		else if (!strcmp("fill", input[0]) || !strcmp("f", input[0])) {
			if (fill_dump(arg[0], arg[1], arg[2], argcount) == 1)
				makehistory(history_count++, str);
			else
				printerror();
		}
		else if (!strcmp("reset", input[0]) || !strcmp("r", input[0])) {
			if (argcount == 1) {
				for (i = 0; i<65536; i++)
					for (j = 0; j<16; j++)
						memory[i][j] = 0;
				makehistory(history_count++, str);
			}
			else
				printerror();
		}
		else if (!strcmp("opcodelist", input[0])) {
			if (print_hash() == 1)
				makehistory(history_count++, str);
			else
				printerror();
		}
		else if (!strcmp("opcode", input[0])) {
			if (search_mnemonic(input[1]) == 1)
				makehistory(history_count++, str);
			else
				printerror();
		}
		else
			printerror();

		arg[0] = last_address;
		i = resetmem(input, 4, 25);
		memset(str, 0, sizeof(char) * 100);
		memset(tstr, 0, sizeof(char) * 100);
	}
}
void printhelp(void)
{
	printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start, end]\ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\n");
}
void makehistory(int count, char *str) {
	NODE *newNode = (NODE*)malloc(sizeof(struct _node));
	newNode->link = NULL;

	newNode->num = count;
	strcpy(newNode->histry, str);

	if (head->link == NULL) {
		head->link = newNode;
	}
	else {
		NODE *cur = head;
		while (cur->link != NULL)
			cur = cur->link;
		cur->link = newNode;
	}
}
int search_mnemonic(char *mnemonic)	//hash table에 찾고자 하는 mnemonic이 있는지 검사
{
	int i;
	OPCD *ptr;
	for (i = 0; i<20; i++)
	{
		ptr = hash_head[i];
		ptr = ptr->link;
		while (ptr != NULL)
		{
			if (!strcmp(mnemonic, ptr->mnemonic))
			{
				printf("opcode is %s\n", ptr->data);
				return 1;
			}
			else
				ptr = ptr->link;
		}
	}
	puts("No Mnemonic!");
	return 0;
}
int hashing(char *mnemonic)//hashing
{
	int i, sum = 0;

	for (i = 0; i<strlen(mnemonic); i++)
		sum += (int)(mnemonic[i] - 'A');

	return sum % 20;
}
//입력받은 값의 hashtable의 index값을 return
void make_hash() {
	FILE *fp;
	char mne[10], op[10], data[10];
	int n;

	fp = fopen("opcode.txt", "r");

	while (!feof(fp))
	{
		fscanf(fp, "%s %s %s", op, mne, data);
		n = hashing(mne);
		push_hash(n, op, mne);
	}

	fclose(fp);
}
void push_hash(int n, char *opcode, char *mnemonic) {
	OPCD *newNode = (OPCD*)malloc(sizeof(struct _opcd));
	newNode->link = NULL;

	strcpy(newNode->data, opcode);
	strcpy(newNode->mnemonic, mnemonic);

	if (hash_head[n]->link == NULL)
		hash_head[n]->link = newNode;
	else
	{
		OPCD *cur = hash_head[n];
		while (cur->link != NULL)
			cur = cur->link;
		cur->link = newNode;
	}
}
int print_hash(void) {
	int i, flag;
	OPCD *ptr;
	puts("");
	for (i = 0; i<20; i++) {
		flag = 0;
		ptr = hash_head[i];
		printf("%-2d : ", i);
		ptr = ptr->link;
		while (ptr != NULL)
		{
			flag = 1;
			if (ptr->link == NULL)
				printf("[%s,%s]\n", ptr->mnemonic, ptr->data);
			else
				printf("[%s,%s] -> ", ptr->mnemonic, ptr->data);
			ptr = ptr->link;
		}
		if (!flag)
			puts("");
	}
	puts("");
	return 1;
}
// 메모리의 arg1 번지의 값을 arg2 값으로 변경
int edit_dump(int arg1, int arg2, int argcount) {
	if (argcount != 3)
		return 0;
	else if (arg1 < 0 || arg1 > 0xFFFFF || arg2 <0 || arg2 > 0xFF)
		return 0;
	memory[arg1 / 16][arg1 % 16] = arg2;
	return 1;
}
// 메모리의 arg1~arg2 번지의 값을 arg2 값으로 변경
int fill_dump(int arg1, int arg2, int arg3, int argcount) {
	if (argcount != 4)
		return 0;
	else if (arg1 < 0 || arg1 > 0xFFFFF || arg2 < 0 || arg2 > 0xFFFFF || arg1 >arg2)
		return 0;
	else if (arg3 < 0 || arg3 > 0xFF)
		return 0;
	while (arg1 <= arg2)
	{
		memory[arg1 / 16][arg1 % 16] = arg3;
		arg1++;
	}
	return 1;
}
/* 기본적으로 10라인이 출력된다. (한 라인은 메모리의 16개 바이트로 구성)
dump의 실행으로 출력된 마지막 address는 내부에 저장하고 있다.
다시 dump를 실행시키면 마지막(address + 1) 번지부터 출력된다.
dump 명령어가 처음 시작될 때는 0 번지부터 출력된다
dump가 출력할 시 boundary check를 하여 exception error 처리한다.*/
int print_dump(int arg1, int arg2, int argcount) {
	int j, in_d, index;
	if (argcount == 2) {
		if (arg1>0xFFFFF || arg1 < 0)
			return 0;
	}
	else if (argcount == 3) {
		if (arg1>0xFFFFF || arg1 < 0)
			return 0;
		else if (arg2>0xFFFFF || arg2<0)
			return 0;
		else if (arg1>arg2)
			return 0;
		arg2++;
	}
	else if (argcount>3)
		return 0;
	in_d = (arg1 / 16) * 16;
	index = in_d;
	while (index<arg2)
	{
		//left
		printf("%05X ", in_d);
		//middle
		for (j = 0; j<16; j++)	//16개 출력
		{
			if (index >= arg1 && index<arg2)	printf("%02X ", (unsigned char)memory[index / 16][index % 16]);
			else if (index == 0xFFFFF)
				printf("%02X ", (unsigned char)memory[0xFFFF][0xF]);
			else printf("   ");	//아니면 공백출력
			index++;
		}
		printf("; ");
		//right
		for (j = 0; j<16; j++)
		{
			if (memory[in_d / 16][(in_d + j) % 16] >= 0x20 && memory[in_d / 16][(in_d + j) % 16] <= 0x7E)	printf("%c", (unsigned char)memory[in_d / 16][(in_d + j) % 16]);
			else printf(".");	// 0x7E<메모리 값<0x20이면 .으로 출력
		}
		puts("");
		in_d += 16;
	}
	last_address = arg2 % 0xFFFFF;
	return 1;
}
/*
현재까지 사용한 명령어들을 순서대로 번호와 함께 보여준다. 
가장 최근 사용한 명령어가 리스트의 하단에 오도록 한다.
*/
void printhistory(void) {
	NODE *temp = head;
	while (temp->link != NULL)
	{
		temp = temp->link;
		printf("%-5d %s\n", temp->num, temp->histry);
	}
}
int commacheck(char str[100], int argnum)
{
	int count = 0, i;
	char *tok;
	if (str[0] == ',' || str[strlen(str) - 1] == ',') //첫번째나 끝이 ,일때
		return 2;
	for (i = 1; i<strlen(str); i++) {
		if (str[i] == ',') {
			if (str[i - 1] == ',')				   // ,,일때
				return 2;
			else {
				count++;
			}
		}
	}
	if (argnum == 1)return 1;
	else if (argnum == 3)
	{
		tok = strtok(str, " \t");
		tok = strtok(NULL, " \t");
		if (tok[0] == ',')
			return 2;
	}
	else if (argnum == 4)
	{
		tok = strtok(str, " \t");
		tok = strtok(NULL, " ,\t");
		tok = strtok(NULL, " \t");
		if (tok[0] == ',')
			return 2;
	}
	if (argnum - count == 2)
		return 1;
	else
		return 2;
}
void printerror(void)
{
	puts("ERROR!!");
}
int resetmem(char(*mem)[25], int x, int y)
{
	int i;
	for (i = 0; i<x; i++)
		memset(mem[i], 0, sizeof(char)*y);
	return 1;
}
