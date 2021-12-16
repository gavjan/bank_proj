#include <stdbool.h>
#include <stdlib.h>
#include <pwd.h>
#include <string.h>
#include "safe.h"
#include "menu.h"
#include "list.h"
int exec_select(char *buffer, bool err) {
	char *str = NULL;
	append(&str, "-------Select User-------", START_ENDL);
	if (err)
		append(&str, "- User not found", START_ENDL);
	append(&str, "Enter a username: ", START);
	prompt_str(str, buffer);

	bool found = get_user_fullname(buffer, NULL);
	if (!found)
		exec_select(buffer, true);
	return EXIT_SUCCESS;
}
int exec_add(char* selected_user) {
	char name[MAX_NAME + 1];
	char sum[MAX_NAME + 1];
	char date[DATE_LEN + 1];

	int type = enter_deposit_or_credit();
	assert_exit(type);
	if(selected_user[0])
		strcpy(name, selected_user);
	else
		exec_select(name, false);
	enter_sum(sum, false);
	enter_date(date, NO_ERR, NO_DATE_CHECK, NO_TYPE);
	int percent = enter_percent(false);

	return add_deposit_credit(name, sum, date, percent, type);
}
int exec_modify() {
	int type = enter_deposit_or_credit();
	assert_exit(type);
	uint64_t num = select_deposit_credit(type, false, false);
	int operation_type = pick_modify_type();
	assert_exit(operation_type);
	int ret_code;
	switch (operation_type) {
		case NEW_SUM:
			ret_code = exec_new_sum(num, type);
			break;
		case NEW_PERCENT:
			ret_code = exec_new_percent(num, type);
			break;
		case END_DATE:
			ret_code = exec_end_date(num, type);
			break;
	}

	return ret_code;
}
int exec_check() {
	int type = enter_deposit_or_credit();
	assert_exit(type);
	uint64_t num = select_deposit_credit(type, false, true);
	return show_info(num,type);
}
int main() {
	char user[MAX_NAME + 1] = {0};
	int ret_code = EXIT_SUCCESS;

	while (ret_code != EXIT) {
		switch (main_menu(ret_code, user)) {
			case SELECT: ret_code = exec_select(user, false);
				break;
			case CHECK:
				ret_code = exec_check();
				break;
			case ADD:
				ret_code = exec_add(user);
				break;
			case MODIFY:
				ret_code = exec_modify();
				break;
			case EXIT: ret_code = EXIT;
				break;
		}
	}
	return 0;
}
