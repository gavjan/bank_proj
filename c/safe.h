#ifndef BANK_PROJ__SAFE_H_
#define BANK_PROJ__SAFE_H_

#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#define CREDITS_LOC "/credits/"
#define DEPOSITS_LOC "/deposits/"
#define CREDITS_COUNTER "/credits/.counter"
#define DEPOSITS_COUNTER "/deposits/.counter"
#define MAX_GETPW (int)sysconf(_SC_GETPW_R_SIZE_MAX)
#define assert_pattern(p) if(p == false) return false

void* safe_malloc(size_t size);

enum messages {
	NO_ERR = 0,

	ADD_FAILED_NO_USER = -2,
	EXIT_HELP = -3,
	ADDED = -4,
	CHOWN_FAILED = -5,
	FOPEN_FAILED = -6,
	MODIFIED = -7,

	WRONG_DATE = -8,
	SOONER_DATE = -9
};
enum ASCII {
	EXIT = -1,
	EOT = 3,
	ETX = 4,
	ESC = 27
};
enum menu {
	SELECT = '1',
	CHECK = '2',
	ADD= '3',
	MODIFY = '4',

	DEPOSIT = '1',
	CREDIT = '2',

	NEW_SUM = '1',
	NEW_PERCENT = '2',
	END_DATE = '3',
};
enum limits {
	MAX_NAME = 32, // According to (man useradd)
	MAX_FULLNAME = 256,
};
enum printing {
	NO_OPTIONS,
	ENDL,
	START,
	START_ENDL
};
enum misc {
	NO_DATE_CHECK = 0,
	NO_TYPE = 0,
	DATE_DELIMITER_1 = 2, // dd*mm/yyyy
	DATE_DELIMITER_2 = 5, // dd/mm*yyyy
};
bool chown_wrapper(char* user, char* filename);
char* make_filename(uint64_t num, int type);
bool isdate(char* date);
bool get_user_fullname(char* username, char* fullname);
int prompt(char *str);
void prompt_str(char *str, char* buffer);
void append(char **a, char *b, int options);
uint64_t get_num(int type);
int add_deposit_credit(char* name, char* sum, char* date, int percent, int type);
time_t get_last_date(uint64_t num, int type);
bool late_enough_date(char*buffer, uint64_t num, int type);
int show_info(uint64_t num, int type);
#endif //BANK_PROJ__SAFE_H_
