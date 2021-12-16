#ifndef BANK_PROJ__MENU_H_
#define BANK_PROJ__MENU_H_

#define assert_exit(p) if(p == EXIT) return EXIT
#define assert_failed(p) if(p == false) return CHOWN_FAILED

#include <stdint.h>
int exec_end_date(uint64_t num, int type);
int exec_new_sum(uint64_t num, int type);
int exec_new_percent(uint64_t num, int type);
int main_menu(int err_code, char* user);
int enter_deposit_or_credit();
void enter_sum(char* buffer, bool err);
void enter_date(char*buffer, int err, uint64_t num, int type);
int enter_percent(bool err);
int pick_modify_type();
uint64_t select_deposit_credit(int type, bool err, bool read_only);

#endif //BANK_PROJ__MENU_H_
