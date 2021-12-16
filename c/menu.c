#define _GNU_SOURCE
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <pwd.h>
#include <sys/stat.h>
#include "menu.h"
#include "list.h"
#include "safe.h"

int main_menu(int err_code, char* user) {
	char *str = NULL;
	append(&str, "--------Main Menu--------", START_ENDL);
	if(user[0]) {
		append(&str, "+ Selected Client: ", START);
		append(&str, user, ENDL);
	}
	switch(err_code) {
		case MODIFIED:
			append(&str, "- Successfully Modified", START_ENDL);
			break;
		case ADDED:
			append(&str, "- Successfully added", START_ENDL);
			break;
		case FOPEN_FAILED:
			append(&str, "- Add failed: problems with opening files", START_ENDL);
			break;
		case CHOWN_FAILED:
			append(&str, "- Add failed: problems with chown", START_ENDL);
			break;
		case ADD_FAILED_NO_USER:
			append(&str, "- Add failed: selected user no longer exists", START_ENDL);
			break;
		case EXIT_HELP:
			append(&str, "- To exit press ESC", START_ENDL);
			break;
	}
	append(&str, "1. Select Client", START_ENDL);
	append(&str, "2. Check Deposits/Credits", START_ENDL);
	append(&str, "3. Add Deposit/Credit", START_ENDL);
	append(&str, "4. Modify Deposit/Credit:", START_ENDL);

	int pick = prompt(str);

	assert_exit(pick);
	if(pick < '1' || '4' < pick) {
		return main_menu(EXIT_HELP, user);
	}

	return pick;
}
int enter_deposit_or_credit() {
	char *str = NULL;
	append(&str, "-------Deposit/Credit-------", START_ENDL);
	append(&str, "1. Deposit", START_ENDL);
	append(&str, "2. Credit", START_ENDL);
	int pick = prompt(str);

	assert_exit(pick);
	if(pick != DEPOSIT && pick != CREDIT)
		return enter_deposit_or_credit();

	return pick;
}
void enter_sum(char* buffer, bool err) {
	char *str = NULL;
	append(&str, "--------Enter Sum--------", START_ENDL);
	if(err)
		append(&str, "- Wrong sum entered", START_ENDL);
	append(&str, "Enter a sum: ", START);
	prompt_str(str, buffer);

	bool saw_comma = false;
	int i;
	for(i = 0; i < MAX_NAME; i++) {
		if(buffer[i] == '\0')
			break;
		if(!isdigit(buffer[i]) && (buffer[i] != ',' || saw_comma))
			return enter_sum(buffer, true);
		if(buffer[i] == ',')
			saw_comma = true;
	}
	if(i < 1 || buffer[0] == ',' || buffer[i-1] == ',')
		enter_sum(buffer, true);

}
void enter_date(char*buffer, int err, uint64_t num, int type) {
	char *str = NULL;
	append(&str, "-------Start Date--------", START_ENDL);
	if(err == WRONG_DATE)
		append(&str, "- Wrong date entered", START_ENDL);
	if(err == SOONER_DATE)
		append(&str, "- Enter date is too early", START_ENDL);
	append(&str, "Enter a date (dd.mm.yyyy): ", START);
	prompt_str(str, buffer);
	if(!isdate(buffer))
		enter_date(buffer, WRONG_DATE, num, type);

	if(num != NO_DATE_CHECK && !late_enough_date(buffer, num, type)) {
		enter_date(buffer, SOONER_DATE, num, type);
	}
}
int enter_percent(bool err) {
	char buffer[MAX_NAME + 1];
	char *str = NULL;
	append(&str, "--------Percent----------", START_ENDL);
	if(err)
		append(&str, "- Wrong percent entered", START_ENDL);
	append(&str, "Enter a percent (without the % sign): ", START);
	prompt_str(str, buffer);
	unsigned len = strlen(buffer);
	if(len != 1 && len != 2) return enter_percent(true);
	if(len == 1 && !isdigit(buffer[0])) enter_percent(true);
	if(len == 2 && (!isdigit(buffer[0]) || !isdigit(buffer[1]))) enter_percent(true);
	return atoi(buffer);
}
uint64_t select_deposit_credit(int type, bool err, bool read_only) {
	char *str = NULL;
	append(&str, "--------Enter Number--------", START_ENDL);
	if(err)
		append(&str, "- Wrong number entered (Possibly closed Deposit/Credit)", START_ENDL);
	if(type == DEPOSIT)
		append(&str, "Enter Deposit number: ", START);
	else
		append(&str, "Enter Credit number: ", START);

	char buffer[MAX_NAME];
	prompt_str(str, buffer);
	uint64_t num = strtoull(buffer, (char **)NULL, 10);
	char* filename = make_filename(num, type);

	bool exists = access(filename, F_OK) == 0;
	bool can_write = fopen(filename, "a");
	free(filename);

	return exists && (can_write || read_only) ? num : select_deposit_credit(type, true, read_only);
}
bool chmod_wrapper(char* filename) {
	int perm = (S_IRUSR|S_IRGRP); // r--r-----
	if (chmod(filename,perm) < 0) {
		return false;
	}
	return true;
}
bool get_owner(char* filename, char* buffer) {
	struct stat info = {0};
	if(stat(filename, &info)==-1)
		return false;
	struct passwd *pw = getpwuid(info.st_uid);
	if(!pw)
		return false;
	strcpy(buffer, pw->pw_name);
	return true;
}
int make_read_only(uint64_t num, int type) {
	char* filename = make_filename(num, type);
	char user[MAX_NAME + 1];
	char officer[MAX_NAME + 1];

	assert_failed(get_owner(filename, user));

	cuserid(officer);
	assert_failed(chown_wrapper(officer, filename));

	assert_failed(chmod_wrapper(filename));
	assert_failed(chown_wrapper(user, filename));
	free(filename);
	return MODIFIED;
}
int exec_end_date(uint64_t num, int type) {
	char date[DATE_LEN + 1];
	enter_date(date, NO_ERR, num, type);


	char* filename = make_filename(num, type);
	FILE* f = fopen(filename, "a");
	free(filename);
	if(!f)
		return FOPEN_FAILED;


	fprintf(f, "Date: %s\n", date);
	fclose(f);
	make_read_only(num, type);
	return make_read_only(num, type);
}
int exec_new_percent(uint64_t num, int type) {
	char date[DATE_LEN + 1];
	enter_date(date, NO_ERR, num, type);

	int percent = enter_percent(false);

	char* filename = make_filename(num, type);
	FILE* f = fopen(filename, "a");
	free(filename);
	if(!f)
		return FOPEN_FAILED;


	fprintf(f, "Date: %s\n", date);
	fprintf(f, "Procent: %d\n", percent);
	fclose(f);
	return MODIFIED;
}
int exec_new_sum(uint64_t num, int type) {
	char date1[DATE_LEN + 1];
	char date2[DATE_LEN + 1];
	char sum[MAX_NAME + 1];

	enter_date(date1, NO_ERR, num, type);
	enter_sum(sum, false);
	enter_date(date2, NO_ERR, num, type);
	int percent = enter_percent(false);

	char* filename = make_filename(num, type);
	FILE* f = fopen(filename, "a");
	free(filename);
	if(!f)
		return FOPEN_FAILED;


	fprintf(f, "Date: %s\n", date1);
	fprintf(f, "Sum: %s\n", sum);
	fprintf(f, "Date: %s\n", date2);
	fprintf(f, "Procent: %d\n", percent);
	fclose(f);
	return MODIFIED;
}
int pick_modify_type() {
	char *str = NULL;
	append(&str, "--------Modify Type---------", START_ENDL);
	append(&str, "1. New Sum", START_ENDL);
	append(&str, "2. New Percent", START_ENDL);
	append(&str, "3. End Date", START_ENDL);
	int pick = prompt(str);
	assert_exit(pick);

	if(pick < '1' || '3' < pick)
		return pick_modify_type();

	return pick;
}
