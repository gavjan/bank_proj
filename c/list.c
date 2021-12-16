#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "list.h"
static inline list_t* new_node() {
	list_t* l = malloc(sizeof(list_t));
	if(l == NULL) return NULL;
	l->next = NULL;
	return l;
}
period_t new_period(char type, char* date, char* sum, char* date2, char* procent) {
	period_t p = {0};
	p.type = type;
	switch(type) {
		case TYPE_DSDP:
			strcpy(p.date, date);
			strcpy(p.sum, sum);
			strcpy(p.date2, date2);
			strcpy(p.procent, procent);
			break;
		case TYPE_DP:
			strcpy(p.date, date);
			strcpy(p.procent, procent);
			break;
		case TYPE_D:
			strcpy(p.date, date);
			break;
	}
	return p;
}
void list_free(list_t** head) {
	if(head == NULL) return;
	list_t* l = *head;
	*head = NULL;
	while(l != NULL) {
		list_t* del = l;
		l = l->next;
		free(del);
	}
}
bool list_insert(list_t** l, period_t data) {
	list_t* new = new_node();
	if(new == NULL) return false;
	new->data = data;
	new->next = *l;
	*l = new;
	return true;
}
void print_period(period_t p) {
	switch(p.type) {
		case TYPE_DSDP:
			printf("Date: %s\n", p.date);
			printf("Sum: %s\n", p.sum);
			printf("Date: %s\n", p.date2);
			printf("Procent: %s\n\n", p.procent);
			break;
		case TYPE_DP:
			printf("Date: %s\n", p.date);
			printf("Procent: %s\n\n", p.procent);
			break;
		case TYPE_D:
			printf("Date: %s\n\n", p.date);
			break;
	}
}
void print_list(list_t* l) {
	while(l) {
		print_period(l->data);
		l=l->next;
	}
}