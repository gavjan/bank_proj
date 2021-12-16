#ifndef BANK_PROJ__LIST_H_
#define BANK_PROJ__LIST_H_
#define assert_insert(p) if(p == false) return false
#include <stdbool.h>
enum max{
	SUM_LEN = 20,
	PROCENT_LEN = 2,
	DATE_LEN = 10,
};
enum type {
	TYPE_DSDP,
	TYPE_DP,
	TYPE_D
};
typedef struct period {
	char date[DATE_LEN + 1];
	char sum[SUM_LEN + 1];
	char date2[DATE_LEN + 1];
	char procent[PROCENT_LEN + 1];
	char type;
} period_t;

typedef struct list {
	period_t data;
	struct list* next;
} list_t;
period_t new_period(char type, char* date, char* sum, char* date2, char* procent);
void list_free(list_t** head);
bool list_insert(list_t** l, period_t data);
void print_list(list_t* l);
#endif //BANK_PROJ__LIST_H_
