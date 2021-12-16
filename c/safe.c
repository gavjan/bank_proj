#define _GNU_SOURCE
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>
#include <time.h>
#include <ctype.h>
#include <regex.h>
#include "list.h"
#include "safe.h"

time_t date_to_seconds(char* date) {
	struct tm tm = { 0 };
	time_t t;
	strptime(date, "%d.%m.%Y", &tm);
	if (strptime(date, "%d.%m.%Y", &tm) == NULL)
			return WRONG_DATE;

	t = mktime(&tm);

	return t;
}
bool isdate(char* date) {
	if(strlen(date) != DATE_LEN) return false;

	for(int i = 0; i < DATE_LEN; i++) {
		if(i == DATE_DELIMITER_1 || i == DATE_DELIMITER_2) {
			if(date[i] != '.')
				return false;
		} else if(!isdigit(date[i]))
			return false;
	}
	return date_to_seconds(date) != WRONG_DATE;
}
void* safe_malloc(size_t size) {
	void* result=malloc(size);
	if(!result)
		exit(1);
	return result;
}
void prompt_str(char *str, char* buffer) {
	system("/bin/stty cooked");

	// Clear Terminal
	printf("\e[1;1H\e[2J");
	printf("%s", str);
	free(str);
	int i;
	for(i = 0; i < MAX_NAME; i++) {
		int c = getchar();
		if(c == '\n' || c == EOF)
			break;

		buffer[i] = (char)c;
	}
	buffer[i] = '\0';
}
int prompt(char *str) {
	system("/bin/stty raw");
	// Clear Terminal
	printf("\e[1;1H\e[2J");
	printf("%s\r", str);
	free(str);

	int input = getchar();
	if (input == EOT || input == ETX || input == ESC || input == EOF)
		input = EXIT;

	system("/bin/stty cooked");
	return input;
}
void append(char **a, char *b, int options) {
	char *tmp = *a;
	if (*a) {
		switch (options) {
			case NO_OPTIONS: asprintf(a, "%s%s", *a, b);
				break;
			case ENDL: asprintf(a, "%s%s\n", *a, b);
				break;
			case START: asprintf(a, "%s\r%s", *a, b);
				break;
			case START_ENDL: asprintf(a, "%s\r%s\n", *a, b);
				break;
			default: exit(1);
		}
	} else {
		if (options == ENDL || options == START_ENDL)
			asprintf(a, "%s\n", b);
		else
			asprintf(a, "%s", b);
	}

	if(tmp)
		free(tmp);
	if(!*a)
		exit(1);
}
uint64_t get_num(int type) {
	FILE *f;
	char* filename = (type == CREDIT ? CREDITS_COUNTER : DEPOSITS_COUNTER);
	uint64_t num = 0;
	f = fopen(filename, "r");
	if(f) {
		fscanf(f, "%ld", (long*)&num);
		fclose(f);
	}
	f = fopen(filename, "w");
	fprintf(f, "%lu", ++num);

	fclose(f);
	return num;
}
bool get_user_fullname(char* username, char* fullname) {
	struct passwd pwd;
	struct passwd *result;
	char* buffer = safe_malloc(MAX_GETPW);

	int s = getpwnam_r(username, &pwd, buffer, MAX_GETPW, &result);
	if (result == NULL) {
		if (s == 0)
			return false;

		errno = s;
		perror("getpwnam_r");
		exit(EXIT_FAILURE);
	}
	if(fullname)
		strcpy(fullname, pwd.pw_gecos);
	free(buffer);
	return true;
}
bool get_uid(char* user, uid_t* uid) {
	struct passwd *pwd = getpwnam(user);
	if (pwd == NULL)
		return false;

	*uid = pwd->pw_uid;
	return true;
}
bool chown_wrapper(char* user, char* filename) {
	uid_t uid;
	if(!get_uid(user, &uid)) return false;
	if (chown(filename, uid, -1) == -1)
		return false;

	return true;
}
char* make_filename(uint64_t num, int type) {
	char num_str[MAX_NAME];
	sprintf(num_str, "%lu", num);

	char *filename = NULL;
	append(&filename, type == CREDIT ? CREDITS_LOC : DEPOSITS_LOC, NO_OPTIONS);
	append(&filename, num_str, NO_OPTIONS);
	append(&filename, ".txt", NO_OPTIONS);
	return filename;
}
int add_deposit_credit(char* name, char* sum, char* date, int percent, int type) {
	uint64_t num = get_num(type);

	char fullname[MAX_FULLNAME + 1];
	if(!get_user_fullname(name, fullname))
		return ADD_FAILED_NO_USER;


	char* filename = make_filename(num, type);
	FILE* f = fopen(filename, "w");
	if(!f)
		return FOPEN_FAILED;


	fprintf(f, "Name: %s\n", fullname);
	fprintf(f, "Number: %lu\n", num);
	fprintf(f, "Sum: %s\n", sum);
	fprintf(f, "Date: %s\n", date);
	fprintf(f, "Procent: %d\n", percent);

	if(!chown_wrapper(name, filename))
		return CHOWN_FAILED;

	free(filename);
	fclose(f);
	return ADDED;
}
regex_t regex_pattern(char* pattern) {
	regex_t re;
	if (regcomp(&re, pattern, REG_EXTENDED) != 0) {
		fprintf(stderr, "Failed to compile regex \n");
		exit(EXIT_FAILURE);
	}
	return re;
}
size_t read_file(FILE* f, char** buffer) {
	fseek(f, 0, SEEK_END);
	size_t length = ftell (f);
	fseek(f, 0, SEEK_SET);
	char* buff = malloc(sizeof(char)*length + 1);

	size_t end = fread(buff, sizeof(char), length, f);
	buff[end] = '\0';
	*buffer = buff;
	return length;
}
time_t get_last_date(uint64_t num,int type) {
	char date[DATE_LEN + 1];
	const int GROUPS = 1;
	regmatch_t rm[GROUPS+1];
	char* filename = make_filename(num,type);
	char* buffer;

	FILE* f = fopen(filename, "r");
	if(!f)
		return FOPEN_FAILED;

	size_t length = read_file(f,&buffer);

	char* end = buffer + length;
	regex_t re = regex_pattern("Date: (.{10})");
	char* start = buffer;
	while(start < end) {
		if (regexec(&re, start, GROUPS+1, rm, 0) != 0)
			break;
		sprintf(date, "%.*s", (int)(rm[1].rm_eo - rm[1].rm_so), start + rm[1].rm_so);

		start += rm[0].rm_eo;
	}


	free(buffer);
	regfree(&re);
	free(filename);
	return date_to_seconds(date);
}
bool late_enough_date(char*buffer, uint64_t num, int type) {
	time_t entered = date_to_seconds(buffer);
	time_t last = get_last_date(num,type);
	return last < entered;

}
bool check_pattern(char* pattern, char* buffer, char* start,  int groups, regmatch_t* rm, size_t* offset) {
	regex_t re = regex_pattern(pattern);
	start +=*offset;
	if (regexec(&re, start, groups+1, rm, 0) != REG_NOERROR) {
		regfree(&re);
		return false;
	}

	sprintf(buffer, "%.*s", (int)(rm[1].rm_eo - rm[1].rm_so), start + rm[1].rm_so);
	*offset += rm[0].rm_eo;
	regfree(&re);
	return true;
}
size_t assert_d(char* first_line, list_t** l) {
	char date[DATE_LEN + 1];
	if(first_line[0] != 'D') return false;

	strcpy(date, first_line + strlen("Date: "));
	assert_insert(list_insert(l, new_period(TYPE_D,date,"","","")));
	return true;
}
size_t assert_dp(char* first_line, char* start, int groups, regmatch_t* rm, list_t** l) {
	char date[DATE_LEN + 1];
	char procent[3];
	size_t offset = 0;

	if(first_line[0] != 'D') return false;
	assert_pattern(check_pattern("^\n*Procent: ([0-9]{1,2})",	procent,	start, groups, rm, &offset));
	strcpy(date, first_line + strlen("Date: "));


	assert_insert(list_insert(l, new_period(TYPE_DP,date,"","",procent)));
	return offset;
}
size_t assert_dsdp(char* first_line, char* start, int groups, regmatch_t* rm, list_t** l) {
	char date1[DATE_LEN + 1];
	char date2[DATE_LEN + 1];
	char sum[MAX_NAME + 1];
	char procent[3];
	size_t offset = 0;

	if(first_line[0] != 'D') return false;
	assert_pattern(check_pattern("^\n*Sum: ([0-9]*,?[0-9]+)",	sum,			start, groups, rm, &offset));
	assert_pattern(check_pattern("^\n*Date: (.{10})", 					date2,		start, groups, rm, &offset));
	assert_pattern(check_pattern("^\n*Procent: ([0-9]{1,2})",	procent,	start, groups, rm, &offset));

	strcpy(date1, first_line + strlen("Date: "));
	assert_insert(list_insert(l, new_period(TYPE_DSDP,date1,sum,date2,procent)));
	return offset;
}
bool print_info(uint64_t num, int type) {
	char line[MAX_FULLNAME + 8];
	char header[MAX_FULLNAME + 128];
	const int GROUPS = 1;
	regmatch_t rm[GROUPS + 1];
	char* filename = make_filename(num,type);
	char* buffer;

	FILE* f = fopen(filename, "r");
	if(!f)
		return false;

	size_t length = read_file(f,&buffer);

	char* end = buffer + length;
	regex_t re = regex_pattern("(Date: .{10}|Sum: [0-9]*,?[0-9]+|Procent: [0-9]{1,2})");

	regex_t re_header = regex_pattern("(Name: [^\n]+\nNumber: [0-9]+\nSum: [0-9]*,?[0-9]+\nDate: .{10}\nProcent: [0-9]{1,2})");
	if (regexec(&re_header, buffer, GROUPS+1, rm, 0) != 0)
		return false;
	sprintf(header, "%.*s", (int)(rm[1].rm_eo - rm[1].rm_so), buffer + rm[1].rm_so);


	bool corrupt = false;
	char* start = buffer + rm[0].rm_eo;
	list_t* l = NULL;
	while(start < end) {
		if (regexec(&re, start, GROUPS+1, rm, 0) != 0)
			break;

		sprintf(line, "%.*s", (int)(rm[1].rm_eo - rm[1].rm_so), start + rm[1].rm_so);
		start+= rm[0].rm_eo;

		size_t ret = assert_dsdp(line, start, GROUPS, rm, &l);
		start += ret;
		if(ret != 0) continue;

		ret = assert_dp(line, start, GROUPS, rm, &l);
		start += ret;
		if(ret != 0) continue;

		ret = assert_d(line, &l);
		if(!ret) {
			corrupt = true;
			break;
		}
	}

	printf("%s\n\n", header);
	print_list(l);

	free(buffer);
	regfree(&re);
	regfree(&re_header);
	free(filename);
	list_free(&l);
	return !corrupt;
}
int show_info(uint64_t num, int type) {
	// Clear Terminal
	printf("\e[1;1H\e[2J");

	if(!print_info(num,type))
		printf("Deposit/Credit File is corrupt\n");



	printf("\n--\nPress Enter to go back to main menu");
	int input = getchar();
	if (input == EOT || input == ETX || input == ESC || input == EOF)
		return EXIT;
	return NO_ERR;
}